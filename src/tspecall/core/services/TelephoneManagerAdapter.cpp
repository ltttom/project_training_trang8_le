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
 * @file        TelephoneManagerAdapter.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include <unistd.h>
#include "TelephoneManagerAdapter.h"
#include "DisconnectCause.h"
#include <TelephonyManagerConstants.hpp>

namespace ecallapp {

TelephoneManagerAdapter::TelephoneManagerAdapter()
{
    // mSMSMgrService         = SMSManager::instance();
    mPhoneStateListener    = nullptr;

    mIncomingNumber  = "";
}

TelephoneManagerAdapter* TelephoneManagerAdapter::instance {nullptr};
TelephoneManagerAdapter* TelephoneManagerAdapter::getInstance()
{
    if (instance == nullptr)
    {
        instance = new TelephoneManagerAdapter();
    }
    return instance;
}

android::sp<telephony::ITelephonyRegistry> TelephoneManagerAdapter::getService()
{
    /* Initialize data */
    mDisconnectCause = DisconnectCause::NOT_VALID;

    if (mTelephonyRegistry == nullptr)
    {
        mTelephonyRegistry = android::interface_cast<telephony::ITelephonyRegistry>(android::defaultServiceManager()->getService(android::String16("service_layer.TelephonyRegistry")));
    }
    return mTelephonyRegistry;
}

void TelephoneManagerAdapter::registerService()
{
    LOG_I(DLT_STRING("registerService"));
    mHandler = ECallAppHandler::getInstance();
    auto func = [&](const android::wp<android::IBinder>& who){
        TelephoneManagerAdapter::getInstance()->onBinderDied(who);
    };
    mServiceDeathRecipient = android::sp<ServiceDeathRecipient>(new ServiceDeathRecipient(func));
    if (mTelephonyRegistry != nullptr) {
        mTelephonyRegistry = nullptr;
    }
    (void)getService();

    if (mPhoneStateListener != nullptr) {
        mPhoneStateListener = nullptr;
    }
    mPhoneStateListener = std::make_shared<ECallPhoneStateListener>(*this);
    bool error {true};

    if(mTelephonyRegistry != nullptr && mServiceDeathRecipient != nullptr)
    {
        if (android::OK == android::IInterface::asBinder(mTelephonyRegistry)->linkToDeath(mServiceDeathRecipient)) {
            uint64_t events { static_cast<uint64_t>(telephony::PhoneStateListener::LISTEN_PRECISE_CALL_STATE)};
            events |= static_cast<uint64_t>(telephony::PhoneStateListener::LISTEN_DISCONNECT_CAUSE);
            events |= static_cast<uint64_t>(telephony::PhoneStateListener::LISTEN_ECALL_STATUS_CHANGED);
            events |= static_cast<uint64_t>(telephony::PhoneStateListener::LISTEN_CALL_STATE);
            telephony::TelephonyManager::listen(mPhoneStateListener, events);
            error = false;
        }
    }

    if (error) {
        LOG_E(DLT_STRING("[TP] Cannot register Telephony manager Service, try again after 500 ms"));
        (void)(mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_TELEPHONEMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS));
    }
}

void TelephoneManagerAdapter::onPreciseCallStateChanged(const int32_t state)
{
    /* Notify to handler */
    if(mHandler != nullptr)
    {
        LOG_I(DLT_STRING("call state = "), DLT_INT(state));

        // The RECV_MSG_TELE_CALL_STATE_CHANGED message is processed as a separate event based on the telephony state.
        if (state == telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE) {
            LOG_I(DLT_STRING("telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE"));
            (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CALL_DISCONNECTED), state)->sendToTarget());
        }
        else if (state == telephony::TelephonyManager::PRECISE_CALL_STATE_ACTIVE) {
            LOG_I(DLT_STRING("telephony::TelephonyManager::PRECISE_CALL_STATE_ACTIVE"));
            (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CALL_CONNECTED), state)->sendToTarget());
        }
        else if (state == telephony::TelephonyManager::PRECISE_CALL_STATE_INCOMING) {
            LOG_I(DLT_STRING("telephony::TelephonyManager::PRECISE_CALL_STATE_INCOMING"));
            (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_INCOMING_CALL_TRIGGER), state)->sendToTarget());
        }
        else {
            LOG_I(DLT_STRING("don't handle this state: "), DLT_INT(state));
        }
    }
}

void TelephoneManagerAdapter::onCallStateChanged(const int32_t state, const std::string &incomingNumber)
{
    LOG_I(DLT_STRING("call state = "), DLT_INT(state));
    mIncomingNumber = "";
    if((state == telephony::TelephonyManager::CALL_STATE_RINGING) && (incomingNumber.length() > 0U))
    {
        LOG_I(DLT_STRING("incomingNumber = "), DLT_STRING(incomingNumber.c_str()));
        mIncomingNumber = incomingNumber;
    }
}

std::string TelephoneManagerAdapter::getIncomingNumber() noexcept
{
    return mIncomingNumber;
}

void TelephoneManagerAdapter::onDisconnectCause(const int32_t cause)
{
    LOG_I(DLT_STRING("disconnectCause = "), DLT_INT(cause));
    mDisconnectCause = cause;
}

int32_t TelephoneManagerAdapter::getDisconnectCause()
{
    LOG_I(DLT_STRING("mDisconnectCause = "), DLT_INT(mDisconnectCause));
    return mDisconnectCause;
}

bool TelephoneManagerAdapter::isCallDisconnectedNormal() {
    bool ret{false};
    ret = (mDisconnectCause == DisconnectCause::NORMAL) || (mDisconnectCause == DisconnectCause::LOCAL);
    return ret;
}

void TelephoneManagerAdapter::dial(const std::string number, const bool isECall) const
{
    LOG_I(DLT_STRING("phone number = "), DLT_STRING(number.c_str()), DLT_STRING(", is ECall = "),  DLT_STRING(isECall ? "TRUE" : "FALSE"));
    if (isECall)
    {
        telephony::TelephonyManager::dial(number, telephony::TelephonyManager::CALL_TYPE_EMERGENCY_CALL);
    }
    else
    { // rcall
        telephony::TelephonyManager::dial(number, telephony::TelephonyManager::CALL_TYPE_VOICE_CALL);
    }
}

void TelephoneManagerAdapter::endCall() const
{
    LOG_I(DLT_STRING("endCall"));
    telephony::TelephonyManager::endCall();
}

void TelephoneManagerAdapter::rejectCall() const
{
    LOG_I(DLT_STRING("rejectCall"));
    telephony::TelephonyManager::rejectCall();
}

void TelephoneManagerAdapter::answerRingingCall() const
{
    LOG_I(DLT_STRING("answerRingingCall"));
    telephony::TelephonyManager::answerRingingCall();
}

std::string TelephoneManagerAdapter::getMsisdn() const
{
    std::string temp {telephony::TelephonyManager::getMsisdn()};
    LOG_I(DLT_STRING("getMsisdn: "), DLT_STRING(temp.c_str()));
    return temp;
}

int32_t TelephoneManagerAdapter::getCallState() const
{
    int32_t callState { telephony::TelephonyManager::getCallState() };
    LOG_I(DLT_STRING("callState = "), DLT_INT(callState));
    return callState;
}

bool TelephoneManagerAdapter::clearingCurrentCall() {
    LOG_I(DLT_STRING("clearingCurrentCall"));
    if (!mIsClearingCurrentCall) {
        auto callState = getCallState();
        if(callState != telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE) {
            if (callState == telephony::TelephonyManager::PRECISE_CALL_STATE_INCOMING) {
                LOG_I(DLT_STRING("rejectCall"));
                rejectCall();
            }
            else {
                LOG_I(DLT_STRING("endCall"));
                endCall();
            }
            mIsClearingCurrentCall = true;
        }
        else {
            mIsClearingCurrentCall = false;
        }
    }
    return mIsClearingCurrentCall;
}

void TelephoneManagerAdapter::stopClearingCurrentCall(){
    mIsClearingCurrentCall = false;
}

bool TelephoneManagerAdapter::isClearingCurrentCall() {
    return mIsClearingCurrentCall;
}

std::string TelephoneManagerAdapter::getNetworkOperator() const
{
    return telephony::TelephonyManager::getNetworkOperator();
}

std::string TelephoneManagerAdapter::getSimOperator() const
{
    return telephony::TelephonyManager::getSimOperator();
}

uint32_t TelephoneManagerAdapter::getVoiceServiceState() noexcept
{
    uint32_t ret {0U};
    ret = static_cast<uint32_t>(telephony::TelephonyManager::getServiceState(telephony::TelephonyManager::VOICE_NETWORK));
    return ret;
}

int32_t TelephoneManagerAdapter::getDataServiceState() noexcept
{
    int32_t ret {0};
    ret = telephony::TelephonyManager::getServiceState(telephony::TelephonyManager::DATA_NETWORK);
    return ret;
}

bool TelephoneManagerAdapter::isDataOutOfService() {
    return getDataServiceState() == telephony::TelephonyManager::STATE_OUT_OF_SERVICE;
}

void TelephoneManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who)
{
    NOTUSED(who);
    (void)(ECallAppHandler::getInstance()->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_TELEPHONEMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS));
}

std::string TelephoneManagerAdapter::getImei() const
{
    return telephony::TelephonyManager::getImei();
}

void ECallPhoneStateListener::onPreciseCallStateChanged(const int32_t state)
{
    parent.onPreciseCallStateChanged(state);
}

void ECallPhoneStateListener::onCallStateChanged(const int32_t state, const std::string &incomingNumber)
{
    parent.onCallStateChanged(state, incomingNumber);
}

void ECallPhoneStateListener::onDisconnectCause(const int32_t cause)
{
    parent.onDisconnectCause(cause);
}

} /* End: namespace ECALLDCMNONREG */
