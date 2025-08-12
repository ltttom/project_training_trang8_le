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
 * @file        config_manager_adapter.h
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#ifndef __OEM_CALL_CONFIG_MANAGER_H__
#define __OEM_CALL_CONFIG_MANAGER_H__
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../message_defs.h"

namespace oc {

/**
* @brief This is the list of configuration which need to get from ConfigManager
*
*/
namespace ConfigName
{
    const std::string privacy              = "Privacy";
    const std::string primaryNumber        = "PrimaryNumber";
    const std::string secondaryNumber      = "SecondaryNumber";
    const std::string retryVoiceCallTime   = "RetryVoiceCallTime";
    const std::string retryAcnNotifyTime   = "RetryAcnNotifyTIme";
    const std::string maxAttemptVoiceCall  = "MaxAttemptVoiceCall";
    const std::string maxAttemptDataNotify = "MaxAttemptDataNotify";
    const std::string ecallSubcription     = "EcallSubcription";
    const std::string phoneSubcription     = "PhoneSubcription";
    const std::string phoneSubcriptionType = "PhoneSubcriptionType";
    const std::string appMode              = "AppMode";
    const std::string rcallContract        = "RcallContract";
};

/**
 * @brief
 * This is the class which implement the method to manage configuration.
 *
 */
class ConfigManager {
public:


    /**
     * @brief This is the types of configuration which need to update
     *
     */
    struct UpdateType {
        constexpr static uint32_t APP_MODE  = 1;
        constexpr static uint32_t ECALL     = 2;
        constexpr static uint32_t ACN_CALL  = 3;
        constexpr static uint32_t ACN_PHONE = 4;
    };

    ConfigManager();

    /**
     * @brief This is the method to request update configuration
     *
     */
    bool update(const uint32_t type);

    /**
     * @brief Get the configuration
     *
     * @param name
     * @return std::string
     */
    std::string getConfig(std::string name);

    void doHandleConfigChanged(const android::sp<ConfigNotiData>& data);
    bool getPrivacy(const string& id, bool isACNUS = false);
    bool isSubscripted(const string& id);
//    bool isSubscripted(const string& id);

private:
    /**
     * @brief This is the type of structure that contains the private
     *        properties of the instance.  It is defined in the implementation
     *        and declared here to ensure that it is scoped inside the class.
     */
    struct Impl;

    /**
     * @brief This contain private data of the instance
     *
     */
    std::shared_ptr<Impl> impl_;
};

} // namespace OEMCall App
#endif
