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
 * @file        ECallAppHandler.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef ECALLNONREG_UTILS_ECALLNONREGHANDLER_H
#define ECALLNONREG_UTILS_ECALLNONREGHANDLER_H

#include <utils/Handler.h>
#include <utils/Message.h>
#include "IECallAppIf.h"

namespace ecallapp {
class ECallAppHandler : public sl::Handler {
public:
    ECallAppHandler(sp<sl::SLLooper>& looper, IECallAppIf& app) noexcept :
        Handler(looper),
        mApplication(app) {}

    virtual void handleMessage(const sp<sl::Message>& handlemsg) override;
    void handleMessageInternal(const sp<sl::Message>& handlemsg);
    void init(ECallAppHandler* const) const;
    static ECallAppHandler* getInstance();

private:
    // mApplication is an object of ECallApp2 class, which would be used to handle
    // the messages from message queue.
    IECallAppIf& mApplication;
    static ECallAppHandler* instance;
};

} /* End: namespace ECALLDCMNONREG */
#endif // ECALLNONREG_UTILS_ECALLNONREGHANDLER_H
