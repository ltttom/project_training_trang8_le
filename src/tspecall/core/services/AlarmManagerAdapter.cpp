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
 * @file        AlarmManagerAdapter.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "AlarmManagerAdapter.h"
#include "logger.h"
#include "../utils/ParamsDef.h"

namespace ecallapp {
AlarmManagerAdapter::AlarmManagerAdapter() {
}

AlarmManagerAdapter::~AlarmManagerAdapter() {
}

AlarmManagerAdapter* AlarmManagerAdapter::instance = nullptr;
AlarmManagerAdapter* AlarmManagerAdapter::getInstance() {
    if (instance == nullptr) {
        instance = new AlarmManagerAdapter();
    } else {
    }
    return instance;
}

void AlarmManagerAdapter::registerService() {
    LOG_I(DLT_STRING("registerService"));
    mHandler = ECallAppHandler::getInstance();
    if (mAlarmservice != nullptr) {
        mAlarmservice = nullptr;
        mAlarmMgr = nullptr;
        mAlarmListen = nullptr;
    } else {
    }

    mAlarmservice = android::interface_cast<IAlarmManagerService>(android::defaultServiceManager()->getService(android::String16(ALARM_SRV_NAME)));
    mServiceDeathRecipient = new ServiceDeathRecipient( [&] ( const android::wp<android::IBinder>& who ) {AlarmManagerAdapter::getInstance()->onBinderDied(who);} );

    if (mAlarmservice != nullptr) {
        (void)android::IInterface::asBinder(mAlarmservice)->linkToDeath(mServiceDeathRecipient);
        mAlarmListen = android::sp<AlarmAdapterListener>{new AlarmAdapterListener(*this)}; // using andoird sp instead of raw pointer
        mAlarmMgr = android::sp<AlarmManager>{new AlarmManager(mAlarmListen)}; // using andoird sp instead of raw pointer
    } else {
        LOG_I(DLT_STRING("mAlarmservice null"));
        mAlarmservice = nullptr;
        mAlarmMgr = nullptr;
        mAlarmListen = nullptr;
        if (mHandler != nullptr) { // null checking before using
            (void)mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_ALARMMGR)),timeMsParams::TIME_SEND_RETRY_DELAY_MS);
        }
        else {
            LOG_E(DLT_STRING("handler pointer is null"));
        }
    }
}

error_t AlarmManagerAdapter::setAlarmFromNow(const int64_t& delayMs, const int32_t& alarmID) {
    LOG_I(DLT_STRING("AlarmManagerAdapter::setAlarmFromNow, alarmID: "), DLT_INT32(alarmID));
    error_t ret = E_ERROR;
    if (mAlarmMgr != nullptr && delayMs > 0) { // Need to ensure that the duration of timer has to bigger than 0, if not send the timer expired immediately 
        ret = mAlarmMgr->setAlarmFromNow(delayMs, alarmID);
    }
    if (ret != E_OK) {
        LOG_E(DLT_STRING("setAlarmFromNow fail: expried now"));
        ECallAppHandler* tHandler = ECallAppHandler::getInstance();
        if (tHandler != nullptr) {
            tHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CBWT_TIMER_EXPIRED))->sendToTarget();
        }
    }
    return ret;
}

error_t AlarmManagerAdapter::cancel(const int32_t& alarmID) {
    LOG_I(DLT_STRING("AlarmManagerAdapter::cancel, alarmID: "), DLT_INT32(alarmID));
    error_t ret = E_ERROR;
    if (mAlarmMgr != nullptr) {
        ret = mAlarmMgr->cancel(alarmID);
    }
    if (ret != E_OK) {
        LOG_E(DLT_STRING("cancel fail"));
    }
    return ret;
}

void AlarmAdapterListener::onAlarmExpired(const int32_t alarmID) {
	LOG_I(DLT_STRING("AlarmManagerAdapter::onAlarmExpired: "), DLT_INT32(alarmID));
    if (alarmID == ECALL_CALLBACK_WAITING_ALARM_ID) {
        (void)mParent.getHandler()->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CBWT_TIMER_EXPIRED))->sendToTarget();
    }
}

void AlarmManagerAdapter::onBinderDied(const android::wp<android::IBinder> &who) {
    LOG_I(DLT_STRING("AlarmManagerAdapter::onBinderDied"));
    mAlarmservice = nullptr;
    mAlarmMgr = nullptr;
    mAlarmListen = nullptr;
    if (mHandler != nullptr) { // null checking
        (void)mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_ALARMMGR)),timeMsParams::TIME_SEND_RETRY_DELAY_MS);
    }
    else {
        LOG_E(DLT_STRING("handler pointer is null"));
    }
}
} /* namespace eCall */
