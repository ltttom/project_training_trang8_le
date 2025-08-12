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
 * @file        DataBuilderUS.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "DataBuilderUS.h"
#include "../../../core/services/ConfigurationManagerAdapter.h"
#include "../../../core/services/PowerManagerAdapter.h"
#include "../../../core/services/TelephoneManagerAdapter.h"
#include "tspecall/core/services/LocationManagerAdapter.h"
#include "tspecall/core/services/VehicleManagerAdapter.h"
#include "tspecall/core/services/ApplicationManagerAdapter.h"
#include "logger.h"
namespace ecallapp{

struct DataBuilderUS::Impl {
    bool isNormalCDS = true;
    std::string ignition = "unknown";

    Json::Value root;

};

DataBuilderUS::DataBuilderUS()
    :impl_(new Impl())
{
}

std::string DataBuilderUS::getIgState(int32_t igState, int32_t accState){
    std::string ignition;
    if (igState == MCU_VALUE_STATUS::MCU_STATUS_OFF && accState == MCU_VALUE_STATUS::MCU_STATUS_OFF) {
        ignition = "ignitionOff";
    }
    else if (
        (igState == MCU_VALUE_STATUS::MCU_STATUS_ON && accState == MCU_VALUE_STATUS::MCU_STATUS_ON) ||
        (igState == MCU_VALUE_STATUS::MCU_STATUS_OFF && accState == MCU_VALUE_STATUS::MCU_STATUS_ON)
    ){
        ignition = "ignitionOn";
    }
    else if (
        (igState == MCU_VALUE_STATUS::MCU_STATUS_ON && accState == MCU_VALUE_STATUS::MCU_STATUS_OFF)) {
        ignition = "engineOn";
    }
    else {
        ignition = "unknown";
    }
    return ignition;
}

std::string DataBuilderUS::build() {
    LOG_I(DLT_STRING("Building payload"));
    impl_->root["ignition"] = impl_->ignition;

    impl_->root["gpsData"] = LocationManagerAdapter::getInstance()->getGPSData();

    auto crashTelemetry = VehicleManagerAdapter::getInstance()->getCrashTelemetry();

    // 1 for heavy CDS - 0 for light CDS
    // TSP call just suport hevy CDS
    crashTelemetry["crashLevel"]  = (impl_->isNormalCDS) ? "1" : "0";

    Json::Value uniqueIdentifier;
/**
 * @brief TCU ID
 * string
 */
    std::string tMSISDN = TelephoneManagerAdapter::getInstance()->getMsisdn();
    if (tMSISDN.empty()) {
        LOG_W(DLT_STRING("MSISDN is empty, using config value"));
        tMSISDN = ConfigurationManagerAdapter::getInstance()->getConfig(ecall_config::CF_MSISDN, DataFrom::Coding);
    }
    uniqueIdentifier["msisdn"] = tMSISDN;

/**
 * @brief Vehicle Identification Number
 * string
 */
    impl_->root["vin"] = ConfigurationManagerAdapter::getInstance()->getConfig(ecall_config::CF_VIN, DataFrom::Coding);
    impl_->root["crashTelemetry"] = crashTelemetry;
    impl_->root["locationDetails"] = LocationManagerAdapter::getInstance()->getLocationDetail();
/**
 * @brief The MSISDN of the TCU will be provided as the unique id for the communications.
 *
 */
    impl_->root["uniqueIdentifier"] = uniqueIdentifier;

    Json::StreamWriterBuilder stringBuilder;

    // "precision": int
    // Number of precision digits for formatting of real values.
    // https://open-source-parsers.github.io/jsoncpp-docs/doxygen/class_json_1_1_stream_writer_builder.html#details
    stringBuilder.settings_["precision"] = 5;
    stringBuilder.settings_["precisionType"] = "decimal";
    return Json::writeString(stringBuilder, impl_->root);
}

IDataBuilder &DataBuilderUS::setCDS(bool isNormalCDS) {
    impl_->isNormalCDS = isNormalCDS;
    VehicleManagerAdapter::getInstance()->collectCanData();
    return *this;
}

IDataBuilder &DataBuilderUS::setGPS() {
    LocationManagerAdapter::getInstance()->collectGPSData(LocationManagerAdapter::Type::ACN);
    return *this;

}

IDataBuilder &DataBuilderUS::setUtil(const std::string ignition) {
    impl_->ignition = ignition;
    return *this;
}

IDataBuilder &DataBuilderUS::setTriggerType(std::string triggerType, bool isDessCall) {
    LOG_D(DLT_STRING("Trigger Type:"), DLT_STRING(triggerType.c_str()));
    if (triggerType == FEATURE_NAMES::HONDA_SOS_NA) {
        impl_->root["sendTypeId"] = "SOS";
    } else if (triggerType == FEATURE_NAMES::HONDA_ACN_NA) {
        if (isDessCall == true) {
            impl_->root["sendTypeId"] = "DESS";
        } else {
            impl_->root["sendTypeId"] = "ACN";
        }
    } else {
        impl_->root["sendTypeId"] = "UNKNOWN";
    }
    return *this;
}

}
