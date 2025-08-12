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
 * @file        ConfigurationManagerAdapter.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "ConfigurationManagerAdapter.h"
#include <fstream>      // std::ifstream
#include <json-for-modern-cpp/json.hpp>
#include <json/json.h>
#include "tspecall/core/utils/CommonFunctions.h"

namespace ecallapp {

ConfigurationManagerAdapter::ConfigurationManagerAdapter() {
    LOG_I(DLT_STRING("ConfigurationManagerAdapter::ConfigurationManagerAdapter"));
}

std::shared_ptr<ConfigurationManagerAdapter> ConfigurationManagerAdapter::instance {};
ConfigurationManagerAdapter* ConfigurationManagerAdapter::getInstance() {
    if (instance == nullptr) {
        instance = std::make_shared<ConfigurationManagerAdapter>();
    }
    return instance.get();
}

android::sp<IConfigurationManagerService> ConfigurationManagerAdapter::getService() {
    if (mConfigMgrService == nullptr) {
        mConfigMgrService = android::interface_cast<IConfigurationManagerService> (
                    android::defaultServiceManager()->getService(
                        android::String16(CONFIGURATION_SRV_NAME)
                        )
                    );
    }
    return mConfigMgrService;
}

void ConfigurationManagerAdapter::registerService() {
    LOG_I(DLT_STRING("registerService"));
    (void)getService();
    mHandler = ECallAppHandler::getInstance();
    mServiceDeathRecipient = android::sp<ServiceDeathRecipient>{new ServiceDeathRecipient( [&] ( const android::wp<android::IBinder>& who ){
        ConfigurationManagerAdapter::getInstance()->onBinderDied(who);
    })};
    if (mConfigReceiver == nullptr) {
        mConfigReceiver = android::sp<IConfigurationManagerReceiver>{new ConfigReceiver(*this)};
    }
    bool error {true};
    if( mConfigMgrService != nullptr) {
        int32_t result {static_cast<int32_t>(android::IInterface::asBinder(mConfigMgrService)->linkToDeath(mServiceDeathRecipient))};
        if (result == static_cast<int32_t>(android::OK) ) {
            if (mConfigMgrService->registerReceiver(DataFrom::General, "", mConfigReceiver) == android::OK) {
                error = false;
            }

            std::vector<std::string> patterns_to_be_listened {};
            for (const auto& it : mConfigTable) {
                patterns_to_be_listened.push_back(it.second);
            }

            result = static_cast<int32_t>(mConfigMgrService->registerDataChangedEvent(mConfigReceiver, patterns_to_be_listened));
            if (result == static_cast<int32_t>(android::OK) ) {
                LOG_D(DLT_STRING("Registed registerDataChangedEvent successfully"));
                error = false;
            }
            else {
                LOG_D(DLT_STRING("Cannot register data changed. result = "), DLT_INT(result));
            }
        }
    }

    if (error) {
        LOG_E(DLT_STRING("Registed Config Service failed"));
        if (mHandler != nullptr) {
            (void)(mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_CONFIGMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS));
        }
    }
}

// Tsp eCall currently didn't handle any config data changed event
void ConfigurationManagerAdapter::handleConfigDataChanged(const std::string& name, const std::string& value) noexcept {

}

std::string ConfigurationManagerAdapter::getConfig(const std::string &name, const DataFrom& sourceType) const noexcept
{
	LOG_I(DLT_STRING("getConfig: "), DLT_STRING(name.c_str()));
    std::string dfValue {};
    if(name == ecall_config::CF_CBWT_TIME_SHORTEN_ENABLE_NAME) {
        dfValue = ecall_config::CF_CBWT_TIME_SHORTEN_ENABLE_VALUE;
    }
    else if (name == ecall_config::CF_DATA_TRANSMISSION_ENABLE_NAME) {
        dfValue = ecall_config::CF_DATA_TRANSMISSION_ENABLE_VALUE;
    }
    else if (name == ecall_config::CF_DATA_TRANSMISSION_NAME) {
        dfValue = ecall_config::CF_DATA_TRANSMISSION_VALUE;
    }
    else if (name == ecall_config::CF_PHONE_NUMBER_ENABLE_NAME) {
        dfValue = ecall_config::CF_PHONE_NUMBER_ENABLE_VALUE;
    }
    else if (name == ecall_config::CF_PHONE_NUMBER_NAME) {
        dfValue = ecall_config::CF_PHONE_NUMBER_VALUE;
    } 
    else if (name == ecall_config::CF_VIN) {
        dfValue = ecall_config::CF_VIN_DEFAULT;
    }
    else if (name == ecall_config::CF_DATA_TRANSMISSION_SKIP) {
        dfValue = ecall_config::CF_DATA_TRANSMISSION_SKIP_VALUE;
    }
    else if (name == ecall_config::CF_MSISDN) {
        dfValue = ecall_config::CF_MSISDN_DEFAULT;
    } 
    else {
        LOG_D(DLT_STRING("An unused value"));
    }

    std::string val = "";
    try {
        ConfigResponse ret = ConfigManager::instance()->getData(sourceType, name);
        if (ret.result == ConfigResult::NoError) {
            val = ret.read();
	        LOG_I(DLT_STRING("success, val: "), DLT_STRING(val.c_str()));
        }
        else {
            val = dfValue;
	        LOG_I(DLT_STRING("fail, val: "), DLT_STRING(val.c_str()));
        }
    } catch (...) {
        val = dfValue;
	    LOG_I(DLT_STRING("exception, val: "), DLT_STRING(val.c_str()));
    }
 
    return val;
}

int32_t ConfigurationManagerAdapter::getConfigIntValue(const std::string &name) const noexcept
{
	LOG_I(DLT_STRING("getConfigIntValue: "), DLT_STRING(name.c_str()));
    int32_t res {-1};
    if(name == ecall_config::CF_CBWT_TIME_NAME) {
        res = ecall_config::CF_CBWT_TIME_MS_VALUE;
    }
    ConfigResponse ret = ConfigManager::instance()->getData(DataFrom::General, name);
    if (ret.result == ConfigResult::NoError) {
        const std::string val = ret.read();
        bool checkValid {false};
        res = CommonFunctions::stringToInt32(val, checkValid, res);
        if (!checkValid) {
            LOG_W(DLT_STRING("The input string is invalid"));
        }
        LOG_I(DLT_STRING("val: "), DLT_STRING(val.c_str()), DLT_STRING(", res: "),  DLT_INT(res));
    }
    else {
        LOG_I(DLT_STRING("fail, res: "), DLT_INT(res));
    }
    return res;
}

// Tsp eCall currently didn't set any config data
void ConfigurationManagerAdapter::setConfig(const std::string &name, const std::string &value) noexcept
{

}
// Tsp eCall currently didn't set any config data
void ConfigurationManagerAdapter::setConfig(const std::string &name, const int32_t &value) noexcept
{

}

void ConfigurationManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who) {
    LOG_I(DLT_STRING("onBinderDied"));
    NOTUSED(who);
    if (mHandler != nullptr) {
        (void)(mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_CONFIGMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS));
    }
}

void ConfigReceiver::onConfigDataChanged(std::string& name, std::string& value) {
    parent.handleConfigDataChanged(name, value);
}

void ConfigReceiver::onDataChanged(const std::string &pattern, const config::data_t &value) noexcept {

}

// http://vscb.lge.com:8080/cb/issue/21960128
// http://collab.lge.com/main/display/HONDATELE/Privacy+Setting
// http://avncb.lge.com:8080/cb/tracker/100852247
// 4. Telematics Service
// 4.4. Privacy Setting (PPP/GDPR)
bool ConfigurationManagerAdapter::getPrivacy(const std::string &funcId, const bool& isACNUS) const {
    bool ret{true};
    std::ifstream ifs("data/systemrw/ppp/setting/ppp-list.json");

    if (ifs) {
        try {
            Json::Value jsonRoot;
            const Json::CharReaderBuilder builder;
            std::string errmsg;
            if (Json::parseFromStream(builder, ifs, &jsonRoot, &errmsg)) {
                // isDSSEnabled = false (This country disallow PPP feature take control. PPP have no power here)
                // funcId doesn't exits in ppp-list.json (PPP doesn't control you function at that time)
                // forceON = true ( If your function has forceON flag = true, it can over ppp control)
                // allSetting = true & setting(funcId) = true (After all, if your function is normal, let check allSetting and the owner service setting)
                if (jsonRoot["isDSSEnabled"].asBool() == false) { // Privacy setting not control
                    ret = true;
                } else {
                    for (auto& s : jsonRoot["services"]) {
                        bool found {false};
                        for (auto& f : s["functions"]) {
                            if (f["funcId"].asString() == funcId) {
                                if (f["forceON"].asBool() == true) {
                                    ret = true;
                                } else {
                                    ret = isACNUS ? jsonRoot["allSetting"]["value"].asBool() 
                                                   : (s["setting"]["value"].asBool() && jsonRoot["allSetting"]["value"].asBool());
                                }
                                found = true; // Function ID found
                                break;
                            }
                        }
                        if (found) { // If found, exit the outer loop
                            break;
                        }
                    }
                }
            }
        } catch (...) {
            LOG_E(DLT_STRING("Exception occurred"));
        }
    } else {
        LOG_E(DLT_STRING("Can not check Privacy Setting"));
    }
    LOG_I(DLT_STRING("funcId: "), DLT_STRING(funcId.c_str()), DLT_STRING(", ret: "), DLT_STRING(ret ? "true" : "false"));
    return ret; // Single exit point
}

// 26MY spec: //http://avncb.lge.com:8080/cb/issue/29704670
// https://orca.rd.honda.co.jp/svn/TSU_26Model/26TSU/HMtoLG/Honda_SPEC(latest)/common/GTC_SPEC/GTC(26M_GEN3)_API/06-03_GTC_Vehicle_Connector_for_TSU_Gen3_IF_Definition_Order.xlsx
// https://orca.rd.honda.co.jp/svn/TSU_26Model/26TSU/HMtoLG/Honda_SPEC(latest)/common/GTC_SPEC/GTC(26M_GEN2+)_API/API/03_GTC_Vehicle_Connector_for_TSU_Gen2p5_IF_Definition_Order.xlsx
bool ConfigurationManagerAdapter::isSubscripted(const std::string& id) const {
    bool ret{false};
    std::ifstream ifs("/data/systemrw/tsu_provisioning_data.json");
    if (ifs) {
        try {
            nlohmann::json js = nlohmann::json::parse(ifs);
            // servicePackage Object
            // servicePackage.services services[] array
            // servicePackage.services[].id Mandatory String - 7 | 8 | 9 | 30 | 31 Service id.
            // - 7: Auto-Crash Notification (Heavy crash with Airbag) to PSAP
            // - 8: Emergency Call (emergency-call)
            // - 9: Roadside Assistance (roadside-call)
            // - 30: Auto-Crash Notification (Heavy crash with Airbag) to GTC
            // - 31: Auto-Crash Notification (Light crash w/o Airbag) to GTC
            // CoreECUへ移管に伴い、削除
            // servicePackage.services[].status Mandatory Boolean - true | false Service offer state.
            // - true : Service is available.
            // - false : Service is not available.
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
	LOG_I(DLT_STRING("funcId: "), DLT_STRING(id.c_str()), DLT_STRING(", ret: "),  DLT_STRING(ret ? "true" : "false"));
    return ret;
}

} /* End: namespace ECALLDCMNONREG */
