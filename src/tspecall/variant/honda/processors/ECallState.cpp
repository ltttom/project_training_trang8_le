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
 * @file        ECallState.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "ECallState.h"
#include "tspecall/core/services/SystemPropertyAdapter.h"

namespace ecallapp {
ECallState::ECallState() :
mCurrentState(ecall_state::BOOTUP),
mPreviousState(ecall_state::BOOTUP)
{
}

int32_t ECallState::getCurrentState() const {
    return mCurrentState;
}
int32_t ECallState::getPreviousState() const{
    return mPreviousState;
}

/**
 * changeState: save data of current state by SystemPropertyAdapter
 */
void ECallState::changeState(int32_t newState) {

    if (newState != mCurrentState) {
        if (ecall_state::ECallStateMap.find(newState) == ecall_state::ECallStateMap.end() ||
            ecall_state::ECallStateMap.find(mCurrentState) == ecall_state::ECallStateMap.end()) {
            LOG_E(DLT_STRING("changeState() Invalid state: "), DLT_INT(newState), DLT_STRING(" or "), DLT_INT(mCurrentState));
            return;
        }

        mPreviousState = mCurrentState;
        mCurrentState = newState;

        LOG_I(DLT_STRING("changeState() "),  DLT_STRING(ecall_state::ECallStateMap.at(mPreviousState).getName()),
            DLT_STRING(" -> "),  DLT_STRING(ecall_state::ECallStateMap.at(mCurrentState).getName()));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(mCurrentState));
    }
    else {
        LOG_I(DLT_STRING("changeState() failure due to previous = current"));
    }
}

}