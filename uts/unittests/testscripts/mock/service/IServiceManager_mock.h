namespace android {

class MockIServiceManager {
public:
    MOCK_CONST_METHOD1(getService, sp<IBinder>(const String16&));
    MOCK_CONST_METHOD1(checkService, sp<IBinder>(const String16& name));
    MOCK_METHOD3(addService, status_t(const String16&, const sp<IBinder>&, bool));
    MOCK_METHOD0(listServices, Vector<String16>());
    MOCK_METHOD0(defaultServiceManager,android::sp<IServiceManager>());
    MOCK_METHOD0(android_IBinder_isBinderAliveProxy,bool());
};

MockIServiceManager * M_IServiceManager;

class MockBnServiceManager {
public:
    MOCK_METHOD4(onTransact, status_t(uint32_t, const Parcel&, Parcel*, uint32_t));
};

MockBnServiceManager * M_BnServiceManager;

sp<IServiceManager> defaultServiceManager()
{
    IBinder* pBinder = new BpBinder(0);
    sp<IServiceManager> retVal = interface_cast<IServiceManager>(pBinder);
    return retVal;
}

class ITest_Binder : public android::IBinder
{
    public:
    ITest_Binder() {};
    virtual ~ITest_Binder() {};
    virtual const android::String16& getInterfaceDescriptor() const{};
    virtual bool isBinderAlive() const {
        return M_IServiceManager->android_IBinder_isBinderAliveProxy();
    };
    virtual status_t pingBinder() {};
    virtual status_t dump(int fd, const Vector<android::String16>& args) {};
    virtual status_t transact( uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0) {};
    virtual status_t linkToDeath(const android::sp<DeathRecipient>& recipient, void* cookie = NULL, uint32_t flags = 0) { };
    virtual status_t unlinkToDeath( const android::wp<DeathRecipient>& recipient, void* cookie = NULL, uint32_t flags = 0, android::wp<DeathRecipient>* outRecipient = NULL)
    {
        return 0;
    };
    virtual void attachObject( const void* objectID, void* object, void* cleanupCookie, object_cleanup_func func) {};
    virtual void* findObject(const void* objectID) const {};
    virtual void detachObject(const void* objectID) {};
    //error_t registerSystemPostReceiver (const android::sp<android::ISystemPostReceiver> & receiver0, const int32_t post1)
    //{
     //   return 0;
    //};
};
class ITest_ServiceManager : public android::IServiceManager {
public:
    sp<android::IBinder> getService( const android::String16& name0) const {
        return M_IServiceManager->getService(name0);
    };
    sp<android::IBinder> checkService( const android::String16& name) const { };
    status_t addService( const android::String16& name, const sp<android::IBinder>& service, bool allowIsolated = false){ };
    Vector<android::String16> listServices(){ };
    ITest_Binder* onAsBinder() { };
};

// ----------------------------------------------------------------------

class BpServiceManager : public BpInterface<IServiceManager>
{
public:
    BpServiceManager(const sp<IBinder>& impl)
        : BpInterface<IServiceManager>(impl)
    {
    }

    virtual sp<IBinder> getService(const String16& name) const
    {
    return M_IServiceManager->getService(name);
    }

    virtual sp<IBinder> checkService( const String16& name) const
    {
    return M_IServiceManager->checkService(name);
    }  
    virtual status_t addService(const String16& name, const sp<IBinder>& service,
            bool allowIsolated)
    {
    return M_IServiceManager->addService(name, service, allowIsolated);
    }

    virtual Vector<String16> listServices()
    {
    return M_IServiceManager->listServices();
    }
};

IMPLEMENT_META_INTERFACE(ServiceManager, "android.os.IServiceManager");

// ----------------------------------------------------------------------


status_t BnServiceManager::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    return M_BnServiceManager->onTransact(code, data, reply, flags);
}
}  // namespace android
