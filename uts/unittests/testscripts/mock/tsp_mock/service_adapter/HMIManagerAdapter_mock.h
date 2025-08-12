#ifndef HMI_MANAGER_ADAPTER_MOCK_H
#define HMI_MANAGER_ADAPTER_MOCK_H

// using namespace ecallapp;

class MockHMIManagerAdapter : public HMIManagerAdapter {
public:
    MOCK_METHOD0(getInstance, HMIManagerAdapter*());
    MOCK_METHOD0(getService, android::sp<IHMIManagerService>());
    MOCK_METHOD0(registerService, void());
    MOCK_METHOD2(requestLedPattern, void(const int32_t& pattern, const bool& isOn));
    MOCK_CONST_METHOD2(sendButtonEventACKToMCU, void(const uint32_t& index, const uint8_t& action));
    MOCK_METHOD0(turnOffCurrentLed, void());
    MOCK_METHOD3(handleHMIReceived, void(const uint32_t& type, const uint32_t& index, const uint32_t& action));
    MOCK_METHOD1(onBinderDied, void(const android::wp<android::IBinder>& who));
};

MockHMIManagerAdapter* M_HMIManagerAdapter;

std::shared_ptr<HMIManagerAdapter> HMIManagerAdapter::instance = nullptr;
HMIManagerAdapter* HMIManagerAdapter::getInstance() 
{
    if (instance == nullptr) {
        instance = std::make_shared<HMIManagerAdapter>();
    }
    return instance.get();
}

HMIManagerAdapter::HMIManagerAdapter() {}

void HMIManagerAdapter::registerService() {
    M_HMIManagerAdapter->registerService();
}

void HMIManagerAdapter::requestLedPattern(const int32_t& pattern, const bool& isOn) {
    M_HMIManagerAdapter->requestLedPattern(pattern, isOn);
}

void HMIManagerAdapter::sendButtonEventACKToMCU(const uint32_t& index, const uint8_t& action) const {
    M_HMIManagerAdapter->sendButtonEventACKToMCU(index, action);
}

void HMIManagerAdapter::turnOffCurrentLed() {
    M_HMIManagerAdapter->turnOffCurrentLed();
}

void HMIManagerAdapter::handleHMIReceived(const uint32_t& type, const uint32_t& index, const uint32_t& action) {
    M_HMIManagerAdapter->handleHMIReceived(type, index, action);
}

void HMIManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who) {
    M_HMIManagerAdapter->onBinderDied(who);
}



#endif // HMI_MANAGER_ADAPTER_MOCK_H