#ifndef SOMEIP_MANAGER_H
#define SOMEIP_MANAGER_H

// app manager service
#include <services/ApplicationManagerService/IApplicationManagerService.h>
#include <services/ApplicationManagerService/IApplicationManagerServiceType.h>
#include <services/ApplicationManagerService/ISystemPostReceiver.h>

  // someip provider
#include <services/SomeipProviderManagerService/SomeipProviderManager.h>
#include <services/SomeipProviderManagerService/SomeipProviderCommand.h>
#include <services/SomeipProviderManagerService/EnumSomeipRxMsgID.h>
#include <services/SomeipProviderManagerService/EnumSomeipTxMsgID.h>
#include <services/SomeipProviderManagerService/ITidlSomeipProvider.h>
#include <utils/RefBase.h>
#include "../oem_call_handler.h"
#include "../oem_call_app_constants.h"

class IcbProviderReceiver;

namespace oc {
class someIP_manager : public android::RefBase
{
public:
    someIP_manager();
    void registerSomeipProviderReceiver();
    void onBinderDied(const android::wp<android::IBinder>& service);
    void RxDataToApp(EnumSomeipRxMsgID& kSomeipRxMsgID, size_t& bufSomeipDataLen, uint8_t* bufSomeipData);
    static someIP_manager *getInstance();
    void initHandle(android::sp<OemCallHandler> handler);
    static error_t sendToICB(EnumSomeipTxMsgID id, uint8_t data);

private:
    sp<ISomeipProviderManagerService> mSomeipProviderMgr;
    static someIP_manager *s_someIPMgr;
    android::sp<OemCallHandler> mHandler = nullptr;
    android::sp<ITidlSomeipProvider> mProviderReceiver;
    android::sp<ServiceDeathRecipient> mServiceDeathRecipient;
};


class IcbProviderReceiver : public BnTidlSomeipProvider
{
public:
    IcbProviderReceiver(someIP_manager& rcvMgr) : mRcvMgr(rcvMgr) {}
    virtual void RxDataToApp(EnumSomeipRxMsgID& kSomeipRxMsgID, size_t& bufSomeipDataLen, uint8_t* bufSomeipData) {
        return mRcvMgr.RxDataToApp(kSomeipRxMsgID, bufSomeipDataLen, bufSomeipData);
    }
private:
    someIP_manager& mRcvMgr;
};
}
#endif // SOMEIP_MANAGER_H
