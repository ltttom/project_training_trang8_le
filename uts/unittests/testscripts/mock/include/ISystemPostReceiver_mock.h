class MockISystemPostReceiver : public ISystemPostReceiver {
    public:
        MOCK_METHOD1(onSystemPostReceived, bool(const android::sp<Post>& systemPost));
};

MockISystemPostReceiver *M_ISystemPostReceiver;

ISystemPostReceiver::ISystemPostReceiver(){}
ISystemPostReceiver::~ISystemPostReceiver(){}

bool ISystemPostReceiver::onSystemPostReceived(const android::sp<Post>& systemPost) {
    return M_ISystemPostReceiver->onSystemPostReceived(systemPost);
}


class MockBnSystemPostReceiver : public BnSystemPostReceiver {
    public:
        MOCK_METHOD4(onTransact, android::status_t(uint32_t code, const android::Parcel& data, android::Parcel* reply, uint32_t flags));
};

MockBnSystemPostReceiver *M_BnSystemPostReceiver;

android::status_t BnSystemPostReceiver::onTransact(uint32_t code, const android::Parcel& data, android::Parcel* reply, uint32_t flags) {
    return M_BnSystemPostReceiver->onTransact(code, data, reply, flags);
}


