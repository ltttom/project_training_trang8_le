#ifndef COMMUNICATION_MANAGER_ADAPTER_MOCK_H
#define COMMUNICATION_MANAGER_ADAPTER_MOCK_H

// using namespace ecallapp;

class MockCommunicationManagerAdapter : public CommunicationManagerAdapter {
public:
    MOCK_METHOD0(registerService, void());
    MOCK_METHOD2(requestSendACKToMCU, void(const uint32_t& prev, const uint32_t& curr));
    MOCK_METHOD0(getService, android::sp<ICommunicationManagerService>());
    MOCK_METHOD1(onBinderDied, void(const android::wp<android::IBinder>& who));
    MOCK_METHOD1(onCommReceive, void(const sp<CommunicationData>& commData));
};

std::shared_ptr<CommunicationManagerAdapter> CommunicationManagerAdapter::instance = nullptr;
CommunicationManagerAdapter* CommunicationManagerAdapter::getInstance() 
{
    if (instance == nullptr) {
        instance = std::make_shared<CommunicationManagerAdapter>();
    }
    return instance.get();
}

MockCommunicationManagerAdapter* M_CommunicationManagerAdapter;

CommunicationManagerAdapter::CommunicationManagerAdapter() {}

void CommunicationManagerAdapter::registerService() {
    M_CommunicationManagerAdapter->registerService();
}

void CommunicationManagerAdapter::requestSendACKToMCU(const uint32_t& prev, const uint32_t& curr) {
    M_CommunicationManagerAdapter->requestSendACKToMCU(prev, curr);
}

android::sp<ICommunicationManagerService> CommunicationManagerAdapter::getService() {
    return M_CommunicationManagerAdapter->getService();
}

void CommunicationManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who) {
    M_CommunicationManagerAdapter->onBinderDied(who);
}

void CommunicationManagerAdapter::onCommReceive(const sp<CommunicationData>& commData) {
    M_CommunicationManagerAdapter->onCommReceive(commData);
}

#endif // COMMUNICATION_MANAGER_ADAPTER_MOCK_H