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
 * @file        CPDSmartphoneProcessor.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "CPDSmartphoneProcessor.h"
#include "../../../core/utils/CommonFunctions.h"
#include "../../../core/services/RemoteIfManagerAdapter.h"
#include "../../../core/services/PowerManagerAdapter.h"
#include "../../../core/services/ApplicationManagerAdapter.h"
#include "../../../core/services/ConfigurationManagerAdapter.h"
#include "InternalParamsDef.h"
#include <cstdint>
namespace ecallapp
{

void CPDSmartphoneProcessor::initialize()
{
    LOG_I("initialize");
    updateState(CPDState::IDLE);
    m_CPDTimeoutTimer = android::sp<Timer>{new Timer(new ECallTimerHandler(), static_cast<int32_t>(ecall_na_timer::GTC_CPD_TIMEOUT_TIMER_ID))};
    m_CPDTimeoutTimer->setDurationMs(ecall_na_timer::GTC_CPD_TIMEOUT_TIMER_DURATION, 0U);
}

void CPDSmartphoneProcessor::processorHandleMessage(const uint32_t& what, const android::sp<sl::Message>& msgData)
{
    if (m_state == CPDState::NOT_INIT)
    {
        LOG_I(DLT_STRING("CPD smartphone processor cannot handle message in NOT_INIT state"));
        return;
    }

    switch (static_cast<msg::MSG>(what))
    {
        case msg::MSG::APPM_REQUEST_LAUNCH:
        {
            android::sp<AppMgrEventData> pAppMgrEventData {nullptr};
            msgData->getObject(pAppMgrEventData);
            if (pAppMgrEventData != nullptr) {
                LOG_I(DLT_STRING("requestLaunch, featurename = "), DLT_STRING(pAppMgrEventData->mFeatureName.c_str()));
                launchFeature(pAppMgrEventData->mFeatureName);
            }

            break;
        }

        case msg::MSG::APPM_REQUEST_IGNORE:
        {
            android::sp<AppMgrEventData> pAppMgrEventData {nullptr};
            msgData->getObject(pAppMgrEventData);
            if (pAppMgrEventData != nullptr) {
                LOG_I(DLT_STRING("requestIgnore, featurename = "), DLT_STRING(pAppMgrEventData->mFeatureName.c_str()));
                ignoreFeature(pAppMgrEventData->mFeatureName);
            }
            break;
        }

        case msg::MSG::REQ_CPA_SMARTPHONE_ALERT:
        {
            if (PowerManagerAdapter::getInstance()->getIGStatus() == MCU_VALUE_STATUS::MCU_STATUS_ON)
            {
                LOG_E(DLT_STRING("CPD Smartphone only works in IG OFF, reject the request"));
                break;
            }

            auto it = EventNames.find(static_cast<EventName>(msgData->arg1));
            if (it == EventNames.end())
            {
                LOG_E(DLT_STRING("Received invalid CPD smartphone request id: "), DLT_INT(msgData->arg1));
                break;
            }

            m_smartphoneEvent = it->second;
            LOG_I(DLT_STRING("queryActionForFeature CPD smartphone for event: "), DLT_STRING(m_smartphoneEvent.c_str()));
            ApplicationManagerAdapter::getInstance()->queryActionForFeature(FEATURE_NAMES::HONDA_CPD_SMARTPHONE);
            break;
        }


        case msg::MSG::MSG_RIM_GTC_RESPONSE:
        {
            if(ConfigurationManagerAdapter::getInstance()->getConfig(ecall_config::CF_RIM_RESPONSE_IGNORED) == "RIM_IGNORED") {
              // timer expired
              break;
            }
            else {
                android::sp<oc::RIMResponseType> response = nullptr;
                msgData->getObject(response);
                if (response == nullptr)
                {
                    break;
                }

                if (response->sendType != ESendType::CPD)
                {
                    // oemcallapp boardcasts the receive message to all processors
                    // not just the correct destination, so processor its own has
                    // to prevent the invalid ID itself
                    break;
                }

                auto serverResp = static_cast<int32_t>(response->serverResponse);
                LOG_I
                (
                    DLT_STRING("Received RIM's serverResponse type "),
                    DLT_INT32(serverResp),
                    DLT_STRING(", turn off CPD Smartphone. (0: SUCCESS, 1: FAILURE, 3: FAILURE_RETRY)")
                );

                endCPDTransmission();
            }
            break;
        }

        default:
        {
            break;
        }
    }
}

void CPDSmartphoneProcessor::launchFeature(const std::string strFeatureName)
{
    if (!isCPDSmartphone(strFeatureName))
    {
        LOG_E
        (
            DLT_STRING("Cannot start CPD smartphone with name "),
            DLT_STRING(strFeatureName.c_str())
        );
        return;
    }

    updateState(CPDState::RUNNING);
    ApplicationManagerAdapter::getInstance()->setFeatureStatus(FEATURE_NAMES::HONDA_CPD_SMARTPHONE, true);
    PowerManagerAdapter::getInstance()->acquirePowerLock();

    // TODO: Not sure yet about the privacy for CPD smartphone
    //
    // if (!spAdapters->spConfigManager->getPrivacy(self::privacy_acn_wphone_id)){
    //     LOG_W(DLT_STRING("ACN with phone privacy OFF"));
    //     return false;
    // }

    // TODO: Not sure yet about the subcrption for CPD smartphone.
    // Refer R-call or ACN with phone code to get it done later
    //
    // if (!checkSubcription(requestType)) {
    //     LOG_W(DLT_STRING("ACN with phone subcrption OFF"));
    //     return false;
    // }

    sendDataToGTC();
    m_CPDTimeoutTimer->start();
    LOG_I(DLT_STRING("CPD smartphone sent data to RIM successfully"));
}

void CPDSmartphoneProcessor::ignoreFeature(const std::string strFeatureName)
{
    if (!isCPDSmartphone(strFeatureName))
    {
        return;
    }

    LOG_I(DLT_STRING("do nothing"));
}

void CPDSmartphoneProcessor::terminateFeature(const std::string strFeatureName)
{
    if (!isCPDSmartphone(strFeatureName))
    {
        return;
    }

    LOG_I(DLT_STRING("do nothing"));
}

void CPDSmartphoneProcessor::onTimerTimeoutEvent(const int32_t& msgId)
{
    if (msgId == ecall_na_timer::GTC_CPD_TIMEOUT_TIMER_ID) {
        LOG_I(DLT_STRING("CPD smartphone timer out. Transform to IDLE state"));
        endCPDTransmission();
    }
}

void CPDSmartphoneProcessor::endCPDTransmission()
{
    PowerManagerAdapter::getInstance()->releasePowerLock();
    m_CPDTimeoutTimer->stop();
    m_smartphoneEvent.clear();
    updateState(CPDState::IDLE);
    ApplicationManagerAdapter::getInstance()->setFeatureStatus(FEATURE_NAMES::HONDA_CPD_SMARTPHONE, false);
}

void CPDSmartphoneProcessor::updateState(CPDState newState)
{
    const auto fromState = CommonFunctions::enumToString(CPDStates, m_state);
    const auto toState = CommonFunctions::enumToString(CPDStates, newState);
    if (m_state != newState)
    {
        LOG_I
        (
            DLT_STRING("CPD state changed from "),
            DLT_STRING(fromState.c_str()),
            DLT_STRING(" to "),
            DLT_STRING(toState.c_str())
        );

        m_state = newState;
    }
}

void CPDSmartphoneProcessor::sendDataToGTC()
{
    DataBuilderCPDSmartphone builder;
    auto payload = builder.setGPS().setUtil(m_smartphoneEvent).build();

    LOG_I
    (
        DLT_STRING("CPD Payload = "),
        DLT_STRING(payload.c_str()),
        DLT_STRING(", Length = "),
        DLT_UINT(payload.length())
    );

    RemoteIfManagerAdapter::getInstance()->send(payload, ESendType::CPD);
}

bool CPDSmartphoneProcessor::isCPDSmartphone(const std::string& feature)
{
    return feature == FEATURE_NAMES::HONDA_CPD_SMARTPHONE;
}

} // </ecallapp>
