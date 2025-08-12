#include "data_collector_gps.h"
#include "../logger.h"
#include <utils/Buffer.h>
// Service manager
#include <binder/IServiceManager.h>

// Location manager
#include <services/LocationManagerService/ILocationManagerServiceType.h>
#include <services/LocationManagerService/LocationData.h>
#include <services/LocationManagerService/ILocationManagerService.h>

#include <chrono>
#include <iomanip>
#include <ctime>
#include "../utils/utils_common.h"
#include <sstream>

namespace oc {
using namespace std::chrono;

/**
 * @brief This contains the private properties of DataBuilderUS class instance
 *
 */
struct GPSCollector::Impl {
    Json::Value root;
    std::string body;

    /**
     * @brief This is the pointer to ConfigManager of OEMCall app
     *
     */

    android::sp<ILocationManagerService> spLocationMgrService;
    /**
     * @brief This is the method to get the location service pointer from
     *        service layer.
     */
    android::sp<ILocationManagerService> getLocationMgrService() {
        if (spLocationMgrService == nullptr) {
            spLocationMgrService = android::interface_cast<ILocationManagerService>(
                                    android::defaultServiceManager()->getService(
                                        android::String16(LOCATION_SRV_NAME)
                                )
            );
        }
        return spLocationMgrService;
    }

    uint32_t calculateAccuracy(float input) {
        uint32_t result = static_cast<uint32_t>(input);
        if (result > 15) result = 0;
        return result;
    }

    std::string validateCourseHeading(float input, uint32_t precision) {
        float result = 0.0;
        stringstream ss;
        ss << std::fixed << std::setprecision(precision) << input;
        ss >> result;

        if (result < 0.0 || result > 359.9) {
            std::string rs = "0.0";
            return rs;
        }
        return ss.str();
    }

    int32_t validateLatitude(double input) {
        auto result =  static_cast<int32_t>(input * 3600000);
        if (result < -324000000 || result > 324000000) {
            return 0;
        }
        return result;
    }

    int32_t validateLongtitude(double input) {
        auto result =  static_cast<int32_t>(input * 3600000);
        if (result < -648000000 || result > 648000000 ) {
            return 0;
        }
        return result;
    }

};

GPSCollector::GPSCollector()
    :impl_(new Impl())
{
}

void GPSCollector::collect(Type type) {
    LOG_I(DLT_STRING("Collecting GPS data"));
    android::sp<LocationData> lc = new LocationData();
    error_t ret ;
    uint32_t rsp_value;
    bool isChanged = false;
    ret = impl_->getLocationMgrService()->LocationLgeGnssCmd( GPS_CMD_LOC_PREFERRED , SUBCMD_GET_PREFERED_MODE,&rsp_value);
    if(ret != 1) {
        LOG_I(DLT_STRING("doesn't ICB pos"));
    } else if(rsp_value == PREFER_ICB_POS) {
        impl_->getLocationMgrService()->LocationLgeGnssCmd( GPS_CMD_LOC_PREFERRED , SUBCMD_SET_TSU_PREFERED ,&rsp_value);
        isChanged = true;
    }

    if (impl_->getLocationMgrService()->IsLocationDataAvailable() == 1) {
        (void)impl_->getLocationMgrService()->getLocationData(lc);
    }
    else {
        impl_->getLocationMgrService()->getLastLocationData(lc);
        LOG_E(DLT_STRING("Location data is not avalable, use lastest location data"));
    }

    impl_->root.clear();

    switch (type) {
    case Type::ACN: // fall through
    case Type::RCALL: {
        Json::Value cordinate;
        Json::Value velocity;
        Json::Value accuracy;
        cordinate["latitude"]       = impl_->validateLatitude(lc->locationData.latitude);
        cordinate["longitude"]      = impl_->validateLongtitude(lc->locationData.longitude);
        cordinate["datum"]          = "wgs84";
        cordinate["format"]         = "mas";
        velocity["value"]           = static_cast<uint32_t>(lc->locationData.speed * 3.6);
        velocity["unit"]            = "kph";
        accuracy["pdop"]            = impl_->calculateAccuracy(lc->locationData.PDOP);
        accuracy["hdop"]            = impl_->calculateAccuracy(lc->locationData.HDOP);

        impl_->root["coordinate"] = cordinate;
        impl_->root["dtTime"] = cv::Common::convertTime(lc->locationData.timestamp);
        impl_->root["velocity"]        = velocity;
        impl_->root["courseHeading"]   = std::stod(impl_->validateCourseHeading(lc->locationData.bearing, 1));
        impl_->root["accuracy"]        = accuracy;
    break;
    }
    default:
    break;
    }

    if(isChanged) {
        impl_->getLocationMgrService()->LocationLgeGnssCmd( GPS_CMD_LOC_PREFERRED , SUBCMD_SET_ICB_PREFERED ,&rsp_value);
        isChanged = false;
    }

}

Json::Value GPSCollector::get() {
    return impl_->root;
}

std::string GPSCollector::getString()
{
    return impl_->body;

}

}
