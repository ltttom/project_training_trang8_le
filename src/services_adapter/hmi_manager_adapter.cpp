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
 * @file        hmi_manager.cpp
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/

#include "hmi_manager_adapter.h"
#include <ios>
#include <services/HMIManagerService/IHMIManagerService.h>
#include <services/HMIManagerService/HMIManager.h>
#include <services/HMIManagerService/HMIType.h>
#include <binder/IServiceManager.h>
#include "../utils/utils_common.h"
#include "../logger.h"
#include "../oem_call_app_constants.h"
#include "Log.h"
#include "app_manager_adapter.h"
#include "../tspecall/core/services/VehicleVariationAdapter.h"


namespace {

    const int ECALL_RUNNING  = 14;
    const int ECALL_ERROR    = 16;
    const int RCALL_ERROR    = 11;
    const int RCALL_RUNNING  = 14;
    const int IDLE = 1;
    const int NO_SUBS = 2;
}

/**
 * @brief This contains the private properties of HmiManager class instance
 *
 */
struct oc::HmiManagerAdapter::Impl {
    android::sp<oc::OemCallHandler> spHandler;

    void processEcallTrigger(bool savingFlash = false) {

        if (savingFlash) {
            if (cv::Common::saveToNVM(1, "ecall.txt") != E_OK)
            {
                LOG_E(DLT_STRING("Cannot save data to NVM"));
            }
        }
        spHandler->obtainMessage(msg::MSG_HMIM_ECALL_TRIGGER)->sendToTarget();
    }
};


oc::HmiManagerAdapter::HmiManagerAdapter(android::sp<oc::OemCallHandler> handler)
    :impl_(new Impl()) {
    LOG_I(DLT_STRING("Creating HmiManagerAdapter ..."));
    impl_->spHandler = handler;
    uint8_t res = 0;
    if (cv::Common::readFromNVM(res, "ecall.txt") != E_ERROR) {
        impl_->spHandler->obtainMessage(msg::MSG_HMIM_ECALL_TRIGGER)->sendToTarget();
    }
}

void oc::HmiManagerAdapter::doHandleHMIMessages(const android::sp<sl::Message> &message, bool savingFlash) {
    auto index = message->arg1;
    if (index == (uint8_t)HMI_TYPE_BUTTON_BCALL) {
        // if (oc::AppManagerAdapter::isAvailableInRegion(self::feature_rcall_jp)) {
          bool isRCallJPAvailable = ecallapp::VehicleVariationAdapter::getInstance()
                                            ->isFeatureAvailableInRegion(ecallapp::feature_name::rcall_japan);

          if (isRCallJPAvailable) {
              uint8_t action = uint8_t(message->arg2);
              uint8_t ack_SW_B = 0x80 | (0x38 & (action >> 1)) | (0x07 &action);
              uint8_t current = action & 0x0F;
              if (current == (uint8_t)HMI_BUTTON_SHORT_PRESS) {
                  LOG_I(DLT_STRING("Rcall press"));
                  HMIManager::instance()->sendEvent(ack_SW_B);
                  impl_->spHandler->obtainMessage(msg::RCALL_TRIGGER)->sendToTarget();
              }
        }
    }

    // check previous
}

void oc::HmiManagerAdapter::doHandleRetrySaveTrigger(const android::sp<sl::Message> &message)
{
    impl_->processEcallTrigger();
}
void oc::HmiManagerAdapter::doHandleSaveTrigger(bool savingFlash)
{
    impl_->processEcallTrigger(savingFlash);
}
void oc::HmiManagerAdapter::requestPattern(const oc::HmiManagerAdapter::Pattern pattern, bool isOn) {
    HMILedPatternInfo ledInfo = {};

    switch (pattern) {
    case Pattern::US_ECALL_CONNECTING:
        LOG_I(DLT_STRING("Request US_ECALL_RUNNING, isOn = "), DLT_BOOL(isOn));
        ledInfo.ledPri = eLED_ECALL_DIAL;
        ledInfo.onTime = isOn ? 1 : 0;
    break;
    case Pattern::US_ACN_CONNECTING:
        LOG_I(DLT_STRING("Request US_ACN_RUNNING, isOn = "), DLT_BOOL(isOn));
        ledInfo.ledPri = eLED_ECALL_DIAL;
        ledInfo.onTime = isOn ? 1 : 0;
    break;
    case Pattern::US_ECALL_CONNECTED:
        LOG_I(DLT_STRING("Request US_ECALL/ACN_CONNECTED, isOn = "), DLT_BOOL(isOn));
        ledInfo.ledPri = eLED_VOICE_CONN;
        ledInfo.onTime = isOn ? 1 : 0;
    break;
    case Pattern::US_ECALL_ERROR:
        LOG_I(DLT_STRING("Request US_ECALL/ACN_ERROR, isOn = "), DLT_BOOL(isOn));
        ledInfo.ledPri = eLED_UAE_CONN_FAIL;
        ledInfo.onTime = isOn ? 1 : 0;
    break;
    case Pattern::JP_RCALL_ERROR:
        LOG_I(DLT_STRING("Request JP_RCALL_ERROR, isOn = "), DLT_BOOL(isOn));
        ledInfo.ledPri = eLED_RCALL_FAIL;
        ledInfo.onTime = isOn ? 1 : 0;
    break;
    case Pattern::JP_RCALL_RUNNING:
        LOG_I(DLT_STRING("Request JP_RCALL_RUNNING, isOn = "), DLT_BOOL(isOn));
        ledInfo.ledPri = eLED_ECALL;
        ledInfo.onTime = isOn ? 1 : 0;
    break;
    case Pattern::LED_IDLE:
        LOG_I(DLT_STRING("Request LED_IDLE, isOn = "), DLT_BOOL(isOn));
        ledInfo.ledPri = eLED_IDLE;
        ledInfo.onTime = isOn ? 1 : 0;
    break;
    case Pattern::LED_OFF_NO_SUBS:
        LOG_I(DLT_STRING("Request LED_OFF_NO_SUBS, isOn = "), DLT_BOOL(isOn));
        ledInfo.ledPri = eLED_NO_SUBS;
        ledInfo.onTime = isOn ? 1 : 0;
    break;
    case Pattern::LED_OFF_OTHER:
        LOG_I(DLT_STRING("Request LED_OFF_OTHER, isOn = "), DLT_BOOL(isOn));
        ledInfo.ledPri = eLED_OTHER;
        ledInfo.onTime = isOn ? 1 : 0;
    break;
    default:
        return;
    }
    if (HMIManager::instance()->setLedPattern(ledInfo) != E_OK){
        LOG_E(DLT_STRING("Cannot set LED pattern"));
    }
}

