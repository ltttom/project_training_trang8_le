namespace oc {

class MockOemCallHandler : public OemCallHandler {
    public:
        //MOCK_METHOD1(handleMessage, void(const sp<sl::Message>& msg));
};

MockOemCallHandler *M_OemCallHandler;

//OemCallHandler::OemCallHandler(sp<sl::SLLooper>& looper, OEMCallAppIF& app):Handler(looper), app_(app){ }

    
//OemCallHandler::~OemCallHandler() {
//}

void OemCallHandler::handleMessage(const sp<sl::Message>& msg) {
    //return M_OemCallHandler->handleMessage(msg);
}

}  // namespace oc

namespace oc {

class MockAdapterHandler : public AdapterHandler {
    public:
};

MockAdapterHandler *M_AdapterHandler;

}  // namespace oc

namespace oc {

class MockServiceDeathRecipient : public ServiceDeathRecipient {
    public:
};

MockServiceDeathRecipient *M_ServiceDeathRecipient;

}  // namespace oc

