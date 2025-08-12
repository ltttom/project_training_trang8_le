using namespace android;
class MockLocation {
public:
    MOCK_METHOD0(IsLocationDataAvailable, uint8_t());
    MOCK_METHOD1(getLocationData, error_t(android::sp<LocationData>& loc_data));
    MOCK_METHOD1(getLastLocationData, error_t(android::sp<LocationData>& loc_data));
    MOCK_METHOD3(LocationLgeGnssCmd, error_t(uint32_t cmdType, uint32_t value, uint32_t *resp_value));

};

MockLocation * M_Location;

constexpr const char* LocationData::LOCDATA_SOURCE;
constexpr const char* LocationData::LOCDATA_TIME_STAMP;
constexpr const char* LocationData::LOCDATA_PPSTIME_STAMP;
constexpr const char* LocationData::LOCDATA_LATITUDE;
constexpr const char* LocationData::LOCDATA_LONGITUDE;
constexpr const char* LocationData::LOCDATA_ALTITUDE;
constexpr const char* LocationData::LOCDATA_SPEED;
constexpr const char* LocationData::LOCDATA_BEARING;
constexpr const char* LocationData::LOCDATA_ACCURACY;
constexpr const char* LocationData::LOCDATA_VER_ACCURACY;
constexpr const char* LocationData::LOCDATA_SPEED_ACCURACY;
constexpr const char* LocationData::LOCDATA_HEAD_ACCURACY;
constexpr const char* LocationData::LOCDATA_FIXTYPE;
constexpr const char* LocationData::LOCDATA_HDOP;
constexpr const char* LocationData::LOCDATA_VDOP;
constexpr const char* LocationData::LOCDATA_PDOP;
constexpr const char* LocationData::LOCDATA_NUMBER_OF_SATELLITES;
constexpr const char* LocationData::LOCDATA_GPS_SVC;
constexpr const char* LocationData::LOCDATA_GLONASS_SVC;
constexpr const char* LocationData::LOCDATA_GALILEO_SVC;
constexpr const char* LocationData::LOCDATA_GPS_CN0;
constexpr const char* LocationData::LOCDATA_GLONASS_CN0;
constexpr const char* LocationData::LOCDATA_GALILEO_CN0;
constexpr const char* LocationData::LOCDATA_GPS_L5_CN0;
constexpr const char* LocationData::LOCDATA_BEARING_AVG;
constexpr const char* LocationData::LOCDATA_FLAGS;
constexpr const char* LocationData::LOCDATA_SMIN;
constexpr const char* LocationData::LOCDATA_SMAJ;
constexpr const char* LocationData::LOCDATA_MAJAA;
constexpr const char* LocationData::LOCDATA_ACCURACY_95;

 constexpr const char* LocationData::LOCDATA_FIX_AVAILABLE;
 constexpr const char* LocationData::LOCDATA_DIFF_AVAILABLE;
 constexpr const char* LocationData::LOCDATA_POS_FIX_SRC;
 constexpr const char* LocationData::LOCDATA_DEAD_RECKONING;
 constexpr const char* LocationData::LOCDATA_CALIBRATED;
 constexpr const char* LocationData::LOCDATA_RAIM_PROBLEM;
 constexpr const char* LocationData::LOCDATA_CANBE_TRUSTED;
 constexpr const char* LocationData::LOCDATA_USED_SYSTEM;
 constexpr const char* LocationData::LOCDATA_ROAD_TYPE;
 constexpr const char* LocationData::LOCDATA_TIME_YEAR;
 constexpr const char* LocationData::LOCDATA_TIME_MONTH;
 constexpr const char* LocationData::LOCDATA_TIME_DAY;
 constexpr const char* LocationData::LOCDATA_TIME_HOUR;
 constexpr const char* LocationData::LOCDATA_TIME_MIN;
 constexpr const char* LocationData::LOCDATA_TIME_SEC;
 constexpr const char* LocationData::LOCDATA_LAT_DELTA_VELOCITY;
 constexpr const char* LocationData::LOCDATA_LONG_DELTA_VELOCITY;
 constexpr const char* LocationData::LOCDATA_COLLISION_UTCTIME;
 constexpr const char* LocationData::LOCDATA_SECONDARY_COLLISION;
 constexpr const char* LocationData::LOCDATA_COMPASS_DIRECTION;

 constexpr const char* LocationData::IMUDATA_VM_X_ACCEL;
 constexpr const char* LocationData::IMUDATA_VM_Y_ACCEL;
 constexpr const char* LocationData::IMUDATA_VM_Z_ACCEL;
 constexpr const char* LocationData::IMUDATA_VM_X_ANG_RATE;
 constexpr const char* LocationData::IMUDATA_VM_Y_ANG_RATE;
 constexpr const char* LocationData::IMUDATA_VM_Z_ANG_RATE;
 constexpr const char* LocationData::IMUDATA_VM_X_ACCEL_CONFIDENCE;
 constexpr const char* LocationData::IMUDATA_VM_Y_ACCEL_CONFIDENCE;
 constexpr const char* LocationData::IMUDATA_VM_Z_ACCEL_CONFIDENCE;
 constexpr const char* LocationData::IMUDATA_VM_X_ANG_RATE_CONFIDENCE;
 constexpr const char* LocationData::IMUDATA_VM_Y_ANG_RATE_CONFIDENCE;
 constexpr const char* LocationData::IMUDATA_VM_Z_ANG_RATE_CONFIDENCE;
 constexpr const char* LocationData::IMUDATA_VM_CALIBRATED;
 constexpr const char* LocationData::LOCDATA_GEOID_HEIGHT;
 constexpr const char* LocationData::LOCDATA_TOW;
 constexpr const char* LocationData::LOCDATA_WEEK;
 constexpr const char* LocationData::LOCDATA_GEODETIC_SYSTEM;

 constexpr const char* LocationData::IMUDATA_VM_SPEED;
 constexpr const char* LocationData::IMUDATA_VM_CURVATURE;
 constexpr const char* LocationData::IMUDATA_VM_CURVATURE_CONFIDENCE;
 constexpr const char* LocationData::LOCDATA_PRE_SHIFT_LONGITUDE;
 constexpr const char* LocationData::LOCDATA_PRE_SHIFT_LATITUDE;

class LocationManagerService : public BnLocationManagerService
{
    
    virtual error_t getLocationData(android::sp<LocationData> loc_data); //--
    virtual error_t getLastLocationData(android::sp<LocationData> pLoc_data); //--
    virtual uint8_t IsLocationStarted(void)
    {
        return 1;
    }
    virtual error_t setLocationStatus(int8_t status)
    {
        return E_OK;
    }

    virtual uint8_t IsLocationDataAvailable(void); //---

    virtual error_t registerReceiver(android::sp<ILocationReceiver> receiver, int32_t mask)
    {
        return E_OK;
    }

    virtual error_t unregisterReceiver(android::sp<ILocationReceiver> receiver)
    {
        return E_OK;
    }

    virtual error_t requestLocationUpdate(uint32_t p_interval, double p_distance, uint32_t p_numUpdates, android::sp<ILocationReceiver> receiver, int32_t mask = 0)
    {
        return E_OK;
    }

    virtual error_t releaseLocationUpdate(android::sp<ILocationReceiver> receiver)
    {
        return E_OK;
    }

    virtual error_t setGpsStartMode(int8_t startType)
    {
        return E_OK;
    }
    virtual error_t getLastLocationData(LocationData_t* loc_data, int32_t data_size)
    {
        return E_OK;
    }

    virtual error_t SetPositionMode(uint32_t mode)
    {
        return E_OK;
    }
    virtual error_t deleteAidingData(uint64_t aidingData)
    {
        return E_OK;
    }
    virtual gps_status_t getGPSStatus()
    {
        return GPS_STATUS_NONE;
    }
    virtual error_t setAgpsServer(const char* host, int32_t port)
    {
        return E_OK;
    }

    virtual error_t setAPNInfo(const char* apn, int16_t agpsBearerType, unsigned long suplVer, const char* suplHost, int32_t suplPort)
    {
        return E_OK;
    }

    virtual error_t setAgpsBearerType(uint8_t agpsBearerType)
    {
        return E_OK;
    }

    virtual uint8_t getGpsPosReportStatus()
    {
        return 0;
    }

    virtual uint8_t getGPSTimeSetFlag()
    {
        return 0;
    }
    virtual error_t setGPSTimeSetFlag(uint8_t flag)
    {
        return E_OK;
    }


    virtual error_t getSensorData(android::sp<SensorData> sensor_data)
    {

    }

    virtual void enableGPSStart(void)
    {

    }
    virtual void disableGPSStart(void)
    {

    }

    virtual void getRecentLocationDelta(android::sp<LocationDeltaData> loc_delta, int64_t timeFrameMs = 0)
    {

    }
    virtual error_t requestLocationUpdate(uint32_t p_interval, double p_distance, uint32_t p_numUpdates, android::sp<ILocationReceiver> receiver )
    {
        return E_OK;
    }

    virtual bool getGNSSJammingStatus(void)
    {
        return true;
    }

    virtual void setNMEALog(uint8_t state)
    {

    }

    virtual void getPredictedLocationData(android::sp<PredictLocationData> PredictLocData)
    {

    }


    virtual void resetLocationInfo(void)
    {

    }

    virtual void setNMEASerialPortOnOff(uint8_t value)
    {

    }

    virtual void requireGNSSDataForTrackpointData(void)
    {

    }

    virtual error_t LocationLgeGnssCmd(uint32_t cmdType, uint32_t value, uint32_t *resp_value)
    {
        return M_Location->LocationLgeGnssCmd(cmdType, value, resp_value);
    }

    virtual void setLabTestMode(uint8_t value)
    {

    }

    virtual void setLabTestAutoStart(uint8_t value)
    {

    }

    virtual void setLogEnable(uint8_t value)
    {

    }

    virtual void setXtraEnable(uint8_t value)
    {

    }

    virtual bool GetResultFromCheckNmeaCb(void)
    {

    }

    virtual void setNTPDownload(void)
    {

    }

    virtual void setXdownload(void)
    {

    }

    virtual void requestSVT(void)
    {

    }
    virtual void requestSVT2(uint8_t value)
    {

    }
    virtual uint8_t IsXtraStatus(void)
    {
        return 1;
    }
    virtual void setBlockGpsStart(uint8_t value)
    {

    }
    virtual void NmeaLogFileDisplay(uint8_t value)
    {

    }
    virtual void requestSvtGpsFixWaitLongMs(void)
    {

    }
    virtual void requestSvtGpsFixWaitShortMs(void)
    {

    }
    virtual bool clearDTCStatus(int32_t pDtcName)
    {
        return true;
    }
    virtual bool storeLocation(void)
    {
        return  true;
    }

    virtual bool getRecentLocationN(int32_t reqSize, LocationData* recentLocations, int32_t &retSize)
    {
        return true;
    }
    virtual int32_t getGPSStartMode() {
        return 1;
    }
    virtual bool setUsedGnssSystem(int32_t param)
    {
        return true;
    }

    virtual bool getUsedGnssSystem(int32_t &status)
    {
        return true;
    }

    virtual bool setMinCn0(int32_t param)
    {
        return true;
    }
    virtual bool getMinCn0(int32_t &status)
    {
        return true;
    }
    virtual void cmdTest_SetLocationData(android::sp<LocationData> pLocData)
    {

    }
    virtual void cmdTest_blockNMEAinput(bool pValue)
    {

    }

    virtual int32_t setCWCn0Check(int32_t param) {};
    virtual int32_t getCWCn0Result(int32_t param) {};

    virtual error_t setCollisionFactor(float heading_curr, float vE_curr, float vN_curr, float heading_next, float vE_next, float vN_next){};

    virtual error_t setControlConstellation(int8_t satSystem, int8_t controlType){};
    virtual error_t setGpsStop(){};

    virtual error_t releaseDrUpdate(android::sp<ILocationRmmReceiver> receiver ) {};
    virtual error_t requestDrUpdate(uint32_t p_interval, double p_distance, uint32_t p_numUpdates, android::sp<ILocationRmmReceiver> receiver ){};

    virtual error_t requestDrCalibrationUpdate(uint32_t p_interval, double p_distance, uint32_t p_numUpdates, android::sp<ILocationRmmReceiver> receiver ) {};
    virtual error_t releaseDrCalibrationUpdate(android::sp<ILocationRmmReceiver> receiver ) {};

    virtual error_t requestDrPpsModeUpdate(uint32_t p_interval, double p_distance, uint32_t p_numUpdates, android::sp<ILocationRmmReceiver> receiver ) {};
    virtual error_t releaseDrPpsModeUpdate(android::sp<ILocationRmmReceiver> receiver ) {};

    virtual error_t requestLocationSvDataUpdate(uint32_t p_interval, double p_distance, uint32_t p_numUpdates, android::sp<ILocationRmmReceiver> receiver ) {};
    virtual error_t releaseLocationSvDataUpdate(android::sp<ILocationRmmReceiver> receiver ) {};

    virtual error_t setCorrections(int8_t enable_control) {};
    virtual error_t setElevationAngle(float angle) {};
    virtual error_t getElevationAngle() {};
    virtual error_t registerReceiverLocationEthernetData(android::sp<ILocationRmmReceiver> receiver , int32_t mask) {};
    virtual error_t unRegisterReceiverLocationEthernetData(android::sp<ILocationRmmReceiver> receiver ) {};
    virtual error_t DataConnectionStateChanged(int32_t state, int32_t networkType) {};
    virtual bool getRecentDrLocationN(int32_t reqSize, LocationData* recentLocations, int32_t &retSize) {};
    virtual void getNMEAData(android::Vector<android::String8>& vecNMEA) {};
    virtual void getLastValidNMEAData(android::Vector<android::String8>& vecNMEA){};

};



uint8_t LocationManagerService::IsLocationDataAvailable()
{
    return M_Location->IsLocationDataAvailable();
}

error_t LocationManagerService::getLocationData(android::sp<LocationData> loc_data)
{
    // android::sp<LocationData> locData = nullptr;
    // error_t ret = M_Location->getLocationData(locData);
    // loc_data->setLocationData(locData);
    // //*(loc_data.get()) = *(locData.get());
    // return ret;
    loc_data->locationData.timestamp = 100;
    loc_data->locationData.PDOP = 16.0;
    loc_data->locationData.HDOP = 17.0;
    return E_OK;

}

error_t LocationManagerService::getLastLocationData(android::sp<LocationData> pLoc_data)
{
    // android::sp<LocationData> locData = nullptr;
    // error_t ret = M_Location->getLastLocationData(locData);
    // pLoc_data->setLocationData(locData);
    // //*(pLoc_data.get()) = *(locData.get());
    // return ret;
    return M_Location->getLastLocationData(pLoc_data);
}
