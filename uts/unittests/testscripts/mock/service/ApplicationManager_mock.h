class MockApplicationManager {
 public:
    //MockApplicationManager() : ApplicationManager() {}
    MOCK_METHOD0(instance, ApplicationManager*());
    MOCK_METHOD1(getBootCompleted, bool(bool bPreComplete));
    MOCK_METHOD2(registerReceiver, error_t(const android::sp<ISystemPostReceiver>& receiver, const int32_t post));
};

MockApplicationManager * M_ApplicationManager;

ApplicationManager::ApplicationManager()
{
}

ApplicationManager::~ApplicationManager()
{
}

ApplicationManager* ApplicationManager::instance()
{
    return M_ApplicationManager->instance();
}

error_t ApplicationManager::registerReceiver(const android::sp<ISystemPostReceiver>& receiver, const int32_t post) 
{
    return M_ApplicationManager->registerReceiver(receiver, post);
}

bool ApplicationManager::getBootCompleted (bool bPreComplete)
{
    return M_ApplicationManager->getBootCompleted(bPreComplete);
}