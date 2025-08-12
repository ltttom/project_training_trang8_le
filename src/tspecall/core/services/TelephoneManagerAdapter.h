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
 * @file        TelephoneManagerAdapter.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef TSP_TELEPHONEMANAGERSERVICE_H
#define TSP_TELEPHONEMANAGERSERVICE_H

#include <iostream>
#include <memory>
#include <string>
#include <TelephonyManager.hpp>
#include <ITelephonyRegistry.hpp>
#include <ServiceState.hpp>
#include <binder/IServiceManager.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>

#include "../utils/ECallAppHandler.h"
#include "logger.h"
#include "../utils/ServiceDeathRecipient.h"
#include "../utils/ParamsDef.h"
#include "../utils/ECallTimerHandler.h"

namespace ecallapp {

namespace phone_service_state
{
    constexpr int32_t STATE_IN_SERVICE { 0 };
    /** Device is not registered with any operator */
    constexpr int32_t STATE_OUT_OF_SERVICE { 1 };
    /** The device is registered and locked. Only emergency numbers are allowed */
    constexpr int32_t STATE_EMERGENCY_ONLY { 2 };
    /** Radio of telephony is explicitly powered off */
    constexpr int32_t STATE_POWER_OFF { 3 };
}

namespace debugLog
{
    static std::unordered_map<int32_t, std::string> callStateName {
        {telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE,          "PRECISE_CALL_STATE_IDLE"          },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_ACTIVE,        "PRECISE_CALL_STATE_ACTIVE"        },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_HOLDING,       "PRECISE_CALL_STATE_HOLDING"       },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_DIALING,       "PRECISE_CALL_STATE_DIALING"       },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_ALERTING,      "PRECISE_CALL_STATE_ALERTING"      },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_INCOMING,      "PRECISE_CALL_STATE_INCOMING"      },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_WAITING,       "PRECISE_CALL_STATE_WAITING"       },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_DISCONNECTED,  "PRECISE_CALL_STATE_DISCONNECTED"  },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_DISCONNECTING, "PRECISE_CALL_STATE_DISCONNECTING" }
    };
}

class TelephoneManagerAdapter {
public:
    TelephoneManagerAdapter();
    virtual ~TelephoneManagerAdapter() = default;

	TelephoneManagerAdapter(const TelephoneManagerAdapter& ) = default;
    TelephoneManagerAdapter& operator=(const TelephoneManagerAdapter& ) = default;
    TelephoneManagerAdapter(TelephoneManagerAdapter&& ) = default;
    TelephoneManagerAdapter& operator=(TelephoneManagerAdapter&& ) = default;

    static TelephoneManagerAdapter* getInstance();
    void registerService();
    void dial(const std::string number, const bool isECall) const;
    void endCall() const;
    void rejectCall() const;
    void answerRingingCall() const;
    bool clearingCurrentCall();
    void stopClearingCurrentCall();
    bool isClearingCurrentCall();

    std::string getIncomingNumber() noexcept;
    std::string getMsisdn() const;
    std::string getNetworkOperator() const;
    std::string getSimOperator() const;
    std::string getImei() const;
    
    int32_t getDisconnectCause();
    int32_t getCallState() const;
    uint32_t getVoiceServiceState() noexcept; // refer define in namespace phone_service_state
    int32_t getDataServiceState() noexcept;   // refer define in namespace phone_service_state
    bool isCallDisconnectedNormal();
    bool isDataOutOfService();

    void onPreciseCallStateChanged(const int32_t state);    // notify Precise Voice Call State
    void onCallStateChanged(const int32_t state, const std::string &incomingNumber);    // notify Incoming Voice Call
    void onDisconnectCause(const int32_t cause);

private:
    android::sp<telephony::ITelephonyRegistry> getService();

    // void writeSms(const sp<SMSFormatData> smsContent, const std::string timestamp, const uint8_t service);
    void generateMsgSeq(char_t* const) const noexcept;

    // Handle Retry MO SMS & Cancel retry
    void retryMOSMS();

    /* For SMS */
    void onBinderDied(const android::wp<android::IBinder>& who);
    void onTimerTimeoutEvent(const int32_t timerId);

private:
    static TelephoneManagerAdapter *instance;
    android::sp<ECallAppHandler>             mHandler;
    android::sp<ServiceDeathRecipient>          mServiceDeathRecipient { nullptr };
    android::sp<telephony::ITelephonyRegistry>  mTelephonyRegistry     { nullptr };
    std::shared_ptr<telephony::PhoneStateListener> mPhoneStateListener { nullptr };
    std::string                                 mPhoneNumber;
    std::string                                 mIncomingNumber;
    int32_t                                     mDisconnectCause{-1};
    std::string                                 mEventTimestamp;
    bool mIsClearingCurrentCall {false};
};

class ECallPhoneStateListener : public telephony::PhoneStateListener {
public:
    ECallPhoneStateListener(TelephoneManagerAdapter& pr) noexcept: parent(pr) {}

    void onPreciseCallStateChanged(const int32_t state) override;
    void onCallStateChanged(const int32_t state, const std::string &incomingNumber) override;
    void onDisconnectCause(const int32_t cause) override;

private:
    TelephoneManagerAdapter& parent;
};

} /* End: namespace ECALLDCMNONREG */
#endif // TSP_TELEPHONEMANAGERSERVICE_H