#ifndef REMOTEIFMANAGERADAPTER_MOCK_H
#define REMOTEIFMANAGERADAPTER_MOCK_H

// using namespace ecallapp;
class MockRemoteIfManagerAdapter : public RemoteIfManagerAdapter {
public:
    MOCK_METHOD0(getInstance, RemoteIfManagerAdapter*());
    MOCK_METHOD0(getService, android::sp<IRemoteIfManagerService>());
    MOCK_METHOD0(registerService, void());
    MOCK_METHOD2(send, void(const std::string& payload, ESendType type));
    MOCK_METHOD1(onBinderDied, void(const android::wp<android::IBinder>& who));
};

MockRemoteIfManagerAdapter* M_RemoteIfManagerAdapter;

RemoteIfManagerAdapter* RemoteIfManagerAdapter::instance = nullptr;

RemoteIfManagerAdapter* RemoteIfManagerAdapter::getInstance() 
{
    if (instance == nullptr) {
        instance = new RemoteIfManagerAdapter();
    }
    return instance;
}

RemoteIfManagerAdapter::RemoteIfManagerAdapter() {}

void RemoteIfManagerAdapter::registerService() {
    M_RemoteIfManagerAdapter->registerService();
}

android::sp<IRemoteIfManagerService> RemoteIfManagerAdapter::getService() {
    return M_RemoteIfManagerAdapter->getService();
}

void RemoteIfManagerAdapter::send(const std::string& payload, ESendType type) {
    M_RemoteIfManagerAdapter->send(payload, type);
}

void RemoteIfManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who) {
    M_RemoteIfManagerAdapter->onBinderDied(who);
}

class MockRIMReceiver : public RIMReceiver {
public:
    MockRIMReceiver(RemoteIfManagerAdapter& parent) : RIMReceiver(parent) {}
    MOCK_METHOD2(respCertiDownToApp, void(EOrderType orderType, sp<CRespCertiDownData>& respCertiDownData));
    MOCK_METHOD2(respNetConnTestToApp, void(EOrderType orderType, sp<CRespNetConnTestData>& respNetConnTestData));
    MOCK_METHOD2(respOrderToApp, void(EOrderType orderType, sp<CRespOrderData>& respOrderData));
    MOCK_METHOD2(respReplyToApp, void(EOrderType orderType, sp<CRespReplyData>& respReplyData));
    MOCK_METHOD2(respSendToApp, void(ESendType sendType, sp<CRespSendData>& respSendData));
};

#endif // REMOTEIFMANAGERADAPTER_MOCK_H