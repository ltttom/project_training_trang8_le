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
 * @file        NADESSECall.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "NADESSECall.h"
#include "../../../core/services/VehicleManagerAdapter.h"

namespace ecallapp {

    NADESSECall::NADESSECall() {
        mTSUToRVUECallNoRequestCount = 0U;
        mRVUtoTSUMessValue = self::DESS_UNDEFINDED;

        mTimerHandler = nullptr;
        mECallStatusCancelRequest = nullptr;
        mECallStatus = nullptr;
        mECallNotificationRequestTimer = nullptr;
        mECallNotificationNoRequestTimer = nullptr;
        mECallNotificationCancelTimer = nullptr;
        mDessConfirmNoRequest = nullptr;
        mDessConfirmRequest = nullptr;
        mDessConfirmCancelRequest = nullptr;
        mECallStatusNotificationValue = dess_param::TSU_TO_RVU_ECALL_NO_REQUEST;
    }

    void NADESSECall::initialize() {
        mTimerHandler = std::make_unique<ECallTimerHandler>();

        mECallStatusCancelRequest = android::sp<Timer>{new Timer(mTimerHandler.get(), static_cast<int32_t>(ecall_na_timer::ECALL_STATUS_CANCEL_REQUEST_30S))};
        mECallStatusCancelRequest->setDurationMs(ecall_na_timer::ECALL_STATUS_CANCEL_REQUEST_30S_DURATION, 0U);

        mECallStatus = android::sp<Timer>{new Timer(mTimerHandler.get(), static_cast<int32_t>(ecall_na_timer::ECALL_STATUS_30S))};
        mECallStatus->setDurationMs(ecall_na_timer::ECALL_STATUS_30S_DURATION, 0U);

        // ecall noty TSU RVU Tx timer
        mECallNotificationRequestTimer = android::sp<Timer>{new Timer(mTimerHandler.get(), static_cast<int32_t>(ecall_na_timer::ECALL_NOTIFICATION_TIMER_REQUEST))};
        mECallNotificationRequestTimer->setDurationMs(ecall_na_timer::ECALL_NOTIFICATION_TIMER_REQUEST_DURATION, 0U);
        mECallNotificationNoRequestTimer = android::sp<Timer>{new Timer(mTimerHandler.get(), static_cast<int32_t>(ecall_na_timer::ECALL_NOTIFICATION_TIMER_NO_REQUEST))};
        mECallNotificationNoRequestTimer->setDurationMs(ecall_na_timer::ECALL_NOTIFICATION_TIMER_NO_REQUEST_DURATION, 0U);
        mECallNotificationCancelTimer = android::sp<Timer>{new Timer(mTimerHandler.get(), static_cast<int32_t>(ecall_na_timer::ECALL_NOTIFICATION_CANCEL_ECALL_TRIGGER))};
        mECallNotificationCancelTimer->setDurationMs(ecall_na_timer::ECALL_NOTIFICATION_CANCEL_ECALL_TRIGGER_DURTAION, 0U);

        // ACK Tx timer (TSU - RVU)
        mDessConfirmNoRequest = android::sp<Timer>{new Timer(mTimerHandler.get(), static_cast<int32_t>(ecall_na_timer::DESS_CONFIRM_TIMER_NO_REQUEST))};
        mDessConfirmNoRequest->setDurationMs(ecall_na_timer::DESS_CONFIRM_TIMER_NO_REQUEST_DURATION, 0U);
        mDessConfirmRequest = android::sp<Timer>{new Timer(mTimerHandler.get(), static_cast<int32_t>(ecall_na_timer::DESS_CONFIRM_TIMER_REQUEST))};
        mDessConfirmRequest->setDurationMs(ecall_na_timer::DESS_CONFIRM_TIMER_REQUEST_DURATION, 0U);
        mDessConfirmCancelRequest = android::sp<Timer>{new Timer(mTimerHandler.get(), static_cast<int32_t>(ecall_na_timer::DESS_CONFIRM_TIMER_CANCEL_REQUEST))};
        mDessConfirmCancelRequest->setDurationMs(ecall_na_timer::DESS_CONFIRM_TIMER_CANCEL_REQUEST_DURATION, 0U);

        mTSUToRVUECallNoRequestCount = 0U;
    }

    void NADESSECall::launchFeature(const std::string strFeatureName) {
        LOG_I(DLT_STRING("launchFeature, feature name = "), DLT_STRING(strFeatureName.c_str()));
    }

    void NADESSECall::ignoreFeature(const std::string strFeatureName) {
        LOG_I(DLT_STRING("ignoreFeature, feature name = "), DLT_STRING(strFeatureName.c_str()));
    }

    void NADESSECall::terminateFeature(const std::string strFeatureName) {
        LOG_I(DLT_STRING("terminateFeature, feature name = "), DLT_STRING(strFeatureName.c_str()));
    }

    void NADESSECall::sendECallRequestNotificationToRVU(const uint32_t value) {
        // 2s tx confirm timer
        mECallNotificationRequestTimer->stop();
        mECallNotificationCancelTimer->stop();
        mECallNotificationNoRequestTimer->stop();
        if (value == dess_param::TSU_TO_RVU_ECALL_REQUEST) {
            LOG_I(DLT_STRING("dess_param::TSU_TO_RVU_ECALL_REQUEST"));
            mECallStatusNotificationValue = dess_param::TSU_TO_RVU_ECALL_REQUEST;
            // http://avncb.lge.com:8080/cb/issue/28977150
            VehicleManagerAdapter::getInstance()->notifySOSPressedToRVU(mECallStatusNotificationValue);
            // stop 30s timer
            mECallStatusCancelRequest->stop();

            // The retry timeout shall be 30 s.
            mECallStatus->stop();
            mECallStatus->start();
            // 2s tx confirm timer
            mECallNotificationRequestTimer->stop();
            mECallNotificationRequestTimer->start();
        }
        else if (value == dess_param::TSU_TO_RVU_ECALL_NO_REQUEST) {
            LOG_I(DLT_STRING("dess_param::TSU_TO_RVU_ECALL_NO_REQUEST"));
            mECallStatusNotificationValue = dess_param::TSU_TO_RVU_ECALL_NO_REQUEST;
            // stop 30s timer
            mECallStatus->stop();
            mECallStatusCancelRequest->stop();

            mTSUToRVUECallNoRequestCount = 0U;
            VehicleManagerAdapter::getInstance()->notifySOSPressedToRVU(mECallStatusNotificationValue);
            // stop and then start 2s tx timer
            mECallNotificationNoRequestTimer->stop();
            mECallNotificationNoRequestTimer->start();
        }
        else if (value == dess_param::TSU_TO_RVU_ECALL_CANCEL) {
            LOG_I(DLT_STRING("dess_param::TSU_TO_RVU_ECALL_CANCEL"));
            mECallStatusNotificationValue = dess_param::TSU_TO_RVU_ECALL_CANCEL;
            VehicleManagerAdapter::getInstance()->notifySOSPressedToRVU(mECallStatusNotificationValue);
            // stop 30s timer
            mECallStatus->stop();

            // start 30s timer
            mECallStatusCancelRequest->stop();
            mECallStatusCancelRequest->start();

            // start tx timer
            mECallNotificationCancelTimer->stop();
            mECallNotificationCancelTimer->start();
        }
        else {
            LOG_I(DLT_STRING("default branch"));
        }

    }

    void NADESSECall::processorHandleMessage(const uint32_t& what, const android::sp<sl::Message>& msgData) {
        LOG_I(DLT_STRING("processorHandleMessage"));
        switch (what) {
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_BOOT_COMPLETED) : {
            LOG_I(DLT_STRING("RECV_MSG_BOOT_COMPLETED"));
        }
        break;
        // http://avncb.lge.com:8080/cb/issue/28977124
        // DESS_ECALL_NO_REQUEST: 0: No request
        // DESS_ECALL_REQUEST: 1: Requested
        // DESS_CANCEL_ECALL_TRIGGER: 2: Request cancelled
        // other value: 0: no request
        case ENUM_TO_UINT32(msg::MSG::MES_STATUS_NOTIFICATION) : {
            LOG_I(DLT_STRING("Received message MES_STATUS_NOTIFICATION = "), DLT_INT(msgData->arg1) );
            mDessConfirmNoRequest->stop();
            mDessConfirmRequest->stop();
            mDessConfirmCancelRequest->stop();
            switch (msgData->arg1) {
                // http://avncb.lge.com:8080/cb/issue/28977152
                // MES_RECEIVED_VALUE
                case self::DESS_ECALL_NO_REQUEST: { // RVU - TSU: no request dess eCall
                    LOG_I(DLT_STRING("DESS_ECALL_NO_REQUEST"));
                    mRVUtoTSUMessValue = self::DESS_ECALL_NO_REQUEST;
                    VehicleManagerAdapter::getInstance()->sendACKToRVU(self::DESS_ECALL_NO_REQUEST);
                    // DESS_CONFIRM_TIMER_NO_REQUEST_DURATION: 2s Tx Confirm
                    mDessConfirmNoRequest->stop();
                    mDessConfirmNoRequest->start();
                }
                    break;
                case self::DESS_ECALL_REQUEST: { // RVU - TSU: request dess eCall
                    LOG_I(DLT_STRING("DESS_ECALL_REQUEST"));
                    mRVUtoTSUMessValue = self::DESS_ECALL_REQUEST;
                    // (void)CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::DESS_FILE);
                    ECallAppHandler::getInstance()->handleMessageInternal(ECallAppHandler::getInstance()->obtainMessage(ENUM_TO_INT(msg::AppMSG::DESS_ECALL_REQUEST_MSG)));
                    VehicleManagerAdapter::getInstance()->sendACKToRVU(self::DESS_ECALL_REQUEST);
                    // 2s Tx Confirm
                    mDessConfirmRequest->stop();
                    mDessConfirmRequest->start();
                }
                    break;
                case self::DESS_CANCEL_ECALL_TRIGGER: { // RVU - TSU: request cancel dess eCall
                    LOG_I(DLT_STRING("DESS_CANCEL_ECALL_TRIGGER"));
                    mRVUtoTSUMessValue = self::DESS_CANCEL_ECALL_TRIGGER;
                    VehicleManagerAdapter::getInstance()->sendACKToRVU(self::DESS_CANCEL_ECALL_TRIGGER);
                    ECallAppHandler::getInstance()->handleMessageInternal(ECallAppHandler::getInstance()->obtainMessage(ENUM_TO_INT(msg::AppMSG::DESS_ECALL_CANCEL_MSG)));
                    // DESS_CONFIRM_TIMER_CANCEL_REQUEST_DURATION: 2s Tx Confirm
                    mDessConfirmCancelRequest->stop();
                    mDessConfirmCancelRequest->start();
                    break;
                }
                default: {  // RVU - TSU: default no request dess eCall
                    LOG_I(DLT_STRING("default DESS_ECALL_NO_REQUEST"));
                    mRVUtoTSUMessValue = self::DESS_ECALL_NO_REQUEST;
                    VehicleManagerAdapter::getInstance()->sendACKToRVU(self::DESS_ECALL_NO_REQUEST);
                    //2s Tx Confirm
                    mDessConfirmNoRequest->stop();
                    mDessConfirmNoRequest->start();
                    break;
                }
            }
            break;
        }

        case ENUM_TO_UINT32(msg::MSG::MES_RECEIVED_VALUE): {
            LOG_I(DLT_STRING("Received message MSG_CONFIRM_RECEIVED_VALUE = "), DLT_INT(msgData->arg1));
            // MES_STATUS_NOTIFICATION - MES_RECEIVED_VALUE : Tx Confirmed
            switch (msgData->arg1) {
                case self::DESS_ECALL_NO_REQUEST: {
                    LOG_I(DLT_STRING("DESS_ECALL_NO_REQUEST"));
                    mDessConfirmNoRequest->stop();
                    break;
                }
                case self::DESS_ECALL_REQUEST: {
                    LOG_I(DLT_STRING("DESS_ECALL_REQUEST"));
                    mDessConfirmRequest->stop();
                    break;
                }
                case self::DESS_CANCEL_ECALL_TRIGGER: {
                    LOG_I(DLT_STRING("DESS_CANCEL_ECALL_TRIGGER"));
                    mDessConfirmCancelRequest->stop();
                    break;
                }
                default:
                    break;
            }
            break;
        }

        // http://avncb.lge.com:8080/cb/issue/28977125
        // ECALL_STATUS_NOTIFICATION (TSU - RVU value)
        // 0: Noti No request
        // 1: Requested
        // 2: Request cancelled
        // other value: 0: no request
        case ENUM_TO_UINT32(msg::AppMSG::TSU_TO_RVU_ECALL_STATUS_NOTIFICATION_NO_REQUEST): {
            LOG_I(DLT_STRING("TSU_TO_RVU_ECALL_STATUS_NOTIFICATION_NO_REQUEST"));
            sendECallRequestNotificationToRVU(dess_param::TSU_TO_RVU_ECALL_NO_REQUEST);
            break;
        }
        case ENUM_TO_UINT32(msg::AppMSG::TSU_TO_RVU_ECALL_STATUS_NOTIFICATION_REQUEST): {
            LOG_I(DLT_STRING("TSU_TO_RVU_ECALL_STATUS_NOTIFICATION_REQUEST"));
            sendECallRequestNotificationToRVU(dess_param::TSU_TO_RVU_ECALL_REQUEST);
            break;
        }
        case ENUM_TO_UINT32(msg::AppMSG::TSU_TO_RVU_ECALL_STATUS_NOTIFICATION_CANCEL): {
            LOG_I(DLT_STRING("TSU_TO_RVU_ECALL_STATUS_NOTIFICATION_CANCEL"));
            sendECallRequestNotificationToRVU(dess_param::TSU_TO_RVU_ECALL_CANCEL);
            break;
        }

        // ACK for ecall request noti / ecall cancel request noti
        // http://avncb.lge.com:8080/cb/issue/28977151
        // Condition 1-2
        // ①ACK was received: ECALL_RECEIVED_VALUE =1 was received from the ADAS ECU.
        // ②The timeout occurred: ECALL_STATUS_NOTIFICATION＝1 was transmitted for 30 s.
        // Condition 3-2
        // ①ACK was received: ECALL_RECEIVED_VALUE = 2 was received from the ADAS ECU.
        // ②The timeout occurred: ECALL_STATUS_NOTIFICATION = 2 was transmitted for 30 s.
        case ENUM_TO_UINT32(msg::MSG::MES_ECALL_RECEIVED_VALUE): {
            LOG_I(DLT_STRING("MES_ECALL_RECEIVED_VALUE"));
            sendECallRequestNotificationToRVU(dess_param::TSU_TO_RVU_ECALL_NO_REQUEST);
            break;
        }
        case ENUM_TO_UINT32(msg::MSG::MES_ECALL_STATUS_NOTIFICATION): {
            LOG_I(DLT_STRING("Tx Confirm for ECALL_STATUS_NOTIFICATION "));
            switch (msgData->arg1) {
                case self::DESS_ECALL_NO_REQUEST: {
                    mECallNotificationNoRequestTimer->stop();
                    break;
                }
                case self::DESS_ECALL_REQUEST: {
                    mECallNotificationRequestTimer->stop();
                    break;
                }
                case self::DESS_CANCEL_ECALL_TRIGGER: {
                    mECallNotificationCancelTimer->stop();
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default :
            break;
        }
    }

    void NADESSECall::onTimerTimeoutEvent(const int32_t& msgId) {
        LOG_I(DLT_STRING("onTimerTimeoutEvent "), DLT_INT(msgId));

        // ACK Tx confirm timer
        if(msgId == ecall_na_timer::DESS_CONFIRM_TIMER_NO_REQUEST) {
            if(mRVUtoTSUMessValue == self::DESS_ECALL_NO_REQUEST) {
                LOG_I(DLT_STRING("2s timer timeout, OemCallApp has not received DESS_CONFIRM_TIMER_NO_REQUEST confirmation from VM yet, hence resend ACK..."));
                VehicleManagerAdapter::getInstance()->sendACKToRVU(self::DESS_ECALL_NO_REQUEST);
                mDessConfirmNoRequest->stop();
                mDessConfirmNoRequest->start();
            }

        }
        // ACK Tx confirm timer
        else if(msgId == ecall_na_timer::DESS_CONFIRM_TIMER_REQUEST) {
            if(mRVUtoTSUMessValue == self::DESS_ECALL_REQUEST) {
                LOG_I(DLT_STRING("2s timer timeout, OemCallApp has not received DESS_CONFIRM_TIMER_NO_REQUEST confirmation from VM yet, hence resend ACK..."));
                VehicleManagerAdapter::getInstance()->sendACKToRVU(self::DESS_ECALL_REQUEST);
                mDessConfirmRequest->stop();
                mDessConfirmRequest->start();
            }

        }
        // ACK Tx confirm timer
        else if(msgId == ecall_na_timer::DESS_CONFIRM_TIMER_CANCEL_REQUEST) {
            if(mRVUtoTSUMessValue == self::DESS_CANCEL_ECALL_TRIGGER) {
                LOG_I(DLT_STRING("2s timer timeout, OemCallApp has not received DESS_CONFIRM_TIMER_CANCEL_REQUEST confirmation from VM yet, hence resend ACK..."));
                VehicleManagerAdapter::getInstance()->sendACKToRVU(self::DESS_CANCEL_ECALL_TRIGGER);
                mDessConfirmCancelRequest->stop();
                mDessConfirmCancelRequest->start();
            }
        }

        // http://avncb.lge.com:8080/cb/issue/28977151
        // Condition 1-2
        // ②The timeout occurred: ECALL_STATUS_NOTIFICATION＝1 was transmitted for 30 s.
        else if(msgId == ecall_na_timer::ECALL_STATUS_30S)  {
            LOG_I(DLT_STRING("ECALL_STATUS_30S"));
            // 2s tx confirm timer
            mECallNotificationRequestTimer->stop();
            mECallNotificationCancelTimer->stop();
            mECallNotificationNoRequestTimer->stop();
            sendECallRequestNotificationToRVU(dess_param::TSU_TO_RVU_ECALL_NO_REQUEST);
        }
        // http://avncb.lge.com:8080/cb/issue/28977151
        // Condition 3-2
        // ②The timeout occurred: ECALL_STATUS_NOTIFICATION＝2 was transmitted for 30 s.
        else if(msgId == ecall_na_timer::ECALL_STATUS_CANCEL_REQUEST_30S) {
            LOG_I(DLT_STRING("ECALL_STATUS_CANCEL_REQUEST_30S"));
            // 2s tx confirm timer
            mECallNotificationRequestTimer->stop();
            mECallNotificationCancelTimer->stop();
            mECallNotificationNoRequestTimer->stop();
            sendECallRequestNotificationToRVU(dess_param::TSU_TO_RVU_ECALL_NO_REQUEST);
        }

        // ECall noti Tx confirm
        else if(msgId == ecall_na_timer::ECALL_NOTIFICATION_TIMER_REQUEST) {
            LOG_I(DLT_STRING("ECALL_NOTIFICATION_TIMER_REQUEST time out"));
            if (mECallStatusNotificationValue == dess_param::TSU_TO_RVU_ECALL_REQUEST) {
                VehicleManagerAdapter::getInstance()->notifySOSPressedToRVU(mECallStatusNotificationValue);
                mECallNotificationRequestTimer->stop();
                mECallNotificationRequestTimer->start();
            }
        }
        else if(msgId == ecall_na_timer::ECALL_NOTIFICATION_TIMER_NO_REQUEST) {
            LOG_I(DLT_STRING("ECALL_NOTIFICATION_TIMER_NO_REQUEST time out"));

            if (mECallStatusNotificationValue == dess_param::TSU_TO_RVU_ECALL_NO_REQUEST) {
                mTSUToRVUECallNoRequestCount++;
                if (mTSUToRVUECallNoRequestCount <= 15U) { // retry for 30s to reduce waisting performance
                    VehicleManagerAdapter::getInstance()->notifySOSPressedToRVU(mECallStatusNotificationValue);
                    mECallNotificationNoRequestTimer->stop();
                    mECallNotificationNoRequestTimer->start();
                }
                else {
                    mTSUToRVUECallNoRequestCount = 0U;
                }
            }
        }
        else if(msgId == ecall_na_timer::ECALL_NOTIFICATION_CANCEL_ECALL_TRIGGER) {
            LOG_I(DLT_STRING("ECALL_NOTIFICATION_CANCEL_ECALL_TRIGGER time out"));
            if (mECallStatusNotificationValue == dess_param::TSU_TO_RVU_ECALL_CANCEL) {
                VehicleManagerAdapter::getInstance()->notifySOSPressedToRVU(mECallStatusNotificationValue);
                mECallNotificationCancelTimer->stop();
                mECallNotificationCancelTimer->start();
            }
        }
    }
}