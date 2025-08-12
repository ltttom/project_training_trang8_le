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
 * @file        IProcessorBase.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef ECALL_APP_IPROCESSORBASE_H
#define ECALL_APP_IPROCESSORBASE_H

#include <utils/Message.h>
#include <memory>
#include "../utils/ParamsDef.h"

namespace ecallapp {

class IProcessorBase : public std::enable_shared_from_this<IProcessorBase>{
public:
    IProcessorBase() = default;
    virtual ~IProcessorBase() = default;
    virtual void processorHandleMessage(const uint32_t& what, const android::sp<sl::Message>& msgData) = 0;
    virtual void launchFeature(const std::string strFeatureName) = 0;
    virtual void terminateFeature(const std::string strFeatureName) = 0;
    virtual void ignoreFeature(const std::string strFeatureName) = 0;
    virtual void onTimerTimeoutEvent(const int32_t& msgId) = 0;
    virtual void initialize() = 0;
};

}
#endif // ECALL_APP_IPROCESSORBASE_H
