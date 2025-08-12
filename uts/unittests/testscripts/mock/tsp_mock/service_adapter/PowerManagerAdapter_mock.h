#ifndef POWERMANAGERADAPTER_MOCK_H
#define POWERMANAGERADAPTER_MOCK_H

// using namespace ecallapp;

class MockPowerManagerAdapter : public PowerManagerAdapter {
public:
    MockPowerManagerAdapter(){}
    virtual ~MockPowerManagerAdapter(){}
    MOCK_METHOD0(getInstance, PowerManagerAdapter*());
    MOCK_METHOD0(registerService, void());
    MOCK_METHOD0(getService, android::sp<IPowerManagerService>());
    MOCK_METHOD0(acquirePowerLock, void());
    MOCK_METHOD0(releasePowerLock, void());
    MOCK_METHOD0(onPowerLockRelease, void());
    MOCK_METHOD0(isIgnitionOn, bool());
    MOCK_METHOD2(onPowerStateChanged, void(const int32_t newState, const int32_t reason));
    MOCK_METHOD3(onErrControlPower, void(const int32_t err_reason, const int32_t errPowerID, const int32_t currPowerID));
    MOCK_METHOD1(onPowerModeChanged, void(const int32_t newMode));
    MOCK_METHOD2(onExtValueChanged, void(const int32_t listenIndex, const int32_t value));
    MOCK_METHOD1(onBinderDied, void(const android::wp<android::IBinder>& who));
    MOCK_METHOD0(getACCStatus, int32_t());
    MOCK_METHOD0(getIGStatus, int32_t());
    MOCK_METHOD1(requestBUBModeACNWithPhone, bool(bool isOn));
    MOCK_METHOD1(requestBUBModeACNRunning, bool(bool isOn));
    MOCK_METHOD0(resetPowerLockCounter, void());
};

MockPowerManagerAdapter* M_PowerManagerAdapter;

PowerManagerAdapter* PowerManagerAdapter::instance = nullptr;

PowerManagerAdapter* PowerManagerAdapter::getInstance() 
{
    if (instance == nullptr) {
        instance = new PowerManagerAdapter();
    }
    return instance;
}

PowerManagerAdapter::PowerManagerAdapter() {}

PowerManagerAdapter::~PowerManagerAdapter() noexcept {}

void PowerManagerAdapter::registerService() {
    M_PowerManagerAdapter->registerService();
}

android::sp<IPowerManagerService> PowerManagerAdapter::getService() const noexcept {
    return M_PowerManagerAdapter->getService();
}

void PowerManagerAdapter::acquirePowerLock() {
    M_PowerManagerAdapter->acquirePowerLock();
}

void PowerManagerAdapter::releasePowerLock() {
    M_PowerManagerAdapter->releasePowerLock();
}

bool PowerManagerAdapter::isIgnitionOn() {
    return M_PowerManagerAdapter->isIgnitionOn();
}

void PowerManagerAdapter::onPowerStateChanged(const int32_t newState, const int32_t reason) const {
    M_PowerManagerAdapter->onPowerStateChanged(newState, reason);
}

void PowerManagerAdapter::onErrControlPower(const int32_t err_reason, const int32_t errPowerID, const int32_t currPowerID) const {
    M_PowerManagerAdapter->onErrControlPower(err_reason, errPowerID, currPowerID);
}

void PowerManagerAdapter::onPowerModeChanged(const int32_t newMode) {
    M_PowerManagerAdapter->onPowerModeChanged(newMode);
}

void PowerManagerAdapter::onExtValueChanged(const int32_t listenIndex, const int32_t value) {
    M_PowerManagerAdapter->onExtValueChanged(listenIndex, value);
}

void PowerManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who) {
    M_PowerManagerAdapter->onBinderDied(who);
}

int32_t PowerManagerAdapter::getACCStatus() {
    return M_PowerManagerAdapter->getACCStatus();
}

int32_t PowerManagerAdapter::getIGStatus() {
    return M_PowerManagerAdapter->getIGStatus();
}

bool PowerManagerAdapter::requestBUBModeACNWithPhone(bool isOn) {
    return M_PowerManagerAdapter->requestBUBModeACNWithPhone(isOn);
}

bool PowerManagerAdapter::requestBUBModeACNRunning(bool isOn) {
    return M_PowerManagerAdapter->requestBUBModeACNRunning(isOn);
}

void PowerManagerAdapter::resetPowerLockCounter() {
    M_PowerManagerAdapter->resetPowerLockCounter();
}
#endif // POWERMANAGERADAPTER_MOCK_H