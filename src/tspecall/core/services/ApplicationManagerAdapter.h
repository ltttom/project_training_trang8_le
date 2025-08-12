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
 * @file        ApplicationManagerAdapter.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef ECALLNONREG_APPLICATIONMANAGERADAPTER_H
#define ECALLNONREG_APPLICATIONMANAGERADAPTER_H

#include <iostream>
#include <string>
#include <binder/IServiceManager.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <vector>
#include <map>
#include <mutex>
#include <queue>
#include <memory>

#include "Error.h"
#include "services/ApplicationManagerService/IApplicationManagerService.h" 
#include "services/ApplicationManagerService/IApplicationManagerServiceType.h"

#include "../utils/ECallAppHandler.h"
#include "logger.h"
#include "../utils/ServiceDeathRecipient.h"
#include "../utils/ECallTimerHandler.h"

namespace ecallapp {
    
    namespace FEATURE_NAMES {
        static const std::string HONDA_ACN_NA {"acn-us"};
        static const std::string HONDA_ACN_NA_INCOMING {"acn-us-incomming"};
        static const std::string HONDA_ACN_NA_CBWT {"acn-us-cbwt"};
        static const std::string HONDA_DESS_NA {"dess-us"};
        static const std::string HONDA_DESS_NA_INCOMING {"dess-us-incomming"};
        static const std::string HONDA_DESS_NA_CBWT {"dess-us-cbwt"};
        static const std::string HONDA_SOS_NA {"ecall-us"};
        static const std::string HONDA_SOS_NA_INCOMING {"ecall-us-incomming"};
        static const std::string HONDA_SOS_NA_CBWT {"ecall-us-cbwt"};
        static const std::string HONDA_ACN_WITH_GTC {"acn-phone"};
        static const std::string HONDA_RCALL_JP {"rcall-japan"};
        static const std::string HONDA_CPD_SMARTPHONE {"cpd_smartphone"};

        static const std::string ECALL_UAE {"ecall_uae"};
        static const std::string ACN_UAE {"acn_uae"};
        static const std::string ECALL_JP {"ecall_kj"};
        static const std::string ACN_JP {"acn_kj"};
    }

namespace APP_MGR_CONFIG {
    /* Timer Id */
    enum class TIMER_ID : int32_t
    {
        QUERY_FEATURE_TIMER,
        /* Last item */
        TIMER_NUM
    };

    namespace FEATURE_ACTIONS {
        static const std::string LAUNCH             { "LAUNCH" };
        static const std::string UPDATE             { "UPDATE" };
        static const std::string IGNORE             { "IGNORE" };
        static const std::string TRIGGER            { "TRIGGER" };
        static const std::string UNKNOWN            { "" };
    }

    static const std::string APP_NAME               { "oemcall" };

    static std::unordered_map<FeatureAction, std::string> featureActionMap 
    {
        { FeatureAction::LAUNCH,  FEATURE_ACTIONS::LAUNCH  },
        { FeatureAction::UPDATE,  FEATURE_ACTIONS::UPDATE  },
        { FeatureAction::IGNORE,  FEATURE_ACTIONS::IGNORE  },
        { FeatureAction::TRIGGER, FEATURE_ACTIONS::TRIGGER },
    };    
}

class AppMgrEventData : public android::RefBase {
public: 
    AppMgrEventData() = delete;
    AppMgrEventData(const std::string& strActionName, const std::string& strFeatureName, const std::string& strFeatureName2 = "") 
        : RefBase()
    {
        mFeatureAction = strActionName;
        mFeatureName   = strFeatureName;
        mFeatureName2   = strFeatureName2;
        (void)strActionName;
        (void)strFeatureName;
    } 

    AppMgrEventData(const FeatureAction& featureAction, const std::string& strFeatureName, const std::string& strFeatureName2 = "")
        : RefBase()
    {
        if(APP_MGR_CONFIG::featureActionMap.find(featureAction) != APP_MGR_CONFIG::featureActionMap.end())
        {
            mFeatureAction = APP_MGR_CONFIG::featureActionMap[featureAction];
        }
        else 
        {
            mFeatureAction = APP_MGR_CONFIG::FEATURE_ACTIONS::IGNORE;
        }
        mFeatureName = strFeatureName;
        mFeatureName2   = strFeatureName2;
        (void)featureAction;
        (void)strFeatureName;
    }
public:
    std::string mFeatureAction;
    std::string mFeatureName;
    std::string mFeatureName2;
};

class ApplicationManagerAdapter : public android::RefBase {
    class SystemPostReceiver : public BnSystemPostReceiver{
        public:
            SystemPostReceiver(ApplicationManagerAdapter &rReceiverMgr) noexcept 
                : RefBase(), BnSystemPostReceiver(), mrReceiverMgr(rReceiverMgr) {
            }
            virtual bool onSystemPostReceived(const sp<Post> &systemPost) override {
                return mrReceiverMgr.onSystemPostReceived(systemPost);
            }

        private:
            ApplicationManagerAdapter &mrReceiverMgr;
    };

    bool onSystemPostReceived(const android::sp<Post> &systemPost);

public:
    static ApplicationManagerAdapter* getInstance();

    void registerService();
    bool getBootComplete() const noexcept;
    
    void queryActionForFeature(const std::string& feature, const bool timeOutLimit = false);
    int32_t setFeatureStatus(const std::string& name, const bool& onOf);
    int32_t getFeatureStatus(const std::string& name);
    static bool isAvailableInRegion(const std::string feature);

    void doFeatureActionPerformed(const sp<sl::Message>& msg);
    void doHandlePostActionDelivered(const sp<sl::Message>& msg);
    void doHandlePostFeatureActionDelivered(const sp<sl::Message>& msg);
    void doHandlePostFeatureStatusChanged(const sp<sl::Message>& msg);
    void handleQueryActionForFeatureTimerTimeOut();
    void sendPostACNUS_ON_OFF(int32_t onOff);

private:
    ApplicationManagerAdapter();
    android::sp<IApplicationManagerService> getService();
    void onBinderDied(const android::wp<android::IBinder>& who);

private:
    static ApplicationManagerAdapter*       instance;
    android::sp<IApplicationManagerService> mAppManager       { nullptr };
    android::sp<ISystemPostReceiver>        mSystemReceiver   { nullptr };
    android::sp<ECallAppHandler>         mHandler;
    android::sp<ServiceDeathRecipient>      mServiceDeathRecipient;
    bool                                    mIsBootCompleted;

    ECallTimerHandler* mTimer2sQuerryActionForFeatureHandler{nullptr};
    android::sp<Timer> mTimer2sQuerryActionForFeature{nullptr};

    std::unordered_map<std::string, std::string> mWaitingList;
    std::string mWaitingFeature;
    bool mIsWaitingFeatureLaunched;
};

} /* End: namespace ECALLDCMNONREG */

#endif /* ECALLNONREG_APPLICATIONMANAGERADAPTER_H */
