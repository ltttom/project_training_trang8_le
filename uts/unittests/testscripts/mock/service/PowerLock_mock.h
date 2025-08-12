class MockPowerLock {
    public:
        MOCK_METHOD2(acquire, int32_t(int32_t timeout_sec = 0, bool disbaleTimer = false));
        MOCK_METHOD0(release, int32_t());
};

MockPowerLock *M_PowerLock;
PowerLock::PowerLock(){}

int32_t PowerLock::acquire(int32_t timeout_sec, bool disbaleTimer) {
    return M_PowerLock->acquire(timeout_sec, disbaleTimer);
}

int32_t PowerLock::release()
{
    return M_PowerLock->release();
}