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
 * @file        ConfigurationManagerAdapter.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef ECALLNONREG_CONFIGURATIONMANAGERADAPTER_H
#define ECALLNONREG_CONFIGURATIONMANAGERADAPTER_H

#include <services/ConfigurationManagerService/IConfigurationManagerReceiver.h>
#include <services/ConfigurationManagerService/IConfigurationManagerService.h>
#include <services/ConfigurationManagerService/ConfigManager.h>
#include <binder/IServiceManager.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>

#include "../utils/ECallAppHandler.h"
#include "logger.h"
#include "../utils/ServiceDeathRecipient.h"
#include "../utils/ParamsDef.h"

namespace ecallapp {

namespace ecall_config {

// Config for testing
static const std::string CONFIG_ENABLE = "TRUE";
static const std::string CONFIG_DISABLE = "FALSE";

static const int32_t CF_CBWT_TIME_MS_VALUE = 10000; //@sldd test
static const std::string CF_CBWT_TIME_NAME = "CBWT_TIME_NAME"; //@sldd test
static const std::string CF_CBWT_TIME_SHORTEN_ENABLE_VALUE = CONFIG_DISABLE; //@sldd test
static const std::string CF_CBWT_TIME_SHORTEN_ENABLE_NAME = "CBWT_TIME_SHORTEN_ENABLE_NAME"; //@sldd test

static const std::string CF_PHONE_NUMBER_VALUE = "0973795341"; //@todo Need to fixed default phone number at production
static const std::string CF_PHONE_NUMBER_NAME = "PHONE_NUMBER"; //@sldd test
static const std::string CF_PHONE_NUMBER_ENABLE_VALUE = CONFIG_DISABLE; //@sldd test
static const std::string CF_PHONE_NUMBER_ENABLE_NAME = "PHONE_NUMBER_ENABLE"; //@sldd test

static const std::string CF_CONFIG_VALUE_SUCCESS = "SUCCESS"; //@sldd test
static const std::string CF_CONFIG_VALUE_FAILURE = "FAILURE"; //@sldd test

static const std::string CF_DATA_TRANSMISSION_VALUE = CF_CONFIG_VALUE_SUCCESS; //@sldd test
static const std::string CF_DATA_TRANSMISSION_NAME = "DATA_TRANSMISSION_NAME"; //@sldd test
static const std::string CF_DATA_TRANSMISSION_ENABLE_VALUE = CONFIG_DISABLE; //@sldd test
static const std::string CF_DATA_TRANSMISSION_ENABLE_NAME = "DATA_TRANSMISSION_ENABLE_NAME"; //@sldd test
static const std::string CF_DATA_TRANSMISSION_SKIP = "DATA_TRANSMISSION_SKIP"; //@sldd test
static const std::string CF_DATA_TRANSMISSION_SKIP_VALUE = CONFIG_DISABLE; //@sldd test
static const std::string CF_RIM_RESPONSE_IGNORED = "RIM_RESPONSE_IGNORED"; //@sldd test

static const std::string CF_VOICE_TRANSMISSION_VALUE = CF_CONFIG_VALUE_SUCCESS; //@sldd test
static const std::string CF_VOICE_TRANSMISSION_NAME = "VOICE_TRANSMISSION_NAME"; //@sldd test
static const std::string CF_VOICE_TRANSMISSION_ENABLE_VALUE = CONFIG_DISABLE; //@sldd test
static const std::string CF_VOICE_TRANSMISSION_ENABLE_NAME = "VOICE_TRANSMISSION_ENABLE_NAME"; //@sldd test

static const std::string TRANSMISSION_STATE_INSERVICE = "IN_SERVICE"; //@sldd test
static const std::string TRANSMISSION_STATE_OUT_OF_SERVICE = "OUT_OF_SERVICE"; //@sldd test
static const std::string CF_DATA_TRANSMISSION_STATE_NAME = "VOICE_TRANSMISSION_STATE"; //@sldd test
static const std::string CF_DATA_TRANSMISSION_STATE_VALUE = TRANSMISSION_STATE_INSERVICE; //@sldd test

static const std::string CF_SERVER_ULR_ENABLE_NAME = "SERVER_ULR_ENABLE"; //@sldd test
static const std::string CF_SERVER_ULR_ENABLE_VALUE = CONFIG_DISABLE; //@sldd test
static const std::string CF_SERVER_ULR_NAME = "SERVER_ULR"; //@sldd test
static const std::string CF_SERVER_REAL_ENABLE_NAME = "SERVER_REAL_ENABLE"; //@sldd test
static const std::string CF_SERVER_REAL_ENABLE_VALUE = CONFIG_DISABLE; //@sldd test
static const std::string CF_TEST_SENDING_FUNCTION_NAME = "TEST_SENDING_FUNCTION"; //@sldd test

//@todo Need to fixed default phone number at production
static const std::string CF_VIN_DEFAULT = "LJCPCBLCX11000237"; //@sldd test
static const std::string CF_VIN = "VIN";

static const std::string CF_MSISDN_DEFAULT = "unknown";
static const std::string CF_MSISDN = "MSISDN";

}

class ConfigurationManagerAdapter
{

public:
    ConfigurationManagerAdapter();

    static ConfigurationManagerAdapter* getInstance();
    void registerService();
    void handleConfigDataChanged(const std::string& name, const std::string& value) noexcept;
    std::string getConfig(const std::string &name, const DataFrom& sourceType = DataFrom::General) const noexcept;
    void setConfig(const std::string &name, const std::string &value) noexcept;
    int32_t getConfigIntValue(const std::string &name) const noexcept;
    void setConfig(const std::string &name, const int32_t &value) noexcept;
    bool getPrivacy(const std::string &funcId, const bool& isACNUS = false) const;
    bool isSubscripted(const std::string& id) const;

private:
    android::sp<IConfigurationManagerService> getService();
    void onBinderDied(const android::wp<android::IBinder>& who);

private:
    static std::shared_ptr<ConfigurationManagerAdapter> instance;
    android::sp<IConfigurationManagerService> mConfigMgrService;
    android::sp<ECallAppHandler> mHandler;

    android::sp<ServiceDeathRecipient> mServiceDeathRecipient  { nullptr };
    android::sp<IConfigurationManagerReceiver> mConfigReceiver { nullptr };
    std::unordered_map<std::string, std::string > mConfigTable;
};

class ConfigReceiver : public BnConfigurationManagerReceiver {
public:
    explicit ConfigReceiver(ConfigurationManagerAdapter& pr) noexcept
        : RefBase(), BnConfigurationManagerReceiver(), parent(pr) {}

    void onConfigDataChanged(std::string& name, std::string& value) override;
    void onDataChanged(const std::string &pattern, const config::data_t &value) noexcept override;

private:
    ConfigurationManagerAdapter& parent;
};

} /* End: namespace ECALLDCMNONREG */
#endif // ECALLNONREG_CONFIGURATIONMANAGERADAPTER_H
