#ifndef FLAG_NA_ECALL_PROCESSOR
#define FLAG_NA_ECALL_PROCESSOR

#include "tspecall/variant/honda/processors/ECallState.h"
#include "tspecall/core/processors/IProcessorBase.h"
#include "tspecall/core/utils/ECallTimerHandler.h"
#include "tspecall/core/utils/CommonFunctions.h"
#include "tspecall/core/utils/ECallAppHandler.h"
#include "tspecall/variant/honda/processors/InternalParamsDef.h"
#include "tspecall/variant/honda/processors/NAECallRecovery.h"

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
    // Handle system booing up completed event
    virtual void doSystemBootUpCompleted();

    // Detecting manual, auto ecall signal funtion
    virtual void doECallCDSTrigger(const uint32_t prev,const uint32_t cur);
    virtual void doECallSRSTrigger();
    virtual void onHMIReceived(const uint32_t& index, const uint32_t& action);
    virtual void doECallButtonTrigger(bool isAfterRecovery = false);
    
    // launch feature action
    virtual void launchRecoveredECall(std::string strFeatureName);
    virtual void launchAutoECall(std::string strFeatureName);
    virtual void launchDessECall(std::string strFeatureName);
    virtual void launchManualECall(std::string strFeatureName);
    virtual void doActionsAtLaunchingTime(bool isRecover);

    virtual void doECallCancelling();
    virtual void doPreparingMSD();
    virtual void doMSDTransmission();
    virtual void doMSDSuccess();
    virtual void doDataRetryStrategy();
    virtual void doHandleIncommingCall();
    virtual void doVoiceCallRetryStrategy();
    virtual void doVoiceCallRetry();
    virtual void doCallConnecting();
    virtual void doCallDialling();
    virtual void doECallConnected();
    virtual void transitsToCBWTMode();
    virtual void transitsToCBWTModeAgain();
    virtual void doECallTermination();
    virtual void playLedPatternInMs(const int32_t ledPartten, const int32_t timeMs);
    virtual void doHandlePowerStateChanged();

    virtual void doDESSECallTrigger(bool isAfterRecovery = false);
    virtual void doDESSECallCancel();

    virtual void doResetECall();
    virtual void doHandleInternalAppMsg(const uint32_t& what, const android::sp<sl::Message>& msgData);

    virtual bool checkIgAccCondition();

    virtual void doOnTimeReceived();

    ECallState mECallState;
    ECallState mECallCancellingState;
    ECallState mECallClearAudioState;
    std::unique_ptr<ECallTimerHandler> mTimerHandler;
    android::sp<Timer> mDataRetryTimer;
    android::sp<Timer> mCancelingTimer;
    android::sp<Timer> mCallRetryTimer;
    android::sp<Timer> mCallDiallingTimer;

    int32_t mAttemptVoiceCount;
    int32_t mMaxAttemptVoice;
    int32_t mAttemptDataCount;
    int32_t mMaxAttemptData;
    bool mIsCallConnected;
    bool mTerminateAfterCall;
    std::string mCurrentFeatureName;
    std::string mMSD;
    bool mIsDessCall;
    bool mIsQueryRecoveredECall;
    NAECallRecovery mECallRecovery;
};

}


#endif //FLAG_NA_ECALL_PROCESSOR