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
 * @file        TimeManagerAdapter.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "TimeManagerAdapter.h"
#include <services/TimeManagerService/TimeManager.h>
#include "tspecall/core/utils/ECallAppHandler.h"
#include "logger.h"
#include "tspecall/core/utils/ParamsDef.h"
namespace ecallapp {

TimeManagerAdapter::TimeManagerAdapter() {
}

std::shared_ptr<TimeManagerAdapter>& TimeManagerAdapter::getInstance()
{
    static std::shared_ptr<TimeManagerAdapter> sTimeManagerAdapter(new TimeManagerAdapter());
    return sTimeManagerAdapter;
}

void TimeManagerAdapter::registerService() {
    mTimeReceiver = android::sp<TimeManagerAdapter::TimeReceiver>(new TimeManagerAdapter::TimeReceiver());
    TimeManager& tm = TimeManager::getInstance();
    error_t result = tm.registerTimeChangedReceiver(mTimeReceiver);
    if (result != static_cast<error_t>(E_OK)) {
        LOG_E(DLT_STRING("Register fail"), DLT_INT(static_cast<int32_t>(result)));
        ECallAppHandler* tHandler = ECallAppHandler::getInstance();
        if (tHandler != nullptr) {
            (void)(tHandler->sendMessageDelayed(tHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_TIMEMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS));
        }
    } else {
        LOG_D(DLT_STRING("register successfully"));
    }
}

void TimeManagerAdapter::TimeReceiver::onTimeReceived(const int32_t time_type, const int64_t GMT_msec, const int64_t diff_time_msec) {
    LOG_I(DLT_STRING("Time type:"), DLT_INT(time_type));
    if (static_cast<int32_t>(timeservice::NETWORK_TIME) == time_type
    || static_cast<int32_t>(timeservice::GPS_TIME) == time_type
    || static_cast<int32_t>(timeservice::RTC_TIME) == time_type) {
        ECallAppHandler* tHandler = ECallAppHandler::getInstance();
        if (tHandler != nullptr) {
            (void)(tHandler->sendMessage(tHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_ON_TIME_RECEIVED))));
        }
    }
}

void TimeManagerAdapter::TimeReceiver::onTimeOffsetReceived(const int32_t time_type, const int32_t offset_min, const int32_t dst) {}

int64_t TimeManagerAdapter::getCurrentTimestamp() {
    int64_t timeStamp = 0;
    TimeManager& tm = TimeManager::getInstance();
    timeservice::timedata_t timeData = {};
    int32_t timeType = tm.getCurrentTime(&timeData);
    if (static_cast<int32_t>(timeservice::NETWORK_TIME) == timeType
        || static_cast<int32_t>(timeservice::GPS_TIME) == timeType
        || static_cast<int32_t>(timeservice::RTC_TIME) == timeType) {
        timeStamp = tm.getCurrentMilliSec();
        LOG_I(DLT_STRING("Time = (ms)"), DLT_INT64(timeStamp));
    }
    else {
        LOG_I(DLT_STRING("Can not get a Time"));
    }
    LOG_I(DLT_STRING("Time type ="), DLT_INT32(timeType));
    return timeStamp;
}
} //namespace ecallapp