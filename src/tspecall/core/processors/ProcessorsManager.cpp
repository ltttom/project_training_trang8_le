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
 * @file        ProcessorsManager.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "ProcessorsManager.h"
#include "logger.h"
#include "../utils/ParamsDef.h"
#include "../../variant/honda/processors/NAECallProcessor.h"
#include "../../variant/honda/processors/NADESSECall.h"
#include "../../variant/honda/processors/CPDSmartphoneProcessor.h"
#include "../services/VehicleVariationAdapter.h"

namespace ecallapp {

/**
 * This class is the oversee the logic of NA-ECall, NA-DESS and CPD processors.
 * After the initialization, the processor list would contains NAECallProcessor, NADESSECall and CPDSmartphoneProcessor
 * */
ProcessorsManager::ProcessorsManager() {
    LOG_I(DLT_STRING("ProcessorsManager"));
    const bool isECallNAAvailable =
                      VehicleVariationAdapter::getInstance()->isFeatureAvailableInRegion(feature_name::ecall_us) ||
                      VehicleVariationAdapter::getInstance()->isFeatureAvailableInRegion(feature_name::ecall_us_pf3);

    const bool isDESSCallNAAvailable =
                      VehicleVariationAdapter::getInstance()->isFeatureAvailableInRegion(feature_name::na_dess_ecall)
                      && isECallNAAvailable;
    if (isECallNAAvailable) {
        mListProcessors.push_back(std::make_shared<NAECallProcessor>());
    }
	  if (isDESSCallNAAvailable) {
        mListProcessors.push_back(std::make_shared<NADESSECall>());
    }

    if (VehicleVariationAdapter::getInstance()->isFeatureAvailableInRegion(feature_name::cpd_smartphone_allert))
    {
        LOG_I(DLT_STRING("CPD smartphone processor is added"));
        mListProcessors.push_back(std::make_shared<CPDSmartphoneProcessor>());
    }

    for (auto& processor : mListProcessors) {
        processor->initialize();
    }
}

/**
 * handleMessage:
 * Each element popped queue would be passed to this method.
 * Its processors process/handle the element one by one.
 * Actually, based on message's what, only one processor would handle.
 */
void ProcessorsManager::handleMessage(const uint32_t& what, const android::sp<sl::Message>& msg) {
    for (auto& processor : mListProcessors) {
        processor->processorHandleMessage(what, msg);
    }
}

void ProcessorsManager::handleTimerTimeoutEvent(const int32_t& msgId) {
    for (auto& processor : mListProcessors) {
        processor->onTimerTimeoutEvent(msgId);
    }
}

/**
 * requestLaunch
 * When ApplicationManager identifies the action LAUNCH for the feature, this function is called
 */
void ProcessorsManager::requestLaunch(const std::string strFeatureName) {
    for (auto& processor : mListProcessors) {
        processor->launchFeature(strFeatureName);
    }
}

void ProcessorsManager::requestIgnore(const std::string strFeatureName) {
    for (auto& processor : mListProcessors) {
        processor->ignoreFeature(strFeatureName);
    }
}

void ProcessorsManager::requestTerminate(const std::string strFeatureName) {
    for (auto& processor : mListProcessors) {
        processor->terminateFeature(strFeatureName);
    }
}
}