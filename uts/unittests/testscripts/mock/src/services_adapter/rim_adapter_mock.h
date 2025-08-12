namespace oc {

class MockRIMAdapter : public RIMAdapter {
    public:
    MockRIMAdapter() : RIMAdapter(nullptr) {}
    MOCK_METHOD2(send, void(const std::string& payload, ESendType type));
    MOCK_METHOD0(stop, void());
};

MockRIMAdapter *M_RIMAdapter;

RIMAdapter::RIMAdapter(android::sp<OemCallHandler> spHandler)
{
}

void RIMAdapter::send(const std::string& payload, ESendType type) {
    return M_RIMAdapter->send(payload, type);
}

void RIMAdapter::stop() {
    return M_RIMAdapter->stop();
}
}  // namespace oc
