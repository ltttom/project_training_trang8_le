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
 * @file        CommunicationManagerAdapter.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef ECALLNONREG_COMMUNICATIONMANAGERADAPTER_H
#define ECALLNONREG_COMMUNICATIONMANAGERADAPTER_H

#include <memory>
#include <services/CommunicationManagerService/CM_Protocol.h>
#include "services/CommunicationManagerService/ICommunicationManagerService.h"
#include "services/CommunicationManagerService/ICommunicationReceiver.h"
#include "services/CommunicationManagerService/CommunicationData.h"
#include <binder/IServiceManager.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include "../utils/ECallAppHandler.h"
#include "logger.h"
#include "../utils/ServiceDeathRecipient.h"
#include "../utils/ParamsDef.h"

namespace ecallapp {

class CommunicationManagerAdapter {

    class CommunicationReceiver : public BnCommunicationReceiver {
        public:
            explicit CommunicationReceiver(CommunicationManagerAdapter& commRcv) noexcept 
            : RefBase(), BnCommunicationReceiver(), mCommRcv(commRcv) {}
            virtual ~CommunicationReceiver() = default;

            CommunicationReceiver(const CommunicationReceiver& ) = default;
            CommunicationReceiver& operator=(const CommunicationReceiver& ) = default;

            CommunicationReceiver(CommunicationReceiver&& ) = default;
            CommunicationReceiver& operator=(CommunicationReceiver&& ) = default;

            virtual error_t onReceive(sp<CommunicationData>& commData) override {
                mCommRcv.onCommReceive(commData);
                return {E_OK};
            }
        private:
            CommunicationManagerAdapter& mCommRcv;
    };
public:
    CommunicationManagerAdapter();
    static CommunicationManagerAdapter* getInstance();
    void registerService();
    void requestSendACKToMCU(const uint32_t& prev, const uint32_t& curr);

private: 
    android::sp<ICommunicationManagerService> getService();
    void onBinderDied(const android::wp<android::IBinder>& who);
    void onCommReceive(const sp<CommunicationData>& commData);

private:
    static std::shared_ptr<CommunicationManagerAdapter> instance;
    android::sp<ECallAppHandler>           mHandler;
    android::sp<ServiceDeathRecipient>        mServiceDeathRecipient = nullptr;
    android::sp<ICommunicationReceiver>       mCommReceiver          = nullptr;
    android::sp<ICommunicationManagerService> mCommMgr               = nullptr;
};

} /* End: namespace ECALLDCMNONREG */
#endif // ECALLNONREG_COMMUNICATIONMANAGERADAPTER_H
