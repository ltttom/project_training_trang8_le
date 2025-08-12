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
 * @file        RemoteIfManagerAdapter.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "RemoteIfManagerAdapter.h"
#include "../utils/CommonFunctions.h"

namespace ecallapp {
RemoteIfManagerAdapter::RemoteIfManagerAdapter() {
    /* Initialize data */
    sendNum = 0;
}

android::sp<IRemoteIfManagerService> RemoteIfManagerAdapter::getService() {
    if (mRimMgr == nullptr){
        mRimMgr = android::interface_cast<IRemoteIfManagerService>(android::defaultServiceManager()->getService(android::String16(REMOTEIF_SRV_NAME)));
    }
    return mRimMgr;
}

std::shared_ptr<RemoteIfManagerAdapter> RemoteIfManagerAdapter::instance {};
RemoteIfManagerAdapter* RemoteIfManagerAdapter::getInstance() {
    if (instance ==nullptr) {
        instance = std::make_shared<RemoteIfManagerAdapter>();
    }
    return instance.get();
}

void RemoteIfManagerAdapter::registerService() 
{
    LOG_I(DLT_STRING("registerService()"));
    mHandler = ECallAppHandler::getInstance();
    if (mRimMgr != nullptr) 
    {
        mRimMgr = nullptr;
        mRimReceiver = nullptr;
    }
    mServiceDeathRecipient = android::sp<ServiceDeathRecipient>{new ServiceDeathRecipient( [&] ( const android::wp<android::IBinder>& who ){
        RemoteIfManagerAdapter::getInstance()->onBinderDied(who);
    })};

    getService();
    bool isSuccess{false};
    if(mRimMgr != nullptr) {
        if (android::OK == android::IInterface::asBinder(mRimMgr)->linkToDeath(mServiceDeathRecipient)) {
            mRimReceiver = new RIMReceiver(*this);
            if (mRimMgr->registerReceiverRemoteIfMgrReceiverRespSendToApp(mRimReceiver, ESendType::ACN) == android::OK &&
                mRimMgr->registerReceiverRemoteIfMgrReceiverRespSendToApp(mRimReceiver, ESendType::ROADSIDE_CALL) == android::OK &&
                mRimMgr->registerReceiverRemoteIfMgrReceiverRespSendToApp(mRimReceiver, ESendType::CPD) == android::OK)
                {
                isSuccess = true;
            }
            else {} 
        }
        else {}
    } else {}
    if (!isSuccess) {
        if (mHandler != nullptr) {
            (void)(mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_RIMMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS));
        }
    }
}

void RemoteIfManagerAdapter::send(const std::string& payload, ESendType type) {
    std::string eventTime = CommonFunctions::getUtcTime();
    android::sp< CReqSendData > reqSendData = new CReqSendData();
	LOG_I(DLT_STRING("[RIM] Start send data, EventTime = "), DLT_STRING(eventTime.c_str()), 
    DLT_STRING(" , EventTimeLength = "),  DLT_UINT(eventTime.length()), 
    DLT_STRING(", payloadLength = "), DLT_UINT(payload.length()), 
    DLT_STRING(", sendNum ="),  DLT_UINT64(sendNum));
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
    RemoteIfManager::instance()->reqSendToServer(type,reqSendData);
}

void RemoteIfManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who) {
    LOG_I(DLT_STRING("onBinderDied"));
    (void)mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_RIMMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS);
}
}
