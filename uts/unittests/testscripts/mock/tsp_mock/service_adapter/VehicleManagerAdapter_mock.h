#ifndef VEHICLE_MANAGER_ADAPTER_MOCK_H
#define VEHICLE_MANAGER_ADAPTER_MOCK_H
// using namespace ecallapp;
class MockVehicleManagerAdapter : public VehicleManagerAdapter {
public:
    MOCK_METHOD0(registerService, void());
    MOCK_METHOD1(handleSRSSignal, void(const uint32_t sigID));
    MOCK_METHOD2(sendVehicleData, void(const uint32_t channel, android::sp<VehicleData> data));
    MOCK_METHOD1(getCanData, uint8_t(const SigInd_Rx id));
    MOCK_METHOD2(onReceivedCanSignal, void(const uint32_t channel, const android::sp<VehicleData>& vehicleData));
    MOCK_METHOD1(onBinderDied, void(const android::wp<android::IBinder>& who));
    MOCK_METHOD1(onTimerTimeoutEvent, void(const int32_t& msgId));
    MOCK_METHOD1(send_BCAN_CANCEL_DISP, void(const uint32_t value));
    MOCK_METHOD1(sendBCAN_CANCEL_DISP, void(const uint32_t value));
    MOCK_METHOD1(sendACKToRVU, void(const uint32_t value));
    MOCK_METHOD1(notifySOSPressedToRVU, void(const uint32_t value));
    MOCK_METHOD0(collectCanData, void());
    MOCK_METHOD0(collectIntialData, void());
    MOCK_METHOD0(getCrashTelemetry, Json::Value());
    MOCK_METHOD0(getEventDetails, Json::Value());
    MOCK_METHOD0(getOccupantsCount, std::string());
    MOCK_METHOD0(clearCanData, void());
};

MockVehicleManagerAdapter* M_VehicleManagerAdapter;

std::shared_ptr<VehicleManagerAdapter> VehicleManagerAdapter::instance = nullptr;

VehicleManagerAdapter* VehicleManagerAdapter::getInstance() 
{
    if (instance == nullptr) {
        instance = std::make_shared<VehicleManagerAdapter>();
    }
    return instance.get();
}

VehicleManagerAdapter::VehicleManagerAdapter() {}

VehicleManagerAdapter::~VehicleManagerAdapter() {}

void VehicleManagerAdapter::registerService() {
    M_VehicleManagerAdapter->registerService();
}

void VehicleManagerAdapter::handleSRSSignal(const uint32_t sigID) {
    M_VehicleManagerAdapter->handleSRSSignal(sigID);
}

void VehicleManagerAdapter::sendVehicleData(const uint32_t channel, android::sp<VehicleData> data) {
    M_VehicleManagerAdapter->sendVehicleData(channel, data);
}

uint8_t VehicleManagerAdapter::getCanData(const SigInd_Rx id) {
    return M_VehicleManagerAdapter->getCanData(id);
}

void VehicleManagerAdapter::onReceivedCanSignal(const uint32_t channel, const android::sp<VehicleData>& vehicleData) {
    M_VehicleManagerAdapter->onReceivedCanSignal(channel, vehicleData);
}

void VehicleManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who) {
    M_VehicleManagerAdapter->onBinderDied(who);
}

void VehicleManagerAdapter::onTimerTimeoutEvent(const int32_t& msgId) {
    M_VehicleManagerAdapter->onTimerTimeoutEvent(msgId);
}

void VehicleManagerAdapter::send_BCAN_CANCEL_DISP(const uint32_t value) {
    M_VehicleManagerAdapter->send_BCAN_CANCEL_DISP(value);
}

void VehicleManagerAdapter::sendBCAN_CANCEL_DISP(const uint32_t value) {
    M_VehicleManagerAdapter->sendBCAN_CANCEL_DISP(value);
}

void VehicleManagerAdapter::sendACKToRVU(const uint32_t value) {
    M_VehicleManagerAdapter->sendACKToRVU(value);
}

void VehicleManagerAdapter::notifySOSPressedToRVU(const uint32_t value) {
    M_VehicleManagerAdapter->notifySOSPressedToRVU(value);
}

void VehicleManagerAdapter::collectCanData() {
    M_VehicleManagerAdapter->collectCanData();
}

void VehicleManagerAdapter::collectIntialData() {
    M_VehicleManagerAdapter->collectIntialData();
}

Json::Value VehicleManagerAdapter::getCrashTelemetry() {
    return M_VehicleManagerAdapter->getCrashTelemetry();
}

Json::Value VehicleManagerAdapter::getEventDetails() {
    return M_VehicleManagerAdapter->getEventDetails();
}

std::string VehicleManagerAdapter::getOccupantsCount() {
    return M_VehicleManagerAdapter->getOccupantsCount();
}

void VehicleManagerAdapter::clearCanData() {
    M_VehicleManagerAdapter->clearCanData();
}

class MockVCMReceiver : public VCMReceiver {
public:
    explicit MockVCMReceiver(VehicleManagerAdapter& vcm) noexcept : VCMReceiver(vcm) {}
    MOCK_METHOD2(onReceived, void(const uint32_t channel, const sp<VehicleData>& vehicleData));
    MOCK_METHOD1(onTimerTimeoutEvent, void(const int32_t& msgId));
};

#endif // VEHICLE_MANAGER_ADAPTER_MOCK_H