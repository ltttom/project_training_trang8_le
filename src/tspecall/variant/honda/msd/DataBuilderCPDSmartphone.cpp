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
 * @file        DataBuilderCPDSmartphone.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "DataBuilderCPDSmartphone.h"
#include "../../../core/services/LocationManagerAdapter.h"
#include "logger.h"

namespace ecallapp
{

std::string DataBuilderCPDSmartphone::build()
{
    LOG_I(DLT_STRING("Building payload for CPD smart phone"));
    m_root.clear();
     m_root["gpsData"] = LocationManagerAdapter::getInstance()->getGPSData(LocationManagerAdapter::Type::CPD_Smartphone);
    m_root["eventName"] = m_eventName;

    Json::StreamWriterBuilder stringBuilder;
    stringBuilder.settings_["precision"]        = 5;
    stringBuilder.settings_["precisionType"]    = "decimal";
    return Json::writeString(stringBuilder, m_root);
}

IDataBuilder& DataBuilderCPDSmartphone::setCDS(bool isNormalCDS)
{
    return *this;
}

IDataBuilder& DataBuilderCPDSmartphone::setGPS()
{
    LocationManagerAdapter::getInstance()->collectGPSData(LocationManagerAdapter::Type::CPD_Smartphone);
    return *this;
}

IDataBuilder& DataBuilderCPDSmartphone::setUtil(std::string eventName)
{
    m_eventName = eventName;
    return *this;
}

IDataBuilder& DataBuilderCPDSmartphone::setTriggerType(std::string triggerType, bool isDessCall) {
    (void)triggerType;
    (void)isDessCall;
    LOG_I(DLT_STRING("setTriggerType"));
    return *this;
}

} // </ecallapp>
