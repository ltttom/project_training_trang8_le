
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
 * @file        NAECallStateMachine.cpp
 * @author      eunbi.lee@lge.com
 * @version     1.0.00
*/
#include "NAECallStateMachine.h"
#include "NAECallStates.h"
#include "NAECallProcessor.h"
#include "../../../core/utils/ParamsDef.h"
#include "../../../core/services/PowerManagerAdapter.h"
#include "../../../core/services/AudioManagerAdapter.h"
#include "../../../core/services/TelephoneManagerAdapter.h"
#include "../../../core/services/ConfigurationManagerAdapter.h"
#include "../../../core/services/HMIManagerAdapter.h"
#include "../../../core/services/CommunicationManagerAdapter.h"
#include "../../../core/services/VehicleManagerAdapter.h"
#include "../../../core/services/ApplicationManagerAdapter.h"
#include "../../../core/services/SystemPropertyAdapter.h"
#include "ECallVariableRepo.h"
namespace ecallapp {
NAECallStateMachine::NAECallStateMachine(NAECallProcessor* processor)
    : mCurrentStateId(ecall_state::BOOTUP), mProcessor(processor) {
    mCurrentState = &mBootupState;
}

NAECallStateMachine::~NAECallStateMachine() {
}

void NAECallStateMachine::handleMessage(const uint32_t& what, const uint32_t eventParam1, const uint32_t eventParam2) {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (mCurrentState) {
        mCurrentState->process(this, what, eventParam1, eventParam2);
    } else {
        LOG_E(DLT_STRING("Invalid state object"));
    }
}

void NAECallStateMachine::changeState(const int32_t state) {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);

    if (mCurrentStateId == state) {
        LOG_I(DLT_STRING("State is already changed to the requested state"), DLT_INT(state));
        return;
    }

    if (mCurrentState) {
        mCurrentState->exit(this);
    }
    mCurrentStateId = state;
    mCurrentState = getStateFromId(mCurrentStateId);

    if (mCurrentState) {
        mCurrentState->enter(this);
        if (ecall_state::ECallStateMap[state].isRecoverCandidate()) {
            saveECallState(mCurrentStateId); //for recovery
        }
    }
    else {
        LOG_E(DLT_STRING("changeState: Failed to change new state, mCurrentState is null"));
    }
}

int32_t NAECallStateMachine::getCurrentState() const {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    return mCurrentStateId;
}

ECallStateBase* NAECallStateMachine::getStateFromId(const int32_t stateId) {
    ECallStateBase* resultState = nullptr;
    std::string stateName = "UNKNOWN";

    switch (stateId) {
        case ecall_state::BOOTUP:
            resultState = &mBootupState;
            stateName = "BOOTUP";
            break;
        case ecall_state::RECOVERY:
            resultState = &mRecoveryState;
            stateName = "RECOVERY";
            break;
        case ecall_state::POWER_OFF:
            resultState = &mPowerOffState;
            stateName = "POWER_OFF";
            break;
        case ecall_state::STANDBY:
            resultState = &mStandbyState;
            stateName = "STANDBY";
            break;
        case ecall_state::CANCEL_PERIOD:
            resultState = &mCancelPeriodState;
            stateName = "CANCEL_PERIOD";
            break;
        case ecall_state::MSD_PREPARING:
            resultState = &mMsdPreparingState;
            stateName = "MSD_PREPARING";
            break;
        case ecall_state::MSD_TRANSMISSION:
            resultState = &mMsdTransmissionState;
            stateName = "MSD_TRANSMISSION";
            break;
        case ecall_state::CLEAR_CURRENT_CALL:
            resultState = &mClearCurrentCallState;
            stateName = "CLEAR_CURRENT_CALL";
            break;
        case ecall_state::CALL_CONNECTING:
            resultState = &mCallConnectingState;
            stateName = "CALL_CONNECTING";
            break;
        case ecall_state::CALL_CONNECTED:
            resultState = &mCallConnectedState;
            stateName = "CALL_CONNECTED";
            break;
        case ecall_state::CALLBACK_WAITING:
            resultState = &mCallbackWaitingState;
            stateName = "CALLBACK_WAITING";
            break;
        case ecall_state::CALLBACK_RUNNING:
            resultState = &mCallbackRunningState;
            stateName = "CALLBACK_RUNNING";
            break;
        default:
            LOG_E(DLT_STRING("Invalid state ID: "), DLT_INT(stateId));
            resultState = &mBootupState;
            stateName = "BOOTUP";
            break;
    }

    LOG_I(DLT_STRING("getStateFromId - Mapped state ID "), DLT_INT(stateId), DLT_STRING(" to state name: "), DLT_STRING(stateName.c_str()));

    return resultState;
}
void NAECallStateMachine::acquirePowerLock() {
    PowerManagerAdapter::getInstance()->acquirePowerLock();
}
void NAECallStateMachine::requestLedEcallReady() {
    HMIManagerAdapter::getInstance()->requestLedPattern(ecallapp::LED_ECAL_IDLE, true);
}
void NAECallStateMachine::requestLedPatternWithState(const uint32_t ledPattern, bool state) {
    HMIManagerAdapter::getInstance()->requestLedPattern(ledPattern, state);
}
void NAECallStateMachine::turnOffCurrentLed() {
    HMIManagerAdapter::getInstance()->turnOffCurrentLed();
}
bool NAECallStateMachine::isAudioPlaying() {
    return AudioManagerAdapter::getInstance()->isPlaying();
}
void NAECallStateMachine::playAudioFile(const std::string& audioTone) {
    AudioManagerAdapter::getInstance()->playAudioFile(audioTone);
}
void NAECallStateMachine::stopAudioFile() {
    AudioManagerAdapter::getInstance()->stopAudioFile();
}
void NAECallStateMachine::openCallAudioPath() {
    AudioManagerAdapter::getInstance()->openCallAudioPath();
}
void NAECallStateMachine::setAudioMute(bool muteState) {
    AudioManagerAdapter::getInstance()->setMute(muteState);
}
int32_t NAECallStateMachine::getDataServiceState() {
    return TelephoneManagerAdapter::getInstance()->getDataServiceState();
}
bool NAECallStateMachine::clearCurrentCall() {
    return TelephoneManagerAdapter::getInstance()->clearingCurrentCall();
}
void NAECallStateMachine::stopClearingCurrentCall() {
    TelephoneManagerAdapter::getInstance()->stopClearingCurrentCall();
}
std::string NAECallStateMachine::getConfigValue(const std::string& configName) {
    return ConfigurationManagerAdapter::getInstance()->getConfig(configName);
}
void NAECallStateMachine::queryActionForFeature(const std::string& featureName, bool state) {
    mProcessor->setQueryFeatureName(featureName);
    ApplicationManagerAdapter::getInstance()->queryActionForFeature(featureName, state);
}
void NAECallStateMachine::sendBCANCancelDisplay(const uint32_t value) {
    VehicleManagerAdapter::getInstance()->sendBCAN_CANCEL_DISP(value);
}
int32_t NAECallStateMachine::getDisconnectCause() {
    return TelephoneManagerAdapter::getInstance()->getDisconnectCause();
}
bool NAECallStateMachine::isCallDisconnectedNormal() {
    return  TelephoneManagerAdapter::getInstance()->isCallDisconnectedNormal();
}
void NAECallStateMachine::setFeatureStatus(const std::string& name, const bool& OnOff) {
    ApplicationManagerAdapter::getInstance()->setFeatureStatus(name, OnOff);
}
void NAECallStateMachine::answerRingingCall() {
    TelephoneManagerAdapter::getInstance()->answerRingingCall();
}
void NAECallStateMachine::requestBUBModeACNRunning() {
    PowerManagerAdapter::getInstance()->requestBUBModeACNRunning(true);
}
void NAECallStateMachine::requestBUBModeECallRunning() {
    PowerManagerAdapter::getInstance()->requestBUBModeECallRunning(true);
}
void NAECallStateMachine::collectIntialData() {
    VehicleManagerAdapter::getInstance()->collectIntialData();
}
void NAECallStateMachine::saveECallState(const int32_t state) {
    SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(state));
}

void NAECallStateMachine::processAudioStopped(const int32_t audioState) {
    LOG_I(DLT_STRING("processAudioStopped, audioState = "), DLT_INT(audioState));
    switch (audioState) {
    case ecall_audio_state::CLEAR_TONE_1:
        LOG_I(DLT_STRING("Clear current audio, open tone 1"));
        playAudioFile(audio::TONE_1);
        break;
    case ecall_audio_state::CLEAR_TONE_2:
        LOG_I(DLT_STRING("Clear current audio, open tone 2"));
        playAudioFile(audio::TONE_2);
        break;
    case ecall_audio_state::CLEAR_TONE_3:
        LOG_I(DLT_STRING("Clear current audio, open tone 3"));
        playAudioFile(audio::TONE_3);
        break;
    default:
        LOG_I(DLT_STRING("Clear current audio"));
        break;
    }
}

bool NAECallStateMachine::doECallRecovery() {
    return mProcessor->mECallRecovery.doECallRecovery();
}

void NAECallStateMachine::calculateT9RemainingTime(std::string timeType) {
    mProcessor->mECallRecovery.calculateT9RemainingTime(timeType);
}

int64_t NAECallStateMachine::getRemainingTime(std::string timeType) {
    return mProcessor->mECallRecovery.getRemainingTime(timeType);
}

void NAECallStateMachine::doRecoveringAtSignalDetectingTimePartOfMSD() {
    mProcessor->mECallRecovery.doRecoveringAtSignalDetectingTimePartOfMSD();
}

void NAECallStateMachine::doResetCBWTFlags() {
    mProcessor->mECallRecovery.resetCBWTFlags();
}

void NAECallStateMachine::doRecoveringMSD() {
    mProcessor->doRecoveringMSD();
}

void NAECallStateMachine::performClearMSD() {
    mProcessor->clearMSD();
}

void NAECallStateMachine::doRecoveringTelNo() {
    mProcessor->mECallRecovery.doRecoveringTelNo();
}

void NAECallStateMachine::startCancellingTimer() {
    mProcessor->mCancelingTimer->start();
}
void NAECallStateMachine::stopCancellingTimer() {
    mProcessor->mCancelingTimer->stop();
}
void NAECallStateMachine::startCallDiallingTimer() {
    mProcessor->mCallDiallingTimer->start();
}
void NAECallStateMachine::stopCallDiallingTimer() {
    mProcessor->mCallDiallingTimer->stop();
}
void NAECallStateMachine::startCallRetryTimer() {
    mProcessor->mCallRetryTimer->start();
}
void NAECallStateMachine::stopCallRetryTimer() {
    mProcessor->mCallRetryTimer->stop();
}
void NAECallStateMachine::performECallTermination() {
    mProcessor->doECallTermination();
}

void NAECallStateMachine::prepareMSD() {
    mProcessor->doPreparingMSD();
}
void NAECallStateMachine::performMSDTransmission() {
    mProcessor->doMSDTransmission();
}
void NAECallStateMachine::transitionToCBWTMode() {
    mProcessor->transitsToCBWTMode();
}
bool NAECallStateMachine::shouldRetryVoiceCall() {
    return mProcessor->doVoiceCallRetryStrategy();
}
bool NAECallStateMachine::shouldRetryDataTransmission () {
    return mProcessor->doDataRetryStrategy();
}
void NAECallStateMachine::setAttemptVoiceCount(int32_t count) {
    mProcessor->mAttemptVoiceCount = count;
}
int32_t NAECallStateMachine::getAttemptVoiceCount() {
    return mProcessor->mAttemptVoiceCount;
}
int32_t NAECallStateMachine::getMAXAttemptVoice() {
    return mProcessor->mMaxAttemptVoice;
}
void NAECallStateMachine::setAttemptDataCount(int32_t count) {
    mProcessor->mAttemptDataCount = count;
}
int32_t NAECallStateMachine::getAttemptDataCount() {
    return mProcessor->mAttemptDataCount;
}
int32_t NAECallStateMachine::getMAXAttemptData() {
    return mProcessor->mMaxAttemptData;
}
bool NAECallStateMachine::IsSLDDTest() {
    return (getConfigValue(ecall_config::CF_DATA_TRANSMISSION_ENABLE_NAME) == ecall_config::CONFIG_ENABLE);
}
bool NAECallStateMachine::IsSLDDTest_DataTransmitSUCCESS() {
    return (getConfigValue(ecall_config::CF_DATA_TRANSMISSION_NAME) == ecall_config::CF_CONFIG_VALUE_SUCCESS);
}
bool NAECallStateMachine::IsSLDDTest_DataTransmitFAIL() {
    return (getConfigValue(ecall_config::CF_DATA_TRANSMISSION_NAME) == ecall_config::CF_CONFIG_VALUE_FAILURE);
}

bool NAECallStateMachine::confirmSignalReceived(const int32_t eCallType) {
    bool ret = false;
    if (eCallType == ecall_type::ACN) {
        error_t err = CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::ACN_FILE);
        CommonFunctions::removeFromNVM(ecall_file::SOS_FILE);
        CommonFunctions::removeFromNVM(ecall_file::DESS_FILE);
        ret = (err == E_OK) ? true : false;
    }
    else if (eCallType == ecall_type::DESS) {
        error_t err = CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::DESS_FILE);
        CommonFunctions::removeFromNVM(ecall_file::ACN_FILE);
        CommonFunctions::removeFromNVM(ecall_file::SOS_FILE);
        ret = (err == E_OK) ? true : false;
    }
    else if (eCallType == ecall_type::SOS) {
        error_t err = CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::SOS_FILE);
        CommonFunctions::removeFromNVM(ecall_file::ACN_FILE);
        CommonFunctions::removeFromNVM(ecall_file::DESS_FILE);
        ret = (err == E_OK) ? true : false;
    }
    else {
        ret = false;
    }
    return ret;
}
void NAECallStateMachine::clearSignalReceived() {
    CommonFunctions::removeFromNVM(ecall_file::ACN_FILE);
    CommonFunctions::removeFromNVM(ecall_file::DESS_FILE);
    CommonFunctions::removeFromNVM(ecall_file::SOS_FILE);
}
void NAECallStateMachine::performCallDialling() {
    mProcessor->doCallDialling();
}
void NAECallStateMachine::checkAndEndCall() {
    const int32_t tCurrTelephoneState = TelephoneManagerAdapter::getInstance()->getCallState();
    if ((tCurrTelephoneState != telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE) &&
        (tCurrTelephoneState != telephony::TelephonyManager::PRECISE_CALL_STATE_INCOMING)) {
        TelephoneManagerAdapter::getInstance()->endCall();
    }
}
void NAECallStateMachine::performsetCallBackWaitingTimer() {
    mProcessor->setCallBackWaitingTimer();
}
void NAECallStateMachine::performActionsAtLaunchingTime(bool isRecover) {
    mProcessor->doActionsAtLaunchingTime(isRecover);
}
void NAECallStateMachine::playLedPatternInMs(const int32_t ledPartten, const int32_t timeMs) {
    mProcessor->playLedPatternInMs(ledPartten, timeMs);
}

void NAECallStateMachine::doNotifySOSRequest() {
    mProcessor->doNotifySOSRequest();
}
void NAECallStateMachine::doNotifySOSNoRequest() {
    mProcessor->doNotifySOSNoRequest();
}
void NAECallStateMachine::doNotifySOSCancel() {
    mProcessor->doNotifySOSCancel();
}
void NAECallStateMachine::sendSOSAck(int32_t arg1, int32_t arg2) {
    mProcessor->sendSOSAck(arg1, arg2);
}
void NAECallStateMachine::sendCDSAck(int32_t arg1, int32_t arg2) {
    mProcessor->sendCDSAck(arg1, arg2);
}
void NAECallStateMachine::doSignalAckOnActiveCall(int32_t event, int32_t arg1, int32_t arg2) {
    mProcessor->doSignalAckOnActiveCall(event, arg1, arg2);
}
void NAECallStateMachine::sendPostACNUS_ON_OFF(int32_t onOff) {
    ApplicationManagerAdapter::getInstance()->sendPostACNUS_ON_OFF(onOff);
}

} // namespace ecallapp
