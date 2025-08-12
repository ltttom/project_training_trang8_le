class MockPowerManager {
 public:
    MOCK_METHOD0(instance, PowerManager*());
    MOCK_METHOD2(requestToGet, error_t(int32_t power_index, int32_t& value));
};

MockPowerManager * M_PowerManager;

PowerManager::PowerManager(){}
PowerManager* PowerManager::instance()
{
    return M_PowerManager->instance();
}

error_t PowerManager::requestToGet(int32_t power_index, int32_t& value) {
    return M_PowerManager->requestToGet(power_index, value);
}