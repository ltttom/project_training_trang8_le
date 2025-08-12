namespace oc {

class MockOEMCallApp : public OEMCallApp {
    public:
        //MOCK_METHOD0(onCreate, void());
        //MOCK_METHOD0(onDestroy, void());
        //MOCK_METHOD2(onActive, void(const int32_t param, std::string param2));
        //MOCK_METHOD2(onInactive, void(const int32_t param, std::string param2));
        MOCK_METHOD2(onFeatureStatusChanged, error_t(const std::string feature, const FeatureStatus status));
        MOCK_METHOD2(onFeatureActionPerformed, error_t(const FeatureAction action, const std::string feature));
        //MOCK_METHOD1(onPostReceived, void(const sp<Post>& post));
        //MOCK_METHOD2(onHMIReceived, void(const uint32_t type, const uint32_t action));
        //MOCK_METHOD2(doOemCallHandler, void(uint32_t what, const android::sp<sl::Message>& msg));
        //MOCK_METHOD1(doHandleRetryRegisterReceiver, void(uint32_t who));
        //MOCK_METHOD1(onSLDDTest, void(const uint32_t what));
        //MOCK_METHOD0(doBootCompleted, void());
        //MOCK_METHOD0(updateFeatureObject, void());
        //MOCK_METHOD1(runTest, void(const uint32_t testId));
};

MockOEMCallApp *M_OEMCallApp;

OEMCallApp::OEMCallApp() :
        isOEMCallCreated_(false)
    {
}

OEMCallApp::~OEMCallApp() {
}

void OEMCallApp::onCreate() {
    //return M_OEMCallApp->onCreate();
}

void OEMCallApp::onDestroy() {
    //return M_OEMCallApp->onDestroy();
}

void OEMCallApp::onActive(const int32_t param, std::string param2) {
    //return M_OEMCallApp->onActive(param, param2);
}

void OEMCallApp::onInactive(const int32_t param, std::string param2) {
    //return M_OEMCallApp->onInactive(param, param2);
}

error_t OEMCallApp::onFeatureStatusChanged(const std::string feature, const FeatureStatus status) {
    return M_OEMCallApp->onFeatureStatusChanged(feature, status);
}

error_t OEMCallApp::onFeatureActionPerformed(const FeatureAction action, const std::string feature) {
    return M_OEMCallApp->onFeatureActionPerformed(action, feature);
}

void OEMCallApp::onPostReceived(const sp<Post>& post) {
    //return M_OEMCallApp->onPostReceived(post);
}

void OEMCallApp::onHMIReceived(const uint32_t type, const uint32_t action) {
    //return M_OEMCallApp->onHMIReceived(type, action);
}

void OEMCallApp::doOemCallHandler(uint32_t what, const android::sp<sl::Message>& msg) {
    //return M_OEMCallApp->doOemCallHandler(what, msg);
}

void OEMCallApp::doHandleRetryRegisterReceiver(uint32_t who) {
    //return M_OEMCallApp->doHandleRetryRegisterReceiver(who);
}

void OEMCallApp::onSLDDTest(const uint32_t what) {
    //return M_OEMCallApp->onSLDDTest(what);
}

void OEMCallApp::doBootCompleted() {
    //return M_OEMCallApp->doBootCompleted();
}

//void OEMCallApp::updateFeatureObject() {
//    //return M_OEMCallApp->updateFeatureObject();
//}

void OEMCallApp::runTest(const uint32_t testId) {
    //return M_OEMCallApp->runTest(testId);
}

}  // namespace oc

