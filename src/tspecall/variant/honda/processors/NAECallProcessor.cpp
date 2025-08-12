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
 * @file        NAECallProcessor.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "NAECallProcessor.h"
#include "../../../core/services/PowerManagerAdapter.h"
#include "../../../core/services/ApplicationManagerAdapter.h"
#include "../../../core/services/AudioManagerAdapter.h"
#include "../../../core/services/TelephoneManagerAdapter.h"
#include "../../../core/services/HTTPSManagerAdapter.h"
#include "../../../core/services/AlarmManagerAdapter.h"
#include "../../../core/services/ConfigurationManagerAdapter.h"
#include "../../../core/services/HMIManagerAdapter.h"
#include "../../../core/services/CommunicationManagerAdapter.h"
#include "../../../core/services/VehicleManagerAdapter.h"
#include "tspecall/variant/honda/msd/DataBuilderUS.h"
#include "tspecall/core/services/SystemPropertyAdapter.h"
#include "tspecall/core/services/TimeManagerAdapter.h"
#include "../../../core/utils/ParamsDef.h"
#include "ECallVariableRepo.h"

namespace ecallapp {

    NAECallProcessor::NAECallProcessor() {
        mTimerHandler = nullptr;
        mCancelingTimer = nullptr;
        mCallRetryTimer = nullptr;
        mCallDiallingTimer = nullptr;
        mStateMachine = nullptr;
        mAttemptVoiceCount = 0;
        mMaxAttemptVoice = 0;
        mAttemptDataCount = 0;
        mMaxAttemptData = NUMBER_OF_MSD_TRANSMISSION_ATTEMP;
    }

    void NAECallProcessor::initialize() {
        mTimerHandler = std::make_unique<ECallTimerHandler>();

        mCancelingTimer = android::sp<Timer>{new Timer(mTimerHandler.get(), static_cast<int32_t>(ecall_na_timer::CANCELLING_TIMER_ID))};
        mCancelingTimer->setDurationMs(ecall_na_timer::CANCELLING_TIMER_DURATION, 0U);

        mCallDiallingTimer = android::sp<Timer>{new Timer(mTimerHandler.get(), static_cast<int32_t>(ecall_na_timer::CALL_DIALLING_TIMER_ID))};
        mCallDiallingTimer->setDurationMs(ecall_na_timer::CALL_DIALLING_TIMER_DURATION, 0U);

        mCallRetryTimer = android::sp<Timer>{new Timer(mTimerHandler.get(), static_cast<int32_t>(ecall_na_timer::CALL_RETRY_TIMER_ID))};
        mCallRetryTimer->setDurationMs(ecall_na_timer::CALL_RETRY_TIMER_DURATION, 0U);

        mStateMachine = new NAECallStateMachine(this);
    }

    void NAECallProcessor::doNotifySOSRequest() {
        ECallAppHandler::getInstance()->handleMessageInternal(
            ECallAppHandler::getInstance()->obtainMessage(
                ENUM_TO_INT(msg::AppMSG::TSU_TO_RVU_ECALL_STATUS_NOTIFICATION_REQUEST)));
    }

    void NAECallProcessor::doNotifySOSNoRequest() {
        ECallAppHandler::getInstance()->handleMessageInternal(
            ECallAppHandler::getInstance()->obtainMessage(
                ENUM_TO_INT(msg::AppMSG::TSU_TO_RVU_ECALL_STATUS_NOTIFICATION_NO_REQUEST)));
    }

    void NAECallProcessor::doNotifySOSCancel() {
        ECallAppHandler::getInstance()->handleMessageInternal(
            ECallAppHandler::getInstance()->obtainMessage(
                ENUM_TO_INT(msg::AppMSG::TSU_TO_RVU_ECALL_STATUS_NOTIFICATION_CANCEL)));
    }

    void NAECallProcessor::sendSOSAck(int32_t arg1, int32_t arg2) {
        HMIManagerAdapter::getInstance()->sendButtonEventACKToMCU(arg1, arg2);
    }

    void NAECallProcessor::sendCDSAck(int32_t arg1, int32_t arg2) {
        CommunicationManagerAdapter::getInstance()->requestSendACKToMCU(arg1, arg2);
    }

    void NAECallProcessor::doSignalAckOnActiveCall(int32_t event, int32_t arg1, int32_t arg2) {
        switch(event) {
            case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH):
                sendCDSAck(arg1, arg2);
                break;

            case ENUM_TO_UINT32(msg::MSG::RECV_MSG_HMI_BUTTON_SHORT_PRESS):
                sendSOSAck(arg1, arg2);
                break;

            case ENUM_TO_UINT32(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER):
            default:
                break;
        }
    }

    void NAECallProcessor::setQueryFeatureName(const std::string& featureName) {
        mQueryFeatureName = featureName;
    }

    std::string NAECallProcessor::getQueryFeatureName() const {
        return mQueryFeatureName;
    }

    void NAECallProcessor::doActionsAtLaunchingTime(bool isRecover) {
        LOG_I(DLT_STRING("doActionsAtLaunchingTime"));
        auto& repo = ECallVariableRepo::getInstance();
        (void)AudioManagerAdapter::getInstance()->setMute(true);
        // acquirePowerLock for TSU not to turn to sleep mode.
        PowerManagerAdapter::getInstance()->acquirePowerLock();
        mMaxAttemptVoice = (repo.getECallType() == ecall_type::ACN) ? VOICE_DIALING_INFINITE_ATTEMPT : VOICE_DIALING_ATTEMPT_IN_MANUAL_MODE;
        mMaxAttemptData = NUMBER_OF_MSD_TRANSMISSION_ATTEMP; // unlimited time

        if (repo.getECallType() == ecall_type::ACN) {
            ApplicationManagerAdapter::getInstance()->sendPostACNUS_ON_OFF(ecallapp::ACNUS_ON);
        }
        if (!isRecover) {
            if(AudioManagerAdapter::getInstance()->isPlaying()) {
                ECallVariableRepo::getInstance().setECallClearAudioState(ecall_audio_state::CLEAR_TONE_1);
                (void)AudioManagerAdapter::getInstance()->stopAudioFile();
            }
            else {
                (void)AudioManagerAdapter::getInstance()->playAudioFile(audio::TONE_1); // dialling/preparation sound tone
            }
            HMIManagerAdapter::getInstance()->requestLedPattern(ecallapp::LED_ECALL_DIALING, true); // dialling/preparation led pattern
        }
    }

    void NAECallProcessor::launchFeature(const std::string strFeatureName) {
        LOG_I(DLT_STRING("launchFeature, feature name = "), DLT_STRING(strFeatureName.c_str()));
    }

    void NAECallProcessor::ignoreFeature(const std::string strFeatureName) {
        LOG_I(DLT_STRING("ignoreFeature, feature name = "), DLT_STRING(strFeatureName.c_str()));
    }

    void NAECallProcessor::terminateFeature(const std::string strFeatureName) {
        LOG_I(DLT_STRING("terminateFeature, feature name = "), DLT_STRING(strFeatureName.c_str()));
    }

    void NAECallProcessor::doRecoveringMSD() {
        LOG_I(DLT_STRING("Recover MSD"));
        mMSD = mECallRecovery.recoverMSD();
        if (mMSD.empty()) {
            LOG_W(DLT_STRING("No recovered MSD found"));
        }
    }

    void NAECallProcessor::clearMSD() {
        LOG_I(DLT_STRING("Clear MSD"));
        mMSD.clear();
        VehicleManagerAdapter::getInstance()->clearCanData();
    }

    void NAECallProcessor::doPreparingMSD(){
        LOG_I(DLT_STRING("doPreparingMSD"));
        const int32_t igState = PowerManagerAdapter::getInstance()->getIGStatus();
        const int32_t accState = PowerManagerAdapter::getInstance()->getACCStatus();
        const std::string ignition = DataBuilderUS::getIgState(igState, accState);
        HMIManagerAdapter::getInstance()->requestLedPattern(LED_ECALL_MSD_SENDING, true); // led sending MSD

        if(AudioManagerAdapter::getInstance()->isPlaying()) {
            ECallVariableRepo::getInstance().setECallClearAudioState(ecall_audio_state::CLEAR_TONE_2);
            (void)AudioManagerAdapter::getInstance()->stopAudioFile();
        }
        else {
            (void)AudioManagerAdapter::getInstance()->playAudioFile(audio::TONE_2); // sending MSD sound tone
        }

        if (mMSD.empty()) {
            LOG_I(DLT_STRING("Build a new MSD"));
            DataBuilderUS builder;
            auto& repo = ECallVariableRepo::getInstance();
            mMSD = builder.setCDS(true).setGPS().setUtil(ignition).setTriggerType(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], repo.getECallType() == ecall_type::DESS).build();
            bool errorOccur{false};
            CommonFunctions::saveTSPMSD(mMSD, errorOccur);
            if (errorOccur) {
                LOG_E(DLT_STRING("Failed to save MSD to TSP"));
            }
        }
        else {
            LOG_I(DLT_STRING("Use MSD from recovery"));
        }
    }

    void NAECallProcessor::doMSDTransmission() {
        LOG_I(DLT_STRING("doMSDTransmission"));
        mAttemptDataCount++;
        LOG_I(DLT_STRING("doMSDTransmission: the"), DLT_UINT(mAttemptDataCount), DLT_STRING("(st/nd) time"));

        if (ConfigurationManagerAdapter::getInstance()->getConfig(ecall_config::CF_DATA_TRANSMISSION_SKIP) == ecall_config::CONFIG_ENABLE) {
            doMSDSuccess();
        }
        else {
            if (ConfigurationManagerAdapter::getInstance()->getConfig(ecall_config::CF_SERVER_ULR_ENABLE_NAME) ==  ecall_config::CONFIG_ENABLE) {
                const std::string tUrl = ConfigurationManagerAdapter::getInstance()->getConfig(ecall_config::CF_SERVER_ULR_NAME);
                LOG_I(DLT_STRING("Test with server: "), DLT_STRING(tUrl.c_str()));
                HTTPSManagerAdapter::getInstance()->sendRequestAsync2(tUrl, mMSD);
            }
            else if (ConfigurationManagerAdapter::getInstance()->getConfig(ecall_config::CF_TEST_SENDING_FUNCTION_NAME) ==  ecall_config::CONFIG_ENABLE) {
                HTTPSManagerAdapter::getInstance()->sendRequestAsync1(TEST_CENTER_URI, mMSD);
            }
            else if (ConfigurationManagerAdapter::getInstance()->getConfig(ecall_config::CF_SERVER_REAL_ENABLE_NAME) ==  ecall_config::CONFIG_ENABLE) {
                HTTPSManagerAdapter::getInstance()->sendRequestAsync2(REAL_CENTER_URI, mMSD, true);
            }
            else {
                HTTPSManagerAdapter::getInstance()->sendRequestAsync2(TEST_CENTER_URI, mMSD);
            }
        }
    }

    // Only For Sldd test
    void NAECallProcessor::doMSDSuccess() {
        LOG_I(DLT_STRING("doMSDSuccess"));
        mAttemptVoiceCount = 0;
        if(AudioManagerAdapter::getInstance()->isPlaying()) {
            ECallVariableRepo::getInstance().setECallClearAudioState(ecall_audio_state::CLEAR_TONE_1);
            (void)AudioManagerAdapter::getInstance()->stopAudioFile();
        }
        else {
            (void)AudioManagerAdapter::getInstance()->playAudioFile(audio::TONE_1); // eCall dialling tone
        }

        HMIManagerAdapter::getInstance()->turnOffCurrentLed();
        HMIManagerAdapter::getInstance()->requestLedPattern(LED_ECALL_DIALING, true); // ecall dialling led
    }


    /**
     * @brief Executes the data retry strategy and determines whether a retry should be performed.
     *
     * This function evaluates the current state and conditions to decide if a data retry
     * should be attempted. The return value indicates the retry decision:
     * - true: Retry should be performed.
     * - false: Retry should not be performed.
     *
     * @return bool Indicates whether a retry should be performed.
     */
    bool NAECallProcessor::doDataRetryStrategy() {
        if (mAttemptDataCount >= mMaxAttemptData) {
            LOG_I(DLT_STRING("mAttemptDataCount = "), DLT_UINT(mAttemptDataCount), DLT_STRING(": No retry anymore"));
            return false;   // no retry
        }
        else {
            LOG_I(DLT_STRING("mAttemptDataCount =  "), DLT_UINT(mAttemptDataCount), DLT_STRING(": will retry"));
        }
        return true;
    }

    bool NAECallProcessor::doVoiceCallRetryStrategy() {
        LOG_I(DLT_STRING("doCallConnectingRetryStrategy,")
                    , DLT_STRING(" mAttemptVoiceCount: "), DLT_INT(mAttemptVoiceCount ));

        // Retry 3 time for SOS, unlimited time for ACN
        if ( (mMaxAttemptVoice < 0) || (mAttemptVoiceCount < mMaxAttemptVoice) ) {// mMaxAttemptVoice < 0 means retry unlimited time
            LOG_I(DLT_STRING("doCallConnectingRetryStrategy : retry"));
            return true;
        }
        else {
            LOG_I(DLT_STRING("doCallConnectingRetryStrategy : exit retry"));
            return false;
        }
    }

    void NAECallProcessor::doCallDialling() {
        LOG_I(DLT_STRING("doCallDialling"));
        mAttemptVoiceCount++;
        if (ConfigurationManagerAdapter::getInstance()->getConfig(ecall_config::CF_PHONE_NUMBER_ENABLE_NAME) == ecall_config::CONFIG_ENABLE) {
            TelephoneManagerAdapter::getInstance()->dial(
                ConfigurationManagerAdapter::getInstance()->getConfig(ecall_config::CF_PHONE_NUMBER_NAME)
                , true);
        } else {
            std::string phoneNumber = HTTPSManagerAdapter::getInstance()->getTelNo();
            LOG_D(DLT_STRING("Get phoneNumber from response data:"), DLT_STRING(phoneNumber.c_str()));
            if (phoneNumber.empty()) {
                phoneNumber = ConfigurationManagerAdapter::getInstance()->getConfig(ecall_config::CF_PHONE_NUMBER_NAME);
            }
            SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::TEL_NO, phoneNumber);
            TelephoneManagerAdapter::getInstance()->dial(phoneNumber, true);
        }
    }

    void NAECallProcessor::setCallBackWaitingTimer() {
        LOG_I(DLT_STRING("setCallBackWaitingTimer"));

        AlarmManagerAdapter::getInstance()->cancel(ECALL_CALLBACK_WAITING_ALARM_ID);
        int64_t CBWTCountdown{-1};
        int64_t recoverCBWTTime = mECallRecovery.getRemainingTime(ecalltimer::CBWT);
        if (recoverCBWTTime != -1) { // Verify if the recovery process is running
            CBWTCountdown = recoverCBWTTime;
        } else {
            if (ConfigurationManagerAdapter::getInstance()->getConfig(ecall_config::CF_CBWT_TIME_SHORTEN_ENABLE_NAME) == ecall_config::CONFIG_ENABLE) {
                CBWTCountdown = static_cast<int64_t>(ConfigurationManagerAdapter::getInstance()->getConfigIntValue(ecall_config::CF_CBWT_TIME_NAME));
            } else {
                CBWTCountdown = static_cast<int64_t>(ecall_na_timer::CALLBACK_WAITING_TIMER_DURATION);
            }
            // Save the timestamp and countdown timer only during the initial run
            int64_t currentTimestamp = TimeManagerAdapter::getCurrentTimestamp();
            SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::CBWT_START_TIME, std::to_string(currentTimestamp));
            SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::CBWT_COUNTDOWN, std::to_string(CBWTCountdown));
            LOG_I(DLT_STRING("setCallBackWaitingTimer, currentTimestamp:"), DLT_INT64(currentTimestamp), DLT_STRING("CBWTCountdown:"), DLT_INT64(CBWTCountdown));
        }
        if (CBWTCountdown > 0) {
            AlarmManagerAdapter::getInstance()->setAlarmFromNow(CBWTCountdown, ECALL_CALLBACK_WAITING_ALARM_ID);
            LOG_I(DLT_STRING("setCallBackWaitingTimer, setAlarmFromNow()"), DLT_STRING(" CBWTCountdown:"), DLT_INT64(CBWTCountdown));
        }
        else {
            ECallAppHandler* tHandler = ECallAppHandler::getInstance();
            if (tHandler != nullptr) {
                tHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CBWT_TIMER_EXPIRED))->sendToTarget();
            }
        }
    }

    void NAECallProcessor::transitsToCBWTMode() {
        LOG_I(DLT_STRING("transitsToCBWTMode"));

        auto& repo = ECallVariableRepo::getInstance();
        (void)ApplicationManagerAdapter::getInstance()->queryActionForFeature(ecall_feature::appfeature[ecall_feature::CALLBACK_WAITING][repo.getECallType()]);
        (void)ApplicationManagerAdapter::getInstance()->setFeatureStatus(ecall_feature::appfeature[ecall_feature::CALLBACK_WAITING][repo.getECallType()], true);

        (void)AudioManagerAdapter::getInstance()->stopAudioFile();
        AudioManagerAdapter::getInstance()->closeCallAudioPath();
        // mTerminateAfterCall = false;
        (void)AudioManagerAdapter::getInstance()->setMute(false);
        PowerManagerAdapter::getInstance()->releasePowerLock();
    }

    void NAECallProcessor::doECallTermination() {
        LOG_I(DLT_STRING("doECallTermination"));
        HMIManagerAdapter::getInstance()->turnOffCurrentLed();

        AudioManagerAdapter::getInstance()->closeCallAudioPath();
        (void)AudioManagerAdapter::getInstance()->stopAudioFile();
        (void)AudioManagerAdapter::getInstance()->setMute(false);

        (void)CommonFunctions::removeFromNVM(ecall_file::SOS_FILE);
        (void)CommonFunctions::removeFromNVM(ecall_file::ACN_FILE);
        (void)CommonFunctions::removeFromNVM(ecall_file::DESS_FILE);

        PowerManagerAdapter::getInstance()->releasePowerLock();
        (void)PowerManagerAdapter::getInstance()->requestBUBModeACNRunning(false);
        (void)PowerManagerAdapter::getInstance()->requestBUBModeECallRunning(false);
        auto& repo = ECallVariableRepo::getInstance();
        if (repo.getECallType() == ecall_type::ACN) {
            ApplicationManagerAdapter::getInstance()->sendPostACNUS_ON_OFF(ecallapp::ACNUS_OFF);
        }

        mCallRetryTimer->stop();
        mCallDiallingTimer->stop();
        AlarmManagerAdapter::getInstance()->cancel(ECALL_CALLBACK_WAITING_ALARM_ID);
        mECallRecovery.resetCBWTFlags();
    }

    void NAECallProcessor::playLedPatternInMs(const int32_t ledPartten, const int32_t timeMs) {
        LOG_I(DLT_STRING("playLedPatternInMs: "), DLT_INT(timeMs));
        HMIManagerAdapter::getInstance()->requestLedPattern(ledPartten, true);
        ECallAppHandler::getInstance()->sendMessageDelayed(
            ECallAppHandler::getInstance()->obtainMessage(
                ENUM_TO_INT(msg::MSG::MSG_TURN_OFF_LED),
                ENUM_TO_INT(ledPartten)
            ), timeMs
        );
    }

    void NAECallProcessor::doHandlePowerStateChanged() {
        const int32_t tCurrentStatus = mStateMachine->getCurrentState();
        if( (tCurrentStatus == ecall_state::STANDBY) || (tCurrentStatus == ecall_state::POWER_OFF) ) {
            if (PowerManagerAdapter::getInstance()->isIgnitionOn()) {
                HMIManagerAdapter::getInstance()->requestLedPattern(ecallapp::LED_ECAL_IDLE, true);
            }
            else {
                HMIManagerAdapter::getInstance()->turnOffCurrentLed();
            }
        }
    }

    /**
     * processorHandleMessage:
     * Handle the message extracted from queue.
     */
    void NAECallProcessor::processorHandleMessage(const uint32_t& what, const android::sp<sl::Message>& msgData) {
        LOG_I(DLT_STRING("processorHandleMessage: "), DLT_UINT(what));

        if (msgData == nullptr) {
            LOG_I(DLT_STRING("msgData is null, skipping message handling for what = "), DLT_UINT32(what));
            return;
        }

        switch (what) {
            case ENUM_TO_UINT32(msg::MSG::RECV_MSG_BOOT_COMPLETED):
            {
                LOG_I(DLT_STRING("RECV_MSG_BOOT_COMPLETED"));
                ECallAppHandler::getInstance()->handleMessageInternal(ECallAppHandler::getInstance()->obtainMessage(ENUM_TO_INT(msg::AppMSG::TSU_TO_RVU_ECALL_STATUS_NOTIFICATION_NO_REQUEST)));
            }
            break;

            case ENUM_TO_UINT32(msg::MSG::MSG_POWER_STATE_CHANGED):
            {
                auto& repo = ECallVariableRepo::getInstance();
                LOG_I(DLT_STRING("MSG_POWER_STATE_CHANGED"));
                repo.updateIgAccCondition();
            }
            break;

            // case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH) : {
            //     LOG_I(DLT_STRING("RECV_MSG_CDS_HEAVY_CRASH"));
            //     CommunicationManagerAdapter::getInstance()->requestSendACKToMCU(msgData->arg1, msgData->arg2);
            // }
            // break;

            case ENUM_TO_UINT32(msg::MSG::MSG_TURN_OFF_LED):
            {
                LOG_I(DLT_STRING("MSG_TURN_OFF_LED after time out "), DLT_INT(msgData->arg1));
                HMIManagerAdapter::getInstance()->requestLedPattern(msgData->arg1, false);
            }
            break;

            case ENUM_TO_UINT32(msg::MSG::MESS_RESET_ECALL_SLDD):
            {
                LOG_I(DLT_STRING("MESS_RESET_ECALL_SLDD: "));
                doResetECall();
            }
            break;

            case ENUM_TO_UINT32(msg::MSG::MESS_TEST_RECOVER_ECALL_SLDD):
            {
                LOG_I(DLT_STRING("MESS_TEST_RECOVER_ECALL_SLDD: "), DLT_UINT(msgData->arg1));
                mECallRecovery.doSetConditionToTestRecoverECall(msgData->arg1);
            }
            break;

            case ENUM_TO_UINT32(msg::MSG::RECV_MSG_ON_TIME_RECEIVED):
            {
                // Determine if the remaining timer needs to be recalculated after recovery
                LOG_I(DLT_STRING("RECV_MSG_ON_TIME_RECEIVED in state"), DLT_STRING(ecall_state::ECallStateMap.at(mStateMachine->getCurrentState()).getName()));
                if (mECallRecovery.getCBWTFlag()) {
                    doOnTimeReceived();
                }
            }
            break;

            case ENUM_TO_INT(msg::MSG::RECV_MSG_HMI_BUTTON_LONG_PRESS):
                LOG_I(DLT_STRING("RECV_MSG_HMI_BUTTON_LONG_PRESS from HMI Manager "), DLT_UINT(what));
                HMIManagerAdapter::getInstance()->sendButtonEventACKToMCU(msgData->arg1, msgData->arg2);
            break;

            case ENUM_TO_INT(msg::MSG::APPM_REQUEST_LAUNCH):
            {
                android::sp<AppMgrEventData> pAppMgrEventData {nullptr};
                msgData->getObject(pAppMgrEventData);
                if (pAppMgrEventData != nullptr) {
                    if (getQueryFeatureName() != pAppMgrEventData->mFeatureName) {
                        LOG_I(DLT_STRING("APPM_REQUEST_LAUNCH, Feature name mismatch, ignore it, query:"), DLT_STRING(getQueryFeatureName().c_str()),
                            DLT_STRING(" ,received:"), DLT_STRING(pAppMgrEventData->mFeatureName.c_str()));
                        return;
                    }
                }
            }
            break;

            default:
            break;
        }

        mStateMachine->handleMessage(what, msgData->arg1, msgData->arg2);
    }

    void NAECallProcessor::onTimerTimeoutEvent(const int32_t& msgId) {
        LOG_I(DLT_STRING("onTimerTimeoutEvent "), DLT_INT(msgId));
    }

    void NAECallProcessor::doResetECall() {
        LOG_I(DLT_STRING("doResetECall"));
#ifndef __UNITTEST__
        const int32_t tCurrentStatus = mStateMachine->getCurrentState();
#endif
        // Force reset without concerning ecall state
        LOG_I( DLT_STRING("Reset state "), DLT_STRING(ecall_state::ECallStateMap.at(tCurrentStatus).getName()), DLT_STRING(" to STANDBY!"));
        //cancel timer
        mCancelingTimer->stop();
        mCallRetryTimer->stop();
        mCallDiallingTimer->stop();
        AlarmManagerAdapter::getInstance()->cancel(ECALL_CALLBACK_WAITING_ALARM_ID);
        mECallRecovery.resetCBWTFlags();

        if(TelephoneManagerAdapter::getInstance()->getCallState() != telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE){
            TelephoneManagerAdapter::getInstance()->endCall();
        }

        auto& repo = ECallVariableRepo::getInstance();
        if (repo.getECallType() != ecall_type::UNDEFINED) {
            (void)ApplicationManagerAdapter::getInstance()->setFeatureStatus(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], false);
            (void)ApplicationManagerAdapter::getInstance()->setFeatureStatus(ecall_feature::appfeature[ecall_feature::CALLBACK_WAITING][repo.getECallType()], false);
            (void)ApplicationManagerAdapter::getInstance()->setFeatureStatus(ecall_feature::appfeature[ecall_feature::INCOMING_CALL][repo.getECallType()], false);
        }
        HMIManagerAdapter::getInstance()->turnOffCurrentLed();
        AudioManagerAdapter::getInstance()->closeCallAudioPath();
        (void)AudioManagerAdapter::getInstance()->stopAudioFile();
        (void)AudioManagerAdapter::getInstance()->setMute(false);

        (void)CommonFunctions::removeFromNVM(ecall_file::SOS_FILE);
        (void)CommonFunctions::removeFromNVM(ecall_file::ACN_FILE);
        (void)CommonFunctions::removeFromNVM(ecall_file::DESS_FILE);

        if (PowerManagerAdapter::getInstance()->isIgnitionOn()) {
            mStateMachine->changeState(ecall_state::STANDBY);
            HMIManagerAdapter::getInstance()->requestLedPattern(ecallapp::LED_ECAL_IDLE, true);
        }
        else {
            mStateMachine->changeState(ecall_state::POWER_OFF);
        }
        PowerManagerAdapter::getInstance()->releasePowerLock();
        (void)PowerManagerAdapter::getInstance()->requestBUBModeACNRunning(false);
        ApplicationManagerAdapter::getInstance()->sendPostACNUS_ON_OFF(ecallapp::ACNUS_OFF); // Reset Post ACN US ON OFF
    }

    void NAECallProcessor::doOnTimeReceived() {
        LOG_I(DLT_STRING("doOnTimeReceived"));
        const int32_t tCurrentStatus = mStateMachine->getCurrentState();
        if (tCurrentStatus == ecall_state::CALLBACK_WAITING || tCurrentStatus == ecall_state::CALLBACK_RUNNING) {
            mECallRecovery.calculateT9RemainingTime(ecalltimer::CBWT);
            int64_t tCBWTCountdown = mECallRecovery.getRemainingTime(ecalltimer::CBWT);
            if (tCBWTCountdown < 0) {
                LOG_D(DLT_STRING("This countdown hasn't been calculated yet"));
            } else if (tCBWTCountdown == 0) {
                ECallAppHandler* tHandler = ECallAppHandler::getInstance();
                if (tHandler != nullptr) {
                    tHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CBWT_TIMER_EXPIRED))->sendToTarget();
                }
            } else {
                AlarmManagerAdapter::getInstance()->cancel(ECALL_CALLBACK_WAITING_ALARM_ID);
                AlarmManagerAdapter::getInstance()->setAlarmFromNow(tCBWTCountdown, ECALL_CALLBACK_WAITING_ALARM_ID);
            }
        } else {
            LOG_D(DLT_STRING("Receive time while status is"), DLT_STRING(ecall_state::ECallStateMap.at(tCurrentStatus).getName()));
            mECallRecovery.resetCBWTFlags();
        }
    }
}
