
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
 * @file        NAECallProcessor.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef FLAG_NA_ECALL_PROCESSOR
#define FLAG_NA_ECALL_PROCESSOR
#include "ECallState.h"
#include "../../../core/processors/IProcessorBase.h"
#include "../../../core/utils/ECallTimerHandler.h"
#include "../../../core/utils/CommonFunctions.h"
#include "../../../core/utils/ECallAppHandler.h"
#include "InternalParamsDef.h"
#include "NAECallRecovery.h"
#include "NAECallStateMachine.h"

namespace ecallapp {

class NAECallProcessor : public IProcessorBase {
public:
    NAECallProcessor();
    virtual void processorHandleMessage(const uint32_t& what, const android::sp<sl::Message>& msgData) override;
    virtual void launchFeature(const std::string strFeatureName) override;
    virtual void ignoreFeature(const std::string strFeatureName) override;
    virtual void terminateFeature(const std::string strFeatureName) override;
    virtual void initialize() override;
    virtual void onTimerTimeoutEvent(const int32_t& msgId) override;

private:
    void launchRecoveredECall(std::string strFeatureName);
    void doMSDSuccess();
    void doHandlePowerStateChanged();
    void doResetECall();
    void doOnTimeReceived();

    NAECallStateMachine* mStateMachine;
    std::string mMSD;
    std::string mQueryFeatureName;

public:
    void doActionsAtLaunchingTime(bool isRecover);
    void doECallTermination();
    void doPreparingMSD();
    void doMSDTransmission();
    bool doDataRetryStrategy();
    void doCallDialling();
    void transitsToCBWTMode();
    void setCallBackWaitingTimer();
    bool doVoiceCallRetryStrategy();
    void playLedPatternInMs(const int32_t ledPartten, const int32_t timeMs);
    void doNotifySOSCancel();
    void doNotifySOSRequest();
    void doNotifySOSNoRequest();
    void sendSOSAck(int32_t arg1, int32_t arg2);
    void sendCDSAck(int32_t arg1, int32_t arg2);
    void doSignalAckOnActiveCall(int32_t event, int32_t arg1, int32_t arg2);
    void setQueryFeatureName(const std::string& featureName);
    bool doECallRecovery();
    void doRecoveringMSD();
    void clearMSD();

    std::string getQueryFeatureName() const;
    std::unique_ptr<ECallTimerHandler> mTimerHandler;
    NAECallRecovery mECallRecovery;

    android::sp<Timer> mCancelingTimer;
    android::sp<Timer> mCallDiallingTimer;
    android::sp<Timer> mCallRetryTimer;
    int32_t mAttemptDataCount;
    int32_t mAttemptVoiceCount;
    int32_t mMaxAttemptVoice;
    int32_t mMaxAttemptData;
};

}
#endif //FLAG_NA_ECALL_PROCESSOR
