#ifndef VEHICLE_VARIATION_INFO_ADAPTER_MOCK_H
#define VEHICLE_VARIATION_INFO_ADAPTER_MOCK_H

namespace ecallapp {

class MockVehicleVariationAdapter : public VehicleVariationAdapter {
public:
  MOCK_METHOD1(isFeatureAvailableInRegion, bool(const string FeatureName));
};

MockVehicleVariationAdapter* M_VehicleVariationAdapter;

VehicleVariationAdapter* VehicleVariationAdapter::instance = nullptr;

VehicleVariationAdapter* VehicleVariationAdapter::getInstance()
{
    if (instance == nullptr) {
        instance = new VehicleVariationAdapter();
    }
    return instance;
}

VehicleVariationAdapter::VehicleVariationAdapter() {}

VehicleVariationAdapter::~VehicleVariationAdapter() {
}

bool VehicleVariationAdapter::isFeatureAvailableInRegion(const string FeatureName) {
  return M_VehicleVariationAdapter->isFeatureAvailableInRegion(FeatureName);
}

}

#endif // VEHICLE_VARIATION_INFO_ADAPTER_MOCK_H