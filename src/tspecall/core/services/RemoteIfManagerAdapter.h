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
 * @file        RemoteIfManagerAdapter.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include <services/RemoteIfManagerService/RemoteIfManager.h>
#include <services/RemoteIfManagerService/IRemoteIfManagerService.h>
#include <services/RemoteIfManagerService/IRemoteIfManagerServiceType.h>
#include <services/RemoteIfManagerService/ESendType.h>
#include <services/RemoteIfManagerService/EOrderType.h>
#include <services/RemoteIfManagerService/EServerRespType.h>
#include <services/TimeManagerService/TimeManager.h>
#include "../utils/ECallAppHandler.h"
#include "../utils/ECallTimerHandler.h"
#include "logger.h"
#include "../utils/ServiceDeathRecipient.h"
#include "../utils/ParamsDef.h"
#include "../../../message_defs.h"
#include <binder/IServiceManager.h>

namespace ecallapp {
    class RemoteIfManagerAdapter {
    public:
        RemoteIfManagerAdapter();
        ~RemoteIfManagerAdapter() = default;
        static RemoteIfManagerAdapter* getInstance();
        android::sp<IRemoteIfManagerService> getService();
        void registerService();
        void send(const std::string& payload, ESendType type);
        void stop() {sendNum = 0;}
        void onBinderDied(const android::wp<android::IBinder>& who);

    private:
        static std::shared_ptr<RemoteIfManagerAdapter> instance;
        android::sp<IRemoteIfMgrReceiver> mRimReceiver = nullptr;
        android::sp<ECallAppHandler> mHandler;
        android::sp<IRemoteIfManagerService> mRimMgr{nullptr};
        android::sp<ServiceDeathRecipient>   mServiceDeathRecipient = nullptr;
        uint64_t sendNum;
    };

    class RIMReceiver : public BnRemoteIfMgrReceiver {
    public:
        RIMReceiver(RemoteIfManagerAdapter& parrent) : mParrent(parrent) {}
        virtual ~RIMReceiver() {}
        virtual void respCertiDownToApp(EOrderType orderType, sp<CRespCertiDownData>& respCertiDownData) {}
        virtual void respNetConnTestToApp(EOrderType orderType, sp<CRespNetConnTestData>& respNetConnTestData) {}
        virtual void respOrderToApp(EOrderType orderType, sp<CRespOrderData>& respOrderData) {}
        virtual void respReplyToApp(EOrderType orderType, sp<CRespReplyData>& respReplyData) {}
        virtual void respSendToApp(ESendType sendType, sp<CRespSendData>& respSendData) {
            android::sp<oc::RIMResponseType> resp = new oc::RIMResponseType();
            resp->sendType = sendType;
            resp->serverResponse = respSendData->getServerResponse();
            resp->responseBody = std::string((const char*)respSendData->getRespSendBody(), respSendData->getRespSendBodyLen());
            resp->errorCode = std::string((const char*)respSendData->getErrorCode(), respSendData->getErrorCodeLen());
            resp->errorMessage= std::string((const char*)respSendData->getErrorMessage(), respSendData->getErrorMessageLen());
            ECallAppHandler::getInstance()->obtainMessage(ENUM_TO_INT(msg::MSG::MSG_RIM_GTC_RESPONSE), resp)->sendToTarget();
        }
        private:
            RemoteIfManagerAdapter& mParrent;
    };
}