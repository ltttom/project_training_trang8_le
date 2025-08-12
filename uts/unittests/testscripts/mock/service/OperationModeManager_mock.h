class MockOperationModeManager {
public:
    MOCK_METHOD0(instance, OperationModeManager*());
    // MOCK_METHOD2(registerReceiverITKEYIT2KEY, error_t(serviceid_t id, android::sp<IOperationModeManagerReceiver> receiver));
    // registerReceiverOperationModeReceiverOperationModeChanged : OperationModeReceiver receiver operationModeChanged receiver_function
    MOCK_METHOD2(registerReceiverOperationModeReceiverOperationModeChanged, error_t(const android::sp<IOperationModeReceiver>& receiver, EOperationMode OpMode));
        // MOCK_METHOD1(unregisterReceiverITKEYIT2KEY, error_t(const android::sp<IOperationModeManagerReceiver> receiver));
        // registerReceiverOperationModeReceiverOperationModeChanged : OperationModeReceiver receiver operationModeChanged receiver_function
    MOCK_METHOD2(unregisterReceiverOperationModeReceiverOperationModeChanged, error_t(const android::sp<IOperationModeReceiver>& receiver, EOperationMode OpMode));
    MOCK_METHOD0(reregisterReceiver, error_t());
    // auto CGA start : wishtoUseAPI
    MOCK_METHOD1(getOperationMode,error_t(EOperationMode& OpMode));
    MOCK_METHOD1(operationModeChanged,error_t(EOperationMode& OpMode));
    MOCK_METHOD1(setOperationMode,error_t(EOperationMode& OpMode));
    // auto CGA end : wishtoUseAPI

    //MOCK_METHOD2(wishtoUseAPI_1, error_t(int32_t switchOn , int32_t switchOff));
    //MOCK_METHOD1(wishtoUseAPI_2, error_t(int32_t switchOff));
    //MOCK_METHOD2(wishtoUseAPI_3, error_t(uint32_t setDTC , android::sp<OperationModeData>& xxData));
    //MOCK_METHOD1(wishtoUseAPI_4, error_t(android::sp<OperationModeData>& xxData));
    //MOCK_METHOD1(wishtoUseAPI_5, error_t(android::sp<OperationModeData>& xxData));
    //MOCK_METHOD1(wishtoUseAPI_6, error_t(android::sp<OperationModeData>& xxData));
    //MOCK_METHOD1(wishtoUseAPI_7, error_t(android::sp<OperationModeData>& xxData));
    //MOCK_METHOD1(wishtoUseAPI_8, error_t(android::sp<OperationModeData>& xxData));

    MOCK_METHOD1(onBinderDied, void(const android::wp<android::IBinder>& who));
    MOCK_METHOD0(getService, android::sp<IOperationModeManagerService>());
    MOCK_METHOD0(getService_mock, android::sp<IOperationModeManagerService>());
};



MockOperationModeManager * M_OperationModeManager;


OperationModeManager::~OperationModeManager()
{

}

OperationModeManager* OperationModeManager::instance()
{
    return M_OperationModeManager->instance();
}

    // registerReceiverOperationModeReceiverOperationModeChanged : OperationModeReceiver receiver operationModeChanged receiver_function
error_t OperationModeManager::registerReceiverOperationModeReceiverOperationModeChanged(
        const android::sp< IOperationModeReceiver >& receiver       // IOperationModeReceiver receiver class
        , EOperationMode OpMode
)
{
    return M_OperationModeManager->registerReceiverOperationModeReceiverOperationModeChanged(
        receiver
        , OpMode
    );
}

    // unregisterReceiverOperationModeReceiverOperationModeChanged : OperationModeReceiver receiver operationModeChanged receiver_function
error_t OperationModeManager::unregisterReceiverOperationModeReceiverOperationModeChanged(
        const android::sp< IOperationModeReceiver >& receiver       // IOperationModeReceiver receiver class
        , EOperationMode OpMode
)
{
    return M_OperationModeManager->unregisterReceiverOperationModeReceiverOperationModeChanged(
        receiver
        , OpMode
    );
}

error_t OperationModeManager::reregisterReceiver()
{
    return M_OperationModeManager->reregisterReceiver();
}

// auto CGA start : wishtoUseAPI
error_t OperationModeManager::getOperationMode(EOperationMode& OpMode)
{
    return M_OperationModeManager->getOperationMode(OpMode);
}

error_t OperationModeManager::operationModeChanged(
        EOperationMode& OpMode
        )
{
    return M_OperationModeManager->operationModeChanged(
        OpMode
        );
}

error_t OperationModeManager::setOperationMode(
        EOperationMode& OpMode
        )
{
    return M_OperationModeManager->setOperationMode(
        OpMode
        );
}
// auto CGA end : wishtoUseAPI



// error_t OperationModeManager::wishtoUseAPI_1(int32_t switchOn , int32_t switchOff)
// {
//     return M_OperationModeManager->wishtoUseAPI_1(switchOn, switchOff);
// }
// error_t OperationModeManager::getOperationMode(
//         EOperationMode& OpMode
// )
// {
//     return M_OperationModeManager->getOperationMode(
//         OpMode
//     );
// }
// error_t OperationModeManager::operationModeChanged(
//         EOperationMode& OpMode
// )
// {
//     return M_OperationModeManager->operationModeChanged(
//         OpMode
//     );
// }
// error_t OperationModeManager::setOperationMode(
//         EOperationMode& OpMode
// )
// {
//     return M_OperationModeManager->setOperationMode(
//         OpMode
//     );
// }


void OperationModeManager::onBinderDied(const android::wp<android::IBinder>& who)
{
    //M_OperationModeManager->onBinderDied(who);
}

OperationModeManager::OperationModeManager()
{

}

android::sp<IOperationModeManagerService> OperationModeManager::getService()
{
    return M_OperationModeManager->getService();
}

android::sp<IOperationModeManagerService> OperationModeManager::getService_mock()
{
    return M_OperationModeManager->getService();
}
