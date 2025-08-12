
class MockBinder : public IBinder {
 public:
  MOCK_CONST_METHOD0(getInterfaceDescriptor, const String16&());
  MOCK_CONST_METHOD0(isBinderAlive, bool());
  MOCK_METHOD0(pingBinder, status_t());
  MOCK_METHOD2(dump, status_t(int fd, const Vector<String16>& args));
  MOCK_METHOD4(transact, status_t(uint32_t, const Parcel&, Parcel*, uint32_t));
  MOCK_METHOD3(linkToDeath, status_t(const sp<IBinder::DeathRecipient>&, void*, uint32_t));
  MOCK_METHOD4(unlinkToDeath, status_t(const wp<IBinder::DeathRecipient>&, void*, uint32_t, wp<IBinder::DeathRecipient>*));
  MOCK_CONST_METHOD1(findObject, void*(const void* objectID));
  MOCK_METHOD4(onTransact, status_t(uint32_t, const Parcel&, Parcel*, uint32_t));
  MOCK_METHOD4(attachObject, void(const void* objectID, void* object, void* cleanupCookie, object_cleanup_func func));
  MOCK_METHOD1(detachObject, void(const void* objectID));
  MOCK_METHOD1(asBinder, android::sp<IBinder>(const sp<IInterface>&));
};

MockBinder * M_Binder;

IBinder::IBinder()
    : RefBase()
{
}

IBinder::~IBinder()
{
}

// ---------------------------------------------------------------------------

sp<IInterface>  IBinder::queryLocalInterface(const String16& /*descriptor*/)
{
    return NULL;
}

BpBinder* IBinder::remoteBinder()
{
    return NULL;
}

bool IBinder::checkSubclass(const void* /*subclassID*/) const
{
    return false;
}

const String16& IBinder::getInterfaceDescriptor() const
{
	return M_Binder->getInterfaceDescriptor();
}

bool IBinder::isBinderAlive() const
{
	return M_Binder->isBinderAlive();
}

status_t IBinder::pingBinder()
{
	return M_Binder->pingBinder();
}

status_t IBinder::dump(int fd, const Vector<String16>& args)
{
	return M_Binder->dump(fd, args);
}

status_t IBinder::transact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
	return M_Binder->transact(code, data, reply, flags);
}

status_t IBinder::linkToDeath(const sp<DeathRecipient>& recipient, void* cookie, uint32_t flags)
{
	return M_Binder->linkToDeath(recipient, cookie, flags);
}

status_t IBinder::unlinkToDeath(const wp<DeathRecipient>& recipient, void* cookie, uint32_t flags, wp<DeathRecipient>* outRecipient)
{
	return M_Binder->unlinkToDeath(recipient, cookie, flags, outRecipient);
}

void IBinder::attachObject(const void* objectID, void* object, void* cleanupCookie, object_cleanup_func func)
{
}

void * IBinder::findObject(const void* objectID) const
{
	return M_Binder->findObject(objectID);
}

void IBinder::detachObject(const void* objectID) {
}

android::sp<IBinder> android::IInterface::asBinder(const sp<IInterface>& a){
    return M_Binder->asBinder(a);
}