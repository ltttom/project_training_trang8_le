namespace oc {

class MockPowerManagerAdapter {
    public:
        MOCK_METHOD1(requestBUBModeACNPhone, bool(bool isActive));
        MOCK_METHOD1(requestBUBModeACN, bool(bool isActive));
        MOCK_METHOD1(requestBUBModeECall, bool(bool isActive));
        MOCK_METHOD0(resetIsLocked, void());
        MOCK_METHOD0(getACCStatus, int32_t());
        MOCK_METHOD0(getIGStatus, int32_t());
        MOCK_METHOD0(acquirePowerLock, void());
        MOCK_METHOD0(releasePowerLock, void());
        MOCK_METHOD0(getIgnition, std::string());
        MOCK_METHOD0(isAccOn, bool());
        MOCK_METHOD0(isIGON, bool());
        MOCK_METHOD2(handlePowerStateChanged, void(const uint32_t index, const uint32_t value));
        MOCK_METHOD0(onPowerLockRelease, void());
};

MockPowerManagerAdapter *M_PowerManagerAdapter;

PowerManagerAdapter::PowerManagerAdapter(android::sp<OemCallHandler> spHandler)
//    :impl_(new Impl())
    {
}

PowerManagerAdapter::~PowerManagerAdapter() {
}
void PowerManagerAdapter::resetIsLocked() {
    return M_PowerManagerAdapter->resetIsLocked();
}
bool PowerManagerAdapter::requestBUBModeACNPhone(bool isActive) {
    return M_PowerManagerAdapter->requestBUBModeACNPhone(isActive);
}

bool PowerManagerAdapter::requestBUBModeACN(bool isActive) {
    return M_PowerManagerAdapter->requestBUBModeACN(isActive);
}

bool PowerManagerAdapter::requestBUBModeECall(bool isActive) {
    return M_PowerManagerAdapter->requestBUBModeECall(isActive);
}

int32_t PowerManagerAdapter::getACCStatus() {
    return M_PowerManagerAdapter->getACCStatus();
}

int32_t PowerManagerAdapter::getIGStatus() {
    return M_PowerManagerAdapter->getIGStatus();
}

void PowerManagerAdapter::acquirePowerLock() {
    return M_PowerManagerAdapter->acquirePowerLock();
}

void PowerManagerAdapter::releasePowerLock() {
    return M_PowerManagerAdapter->releasePowerLock();
}

std::string PowerManagerAdapter::getIgnition() {
    return M_PowerManagerAdapter->getIgnition();
}

bool PowerManagerAdapter::isAccOn() {
    return M_PowerManagerAdapter->isAccOn();
}

bool PowerManagerAdapter::isIGON() {
    return M_PowerManagerAdapter->isIGON();
}

void PowerManagerAdapter::handlePowerStateChanged(const uint32_t index, const uint32_t value) {
    return M_PowerManagerAdapter->handlePowerStateChanged(index, value);
}

void PowerManagerAdapter::onPowerLockRelease() {
    return M_PowerManagerAdapter->onPowerLockRelease();
}

}  // namespace oc

