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
 * @file        data_manager.cpp
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#include "rim_adapter.h"

#include <memory> // for smart pointer
#include <map>

// Service manager
#include <binder/IServiceManager.h>


// Remote If manager
#include <services/RemoteIfManagerService/RemoteIfManager.h>
#include <services/RemoteIfManagerService/IRemoteIfManagerService.h>
#include <services/RemoteIfManagerService/IRemoteIfManagerServiceType.h>
#include <services/RemoteIfManagerService/ESendType.h>
#include <services/RemoteIfManagerService/EOrderType.h>
#include <services/RemoteIfManagerService/EServerRespType.h>
#include <services/TimeManagerService/TimeManager.h>

#include "../utils/utils_common.h"


#include "../oem_call_app_constants.h"
#include "../logger.h"
#include <sstream>
#include <iomanip>
#include "data/data_builder_us.h"
#include "../message_defs.h"


namespace {

}

namespace oc {
    constexpr uint32_t REQ_SEND        = 10;
    constexpr uint32_t REQ_STOP        = 11;
    constexpr uint32_t REQ_REGISTER    = 12;
    constexpr uint32_t REC_RESP        = 20;
    constexpr uint32_t REC_BINDER_DIED = 21;

    std::unordered_map <uint32_t, std::string> msgName{
        {REQ_SEND        ,"REQ_SEND"        },
        {REQ_STOP        ,"REQ_STOP"        },
        {REQ_REGISTER    ,"REQ_REGISTER"    },
        {REC_RESP        ,"REC_RESP"        },
        {REC_BINDER_DIED ,"REC_BINDER_DIED" }
    };

    class ReqSendMsg: public RefBase {
    public:
        ReqSendMsg(const std::string &payload, ESendType type) : payload_(payload), type_(type){};
        std::string payload_;
        ESendType type_;
    };


    class RIMReceiver : public BnRemoteIfMgrReceiver {
        public:
            RIMReceiver(const android::sp<AdapterHandler> handler) : handler_(handler) {}
            virtual ~RIMReceiver() {}

        /**
         * @brief respCertiDownToApp
         * @param orderType
         * @param respCertiDownData
         */
        virtual void respCertiDownToApp(EOrderType orderType, sp<CRespCertiDownData>& respCertiDownData) {

        }

        /**
         * @brief respNetConnTestToApp
         * @param orderType
         * @param respNetConnTestData
         */
        virtual void respNetConnTestToApp(EOrderType orderType, sp<CRespNetConnTestData>& respNetConnTestData) {

        }

        /**
         * @brief respOrderToApp
         * @param orderType
         * @param respOrderData
         */
        virtual void respOrderToApp(EOrderType orderType, sp<CRespOrderData>& respOrderData) {

        }

        /**
         * @brief respReplyToApp
         * @param orderType
         * @param respReplyData
         */
        virtual void respReplyToApp(EOrderType orderType, sp<CRespReplyData>& respReplyData) {

        }

        /**
         * @brief respSendToApp
         * @param sendType
         * @param respSendData
         */
        virtual void respSendToApp(ESendType sendType, sp<CRespSendData>& respSendData) {
            sp<RIMResponseType> resp = new RIMResponseType();
            resp->sendType = sendType;
            resp->serverResponse = respSendData->getServerResponse();
            resp->responseBody = std::string((const char*)respSendData->getRespSendBody(), respSendData->getRespSendBodyLen());
            resp->errorCode = std::string((const char*)respSendData->getErrorCode(), respSendData->getErrorCodeLen());
            resp->errorMessage= std::string((const char*)respSendData->getErrorMessage(), respSendData->getErrorMessageLen());
            handler_->obtainMessage(REC_RESP, resp)->sendToTarget();
        }
        private:
            android::sp<AdapterHandler> handler_;
    };


/**
 * @brief This contains the private properties of DataManager class instance
 *
 */
struct RIMAdapter::Impl {
    android::sp<ServiceDeathRecipient>         spServiceDeathRecipient = nullptr;
    android::sp<AdapterHandler>                spRemoteIfHandler = nullptr;
    android::sp<OemCallHandler>                spOemCallHandler = nullptr; // handler for sending message to main thread
    android::sp<IRemoteIfManagerService>       spRIM       = nullptr;
    android::sp<IRemoteIfMgrReceiver>          spRIMReceiver       = nullptr;
    uint64_t sendNum = 0;

    void registerRIM() {
        LOG_I(DLT_STRING(std::string(__func__).c_str())); // print caller function
            // Register receiver for notification from RIM
        spRIM = android::interface_cast<IRemoteIfManagerService>(
             android::defaultServiceManager()->getService(
                 android::String16(REMOTEIF_SRV_NAME)
             )
         );
        bool error = true;

        if( spRIM != nullptr) {
            auto result = android::IInterface::asBinder(spRIM)->linkToDeath(spServiceDeathRecipient);
            if (result == android::OK &&
                spRIM->registerReceiverRemoteIfMgrReceiverRespSendToApp(spRIMReceiver, ESendType::ACN) == android::OK &&
                spRIM->registerReceiverRemoteIfMgrReceiverRespSendToApp(spRIMReceiver, ESendType::ROADSIDE_CALL) == android::OK
            )
                LOG_I(DLT_STRING("[RIM] Registed RIM Service"));
                error = false;
        }

        if (error) {
            LOG_E(DLT_STRING("[RIM] Cannot register RIM Service, try again after"), DLT_UINT32(self::SEND_RETRY_DELAY) );
            spRemoteIfHandler->sendMessageDelayed(
                spRemoteIfHandler->obtainMessage(REQ_REGISTER),
                self::SEND_RETRY_DELAY
            );
        }
    }

    /**
     * @brief This is the funtion to request RemoteIfManager to send data to GTC
     *
     * @return true Request succefully
     * @return false Request failed
     */
    void requestSend(const std::string& payload, ESendType type) {

        std::string eventTime = cv::Common::getUtcTime();
        android::sp< CReqSendData > reqSendData = new CReqSendData();
        LOG_I(DLT_STRING("[RIM] Start send data "),
               DLT_STRING(", EventTime = "), DLT_STRING(eventTime.c_str()),
               DLT_STRING(", EventTimeLength = "), DLT_UINT32(eventTime.length()),
               DLT_STRING(", payloadLength = "), DLT_UINT32(payload.length()),
               DLT_STRING(", sendNum = "), DLT_UINT32(sendNum)
        );
        reqSendData->setData(
            0,
            EResultStatusType::DEFAULT_VALUE,
            EResultReasonType::DEFAULT_VALUE,
            eventTime.length(),
            (uint8_t*)eventTime.c_str(),
            EContentType::APPLICATION_JSON,
            payload.length(),
            (uint8_t*)payload.c_str(),
            ++(sendNum)
        );
        RemoteIfManager::instance()->reqSendToServer(
            type,
            reqSendData
        );
    }

    void reportResponse(const android::sp<RIMResponseType> &resData) {
        spOemCallHandler->obtainMessage(msg::MSG_RIM_GTC_RESPONSE, resData)->sendToTarget();
    }

    void onMsg(const sp<sl::Message> &msg)
    {
        try {
            LOG_I(DLT_STRING("[RIM] Received msg = "), DLT_STRING(msgName[msg->what].c_str()));
        } catch (...) {
        }

        switch (msg->what)
        {
            case REQ_SEND: 
            {
                android::sp<ReqSendMsg> buff = nullptr;
                msg->getObject(buff);
                if (buff == nullptr) {
                    LOG_E(DLT_STRING("[RIM] Request is null"));
                }
                else
                {
                    requestSend(buff->payload_, buff->type_);
                }
            }
            break;

            case REQ_STOP:
            {
                sendNum = 0;
            }
            break;

            case REQ_REGISTER:
            {
                registerRIM();
            }
            break;

            case REC_RESP:
            {
                if (sendNum != 0) 
                {
                    android::sp<RIMResponseType> data = nullptr;
                    msg->getObject(data);
                    if (data == nullptr)
                    {
                        LOG_E(DLT_STRING("[RIM] Data is null"));
                    }
                    else {
                        LOG_I(DLT_STRING("[RIM] SendType = "),DLT_STRING(data->sendType.getStringEnum().c_str()));
                        LOG_I(DLT_STRING("[RIM] Server Resp= "),DLT_STRING(data->serverResponse.getStringEnum().c_str()));
                        LOG_I(DLT_STRING("[RIM] Response body = "),DLT_STRING(data->responseBody.c_str()));
                        LOG_I(DLT_STRING("[RIM] Error Code = "),DLT_STRING(data->errorCode.c_str()));
                        LOG_I(DLT_STRING("[RIM] Error Message = "),DLT_STRING(data->errorMessage.c_str()));
                        reportResponse(data);
                    }
                }
            }
            break;

            case REC_BINDER_DIED:
            {
                break;
            }

            default :
            {
                LOG_E(DLT_STRING("[RIM] Message not defined, MSGID = "), DLT_UINT32(msg->what));
            }
            break;
        }
    }

    void onBinderDied(const android::wp<IBinder> &who)
    {
        LOG_E(DLT_STRING("RemoteIfManager died"));
        sendNum = 0;
        spRemoteIfHandler->obtainMessage(REQ_REGISTER)->sendToTarget();
        spOemCallHandler->obtainMessage(msg::MSG_CM_BINDER_DIED, self::ServiceId::RIM)->sendToTarget();
    }
};

std::unique_ptr<RIMAdapter::Impl> RIMAdapter::impl_ = std::make_unique<RIMAdapter::Impl>();
RIMAdapter::RIMAdapter(android::sp<OemCallHandler> spHandler)
{
    static sp<sl::SLLooper> looper = new sl::SLLooper();
    looper->prepare();
    looper->start(false);
    impl_->spRemoteIfHandler = new AdapterHandler(looper);
    impl_->spRemoteIfHandler->registerHandler(
        [&](const sp<sl::Message> & msg) -> void
        {
            impl_->onMsg(msg);
        });

    impl_->spOemCallHandler = spHandler;
    impl_->spServiceDeathRecipient = new ServiceDeathRecipient();
    impl_->spServiceDeathRecipient->registerHandler(
        [&] (const android::wp<android::IBinder>& who) -> void
        {
            impl_->onBinderDied(who);
        });

    impl_->spRIMReceiver = new RIMReceiver(impl_->spRemoteIfHandler);
    impl_->registerRIM();
}

void RIMAdapter::send(const string &payload, ESendType type)
{
    android::sp<ReqSendMsg> buff = new ReqSendMsg (payload, type);
    impl_->spRemoteIfHandler->obtainMessage(REQ_SEND, buff)->sendToTarget();
}

void RIMAdapter::stop()
{
    impl_->spRemoteIfHandler->obtainMessage(REQ_STOP)->sendToTarget();
}

} // namespace
