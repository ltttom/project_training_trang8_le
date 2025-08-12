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
 * @file        app_manager_adapter.cpp
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/

#include <binder/IServiceManager.h>
#include <services/ApplicationManagerService/IApplicationManagerService.h>
#include "../logger.h"
#include "app_manager_adapter.h"
#include <deque>
#include <string>
#include <unordered_map>
#include "../oem_call_app_constants.h"
#include <list>
namespace {

    enum class FeatureState {
        Idle = 0,
        Requested,
        Waiting,
        Running
    };

    std::unordered_map<FeatureState, std::string> FeatureStateName{
        {FeatureState::Idle         , "IDLE"         },
        {FeatureState::Requested    , "REQUESTED"    },
        {FeatureState::Waiting      , "WAITING"      },
        {FeatureState::Running      , "RUNNING"      }
    };

    struct FeatureInfo {
        FeatureState state;
        std::string featureWaitingName = "";
        std::shared_ptr<oc::AppManagerAdapter::FeatureHandler> handler;
    };

}

namespace oc {

/**
 * @brief This contains the private properties of HmiManager class instance
 *
 */
struct AppManagerAdapter::Impl {

    /**
     * @brief features this is the list of features information.
     */
    std::list< std::shared_ptr< FeatureInfo > > features;

    /**
     * @brief This is the pointer to Application manager service
     *
     */
    android::sp<IApplicationManagerService> spAppMgrService;

    std::deque<std::pair<std::string, std::string>> waitingList;

//    std::unordered_map<std::string> runningFeatures;

    /**
     * @brief This is the method to get the configuration service pointer from
     *        service layer.
     */
    android::sp<IApplicationManagerService> getAppMgrService() {
//        if (spAppMgrService == nullptr) {
        spAppMgrService = android::interface_cast<IApplicationManagerService> (
            android::defaultServiceManager()->getService(
                android::String16("service_layer.ApplicationManagerService"))
        );
//        }
        return spAppMgrService;
    }

    std::string trimWhiteSpace(std::string s) {
        size_t i = 0;
        while (
            (i < s.length())
            && (s[i] <= 32)
        ) {
            ++i;
        }
        size_t j = s.length();
        while (
            (j > 0)
            && (s[j - 1] <= 32)
        ) {
            --j;
        }
        if (j>=i)
            return s.substr(i, j - i);
        else
            return "";
    }

    void requestStart(const std::shared_ptr<FeatureInfo>& info, const std::string type) {
        auto name = info->handler->featureName;
        if (!info->handler->onStart(type, name)){
            LOG_E(DLT_STRING("Cannot start feature, "), DLT_STRING(name.c_str()));
        }
    }

    void requestUpgrade(const std::shared_ptr<FeatureInfo>& info, const std::string type) {
        auto name = info->handler->featureName;
        if (!info->handler->onUpgrade(type, name)){
            LOG_E(DLT_STRING("Cannot upgrade feature, "), DLT_STRING(name.c_str()));
        }
    }

    void requestTrigger(const std::shared_ptr<FeatureInfo>& info, const std::string type) {
        auto name = info->handler->featureName;
        if (!info->handler->onTrigger(type, name)){
            LOG_E(DLT_STRING("Cannot trigger feature, "), DLT_STRING(name.c_str()));
        }
    }

    void requestStop(const std::shared_ptr<FeatureInfo>& info, const std::string type = "") {
        auto name = info->handler->featureName;
        if (!info->handler->onStop(type, name)){
            LOG_E(DLT_STRING("Cannot stop feature, "), DLT_STRING(name.c_str()));
        }
    }

    void onIgnore(const std::shared_ptr<FeatureInfo>& info, const std::string type = "") {
        auto name = info->handler->featureName;
        if (!info->handler->onIgnore(type, name)){
            LOG_E(DLT_STRING("Cannot stop ignored, "), DLT_STRING(name.c_str()));
        }
    }

    void requestACNPhonePostPonse (const std::shared_ptr<FeatureInfo>& info) {
        if (!info->handler->onPostponed(self::acn_phone_postponse, self::feature_acn_phone)){
            LOG_E(DLT_STRING("Cannot postponse feature"));
        }
    }

    void requestACNPhoneResume (const std::shared_ptr<FeatureInfo>& info) {
        if (!info->handler->onPostponed(self::acn_phone_resumse, self::feature_acn_phone)){
            LOG_E(DLT_STRING("Cannot resumse feature"));
        }
    }

    const std::shared_ptr<FeatureInfo> getFeatureInfo (const std::string featureName){
        LOG_I(DLT_STRING("Name = "), DLT_STRING(featureName.c_str()));
        for (auto& feature : features) {
            if (feature->handler->featureName == featureName) {
                return feature;
            }
        }
        LOG_E(DLT_STRING("Feature don't exist"));
        return nullptr;
    };

    void updateState(const std::shared_ptr<FeatureInfo>& info, FeatureState newState) {
        LOG_I(  DLT_STRING("Feature name: "), DLT_STRING(info->handler->featureName.c_str()),
                DLT_STRING(", Previos state: "), DLT_STRING(FeatureStateName.at(info->state).c_str()),
                DLT_STRING(", New state: "), DLT_STRING(FeatureStateName.at(newState).c_str())
        );
        info->state = newState;
    }

    void sendPostACNUS_ON_OF(int onOff){
        getAppMgrService();
        const char* acn_us_name = "acn_us";
        const char* rma_name = "remotemanageapp";
        android::sp<Post> post = Post::obtain(9999);
        post->arg1 = onOff; // 0: OFF, 1:ON
        post->buffer.setTo((uint8_t*)acn_us_name, strlen(acn_us_name));
        if (spAppMgrService != nullptr) {
            spAppMgrService->sendPost(rma_name, post);
        } else {
            LOG_I(DLT_STRING("spAppMgrService == nulll"));
        }
    }

};


AppManagerAdapter::AppManagerAdapter()
    :impl_(new Impl()){
    LOG_I(DLT_STRING("Creating AppManager Adapter ..."));
    impl_->features.clear();
    impl_->waitingList.clear();
}

void AppManagerAdapter::sendPostACNUS_ON_OF(int onOff){
    impl_->sendPostACNUS_ON_OF(onOff);
}

void AppManagerAdapter::addFeature(const std::shared_ptr <AppManagerAdapter::FeatureHandler> featureHandler) {
    LOG_I(DLT_STRING("Adding feature: "), DLT_STRING(featureHandler->featureName.c_str()));
    std::shared_ptr <FeatureInfo> info = std::make_shared < FeatureInfo > ();
    info->handler = featureHandler;
    info->state   = FeatureState::Idle;

    impl_->features.push_back(std::move(info));
}

void AppManagerAdapter::queryActionForFeature(const std::string name, const std::string type){
    LOG_I( DLT_STRING("name : "), DLT_STRING(name.c_str()), DLT_STRING("type : "), DLT_STRING(type.c_str()));
    if (impl_->getAppMgrService() == nullptr){
        return;
    }
    auto action = impl_->getAppMgrService()->queryActionForFeature(name);
    auto& info  = impl_->getFeatureInfo(name);
    if (info == nullptr) return;
    switch (action) {
    case serviceinfo::AppMgrRequestResult::LAUNCH:
        LOG_I(DLT_STRING("Starting : "), DLT_STRING(name.c_str()));
        impl_->requestStart(info, type);
        break;
    case serviceinfo::AppMgrRequestResult::UPDATE:
    case serviceinfo::AppMgrRequestResult::POSTPONE:
        LOG_I(DLT_STRING("Wating : "), DLT_STRING(name.c_str()));
        impl_->updateState(info, FeatureState::Waiting);
        impl_->waitingList.push_back({name, type});
        break;
    case serviceinfo::AppMgrRequestResult::TRIGGER:
    case serviceinfo::AppMgrRequestResult::IGNORE:
        impl_->onIgnore(info);
    default:
        break;
    }
}

void AppManagerAdapter::doHandleAppMgrResponse(const sp<sl::Message>& msg) {
    auto result = msg->arg1;
    std::string causeBy= "";
    if (!msg->buffer.empty()) {
        causeBy = impl_->trimWhiteSpace({ msg->buffer.data(),msg->buffer.data() + msg->buffer.size()});
    }
    LOG_I(DLT_STRING("result : "), DLT_UINT32(result), DLT_STRING("causeBy: "), DLT_STRING(causeBy.c_str()));

    if (
            result == serviceinfo::AppMgrRequestResult::UPDATE ||
            result == serviceinfo::AppMgrRequestResult::POSTPONE
    ){
        for (auto& feature: impl_->features) {
            if (feature->state == FeatureState::Waiting) {
                feature->featureWaitingName = causeBy;
            }
        }
    }
}

void AppManagerAdapter::doHandlePostFeatureStatusChanged(const sp<sl::Message>& msg) {
    sp<Post> post = nullptr;
    std::string changedFeatureName = "";

    try {
        msg->getObject(post);
    } catch (...) {
        post = nullptr;
    }

    if(post != nullptr) {
        try {
            changedFeatureName = (const char*) post->buffer.data();
        } catch (...) {
            LOG_E(DLT_STRING("Feature name uncorrect"));
            changedFeatureName = "";
        }
        uint32_t featureState = (FeatureStatus) post->arg1;
        LOG_I(
            DLT_STRING("changed feature name: "),
            DLT_STRING(changedFeatureName.c_str()),
            DLT_STRING("size:  "),
            DLT_UINT32(changedFeatureName.size()),
            DLT_STRING(", state: "),
            DLT_UINT32(featureState)
        );

        if (changedFeatureName == self::feature_acn_uae
                || changedFeatureName == self::feature_ecall_us
                || changedFeatureName == self::feature_acn_us) {
            auto& info = impl_->getFeatureInfo(self::feature_acn_phone);
            if (info != nullptr) {
                if (featureState == serviceinfo::SysPostFeatureStatus::ON){
                    impl_->requestACNPhonePostPonse(info);
                }
                else {
                    impl_->requestACNPhoneResume(info);
                }
            }
        }

        if (
            !impl_->waitingList.empty() &&
            featureState == serviceinfo::SysPostFeatureStatus::OFF
        ) {
            bool reset = false;
            for (auto& waitingInfo: impl_->waitingList) {
                auto feature = waitingInfo.first;
                auto type    = waitingInfo.second;

                auto& info = impl_->getFeatureInfo(feature);
                if (info != nullptr && info->featureWaitingName == changedFeatureName) {
                    if (reset) {
                        reset = true;
                        LOG_I(DLT_STRING("Starting : "), DLT_STRING(feature.c_str()));
                        impl_->requestUpgrade(info, type);
                    }
                    else {
                        LOG_I(DLT_STRING("Triggering: "), DLT_STRING(feature.c_str()));
                        impl_->requestTrigger(info, type); // TODO: FIXME
                    }
                    info->featureWaitingName = "";
                }
            }
            if (reset) impl_->waitingList.clear();
        }
    }
}

void AppManagerAdapter::doHandlePostActionDelivered(const sp<sl::Message>& msg) {
    sp<Post> post = nullptr;
    msg->getObject(post);
    if(post != nullptr) {
        const std::string who = (const char*) post->buffer.data();
        const FeatureAction action = (FeatureAction) post->arg1;

        LOG_I(
            DLT_STRING("feature name: "),
            DLT_STRING(who.c_str()),
            DLT_STRING("action :  "),
            DLT_UINT32(action)
        );
        if(action == FeatureAction::UPDATE) {
            for (auto& featureInfo : impl_->features) {
                auto name = featureInfo->handler->featureName;
                if (featureInfo->state == FeatureState::Running) {
                    if (self::updateMatrix.count(who  + ";" + name)) {
                        LOG_I(DLT_STRING("Terminating [") ,DLT_STRING(name.c_str()) ,DLT_STRING("] for [") ,DLT_STRING(who.c_str()) ,DLT_STRING("] to run") );
                        impl_->requestStop(featureInfo);
                    }
                }
            }
        }
    }
}

bool AppManagerAdapter::requestActiveFeature(const std::string name) {
    auto info = impl_->getFeatureInfo(name);
        if (
        info == nullptr ||
        impl_->getAppMgrService() == nullptr
    ) {
        return false;
    }
    LOG_I(DLT_STRING(name.c_str()), DLT_STRING(", Previous State: "), DLT_STRING(FeatureStateName.at(info->state).c_str()));
    #ifndef __UT_BUILD__
    uint32_t r = TIGER_ERR::E_ERROR;
    r = impl_->getAppMgrService()->setFeatureStatus("oemcall", name, FeatureStatus::ON);
    if (r != TIGER_ERR::E_OK) {
            LOG_E(
                DLT_STRING("Cannot request set feature state for ")
               ,DLT_STRING(name.c_str())
               ,DLT_STRING("AppMgr return : ")
               ,DLT_UINT32(r)
            );
            return false;
    }

    impl_->updateState(info, FeatureState::Running);
    #endif
    return true;
}

bool AppManagerAdapter::requestDeactiveFeature(const std::string name) {
    auto info = impl_->getFeatureInfo(name);
    if (
        info == nullptr ||
        impl_->getAppMgrService() == nullptr
    ) {
        return false;
    }
    LOG_I(DLT_STRING(name.c_str()), DLT_STRING(", State: "), DLT_STRING(FeatureStateName.at(info->state).c_str()));
    auto r = impl_->getAppMgrService()->setFeatureStatus("oemcall", name, FeatureStatus::OFF);
    if (r != TIGER_ERR::E_OK) {
            LOG_E(
                DLT_STRING("Cannot request set feature state for ")
               ,DLT_STRING(name.c_str())
               ,DLT_STRING("AppMgr return : ")
               ,DLT_UINT32(r)
            );
            return false;
    }
    impl_->updateState(info, FeatureState::Idle);
    return true;
}

bool AppManagerAdapter::getFeatureState(const std::string featureName) {
    auto info = impl_->getFeatureInfo(featureName);
    if (info == nullptr) return false;
    LOG_I(DLT_STRING(featureName.c_str()), DLT_STRING(", State: "), DLT_STRING(FeatureStateName.at(info->state).c_str()));
    if (info->state == FeatureState::Running) {
        return true;
    }
    return false;
}

bool AppManagerAdapter::isAvailableInRegion(const std::string feature)
{
    auto appMgr = android::interface_cast<IApplicationManagerService> (
        android::defaultServiceManager()->getService(
            android::String16("service_layer.ApplicationManagerService"))
    );
    if (appMgr != nullptr) {
        return appMgr->isAvailableInRegion(feature);
    }
    return false;
}

} // namespace oc
