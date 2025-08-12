class MockTimeManagerAdapter : public TimeManagerAdapter {
public:
    MOCK_METHOD0(getCurrentTimestamp, int64_t());
    MOCK_METHOD0(registerService, void());
};
MockTimeManagerAdapter* M_TimeManagerAdapter = nullptr;
TimeManagerAdapter::TimeManagerAdapter() {}
int64_t TimeManagerAdapter::getCurrentTimestamp() {
    return M_TimeManagerAdapter->getCurrentTimestamp();
}