class MockIPowerManagerService {
public:
    MOCK_METHOD2(getCurrentPowerState , int32_t(int32_t opt, int32_t event));
    MOCK_METHOD3(registerPowerStateReceiver , error_t(android::sp<IPowerStateReceiver> receiver,
                                              int32_t mask,
                                              bool notify_now));

    MOCK_METHOD1(unregisterPowerStateReceiver , error_t(android::sp<IPowerStateReceiver> receiver));
    MOCK_METHOD1(reboot , void(int32_t reason));
    MOCK_METHOD1(poweroff , void(int32_t reason));
    MOCK_METHOD1(setIdleTimer , error_t(int32_t reason));
    MOCK_METHOD0(getIdleTimer , int32_t());
    MOCK_METHOD3(insertPowerLock , int32_t(int32_t id,
                                    android::sp<android::IBinder> token,
                                    PowerLockType lock_type));

    MOCK_METHOD2(removePowerLock , error_t(int32_t id, PowerLockType lock_type));
    MOCK_METHOD2(acquirePowerLock , error_t(int32_t id, PowerLockType lock_type));
    MOCK_METHOD2(releasePowerLock , error_t(int32_t id, PowerLockType lock_type));
    MOCK_METHOD2(getPowerLockCount , int32_t(int32_t all, PowerLockType lock_type));

    MOCK_METHOD2(input_event , void(int32_t event, int32_t value));
    MOCK_METHOD2(getStrStateOrEvent , android::sp<Buffer>(int32_t state_event, int32_t opt));

    MOCK_METHOD1(get_property, int32_t(const char* name));
    MOCK_METHOD1(get_property_str, android::sp<Buffer>(const char* name));
    MOCK_METHOD2(set_property , void(const char* name, const char* value));
    MOCK_METHOD2(set_property , void(const char* name, const int32_t i_value));
    MOCK_METHOD2(requestToSet , error_t(int32_t power_index, int32_t value));
    MOCK_METHOD2(requestToSet , error_t(int32_t power_index, int32_t& value));
    MOCK_METHOD2(requestToGet , error_t(int32_t power_index, int32_t& value));
    MOCK_METHOD1(monitoringSleepMode, error_t(int32_t value));
    MOCK_METHOD1(setDebugTestCase, void(int32_t test_case));
};

class MockBnPowerManagerService {
public:
    MOCK_METHOD4(onTransact, android::status_t(uint32_t code, const android::Parcel& data, android::Parcel* reply, uint32_t flags));
};



MockIPowerManagerService * M_IPowerManagerService;

MockBnPowerManagerService * M_BnPowerManagerService;

android::status_t BnPowerManagerService::onTransact(uint32_t code, const android::Parcel& data, android::Parcel* reply, uint32_t flags)
{
    return M_BnPowerManagerService->onTransact(code, data, reply, flags);
}


class ITestIPowerManagerService : public BnPowerManagerService
{
public:
    virtual int32_t getCurrentPowerState(int32_t opt, int32_t event)
    {
        return M_IPowerManagerService->getCurrentPowerState(opt, event);
    }

    virtual error_t registerPowerStateReceiver(android::sp<IPowerStateReceiver> receiver,
                                              int32_t mask, bool notify_now)
    {
        return M_IPowerManagerService->registerPowerStateReceiver(receiver, mask, notify_now);
    }

    virtual error_t unregisterPowerStateReceiver(android::sp<IPowerStateReceiver> receiver)
    {
        return M_IPowerManagerService->unregisterPowerStateReceiver(receiver);
    }

    virtual void reboot(int32_t reason)
    {
        return M_IPowerManagerService->reboot(reason);
    }
    virtual void poweroff(int32_t reason)
    {
        return M_IPowerManagerService->poweroff(reason);
    }

    virtual void setIdleTimer(int32_t seconds)
    {
        return M_IPowerManagerService->setIdleTimer(seconds);
    }
    virtual int32_t getIdleTimer()
    {
        return M_IPowerManagerService->getIdleTimer();
    }

    virtual int32_t insertPowerLock(int32_t id, android::sp<android::IBinder> token, PowerLockType lock_type)
    {
        return M_IPowerManagerService->insertPowerLock(id, token, lock_type);
    }
    virtual error_t removePowerLock(int32_t id, PowerLockType lock_type)
    {
        return M_IPowerManagerService->removePowerLock(id, lock_type);
    }
    virtual error_t acquirePowerLock(int32_t id, PowerLockType lock_type)
    {
        return M_IPowerManagerService->acquirePowerLock(id, lock_type);
    }
    virtual error_t releasePowerLock(int32_t id, PowerLockType lock_type)
    {
        return M_IPowerManagerService->releasePowerLock(id, lock_type);
    }
    virtual int32_t getPowerLockCount(int32_t all, PowerLockType lock_type)
    {
        return M_IPowerManagerService->getPowerLockCount(all, lock_type);
    }

    virtual void input_event(int32_t event, int32_t value)
    {
        return M_IPowerManagerService->input_event(event, value);
    }

    virtual android::sp<Buffer> getStrStateOrEvent(int32_t state_event, int32_t opt)
    {
        return M_IPowerManagerService->getStrStateOrEvent(state_event, opt);
    }

    virtual int32_t get_property(const char* name)
    {
        return M_IPowerManagerService->get_property(name);
    }
    virtual android::sp<Buffer> get_property_str(const char* name)
    {
        return M_IPowerManagerService->get_property_str(name);
    }
    virtual void set_property(const char* name, const char* value)
    {
        return M_IPowerManagerService->set_property(name, value);
    }
    virtual void set_property(const char* name, const int32_t i_value)
    {
        return M_IPowerManagerService->set_property(name, i_value);
    }
    virtual error_t requestToSet(int32_t power_index, int32_t value)
    {
        //return M_IPowerManagerService->requestToSet(power_index, value);
    }
    virtual error_t requestToSet(int32_t power_index, int32_t& value)
    {
        //return M_IPowerManagerService->requestToSet(power_index, value);
    }
    virtual error_t requestToGet(int32_t power_index, int32_t& value)
    {
        return M_IPowerManagerService->requestToGet(power_index, value);
    }

    virtual error_t monitoringSleepMode(int32_t value)
    {
        return M_IPowerManagerService->monitoringSleepMode(value);
    }
    virtual void setDebugTestCase(int32_t test_case) 
    { 
        return M_IPowerManagerService->setDebugTestCase(test_case);
    }
};
 