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
 * @file        NADESSECall.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef NA_DESS_ECALL_PROCESSOR
#define NA_DESS_ECALL_PROCESSOR

#include "ECallState.h"
#include "../../../core/processors/IProcessorBase.h"
#include "../../../core/utils/ECallTimerHandler.h"
#include "../../../core/utils/CommonFunctions.h"
#include "../../../core/utils/ECallAppHandler.h"
#include "InternalParamsDef.h"

namespace ecallapp {

class NADESSECall : public IProcessorBase {
public:
    NADESSECall();
    virtual void processorHandleMessage(const uint32_t& what, const android::sp<sl::Message>& msgData) override;
    virtual void launchFeature(const std::string strFeatureName) override;
    virtual void ignoreFeature(const std::string strFeatureName) override;
    virtual void terminateFeature(const std::string strFeatureName) override;
    virtual void initialize() override;
    virtual void onTimerTimeoutEvent(const int32_t& msgId) override;
    private:
    void sendECallRequestNotificationToRVU(const uint32_t value);
    uint32_t mTSUToRVUECallNoRequestCount;
    uint32_t mRVUtoTSUMessValue;
    std::unique_ptr<ECallTimerHandler> mTimerHandler;
    android::sp<Timer> mECallStatusCancelRequest;
    android::sp<Timer> mECallStatus;
    android::sp<Timer> mECallNotificationRequestTimer;
    android::sp<Timer> mECallNotificationNoRequestTimer;
    android::sp<Timer> mECallNotificationCancelTimer;
    android::sp<Timer> mDessConfirmNoRequest;
    android::sp<Timer> mDessConfirmRequest;
    android::sp<Timer> mDessConfirmCancelRequest;
    uint32_t mECallStatusNotificationValue;
};

}


#endif //NA_DESS_ECALL_PROCESSOR