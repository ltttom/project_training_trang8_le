
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
 * @file        NAECallStateMachine.h
 * @author      eunbi.lee@lge.com
 * @version     1.0.00
*/
#ifndef FLAG_NA_ECALL_STATE_MACHINE
#define FLAG_NA_ECALL_STATE_MACHINE
#include "ECallState.h"
#include "NAECallStates.h"
#include "../../../core/utils/CommonFunctions.h"
#include <memory>
#include <mutex>
#include "InternalParamsDef.h"

namespace ecallapp {
// Forward declaration
class NAECallProcessor;
class ECallStateBase;
class BootupState;
class PowerOffState;
class StandbyState;
class CancelPeriodState;
class MsdPreparingState;
class MsdTransmissionState;
class ClearCurrentCallState;
class CallConnectingState;
class CallConnectedState;
class CallbackWaitingState;
class CallbackRunningState;
/**
 * @brief Moore Machine implementation for eCall state handling
 *
 * This class implements a Moore Machine pattern where outputs depend only on the current state.
 * Each state has its own handler method that processes messages based on the current state.
 */
class NAECallStateMachine {
public:
    /**
     * @brief Constructor
     *
     * @param processor Reference to the NAECallProcessor that owns this state machine
     */
    NAECallStateMachine(NAECallProcessor* processor);
    /**
     * @brief Destructor
     */
    ~NAECallStateMachine();
    /**
     * @brief Handle a message based on the current state
     *
     * @param what Message type
     * @param eventParam1 First event parameter
     * @param eventParam2 Second event parameter
     */
    void handleMessage(const uint32_t& what, const uint32_t eventParam1, const uint32_t eventParam2);
    /**
     * @brief Set the current state
     *
     * @param state New state to set
     */
    void changeState(const int32_t state);
    /**
     * @brief Get the current state
     *
     * @return Current state
     */
    int32_t getCurrentState() const;

    // Helper function for handling audio stopped state
    void processAudioStopped(const int32_t audioState);
    // Wrapper functions for adapter calls
    void acquirePowerLock();
    void requestLedPatternWithState(const uint32_t ledPattern, bool state);
    void turnOffCurrentLed();
    bool isAudioPlaying();
    void playAudioFile(const std::string& audioTone);
    void stopAudioFile();
    void openCallAudioPath();
    void setAudioMute(bool muteState);
    int32_t getDataServiceState();
    bool clearCurrentCall();
    void stopClearingCurrentCall();
    std::string getConfigValue(const std::string& configName);
    void queryActionForFeature(const std::string& featureName, bool state);
    void sendBCANCancelDisplay(const uint32_t value);
    void setFeatureStatus(const std::string& name, const bool& OnOff);
    void answerRingingCall();
    // Wrapper functions for mProcessor calls
    void startCancellingTimer();
    void stopCancellingTimer();
    void startCallDiallingTimer();
    void stopCallDiallingTimer();
    void startCallRetryTimer();
    void stopCallRetryTimer();

    void performECallTermination();
    void prepareMSD();
    void performMSDTransmission();
    void transitionToCBWTMode();
    void setAttemptVoiceCount(int32_t count);
    int32_t getAttemptVoiceCount();
    int32_t getMAXAttemptVoice();
    bool shouldRetryDataTransmission();
    bool shouldRetryVoiceCall();
    bool IsSLDDTest();
    bool IsSLDDTest_DataTransmitSUCCESS();
    bool IsSLDDTest_DataTransmitFAIL();
    bool confirmSignalReceived(const int32_t eCallType);
    void clearSignalReceived();
    // bool saveToNVM(const std::string& fileName);
    // bool removeFromNVM(const std::string& fileName);
    void performCallDialling();
    int32_t getDisconnectCause();
    bool isCallDisconnectedNormal();
    // Convenience functions
    void requestLedEcallReady();
    void checkAndEndCall();
    void performsetCallBackWaitingTimer();
    void requestBUBModeACNRunning();
    void requestBUBModeECallRunning();
    void performActionsAtLaunchingTime(bool isRecover = false);
    void playLedPatternInMs(const int32_t ledPartten, const int32_t timeMs);
    void collectIntialData();
    void saveECallState(const int32_t state);
    bool doECallRecovery();
    void calculateT9RemainingTime(std::string timeType);
    int64_t getRemainingTime(std::string timeType);
    void doRecoveringAtSignalDetectingTimePartOfMSD();
    void doRecoveringMSD();
    void performClearMSD();
    void doRecoveringTelNo();
    void doNotifySOSRequest();
    void doNotifySOSNoRequest();
    void doNotifySOSCancel();
    void sendSOSAck(int32_t arg1, int32_t arg2);
    void sendCDSAck(int32_t arg1, int32_t arg2);
    void doSignalAckOnActiveCall(int32_t event, int32_t arg1, int32_t arg2);
    void setAttemptDataCount(int32_t count);
    int32_t getAttemptDataCount();
    int32_t getMAXAttemptData();
    void sendPostACNUS_ON_OFF(int32_t onOff);
    void doResetCBWTFlags();
private:
    int mCurrentStateId;
    ECallStateBase* mCurrentState;
    // Reference to the processor that owns this state machine
    NAECallProcessor* mProcessor;

    // State instances - changed from unique_ptr to direct instances
    BootupState mBootupState;
    RecoveryState mRecoveryState;
    PowerOffState mPowerOffState;
    StandbyState mStandbyState;
    CancelPeriodState mCancelPeriodState;
    MsdPreparingState mMsdPreparingState;
    MsdTransmissionState mMsdTransmissionState;
    ClearCurrentCallState mClearCurrentCallState;
    CallConnectingState mCallConnectingState;
    CallConnectedState mCallConnectedState;
    CallbackWaitingState mCallbackWaitingState;
    CallbackRunningState mCallbackRunningState;
    // Helper method to get state object from state ID
    ECallStateBase* getStateFromId(int32_t stateId);
    mutable std::recursive_mutex stateMutex;
    };
} // namespace ecallapp
#endif // FLAG_NA_ECALL_STATE_MACHINE
