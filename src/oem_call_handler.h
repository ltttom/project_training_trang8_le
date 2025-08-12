/**
 * @copyright
 * Copyright (c) 2020 by LG Electronics Inc.
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
 * @file        oem_call_handler.h
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/

#ifndef __OEM_CALL_HANDLER_H__
#define __OEM_CALL_HANDLER_H__
#include <utils/Handler.h>
#include "oem_call_app_if.h"
#include <functional>
#include "logger.h"

namespace oc {

    class OemCallHandler : public sl::Handler {
        public:
            OemCallHandler(sp<sl::SLLooper>& looper, OEMCallAppIF& app): Handler(looper), app_(app) {}
            virtual ~OemCallHandler()=default;
            virtual void handleMessage(const sp<sl::Message>& msg);
        private:
            OEMCallAppIF& app_;
    };

    /**
     * @brief The AdapterHandler class Define a common interface to all AdapterHandler instance.
     *        It can be used to create handle object for specific adapter.
     */
    class AdapterHandler : public sl::Handler {
        public:
            AdapterHandler(sp<sl::SLLooper>& looper):Handler(looper)
            {
            }

            virtual ~AdapterHandler()
            {
            }

            virtual void handleMessage(const sp<sl::Message>& msg)
            {
                if (!onMsg_)
                {
                    //TODO: add one more property as a name to know which is which
                    LOG_E(DLT_STRING("handleMessage handler is not registered yet"));
                }
                else
                {
                    onMsg_(msg);
                }
            };

            void registerHandler(std::function<void(const sp<sl::Message>& msg)>&& onMsg)
            {
                LOG_I(DLT_STRING("AdapterHandler::registerHandler"));
                onMsg_ = onMsg;
            }

        private:
            std::function<void(const sp<sl::Message>& msg)> onMsg_;
    };

    /**
     * To handle when Binder communication is broken
     */
    class ServiceDeathRecipient : public android::IBinder::DeathRecipient{
        public:
            ServiceDeathRecipient() = default;
            virtual ~ServiceDeathRecipient() = default;

            void binderDied(const android::wp<android::IBinder>& who) override
            {
                if (!onBinderDied_)
                {
                    //TODO: add one more property as a name to know which is which
                    LOG_E(DLT_STRING("binderDied handler is not registered yet"));
                }
                else
                {
                    onBinderDied_(who);
                }
            }

            void registerHandler(std::function<void(const android::wp<android::IBinder>&)>&& onBinderDied)
            {
                LOG_I(DLT_STRING("ServiceDeathRecipient::registerHandler"));
                onBinderDied_ = onBinderDied;
            }

        private:
            std::function<void (const android::wp<android::IBinder>&)> onBinderDied_;
    };

}

#endif
