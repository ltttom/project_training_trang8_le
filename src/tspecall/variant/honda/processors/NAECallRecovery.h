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
 * @file        NAECallRecovery.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef FLAG_NA_ECALL_RECOVERY
#define FLAG_NA_ECALL_RECOVERY

#include "../../../core/processors/IProcessorBase.h"
#include "../../../core/utils/ECallTimerHandler.h"
#include "../../../core/utils/CommonFunctions.h"
#include "../../../core/utils/ECallAppHandler.h"
#include "../../../core/utils/CommonFunctions.h"
#include "InternalParamsDef.h"
#include <mutex>
namespace ecallapp {

class NAECallRecovery {
public:
    NAECallRecovery();
    // returned value: true if need to recover previous ecall
    bool doECallRecovery();
    void doRecoveringAtSignalDetectingTimePartOfMSD(); //restore SRS_PS_OCCUPIED, SRS_DR_BELT_STATUS, SRS_PS1_BELT_STATUS
    std::string recoverMSD();
    void doRecoveringTelNo();
    // sldd to test recovery function
    void doSetConditionToTestRecoverECall(uint32_t recoveryCase);
    /**
     * @brief calculate remaining time for a timer
     * @param timeType

     */
    void calculateT9RemainingTime(std::string timeType);
    /**
     * @brief get remaining time of a timer after recovery
     * @param timeType
     * @return
     */
    int64_t getRemainingTime(std::string timeType);
    /**
     * @brief Verify if the countdown CBWT timer needs to be recalculated
     *
     * @return true
     * @return false
     */
    bool getCBWTFlag() const;
    /**
     * @brief Reset the recovery flags for timer
     */
    void resetCBWTFlags();

private:
    int32_t getStateToRecover();
    int32_t getECallTypeToRecover();
    int32_t getRecoveryMessage(int32_t stateToRecover);
    void doPreconditionForRecovery(int32_t stateToRecover);
    /**
     * @brief The flag will be set if the TSU cannot retrieve the current time from TimeManager during recovery
     *
     */
    bool mCBWTFlag;
    /**
     * @brief A map will store remaining time of each timer after recovery
     *
     */
    std::map<std::string, int64_t> mRemainingTimeMap;
    std::mutex readMsdFileMutex;
};

}

#endif //FLAG_NA_ECALL_RECOVERY