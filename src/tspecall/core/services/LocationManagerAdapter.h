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
 * @file        LocationManagerAdapter.h
 * @author      lam.pham@lge.com
 * @version     1.0.00
*/
#ifndef ECALLNONREG_LOCATIONMANAGER_H
#define ECALLNONREG_LOCATIONMANAGER_H

#include <cstdint>
#include <json/json.h>
#include <services/LocationManagerService/ILocationManagerService.h>
#include <limits>

#include <binder/IServiceManager.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <unordered_map>
#include "Error.h"

#include "../utils/ECallAppHandler.h"
#include "logger.h"
#include "../utils/ServiceDeathRecipient.h"
#include "../utils/ParamsDef.h"

namespace ecallapp {

class LocationManagerAdapter
{
public:
    enum class Type : uint8_t {
        ACN,            // see http://avncb.lge.com:8080/cb/issue/31435922 
        CPD_Smartphone, // see http://avncb.lge.com:8080/cb/issue/32675669
    };

public:
    enum GPS_SUBCMD_SET_LOC_PREFERRED {   
        SUBCMD_SET_TSU_PREFERED = 0,
        SUBCMD_SET_ICB_PREFERED,
        SUBCMD_GET_PREFERED_MODE
    };

    enum PREFERRED_MODE{     
        PREFER_TSU_POS = 0,
        PREFER_ICB_POS,
        PREFER_MODE_MAX
    };

public:
    LocationManagerAdapter();
    static LocationManagerAdapter* getInstance();
    
    virtual void registerService();
/**
     * @brief Collects GPSData (in MSD)
     *
     * @details This function collects data from TSU when the call is triggered by Manual, ACN/DESS call and CPD alert. 
     * It will save GPS Data correspoinding Type of services.
     * 
     * @param Type is enum class including {ACN, CPD_Smartphone}
     */
    void collectGPSData(const Type type);
    /**
     * @brief Set GPSData (in MSD) associate to CollectionGPSData() function
     * 
     * @details This function set GPS Data into internal gpsDataMap by
     * a LocationData input from andorid service.
     *  
     * @param type the call type service ecallapp::Type, 
     * @param lc a strong pointer to LocationData class.
     */
    void setGPSData(const Type type, const android::sp<LocationData>& lc);
    /**
     * @brief Get GPSData (in MSD) 
     * 
     * This function get GPS data from internal gpsDataMap with specific type
     * of ecallapp. It return a GPS Data in Json Value for collecting in 
     * collectGPSData() fucntion.
     * @param This inputs a call type service ecallapp::Type.
     * @return This returns a Json::Value contain GPS Data
     */
    Json::Value getGPSData(const Type type);
    Json::Value getGPSData();
    /**
     * @brief getting GPS Location Detail (in MSD) at when the call is triggered by Manual and ACN/DESS Call 
     * @return This returns a Json::Value contain GPS Value including latitude, longitude,
     * altitude, timestamp , azimuth, positioning accuracy, acceleration, and speed
     */
    Json::Value getLocationDetail();

    std::string getGPSBody();

private:
    // Interfaces from ILocationManager
    virtual android::sp<ILocationManagerService> getLocationManagerService();

    void onBinderDied(const android::wp<android::IBinder>& who);

     /**
     * @brief Validate courseHeading (in MSD)
     * @details This function validates the input and warranty it in range(0, 359.9)
     * @param input float type The heading of direction of travel of the vehicle in degrees.
     * @param precision precision
     * @return a float value
     */
    float validateCourseHeading(const float input, const int32_t precision);
    
     /**
     * @brief Validate latitude and longitude in GPS Data (in MSD)
     * This function checks if the input GPS coordinates are valid.
     * @param input This is a double value for GPS coordinates 
     * @param coord Coordinate name (either "latitude" or "longitude")
     * @return a int32_t value
     */
    int32_t validateLatitudeOrLongitude(const double input, const std::string& coord);

    static int32_t validateLatitude(const double input);
    static int32_t validateLongtitude(const double input);
    /**
     * @brief Check accuracy of HDOP and PDOP in GPS Data
     * @details according to the definition in GPS_Value, the HDOP/PDOP
     * should be in the range [0, 15], otherwise return 0 as stated
     * in the requirement
     * @param Value of HDOP and PDOP in float type is associated
     * @return return integer part
     */
    static uint32_t calculateAccuracy(const float input);

private:
    static std::shared_ptr<LocationManagerAdapter> instance;
    android::sp<ECallAppHandler>      mHandler;
    android::sp<ServiceDeathRecipient>   mServiceDeathRecipient = nullptr;
    android::sp<ILocationManagerService> mLocationservice = nullptr;

    // Map to store GPS data for different types
    std::unordered_map<Type, Json::Value> mGpsDataMap;

    // Store string of call type
    std::unordered_map< Type, std::string > typeGPSString{
        {Type::ACN              , "ACN"             },
        {Type::CPD_Smartphone   , "CPD_Smartphone"  }
    };

    Json::Value mGPSData;
    Json::Value mLocationDetail;
    std::string mMSDBody;
};

} /* End: namespace ECALLDCMNONREG */
#endif // ECALLNONREG_LOCATIONMANAGER_H
