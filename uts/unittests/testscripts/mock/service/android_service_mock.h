#include <services/CommunicationManagerService/ICommunicationManagerService.h>
#include <services/MqttManagerService/IMqttManagerReceiver.h>
#include <services/VehicleManagerService/VehicleData.h>
#include <services/VehicleManagerService/IVehicleNetworkReceiver.h>
#include <services/VehicleManagerService/IVehicleReceiver.h>
#include <services/VehicleManagerService/IVehicleManagerService.h>
#include <services/MqttManagerService/IMqttManagerService.h>

class MockAndroidService {
public:
    MOCK_METHOD0(defaultServiceManager,android::sp<IServiceManager>());
    MOCK_METHOD1(getService,android::IBinder*(const android::String16 &  name0));   
    // MOCK_METHOD2(sl_Handler_obtainMessageProxy_2,sl::Message*(int32_t what0, void* obj));
    // MOCK_METHOD1(sl_Handler_obtainMessageProxy,sl::Message*(int32_t what0));
    MOCK_METHOD0(sl_Message_sendToTargetProxy,bool());
    MOCK_METHOD0(android_IBinder_isBinderAliveProxy,bool());

    MOCK_METHOD0(Buffer_data, uint8_t*());
    // MOCK_METHOD0(Buffer_size, uint32_t());
    // MOCK_METHOD2(sl_Handler_sendMessageDelayedProxy,bool(const android::sp<Message>& message, int64_t delayMs));
};

MockAndroidService *M_MockAndroidService;

// SystemService::SystemService(const char *name)
// {
// }

// SystemService::~SystemService()
// {
// }

// bool sl::Message::sendToTarget ()
// {
//     return M_MockAndroidService->sl_Message_sendToTargetProxy();
// }

// error_t SystemService::onStart()
// {
// }

// sl::Handler::Handler ()
// {
// }

// sl::Handler::Handler (android::sp<sl::SLLooper> &  looper0)
// {
// }

class ITest_Binder : public android::IBinder
{
    public:
    ITest_Binder() {};
    virtual ~ITest_Binder() {};
    virtual const android::String16& getInterfaceDescriptor() const{};
    virtual bool isBinderAlive() const {
        return M_MockAndroidService->android_IBinder_isBinderAliveProxy();
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
        return M_MockAndroidService->getService(name0);
    };
    sp<android::IBinder> checkService( const android::String16& name) const { };
    status_t addService( const android::String16& name, const sp<android::IBinder>& service, bool allowIsolated = false){ };
    Vector<android::String16> listServices(){ };
    ITest_Binder* onAsBinder() { };
};

android::sp<android::IServiceManager> android::defaultServiceManager()
{
    return M_MockAndroidService->defaultServiceManager();
}

// android::sp<sl::Message> sl::Handler::obtainMessage(int32_t what0)
// {
//     return M_MockAndroidService->sl_Handler_obtainMessageProxy(what0);
// }

// android::sp<sl::Message> sl::Handler::obtainMessage(int32_t what0, void* obj)
// {
//     return M_MockAndroidService->sl_Handler_obtainMessageProxy_2(what0, obj);
// }

// bool sl::Handler::sendMessageDelayed(const android::sp<Message>& message, int64_t delayMs)
// {
//     //return M_RemoteIfManagerService->sl_Handler_sendMessageDelayedProxy(message, delayMs);
//     return true;
// }

// error_t sl::SLLooper::start(bool attachCurrentThread)
// {
//      return E_OK;
// }

class testVehicleManagerService : public BnVehicleManagerService {
    virtual error_t registerVehicleReceiver(uint32_t moduleID, sp<IVehicleReceiver>& receiver){};
	virtual error_t unregisterVehicleReceiver(uint32_t moduleID){};
	virtual error_t registerVehicleNetworkReceiver(uint32_t moduleID, sp<IVehicleNetworkReceiver>& receiver){};
	virtual error_t unregisterVehicleNetworkReceiver(uint32_t moduleID){};
	virtual error_t addSignalIndex(uint32_t moduleID, uint32_t dataID){};
	virtual error_t removeSignalIndex(uint32_t moduleID, uint32_t dataID){};
	virtual error_t addFrameIndex(uint32_t moduleID, uint32_t frameID){};
	virtual	error_t removeFrameIndex(uint32_t moduleID, uint32_t frameID){};
	virtual error_t addFrameIndexForFrameId(uint32_t moduleID, int64_t frameID, uint8_t frequency = 0U /* ON_CHANGE */){};
	virtual error_t removeFrameIndexForFrameId(uint32_t moduleID, long long frameID){};
    virtual error_t addSignalIndexForTimeout(uint32_t moduleID, uint32_t dataID){};
    virtual error_t removeSignalIndexForTimeout(uint32_t moduleID, uint32_t dataID){};
    virtual error_t addFrameIndexForTimeout(uint32_t moduleID, uint32_t frameID){};
    virtual error_t removeFrameIndexForTimeout(uint32_t moduleID, uint32_t frameID){};
    virtual error_t addFrameIdForTimeout(uint32_t moduleID, int64_t frameID){};
    virtual error_t removeFrameIdForTimeout(uint32_t moduleID, int64_t frameID){};
    
    std::string cmdGetFrameNameFromIndex(uint32_t frameIndex) override {};
    std::string cmdGetSignalNameFromIndex(uint32_t signalIndex) override {};
    error_t cmdSendNetworkStatus(uint32_t channelIndex, uint32_t networkStatus) override {};


	//Change the option if you want to receive valType as TYPE_ERROR for values that have never been received.
	virtual sp<VehicleData> getVehicleValue(uint32_t dataID, uint32_t option);
	virtual sp<VehicleData> getFrameData(long long frameId){};
	virtual error_t sendVehicleData(uint32_t channel, sp<VehicleData>& vehicleData);
	//Please set the value defined in REQ_ACK_DATA_TYPE to the data type.
	virtual error_t sendAckToMcu(uint32_t channel, uint32_t dataType, long long dataID, sp<VehicleData>& vehicleData){};
	virtual uint32_t getVehicleNetworkStatus(uint32_t channel, const std::string& ifname){};
	virtual error_t sendDsfListToMcu(DsfList* list, uint32_t numOfElement){};
	virtual error_t clearVehicleData(uint32_t dataID){};
	virtual int32_t getVMIdxFromSigInfo(long long frameId, int32_t startBytePos, int32_t startBitPos, int32_t length){};
	virtual error_t reqRestrictedMode(int32_t type, int32_t onoff){};
	virtual error_t	reqSpecificVehicleData(int32_t reqType, sp<Buffer>& rcvData){};
	virtual uint32_t getSignalStatus(uint32_t dataID){};
	virtual uint32_t getFrameStatus(int64_t frameID){};

//=============== for sldd command===============
	virtual error_t cmdSendVehicleData(const std::string& name, const std::string& value, const std::string& option){};
	virtual int32_t cmdGetSigIdxFromName(const std::string& name){};
	virtual int32_t cmdGetFrameIdxFromName(const std::string& name){};
	virtual std::string cmdGetSigInitialValue(uint32_t dataID){};
	virtual std::string cmdGetSigDirection(uint32_t dataID){};
	virtual error_t cmdSetSignalLogPrint(int32_t sigIdx, bool onOff){};
	virtual error_t cmdSetFrameLogPrint(int32_t frameIdx, bool onOff){};
    virtual error_t cmdSendRxTimeout(const std::string& name, const std::string& value){};
    virtual int32_t cmdGetRxFrameStartIndex(){};
    virtual int32_t cmdGetRxFrameEndIndex(){};
    virtual int32_t cmdGetRxSignalStartIndex(){};
    virtual int32_t cmdGetRxSignalEndIndex(){};
    virtual int64_t cmdGetRxFrameIDFromIndex(uint32_t frameIndex){};
    public:
    //Fake
    // error_t registerResponseReceiver(
    //      const android::sp< IRemoteDiagReceiver >& receiver,const ModuleId& moduleID){}
};


class MocktestVehicleManagerService {
public:
    MOCK_METHOD2(getVehicleValue, sp<VehicleData>(uint32_t dataID, uint32_t option = 0));
    MOCK_METHOD2(sendVehicleData, error_t(uint32_t channel, sp<VehicleData>& vehicleData));
};

MocktestVehicleManagerService * M_testVehicleManagerService;

error_t testVehicleManagerService::sendVehicleData(uint32_t channel, sp<VehicleData>& vehicleData)
{
    return M_testVehicleManagerService->sendVehicleData(channel, vehicleData);
}
sp<VehicleData> testVehicleManagerService::getVehicleValue (uint32_t dataID, uint32_t option = 0)
{
    return M_testVehicleManagerService->getVehicleValue(dataID, option);
}
