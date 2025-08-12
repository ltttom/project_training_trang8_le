#include "data_builder_rcall.h"
#include "../oem_call_app_constants.h"
#include "../logger.h"
#include "../utils/utils_common.h"
#include <TelephonyManager.hpp>
#include <sstream>

namespace oc{

/**
 * @brief This contains the private properties of DataBuilderUS class instance
 *
 */
struct DataBuilderRCall::Impl {
    Json::Value root;
    std::string ignition = "unknown";

    // Recursively copy the values of b into a. Both a and b must be objects.
    void update(Json::Value& a, Json::Value& b) {
        if (!a.isObject() || !b.isObject()) return;

        for (const auto& key : b.getMemberNames()) {
            if (a[key].isObject()) {
                update(a[key], b[key]);
            } else {
                a[key] = b[key];
            }
        }
    }
};

DataBuilderRCall::DataBuilderRCall()
    :impl_(new Impl()) {
    upGPSCollector_ = std::make_unique<GPSCollector> ();
    upCanCollector_ = nullptr;
}

std::string DataBuilderRCall::build() {
    LOG_I(DLT_STRING("Building payload"));
    impl_->root.clear();
    impl_->root["ignition"] = impl_->ignition;
    impl_->root["gpsData"]  = upGPSCollector_->get();
    Json::StreamWriterBuilder stringBuilder;
    LOG_I(DLT_STRING("Build payload sucessfully"));
    stringBuilder.settings_["precision"] = 5;
    stringBuilder.settings_["precisionType"] = "decimal";
    return Json::writeString(stringBuilder, impl_->root);

}

IDataBuilder &DataBuilderRCall::setGPS() {
    upGPSCollector_->collect(GPSCollector::Type::RCALL);
    return *this;

}

IDataBuilder &DataBuilderRCall::setUtil(const std::string ignition) {
    impl_->ignition = ignition;
    return *this;
}

}
