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
 * @file        power_manager_adapter.cpp
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/

#include <services/PowerManagerService/IPowerManagerService.h>
#include <binder/IServiceManager.h>
#include <services/PowerManagerService/PowerManager.h>
#include <services/PowerManagerService/PowerIndexEnum.h>
#include <services/PowerManagerService/PowerIndexEnum.h>
#include <services/PowerManagerService/PowerLock.h>
#include "../logger.h"
#include "power_manager_adapter.h"
#include "../oem_call_handler.h"
#include "../oem_call_app_constants.h"
#include "../tspecall/core/services/PowerManagerAdapter.h"
namespace {
    /**
     * @brief This is the Power Index of ACN, check Power index file of Power Manager
     *      for more information.
     *
     */
    struct PowerIndex {
        static constexpr uint32_t ACN = 1101;
        static constexpr uint32_t ECALL = 1104;
        static constexpr uint32_t ACN_PHONE = 1106;
    };

}

namespace oc {

    template<class Target>
    class PowerLockListener: public PowerLockCallback
    {
        public:
            // TODO: remove memory leak
            PowerLockListener(Target &powerAdapter) : powerAdapter_(powerAdapter) {}
            virtual ~PowerLockListener() {}
            virtual void expiredtimeout() override {
                powerAdapter_.onPowerLockRelease();
            }
        private:
            Target& powerAdapter_;
    };

    class PowerStateReceiver: public BnPowerStateReceiver{
        public:
            PowerStateReceiver(const android::sp<AdapterHandler> handler) : handler_(handler) {}
            virtual ~PowerStateReceiver() {}
            virtual void onPowerStateChanged(const int32_t newState, const int32_t reason) {
            }
            virtual void onPowerModeChanged(const int32_t newMode){
                 handler_->obtainMessage( msg::MSG_POWER_MODE_CHANGED, newMode)->sendToTarget();
            }
            virtual void onExtValueChanged(const int32_t listenIndex, const int32_t value) {
                 handler_->obtainMessage( msg::MSG_POWER_STATE_CHANGED, listenIndex, value)->sendToTarget();
            }
        private:
            android::sp<AdapterHandler> handler_;
    };


/**
 * @brief This contains the private properties of HmiManager class instance
 *
 */
struct PowerManagerAdapter::Impl {
    int32_t igState = -1;
    int32_t accState= -1;
    bool isLocked = false;
    android::sp<OemCallHandler>                spOemCallHandler = nullptr; // handler for sending message to main thread

    /**
     * @brief This is the pointer to Power manager service
     *
     */
    android::sp<IPowerManagerService> spPowerMgrService;

    /**
     * @brief spPowerLock This is the PowerLock instance which used to preventing power go to
     *        sleep mode.
     */
    android::sp<PowerLock> spPowerLock;

    PowerLockListener<PowerManagerAdapter>* powerLockCallBack = nullptr;

    /**
     * @brief This is the method to get the configuration service pointer from
     *        service layer.
     */
    android::sp<IPowerManagerService> getPowerMgrService() {
//        if (spPowerMgrService == nullptr) {
            spPowerMgrService = android::interface_cast<IPowerManagerService>(
                                    android::defaultServiceManager()->getService(
                                        android::String16(POWER_SRV_NAME)
                                )
            );
//        }
        return spPowerMgrService;
    }

};


PowerManagerAdapter::PowerManagerAdapter(android::sp<OemCallHandler> spHandler)
    :impl_(new Impl()){
    LOG_I(DLT_STRING("Creating PowerAdapter ..."));
    impl_->spOemCallHandler = spHandler;

    impl_->powerLockCallBack = new PowerLockListener<PowerManagerAdapter>(*this);
    impl_->spPowerLock = new PowerLock(getpid(), impl_->powerLockCallBack);

    if (impl_->getPowerMgrService()->requestToGet(MCU_STATUS_IG1, impl_->igState) != TIGER_ERR::E_OK) {
        LOG_E(DLT_STRING("Cannot get ig"));
    }
    if (impl_->getPowerMgrService()->requestToGet(MCU_STATUS_ACC, impl_->accState) != TIGER_ERR::E_OK){
        LOG_E(DLT_STRING("Cannot get acc"));
    };

}

PowerManagerAdapter::~PowerManagerAdapter() {
    if (impl_->powerLockCallBack!= nullptr) {
        delete impl_->powerLockCallBack;
    }

}

bool PowerManagerAdapter::requestBUBModeACNPhone(bool isActive) {
    LOG_I(DLT_STRING("Requesting BUB mode, isActive = "), DLT_BOOL(isActive));
#ifdef __POWER_TEST__
    return true;
#endif // __POWER_TEST__


    if (
        PowerManager::instance()->requestToSet(PowerIndex::ACN_PHONE, isActive) != TIGER_ERR::E_OK
    ) {
        LOG_E(DLT_STRING("Cannot request BUB mode"));
        return false;
    }
    return true;
}

bool PowerManagerAdapter::requestBUBModeACN(bool isActive) {
    LOG_I(DLT_STRING("Requesting BUB mode, isActive = "), DLT_BOOL(isActive));
#ifdef __POWER_TEST__
    return true;
#endif // __POWER_TEST__


    if (
        PowerManager::instance()->requestToSet(PowerIndex::ACN, isActive) != TIGER_ERR::E_OK
    ) {
        LOG_E(DLT_STRING("Cannot request BUB mode"));
        return false;
    }
    return true;
}

bool PowerManagerAdapter::requestBUBModeECall(bool isActive) {
    LOG_I(DLT_STRING("Requesting BUB mode, isActive = "), DLT_BOOL(isActive));
#ifdef __POWER_TEST__
    return true;
#endif // __POWER_TEST__


    if (
        PowerManager::instance()->requestToSet(PowerIndex::ECALL, isActive) != TIGER_ERR::E_OK
    ) {
        LOG_E(DLT_STRING("Cannot request BUB mode"));
        return false;
    }
    return true;
}

int32_t PowerManagerAdapter::getACCStatus()
{
    int32_t nstate = MCU_VALUE_STATUS::MCU_STATUS_UNKNOWN;
    PowerManager::instance()->requestToGet(MCU_STATUS_ACC, nstate);
    LOG_I(DLT_STRING("ACC value = "), DLT_INT32(nstate));
    return nstate;
}
int32_t PowerManagerAdapter::getIGStatus()
{
    int32_t nstate = MCU_VALUE_STATUS::MCU_STATUS_UNKNOWN;
    PowerManager::instance()->requestToGet(MCU_STATUS_IG1, nstate);
    LOG_I(DLT_STRING("IG value = "), DLT_INT32(nstate));
    return nstate;
}

void PowerManagerAdapter::acquirePowerLock() {
    uint32_t result = -1;
    ecallapp::PowerManagerAdapter::getInstance()->setAcnWithPhoneRunning(true);
    if (!impl_->isLocked) {
        result= impl_->spPowerLock->acquire();
        impl_->isLocked = true;
    }
    LOG_I(DLT_STRING("Result = "),DLT_INT32(result), DLT_STRING(", (-2)API not ready, -1 (Related PowerLock Not Existed), (>=1) (Ref count)"));
}

void PowerManagerAdapter::resetIsLocked () {
    impl_->isLocked = false;
}

void PowerManagerAdapter::releasePowerLock() {
    uint32_t result = -1;
    // Because power lock counter is counted by pid, 
    // Need the condition: both legacy features (ACN with phone) and new feature terminiated, release power lock is call
    // If new feature or legacy feature released power lock before another one, power is able to transact to LPM
    ecallapp::PowerManagerAdapter::getInstance()->setAcnWithPhoneRunning(false);
    int32_t tLockCount = ecallapp::PowerManagerAdapter::getInstance()->getPowerLockCount();
    if (impl_->isLocked && tLockCount == 0) {
        result= impl_->spPowerLock->release();
        impl_->isLocked = false;
    }
    LOG_I(DLT_INT32(result), DLT_STRING("(-2)API not ready, -1 (Related PowerLock Not Existed), (>1) (Ref count)"));
}

std::string PowerManagerAdapter::getIgnition() {
    std::string ignition;
    LOG_I(
        DLT_STRING("IGState:" ),DLT_UINT32(impl_->igState),
        DLT_STRING("ACCState:" ),DLT_UINT32(impl_->accState)
    );

    if (impl_->igState == MCU_VALUE_STATUS::MCU_STATUS_OFF && impl_->accState == MCU_VALUE_STATUS::MCU_STATUS_OFF) {
        ignition = "ignitionOff";
    }
    else if (
         (impl_->igState == MCU_VALUE_STATUS::MCU_STATUS_ON && impl_->accState == MCU_VALUE_STATUS::MCU_STATUS_ON) ||
         (impl_->igState == MCU_VALUE_STATUS::MCU_STATUS_OFF && impl_->accState == MCU_VALUE_STATUS::MCU_STATUS_ON)
     ){
        ignition = "ignitionOn";
    }
    else if (
         (impl_->igState == MCU_VALUE_STATUS::MCU_STATUS_ON && impl_->accState == MCU_VALUE_STATUS::MCU_STATUS_OFF)
    ) {
        ignition = "engineOn";
    }
    else {
        ignition = "unknown";
    }

    return ignition;
}

bool PowerManagerAdapter::isAccOn() {
    return (impl_->accState == MCU_VALUE_STATUS::MCU_STATUS_ON);
}

bool PowerManagerAdapter::isIGON() {
    return (impl_->igState == MCU_VALUE_STATUS::MCU_STATUS_ON);
}

void PowerManagerAdapter::handlePowerStateChanged(const uint32_t index, const uint32_t value) {
    LOG_I(
        DLT_STRING("index:" ),DLT_UINT32(index),
        DLT_STRING("value:" ),DLT_UINT32(value)
    );
    if (index == MCU_STATUS_ACC) {
        impl_->accState = value;
    }
    else if (index == MCU_STATUS_IG1) {
        impl_->igState= value;
    }
}

void PowerManagerAdapter::onPowerLockRelease() {
     impl_->spOemCallHandler->obtainMessage( msg::MSG_POWER_LOCK_CALLBACK)->sendToTarget();
}

} // namespace oc
