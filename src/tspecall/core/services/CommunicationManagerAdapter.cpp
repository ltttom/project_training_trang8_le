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
 * @file        CommunicationManagerAdapter.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include <memory>
#include <unistd.h>
#include "CommunicationManagerAdapter.h"
#include "tspecall/core/utils/CommonFunctions.h"

namespace ecallapp {

// Define Crash Signal
constexpr uint8_t CRASH_SIGNAL_LOW = 0b01000U;
constexpr uint8_t CRASH_SIGNAL_HIGH = 0b00100U;
constexpr uint8_t CRASH_SIGNAL_SLIGHT = 0b10000U;
// constexpr uint8_t CRASH_SIGNAL_NON_CRASH = 0b00010U;
// constexpr uint8_t CRASH_SIGNAL_ABNORMAL = 0b100000U;

CommunicationManagerAdapter::CommunicationManagerAdapter() {
}

std::shared_ptr<CommunicationManagerAdapter> CommunicationManagerAdapter::instance {};
CommunicationManagerAdapter* CommunicationManagerAdapter::getInstance() {
    if (instance == nullptr) {
        instance = std::make_shared<CommunicationManagerAdapter>();
    }
    return instance.get();
}

android::sp<ICommunicationManagerService> CommunicationManagerAdapter::getService() {
    if (mCommMgr == nullptr) {
        mCommMgr = interface_cast<ICommunicationManagerService> (defaultServiceManager()->getService(String16(COMMUNICATION_SRV_NAME)));
    }
    return mCommMgr;
}

void CommunicationManagerAdapter::registerService() {
    LOG_I(DLT_STRING("registerService"));
    mHandler = ECallAppHandler::getInstance();
    mServiceDeathRecipient = android::sp<ServiceDeathRecipient>{new ServiceDeathRecipient( [&] ( const android::wp<android::IBinder>& who ) {
        CommunicationManagerAdapter::getInstance()->onBinderDied(who);
    })};
    mCommMgr = nullptr;
    (void)getService();
    if (mCommReceiver == nullptr) {
        mCommReceiver = std::move(android::sp<ICommunicationReceiver>{new CommunicationReceiver(*this)});
    }
    bool error {true};
    if (mCommMgr != nullptr) {
        if (android::OK == android::IInterface::asBinder(mCommMgr)->linkToDeath(mServiceDeathRecipient)) {
            if (mCommMgr->registerReceiver(mCommReceiver, CATEGORY_AIRBAG) == android::OK) {
                LOG_D(DLT_STRING("Registed AirBag Receiver"));
                error = false;
            }
        }
    }
    if (error) {
        LOG_E(DLT_STRING("Cannot connect AirBag Receiver, try again after 500ms"));
        (void)(mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_COMMMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS));
    }
}

void CommunicationManagerAdapter::onCommReceive(const sp<CommunicationData>& commData) {
    LOG_I(DLT_STRING("onCommReceive"));

    if(commData == nullptr) {
        LOG_E(DLT_STRING("onCommReceive, commData is null"));
        return;
    }

    const uint8_t commCommand    {commData->cmd};
    if (commCommand == AIRBAG_CMD_NOTIFY_EVENT) {
        if (commData->payload->size() == 2U) {
            uint8_t crashType[2];
            (void) memcpy(crashType, commData->payload->data(), commData->payload->size());
            const uint8_t previousCrashType = crashType[0];
            const uint8_t currentCrashType  = crashType[1]; // previousCrashType + currentCrashType

            LOG_I(DLT_STRING("onCommReceive, previousCrashType = "), DLT_UINT8(previousCrashType), DLT_STRING(", currentCrashType = "),  DLT_UINT8(currentCrashType));

            // Heavy crash detection (LOW : 0b01000, HIGH : 0b00100)
            if ((previousCrashType & (CRASH_SIGNAL_LOW | CRASH_SIGNAL_HIGH)) != 0U ||
                (currentCrashType & (CRASH_SIGNAL_LOW | CRASH_SIGNAL_HIGH)) != 0U) {
                (void)(mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH), static_cast<int32_t>(previousCrashType), static_cast<int32_t>(currentCrashType))));
            }

            // Slight crash detection (SLIGHT crash: 0b10000)
            if ((previousCrashType & CRASH_SIGNAL_SLIGHT) != 0U ||
                (currentCrashType & CRASH_SIGNAL_SLIGHT) != 0U) {
                (void)(mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CDS_SLIGHT_CRASH), static_cast<int32_t>(previousCrashType), static_cast<int32_t>(currentCrashType))));
            }
        }
    }
}

void CommunicationManagerAdapter::requestSendACKToMCU(const uint32_t& prev, const uint32_t& curr) {
    if (mCommMgr == nullptr) {
        LOG_I(DLT_STRING("Comm is nullptr"));
    }
    else {
        sp<CommunicationData> data = new CommunicationData();
        data->type = static_cast<uint8_t>(TYPE_RESPONSE);
        data->cmd = AIRBAG_CMD_NOTIFY_EVENT;
        data->category = CATEGORY_AIRBAG;
        data->payload->setSize(2);
        uint8_t tmpData[2];
        tmpData[0] = CommonFunctions::uint32ToUint8(prev);
        tmpData[1] = CommonFunctions::uint32ToUint8(curr);
        (void)memcpy(data->payload->data(), tmpData, sizeof(tmpData));
        if (mCommMgr->sendDataToMcu(data) != E_OK) {
            LOG_I(DLT_STRING("send error"));
        }
        else {
            LOG_I(DLT_STRING("send ok"));
        }
    }
}

void CommunicationManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who) {
    LOG_I(DLT_STRING("Comm die, try again after 500ms"));
    NOTUSED(who);
    (void)(mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_COMMMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS));
}

} /* End: namespace ECALLDCMNONREG */
