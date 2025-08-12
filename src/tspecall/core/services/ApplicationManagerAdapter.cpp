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
 * @file        ApplicationManagerAdapter.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "ApplicationManagerAdapter.h"
#include "ServiceAdapterTimerId.h"
#include "../utils/CommonFunctions.h"
#include "logger.h"
#include "../utils/ParamsDef.h"

namespace ecallapp {

// remove all charaters <= space charater in the left side and right side of string
// example "  acn-us  " -> "acn-us"
static std::string trimWhiteSpace(std::string inputStr) {
    std::string res{inputStr};
    size_t leftIndex{0U};

    // remove all charaters <= space charater in the left side of string
    while ((leftIndex < inputStr.length()) && (static_cast<int32_t>(inputStr[leftIndex]) <= CHARACTER_SPACE_INT_VALUE)) {
        ++leftIndex;
    }
    size_t rigthIndex = inputStr.length();

    // remove all charaters <= space charater in the right side of string
    while ((rigthIndex > 0U) && (static_cast<int32_t>(inputStr[rigthIndex - 1U]) <= CHARACTER_SPACE_INT_VALUE) ) {
        --rigthIndex;
    }
    if (rigthIndex >= leftIndex) {
        res = inputStr.substr(leftIndex, rigthIndex - leftIndex);
    }
    return res;
}


ApplicationManagerAdapter::ApplicationManagerAdapter()
    : RefBase()
{
    /* Initialize data */
    mIsBootCompleted         = false;
    mWaitingFeature = "";
    mIsWaitingFeatureLaunched = false;
}

ApplicationManagerAdapter* ApplicationManagerAdapter::instance = nullptr;
ApplicationManagerAdapter* ApplicationManagerAdapter::getInstance()
{
    if (instance == nullptr) {
        instance = new ApplicationManagerAdapter();
    }
    return instance;
}

/**
 * getService:
 * Find service provided by ApplicationManagerAdapter.
 * Here the OEMCallApp plays the role as proxy.
 */
android::sp<IApplicationManagerService> ApplicationManagerAdapter::getService()
{
    if (mAppManager == nullptr) {
        mAppManager = android::interface_cast<IApplicationManagerService>
                  (
                    android::defaultServiceManager()->getService(android::String16("service_layer.ApplicationManagerService"))
                  );
    }
    return mAppManager;
}

void ApplicationManagerAdapter::registerService()
{
    LOG_I(DLT_STRING("registerService()"));
    mHandler = ECallAppHandler::getInstance();
    // Create a callback function, which is called when the binder is corrupted.
    mServiceDeathRecipient = android::sp<ServiceDeathRecipient>
                        {
                          new ServiceDeathRecipient( [&] ( const android::wp<android::IBinder>& who )
                          {
                            ApplicationManagerAdapter::getInstance()->onBinderDied(who);
                          })
                        };
    //http://collab.lge.com/main/display/HONDATSUBV/2.+priority+matrix+negotiation
    mTimer2sQuerryActionForFeatureHandler = new ECallTimerHandler();
    mTimer2sQuerryActionForFeature = new Timer(mTimer2sQuerryActionForFeatureHandler, static_cast<int32_t>(appmgr::TIMER_2S_QUERYACTIONFORFEATURE_ID));
    mTimer2sQuerryActionForFeature->setDurationMs(timeMsParams::TIMER_2S_QUERYACTIONFORFEATURE_DURATION, 0U);

    if (mAppManager != nullptr)
    {
        mAppManager = nullptr;
        mSystemReceiver = nullptr;
    }

    mAppManager = android::interface_cast<IApplicationManagerService> (
                                  android::defaultServiceManager()->getService(android::String16("service_layer.ApplicationManagerService")));
    if(mAppManager != nullptr) {
        if (android::OK == android::IInterface::asBinder(mAppManager)->linkToDeath(mServiceDeathRecipient)) {
            mSystemReceiver = new SystemPostReceiver(*this);
            if (mAppManager->getBootCompleted()) {
                if (mIsBootCompleted ==  false) {
                    if (mHandler != nullptr) {
                        LOG_I(DLT_STRING("mAppManager->getBootCompleted APPM_BOOT_COMPLETED"));
                        (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::APPM_BOOT_COMPLETED))->sendToTarget());
                        mIsBootCompleted = true;
                    }
                }
            }
            // Register system signal.
            // Application manager sends message to OEMCallApp by invoking this stub method.
            (void)mAppManager->registerSystemPostReceiver(mSystemReceiver, SYS_POST_ALL);
        }
    } else {
        if (mHandler != nullptr) {
            (void)(mHandler->sendMessageDelayed(mHandler->obtainMessage(
                                                                  ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_APPLICATIONMGR)),
                                                                  timeMsParams::TIME_SEND_RETRY_DELAY_MS));
        }
    }
}

bool ApplicationManagerAdapter::getBootComplete() const noexcept
{
    return mIsBootCompleted;
}

bool ApplicationManagerAdapter::onSystemPostReceived(const android::sp<Post> &systemPost)
{
    switch(systemPost->what) {
        case SYS_POST_BOOT_COMPLETED: {
            LOG_I(DLT_STRING("ApplicationManagerAdapter::SYS_POST_BOOT_COMPLETED: "), DLT_INT(systemPost->what));

            if (mIsBootCompleted == false) {
                if (mHandler != nullptr) {
                    (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::APPM_BOOT_COMPLETED))->sendToTarget());
                    mIsBootCompleted = true;
                }
            }
            break;
        }

        case SYS_POST_FEATURE_STATUS_CHANGED: {
            LOG_I(DLT_STRING("ApplicationManagerAdapter::SYS_POST_FEATURE_STATUS_CHANGED: "), DLT_INT(systemPost->what));

            if (mIsBootCompleted && (mHandler!=nullptr)) {
                (void)mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::APPM_FEATURE_STATUS_CHANGED), systemPost)->sendToTarget();
            }

            break;
        }

        case SYS_POST_FEATURE_ACTION_DELIVERED: {
            LOG_I(DLT_STRING("ApplicationManagerAdapter::SYS_POST_FEATURE_ACTION_DELIVERED: "), DLT_INT(systemPost->what));
            if (mIsBootCompleted && (mHandler!=nullptr)) {
                (void)mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::APPM_REQUEST_ACTIONS), systemPost)->sendToTarget();
            }
            break;
        }
        default: {
            LOG_I(DLT_STRING("ApplicationManagerAdapter::not handled: "), DLT_INT(systemPost->what));
            break;
        }
    }
    LOG_I(DLT_STRING("ApplicationManagerAdapter::onSystemPostReceived: "), DLT_INT(systemPost->what));
    return true;
}

/**
 * queryActionForFeature:
 * Ask ApplicationManager about the action to perform on a particular feature.
 */
void ApplicationManagerAdapter::queryActionForFeature(const std::string& feature, const bool timeOutLimit)
{
    LOG_I(DLT_STRING("queryActionForFeature, feature name = "), DLT_STRING(feature.c_str()));
    /* Check if service is valid */
    if (getService() != nullptr)
    {
        /* Query action for feature */
        const FeatureAction action {getService()->queryActionForFeature(feature)};
        mWaitingList.clear();
        mWaitingFeature = feature;
        mIsWaitingFeatureLaunched = false;
        LOG_I(DLT_STRING("feature = "), DLT_STRING(feature.c_str()), DLT_STRING(", action = "),  DLT_INT(action));
        switch(action)
        {
        case FeatureAction::LAUNCH:
        {
            mIsWaitingFeatureLaunched = true;
            android::sp<AppMgrEventData> pEventData {nullptr};
            pEventData = new AppMgrEventData(action, feature);
            (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::APPM_REQUEST_LAUNCH), pEventData)->sendToTarget());
            break;
        }
        case FeatureAction::IGNORE:
        {
            mIsWaitingFeatureLaunched = true;
            android::sp<AppMgrEventData> pEventData {nullptr};
            pEventData = new AppMgrEventData(action, feature);
            (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::APPM_REQUEST_IGNORE), pEventData)->sendToTarget());
            break;
        }

        case FeatureAction::POSTPONE:
        {
            break;
        }
        case FeatureAction::UPDATE:
        {
            if (timeOutLimit) {
                // http://collab.lge.com/main/display/HONDATSUBV/2.+priority+matrix+negotiation
                mTimer2sQuerryActionForFeature->stop();
                mTimer2sQuerryActionForFeature->start();
                mIsWaitingFeatureLaunched = false;
            }
            break;
        }
        case FeatureAction::TRIGGER:
        {
            break;
        }
        default:
            /* Do nothing */
            break;
        }
    }
    else
    {
        LOG_E(DLT_STRING("Service is NOT valid."));
    }
}

void ApplicationManagerAdapter::handleQueryActionForFeatureTimerTimeOut() {
    LOG_I(DLT_STRING("handleQueryActionForFeatureTimerTimeOut, mWaitingFeature: "), DLT_STRING(mWaitingFeature.c_str()));
    if(mIsWaitingFeatureLaunched == false) {
        //http://collab.lge.com/main/display/HONDATSUBV/2.+priority+matrix+negotiation
        //ecall feature has the highest priority, it waits for maximum 2s to operate in the case that other feature has the erro in resource release
        mIsWaitingFeatureLaunched = true;
        android::sp<AppMgrEventData> pEventData {nullptr};
        pEventData = new AppMgrEventData(FeatureAction::LAUNCH, mWaitingFeature);
        (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::APPM_REQUEST_LAUNCH), pEventData)->sendToTarget());
    }
}

int32_t ApplicationManagerAdapter::setFeatureStatus(const std::string& name, const bool& onOf){
    LOG_I(DLT_STRING("setFeatureStatus: "), DLT_STRING(name.c_str()),DLT_STRING(" = "),  DLT_STRING(onOf ? "On" : "Off"));
    int32_t retCode { TIGER_ERR::E_ERROR };
    retCode = getService()->setFeatureStatus(APP_MGR_CONFIG::APP_NAME, name, (onOf ? FeatureStatus::ON : FeatureStatus::OFF));
    return retCode;
}

int32_t ApplicationManagerAdapter::getFeatureStatus(const std::string& name) {
    int32_t value {FeatureStatus::OFF};
    value = getService()->getFeatureStatus(name);
    LOG_I(DLT_STRING("getFeatureStatus: "), DLT_STRING(name.c_str()), DLT_STRING(" = "),  DLT_INT(value));
    return value;
}


void ApplicationManagerAdapter::doFeatureActionPerformed(const sp<sl::Message>& msg) {
    const auto action = msg->arg1;
    std::string feature= "";
    if (!msg->buffer.empty()) {
        feature = trimWhiteSpace({ msg->buffer.data(),msg->buffer.data() + msg->buffer.size()});
    }
    LOG_I(DLT_STRING("action : "), DLT_INT(action), DLT_STRING(", feature "),  DLT_STRING(feature.c_str()));
    if (action == FeatureAction::UPDATE) {
        const android::sp<AppMgrEventData> pEventData = new AppMgrEventData(FeatureAction::UPDATE, feature);
        (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::APPM_REQUEST_TERMINATE), pEventData)->sendToTarget());
    }
}

void ApplicationManagerAdapter::doHandlePostActionDelivered(const sp<sl::Message>& msg) {
    sp<Post> post = nullptr;
    msg->getObject(post);
    if(post != nullptr) {
        const size_t length = post->buffer.size();
        const std::string who = std::string(reinterpret_cast<const char*>(post->buffer.data()), length);
        const auto action = CommonFunctions::enumToString(APP_MGR_CONFIG::featureActionMap, static_cast<FeatureAction>(post->arg1));
        LOG_I(DLT_STRING("feature name: "), DLT_STRING(who.c_str()), DLT_STRING(", action: "),  DLT_STRING(action.c_str()));
    }
}

void ApplicationManagerAdapter::doHandlePostFeatureActionDelivered(const sp<sl::Message>& msg) {
    LOG_I(DLT_STRING("doHandlePostFeatureActionDelivered"));
    android::sp<AppMgrEventData> pAppMgrEventData {nullptr};
    msg->getObject(pAppMgrEventData);
    if (pAppMgrEventData != nullptr) {
        LOG_I(DLT_STRING("feature name 1: "), DLT_STRING(pAppMgrEventData->mFeatureName.c_str()),
        DLT_STRING(", feature name 2: "),  DLT_STRING(pAppMgrEventData->mFeatureName2.c_str()));
        mWaitingList[pAppMgrEventData->mFeatureName] = pAppMgrEventData->mFeatureName2;
    }
}

void ApplicationManagerAdapter::doHandlePostFeatureStatusChanged(const sp<sl::Message>& msg) {
    sp<Post> post = nullptr;
    std::string changedFeatureName = "";

    try {
        msg->getObject(post);
    } catch (...) {
        post = nullptr;
    }

    if(post != nullptr) {
        try {
            const size_t length = post->buffer.size();
            changedFeatureName = std::string(reinterpret_cast<const char*>(post->buffer.data()), length);
        } catch (...) {
            LOG_E(DLT_STRING("Feature name uncorrect"));
            changedFeatureName = "";
        }
        changedFeatureName = trimWhiteSpace(changedFeatureName);
        const uint32_t featureState = static_cast<FeatureStatus>(post->arg1);
        LOG_I(DLT_STRING("changed feature name: "),
              DLT_STRING(changedFeatureName.c_str()),
              DLT_STRING( ", size: "),
              DLT_UINT(changedFeatureName.size()),
              DLT_STRING(", state: "),
              DLT_UINT(featureState),
              DLT_STRING("mWaitingList.size: "),
              DLT_UINT(mWaitingList.size()));

        if (featureState == FeatureStatus::OFF) {
            for (const auto& tPair : mWaitingList) {
                LOG_I(DLT_STRING("mWaitingList.first (waiting feature): "), DLT_STRING(tPair.first.c_str()));
                LOG_I(DLT_STRING("mWaitingList.second (terminated feature): "), DLT_STRING(tPair.second.c_str()));
                if ((tPair.first == mWaitingFeature) && (tPair.second == changedFeatureName) && (tPair.second != "")) {
                    if (mIsWaitingFeatureLaunched == true) {
                        LOG_I(DLT_STRING("curret feature is launched"), DLT_STRING(mWaitingFeature.c_str()));
                    }
                    else {
                        mIsWaitingFeatureLaunched = true;
                        const android::sp<AppMgrEventData> pEventData = new AppMgrEventData(FeatureAction::LAUNCH, tPair.first);
                        (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::APPM_REQUEST_LAUNCH), pEventData)->sendToTarget());
                        mWaitingList[tPair.first] = "";
                    }
                }
            }
        }
    }
}

void ApplicationManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who)
{
    LOG_I(DLT_STRING("onBinderDied"));
    NOTUSED(who);
    (void)mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_APPLICATIONMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS);
}

bool ApplicationManagerAdapter::isAvailableInRegion(const std::string feature)
{
    bool ret {false};
    const auto appMgr = android::interface_cast<IApplicationManagerService> (
        android::defaultServiceManager()->getService(
            android::String16("service_layer.ApplicationManagerService")));
    if (appMgr != nullptr) {
        ret = appMgr->isAvailableInRegion(feature);
    }
    LOG_I(DLT_STRING("isAvailableInRegion, feature: "), DLT_STRING(feature.c_str()), DLT_STRING(" = "),  DLT_STRING(ret ? "true" : "false"));
    return ret;
}

void ApplicationManagerAdapter::sendPostACNUS_ON_OFF(int32_t onOff)
{
    LOG_I(DLT_STRING("sendPostACNUS_ON_OFF, onOff: "), DLT_INT(onOff));
    if(getService() != nullptr) {
        const char* acn_us_name = "acn-us";
        const char* rma_name = "remotemanageapp";
        android::sp<Post> post = Post::obtain(POST_ACN_US_ON_OFF);
        post->arg1 = onOff;
        post->buffer.setTo((uint8_t*)acn_us_name, strlen(acn_us_name));
        getService()->sendPost(rma_name, post);
    } else {
        LOG_E(DLT_STRING("ApplicationManagerAdapter::sendPostACNUS_ON_OFF: Service is NOT valid."));
    }
}

} /* End: namespace ECALLDCMNONREG */
