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
 * @file        ECallAppHandler.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "ECallAppHandler.h"
#include "logger.h"

namespace ecallapp {
/*
 * handleMessage:
 * handle the message from the message queue.
 * It actually forward the message to the ECallApp2 class through the object mApplication.
*/
void ECallAppHandler::handleMessage(const sp<sl::Message>& handlemsg) {
    const int32_t what{handlemsg->what};
    mApplication.doECallhandler(what, handlemsg);
}

void ECallAppHandler::handleMessageInternal(const sp<sl::Message>& handlemsg) {
    const int32_t what{handlemsg->what};
    mApplication.doECallhandlerInternal(what, handlemsg);
}

void ECallAppHandler::init(ECallAppHandler* const Handler) const {
    LOG_I(DLT_STRING("ECallHandler::init"));
    instance = Handler;
}

ECallAppHandler* ECallAppHandler::instance {nullptr};
ECallAppHandler* ECallAppHandler::getInstance() {
    if(instance == nullptr) {
        LOG_I(DLT_STRING("ECallHandler::getInstance nullptr"));
    }
    return instance;
}

} /* End: namespace ECALLDCMNONREG */