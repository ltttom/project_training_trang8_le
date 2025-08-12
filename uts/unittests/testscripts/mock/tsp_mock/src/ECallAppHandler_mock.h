#ifndef ECALLAPPHANDLE_MOCK_H
#define ECALLAPPHANDLE_MOCK_H

class MockECallAppHandler : public ECallAppHandler {
public:
    MockECallAppHandler(sp<sl::SLLooper>& looper, IECallAppIf& app) noexcept :
        ECallAppHandler(looper, app) {}
    MOCK_METHOD1(handleMessage, void(const sp<sl::Message>& handlemsg));
    MOCK_METHOD1(handleMessageInternal, void(const sp<sl::Message>& handlemsg));
    MOCK_METHOD1(init, void(ECallAppHandler* const));
    MOCK_METHOD1(obtainMessage, android::sp<sl::Message>(int32_t what));
    MOCK_METHOD2(sendMessageDelayed, bool(const sp<sl::Message>& sendmsg, uint64_t delayMs));
    MOCK_METHOD0(getInstance, ECallAppHandler*());
};

class IECallAppIfMock: public IECallAppIf {
public:
    IECallAppIfMock() = default;
    ~IECallAppIfMock() = default;

    void doECallhandler(const int32_t& what, const android::sp<sl::Message>& message) override {}
    void doECallhandlerInternal(const int32_t& what, const android::sp<sl::Message>& message) override {}
};

static android::sp<sl::SLLooper> mLooper = new sl::SLLooper();
MockECallAppHandler *M_ECallAppHandler;
static bool mockGetInstance = false;

ECallAppHandler* ECallAppHandler::instance = nullptr;

ECallAppHandler* ECallAppHandler::getInstance()
{
    if (!mockGetInstance) {
        if (instance != nullptr) {
            return instance;
        } else {
            IECallAppIfMock app;
            android::sp<sl::SLLooper> mLooper = new sl::SLLooper();
            return new MockECallAppHandler(mLooper,app);
        }
    } else {
        return M_ECallAppHandler->getInstance();
    }
}

void ECallAppHandler::handleMessage(const sp<sl::Message>& handlemsg) {
    M_ECallAppHandler->handleMessage(handlemsg);
}

void ECallAppHandler::handleMessageInternal(const sp<sl::Message>& handlemsg) {
    M_ECallAppHandler->handleMessageInternal(handlemsg);
}

void ECallAppHandler::init(ECallAppHandler* const handler) const{
    M_ECallAppHandler->init(handler);
}

android::sp<sl::Message> sl::Handler::obtainMessage(int32_t what, int32_t arg1) {
    return M_ECallAppHandler->obtainMessage(what);
}

android::sp<sl::Message> sl::Handler::obtainMessage(int32_t what) {
    return M_ECallAppHandler->obtainMessage(what);
}
android::sp<sl::Message> sl::Handler::obtainMessage(int32_t what, void* obj) {
    return M_ECallAppHandler->obtainMessage(what);
}
android::sp<sl::Message> sl::Handler::obtainMessage(int32_t what, android::sp<android::RefBase> spRef) {
    return M_ECallAppHandler->obtainMessage(what);
}
android::sp<sl::Message> sl::Handler::obtainMessage(int32_t what, int32_t arg1, int32_t arg2) {
    return M_ECallAppHandler->obtainMessage(what);
}
android::sp<sl::Message> sl::Handler::obtainMessage(int32_t what, int32_t arg1, int32_t arg2, void* obj) {
    return M_ECallAppHandler->obtainMessage(what);
}
android::sp<sl::Message> sl::Handler::obtainMessage(int32_t what, int32_t arg1, int32_t arg2, int32_t arg3) {
    return M_ECallAppHandler->obtainMessage(what);
}

bool sl::Handler::sendMessageDelayed(const sp<Message>& sendmsg, uint64_t delayMs) {
    return M_ECallAppHandler->sendMessageDelayed(sendmsg, delayMs);
}

#endif // ECALLAPPHANDLE_MOCK_H