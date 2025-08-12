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
 * @file        PowerManagerAdapter.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef POWERMANAGERADAPTER_H
#define POWERMANAGERADAPTER_H

#include <services/PowerManagerService/PowerIndexEnum.h>
#include <services/PowerManagerService/IPowerManagerService.h>
#include <services/PowerManagerService/PowerManager.h>
#include <services/PowerManagerService/PowerLock.h>
#include <binder/IServiceManager.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <utils/RefBase.h>
#include "../utils/ECallAppHandler.h"
#include "logger.h"
#include "../utils/ServiceDeathRecipient.h"
#include "../utils/ParamsDef.h"
#include <memory>

namespace ecallapp {

class PowerManagerAdapter {

    class PowerLockListener: public PowerLockCallback, public android::RefBase {
        public:
            PowerLockListener(PowerManagerAdapter& powerAdapter) noexcept : mParent(powerAdapter) {}

            virtual void expiredtimeout() noexcept override {
                LOG_I(DLT_STRING("PowerLock acquired period expired."));
                mParent.onPowerLockAcquisitionExpired();
            }
        private:
            PowerManagerAdapter& mParent;
    };

    class PowerAdapterListener : public BnPowerStateReceiver {
    public:
        PowerAdapterListener(PowerManagerAdapter& powerAdapter) noexcept : mParent(powerAdapter){}

        virtual void onErrControlPower(const int32_t err_reason, const int32_t errPowerID, const int32_t currPowerID) override {
            mParent.onErrControlPower(err_reason, errPowerID, currPowerID);
        }
        virtual void onPowerModeChanged(const int32_t newMode) override {
            mParent.onPowerModeChanged(newMode);
        }
        virtual void onExtValueChanged(const int32_t listenIndex, const int32_t value) override {
            mParent.onExtValueChanged(listenIndex, value);
        }

    private:
        PowerManagerAdapter& mParent;
    };

public:
    PowerManagerAdapter();
    ~PowerManagerAdapter() noexcept;

    PowerManagerAdapter(const PowerManagerAdapter& ) = default;
    PowerManagerAdapter& operator=(const PowerManagerAdapter& ) = default;
    PowerManagerAdapter(PowerManagerAdapter&& ) = default;
    PowerManagerAdapter& operator=(PowerManagerAdapter&& ) = default;

    static PowerManagerAdapter* getInstance();
    void registerService();
    android::sp<IPowerManagerService> getService() const noexcept;
    void acquirePowerLock();
    void releasePowerLock();
    void onPowerLockAcquisitionExpired();
    bool isIgnitionOn();
    void onPowerStateChanged(const int32_t newState, const int32_t reason) const;
    void onErrControlPower(const int32_t err_reason, const int32_t errPowerID, const int32_t currPowerID) const;
    void onPowerModeChanged(const int32_t newMode);
    void onExtValueChanged(const int32_t listenIndex, const int32_t value);
    void onBinderDied(const android::wp<android::IBinder>& who);

    int32_t getACCStatus();
    int32_t getIGStatus();
    bool requestBUBModeACNWithPhone(bool isOn);
    bool requestBUBModeACNRunning(bool isOn);
    bool requestBUBModeECallRunning(bool isOn);
    void resetPowerLockCounter();
    int32_t getPowerLockCount();
    void setAcnWithPhoneRunning(bool isAcnWithPhoneRunning);


private:
    static PowerManagerAdapter *instance;
    android::sp<ECallAppHandler>    mHandler;
    android::sp<PowerAdapterListener>  mPowerRcv;
    android::sp<ServiceDeathRecipient> mServiceDeathRecipient;
    android::sp<IPowerManagerService>  mPowerMgrService;
    android::sp<PowerLock> mPowerLock;
    std::unique_ptr<PowerLockCallback> mPowerLockCallback;
    bool mIsLocked;
    int32_t mPowerLockCount;
    bool mIsAcnWithPhoneRunning;
};

} /* End: namespace ECALLDCMNONREG */
#endif // POWERMANAGERADAPTER_H
