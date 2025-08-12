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
 * @file        PowerManagerAdapter.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "PowerManagerAdapter.h"
#include <cstdint>

namespace ecallapp {

PowerManagerAdapter::PowerManagerAdapter() {
    mIsLocked = false;
    mPowerLockCount = 0; // this member variable is used to count the number of features accquiring power lock, so it is initialize to 0
    mHandler = nullptr;
    mServiceDeathRecipient = nullptr;
    mPowerMgrService = nullptr;
    mPowerLock = nullptr;
    mPowerRcv = android::sp<PowerAdapterListener>{new PowerAdapterListener(*this)};
    mPowerLockCallback = std::make_unique<PowerLockListener>(*this);
    mIsAcnWithPhoneRunning = false;
}

PowerManagerAdapter::~PowerManagerAdapter() noexcept{
    mPowerLockCount = 0; // this member variable is used to count the number of features accquiring power lock, so it is reset to 0
}

PowerManagerAdapter* PowerManagerAdapter::instance {nullptr};
PowerManagerAdapter* PowerManagerAdapter::getInstance() {
    if (instance == nullptr) {
        instance = new PowerManagerAdapter();
    }
    return instance;
}

android::sp<IPowerManagerService> PowerManagerAdapter::getService() const noexcept {

    return mPowerMgrService;
}

void PowerManagerAdapter::registerService() {
    LOG_I(DLT_STRING("registerService"));
    mHandler = ECallAppHandler::getInstance();
    mServiceDeathRecipient = android::sp<ServiceDeathRecipient>{new ServiceDeathRecipient( [&] ( const android::wp<android::IBinder>& who ){
        PowerManagerAdapter::getInstance()->onBinderDied(who);
    })};
    if (mPowerMgrService != nullptr) {
        mPowerMgrService = nullptr;
    }

    mPowerMgrService = android::interface_cast<IPowerManagerService>(
            android::defaultServiceManager()->getService(
                android::String16(POWER_SRV_NAME)
                )
            );

    if (mPowerRcv == nullptr) {
        mPowerRcv = android::sp<PowerAdapterListener> {new PowerAdapterListener(*this)};
    }

    uint32_t mask { static_cast<uint32_t>(MASK_FOR_EXT_VALUE_CHANGED_NOTI) };
    mask |= static_cast<uint32_t>(MASK_FOR_PRE_HANDLER_POWER_STATE);
    mask |= static_cast<uint32_t>(MASK_FOR_CHANGED_POWER_MODE_NOTI);
    (void)PowerManager::instance()->registerPowerStateReceiver(mPowerRcv, static_cast<int32_t>(mask));

    if (mPowerLockCallback == nullptr) {
        mPowerLockCallback = std::make_unique<PowerLockListener>(*this);
    }
    if (mPowerLock == nullptr ){
        mPowerLock = android::sp<PowerLock> {new PowerLock(getpid(), mPowerLockCallback.get())};
    }
    bool error {true};
    if(mPowerMgrService != nullptr) {
        if (android::OK == android::IInterface::asBinder(mPowerMgrService)->linkToDeath(mServiceDeathRecipient)) {
            error = false;
        }
    }
    if (error) {
        LOG_E(DLT_STRING("Cannot register Power Service, try again after ms: "), DLT_UINT(timeMsParams::TIME_SEND_RETRY_DELAY_MS));
        if (mHandler != nullptr) {
            (void)(mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_POWERMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS));
        }
    }
}

void PowerManagerAdapter::onPowerStateChanged(const int32_t newState, const int32_t reason) const {
    LOG_I(DLT_STRING("newState = "), DLT_INT(newState), DLT_STRING(" | reason = "),  DLT_INT(reason));
}

void PowerManagerAdapter::onErrControlPower(const int32_t err_reason, const int32_t errPowerID, const int32_t currPowerID) const {
    LOG_I(DLT_STRING("onErrControlPower: "), DLT_INT(err_reason), DLT_INT(errPowerID), DLT_INT(currPowerID));
}

void PowerManagerAdapter::onPowerModeChanged(const int32_t newMode) {
    LOG_I(DLT_STRING("newmode = "), DLT_INT(newMode));
    if (mHandler != nullptr) {
        mHandler->obtainMessage( ENUM_TO_INT(msg::MSG::MSG_POWER_MODE_CHANGED), newMode)->sendToTarget();
    }
}

void PowerManagerAdapter::onExtValueChanged(const int32_t listenIndex, const int32_t value) {
    if (mHandler != nullptr) {
        mHandler->obtainMessage( ENUM_TO_INT(msg::MSG::MSG_POWER_STATE_CHANGED), listenIndex, value)->sendToTarget();
    }
}

void PowerManagerAdapter::acquirePowerLock() {
    LOG_I(DLT_STRING("acquirePowerLock, mPowerLockCount = "), DLT_INT(mPowerLockCount));
    // this member variable is used to count the number of features accquiring power lock, so it is initialize to 0
    // each time of acquiring, the counter incease 1.
    // do the acquire action only 1 time. (do not need to make more redudant acquire action)
    if (mPowerLock != nullptr) {
        mPowerLockCount++;
        if (mPowerLockCount == 1) {
            (void)mPowerLock->acquire();
           mIsLocked = true;
        }
    }

}

void PowerManagerAdapter::releasePowerLock() {
    LOG_I(DLT_STRING("releasePowerLock, mPowerLockCount = "), DLT_INT(mPowerLockCount));
    // this member variable is used to count the number of features accquiring power lock, so it is initialize to 0
    // each time of acquiring, the counter decease 1.
    // do the release action only 1 time. (do not need to make more redudant release action)
    if (mPowerLock != nullptr) {
        mPowerLockCount--;
        // Because power lock counter is counted by pid,
        // Need the condition: both legacy features (ACN with phone) and new feature terminiated, release power lock is call
        // If new feature or legacy feature released power lock before another one, power is able to transact to LPM
        if (mPowerLockCount <= 0) {
            mPowerLockCount = 0;
            (void)mPowerLock->release();
            mIsLocked = false;
        }
    }
}

void PowerManagerAdapter::resetPowerLockCounter() {
    mIsLocked = false;
    // this member variable is used to count the number of features accquiring power lock, so it is reset to 0
    // resetPowerLockCounter is called after acquire lock timer timeout, regardless of the number of current feature accquire lock, lock is reset
    mPowerLockCount = 0;
}

void PowerManagerAdapter::onPowerLockAcquisitionExpired() {
    LOG_I(DLT_STRING("onPowerLockAcquisitionExpired"));
    mIsLocked = false;
    if (mHandler != nullptr) {
        (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED))->sendToTarget());
    }
}

void PowerManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who) {
    LOG_I(DLT_STRING("onBinderDied"));
    NOTUSED(who);
    if (mHandler != nullptr) {
        (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_POWERMGR))->sendToTarget());
    }
}

bool PowerManagerAdapter::isIgnitionOn() {
    bool tIsIGOn {false};
    tIsIGOn = getIGStatus() == MCU_VALUE_STATUS::MCU_STATUS_ON;
    LOG_I(DLT_STRING("isIgnitionOn: "), DLT_STRING(tIsIGOn ? "true" : "false"));
    return tIsIGOn;
}

int32_t PowerManagerAdapter::getACCStatus() {
    int32_t accStatus = MCU_VALUE_STATUS::MCU_STATUS_UNKNOWN;
    PowerManager::instance()->requestToGet(MCU_STATUS_ACC, accStatus);
    LOG_I(DLT_STRING("ACC value "), DLT_INT(accStatus));
    return accStatus;
}

int32_t PowerManagerAdapter::getIGStatus() {
    int32_t igStatus = MCU_VALUE_STATUS::MCU_STATUS_UNKNOWN;
    PowerManager::instance()->requestToGet(MCU_STATUS_IG1, igStatus);
    LOG_I(DLT_STRING("IG value "), DLT_INT(igStatus));
    return igStatus;
}

bool PowerManagerAdapter::requestBUBModeACNWithPhone(bool isOn) {
    error_t ret = PowerManager::instance()->requestToSet(POWER_IDX::APP_RUNNING_ACN_WITH_PHONE, isOn ? BUB_APP_RUNNING : BUB_APP_STOP);
    if (ret != E_OK) {
        LOG_I(DLT_STRING("Notify ECALL ON to PwrMgr error"));
    }
    else {
        LOG_I(DLT_STRING("Notify ECALL ON to PwrMgr E_OK"));
    }

    return ret == E_OK;
}
bool PowerManagerAdapter::requestBUBModeECallRunning(bool isOn) {
    error_t ret = PowerManager::instance()->requestToSet(POWER_IDX::APP_RUNNING_ECALL, isOn ? BUB_APP_RUNNING : BUB_APP_STOP);
    if (ret != E_OK) {
        LOG_I(DLT_STRING("Notify ECALL ON to PwrMgr error"));
    }
    else {
        LOG_I(DLT_STRING("Notify ECALL ON to PwrMgr E_OK"));
    }

    return ret == E_OK;
}
bool PowerManagerAdapter::requestBUBModeACNRunning(bool isOn) {
    error_t ret = PowerManager::instance()->requestToSet(POWER_IDX::APP_RUNNING_ACN, isOn ? BUB_APP_RUNNING : BUB_APP_STOP);
    if (ret != E_OK) {
        LOG_I(DLT_STRING("Notify ECALL ON to PwrMgr error"));
    }
    else {
        LOG_I(DLT_STRING("Notify ECALL ON to PwrMgr E_OK"));
    }

    return ret == E_OK;
}

int32_t PowerManagerAdapter::getPowerLockCount() {
    return mPowerLockCount;
}
void PowerManagerAdapter::setAcnWithPhoneRunning(bool isAcnWithPhoneRunning) {
    mIsAcnWithPhoneRunning = isAcnWithPhoneRunning;
}
} /* End: namespace ECALLDCMNONREG */
