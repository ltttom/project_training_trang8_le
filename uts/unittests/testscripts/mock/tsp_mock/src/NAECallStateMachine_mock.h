#ifndef NAECALLSTATEMACHINE_MOCK_H
#define NAECALLSTATEMACHINE_MOCK_H

namespace ecallapp {

class MockNAECallStateMachine : public NAECallStateMachine {
public:
    MockNAECallStateMachine(NAECallProcessor* processor) : NAECallStateMachine(processor) {}
    virtual ~MockNAECallStateMachine() {}

    MOCK_METHOD3(handleMessage, void(const uint32_t& what, const uint32_t eventParam1, const uint32_t eventParam2));
    MOCK_METHOD1(changeState, void(const int32_t state));
    MOCK_METHOD0(getCurrentState, int32_t());
    MOCK_METHOD1(processAudioStopped, void(const int32_t audioState));
    MOCK_METHOD0(acquirePowerLock, void());
    MOCK_METHOD2(requestLedPatternWithState, void(const uint32_t ledPattern, bool state));
    MOCK_METHOD0(turnOffCurrentLed, void());
    MOCK_METHOD0(isAudioPlaying, bool());
    MOCK_METHOD1(playAudioFile, void(const std::string& audioTone));
    MOCK_METHOD0(stopAudioFile, void());
    MOCK_METHOD0(openCallAudioPath, void());
    MOCK_METHOD1(setAudioMute, void(bool muteState));
    MOCK_METHOD0(getDataServiceState, int32_t());
    MOCK_METHOD0(clearCurrentCall, bool());
    MOCK_METHOD1(getConfigValue, std::string(const std::string& configName));
    MOCK_METHOD2(queryActionForFeature, void(const std::string& featureName, bool state));
    MOCK_METHOD1(sendBCANCancelDisplay, void(const uint32_t value));
    MOCK_METHOD2(setFeatureStatus, void(const std::string& name, const bool& OnOff));
    MOCK_METHOD0(answerRingingCall, void());
    MOCK_METHOD0(startCancellingTimer, void());
    MOCK_METHOD0(stopCancellingTimer, void());
    MOCK_METHOD0(startCallDiallingTimer, void());
    MOCK_METHOD0(stopCallDiallingTimer, void());
    MOCK_METHOD0(startCallRetryTimer, void());
    MOCK_METHOD0(stopCallRetryTimer, void());
    MOCK_METHOD0(performECallTermination, void());
    MOCK_METHOD0(prepareMSD, void());
    MOCK_METHOD0(performMSDTransmission, void());
    MOCK_METHOD0(transitionToCBWTMode, void());
    MOCK_METHOD1(setAttemptVoiceCount, void(int32_t count));
    MOCK_METHOD0(getAttemptVoiceCount, int32_t());
    MOCK_METHOD0(getMAXAttemptVoice, int32_t());
    MOCK_METHOD0(shouldRetryDataTransmission, bool());
    MOCK_METHOD0(shouldRetryVoiceCall, bool());
    MOCK_METHOD0(IsSLDDTest, bool());
    MOCK_METHOD0(IsSLDDTest_DataTransmitSUCCESS, bool());
    MOCK_METHOD0(IsSLDDTest_DataTransmitFAIL, bool());
    MOCK_METHOD1(runDataRetryTimer, void(bool run));
    MOCK_METHOD1(saveToNVM, bool(const std::string& fileName));
    MOCK_METHOD1(removeFromNVM, bool(const std::string& fileName));
    MOCK_METHOD0(performCallDialling, void());
    MOCK_METHOD0(getDisconnectCause, int32_t());
    MOCK_METHOD0(isCallDisconnectedNormal, bool());
    MOCK_METHOD0(requestLedEcallReady, void());
    MOCK_METHOD0(checkAndEndCall, void());
    MOCK_METHOD0(performsetCallBackWaitingTimer, void());
    MOCK_METHOD0(requestBUBModeACNRunning, void());
    MOCK_METHOD1(performActionsAtLaunchingTime, void(bool isRecover));
    MOCK_METHOD2(playLedPatternInMs, void(const int32_t ledPartten, const int32_t timeMs));
    MOCK_METHOD0(collectIntialData, void());
    MOCK_METHOD1(saveECallState, void(const int32_t state));
    MOCK_METHOD0(doECallRecovery, bool());
    MOCK_METHOD1(calculateT9RemainingTime, void(std::string timeType));
    MOCK_METHOD0(doRecoveringAtSignalDetectingTimePartOfMSD, void());
    MOCK_METHOD0(doRecoveringMSD, void());
    MOCK_METHOD0(doRecoveringTelNo, void());
    MOCK_METHOD0(doNotifySOSRequest, void());
    MOCK_METHOD0(doNotifySOSNoRequest, void());
    MOCK_METHOD0(doNotifySOSCancel, void());
    MOCK_METHOD1(setAttemptDataCount, void(int32_t count));
    MOCK_METHOD0(getAttemptDataCount, int32_t());
    MOCK_METHOD0(getMAXAttemptData, int32_t());
};

MockNAECallStateMachine* M_NAECallStateMachine;

NAECallStateMachine::NAECallStateMachine(NAECallProcessor* processor)
    : mCurrentStateId(ecall_state::BOOTUP), mProcessor(processor) {
    mCurrentState = &mBootupState;
}

NAECallStateMachine::~NAECallStateMachine() {
}

int32_t NAECallStateMachine::getCurrentState() const {
    return M_NAECallStateMachine->getCurrentState();
}
void NAECallStateMachine::changeState(const int32_t state) {
    M_NAECallStateMachine->changeState(state);
}
void NAECallStateMachine::handleMessage(const uint32_t& what, const uint32_t eventParam1, const uint32_t eventParam2) {
    M_NAECallStateMachine->handleMessage(what, eventParam1, eventParam2);
}
} // namespace ecallapp
#endif // NAECALLSTATEMACHINE_MOCK_H