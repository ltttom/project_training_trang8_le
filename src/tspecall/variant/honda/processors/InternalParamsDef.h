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
 * @file        InternalParamsDef.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef DESS_UTILS_PARAMSDEF_H
#define DESS_UTILS_PARAMSDEF_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <functional>
#include "../../../core/utils/ParamsDef.h"
#include "../../../core/services/ApplicationManagerAdapter.h"

namespace ecallapp {

static constexpr uint32_t ACN_ECALL_FROM_BEGINNING_RECOVERY                 {1U};
static constexpr uint32_t ACN_ECALL_FROM_PREP_PHASE_RECOVERY                {2U};
static constexpr uint32_t ACN_ECALL_FROM_DATA_TRANSMISSION_PHASE_RECOVERY   {3U};
static constexpr uint32_t ACN_ECALL_FROM_VOICE_CALL_RECOVERY                {4U};
static constexpr uint32_t ACN_ECALL_FROM_CALLBACK_WAITING_RECOVERY          {5U};
static constexpr uint32_t DESS_ECALL_FROM_BEGINNING_RECOVERY                {11U};
static constexpr uint32_t DESS_ECALL_FROM_PREP_PHASE_RECOVERY               {12U};
static constexpr uint32_t DESS_ECALL_FROM_DATA_TRANSMISSION_PHASE_RECOVERY  {13U};
static constexpr uint32_t DESS_ECALL_FROM_VOICE_CALL_RECOVERY               {14U};
static constexpr uint32_t DESS_ECALL_FROM_CALLBACK_WAITING_RECOVERY         {15U};
static constexpr uint32_t SOS_ECALL_FROM_BEGINNING_RECOVERY                 {21U};
static constexpr uint32_t SOS_ECALL_FROM_PREP_PHASE_RECOVERY                {22U};
static constexpr uint32_t SOS_ECALL_FROM_DATA_TRANSMISSION_PHASE_RECOVERY   {23U};
static constexpr uint32_t SOS_ECALL_FROM_VOICE_CALL_RECOVERY                {24U};
static constexpr uint32_t SOS_ECALL_FROM_CALLBACK_WAITING_RECOVERY          {25U};

static constexpr uint32_t LED_LIGHTED_UP_PERIOD_MS                          {5000U};
static constexpr int32_t NUMBER_OF_MSD_TRANSMISSION_ATTEMP                  {2};
static constexpr int32_t VOICE_DIALING_INFINITE_ATTEMPT                     {-1};
static constexpr int32_t VOICE_DIALING_ATTEMPT_IN_MANUAL_MODE               {3};

namespace msg {
enum class AppMSG : int32_t
    {
        // DESS messages
        DESS_BASE = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::APP_INTERNAL_SRV),
        DESS_ECALL_NO_REQUEST_MSG,
        DESS_ECALL_REQUEST_MSG,
        DESS_ECALL_CANCEL_MSG,
        TSU_TO_RVU_ECALL_STATUS_NOTIFICATION_NO_REQUEST,
        TSU_TO_RVU_ECALL_STATUS_NOTIFICATION_REQUEST,
        TSU_TO_RVU_ECALL_STATUS_NOTIFICATION_CANCEL,

        RECOVER_ECALL_TO_STANDBY,
        RECOVER_ECALL_TO_CANCEL_PERIOD,
        RECOVER_ECALL_TO_DATA_TRANSMISSION,
        RECOVER_ECALL_TO_CALL_CONNECT,
        RECOVER_ECALL_TO_CALLBACK_WAITING,
    };
}

namespace dess_param {
    constexpr static uint32_t TSU_TO_RVU_ECALL_NO_REQUEST {0U};
    constexpr static uint32_t TSU_TO_RVU_ECALL_REQUEST {1U};
    constexpr static uint32_t TSU_TO_RVU_ECALL_CANCEL {2U};
}

/* Define timers for processors */
namespace ecall_na_timer {
    constexpr static uint32_t DATA_RETRY_TIMER_ID {101U};
    constexpr static uint32_t DATA_RETRY_TIMER_DURATION { 15000U };
    constexpr static uint32_t CANCELLING_TIMER_ID {100U};
    constexpr static uint32_t CANCELLING_TIMER_DURATION { 10000U };
    constexpr static uint32_t CALLBACK_WAITING_TIMER_ID {200U};
    constexpr static uint32_t CALLBACK_WAITING_TIMER_DURATION {3600000U};
    constexpr static uint32_t CALL_DIALLING_TIMER_ID {250U};
    constexpr static uint32_t CALL_DIALLING_TIMER_DURATION {30000U};
    constexpr static uint32_t CALL_RETRY_TIMER_ID {300U};
    constexpr static uint32_t CALL_RETRY_TIMER_DURATION {3000U};

    constexpr static uint32_t ECALL_STATUS_CANCEL_REQUEST_30S {400U};
    constexpr static uint32_t ECALL_STATUS_CANCEL_REQUEST_30S_DURATION {30000U};
    constexpr static uint32_t ECALL_STATUS_30S {450U};
    constexpr static uint32_t ECALL_STATUS_30S_DURATION {30000U};

    constexpr static uint32_t ECALL_NOTIFICATION_TIMER_REQUEST {500U};
    constexpr static uint32_t ECALL_NOTIFICATION_TIMER_REQUEST_DURATION {2000U};
    constexpr static uint32_t ECALL_NOTIFICATION_TIMER_NO_REQUEST {530U};
    constexpr static uint32_t ECALL_NOTIFICATION_TIMER_NO_REQUEST_DURATION {2000U};
    constexpr static uint32_t ECALL_NOTIFICATION_CANCEL_ECALL_TRIGGER {560U};
    constexpr static uint32_t ECALL_NOTIFICATION_CANCEL_ECALL_TRIGGER_DURTAION {2000U};

    constexpr static uint32_t DESS_CONFIRM_TIMER_NO_REQUEST {600U};
    constexpr static uint32_t DESS_CONFIRM_TIMER_NO_REQUEST_DURATION {2000U};
    constexpr static uint32_t DESS_CONFIRM_TIMER_REQUEST {700U};
    constexpr static uint32_t DESS_CONFIRM_TIMER_REQUEST_DURATION {2000U};
    constexpr static uint32_t DESS_CONFIRM_TIMER_CANCEL_REQUEST {720U};
    constexpr static uint32_t DESS_CONFIRM_TIMER_CANCEL_REQUEST_DURATION {2000U};

    constexpr static uint32_t DESS_NOTIFICATION_CANCEL_ECALL_TRIGGER{800U};
    constexpr static uint32_t DESS_NOTIFICATION_CANCEL_ECALL_TRIGGER_DURATION{2000U};

    /**
     * @brief Retry of sending data to GTC
     * @note "Other data services": Data 1 attempt, then up to 2 retries (3 total) 20s
     * @see http://avncb.lge.com:8080/cb/issue/28609509
     */
    constexpr static uint32_t GTC_CPD_TIMEOUT_TIMER_ID       {900U};
    constexpr static uint32_t GTC_CPD_TIMEOUT_TIMER_DURATION {80000U};
}

namespace ecall_type {
    const static int32_t UNDEFINED {-1};
    const static int32_t ACN {0};
    const static int32_t DESS {1};
    const static int32_t SOS {2};

    const static std::unordered_map<int32_t, std::string> ECALLTYPE_NAME {
        {ACN, "ACN"},
        {DESS, "DESS"},
        {SOS, "SOS"},
    };
}

namespace ecall_file {
    static const std::string ACN_FILE = "cds.txt";
    static const std::string DESS_FILE = "dess.txt";
    static const std::string SOS_FILE = "ecall.txt";

    static const int32_t FILE_DATA = 1U;
}

namespace ecall_feature {
    const static int32_t UNDEFINED {-1};
    const static int32_t OUTGOING_CALL {0};
    const static int32_t CALLBACK_WAITING {1};
    const static int32_t INCOMING_CALL {2};

    const static std::unordered_map<int32_t, std::string> ECALLFEATURE_NAME {
        {OUTGOING_CALL, "OUTGOING_CALL"},
        {CALLBACK_WAITING, "CALLBACK_WAITING"},
        {INCOMING_CALL, "INCOMING_CALL"},
    };

    const static std::vector<std::vector<std::string>> appfeature {
        {FEATURE_NAMES::HONDA_ACN_NA, FEATURE_NAMES::HONDA_ACN_NA, FEATURE_NAMES::HONDA_SOS_NA},
        {FEATURE_NAMES::HONDA_ACN_NA_CBWT, FEATURE_NAMES::HONDA_ACN_NA_CBWT, FEATURE_NAMES::HONDA_SOS_NA_CBWT},
        {FEATURE_NAMES::HONDA_ACN_NA_INCOMING, FEATURE_NAMES::HONDA_ACN_NA_INCOMING, FEATURE_NAMES::HONDA_SOS_NA_INCOMING},
    };
}

}

#endif
