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
 * @file        HMIManagerAdapter.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "HMIManagerAdapter.h"
#include <memory>
#include "tspecall/core/utils/CommonFunctions.h"

namespace ecallapp {
HMIManagerAdapter::HMIManagerAdapter() {
}

std::shared_ptr<HMIManagerAdapter> HMIManagerAdapter::instance {};
HMIManagerAdapter* HMIManagerAdapter::getInstance() {
    if (instance == nullptr) {
        instance = std::make_shared<HMIManagerAdapter>();
    }
    return instance.get();
}

void HMIManagerAdapter::registerService() {
    LOG_I(DLT_STRING("registerService"));

    mHandler = ECallAppHandler::getInstance();
    if (mHandler == nullptr) {
        LOG_E(DLT_STRING("Handle nullptr"));
    }
    if (mHmiReceiver == nullptr) {
        mHmiReceiver = android::sp<HMIReiceiver>{new HMIReiceiver(*this)};
    }

    mService = android::interface_cast<IHMIManagerService>(android::defaultServiceManager()->getService(android::String16(HMI_SRV_NAME)));

    bool error {true};

    if(HMIManager::instance() != nullptr) {
        if (TIGER_ERR::E_OK == HMIManager::instance()->registerReceiver(0, mHmiReceiver)) {
            error = false;
        }
    }
    if (error) {
        LOG_E(DLT_STRING("Cannot register HMI Service, try again after 500ms"));
        (void)(mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_HMIMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS));
    }
}

void HMIManagerAdapter::turnOffCurrentLed() {
    // http://vopengrok.lge.com:8199/xref/toyota_26bev_release/nad/LGE/tiger-src/servicelayer/core/services/hmi-service/include/HMIType.h
    LOG_I(DLT_STRING("Turn off current LED pattern"));
    mCurrentLedPattern.onTime = 0U; // 0 for turn of current pattern LED
    (void)HMIManager::instance()->setLedPattern(mCurrentLedPattern);
}

void HMIManagerAdapter::requestLedPattern(const int32_t& pattern, const bool& isOn) {
        // play LED Pattern
    LOG_I(DLT_STRING("Play Led Pattern "), DLT_INT(pattern), DLT_STRING(", value = "),  DLT_STRING(isOn?"TRUE":"FALSE"));

    mCurrentLedPattern = {};
    if (isOn) {
        // http://vopengrok.lge.com:8199/xref/toyota_26bev_release/nad/LGE/tiger-src/servicelayer/core/services/hmi-service/include/HMIType.h
        mCurrentLedPattern.onTime = 1U; // 1 for turn on current pattern LED
    } else {
        mCurrentLedPattern.onTime = 0U; // 0 for turn of current pattern LED
    }
    switch (pattern)
    {
    case LED_ECALL_DIALING: {
        LOG_I(DLT_STRING("LED_ECALL_DIALING"));
        mCurrentLedPattern.ledPri = LED_ID::eLED_ECALL_DIAL;
        break;
    }
    case LED_ECALL_MSD_SENDING:
    {
        LOG_I(DLT_STRING("LED_ECALL_MSD_SENDING"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_MSD_SENDING;
        break;
    }
    case LED_ECALL_VOICE_CONNECTING:
    {
        LOG_I(DLT_STRING("LED_ECALL_VOICE_CONNECTING"));
        mCurrentLedPattern.ledPri = LED_ID::eLED_VOICE_CONN;
        break;
    }
    case LED_ECALL_DATA_FAIL : {
        LOG_I(DLT_STRING("LED_ECALL_DATA_FAIL"));
        mCurrentLedPattern.ledPri = LED_ID::eLED_JPN_CONN_FAIL;
        break;
    }
    case LED_ECALL_CONN_FAIL:
    {
        LOG_I(DLT_STRING("LED_ECALL_CONN_FAIL"));
        mCurrentLedPattern.ledPri = LED_ID::eLED_JPN_CONN_FAIL;
        break;
    }
    case LED_ECAL_UAE_STANDBY_TIME_AFTER_AUTO:
    {
        LOG_I(DLT_STRING("LED_ECAL_UAE_STANDBY_TIME_AFTER_AUTO"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_ACN_STANDBY;
        break;
    }
    case LED_ECAL_UAE_DTC_PRESENT:
    {
        LOG_I(DLT_STRING("LED_ECAL_UAE_DTC_PRESENT"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_DTC;
        break;
    }
    case LED_ECAL_IDLE:
    {
        LOG_I(DLT_STRING("LED_ECAL_IDLE"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_IDLE;
        break;
    }
    case LED_JAPAN_INIT_CHECK:{
        LOG_I(DLT_STRING("LED_JAPAN_INIT_CHECK"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_BOOTUP;
        break;
    }
    case LED_JAPAN_ERROR_STATE:{
        LOG_I(DLT_STRING("LED_JAPAN_ERROR_STATE"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_ERR_STATE;
        break;
    }

    case LED_JP_RCALL_ERROR: {
        LOG_I(DLT_STRING("LED_JP_RCALL_ERROR"));
        mCurrentLedPattern.ledPri = LED_ID::eLED_RCALL_FAIL;
        break;
    }
    case LED_JP_RCALL_RUNNING: {
        LOG_I(DLT_STRING("LED_JP_RCALL_RUNNING"));
        mCurrentLedPattern.ledPri = LED_ID::eLED_ECALL;
        break;
    }

    case LED_JAPAN_CONFIRM_CONTRACT_STATUS:{
        LOG_I(DLT_STRING("LED_JAPAN_CONFIRM_CONTRACT_STATUS"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_IDLE;
        break;
    }
    case LED_JAPAN_STANDBY_MODE_NO_CONTRACT:
    {
        LOG_I(DLT_STRING("LED_JAPAN_STANDBY_MODE_NO_CONTRACT"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_NO_SUBS;
        break;
    }
    case LED_JAPAN_STANDBY_MODE_WITH_CONTRACT:
    {
        LOG_I(DLT_STRING("LED_JAPAN_STANDBY_MODE_WITH_CONTRACT"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_IDLE;
        break;
    }
    case LED_JAPAN_TEST_CALL:
    {
        LOG_I(DLT_STRING("LED_JAPAN_TEST_CALL"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_MM_CHECKUP;
        break;
    }
    case LED_JAPAN_ECALL:
    {
        LOG_I(DLT_STRING("LED_JAPAN_ECALL"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_ECALL;
        break;
    }
    case LED_JAPAN_AUTO_INCOMING_STANDBY_MODE:
    {
        LOG_I(DLT_STRING("LED_JAPAN_AUTO_INCOMING_STANDBY_MODE"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_AT_INSTANDBY;
        break;
    }
    case LED_JAPAN_VOICE_COMM:
    {
        LOG_I(DLT_STRING("LED_JAPAN_VOICE_COMM"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_ACN;
        break;
    }

    case LED_JAPAN_COMM_FAIL:
    {
        LOG_I(DLT_STRING("LED_JAPAN_COMM_FAIL"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_JPN_CONN_FAIL;
        break;
    }
    case LED_JAPAN_ECALL_NO_DATA:
    {
        LOG_I(DLT_STRING("LED_JAPAN_ECALL_NO_DATA"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_ACN;
        break;
    }
    case LED_JAPAN_TEST_NO_DATA:
    {
        LOG_I(DLT_STRING("LED_JAPAN_TEST_NO_DATA"));
        mCurrentLedPattern.ledPri =LED_ID::eLED_JPN_CONN_FAIL;
        break;
    }
    default:
        break;
    }
    (void)HMIManager::instance()->setLedPattern(mCurrentLedPattern);
}

android::sp<IHMIManagerService> HMIManagerAdapter::getService()
{
    if (mService != nullptr) {
        mService = android::interface_cast<IHMIManagerService> (android::defaultServiceManager()->getService(android::String16(HMI_SRV_NAME)));
    }
    return mService;
}

void HMIManagerAdapter::handleHMIReceived(const uint32_t& type, const uint32_t& index, const uint32_t& action) {
    LOG_I(DLT_STRING("handleHMIReceived: "), DLT_UINT32(type), DLT_UINT32(index), DLT_UINT32(action));

    if (type == static_cast<uint32_t>(HMI_TYPE_BUTTON)){
        const uint8_t tAction = (uint8_t)action;
        const uint8_t current = tAction & 0x0FU;
        const uint8_t previous = (tAction & 0xF0U) >> 4;

        if(index == HMI_TYPE_BUTTON_ECALL) {
            if( (current == (uint8_t) HMI_BUTTON_SHORT_PRESS) || (previous == (uint8_t) HMI_BUTTON_SHORT_PRESS) ) {
                LOG_I(DLT_STRING("HMI_BUTTON_SHORT_PRESS"));
                const sp<sl::Message> message {mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_HMI_BUTTON_SHORT_PRESS), CommonFunctions::convertUint32ToInt32(index), CommonFunctions::convertUint32ToInt32(action))};
                (void)(message->sendToTarget());
            }
            else if ( (current == (uint8_t) HMI_BUTTON_LONG_PRESS) || (previous == (uint8_t) HMI_BUTTON_LONG_PRESS) ) {
                LOG_I(DLT_STRING("HMI_BUTTON_LONG_PRESS"));
                const sp<sl::Message> message {mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_HMI_BUTTON_LONG_PRESS), CommonFunctions::convertUint32ToInt32(index), CommonFunctions::convertUint32ToInt32(action))};
                (void)(message->sendToTarget());
            }
            else {
                LOG_I(DLT_STRING("HMI_TYPE_BUTTON_ECALL is triggered but same button event"));
            }
        }
        else {
            LOG_I(DLT_STRING("default case"));
        }
    }
}

void HMIManagerAdapter::sendButtonEventACKToMCU(const uint32_t& index, const uint8_t& action) const {
    LOG_I(DLT_STRING("sendButtonEventACKToMCU"));
    // Design: http://collab.lge.com/main/display/HONDATELE/2021-02-21+eCall+Trigger+Missing+Prevention+-+Button
    // http://vlm.lge.com/issue/browse/HONDATELE-7689?attachmentSortBy=dateTime&attachmentOrder=asc
    // https://orca.rd.honda.co.jp/jira/browse/FGSTSU-3007
    // legacy code: http://vgit.lge.com/as/c/honda/con/linux/apps/oemcallapp/+/1762607
    // http://vgit.lge.com/as/c/honda/con/linux/apps/oemcallapp/+/1516663/1/src/services_adapter/hmi_manager_adapter.cpp
    // 0xC0 = 0b11000000, 0x38 = 0b00111000, 0x07 = 0b0000000111
    // HMI_BUTTON_RELEAS = (0x00) = 0b00000000
    // HMI_BUTTON_DETECT = (0x01) = 0b00000001
    // HMI_BUTTON_SHORT_PRESS = (0x02) = 0b00000010
    // HMI_BUTTON_DETECT_PRESS = (0x03) = 0b00000011
    // HMI_BUTTON_LONG_PRESS = (0x04) = 0b00000100
    // DETECT|SHOR|LONG = 0b00000111
    const static uint8_t previousDetectionBitmask{0x38U};
    const static uint8_t currentDetectionBitmask{0x07U};
    if (index == static_cast<uint32_t>(HMI_TYPE_BUTTON_ECALL)) {
        // 0xC0 = 0b11000000 for SW_A
        // (0x38U & (action >> 1U)) -> to calculate bit mask for previous detection event action & 0xF0
        // (0x07U & action) -> to calculate bit mask for current detection event action & 0x0F
        const static uint8_t buttonSWABitmask{0xC0U};
        const uint8_t ack_SW_A = buttonSWABitmask | (previousDetectionBitmask & (action >> 1U)) | (currentDetectionBitmask & action);
        (void) HMIManager::instance()->sendEvent(ack_SW_A);
    }
    // 0x80 = 0b10000000, 0x38 = 0b00111000, 0x07 = 0b0000000111
    else if (index == static_cast<uint32_t>(HMI_TYPE_BUTTON_BCALL)) {
        // 0x80 = 0b10000000 for SW_B
        const static uint8_t buttonSWBBitmask{0x80U};
        const uint8_t ack_SW_B = buttonSWBBitmask | (previousDetectionBitmask & (action >> 1U)) | (currentDetectionBitmask & action);
        (void) HMIManager::instance()->sendEvent(ack_SW_B);
    }
    else {
        LOG_W(DLT_STRING("Invalid type button"));
    }
}

void HMIManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who) {
    LOG_I(DLT_STRING("onBinderDied"));
    NOTUSED(who);
    (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_HMIMGR))->sendToTarget());
}

} /* End: namespace ECALLDCMNONREG */

