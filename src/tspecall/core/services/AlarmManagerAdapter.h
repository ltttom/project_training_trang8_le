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
 * @file        AlarmManagerAdapter.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef ADAPTER_ALARMMANAGERADAPTER_H
#define ADAPTER_ALARMMANAGERADAPTER_H
#include <binder/IServiceManager.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include "services/AlarmManagerService/AlarmManager.h"
#include <services/AlarmManagerService/IAlarmManagerService.h>
#include <services/AlarmManagerService/IAlarmManagerServiceType.h>
#include <services/AlarmManagerService/IAlarmExpireListener.h>

#include "../utils/ECallAppHandler.h"
#include "../utils/ServiceDeathRecipient.h"

namespace ecallapp {

static const int32_t ECALL_CALLBACK_WAITING_ALARM_ID = 1234; // define unique id internally.
class AlarmManagerAdapter{
private:
    static AlarmManagerAdapter *instance;
    android::sp<ECallAppHandler> mHandler = nullptr;
    android::sp<ServiceDeathRecipient> mServiceDeathRecipient = nullptr;
    android::sp<AlarmManager> mAlarmMgr = nullptr;
    android::sp<AlarmListener> mAlarmListen = nullptr;
    android::sp<IAlarmManagerService> mAlarmservice = nullptr;

public:
    AlarmManagerAdapter();
    virtual ~AlarmManagerAdapter();
    static AlarmManagerAdapter* getInstance();
    /**
     * @brief inject the callback object to service.
     */
    void registerService();
    /**
     * @brief start the timer with id = alarmaId, duration = delayMs.
     */
    error_t setAlarmFromNow(const int64_t& delayMs, const int32_t& alarmID);
    error_t cancel(const int32_t& alarmID);

public:
    /**
    * @brief handle the event service died (do the reconnect action)
    */
    void onBinderDied(const android::wp<android::IBinder>& who);
    android::sp<ECallAppHandler> getHandler() const {return mHandler;}
};

class AlarmAdapterListener: public AlarmListener {
public:
    AlarmAdapterListener(AlarmManagerAdapter& alarmAdapter)
        : AlarmListener(),
        mParent(alarmAdapter) {
    }
    virtual ~AlarmAdapterListener() {
    }
    /**
    * @brief handle the timer expired event
    */
    virtual void onAlarmExpired(const int32_t alarmID);
private:
    AlarmManagerAdapter& mParent;
};
} /* namespace Adapter */
#endif /* ADAPTER_ALARMMANAGERADAPTER_H */
