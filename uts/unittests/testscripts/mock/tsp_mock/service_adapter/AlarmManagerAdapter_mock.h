#ifndef ALARM_MANAGER_ADAPTER_MOCK_H
#define ALARM_MANAGER_ADAPTER_MOCK_H

// using namespace ecallapp;

class MockAlarmManagerAdapter : public AlarmManagerAdapter {
public:
    MockAlarmManagerAdapter():AlarmManagerAdapter(){}
    ~MockAlarmManagerAdapter(){}
    MOCK_METHOD0(registerService, void());
    MOCK_METHOD2(setAlarmFromNow, error_t(const int64_t& delayMs, const int32_t& alarmID));
    MOCK_METHOD1(cancel, error_t(const int32_t& alarmID));
    MOCK_METHOD1(onBinderDied, void(const android::wp<android::IBinder>& who));
};

MockAlarmManagerAdapter *M_AlarmManagerAdapter;

AlarmManagerAdapter::AlarmManagerAdapter() {}
AlarmManagerAdapter::~AlarmManagerAdapter() {}
AlarmManagerAdapter* AlarmManagerAdapter::instance = nullptr;
AlarmManagerAdapter* AlarmManagerAdapter::getInstance() 
{
    if (instance == nullptr) {
        instance = new AlarmManagerAdapter();
    }
    return instance;
}


void AlarmManagerAdapter::registerService() {
    M_AlarmManagerAdapter->registerService();
}

error_t AlarmManagerAdapter::setAlarmFromNow(const int64_t& delayMs, const int32_t& alarmID) {
    return M_AlarmManagerAdapter->setAlarmFromNow(delayMs, alarmID);
}

error_t AlarmManagerAdapter::cancel(const int32_t& alarmID) {
    return M_AlarmManagerAdapter->cancel(alarmID);
}

void AlarmManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who) {
    M_AlarmManagerAdapter->onBinderDied(who);
}

#endif // ALARM_MANAGER_ADAPTER_MOCK_H