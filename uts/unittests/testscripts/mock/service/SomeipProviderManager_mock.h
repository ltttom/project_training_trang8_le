class MockSomeipProviderManager {
public:
    MOCK_METHOD0(instance, SomeipProviderManager*());

    MOCK_METHOD3(TidlSomeipProSendToICB, error_t(
            EnumSomeipTxMsgID& kSomeipTxMsgID
            , size_t& bufSomeipDataLen
            , uint8_t* bufSomeipData
            ));
};



MockSomeipProviderManager * M_SomeipProviderManager;

SomeipProviderManager::SomeipProviderManager()
{
    
}

SomeipProviderManager::~SomeipProviderManager()
{

}

SomeipProviderManager* SomeipProviderManager::instance()
{
    return M_SomeipProviderManager->instance();
}

error_t SomeipProviderManager::TidlSomeipProSendToICB(
        EnumSomeipTxMsgID& kSomeipTxMsgID
        , size_t& bufSomeipDataLen
        , uint8_t* bufSomeipData
        )
{
return M_SomeipProviderManager->TidlSomeipProSendToICB(
        kSomeipTxMsgID
        ,  bufSomeipDataLen
        , bufSomeipData
        );
}
