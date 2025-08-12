#ifndef ANDROID_MOCK_H
#define ANDROID_MOCK_H

class MockAndroidService {
public:
    MOCK_METHOD0(defaultServiceManager,android::sp<IServiceManager>());
    MOCK_METHOD1(getService,android::IBinder*(const android::String16 &  name0));   
    MOCK_METHOD0(sl_Message_sendToTargetProxy,bool());
    MOCK_METHOD0(android_IBinder_isBinderAliveProxy,bool());

    MOCK_METHOD0(Buffer_data, uint8_t*());
};

MockAndroidService* M_MockAndroidService;

class ITest_ServiceManager : public android::IServiceManager {
public:
    sp<android::IBinder> getService( const android::String16& name0) const {
        return M_MockAndroidService->getService(name0);
    };
    sp<android::IBinder> checkService( const android::String16& name) const { };
    status_t addService( const android::String16& name, const sp<android::IBinder>& service, bool allowIsolated = false){ };
    Vector<android::String16> listServices(){ };
    IBinder* onAsBinder() {
        return nullptr;
     };
};

android::sp<android::IServiceManager> android::defaultServiceManager()
{
    return M_MockAndroidService->defaultServiceManager();
}

#endif //ANDROID_MOCK_H