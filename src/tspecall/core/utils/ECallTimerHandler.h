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
 * @file        ECallTimerHandler.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef ECALLTIMERHANDLER_H
#define ECALLTIMERHANDLER_H

#include <memory>
#include <utils/Timer.h>
#include <functional>
#include "logger.h"
#include "ECallAppHandler.h"
#include "ParamsDef.h"

namespace ecallapp {

class ECallTimerHandler : public TimerTimeoutHandler {
public:
    ECallTimerHandler()  {
        
    }
    void handlerFunction(const int32_t msgId) override {
        (void)(ECallAppHandler::getInstance()->obtainMessage(ENUM_TO_INT(msg::MSG::MSG_TIMER_SRV), msgId)->sendToTarget());
    }
    
};

} /* End: namespace ECALLDCMNONREG */
#endif // ECALLTIMERHANDLER_H
