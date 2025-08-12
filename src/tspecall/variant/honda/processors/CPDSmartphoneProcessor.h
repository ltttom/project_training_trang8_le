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
 * @file        acn_phone_processor.h
 * @author
 *    copyright (c) 2025  Long Nhat Nguyen <nhatlong.nguyen@lgepartner.com>
 * @version     1.0.00
 */
#pragma once

#include "../../../core/processors/IProcessorBase.h"
#include "tspecall/variant/honda/msd/DataBuilderCPDSmartphone.h"
#include "../../../core/utils/ECallTimerHandler.h"
namespace ecallapp
{
#define HORN_OPERATING 1
#define ALERT_END      3

enum class CPDState: int32_t
{
    NOT_INIT,
    IDLE,
    RUNNING
};

const static std::unordered_map< CPDState, std::string > CPDStates
{
    {CPDState::NOT_INIT       , "NOT_INIT"               },
    {CPDState::IDLE           , "IDLE"                   },
    {CPDState::RUNNING        , "RUNNING"                }
};

enum class EventName: int32_t
{
    StartSecurityHorn = HORN_OPERATING,
    EndCpd = ALERT_END,
};

/**
 * @brief A look up table for EventName enum
 * @note the first letter is lower case to match the spec
 *      but the element name has to follow the name convention
 * @see http://avncb.lge.com:8080/cb/issue/32675669
 */
const static std::unordered_map< EventName, std::string > EventNames
{
    {EventName::StartSecurityHorn   , "startSecurityHorn"   },
    {EventName::EndCpd              , "endCpd"              },
};

class CPDSmartphoneProcessor: public IProcessorBase
{
public:
    ~CPDSmartphoneProcessor() override = default;

    void initialize() override;
    void processorHandleMessage(const uint32_t& what, const android::sp<sl::Message>& msgData) override;
    void launchFeature(const std::string strFeatureName) override;
    void terminateFeature(const std::string strFeatureName) override;
    void ignoreFeature(const std::string strFeatureName) override;
    void onTimerTimeoutEvent(const int32_t& msgId) override;

private:
    /**
     * @brief transition to new state if needed
     * @param newState next state
     */
    void updateState(CPDState newState);

    /**
     * @brief collect & send CPD data to GTC, no response required
     */
    void sendDataToGTC();

    /**
     * @brief check if smartphone CPD processor is busy
     * @return true if CPD processor is busy, false otherwise
     */
    bool isCPDSmartphone(const std::string& feature);

    /**
     * @brief end the CPD data transmission
     */
    void endCPDTransmission();

    DataBuilderCPDSmartphone m_data;
    CPDState m_state {CPDState::NOT_INIT};
    std::string m_smartphoneEvent;
    android::sp<Timer> m_CPDTimeoutTimer {nullptr};
};

} // </oc>
