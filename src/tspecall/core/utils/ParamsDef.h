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
 * @file        ParamsDef.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/

#ifndef ECALLNONREG_UTILS_PARAMSDEF_H
#define ECALLNONREG_UTILS_PARAMSDEF_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <functional>
#include "logger.h"

/* Cast int to enum */
#define INT_TO_ENUM(enumType, intVal)   (static_cast<enumType>(intVal))

namespace ecallapp {

/* Cast enum to int */
template <typename Type>
inline constexpr int32_t ENUM_TO_INT(const Type& enumVal) noexcept
{
    return static_cast<int32_t>(enumVal);
}

/* Cast enum to int */
template <typename Type>
inline constexpr uint8_t ENUM_TO_UINT8(const Type& enumVal) noexcept
{
    return static_cast<uint8_t>(enumVal);
}

/* Cast enum to int */
template <typename Type>
inline constexpr uint16_t ENUM_TO_UINT16(const Type& enumVal) noexcept
{
    return static_cast<uint16_t>(enumVal);
}

/* Cast enum to int */
template <typename Type>
inline constexpr uint32_t ENUM_TO_UINT32(const Type& enumVal) noexcept
{
    return static_cast<uint32_t>(enumVal);
}

/* Ignore warning about unused value */
template <typename Type>
inline void NOTUSED (const Type& val) noexcept
{
    static_cast<void>(val);
}

namespace self {
    const uint32_t DESS_UNDEFINDED = 10U;
    const uint32_t DESS_ECALL_NO_REQUEST = 0U;
    const uint32_t DESS_ECALL_REQUEST  = 1U;
    const uint32_t DESS_CANCEL_ECALL_TRIGGER  = 2U;
}

namespace msg {
    enum class INTERNAL_MSG_VALUE_BASE : int32_t
    {
        APP_MANAGER_SRV          = 0,
        AUDIO_MANAGER_SRV        = 1000,
        VEHICLE_MANAGER_SRV      = 2000,
        POWER_MANAGER_SRV        = 3000,
        TELEPHONY_MANAGER_SRV    = 4000,
        HMI_MANAGER_SRV          = 5000,
        COMM_MANAGER_SRV         = 6000,
        LOCATION_MANAGER_SRV     = 7000,
        TIMER_MANAGER_SRV        = 8000,
        CONFIG_MANAGER_SRV       = 9000,
        SOMEIP_MANAGER_SRV       = 11000,
        HTTPS_MANAGER_SRV        = 12000,
        REGION_MANAGER_SRV       = 17000,
        ECALL_MANAGER_SRV        = 21000,
        ALARM_MANAGER_SRV        = 22000,
        RIM_MANAGER_SRV          = 33000,
        TIMER_SRV                = 50000,
        /* For test ecalldcmnonreg: 60000*/
        SLDD_MANAGER_SRV         = 60000,
        /* For defind app internal mess*/
        APP_INTERNAL_SRV         = 100000,
    };

    enum class MSG : int32_t
    {
        // APP manager messages
        APPM_BOOT_COMPLETED = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::APP_MANAGER_SRV),
        APPM_REQUEST_LAUNCH,
        APPM_REQUEST_IGNORE,
        APPM_REQUEST_TERMINATE,
        APPM_REQUEST_ACTIONS,
        APPM_REQUEST_FEATURE_ACTION,
        RECV_MSG_CONNECT_TO_APPLICATIONMGR,
        RECV_MSG_BOOT_COMPLETED,
        APPM_FEATURE_STATUS_CHANGED,
        APPM_RESPONSE_QUERY,
        APPM_SLDD_SEND_POST,

        // AUDIO message
        RECV_MSG_CONNECT_TO_AUDIOMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::AUDIO_MANAGER_SRV),
        RECV_MSG_AUDIO_PLAY_STATE_CHANGED,
        RECV_MSG_AUDIO_PLAYING_STATE,
        RECV_MSG_AUDIO_STOPPED_STATE,
        MSC_CM_US_REQUEST_RETRY_PLAY_AUDIO,

        // VCM message
        RECV_MSG_CONNECT_TO_VCMMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::VEHICLE_MANAGER_SRV),
        RECV_MSG_SRS_SIGNAL,
        RECV_MSG_SRS_ECALL_TRIGGER,
        RECV_MSG_SRS_ECALL_TRIGGER_ACN_WITH_GTC,
        MES_STATUS_NOTIFICATION,
        MES_RECEIVED_VALUE,
        MES_ECALL_STATUS_NOTIFICATION,
        MES_ECALL_RECEIVED_VALUE,
        REQ_SEND_BCAN_CANCEL_DISP,
        REQ_CPA_SMARTPHONE_ALERT,

        // PWMGR message
        RECV_MSG_CONNECT_TO_POWERMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::POWER_MANAGER_SRV),
        RECV_MSG_IGN_ON,
        RECV_MSG_IGN_OFF,
        RECV_MSG_POWER_MODE_CHANGE,
        RECV_MSG_POWERMGR_BUB_MODE,
        RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED,
        MSG_POWER_STATE_CHANGED,
        MSG_POWER_MODE_CHANGED,

        // Telephone message
        RECV_MSG_CONNECT_TO_TELEPHONEMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::TELEPHONY_MANAGER_SRV),
        RECV_MSG_TELE_CALL_STATE_CHANGED,

        // 2025-04-03 added internal message
        RECV_MSG_INCOMING_CALL_TRIGGER,
        RECV_MSG_CALL_CONNECTED,
        RECV_MSG_CALL_DISCONNECTED,

        // HMI message
        RECV_MSG_CONNECT_TO_HMIMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::HMI_MANAGER_SRV),
        RECV_MSG_FROM_HMIMGR,
        RECV_MSG_SOS_BUTTON_PRESSED,       //!< User press SOS button
        RECV_MSG_SOS_BUTTON_STUCKED,
        RECV_MSG_SOS_BUTTON_LONG_PRESSED,  //!< For AU SOS Cancellation case (5-DAP-3-2 / 6.2.6)
        RECV_MSG_SOS_BUTTON_SHORT_PRESSED,
        RECV_MSG_SOS_BUTTON_NO_PRESSED,
        RECV_MSG_MANUAL_ECALL,
        RECV_MSG_MANUAL_CANCEL_ECALL,
        MSG_TURN_OFF_LED,
        RCALL_TURNOFF,
        RCALL_END_ERROR_LED,
        RECV_MSG_HMI_BUTTON_SHORT_PRESS,
        RECV_MSG_HMI_BUTTON_LONG_PRESS,

        // Comm message
        RECV_MSG_CONNECT_TO_COMMMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::COMM_MANAGER_SRV),
        RECV_MSG_AIRBAG_CRASH,
        RECV_MSG_CDS_HEAVY_CRASH,
        RECV_MSG_CDS_HEAVY_CRASH_ACN_WITH_GTC,
        RECV_MSG_CDS_SLIGHT_CRASH,
        RECV_MSG_CDS_SLIGHT_CRASH_ACN_WITH_GTC,
        // RECV_MSG_CDS_NON_CRASH,
        // RECV_MSG_CDS_ABNORMAL_CRASH,

        //
        RECV_MSG_CONNECT_TO_LOCATIONMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::LOCATION_MANAGER_SRV),

        // Configuration message
        RECV_MSG_CONNECT_TO_CONFIGMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::CONFIG_MANAGER_SRV),
        RECV_MSG_FROM_CONFIGMGR_CONFIG_CHANGED,

        // HTTPs message
        RECV_MSG_CONNECT_TO_HTTPSMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::HTTPS_MANAGER_SRV),
        RECV_MSG_RECEIVED_DATA_NOTIFICATION,
        RECV_MSG_RECEIVED_DATA,
        RECV_MSG_MSD_SUCCESS,
        RECV_MSG_MSD_FAILURE,

        // ECallMgr message
        RECV_MSG_CONNECT_TO_ECALLMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::ECALL_MANAGER_SRV),
        RECV_MSG_ECALL_STATE_INITIATE,
        RECV_MSG_ECALL_STATE_IDLE,
        RECV_MSG_ECALL_STATE_PREPARE,
        RECV_MSG_ECALL_STATE_RUNNING,
        RECV_MSG_ECALL_STATE_STANDBY,
        RECV_MSG_ECALL_STATE_PREPAREDATA,
        RECV_MSG_ECALL_STATE_DATACONNECTED,
        RECV_MSG_ECALL_STATE_CALLRETRY,
        RECV_MSG_ECALL_STATE_CREATEDATA,
        RECV_MSG_ECALL_EVENT_CALL_CONNECTED,
        RECV_MSG_ECALL_EVENT_CALL_FAILURE,

        // Alarm Mgr message
        RECV_MSG_CONNECT_TO_ALARMMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::ALARM_MANAGER_SRV),
        RECV_MSG_CBWT_TIMER_EXPIRED,

        // Region Mgr message
        RECV_MSG_CONNECT_TO_REGIONMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::REGION_MANAGER_SRV),

        // Rim
        RECV_MSG_CONNECT_TO_RIMMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::RIM_MANAGER_SRV),
        MSG_RIM_GTC_RESPONSE,

        // Some Ip
        RECV_MSG_CONNECT_TO_SOMEIPMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::SOMEIP_MANAGER_SRV),
        MSG_SOMEIP_PROVIDER_ETH_READY,
        MSG_SOMEIP_REQUEST_CLEARUP_R_CALL,

        // Time
        RECV_MSG_CONNECT_TO_TIMEMGR = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::TIMER_SRV),
        MSG_TIMER_SRV,
        RECV_MSG_ON_TIME_RECEIVED,

        // SLDD message
        RECV_MSG_SLDD_TEST_MESSAGE = ENUM_TO_INT(INTERNAL_MSG_VALUE_BASE::SLDD_MANAGER_SRV),
        RECV_MSG_SLDD_MANAGER_SRV_END,
        MESS_RESET_ECALL_SLDD,
        MESS_TEST_RECOVER_ECALL_SLDD,
    };
}

namespace timeMsParams {
    constexpr static uint32_t TIME_SEND_RETRY_DELAY_MS {500U};
    constexpr static uint32_t SRS_SEPARATING_DURATION_MS {60000U};
    constexpr static uint32_t TIMER_2S_QUERYACTIONFORFEATURE_DURATION {2000U}; //http://collab.lge.com/main/display/HONDATSUBV/2.+priority+matrix+negotiation
}

namespace SystemProperty {
    constexpr const char* CORDINATE_LATITUDE = "Cordinate.Latitude";
    constexpr const char* CORDINATE_LONGITUDE = "Cordinate.Longitude";
    constexpr const char* VELOCITY_VALUE = "Velocity.Value";
    constexpr const char* VELOCITY_PDOP = "Velocity.Pdop";
    constexpr const char* VELOCITY_HDOP = "Velocity.Hdop";
    constexpr const char* VELOCITY_COURSEHEADING = "Velocity.CourseHeading";
    constexpr const char* CRASH_LOCATION_FRONTAL_CRASH_SENSOR = "FrontalCrashSensor";
    constexpr const char* CRASH_LOCATION_SIDE_CRASH_SENSOR = "SideCrashSensor";
    constexpr const char* CRASH_LOCATION_REAR_END_CRASH_SENSOR = "RearEndCrashSensor";

    // at signal detecting time MSD data (SRS_PS_OCCUPIED, SRS_DR_BELT_STATUS, SRS_PS1_BELT_STATUS)
    constexpr const char* CRASH_VEHICLE_SRS_PS_OCCUPIED = "SrsPsOccupied";
    constexpr const char* CRASH_VEHICLE_SRS_DR_BELT_STATUS = "SrsDrBeltStatus";
    constexpr const char* CRASH_VEHICLE_SRS_PS1_BELT_STATUS = "SrsPs1BeltStatus";

    // telNo
    constexpr const char* TEL_NO = "TelNo";

    constexpr const char* ECALL_STATE = "ECall State";
    constexpr const char* CBWT_START_TIME = "CBWT.Start.Time";
    constexpr const char* CBWT_COUNTDOWN = "CBWT.Countdown";
}

namespace ecalltimer {
    constexpr const char* CBWT = "CBWT";
} //namespace ecalltimer

namespace GPS_Value{
    // gpsData.coordinate.latitude :
    // Attribute: number, Length: 9, Value/Format: [-324000000 - 324000000], Default: 0
    // Remarks: Latitude of vehicle position information from GPS. (mas : milliarcseconds)
    const std::string latitude = "latitude";
    // gpsData.coordinate.longitude :
    // Attribute: number, Length: 9, Value/Format: [-648000000 - 648000000], Default: 0
    // Remarks: Longitude of vehicle position information from GPS. (mas : milliarcseconds)
    const std::string longitude = "longitude";
    // gpsData.coordinate.datum :
    // Attribute: string, Length: -, Value/Format: wgs84,
    // Remarks: wgs84 : World Geodetic System (fixed)
    const std::string datum = "datum";
    // gpsData.coordinate.format :
    // Attribute: string, Length: -, Value/Format: wgs84,
    // Remarks: mas : milliarcseconds (fixed)
    const std::string format = "format";
    // gpsData.veclocity.value :
    // Attribute: number, Length: 4, Value/Format: [0 - 1800],
    // Remarks: (kph : kilometers per hour), Default value: 0
    const std::string value = "value";
    // gpsData.veclocity.unit :
    // Attribute: string, Length: -, Value/Format: kph,
    const std::string unit = "unit";
    // gpsData.accuracy.pdop :
    // Attribute: number, Length: 2, Value/Format: [0 - 15],
    // Remarks: Index number of position dilution of precision(PDOP), Default value: 0
    const std::string pdop = "pdop";
    // gpsData.accuracy.hdop :
    // Attribute: number, Length: 2, Value/Format: [0 - 15],
    // Remarks: Index number of horizontal dilution of precision(PDOP), Default value: 0
    const std::string hdop = "hdop";
    //gpsData.coordinate: latitude | longitude | datum | format
    const std::string coordinate ="coordinate";
    // gpsData.dtTime :
    // Attribute: string, Length: 25, Value/Format: YYYY-MM-DDTHH:MM:SS+00:00,
    // Remarks: (UTC Time, RFC3339), Default value: 1900-01-01T00:00:00+00:00
    const std::string dtTime = "dtTime";
    //gpsData.velocity: value | unit
    const std::string velocity = "velocity";
    // gpsData.courseHeading :
    // Attribute: number, Length: 4,1 , Value/Format: [0.0-359.9]
    // Remarks: Vehicle direction of movement information from GPS, Default value: 0.0
    //          0.0 : North 90.0 : East 180.0 : South 270.0 : West
    const std::string courseHeading = "courseHeading";
    /**
    * @brief Accuracy feild including value(speed) | unit.
    *
    */
    const std::string accuracy = "accuracy";
    /**
    * @brief Altitude of the event location in meters.
    *
    */
    const std::string altitude = "altitude";
    /**
    * @brief Timestamp when the location data was recorded in UTC format.
    *
    */
    const std::string timestamp = "timestamp";
    /**
    * @brief Heading of direction of travel of the vehicle in degrees.
    *
    */
    const std::string azimuth = "azimuth";
    /**
    * @brief Accuracy of the location data in meters.
    */
    const std::string positioningaccuracy = "positioningaccuracy";
    /**
    * @brief Rate of change of vehicle speed during the event in meters per second^2.
    *
    */
    const std::string acceleration = "acceleration";
    /**
    * @brief Real-time speed of the vehicle in kilometers per hour.
    *
    */
    const std::string speed = "speed";
    const std::string datumValue = "wgs84";
    const std::string formatValue = "mas";
    const std::string unitValue = "kph";

    // Define a coordinate map
    const std::unordered_map<std::string, std::pair<int32_t, int32_t>> rangeMap = {
    {latitude, {-324000000, 324000000}},
    {longitude, {-648000000, 648000000}}
    };

    const float DEFAULT_FLOAT_VALUE {0.0};
    const int32_t DEFAULT_INT32_VALUE {0};

}

// constant value used for SLDD in ECallApp2
static constexpr uint32_t SLDD_ECALL_SHORT_PRESS_CODE{5001U};
static constexpr uint32_t SLDD_ECALL_LONG_PRESS_CODE{5002U};
static constexpr uint32_t SLDD_DESS_MES_STATUS_NOTIFICATION_0_CODE{5003U};
static constexpr uint32_t SLDD_DESS_MES_STATUS_NOTIFICATION_1_CODE{5004U};
static constexpr uint32_t SLDD_DESS_MES_STATUS_NOTIFICATION_2_CODE{5005U};
static constexpr uint32_t SLDD_RESET_ECALL_CODE{10000U};
static constexpr uint32_t SLDD_RECOVERING_ECALL_CODE{5006U};

static constexpr int32_t CHARACTER_SPACE_INT_VALUE{32};

// ACN-US related constants
static constexpr int32_t POST_ACN_US_ON_OFF{9999};
static constexpr int32_t ACNUS_ON{1};
static constexpr int32_t ACNUS_OFF{0};

// eCall Flag's value
static constexpr uint8_t ECALL_ON{1U};

} /* End: namespace ECALLDCMNONREG */
#endif // ECALLNONREG_UTILS_PARAMSDEF_H