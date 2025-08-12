#include "data_builder_us.h"
#include "../oem_call_app_constants.h"
#include "../logger.h"
namespace oc{

/**
 * @brief This contains the private properties of DataBuilderUS class instance
 *
 */
struct DataBuilderUS::Impl {
    bool isNormalCDS = true;
    std::string ignition = "unknown";

    Json::Value root;

};

DataBuilderUS::DataBuilderUS()
    :impl_(new Impl())
{
    upGPSCollector_ = std::make_unique<GPSCollector> ();
    upCanCollector_ = std::make_unique<CanDataCollector> ();
}

std::string DataBuilderUS::build() {
    LOG_I(DLT_STRING("Building payload"));
    impl_->root["ignition"] = impl_->ignition;
    impl_->root["gpsData"] = upGPSCollector_->get();

    auto crashTelemetry = upCanCollector_->get();
    crashTelemetry["crashLevel"] = (impl_->isNormalCDS) ? "1" : "0";
    impl_->root["crashTelemetry"] = crashTelemetry;

    Json::StreamWriterBuilder stringBuilder;
    stringBuilder.settings_["precision"] = 5;
    stringBuilder.settings_["precisionType"] = "decimal";
    return Json::writeString(stringBuilder, impl_->root);
}

IDataBuilder &DataBuilderUS::setCDS(bool isNormalCDS) {
    impl_->isNormalCDS = isNormalCDS;
    upCanCollector_->collect();
    return *this;
}

IDataBuilder &DataBuilderUS::setGPS() {
    upGPSCollector_->collect(GPSCollector::Type::ACN);
    return *this;

}

IDataBuilder &DataBuilderUS::setUtil(const std::string ignition) {
    impl_->ignition = ignition;
    return *this;
}

}
