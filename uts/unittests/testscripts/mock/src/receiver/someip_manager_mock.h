namespace oc {

class MocksomeIP_manager : public someIP_manager {
    public:
        MOCK_METHOD0(registerSomeipProviderReceiver, void());
        //MOCK_METHOD1(onBinderDied, void(const android::wp<android::IBinder>& service));
        //MOCK_METHOD3(RxDataToApp, void(EnumSomeipRxMsgID& kSomeipRxMsgID, size_t& bufSomeipDataLen, uint8_t* bufSomeipData));
        MOCK_METHOD0(getInstance, someIP_manager *());
        //MOCK_METHOD1(initHandle, void(android::sp<OemCallHandler> handler));
};

MocksomeIP_manager *M_someIP_manager;

someIP_manager::someIP_manager()
{
}

void someIP_manager::registerSomeipProviderReceiver() {
    return M_someIP_manager->registerSomeipProviderReceiver();
}

void someIP_manager::onBinderDied(const android::wp<android::IBinder>& service) {
    //return M_someIP_manager->onBinderDied(service);
}

void someIP_manager::RxDataToApp(EnumSomeipRxMsgID& kSomeipRxMsgID, size_t& bufSomeipDataLen, uint8_t* bufSomeipData) {
    //return M_someIP_manager->RxDataToApp(kSomeipRxMsgID, bufSomeipDataLen, bufSomeipData);
}

someIP_manager * someIP_manager::getInstance() {
    return M_someIP_manager->getInstance();
}

void someIP_manager::initHandle(android::sp<OemCallHandler> handler) {
    //return M_someIP_manager->initHandle(handler);
}

error_t someIP_manager::sendToICB(EnumSomeipTxMsgID id, uint8_t data) {
    //return M_someIP_manager->initHandle(handler);
}

}  // namespace oc

namespace oc {

class MockIcbProviderReceiver : public IcbProviderReceiver {
    public:
};

MockIcbProviderReceiver *M_IcbProviderReceiver;

}  // namespace oc

