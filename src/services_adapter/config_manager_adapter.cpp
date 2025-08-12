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
 * @file        config_manager_adapter.cpp
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/

#include "config_manager_adapter.h"
#include "../logger.h"
#include <map>

#include <binder/IServiceManager.h>

/* Configuration Manager Service */
#include <services/ConfigurationManagerService/IConfigurationManagerReceiver.h>
#include <services/ConfigurationManagerService/IConfigurationManagerService.h>
#include <fstream>      // std::ifstream
#include <json-for-modern-cpp/json.hpp>
#include <json/json.h>


namespace oc {

/**
 * @brief This contains the private properties of ConfigManager class instance
 *
 */
struct ConfigManager::Impl {
    std::map < std::string, std::string > dfConfig {
        {oc::ConfigName::privacy             , "911"},
        {oc::ConfigName::primaryNumber       , "911"},
        {oc::ConfigName::secondaryNumber     , "TEST"},
        {oc::ConfigName::retryVoiceCallTime  , "30"},
        {oc::ConfigName::retryAcnNotifyTime  , "30"},
        {oc::ConfigName::maxAttemptVoiceCall , "3"},
        {oc::ConfigName::maxAttemptDataNotify, "4"},
        {oc::ConfigName::ecallSubcription    , "TEST"},
        {oc::ConfigName::phoneSubcription    , "ON"},
        {oc::ConfigName::phoneSubcriptionType, "Type1"},
        {oc::ConfigName::appMode             , "US"},
        {oc::ConfigName::rcallContract       , "ON"},
    };
    /**
     * @brief This is the instance of data structure which stored configuration queuried from db
     *
     */
    std::map < std::string, std::string > config;

    /**
     * @brief This is the pointer to configuration service
     *
     */
    android::sp<IConfigurationManagerService> spConfigMgrService;

    /**
     * @brief This is the method to get the configuration service pointer from
     *        service layer.
     */
    android::sp<IConfigurationManagerService> getConfigService() {
//        if (spConfigMgrService == nullptr) {
        spConfigMgrService = android::interface_cast<IConfigurationManagerService>(
                                android::defaultServiceManager()->getService(
                                    android::String16(CONFIGURATION_SRV_NAME)
                            )
        );
//        }
        return spConfigMgrService;
    }

    void setDefaultAllConfig() {
        LOG_I(
            DLT_STRING(std::string(__func__).c_str())
        );
        for (auto &x: dfConfig) {
            auto r = config.insert({x.first, x.second});
            if(r.second == false) {
                config[x.first] = x.second;
            }
        }
    }

    /**
     * @brief Set the configuration to default
     *
     */
    void setDefaultConfig(std::string name) {
        config[name] = dfConfig[name];
    }

    bool updateEcallConfig() {
        ProvisioningResponse response;
        #ifndef __OEM_CONFIG__
            response = getConfigService()->getData(DataFrom::Provisioning, "OEMCallECallUS");
            if(response.result == ConfigResult::NoError){
                auto setting = response.read();
                if(setting.find("enabled:true") != std::string::npos){
                    LOG_I(DLT_STRING("Saving configuration"));
                    for(auto& x : config) {
                        auto r = response.read(x.first.c_str());
                        x.second = r;
                    }
                }
                else {
                    LOG_I(DLT_STRING("Set default all config"));
                    setDefaultAllConfig();
                }
            }
            else {
                LOG_E(DLT_STRING("Cannot get configuration"));
                return false;
            }
            return true;
        #else
            for(auto& x : config) {
                auto name = x.first;
                response = getConfigService()->getData(DataFrom::General, name);
                if (response.result == ConfigResult::NoError){
                    x.second = response.read();
                    LOG_I(DLT_STRING("Config Name: "), DLT_STRING(name.c_str()), DLT_STRING("value: "), DLT_STRING(x.second.c_str()));
                }
                else {
                    LOG_E(DLT_STRING("Cannot get configuration for: "), DLT_STRING(name.c_str()));
                }
            }
//            validateConfig();
            return true;
        #endif
    }

    void validateConfig() {
        if (!isNumber(config[oc::ConfigName::retryVoiceCallTime])) {
            config[oc::ConfigName::retryVoiceCallTime] = "20";
        }
        if (!isNumber(config[oc::ConfigName::retryAcnNotifyTime])) {
            config[oc::ConfigName::retryAcnNotifyTime] = "30";
        }
        if (!isNumber(config[oc::ConfigName::maxAttemptVoiceCall])) {
            config[oc::ConfigName::maxAttemptVoiceCall] = "3";
        }
        if (!isNumber(config[oc::ConfigName::maxAttemptDataNotify])) {
            config[oc::ConfigName::maxAttemptDataNotify] = "4";
        }
    }

    bool isNumber (const std::string& s) {
        return !s.empty() && s.find_first_not_of("0123456789") == std::string::npos;
    }
};

ConfigManager::ConfigManager()
    :impl_(new Impl()) {
    LOG_I(DLT_STRING("Creating ConfigManagerAdapter ..."));
    impl_->setDefaultAllConfig();
}

bool ConfigManager::update(const uint32_t type) {
    LOG_I(DLT_STRING("Updating configuration"), DLT_UINT32(type));
    if (type == UpdateType::ECALL) {
        return impl_->updateEcallConfig();
    }
    else if (type == UpdateType::ACN_CALL) {
        return impl_->updateEcallConfig();
        // TODO: update ACN_CALL config
    }
    else if (type == UpdateType::ACN_PHONE) {
        return impl_->updateEcallConfig();
        // TODO: update ACN_PHONE config
    }
    else if (type == UpdateType::APP_MODE) {
        return impl_->updateEcallConfig();
        // TODO: update APP_MODE config
    }
    return true;

}

std::string ConfigManager::getConfig(std::string name) {
    return impl_->config[name];
}

bool ConfigManager::getPrivacy(const string &funcId, bool isACNUS) {
    bool ret = true;
        ifstream ifs("data/systemrw/ppp/setting/ppp-list.json");

        if (ifs) {
            try {
                Json::Value root;
                Json::CharReaderBuilder builder;
                string errmsg;
                if (Json::parseFromStream(builder, ifs, &root, &errmsg)) {

                    if (root["isDSSEnabled"].asBool() == false) // privacy setting not control
                        return true;

                    for (auto& s : root["services"]) {
                        for (auto& f : s["functions"]) {
                            if (f["funcId"].asString() == funcId) {
                                if (f["forceON"].asBool() == true)
                                    return true;
                                else {
                                    if (isACNUS) {
                                        ret = root["allSetting"]["value"].asBool();
                                    }
                                    else {
                                        ret = s["setting"]["value"].asBool() && root["allSetting"]["value"].asBool();
                                    }
                                    return ret;
                                }
                            }
                        }
                    }
                }
            } catch (...) {
                LOG_E(DLT_STRING(std::string(__func__).c_str()), DLT_STRING(": Exception occurred"));
            }

        } else {
            LOG_E(DLT_STRING(std::string(__func__).c_str()), DLT_STRING(": Can not check Privacy Setting"));
        }

        return ret;
}


bool ConfigManager::isSubscripted(const string& id) {
    bool ret = false;
    ifstream ifs("/data/systemrw/tsu_provisioning_data.json");
    if (ifs) {
        try {
            nlohmann::json js = nlohmann::json::parse(ifs);
    //        ConfigResponse cfgRes = impl_->getConfigService()->getJsonData(DataFrom::ProvisionedData, "/data/systemrw/tsu_provisioning_data.json");
            for (const auto& it : js.at("servicePackage").at("services")) {
                if (it.at("id") == id) {
                    ret = it.at("status");
                    break;
                }
            }
        } catch (...) {
            LOG_E(DLT_STRING("Cannot get subscription info for ID : "), DLT_STRING(id.c_str()));
        }

    }

    return ret;
}

void ConfigManager::doHandleConfigChanged(const android::sp<ConfigNotiData>& data) {
    LOG_I(DLT_STRING("Name: "), DLT_STRING(data->name.c_str()), DLT_STRING(", Value: "), DLT_STRING(data->value.c_str()));
    impl_->updateEcallConfig();
}

} // namespace oc
