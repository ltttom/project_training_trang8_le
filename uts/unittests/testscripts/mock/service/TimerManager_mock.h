class MockTimeManager {
public:
    MOCK_METHOD0(getCurrentTime, tm());
};

MockTimeManager *M_TimeManager;

TimeManager::TimeManager() {}

TimeManager& TimeManager::getInstance() {
    static TimeManager tm;

    return tm;
}

tm TimeManager::getCurrentTime() {
    return M_TimeManager->getCurrentTime();
}