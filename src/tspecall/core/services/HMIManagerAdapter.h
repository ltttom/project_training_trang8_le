/**
 * @copyright
 * Copyright (c) 2025 by LG Electronics Inc.
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
 * @file        HMIManagerAdapter.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef ECALLNONREG_HMIMANAGERADAPTER_H
#define ECALLNONREG_HMIMANAGERADAPTER_H
#include <binder/IServiceManager.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <memory>
#include <services/HMIManagerService/IHMIManagerService.h>
#include <services/HMIManagerService/HMIManager.h>

#include "../utils/ECallAppHandler.h"
#include "logger.h"
#include "../utils/ServiceDeathRecipient.h"
#include "../utils/ParamsDef.h"

namespace ecallapp {

    static const int32_t LED_ECALL_DIALING = 100;
    static const int32_t LED_ECALL_MSD_SENDING = 200;
    static const int32_t LED_ECALL_DATA_FAIL = 300;
    static const int32_t LED_ECALL_VOICE_CONNECTING = 400;
    static const int32_t LED_ECALL_CONN_FAIL = 500;
    static const int32_t LED_ECAL_UAE_FATAL_ERROR = 600;
    static const int32_t LED_ECAL_UAE_STANDBY_TIME_AFTER_AUTO = 700;
    static const int32_t LED_ECAL_UAE_DTC_PRESENT = 800;
    static const int32_t LED_ECAL_UAE_LPM_DTC_PRESENT = 900;
    static const int32_t LED_ECAL_UAE_BUB_LOW_VOLTAGE = 1000;
    static const int32_t LED_ECAL_IDLE = 1100;
    static const int32_t LED_ECAL_UAE_OTHER = 1200;
    static const int32_t LED_JAPAN_POWER_OFF = 1300;
    static const int32_t LED_JAPAN_INIT_CHECK = 1400;
    static const int32_t LED_JAPAN_ERROR_STATE = 1500;
    static const int32_t LED_JAPAN_CONFIRM_CONTRACT_STATUS = 1600;
    static const int32_t LED_JAPAN_STANDBY_MODE_NO_CONTRACT = 1700;
    static const int32_t LED_JAPAN_STANDBY_MODE_WITH_CONTRACT = 1800;
    static const int32_t LED_JAPAN_TEST_CALL = 1900;
    static const int32_t LED_JAPAN_LPM_TEST_CALL = 20000;
    static const int32_t LED_JAPAN_ECALL = 21000;
    static const int32_t LED_JAPAN_AUTO_INCOMING_STANDBY_MODE = 22000;
    static const int32_t LED_JAPAN_VOICE_COMM = 23000;
    static const int32_t LED_JAPAN_COMM_FAIL = 24000;
    static const int32_t LED_JAPAN_TEST_DATA_FAIL = 25000;
    static const int32_t LED_JAPAN_TEST_VOICE_FAIL = 26000;
    static const int32_t LED_JAPAN_TEST_INCOMPLETE_CONTRACT = 27000;
    static const int32_t LED_JAPAN_TEST_NO_DATA = 28000;
    static const int32_t LED_JAPAN_ECALL_NO_CONTRACT = 29000;
    static const int32_t LED_JAPAN_ECALL_NO_DATA = 30000;
    static const int32_t LED_JAPAN_MANUAL_DATA_FAIL = 31000;
    static const int32_t LED_JAPAN_AUTO_DATA_FAIL = 32000;
    static const int32_t LED_JAPAN_ECALL_VOICE_FAIL = 33000;
    static const int32_t LED_JP_RCALL_ERROR = 34000;
    static const int32_t LED_JP_RCALL_RUNNING = 35000;

class HMIManagerAdapter {
    class HMIReiceiver : public BnHMIEventReceiver{
    public:
        explicit HMIReiceiver(HMIManagerAdapter& hMIRcv) noexcept 
        : RefBase(), BnHMIEventReceiver(), mHMIRcv(hMIRcv) {}
        virtual ~HMIReiceiver() override = default;

        HMIReiceiver(const HMIReiceiver& ) = default;
        HMIReiceiver& operator=(const HMIReiceiver& ) = default;

        HMIReiceiver(HMIReiceiver&& ) = default;
        HMIReiceiver& operator=(HMIReiceiver&& ) = default;

        virtual void onHMIReceived(const uint32_t type, const uint32_t index, const uint32_t action) override {
            mHMIRcv.handleHMIReceived(type, index, action);
        }
    private:
        HMIManagerAdapter& mHMIRcv;
    };

public:
    HMIManagerAdapter();

    static HMIManagerAdapter* getInstance();
    
    android::sp<IHMIManagerService> getService() ;
    void registerService();
    void requestLedPattern(const int32_t& pattern, const bool& isOn);
    void sendButtonEventACKToMCU(const uint32_t& index, const uint8_t& action) const;
    void turnOffCurrentLed();
protected:
    void handleHMIReceived(const uint32_t& type, const uint32_t& index, const uint32_t& action);
    void onBinderDied(const android::wp<android::IBinder>& who);
    
private:
    static std::shared_ptr<HMIManagerAdapter> instance;
    android::sp<HMIReiceiver> mHmiReceiver = nullptr;
    android::sp<ECallAppHandler> mHandler = nullptr;
    android::sp<ServiceDeathRecipient> mServiceDeathRecipient = nullptr;
    android::sp<IHMIManagerService> mService;
    HMILedPatternInfo mCurrentLedPattern{};
};

} /* End: namespace ECALLDCMNONREG */
#endif // ECALLNONREG_HMIMANAGERADAPTER_H
