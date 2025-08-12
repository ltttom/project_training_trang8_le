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
 * @file        DisconnectCause.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef DISCONNECTCAUSE_H
#define DISCONNECTCAUSE_H

#include <map>
#include <string>
#include "../utils/ParamsDef.h"
#include "logger.h"
namespace ecallapp
{
    namespace DisconnectCause
    {
        // The disconnect cause is not valid (Not received a disconnect cause)
        constexpr int32_t NOT_VALID { -1 };
        // Has not yet disconnected
        constexpr int32_t NOT_DISCONNECTED { 0 };
        // An incoming call that was missed and never answered
        constexpr int32_t INCOMING_MISSED { 1 };
        // Normal; Remote hangup
        constexpr int32_t NORMAL { 2 };
        // Normal; Local hangup
        constexpr int32_t LOCAL { 3 };
        // Outgoing call to busy line
        constexpr int32_t BUSY { 4 };
        // Outgoing call to congested network
        constexpr int32_t CONGESTION { 5 };
        // Not presently used
        constexpr int32_t MMI { 6 };
        // Invalid dial string
        constexpr int32_t INVALID_NUMBER { 7 };
        // Cannot reach the peer
        constexpr int32_t NUMBER_UNREACHABLE { 8 };

    } // namespace DisconnectCause

    static std::unordered_map<int32_t, std::string> callDisconnectedCauseMap {
        {
            { DisconnectCause::NOT_VALID, "NOT_VALID" },
            { DisconnectCause::NOT_DISCONNECTED, "NOT_DISCONNECTED" },
            { DisconnectCause::INCOMING_MISSED, "INCOMING_MISSED" },
            { DisconnectCause::NORMAL, "NORMAL" },
            { DisconnectCause::LOCAL, "LOCAL" },
            { DisconnectCause::BUSY, "BUSY" },
            { DisconnectCause::CONGESTION, "CONGESTION" },
            { DisconnectCause::MMI, "MMI" },
            { DisconnectCause::INVALID_NUMBER, "INVALID_NUMBER" },
            { DisconnectCause::NUMBER_UNREACHABLE, "NUMBER_UNREACHABLE" },
        }
    };
    
    inline std::string CALL_DISCONNECTED_CAUSE_NAME(const int32_t& id) noexcept
    {
        std::string disconnectCauseName{""};
        if(callDisconnectedCauseMap.find(id) != callDisconnectedCauseMap.end()) {
            disconnectCauseName = callDisconnectedCauseMap[id];
        }
        return disconnectCauseName;
    }
} /* End: namespace ECALLDCMNONREG */
#endif // DISCONNECTCAUSE_H
