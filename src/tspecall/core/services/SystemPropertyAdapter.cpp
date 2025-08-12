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
 * @file        SystemPropertyAdapter.cpp
 * @author      lam.pham@lge.com
 * @version     1.0.00
*/
#include <cstring>
#include <system_property.h>
#include <type_traits>

#include "logger.h"
#include "SystemPropertyAdapter.h"

namespace ecallapp {
const uint32_t BUFFER_LENGTH = 256U;

SystemPropertyAdapter::SystemPropertyAdapter() {
    LOG_D(DLT_STRING("SystemPropertyAdapter constructor"));
    mProperty = std::make_unique<SLProperty>("oemcall", true);
    if (mProperty == nullptr) {
        LOG_E(DLT_STRING("Could not init property of oemcall"));
    }
    mProperty->initPropertyCache();
}

std::shared_ptr<SystemPropertyAdapter>& SystemPropertyAdapter::getInstance()
{
    static std::shared_ptr<SystemPropertyAdapter> sSystemPropertyAdapterInstance(new SystemPropertyAdapter());
    return sSystemPropertyAdapterInstance;
}

void SystemPropertyAdapter::saveDataToProperty(const std::string &dataField, const std::string &value) {
    LOG_I(DLT_STRING("key ="), DLT_STRING(dataField.c_str()), DLT_STRING(",value ="), DLT_STRING(value.c_str()));
    if (mProperty != nullptr) {
        mProperty->setProperty(dataField.c_str(), value.c_str());
        bool checkPropertySync = mProperty->syncProperties();
        if (checkPropertySync == false) {
            LOG_E(DLT_STRING("Failed to sync properties"));
        }
    }
}

std::string SystemPropertyAdapter::getDataFromProperty(const std::string &dataField, std::string defaultValue) {
    LOG_I(DLT_STRING("get system property data for"), DLT_STRING(dataField.c_str()));
    if (!mProperty)
    {
        LOG_E(DLT_STRING("Property instance is nullptr, data default is "), DLT_STRING(defaultValue.c_str()));
        return defaultValue;
    }

    char* propertyResp = mProperty->getProperty(dataField.c_str());

    if (!propertyResp)
    {
        LOG_E(DLT_STRING("Property response is nullptr, data default is "), DLT_STRING(defaultValue.c_str()));
        return defaultValue;
    }

    std::string stringRet {propertyResp};
    if (stringRet.empty()) {
        stringRet = defaultValue;
    }

    LOG_I(DLT_STRING("Property data of "), DLT_STRING(dataField.c_str()), DLT_STRING(" is "), DLT_STRING(stringRet.c_str()));
    return stringRet;
}

} //namespace ecallapp
