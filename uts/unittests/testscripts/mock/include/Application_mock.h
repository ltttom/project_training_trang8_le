class MockApplication : public Application {
    public:
        //MOCK_METHOD1(create, void(Context* context));
        //MOCK_CONST_METHOD0(start, void());
        //MOCK_CONST_METHOD0(kickDog, void());
        //MOCK_METHOD2(active, void(const int32_t param, std::string param2));
        //MOCK_METHOD2(inactive, void(const int32_t param, std::string param2));
        MOCK_METHOD2(setFeatureStatus, error_t(const std::string feature, FeatureStatus status));
        MOCK_METHOD1(getFeatureStatus, FeatureStatus(const std::string feature));
        MOCK_METHOD1(queryActionForFeature, FeatureAction(const std::string feature));
        //MOCK_METHOD0(onCreate, void());
        //MOCK_METHOD0(onDestroy, void());
        //MOCK_METHOD2(onActive, void(const int32_t param, const std::string param2));
        //MOCK_METHOD2(onInactive, void(const int32_t param, const std::string param2));
        //MOCK_METHOD0(onWatchDog, void());
        //MOCK_METHOD1(onPostReceived, void(const sp<Post>& post));
        //MOCK_METHOD2(onHMIReceived, void(const uint32_t type, const uint32_t action));
        MOCK_METHOD2(onFeatureStatusChanged, error_t(const std::string feature, const FeatureStatus status));
        MOCK_METHOD2(onFeatureActionPerformed, error_t(const FeatureAction action, const std::string feature));
        MOCK_METHOD3(onFeatureActionDelivered, error_t(const std::string feature, const FeatureAction action, const std::string causedBy));
        MOCK_METHOD1(dump, error_t(LogOutput& out));
        //MOCK_METHOD0(ReadyToDestroy, void());
        //MOCK_METHOD0(onFirstRef, void());
        MOCK_METHOD0(getApplicationService, sp<IApplicationManagerService>());
};

MockApplication *M_Application;

Application::Application(){}

Application::~Application() {
}

void Application::create(Context* context) {
    //return M_Application->create(context);
}

void Application::start() const{
    //return M_Application->start();
}

void Application::kickDog() const{
    //return M_Application->kickDog();
}

void Application::active(const int32_t param, std::string param2) {
    //return M_Application->active(param, param2);
}

void Application::inactive(const int32_t param, std::string param2) {
    //return M_Application->inactive(param, param2);
}

error_t Application::setFeatureStatus(const std::string feature, FeatureStatus status) {
    return M_Application->setFeatureStatus(feature, status);
}

FeatureStatus Application::getFeatureStatus(const std::string feature) {
    return M_Application->getFeatureStatus(feature);
}

FeatureAction Application::queryActionForFeature(const std::string feature) {
    return M_Application->queryActionForFeature(feature);
}

void Application::onCreate() {
    //return M_Application->onCreate();
}

void Application::onDestroy() {
    //return M_Application->onDestroy();
}

void Application::onActive(const int32_t param, const std::string param2) {
    //return M_Application->onActive(param, param2);
}

void Application::onInactive(const int32_t param, const std::string param2) {
    //return M_Application->onInactive(param, param2);
}

void Application::onWatchDog() {
    //return M_Application->onWatchDog();
}

void Application::onPostReceived(const sp<Post>& post) {
    //return M_Application->onPostReceived(post);
}

void Application::onHMIReceived(const uint32_t type, const uint32_t action) {
    //return M_Application->onHMIReceived(type, action);
}

error_t Application::onFeatureStatusChanged(const std::string feature, const FeatureStatus status) {
    return M_Application->onFeatureStatusChanged(feature, status);
}

error_t Application::onFeatureActionPerformed(const FeatureAction action, const std::string feature) {
    return M_Application->onFeatureActionPerformed(action, feature);
}

error_t Application::onFeatureActionDelivered(const std::string feature, const FeatureAction action, const std::string causedBy) {
    return M_Application->onFeatureActionDelivered(feature, action, causedBy);
}

error_t Application::dump(LogOutput& out) {
    return M_Application->dump(out);
}

void Application::ReadyToDestroy() {
    //return M_Application->ReadyToDestroy();
}

void Application::onFirstRef() {
    //return M_Application->onFirstRef();
}

sp<IApplicationManagerService> Application::getApplicationService() {
    return M_Application->getApplicationService();
}


