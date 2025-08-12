#include "someip_manager.h"
#include "../logger.h"

namespace oc {
someIP_manager::someIP_manager()
{
}

void someIP_manager::initHandle(android::sp<OemCallHandler> handler) {
    LOG_I(DLT_STRING("someIP_manager::initHandle"));
    mHandler = handler;
}

void someIP_manager::registerSomeipProviderReceiver()
{
    /* Register SomeipProviderMgr receiver */
    bool error = true;
    mSomeipProviderMgr = android::interface_cast<ISomeipProviderManagerService>(android::defaultServiceManager()->getService(android::String16(SOMEIPPROVIDER_SRV_NAME)));
    if( mSomeipProviderMgr != nullptr ) {
        mServiceDeathRecipient = new ServiceDeathRecipient();
        //LCOV_EXCL_START 
        mServiceDeathRecipient->registerHandler(
            [&] (const android::wp<android::IBinder>& who) -> void
            {
                getInstance()->onBinderDied(who);
            });
        //LCOV_EXCL_STOP 
        auto result = android::IInterface::asBinder(mSomeipProviderMgr)->linkToDeath(mServiceDeathRecipient);
        if (result == android::OK) {
            LOG_I(DLT_STRING("result == android::OK"));
            mProviderReceiver = new IcbProviderReceiver(*this);
            mSomeipProviderMgr->registerReceiverTidlSomeipProviderRxDataToApp(
                        mProviderReceiver, EnumSomeipRxMsgID::kNotiEthernetStatus);
            mSomeipProviderMgr->registerReceiverTidlSomeipProviderRxDataToApp(
                        mProviderReceiver, EnumSomeipRxMsgID::kRxCallStatusChangeRequest);

            error = false;
        }
    }
    if (error == true) {
        mHandler->sendMessageDelayed(
            mHandler->obtainMessage(
                msg::MSG_CM_RETRY_REGISTER_SOMEIP,
                self::ServiceId::SOMEIP
            ),
            self::SEND_RETRY_DELAY
        );
    }
}

void someIP_manager::onBinderDied(const android::wp<android::IBinder>& service)
{
    if ( service == android::IInterface::asBinder(mSomeipProviderMgr)) {
        LOG_E(DLT_STRING("SomeipProviderMgr Service died!"));
        mHandler->sendMessageDelayed(
            mHandler->obtainMessage(
                msg::MSG_CM_RETRY_REGISTER_SOMEIP,
                self::ServiceId::SOMEIP
            ),
            self::SEND_RETRY_DELAY
        );
        mHandler->obtainMessage(msg::MSG_CM_BINDER_DIED, self::ServiceId::SOMEIP)->sendToTarget();
    }
}

void someIP_manager::RxDataToApp(EnumSomeipRxMsgID& kSomeipRxMsgID, size_t& bufSomeipDataLen, uint8_t* bufSomeipData) {
    EnumSomeipRxMsgID rxId = kSomeipRxMsgID;
    sp<Buffer> buf = new Buffer();
    buf->setTo(bufSomeipData, bufSomeipDataLen);
    if (kSomeipRxMsgID == EnumSomeipRxMsgID::kNotiEthernetStatus) {
        if (buf->data() == nullptr) {
            LOG_E(DLT_STRING("buf->data() is null!!"));
            return;
        }
        uint8_t check = buf->data()[0];
        switch (check) {
            case self::ethernetState::ETHERNET_READY:
                mHandler->obtainMessage(msg::MSG_SOMEIP_PROVIDER_ETH_READY, 1)->sendToTarget();
                break;
            default:
                mHandler->obtainMessage(msg::MSG_SOMEIP_PROVIDER_ETH_READY, 0)->sendToTarget();
                break;
        }
    } else if (kSomeipRxMsgID == EnumSomeipRxMsgID::kRxCallStatusChangeRequest) {
        LOG_I(DLT_STRING("msg::MSG_SOMEIP_REQUEST_CLEARUP_R_CALL"));
        if (buf->data() == nullptr) {
            LOG_E(DLT_STRING("buf->data() is null!!"));
            return;
        }
        mHandler->obtainMessage(msg::MSG_SOMEIP_REQUEST_CLEARUP_R_CALL, buf)->sendToTarget();
    } else {
        LOG_I(DLT_STRING("other kSomeipRxMsgID"));
    }
}


error_t someIP_manager::sendToICB(EnumSomeipTxMsgID id, uint8_t data){
    error_t result = E_ERROR;
    size_t len =1;
    sp<Buffer> buff = new Buffer();
    buff->append(&data, len);

    try {

        result = SomeipProviderManager::instance()->TidlSomeipProSendToICB(id, len,buff->data());
    } catch (...) {
        LOG_E(DLT_STRING("SomeIP service crashed"));
    }

    if (result != E_OK) {
        LOG_E(DLT_STRING("Cannot send to ICB, id = "), DLT_UINT8(id));
    }
    return result;
}

someIP_manager *someIP_manager::s_someIPMgr = nullptr;
someIP_manager *someIP_manager::getInstance()
{
    if(s_someIPMgr == nullptr) {
        s_someIPMgr = new someIP_manager();
    } else {

    }
    return s_someIPMgr;
}
}
