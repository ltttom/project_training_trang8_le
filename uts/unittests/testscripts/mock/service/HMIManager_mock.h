class MockHMIManager : HMIManager {
public:
    MOCK_METHOD1(setLedPattern, error_t(const HMILedPatternInfo ledInfo));
    MOCK_METHOD0(instance, HMIManager*());
    MOCK_METHOD2(registerReceiver, error_t(serviceid_t id, android::sp<IHMIEventReceiver> receiver));
};

MockHMIManager *M_HMIManager;

HMIManager::HMIManager()
{

}

HMIManager::~HMIManager()
{
}

HMIManager* HMIManager::instance()
{
    return M_HMIManager->instance();
}

error_t HMIManager::setLedPattern(const HMILedPatternInfo ledInfo) {
    return M_HMIManager->setLedPattern(ledInfo);
}

error_t HMIManager::registerReceiver(serviceid_t id, android::sp<IHMIEventReceiver> receiver) {
    return M_HMIManager->registerReceiver(id, receiver);
}