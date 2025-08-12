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
 * @file        NAECallRecovery.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "tspecall/core/services/SystemPropertyAdapter.h"
#include "../../../core/services/HTTPSManagerAdapter.h"
#include "../../../core/services/VehicleManagerAdapter.h"
#include "../../../core/services/LocationManagerAdapter.h"
#include "../../../core/services/ConfigurationManagerAdapter.h"
#include "tspecall/core/services/TimeManagerAdapter.h"
#include "NAECallRecovery.h"
#include "ECallState.h"
#include "../../../core/utils/ECallAppHandler.h"

namespace ecallapp {

NAECallRecovery::NAECallRecovery() {
    mCBWTFlag = false;
}

bool NAECallRecovery::doECallRecovery() {
    int32_t stateToRecover = getStateToRecover();
    if (stateToRecover == ecall_state::BOOTUP) {
        LOG_I(DLT_STRING("RECOVERY: state is BOOTUP"));
        return false;
    }
    LOG_I(DLT_STRING("RECOVERY: state="), DLT_STRING(ecall_state::ECallStateMap.at(stateToRecover).getName()));

    int32_t eCallType = getECallTypeToRecover();
    if (eCallType == ecall_type::UNDEFINED) {
        LOG_I(DLT_STRING("doesn't need to recover eCall, there is no cds.txt, ecall.txt, dess.txt flag"));
        return false;
    }
    LOG_I(DLT_STRING("RECOVERY: eCall type="), DLT_STRING(ecall_type::ECALLTYPE_NAME.at(eCallType).c_str()));

    int32_t recoverMsg = ecall_state::ECallStateMap.at(stateToRecover).getRecoverMsg();
    if (recoverMsg == 0) {
        LOG_I(DLT_STRING("RECOVERY: no need to recover for STATE: "), DLT_STRING(ecall_state::ECallStateMap.at(stateToRecover).getName()));
        return false;
    }

    ECallAppHandler* tHandler = ECallAppHandler::getInstance();
    if (tHandler == nullptr) {
        LOG_I(DLT_STRING("RECOVERY: ECallAppHandler instance is null, cannot send message."));
        return false;
    }

    tHandler->obtainMessage(recoverMsg, eCallType)->sendToTarget();
    LOG_I(DLT_STRING("RECOVERY: send message to recover eCall, msg="), DLT_INT(recoverMsg), DLT_STRING(", eCallType="), DLT_INT(eCallType));
    return true;
}

int32_t NAECallRecovery::getStateToRecover() {
    bool tIsValid = false;
    std::string strSavedState = std::string();
    int32_t retState = ecall_state::BOOTUP;

    strSavedState= SystemPropertyAdapter::getInstance()->getDataFromProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::BOOTUP));
    retState = CommonFunctions::stringToInt32(strSavedState, tIsValid, ecall_state::BOOTUP);

    if (ecall_state::ECallStateMap.find(retState) == ecall_state::ECallStateMap.end()) {
        LOG_I(DLT_STRING("RECOVERY: state="), DLT_STRING(strSavedState.c_str()), DLT_STRING(" is invalid, set to UNDEFINED"));
        retState = ecall_state::BOOTUP;
    }

    LOG_I(DLT_STRING("RECOVERY: Final recovery state="), DLT_STRING(ecall_state::ECallStateMap.at(retState).getName()));
    return retState;
}

int32_t NAECallRecovery::getECallTypeToRecover() {
    uint8_t res{0U};
    bool isACN = CommonFunctions::readFromNVM(res, ecall_file::ACN_FILE) != E_ERROR;
    bool isSOS = CommonFunctions::readFromNVM(res, ecall_file::SOS_FILE) != E_ERROR;
    bool isDESS = CommonFunctions::readFromNVM(res, ecall_file::DESS_FILE) != E_ERROR;

    return isACN ? ecall_type::ACN :
            isSOS ? ecall_type::SOS :
            isDESS ? ecall_type::DESS : ecall_type::UNDEFINED;
}

void NAECallRecovery::doRecoveringAtSignalDetectingTimePartOfMSD() {
    VehicleManagerAdapter::getInstance()->restoreAtSignalDetectingTimePartOfMSD();
}

std::string NAECallRecovery::recoverMSD() {
    std::lock_guard<std::mutex> lock(readMsdFileMutex);
    std::string recoveredMSDStr = CommonFunctions::readTSPMSD();
    LOG_I(DLT_STRING("Recovered MSD:"), DLT_STRING(recoveredMSDStr.c_str()));
    return recoveredMSDStr;
}

void NAECallRecovery::doRecoveringTelNo() {
    std::string tTelNo = ConfigurationManagerAdapter::getInstance()->getConfig(ecall_config::CF_PHONE_NUMBER_NAME);
    tTelNo = SystemPropertyAdapter::getInstance()->getDataFromProperty(SystemProperty::TEL_NO, tTelNo);
    HTTPSManagerAdapter::getInstance()->setTelNo(tTelNo);
}

void NAECallRecovery::doSetConditionToTestRecoverECall(uint32_t recoveryCase) {
    // acn
    if (recoveryCase == ACN_ECALL_FROM_BEGINNING_RECOVERY) {
        LOG_I(DLT_STRING("set to recover acn ecall from begining"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::STANDBY));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::ACN_FILE);
    }
    else if(recoveryCase == ACN_ECALL_FROM_PREP_PHASE_RECOVERY) {
        LOG_I(DLT_STRING("set to recover acn ecall from preparation phase"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::CANCEL_PERIOD));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::ACN_FILE);
    }
    else if(recoveryCase == ACN_ECALL_FROM_DATA_TRANSMISSION_PHASE_RECOVERY) {
        LOG_I(DLT_STRING("set to recover acn ecall from data transmission phase"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::MSD_TRANSMISSION));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::ACN_FILE);
    }
    else if(recoveryCase == ACN_ECALL_FROM_VOICE_CALL_RECOVERY) {
        LOG_I(DLT_STRING("set to recover acn ecall from voice call phase"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::CALL_CONNECTING));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::ACN_FILE);
    }
    else if(recoveryCase == ACN_ECALL_FROM_CALLBACK_WAITING_RECOVERY) {
        LOG_I(DLT_STRING("set to recover acn ecall from callback waiting phase"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::CALLBACK_WAITING));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::ACN_FILE);
    }
    // dess
    else if (recoveryCase == DESS_ECALL_FROM_BEGINNING_RECOVERY) {
        LOG_I(DLT_STRING("set to recover dess ecall from begining"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::STANDBY));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::DESS_FILE);
    }
    else if(recoveryCase == DESS_ECALL_FROM_PREP_PHASE_RECOVERY) {
        LOG_I(DLT_STRING("set to recover dess ecall from preparation phase"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::CANCEL_PERIOD));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::DESS_FILE);
    }
    else if(recoveryCase == DESS_ECALL_FROM_DATA_TRANSMISSION_PHASE_RECOVERY) {
        LOG_I(DLT_STRING("set to recover dess ecall from data transmission phase"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::MSD_TRANSMISSION));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::DESS_FILE);
    }
    else if(recoveryCase == DESS_ECALL_FROM_VOICE_CALL_RECOVERY) {
        LOG_I(DLT_STRING("set to recover dess ecall from voice call phase"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::CALL_CONNECTING));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::DESS_FILE);
    }
    else if(recoveryCase == DESS_ECALL_FROM_CALLBACK_WAITING_RECOVERY) {
        LOG_I(DLT_STRING("set to recover dess ecall from callback waiting phase"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::CALLBACK_WAITING));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::DESS_FILE);
    }
    // sos
    else if (recoveryCase == SOS_ECALL_FROM_BEGINNING_RECOVERY) {
        LOG_I(DLT_STRING("set to recover sos ecall from begining"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::STANDBY));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::SOS_FILE);
    }
    else if(recoveryCase == SOS_ECALL_FROM_PREP_PHASE_RECOVERY) {
        LOG_I(DLT_STRING("set to recover sos ecall from preparation phase"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::CANCEL_PERIOD));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::SOS_FILE);
    }
    else if(recoveryCase == SOS_ECALL_FROM_DATA_TRANSMISSION_PHASE_RECOVERY) {
        LOG_I(DLT_STRING("set to recover sos ecall from data transmission phase"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::MSD_TRANSMISSION));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::SOS_FILE);
    }
    else if(recoveryCase == SOS_ECALL_FROM_VOICE_CALL_RECOVERY) {
        LOG_I(DLT_STRING("set to recover sos ecall from voice call phase"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::CALL_CONNECTING));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::SOS_FILE);
    }
    else if(recoveryCase == SOS_ECALL_FROM_CALLBACK_WAITING_RECOVERY) {
        LOG_I(DLT_STRING("set to recover sos ecall from callback waiting phase"));
        SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::ECALL_STATE, std::to_string(ecall_state::CALLBACK_WAITING));
        CommonFunctions::saveToNVM(ecall_file::FILE_DATA, ecall_file::SOS_FILE);
    }
    else {
        LOG_I(DLT_STRING("wrong param"));
    }
}

void NAECallRecovery::calculateT9RemainingTime(std::string timeType) {
    int64_t remainingTime = -1;
// Determine the appropriate timer to calculate the remaining time.
// Currently, only CBWT is supported, but this can be extended to other timers in the future.
    if (timeType == ecalltimer::CBWT) {
        mCBWTFlag = false; //reset first
        bool isValid{false};
        std::string tCBWTCountdown = SystemPropertyAdapter::getInstance()->getDataFromProperty(SystemProperty::CBWT_COUNTDOWN, "0");
        int64_t tCBWTCountdownInt = CommonFunctions::stringToInt64(tCBWTCountdown, isValid, 0);
        if (isValid == false) {
            tCBWTCountdownInt = static_cast<int64_t>(ecall_na_timer::CALLBACK_WAITING_TIMER_DURATION);
        } else {
            isValid = false;
        }
        int64_t currentTimestamp = TimeManagerAdapter::getCurrentTimestamp();
// If the timeType is incorrect, the timer will run a full countdown.
        if (currentTimestamp == 0) {
            remainingTime = tCBWTCountdownInt;
            mCBWTFlag = true; //Set a flag to indicate that this timer needs to be recalculated when the time is ready.
        } else {
// get timestamp when we first start CBWT
            std::string tCBWTStartTime = SystemPropertyAdapter::getInstance()->getDataFromProperty(SystemProperty::CBWT_START_TIME, "0");
            int64_t tCBWTStartTimeInt = CommonFunctions::stringToInt64(tCBWTStartTime, isValid, 0);
// check if the timestamp is invalid, then the timer will run a full countdown.
            if (isValid == false || currentTimestamp <= tCBWTStartTimeInt) {
                remainingTime = tCBWTCountdownInt;
// calculate remaining time
            } else {
                remainingTime = tCBWTCountdownInt - (currentTimestamp - tCBWTStartTimeInt);
                if (remainingTime < 0) {
                    remainingTime = 0;
                }
            }
        }
    }
    LOG_I(DLT_STRING("Remaining time of"), DLT_STRING(timeType.c_str()), DLT_STRING(":"), DLT_INT64(remainingTime), DLT_STRING("ms"));
    mRemainingTimeMap[timeType] = remainingTime; //add this countdown to the map to be used later.
}

int64_t NAECallRecovery::getRemainingTime(std::string timeType) {
    int64_t remainingTime{-1};
// if the timer is not in the map, it means that the timer has not been calculated yet, return -1.
    if (mRemainingTimeMap.find(timeType) != mRemainingTimeMap.end()) {
        remainingTime = mRemainingTimeMap[timeType];
        // mRemainingTimeMap.erase(timeType);
        LOG_I(DLT_STRING(timeType.c_str()), DLT_STRING(":"), DLT_INT64(remainingTime));
    }
    return remainingTime;
}

bool NAECallRecovery::getCBWTFlag() const {
    return mCBWTFlag;
}

void NAECallRecovery::resetCBWTFlags() {
    mCBWTFlag = false;
    mRemainingTimeMap.clear();
}

}