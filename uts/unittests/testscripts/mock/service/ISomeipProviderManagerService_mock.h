class MockISomeipProviderManagerService {
public:
// registerReceiverTidlSomeipProviderRxDataToApp : TidlSomeipProvider receiver RxDataToApp receiver_function
MOCK_METHOD2(registerReceiverTidlSomeipProviderRxDataToApp, error_t(const android::sp<ITidlSomeipProvider>& receiver
    , EnumSomeipRxMsgID kSomeipRxMsgID
));
//MOCK_METHOD2(registerReceiverITKEYIT2KEY, error_t(serviceid_t id, android::sp<ISomeipProviderManagerReceiver>& receiver));
// error_t registerReceiverITKEYIT2KEY(serviceid_t id, android::sp<ISomeipProviderManagerReceiver>& receiver);

// registerReceiverTidlSomeipProviderRxDataToApp : TidlSomeipProvider receiver RxDataToApp receiver_function
MOCK_METHOD2(unregisterReceiverTidlSomeipProviderRxDataToApp, error_t(const android::sp<ITidlSomeipProvider>& receiver
    , EnumSomeipRxMsgID kSomeipRxMsgID
));
//MOCK_METHOD1(unregisterReceiverITKEYIT2KEY, error_t(const android::sp<ISomeipProviderManagerReceiver>& receiver));
// error_t unregisterReceiverITKEYIT2KEY(const android::sp<ISomeipProviderManagerReceiver>& receiver);

// auto CGA start : wishtoUseAPI
MOCK_METHOD3(TidlSomeipProSendToICB,
    error_t(
        EnumSomeipTxMsgID& kSomeipTxMsgID
        , size_t& bufSomeipDataLen
        , uint8_t* bufSomeipData
    ));
// auto CGA end : wishtoUseAPI

//MOCK_METHOD2(wishtoUseAPI_1, error_t(int32_t switchOn , int32_t switchOff));
//MOCK_METHOD1(wishtoUseAPI_2, error_t(int32_t switchOff));
//MOCK_METHOD2(wishtoUseAPI_3, error_t(uint32_t setDTC , android::sp<SomeipProviderData>& xxData));
//MOCK_METHOD1(wishtoUseAPI_4, error_t(android::sp<SomeipProviderData>& xxData));
//MOCK_METHOD1(wishtoUseAPI_5, error_t(android::sp<SomeipProviderData>& xxData));
//MOCK_METHOD1(wishtoUseAPI_6, error_t(android::sp<SomeipProviderData>& xxData));
//MOCK_METHOD1(wishtoUseAPI_7, error_t(android::sp<SomeipProviderData>& xxData));
//MOCK_METHOD1(wishtoUseAPI_8, error_t(android::sp<SomeipProviderData>& xxData));


// RECEIVER TEST API




// 3 should equal to  utsArgCount:3
MOCK_METHOD3( testRxDataToApp , error_t(
    EnumSomeipRxMsgID& kSomeipRxMsgID
    , size_t& bufSomeipDataLen
    , uint8_t* bufSomeipData
    ));


};

class MockBnSomeipProviderManagerService {
public:
MOCK_METHOD4(onTransact, android::status_t(uint32_t code, const android::Parcel& data, android::Parcel* reply, uint32_t flags));
};



MockISomeipProviderManagerService * M_ISomeipProviderManagerService;

MockBnSomeipProviderManagerService * M_BnSomeipProviderManagerService;

android::status_t BnSomeipProviderManagerService::onTransact(uint32_t code, const android::Parcel& data, android::Parcel* reply, uint32_t flags)
{
return M_BnSomeipProviderManagerService->onTransact(code, data, reply, flags);
}


class ITestISomeipProviderManagerService : public BnSomeipProviderManagerService
{
public:
//virtual error_t registerReceiverITKEYIT2KEY(serviceid_t id, android::sp<ISomeipProviderManagerReceiver>& receiver) {
    //return M_ISomeipProviderManagerService->registerReceiverITKEYIT2KEY(id, receiver);
//}
// registerReceiverTidlSomeipProviderRxDataToApp : TidlSomeipProvider receiver RxDataToApp receiver_function
virtual error_t registerReceiverTidlSomeipProviderRxDataToApp(
    const android::sp< ITidlSomeipProvider >& receiver       // ITidlSomeipProvider receiver class
    , EnumSomeipRxMsgID kSomeipRxMsgID
)
{
    return M_ISomeipProviderManagerService->registerReceiverTidlSomeipProviderRxDataToApp(
        receiver
        , kSomeipRxMsgID
        );
}

//virtual error_t unregisterReceiverITKEYIT2KEY( const android::sp<ISomeipProviderManagerReceiver>& receiver) {
    //return M_ISomeipProviderManagerService->unregisterReceiverITKEYIT2KEY(receiver);
//}
// unregisterReceiverTidlSomeipProviderRxDataToApp : TidlSomeipProvider receiver RxDataToApp receiver_function
virtual error_t unregisterReceiverTidlSomeipProviderRxDataToApp(
    const android::sp< ITidlSomeipProvider >& receiver       // ITidlSomeipProvider receiver class
    , EnumSomeipRxMsgID kSomeipRxMsgID
)
{
    return M_ISomeipProviderManagerService->unregisterReceiverTidlSomeipProviderRxDataToApp(
        receiver
        , kSomeipRxMsgID
        );
    }

    // auto CGA start : wishtoUseAPI
    virtual error_t TidlSomeipProSendToICB(
        EnumSomeipTxMsgID& kSomeipTxMsgID
        , size_t& bufSomeipDataLen
        , uint8_t* bufSomeipData
    )
    {
    return M_ISomeipProviderManagerService->TidlSomeipProSendToICB(
            kSomeipTxMsgID
            , bufSomeipDataLen
            , bufSomeipData
            );
    }
    // auto CGA end : wishtoUseAPI


    // receiver test API start
    virtual error_t testRxDataToApp(
        EnumSomeipRxMsgID& kSomeipRxMsgID
        , size_t& bufSomeipDataLen
        , uint8_t* bufSomeipData
        )
    {
    return M_ISomeipProviderManagerService->testRxDataToApp(
            kSomeipRxMsgID
            , bufSomeipDataLen
            , bufSomeipData
        );
    }
    // receiver test API end


// CGA_VARIANT:ISomeipProviderManagerService:ISomeipProviderManagerService() START
    /*
    * Write your own code
    */
// CGA_VARIANT:ISomeipProviderManagerService:ISomeipProviderManagerService() END

};
 