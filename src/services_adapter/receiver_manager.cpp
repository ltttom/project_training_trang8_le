/**
 * @copyright
 * Copyright (c) 2020 by LG Electronics Inc.
 * This program or software including the accompanying associated documentation
 * ("Software") is the proprietary software of LG Electronics Inc. and or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate written license agreement between you
 * and LG Electronics Inc. ("Authorized License"). Except as set forth in an
 * Authorized License, LG Electronics Inc. grants no license (express or implied),
 * rights to use, or waiver of any kind with respect to the Software, and LG
 * Electronics Inc. expressly reserves all rights in and to the Software and all
 * intellectual property therein. If you have no Authorized License, then you have
 * no rights to use the Software in any ways, and should immediately notify LG
 * Electronics Inc. and discontinue all use of the Software.
 * @file        receiver_manager.cpp
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#include "receiver_manager.h"
#include <binder/BinderService.h>
#include <binder/Parcel.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <utils/RefBase.h>
#include <utils/Errors.h>

/* Telephony Manager Service */
//#include <services/TelephonyManagerService/base/PhoneStateListener.h>
//#include <services/TelephonyManagerService/base/PhoneManager.h>
//#include <services/TelephonyManagerService/base/PhoneManagerBase.h>
//#include <services/TelephonyManagerService/base/ITelephonyService.h>
#include <TelephonyManager.hpp>
#include <ITelephonyRegistry.hpp>

/* HMI Manager Sevice */
#include <services/HMIManagerService/IHMIManagerService.h>
#include <services/HMIManagerService/HMIManager.h>

/* Power Mode Handler */
#include <services/PowerManagerService/IPowerManagerService.h>
#include <services/PowerManagerService/IPowerStateReceiver.h>
#include <services/PowerManagerService/PowerManager.h>
#include <services/PowerManagerService/PowerIndexEnum.h>

/* Audio handler*/
#include <services/AudioManagerService/IAudioManagerReceiver.h>
#include <services/AudioManagerService/IAudioManagerService.h>

/*VCM*/
#include <services/VehicleManagerService/IVehicleManagerService.h>
#include <services/VehicleManagerService/IVehicleManagerServiceType.h>
#include <services/VehicleManagerService/IVehicleReceiver.h>
#include <services/VehicleManagerService/TSUDataIndex.h>

/*RIM*/
#include <services/RemoteIfManagerService/IRemoteIfManagerService.h>
#include <services/RemoteIfManagerService/IRemoteIfMgrReceiver.h>
#include <services/RemoteIfManagerService/IRemoteIfManagerServiceType.h>
#include <services/RemoteIfManagerService/ESendType.h>
#include <services/RemoteIfManagerService/EOrderType.h>
#include <services/RemoteIfManagerService/EServerRespType.h>

/* Configuration Manager Service */
#include <services/ConfigurationManagerService/IConfigurationManagerReceiver.h>
#include <services/ConfigurationManagerService/IConfigurationManagerService.h>

#include <memory>
#include <sstream>
#include <iomanip>


#include <Error.h>

#include "../oem_call_app_constants.h"
#include "../logger.h"
#include "../message_defs.h"

namespace {
    using namespace oc;


     /**
      * This is a class which implemented the receiver of changed state from
      * HMI
      */
     class HMIReiceiver : public BnHMIEventReceiver{
         public:
             HMIReiceiver(ReceiverManager& parent) : parent_(parent) {}
             virtual ~HMIReiceiver() {}
             virtual void onHMIReceived(uint32_t type, uint32_t index, uint32_t action) {
                 if (type == HMI_TYPE_BUTTON){
                     parent_.getHandler()->obtainMessage(msg::MSG_RECV_HMI, index, action)->sendToTarget();
                 }
             }
         private:
             ReceiverManager& parent_;
     };

    /**
     * This is a class which implemented the receiver of changed state from
     * Power Manager
     */
    class PowerStateReceiver: public BnPowerStateReceiver{
        public:
             PowerStateReceiver(ReceiverManager& parent) : parent_(parent) {}
            virtual ~PowerStateReceiver() {}
            virtual void onPowerStateChanged(const int32_t newState, const int32_t reason) {
            }
            virtual void onPowerModeChanged(const int32_t newMode){
                 parent_.getHandler()->obtainMessage( msg::MSG_POWER_MODE_CHANGED, newMode)->sendToTarget();
            }
            virtual void onExtValueChanged(const int32_t listenIndex, const int32_t value) {
                 parent_.getHandler()->obtainMessage( msg::MSG_POWER_STATE_CHANGED, listenIndex, value)->sendToTarget();
            }
        private:
            ReceiverManager& parent_;
    };


    class ConfigReceiverImpl : public BnConfigurationManagerReceiver {
        public:
            ConfigReceiverImpl(ReceiverManager& parent) : parent_(parent) {}
            virtual void onConfigDataChanged(std::string& name, std::string& value) {
                sp<ConfigNotiData> data = new ConfigNotiData();
                data->name =  name;
                data->value=  value;
                parent_.getHandler()->obtainMessage(msg::MSG_CONFIG_CHANGED, data)->sendToTarget();
            }
        private:
            ReceiverManager& parent_;
    };

}

namespace oc {
    /**
     * @brief This contains the private properties of receiver class instance
     *
     */
    struct ReceiverManager::Impl {
        android::sp<OemCallHandler>                spHandler = nullptr;
        android::sp<ServiceDeathRecipient>         spServiceDeathRecipient = nullptr;
        android::sp<IPowerStateReceiver>           spPowerReceiver = nullptr;
        android::sp<HMIReiceiver> spHmiReceiver;
        android::sp<IConfigurationManagerReceiver> spConfigReceiver   = nullptr;

        android::sp<IPowerManagerService>          spPowerManagerService = nullptr;
        android::sp<IConfigurationManagerService>  spConfig = nullptr;

        void registerPowerService() {
            LOG_I(
                DLT_STRING(std::string(__func__).c_str())
            );
            spPowerManagerService= android::interface_cast<IPowerManagerService>(
                android::defaultServiceManager()->getService(
                    android::String16("service_layer.PowerManagerService")
                )
            );
            bool error = true;

            if(spPowerManagerService != nullptr) {
                auto result = android::IInterface::asBinder(spPowerManagerService)->linkToDeath(spServiceDeathRecipient);
                if (
                    result == android::OK &&
                    spPowerManagerService->registerPowerStateReceiver(spPowerReceiver, MASK_FOR_ALL_NOTI) == TIGER_ERR::E_OK
                ) {
                    error = false;
                    LOG_I(
                        DLT_STRING(std::string(__func__).c_str()),
                        DLT_STRING(", "),
                        DLT_STRING("Registed Power manager Service")
                    );
                }
            }

            if (error) {
                LOG_E(
                    DLT_STRING(std::string(__func__).c_str()),
                    DLT_STRING(", "),
                    DLT_STRING("Cannot register Power manager Service, try again after"),
                    DLT_UINT32(self::SEND_RETRY_DELAY)
                );
                spHandler->sendMessageDelayed(
                    spHandler->obtainMessage(
                        msg::MSG_CM_REQUEST_RETRY_REGISTER_RECEIVER,
                        self::ServiceId::POWER
                    ),
                    self::SEND_RETRY_DELAY
                );
            }
        }

         void registerHMIReceiver() {
             bool error = true;
             auto spHmi = HMIManager::instance();

            if(spHmi != nullptr) {
                auto result = spHmi->registerReceiver(0, spHmiReceiver);
                if (result == TIGER_ERR::E_OK) {
                    LOG_I(DLT_STRING("Registed HMI manager Service"));
                    error = false;
                }
            }
            if (error) {
                LOG_E(
                    DLT_STRING("Cannot register HMI Service, try again after"),
                    DLT_UINT32(self::SEND_RETRY_DELAY)
                );
                spHandler->sendMessageDelayed(
                    spHandler->obtainMessage(
                        msg::MSG_CM_REQUEST_RETRY_REGISTER_RECEIVER,
                        self::ServiceId::HMI
                    ),
                    self::SEND_RETRY_DELAY
                );
            }
         }

        void registerConfigService() {
           LOG_I(DLT_STRING(std::string(__func__).c_str())); //Print caller function
                // Register receiver for notification from RIM
            spConfig = android::interface_cast<IConfigurationManagerService>(
                 android::defaultServiceManager()->getService(
                     android::String16(CONFIGURATION_SRV_NAME)
                 )
             );
            bool error = true;

            if( spConfig != nullptr) {
                auto result = android::IInterface::asBinder(spConfig)->linkToDeath(spServiceDeathRecipient);
                if (result == android::OK )
                    if (
                            spConfig->registerReceiver(DataFrom::General, "", spConfigReceiver) == android::OK
                    ) {
                        LOG_I(
                            DLT_STRING(std::string(__func__).c_str()),
                            DLT_STRING(", "),
                            DLT_STRING("Registed Config Service")
                        );
                        error = false;
                    }

            }

            if (error) {
                LOG_E(
                    DLT_STRING(std::string(__func__).c_str()),
                    DLT_STRING(", "),
                    DLT_STRING("Cannot register Config Service, try again after"),
                    DLT_UINT32(self::SEND_RETRY_DELAY)
                );
                spHandler->sendMessageDelayed(
                    spHandler->obtainMessage(
                        msg::MSG_CM_REQUEST_RETRY_REGISTER_RECEIVER,
                        self::ServiceId::CONFIG
                    ),
                    self::SEND_RETRY_DELAY
                );
            }

        }

        void onBinderDied(const android::wp<android::IBinder>& who)
        {
            if (who == android::IInterface::asBinder(spPowerManagerService)) {
                spHandler->sendMessageDelayed(
                    spHandler->obtainMessage(
                        msg::MSG_CM_REQUEST_RETRY_REGISTER_RECEIVER,
                        self::ServiceId::POWER
                    ),
                    self::SEND_RETRY_DELAY
                );
                spHandler->obtainMessage(msg::MSG_CM_BINDER_DIED, self::ServiceId::POWER)->sendToTarget();
            }
            else if (who == android::IInterface::asBinder(spConfig)) {
                spHandler->sendMessageDelayed(
                    spHandler->obtainMessage(
                        msg::MSG_CM_REQUEST_RETRY_REGISTER_RECEIVER,
                        self::ServiceId::CONFIG
                    ),
                    self::SEND_RETRY_DELAY
                );
                spHandler->obtainMessage(msg::MSG_CM_BINDER_DIED, self::ServiceId::CONFIG)->sendToTarget();
            }
        }
    };

    std::unique_ptr<ReceiverManager::Impl>  ReceiverManager::impl_ = std::make_unique<ReceiverManager::Impl>();
    ReceiverManager::ReceiverManager()
    {
    }

    ReceiverManager::~ReceiverManager() {

    }

    void ReceiverManager::init(
            android::sp<OemCallHandler> spHandler
        ) {
        impl_->spPowerManagerService = nullptr;
        // impl_->spHmiManagerService = nullptr;

        if (spHandler != nullptr) {
            impl_->spHandler = spHandler;
        }

        if(impl_->spHandler != nullptr) {
            impl_->spServiceDeathRecipient = new ServiceDeathRecipient();
        //LCOV_EXCL_START    
            impl_->spServiceDeathRecipient->registerHandler(
                [&] (const android::wp<android::IBinder>& who) -> void
                {
                    impl_->onBinderDied(who);
                });
        //LCOV_EXCL_STOP 
            impl_->spPowerReceiver         = new PowerStateReceiver(*this);
            impl_->spConfigReceiver        = new ConfigReceiverImpl(*this);
            impl_->spHmiReceiver           = new HMIReiceiver(*this);
        }
        
        impl_->registerPowerService();
        impl_->registerConfigService();
        impl_->registerHMIReceiver();
    }

    android::sp<OemCallHandler> ReceiverManager::getHandler() {
        return impl_->spHandler;
    }

    void ReceiverManager::requestRetryRegisterReceiver(uint32_t who) {
        LOG_I(DLT_STRING("ServiceId: "), DLT_UINT(who));
        switch (who) {
            case self::ServiceId::POWER: {
                impl_->registerPowerService();
                break;
            }
            case self::ServiceId::HMI: {
                 impl_->registerHMIReceiver();
                break;
            }
            case self::ServiceId::CONFIG: {
                impl_->registerConfigService();
                break;
            }
            default:
                break;
        }
    }

}
