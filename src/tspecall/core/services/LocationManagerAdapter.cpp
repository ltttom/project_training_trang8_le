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
 * @file        LocationManagerAdapter.cpp
 * @author      lam.pham@lge.com
 * @version     1.0.00
*/
#include "LocationManagerAdapter.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <memory>
#include <services/LocationManagerService/ILocationManagerServiceType.h>
#include "tspecall/core/services/SystemPropertyAdapter.h"
#include "tspecall/core/utils/CommonFunctions.h"

// using namespace android;
#define GPS_CMD_LOC_PREFERRED 8
namespace ecallapp {

LocationManagerAdapter::LocationManagerAdapter()
{
    Json::Value gpsData;
    mGpsDataMap[Type::ACN]              = gpsData;
    mGpsDataMap[Type::CPD_Smartphone]   = gpsData;

    LOG_I(DLT_STRING("Initilize dummy data for type ACN and CPD_Smartphone"));
}

std::shared_ptr<LocationManagerAdapter> LocationManagerAdapter::instance {};
LocationManagerAdapter* LocationManagerAdapter::getInstance() {
    if (instance == nullptr) {
        instance = std::make_shared<LocationManagerAdapter>();
    }
    return instance.get();
}

void LocationManagerAdapter::registerService() {
    LOG_I(DLT_STRING("registerService"));
    mHandler = ECallAppHandler::getInstance();
    mServiceDeathRecipient = android::sp<ServiceDeathRecipient>{new ServiceDeathRecipient( [&] ( const android::wp<android::IBinder>& who ){
        LocationManagerAdapter::getInstance()->onBinderDied(who);
    })};
    if (mLocationservice != nullptr) {
        LOG_E(DLT_STRING("mLocationService nullptr"));
        mLocationservice = nullptr;
    }

    mLocationservice = android::interface_cast<ILocationManagerService> (android::defaultServiceManager()->getService(android::String16(LOCATION_SRV_NAME)));
    if (mLocationservice != nullptr) {
        if (android::OK == android::IInterface::asBinder(mLocationservice)->linkToDeath(mServiceDeathRecipient)) {
            if (mLocationservice->IsLocationStarted() == static_cast<uint8_t>(false))
            {
                (void)mLocationservice->setLocationStatus(LOCATION_ENABLE);
            }
            else
            {
                LOG_I(DLT_STRING("Location already enable"));
            }
        }
    }
    else {
        if (mHandler != nullptr) {
            (void)(mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_LOCATIONMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS));
        }
    }
}

android::sp<ILocationManagerService> LocationManagerAdapter::getLocationManagerService() {
    if (mLocationservice == nullptr) {
        mLocationservice = android::interface_cast<ILocationManagerService> (android::defaultServiceManager()->getService(android::String16(LOCATION_SRV_NAME)));
    }
    return mLocationservice;
}

void LocationManagerAdapter::onBinderDied(const android::wp<android::IBinder> &who) {
    LOG_I(DLT_STRING("onBinderDied"));
    NOTUSED(who);
    (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_LOCATIONMGR))->sendToTarget());
}

void LocationManagerAdapter::collectGPSData(const Type type) {
    LOG_I(DLT_STRING("Collecting GPS data"));
    if (mLocationservice != nullptr) {
        const android::sp<LocationData> lc = android::sp<LocationData>(new LocationData());
        uint32_t rsp_value;
        bool isChanged = false;
        error_t ret = mLocationservice->LocationLgeGnssCmd( GPS_CMD_LOC_PREFERRED , SUBCMD_GET_PREFERED_MODE,&rsp_value);
        if (ret != 1) {
            LOG_E(DLT_STRING("ret:"), DLT_INT(static_cast<int32_t>(ret)));
        }
        if(rsp_value == PREFER_ICB_POS) {
            mLocationservice->LocationLgeGnssCmd( GPS_CMD_LOC_PREFERRED , SUBCMD_SET_TSU_PREFERED ,&rsp_value);
            isChanged = true;
        }
        else {
            LOG_I(DLT_STRING("rsp_value:"), DLT_UINT(rsp_value));
        }


        if (mLocationservice->IsLocationDataAvailable() == 1) {
            (void)mLocationservice->getLocationData(lc);
        }
        else {
            (void)mLocationservice->getLastLocationData(lc);
            LOG_E(DLT_STRING("Location data is not avalable, use lastest location data"));
        }

        mGPSData.clear();
        mLocationDetail.clear();

        switch (type) {
        case Type::CPD_Smartphone:
        {
            setGPSData(type, lc);
        break;
        }
        case Type::ACN: // fall through
        {
            Json::Value cordinate;
            Json::Value velocity;
            Json::Value accuracy;
            std::string latitude = std::to_string(lc->locationData.latitude);
            std::string longitude = std::to_string(lc->locationData.longitude);
            uint32_t speed = static_cast<uint32_t>(lc->locationData.speed * 3.6);
            uint32_t pdop = calculateAccuracy(lc->locationData.PDOP);
            uint32_t hdop = calculateAccuracy(lc->locationData.HDOP);
            std::string courseHeading = std::to_string(lc->locationData.bearing);
            std::string timeStamp = CommonFunctions::convertTime(lc->locationData.timestamp);

            cordinate["latitude"]           = latitude;
            cordinate["longitude"]          = longitude;
            cordinate["datum"]              = "wgs84";
            cordinate["format"]             = "mas";
            velocity["value"]               = std::to_string(speed);
            velocity["unit"]                = "kph";
            accuracy["pdop"]                = pdop;
            accuracy["hdop"]                = hdop;

            mGPSData["coordinate"]     = cordinate;
            mGPSData["dtTime"]         = timeStamp;
            mGPSData["velocity"]       = velocity;
            mGPSData["courseHeading"]  = courseHeading;
            mGPSData["accuracy"]       = accuracy;

/**
 * @brief Altitude of the event location in meters.
 * 
 */
            mLocationDetail["altitude"]            = lc->locationData.altitude;

/**
 * @brief Heading of direction of travel of the vehicle in degrees.
 * 
 */
            mLocationDetail["azimuth"]             = courseHeading;
/**
 * @brief Accuracy of the location data in meters.
 * 
 */
            mLocationDetail["positioningaccuracy"] = lc->locationData.accuracy;

            SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::CORDINATE_LATITUDE, latitude);
            SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::CORDINATE_LONGITUDE, longitude);
            SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::VELOCITY_VALUE, std::to_string(speed));
            SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::VELOCITY_PDOP, std::to_string(pdop));
            SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::VELOCITY_HDOP, std::to_string(hdop));
            SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::VELOCITY_COURSEHEADING, courseHeading);
        break;
        }
        default:
        break;
        }

        if(isChanged) {
            mLocationservice->LocationLgeGnssCmd( GPS_CMD_LOC_PREFERRED , SUBCMD_SET_ICB_PREFERED ,&rsp_value);
        }
    } else {
        LOG_I(DLT_STRING("Locationservice is nullptr"));
    }
}

Json::Value LocationManagerAdapter::getGPSData() {
    return mGPSData;
}
Json::Value LocationManagerAdapter::getGPSData(const Type type) {
    auto it = mGpsDataMap.find(type);
    if (it == mGpsDataMap.end())
    {
        Json::Value emptyJson;
        LOG_E(DLT_STRING("There is no data for type "), DLT_STRING(CommonFunctions::enumToString(typeGPSString, type).c_str()));
        return emptyJson;
    }

    return it->second;
}

Json::Value LocationManagerAdapter::getLocationDetail(){
    return mLocationDetail;
}

std::string LocationManagerAdapter::getGPSBody()
{
    return mMSDBody;
}


float LocationManagerAdapter::validateCourseHeading(const float input, const int32_t precision)
{
    if (input < 0.0f || input >= 360.0f) {
        return 0.0f;
    }

    float factor = std::pow(10.0f, precision);
    float roundedValue = std::round(input * factor) / factor;

    if (roundedValue < 0.0f || roundedValue >= 360.0f) {
        return 0.0f;
    }

    return roundedValue;
}

int32_t LocationManagerAdapter::validateLatitudeOrLongitude(const double input, const std::string& coord)
{
    if (coord.empty())
    {
        LOG_E(DLT_STRING("Invalid coordinate: "), DLT_STRING(coord.c_str()), DLT_STRING(", return "), DLT_INT32(0));
        return GPS_Value::DEFAULT_INT32_VALUE;
    }

    auto it = GPS_Value::rangeMap.find(coord);
    if (it == GPS_Value::rangeMap.end())
    {
        LOG_E(DLT_STRING("Not supporting coordinate: "), DLT_STRING(coord.c_str()), DLT_STRING(", return "), DLT_INT32(0));
        return GPS_Value::DEFAULT_INT32_VALUE;
    }

    int32_t minRange = it->second.first;
    int32_t maxRange = it->second.second;
    int32_t result = static_cast<int32_t>(input * 3600000);
    if (result < minRange || result > maxRange)
    {
        LOG_I(DLT_STRING("Coordinate out of range. ("), DLT_INT32(minRange), DLT_STRING(", "), DLT_INT32(maxRange), DLT_STRING("), return "), DLT_INT32(0));
        return GPS_Value::DEFAULT_INT32_VALUE;
    }

    return result;
}

int32_t LocationManagerAdapter::validateLatitude(const double input) {
    auto result = static_cast<int32_t>(input * 3600000);
    if ( (result < -324000000) || (result > 324000000) ) {
        result = 0;
    }
    return result;
}

int32_t LocationManagerAdapter::validateLongtitude(const double input) {
    auto result =  static_cast<int32_t>(input * 3600000);
    if ( (result < -648000000) || (result > 648000000) ) {
        result = 0;
    }
    return result;
}

uint32_t LocationManagerAdapter::calculateAccuracy(const float input) {
{
    if(input < 0)
    {
        LOG_E(DLT_STRING("Accuracy is under lower bound, return to 0"));
        return 0U;
    }

    uint32_t result = static_cast<uint32_t>(input);
    if (result > 15U) 
    {
        LOG_I(DLT_STRING("Accuracy is over upper bound, return to 0"));
        return 0U;
    }

    return result;
}
}

void LocationManagerAdapter::setGPSData(const Type type, const android::sp<LocationData>& lc)
{
    auto it = mGpsDataMap.find(type);
    if (it == mGpsDataMap.end())
    {
        LOG_E(DLT_STRING("Unsupported setGPSData for type "), DLT_STRING(CommonFunctions::enumToString(typeGPSString, type).c_str()));
        return;
    }

    LOG_I(DLT_STRING("Set GPS Data with type: "), DLT_STRING(CommonFunctions::enumToString(typeGPSString, type).c_str()));

    // clean up previous data
    it->second.clear();

    Json::Value cordinate;
    cordinate[GPS_Value::latitude]      = validateLatitudeOrLongitude(lc->locationData.latitude, GPS_Value::latitude);
    cordinate[GPS_Value::longitude]     = validateLatitudeOrLongitude(lc->locationData.longitude, GPS_Value::longitude);
    cordinate[GPS_Value::datum]         = GPS_Value::datumValue;
    cordinate[GPS_Value::format]        = GPS_Value::formatValue;
    it->second[GPS_Value::coordinate]   = cordinate;
    
    Json::Value velocity;
    velocity[GPS_Value::value]          = static_cast<uint32_t>(lc->locationData.speed * 3.6);
    velocity[GPS_Value::unit]           = GPS_Value::unitValue;
    it->second[GPS_Value::velocity]     = velocity;

    Json::Value accuracy;
    accuracy[GPS_Value::pdop]           = calculateAccuracy(lc->locationData.PDOP);
    accuracy[GPS_Value::hdop]           = calculateAccuracy(lc->locationData.HDOP);
    it->second[GPS_Value::accuracy]     = accuracy;

    it->second[GPS_Value::dtTime]         = CommonFunctions::convertTime(lc->locationData.timestamp);
    it->second[GPS_Value::courseHeading]  = validateCourseHeading(lc->locationData.bearing, 1);
}

} /* End: namespace ECALLDCMNONREG */
