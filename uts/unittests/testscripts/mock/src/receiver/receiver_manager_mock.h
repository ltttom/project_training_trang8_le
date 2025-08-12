namespace oc
{

class MockReceiverManager : public ReceiverManager
{
    public:
        MOCK_METHOD1(init, void(android::sp<OemCallHandler> spHandler));
        MOCK_METHOD0(getHandler, android::sp<OemCallHandler>());
};

MockReceiverManager *M_ReceiverManager;

ReceiverManager::ReceiverManager()
{
}
    
ReceiverManager::~ReceiverManager()
{
}

void ReceiverManager::init(android::sp<OemCallHandler> spHandler)
{
    return M_ReceiverManager->init(spHandler);
}

android::sp<OemCallHandler> ReceiverManager::getHandler()
{
    return M_ReceiverManager->getHandler();
}

void ReceiverManager::requestRetryRegisterReceiver(uint32_t who)
{
    //return M_ReceiverManager->requestRetryRegisterReceiver(who);
}

}  // namespace oc
