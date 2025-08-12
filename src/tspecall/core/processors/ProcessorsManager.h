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
 * @file        ProcessorsManager.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef ECALLNONREG_PROCESSORSMANAGER_H
#define ECALLNONREG_PROCESSORSMANAGER_H

#include <vector>
#include <memory>
#include <map>
#include <string>
#include <utils/Message.h>

#include "IProcessorBase.h"
#include "../utils/ParamsDef.h"

namespace ecallapp {

class ProcessorsManager {
public:
    ProcessorsManager();
    void handleMessage(const uint32_t& what, const android::sp<sl::Message>& msg);
    void requestLaunch(const std::string strFeatureName);
    void requestIgnore(const std::string strFeatureName);
    void requestTerminate(const std::string strFeatureName);
    void handleTimerTimeoutEvent(const int32_t& msgId);

private:
std::vector<std::shared_ptr<IProcessorBase>> mListProcessors;
};

} /* End: namespace ECALLDCMNONREG */

#endif // ECALLNONREG_PROCESSORSMANAGER_H
