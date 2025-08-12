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
 * @file        ECallState.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef ECALLAPP_ECALLSTATE_H
#define ECALLAPP_ECALLSTATE_H
#include <unordered_map>
#include <vector>
#include "../../../core/utils/ParamsDef.h"
#include "InternalParamsDef.h"

namespace ecallapp {

namespace ecall_state {
    const static int32_t BOOTUP {-1};
    const static int32_t RECOVERY {1};
    const static int32_t POWER_OFF {100};
    const static int32_t STANDBY {101};
    // const static int32_t IDLE {100};
    const static int32_t CANCEL_PERIOD {200};
    const static int32_t CLEAR_CURRENT_CALL{250};
    const static int32_t MSD_PREPARING {301};
    const static int32_t MSD_TRANSMISSION {300};
    // const static int32_t DIALING {400};
    const static int32_t CALL_CONNECTING {500};
    const static int32_t CALL_CONNECTED {550};
    const static int32_t CALLBACK_WAITING {600};
    const static int32_t CALLBACK_RUNNING {650};
    // const static int32_t RUNNING {700};

    class ECSValue {
    public:
        ECSValue()
            : name(""), recoverCandidate(false), recoverMsg(0)
        {}

        // Constructor with const reference to avoid unnecessary copies
        ECSValue(const std::string& name, bool recoverCandidate, int32_t recoverMsg)
            : name(name), recoverCandidate(recoverCandidate), recoverMsg(recoverMsg)
        {}

        const char* getName() const { return name.c_str(); }
        bool isRecoverCandidate() const { return recoverCandidate; }
        int32_t getRecoverMsg() const { return recoverMsg; }

    private:
        std::string name;
        bool recoverCandidate {false};
        int32_t recoverMsg;
    };

    static std::unordered_map<int32_t, ECSValue> ECallStateMap {
        {BOOTUP,            ECSValue("BOOTUP",              false,  0)},
        {RECOVERY,          ECSValue("RECOVERY",            false,  0)},
        {POWER_OFF,         ECSValue("POWER_OFF",           false,  0)},
        {STANDBY,           ECSValue("STANDBY",             false,  0)},
        {CANCEL_PERIOD,     ECSValue("CANCEL_PERIOD",       true,   ENUM_TO_INT(msg::AppMSG::RECOVER_ECALL_TO_CANCEL_PERIOD))},
        {MSD_PREPARING,     ECSValue("MSD_PREPARING",       false,  0)},
        {MSD_TRANSMISSION,  ECSValue("MSD_TRANSMISSION",    true,   ENUM_TO_INT(msg::AppMSG::RECOVER_ECALL_TO_DATA_TRANSMISSION))},
        {CLEAR_CURRENT_CALL, ECSValue("CLEAR_CURRENT_CALL", true,   ENUM_TO_INT(msg::AppMSG::RECOVER_ECALL_TO_CALL_CONNECT))},
        {CALL_CONNECTING,   ECSValue("CALL_CONNECTING",     true,   ENUM_TO_INT(msg::AppMSG::RECOVER_ECALL_TO_CALL_CONNECT))},
        {CALL_CONNECTED,    ECSValue("CALL_CONNECTED",      false,  0)},
        {CALLBACK_WAITING,  ECSValue("CALLBACK_WAITING",    true,   ENUM_TO_INT(msg::AppMSG::RECOVER_ECALL_TO_CALLBACK_WAITING))},
        {CALLBACK_RUNNING,  ECSValue("CALLBACK_RUNNING",    false,  0)},
    };
}

namespace ecall_audio_state {
    const static int32_t CLEAR_TONE_1 {100};
    const static int32_t CLEAR_TONE_2 {200};
    const static int32_t CLEAR_TONE_3 {300};
    const static int32_t UNCLEAR_TONE {400};
}

class ECallState {
public:
    ECallState();
    void setName(const std::string name);
    int32_t getCurrentState () const;
    int32_t getPreviousState () const;
    void changeState(int32_t newState);
    void printStatusLog() const;

private:
    int32_t mCurrentState {ecall_state::BOOTUP};
    int32_t mPreviousState {ecall_state::BOOTUP};
    std::string mName;
};

}

#endif // ECALLAPP_ECALLSTATE_H
