class Test_IApplicationManagerService : public BnApplicationManagerService {
public:
    virtual IBinder* onAsBinder() override;
// #ifndef DOXYGEN_SHOULD_SKIP_THIS
    // DECLARE_META_INTERFACE(ApplicationManagerService);
    // virtual error_t enumerateAvailableApplication(bool bReset=false) override;
    // virtual error_t disableAllApplication(void) override;
    // virtual error_t attachApplication(const char* app, android::sp<android::IBinder> proxy) override;
    // virtual error_t enableApplication(const char* app, int32_t reason, bool bIgnoreMode=false) override;
    // virtual void broadcastHMI(const uint32_t type, const uint32_t action)  override;
    // virtual error_t ReadyToDestroy(const char* app) override;
    // virtual error_t readyToRun(const char* app) override;
// #endif

    virtual error_t sendPost(const char* app, const android::sp<Post>& post) override;
    // virtual error_t broadcastPost(const android::sp<Post>& post) override;
    // virtual error_t broadcastSystemPost(const android::sp<Post>& systemPost) override;
    virtual error_t registerSystemPostReceiver(const android::sp<ISystemPostReceiver>& receiver, const int32_t post) override;
    // virtual error_t unregisterSystemPostReceiver(const android::sp<ISystemPostReceiver>& receiver) override;
    // virtual error_t disableApplication(const char* app, int32_t reason) override;
    // virtual bool isEnabled(const char* app) override;

// #ifndef DOXYGEN_SHOULD_SKIP_THIS
    // virtual void setBootCompleted(bool complete) override;
// #endif

    virtual bool getBootCompleted(bool bPreComplete = false) override;
    // virtual error_t requestActive(std::string app, const int32_t param = -1, const std::string param2 = "") override;
    // virtual error_t requestInactive(std::string app, const int32_t param = -1, const std::string param2 = "") override;
    // virtual bool isActive(std::string app) override;
    // virtual error_t triggerEvent(std::string event, const int32_t param = -1, const std::string param2 = "") override;
    virtual error_t setFeatureStatus(const std::string app, const std::string feature, const FeatureStatus status) override;
    virtual FeatureStatus getFeatureStatus(const std::string feature) override;
    virtual FeatureAction queryActionForFeature(const std::string feature) override;
    // virtual std::string getAllFeaturesStatus(void) override;
    // virtual std::string getActionTableForFeature(std::string feature) override;
    virtual bool isAvailableInRegion(const std::string feature) override;
    // virtual int32_t getHighestPriority() override;
    // virtual int32_t getPriority(std::string feature) override;
    // virtual std::string getFeatureOfPriority(uint32_t priority) override;
    // virtual std::string getAllFeaturesByPriority() override;
    // virtual error_t updateServiceStatus(android::sp<android::IBinder> token, std::uintptr_t handle, int32_t presence, int32_t priority, int32_t properties) override;
    // virtual int32_t getPriorityStatus() override;
    // virtual error_t enableAllApplication(void) override;
};


class MockIApplicationManagerService {
public:
    MockIApplicationManagerService() {};
    ~MockIApplicationManagerService() {};
    MOCK_METHOD0(onAsBinder, IBinder*());

// #ifndef DOXYGEN_SHOULD_SKIP_THIS
    // MOCK_METHOD1(asInterface, android::sp<IApplicationManagerService>(const android::sp<android::IBinder>& obj));
    // MOCK_CONST_METHOD0(getInterfaceDescriptor, const android::String16&());

    // MOCK_METHOD1(enumerateAvailableApplication, error_t(bool));
    // MOCK_METHOD0(disableAllApplication, error_t());
    // MOCK_METHOD2(attachApplication, error_t(const char*, android::sp<android::IBinder>));
    // MOCK_METHOD3(enableApplication, error_t(const char*, int32_t, bool));
    // MOCK_METHOD2(broadcastHMI, void(const uint32_t, const uint32_t));
    // MOCK_METHOD1(ReadyToDestroy, error_t(const char*));
    // MOCK_METHOD1(readyToRun, error_t(const char*));
// #endif

    MOCK_METHOD2(sendPost, error_t(const char*, const android::sp<Post>&));
    // MOCK_METHOD1(broadcastPost, error_t(const android::sp<Post>&));
    // MOCK_METHOD1(broadcastSystemPost, error_t(const android::sp<Post>&));
    MOCK_METHOD2(registerSystemPostReceiver, error_t(const android::sp<ISystemPostReceiver>&, const int32_t));
    // MOCK_METHOD1(unregisterSystemPostReceiver, error_t(const android::sp<ISystemPostReceiver>&));
    // MOCK_METHOD2(disableApplication, error_t(const char*, int32_t));
    // MOCK_METHOD1(isEnabled, bool(const char*));

// #ifndef DOXYGEN_SHOULD_SKIP_THIS
    // MOCK_METHOD1(setBootCompleted, void(bool));
// #endif

    MOCK_METHOD1(getBootCompleted, bool(bool));
    // MOCK_METHOD3(requestActive, error_t(std::string, const int32_t, const std::string));
    // MOCK_METHOD3(requestInactive, error_t(std::string, const int32_t, const std::string));
    // MOCK_METHOD1(isActive, bool(std::string));
    // MOCK_METHOD3(triggerEvent, error_t(std::string, const int32_t, const std::string));
    MOCK_METHOD3(setFeatureStatus, error_t(const std::string, const std::string, const FeatureStatus));
    MOCK_METHOD1(getFeatureStatus, FeatureStatus(const std::string));
    MOCK_METHOD1(queryActionForFeature, FeatureAction(const std::string));
    // MOCK_METHOD0(getAllFeaturesStatus, std::string());
    // MOCK_METHOD1(getActionTableForFeature, std::string(std::string));
    MOCK_METHOD1(isAvailableInRegion, bool(const std::string));
    // MOCK_METHOD0(getHighestPriority, int32_t());
    // MOCK_METHOD1(getPriority, int32_t(std::string));
    // MOCK_METHOD1(getFeatureOfPriority, std::string(uint32_t));
    // MOCK_METHOD0(getAllFeaturesByPriority, std::string());
    // MOCK_METHOD5(updateServiceStatus, error_t(android::sp<android::IBinder>, std::uintptr_t, int32_t, int32_t, int32_t));
    // MOCK_METHOD0(getPriorityStatus, int32_t());
    // MOCK_METHOD0(enableAllApplication, error_t());
};


MockIApplicationManagerService* M_IApplicationManagerService = nullptr;

IBinder* Test_IApplicationManagerService::onAsBinder()
{
    return M_IApplicationManagerService->onAsBinder();
}

error_t Test_IApplicationManagerService::registerSystemPostReceiver(const android::sp<ISystemPostReceiver>& receiver, const int32_t post)
{
    return M_IApplicationManagerService->registerSystemPostReceiver(receiver, post);
}

bool Test_IApplicationManagerService::getBootCompleted(bool bPreComplete)
{
    return M_IApplicationManagerService->getBootCompleted(bPreComplete);
}

error_t Test_IApplicationManagerService::setFeatureStatus(const std::string app, const std::string feature, const FeatureStatus status)
{
    return M_IApplicationManagerService->setFeatureStatus( app, feature, status);
}

FeatureStatus Test_IApplicationManagerService::getFeatureStatus(const std::string feature)
{
    return M_IApplicationManagerService->getFeatureStatus(feature);
}

FeatureAction Test_IApplicationManagerService::queryActionForFeature(const std::string feature)
{
    return M_IApplicationManagerService->queryActionForFeature(feature);
}

bool Test_IApplicationManagerService::isAvailableInRegion(const std::string feature)
{
    return M_IApplicationManagerService->isAvailableInRegion(feature);
}

error_t Test_IApplicationManagerService::sendPost(const char* app, const android::sp<Post>& post)
{
    return M_IApplicationManagerService->sendPost(app, post);
}


