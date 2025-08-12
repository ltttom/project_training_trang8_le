/**
 * @copyright
 * Copyright (c) 2025 by LG Electronics Inc.
 * This program or software including the accompanying associated documentation
 * ("Software") is the proprietary software of LG Electronics Inc. and or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate written license agreement between you
 * and LG Electronics Inc. ("Authorized License"). Except as set forth in an
 * Authorized License, LG Electronics Inc. grants no license (express or implied),
 * rights to use, or waiver of any kind with respect to the Software, and LG
 * Electronics Inc. expressly reserves all rights in and to the Software and all
 * intellectual property therein. If you have no Authorized License, then you have
 * no rights to use the Software in any ways, and should immediately notify LG
 * Electronics Inc. and discontinue all use of the Software.
 * @file        ECallApp2.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "ECallApp2.h"
#include "logger.h"
#include "utils/ParamsDef.h"
#include "utils/CommonFunctions.h"
#include "services/CommunicationManagerAdapter.h"
#include "services/ApplicationManagerAdapter.h"
#include "services/AudioManagerAdapter.h"
#include "services/VehicleManagerAdapter.h"
#include "services/PowerManagerAdapter.h"
#include "services/TelephoneManagerAdapter.h"
#include "services/HMIManagerAdapter.h"
#include "services/AlarmManagerAdapter.h"
#include "services/HTTPSManagerAdapter.h"
#include "services/ConfigurationManagerAdapter.h"
#include "services/LocationManagerAdapter.h"
#include "services/RemoteIfManagerAdapter.h"
#include "services/ServiceAdapterTimerId.h"
#include "services/SystemPropertyAdapter.h"
#include "services/TimeManagerAdapter.h"
#include "services/VehicleVariationAdapter.h"

namespace ecallapp {

ECallApp2::ECallApp2() {
    mLooper = nullptr;
    mHandler = nullptr;
    mProcessorsMgr = nullptr;
    mIsBootCompleted = false;
    mParrent = nullptr;
}

ECallApp2::~ECallApp2() {
    mIsBootCompleted = false;
    mLooper = nullptr;
}

/*
 * onCreate:
 * This method is called from the oem_call_app.cpp file, which contain the class OEMCallApp.
 * OEMCallApp is to handle the function: RCall and ACN with phone why the ECallApp2 is in charge of NA-ECall, NA-DESS and CPD.
 * This method is for initialization of the ECallApp2.
*/
void ECallApp2::onCreate() {
    LOG_I(DLT_STRING("onCreate ECallApp()"));
    initHandler();
    ApplicationManagerAdapter::getInstance()->registerService();
}

void ECallApp2::setParrent(oc::OEMCallAppIF * parrent){
    mParrent = parrent;
};

/**
 * This method is called when the bootup is completed.
 * It is used to register the services and create the processor manager.
 * It also add a new element of RECV_MSG_BOOT_COMPLETED to the queue, which will be further processed
 * by method doECallhandler.
 */
void ECallApp2::doBootCompleted(const android::sp<sl::Message>& message) {
    LOG_I(DLT_STRING("doBootCompleted"));

    if (!mIsBootCompleted) {
        mIsBootCompleted = true;

        // // Register service
        LOG_I(DLT_STRING("Start Register services"));
        AudioManagerAdapter::getInstance()->registerService();
        VehicleManagerAdapter::getInstance()->registerService();
        PowerManagerAdapter::getInstance()->registerService();
        TelephoneManagerAdapter::getInstance()->registerService();
        HMIManagerAdapter::getInstance()->registerService();
        CommunicationManagerAdapter::getInstance()->registerService();
        AlarmManagerAdapter::getInstance()->registerService();
        HTTPSManagerAdapter::getInstance()->registerService();
        ConfigurationManagerAdapter::getInstance()->registerService();
        LocationManagerAdapter::getInstance()->registerService();
        RemoteIfManagerAdapter::getInstance()->registerService();
        TimeManagerAdapter::getInstance()->registerService();

        // // Create processor manager
        mProcessorsMgr = std::make_shared<ProcessorsManager>();

        // /* Trigger tasks when bootup is completed */
        mProcessorsMgr->handleMessage(ENUM_TO_UINT32(msg::MSG::RECV_MSG_BOOT_COMPLETED), message);

        // Create default directory in non-volatile memory for storing files
        CommonFunctions::createDefaultPathIfNeeded();
    }
}

void ECallApp2::onDestroy()
{
    LOG_I(DLT_STRING("onDestroy"));
    HTTPSManagerAdapter::getInstance()->onDestroy();
    mIsBootCompleted = false;
    mLooper = nullptr;
}

/*
 * initHandler:
 * This method is called from the onCreate method.
 * It is used to initialize the handler for the ECallApp2.
 * The main job of handler is to cyclically extract the message from the message queue and handle it.
*/
void ECallApp2::initHandler()
{
    LOG_I(DLT_STRING("initHandler"));
    if (mLooper == nullptr) {
        mLooper = sl::SLLooper::myLooper();
    }
    if (mHandler == nullptr) {
        mHandler = std::make_shared<ECallAppHandler>(mLooper, *this);
        mHandler->init(mHandler.get());
    }
}

void ECallApp2::onActive(const int32_t param, const std::string param2) noexcept
{
    LOG_I(DLT_STRING("onActive"));
}

void ECallApp2::onInactive(const int32_t param, const std::string param2) noexcept
{
    LOG_I(DLT_STRING("onInactive"));
}

error_t ECallApp2::onFeatureStatusChanged(const std::string feature, const FeatureStatus status)
{
    LOG_I(DLT_STRING("feature = "), DLT_STRING(feature.c_str()), DLT_STRING(", status = "),  DLT_UINT(static_cast<uint32_t>(status)));
    return E_OK;
}

error_t ECallApp2::onFeatureActionPerformed(const FeatureAction action, const std::string feature)
{
    LOG_I(DLT_STRING("action = "), DLT_STRING(APP_MGR_CONFIG::featureActionMap[action].c_str()), DLT_STRING(", feature = "),  DLT_STRING(feature.c_str()));
    auto msg = mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::APPM_RESPONSE_QUERY), action);
    msg->buffer.setTo(const_cast<char*>(feature.c_str()), feature.size());
    msg->sendToTarget();
    return E_OK;
}

error_t ECallApp2::onFeatureActionDelivered(const std::string feature, const FeatureAction action, const std::string causedBy) {
    LOG_I(DLT_STRING("feature = "), DLT_STRING(feature.c_str()), DLT_STRING(", action = "),  DLT_STRING(APP_MGR_CONFIG::featureActionMap[action].c_str()),
    DLT_STRING(", causedBy = "),  DLT_STRING(causedBy.c_str()));
    if((causedBy != "") && (action == FeatureAction::UPDATE)) {
        android::sp<AppMgrEventData> pEventData = new AppMgrEventData(FeatureAction::UPDATE, feature, causedBy);
        mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::APPM_REQUEST_FEATURE_ACTION), pEventData)->sendToTarget();;
    }
    return E_OK;
}

void ECallApp2::sendACKToMCU(const int32_t arg1, const int32_t arg2) {
    CommunicationManagerAdapter::getInstance()->requestSendACKToMCU(
        CommonFunctions::convertInt32ToUint32(arg1),
        CommonFunctions::convertInt32ToUint32(arg2)
    );
}

/**
 * As the ECallApp2 is the main class for the NA-ECall, NA-DESS and CPD, it is responsible for handling the messages from the message queue.
 * The message queue is filled with the messages from the other classes.
 * This method is called when a message is extracted from the message queue.
 */
void ECallApp2::doECallhandler(const int32_t& what, const android::sp<sl::Message>& message)
{
    if (what == ENUM_TO_INT(msg::MSG::APPM_BOOT_COMPLETED)) {
        doBootCompleted(message);
    }
    // Application message
    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_APPLICATIONMGR)) {
        ApplicationManagerAdapter::getInstance()->registerService();
    }
    else if (what == ENUM_TO_INT(msg::MSG::APPM_RESPONSE_QUERY)) {
        ApplicationManagerAdapter::getInstance()->doFeatureActionPerformed(message);
    }
    else if (what == ENUM_TO_INT(msg::MSG::APPM_REQUEST_ACTIONS)) {
        ApplicationManagerAdapter::getInstance()->doHandlePostActionDelivered(message);
    }
    else if (what == ENUM_TO_INT(msg::MSG::APPM_REQUEST_FEATURE_ACTION)) {
        LOG_I(DLT_STRING("APPM_REQUEST_FEATURE_ACTION"));
        ApplicationManagerAdapter::getInstance()->doHandlePostFeatureActionDelivered(message);
    }
    else if (what == ENUM_TO_INT(msg::MSG::APPM_FEATURE_STATUS_CHANGED)) {
        ApplicationManagerAdapter::getInstance()->doHandlePostFeatureStatusChanged(message);
    }
    // (End) Application message

    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_AUDIOMGR)) {
        AudioManagerAdapter::getInstance()->registerService();
    }
    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_AUDIO_PLAY_STATE_CHANGED)) {
        AudioManagerAdapter::getInstance()->handlePlayStateChanged(message->arg1);
    }
    // If register service failed, try to re-register
    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_VCMMGR)) {
        VehicleManagerAdapter::getInstance()->registerService();
    }
    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_SRS_SIGNAL)) {
        VehicleManagerAdapter::getInstance()->handleSRSSignal(message->arg1);
    }
    else if (what == ENUM_TO_INT(msg::MSG::REQ_SEND_BCAN_CANCEL_DISP)) {
        VehicleManagerAdapter::getInstance()->send_BCAN_CANCEL_DISP(message->arg1);
    }
    // Power
    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_POWERMGR)) {
        PowerManagerAdapter::getInstance()->registerService();
    }
    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED)) {
        PowerManagerAdapter::getInstance()->resetPowerLockCounter();
    }
    //end Power
    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_TELEPHONEMGR)) {
        TelephoneManagerAdapter::getInstance()->registerService();
    }
    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_HMIMGR)) {
        HMIManagerAdapter::getInstance()->registerService();
    }
    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_COMMMGR)) {
        CommunicationManagerAdapter::getInstance()->registerService();
    }
    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_LOCATIONMGR)) {
        LocationManagerAdapter::getInstance()->registerService();
    }
    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_CONFIGMGR)) {
        ConfigurationManagerAdapter::getInstance()->registerService();
    }
    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_ALARMMGR)) {
        AlarmManagerAdapter::getInstance()->registerService();
    }
    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_RIMMGR)) {
        RemoteIfManagerAdapter::getInstance()->registerService();
    } else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_TIMEMGR)) {
        TimeManagerAdapter::getInstance()->registerService();
    }
    //http
    else if (what == ENUM_TO_INT(msg::MSG::RECV_MSG_RECEIVED_DATA_NOTIFICATION)) {
        HTTPSManagerAdapter::getInstance()->onReceivedData(message);
    } else if (what == ENUM_TO_INT(msg::MSG::APPM_SLDD_SEND_POST)) {
        slddTest(message->arg1, message->arg2);
    } else if (what == ENUM_TO_INT(msg::MSG::MSG_TIMER_SRV)) {
        if (mProcessorsMgr != nullptr)  {
            mProcessorsMgr->handleTimerTimeoutEvent(message->arg1);
        }
        if ( (message->arg1 == vehicle::TIMER_SRS_FRONT_ID)
        ||(message->arg1 == vehicle::TIMER_SRS_SIDE_ID)
        ||(message->arg1 == vehicle::TIMER_SRS_REAR_ID)
        ||(message->arg1 == vehicle::TIMER_SRS_ROLLOVER_ID)
        ) {
            VehicleManagerAdapter::getInstance()->onTimerTimeoutEvent(message->arg1);
        }
        if (message->arg1 == appmgr::TIMER_2S_QUERYACTIONFORFEATURE_ID) {
            ApplicationManagerAdapter::getInstance()->handleQueryActionForFeatureTimerTimeOut();
        }
    } else {
        LOG_I(DLT_STRING("forward this message to prcessor block"));
    }

    // pass the message to the processors one by one.
    if (mProcessorsMgr != nullptr)  {
        mProcessorsMgr->handleMessage(static_cast<uint32_t>(what), message);
    }
    else {
        LOG_W(DLT_STRING("mProcessorsMgr is null, hold this message for 500 miliseconds"));
        if (mHandler != nullptr) {
            (void)mHandler->sendMessageDelayed(message, timeMsParams::TIME_SEND_RETRY_DELAY_MS);
        }
    }

    // If the message is not NA-ECall, NA-DESS and CPD related, then forward it to the parent class.
    // The parent class is the OEMCallApp class, which is in charge of RCall and ACN with phone.
    if ( (mParrent != nullptr) && (mParrent->isProcessorNullPtr() == false)) {
        if ( (what == ENUM_TO_INT(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER)) || (what == ENUM_TO_INT(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER_ACN_WITH_GTC)) ) {
            const bool isACNWithPhoneAvailable =
                                      VehicleVariationAdapter::getInstance()->isFeatureAvailableInRegion(feature_name::acn_phone);
            if (isACNWithPhoneAvailable) {
                LOG_I(DLT_STRING("ACN with phone heavy"));
                if (mHandler != nullptr) {
                    const sp<sl::Message> tMessage {mHandler->obtainMessage(oc::msg::MSG_CDS_TRIGGER_AFTER_WAITING)};
                    mParrent->doOemCallHandler(CommonFunctions::convertUint32ToInt32(oc::msg::MSG_CDS_TRIGGER_AFTER_WAITING), tMessage);
                }
            }
        }
        else if( (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH)) || (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH_ACN_WITH_GTC)) ) {
            const bool isACNWithPhoneAvailable =
                                      VehicleVariationAdapter::getInstance()->isFeatureAvailableInRegion(feature_name::acn_phone);
            const bool isACNUSAvailable =
                                      VehicleVariationAdapter::getInstance()->isFeatureAvailableInRegion(feature_name::ecall_us)
                                   || VehicleVariationAdapter::getInstance()->isFeatureAvailableInRegion(feature_name::ecall_us_pf3);
            if (isACNWithPhoneAvailable) {
                LOG_I(DLT_STRING("ACN with phone heavy"));
                if (mHandler != nullptr) {
                    const sp<sl::Message> tMessage {mHandler->obtainMessage(oc::msg::MSG_CDS_TRIGGER_AFTER_WAITING)};
                    mParrent->doOemCallHandler(CommonFunctions::convertUint32ToInt32(oc::msg::MSG_CDS_TRIGGER_AFTER_WAITING), tMessage);
                }
            }
            if(!isACNUSAvailable) {
                /* 
                 * If MCU does not receive ACK for collision signanl: retransmit every 1 second
                 * If MCU receives ACK for collision signanl: stop retransmission
                 */
                sendACKToMCU(message->arg1, message->arg2);
            }
        }
        else if( (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CDS_SLIGHT_CRASH)) || (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CDS_SLIGHT_CRASH_ACN_WITH_GTC)) ) {
            const bool isACNWithPhoneAvailable =
                                      VehicleVariationAdapter::getInstance()->isFeatureAvailableInRegion(feature_name::acn_phone);

            if (isACNWithPhoneAvailable) {
                LOG_I(DLT_STRING("ACN with phone slight"));
                if (mHandler != nullptr) {
                    const sp<sl::Message> tMessage {mHandler->obtainMessage(oc::msg::MSG_COMM_WPHONE_CDS_LIGHT_TRIGGER)};
                    mParrent->doOemCallHandler(CommonFunctions::convertUint32ToInt32(oc::msg::MSG_COMM_WPHONE_CDS_LIGHT_TRIGGER), tMessage);
                }
            }
            /*
             * ACK for Slight CDS should be sent even in regions where the ACN with Phone feature is not supported 
             *
             * If MCU does not receive ACK for collision signanl: retransmit every 1 second
             * If MCU receives ACK for collision signanl: stop retransmission
             */
            sendACKToMCU(message->arg1, message->arg2);
        }
    }
    else {
        if ( (what == ENUM_TO_INT(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER)) || (what == ENUM_TO_INT(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER_ACN_WITH_GTC)) ) {
            const bool isACNWithPhoneAvailable =
                                      VehicleVariationAdapter::getInstance()->isFeatureAvailableInRegion(feature_name::acn_phone);
            if (isACNWithPhoneAvailable) {
                if (mHandler != nullptr) {
                    LOG_I(DLT_STRING("block ACN with phone heavy 500ms"));
                    (void)mHandler->sendMessageDelayed(mHandler->obtainMessage(
                                                                                ENUM_TO_INT(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER_ACN_WITH_GTC)),
                                                                                timeMsParams::TIME_SEND_RETRY_DELAY_MS
                                                                              );
                }
            }
        }
        else if( (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH)) || (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH_ACN_WITH_GTC)) ) {
            const bool isACNWithPhoneAvailable =
                                      VehicleVariationAdapter::getInstance()->isFeatureAvailableInRegion(feature_name::acn_phone);
            if (isACNWithPhoneAvailable) {
                if (mHandler != nullptr) {
                    LOG_I(DLT_STRING("block ACN with phone (cds)heavy 500ms"));
                    (void)mHandler->sendMessageDelayed(mHandler->obtainMessage(
                                                                                ENUM_TO_INT(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH_ACN_WITH_GTC)),
                                                                                timeMsParams::TIME_SEND_RETRY_DELAY_MS
                                                                              );
                }
            }
        }
        else if( (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CDS_SLIGHT_CRASH)) || (what == ENUM_TO_INT(msg::MSG::RECV_MSG_CDS_SLIGHT_CRASH_ACN_WITH_GTC)) ) {
            const bool isACNWithPhoneAvailable =
                                      VehicleVariationAdapter::getInstance()->isFeatureAvailableInRegion(feature_name::acn_phone);
            if (isACNWithPhoneAvailable) {
                if (mHandler != nullptr) {
                    LOG_I(DLT_STRING("block ACN with phone slight 500ms"));
                    (void)mHandler->sendMessageDelayed(mHandler->obtainMessage(
                                                                                ENUM_TO_INT(msg::MSG::RECV_MSG_CDS_SLIGHT_CRASH_ACN_WITH_GTC)),
                                                                                timeMsParams::TIME_SEND_RETRY_DELAY_MS
                                                                              );
                }
            }
        }
    }
}


/**
 * For testing purpose, this method is used to simulate the SLDD button press.
 */
void ECallApp2::slddTest(uint32_t num, uint32_t arg1) {
    if (mHandler == nullptr) {
        LOG_E(DLT_STRING("mHandler is null, do not thing"));
    }
    else if(num == SLDD_ECALL_SHORT_PRESS_CODE) {// ecall short press
        LOG_I(DLT_STRING("SLDD short press"));
        const sp<sl::Message> message {mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_HMI_BUTTON_SHORT_PRESS))};
        (void)(message->sendToTarget());
    }
    else if (num == SLDD_ECALL_LONG_PRESS_CODE) {// ecall long press
        LOG_I(DLT_STRING("SLDD long press"));
        const sp<sl::Message> message {mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_HMI_BUTTON_LONG_PRESS))};
        (void)(message->sendToTarget());
    }
    else if (num == SLDD_DESS_MES_STATUS_NOTIFICATION_0_CODE) {// dess ecall MES_STATUS_NOTIFICATION 0
        LOG_I(DLT_STRING("MES_STATUS_NOTIFICATION 0"));
        mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::MES_STATUS_NOTIFICATION), 0));
    }
    else if (num == SLDD_DESS_MES_STATUS_NOTIFICATION_1_CODE) { // dess ecall MES_STATUS_NOTIFICATION 1
        LOG_I(DLT_STRING("MES_STATUS_NOTIFICATION 1"));
        mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::MES_STATUS_NOTIFICATION), 1));
    }
    else if (num == SLDD_DESS_MES_STATUS_NOTIFICATION_2_CODE) {// dess ecall MES_STATUS_NOTIFICATION 2
        LOG_I(DLT_STRING("MES_STATUS_NOTIFICATION 2"));
        mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::MES_STATUS_NOTIFICATION), 2));
    }
    else if (num == SLDD_RESET_ECALL_CODE) {// reset eCall
        LOG_I(DLT_STRING("Reset eCall!!!"));
        mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::MESS_RESET_ECALL_SLDD)));
    }
    else if (num == SLDD_RECOVERING_ECALL_CODE) {
        LOG_I(DLT_STRING("test recovering ecall: "), DLT_UINT(arg1));
        mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::MESS_TEST_RECOVER_ECALL_SLDD), arg1));
    }
    else {
        LOG_I(DLT_STRING("default case"));
    }
}

void ECallApp2::doECallhandlerInternal(const int32_t& what, const android::sp<sl::Message>& message) {
    LOG_I(DLT_STRING("doECallhandlerInternal"));
    if (what == ENUM_TO_INT(msg::MSG::APPM_REQUEST_LAUNCH)) {
        if (mProcessorsMgr != nullptr) {
            android::sp<AppMgrEventData> pAppMgrEventData {nullptr};
            message->getObject(pAppMgrEventData);
            if (pAppMgrEventData != nullptr) {
                LOG_I(DLT_STRING("requestLaunch, featurename = "), DLT_STRING(pAppMgrEventData->mFeatureName.c_str()));
                mProcessorsMgr->requestLaunch(pAppMgrEventData->mFeatureName);
            }

        }
        else { LOG_E(DLT_STRING("mProcessorsMgr is null"));}
    }
    else if (what == ENUM_TO_INT(msg::MSG::APPM_REQUEST_IGNORE)) {
        if (mProcessorsMgr != nullptr) {
            android::sp<AppMgrEventData> pAppMgrEventData {nullptr};
            message->getObject(pAppMgrEventData);
            if (pAppMgrEventData != nullptr) {
                LOG_I(DLT_STRING("requestIgnore, featurename = "), DLT_STRING(pAppMgrEventData->mFeatureName.c_str()));
                mProcessorsMgr->requestIgnore(pAppMgrEventData->mFeatureName);
            }
        }
        else { LOG_E(DLT_STRING("mProcessorsMgr is null"));}
    }
    else if (what == ENUM_TO_INT(msg::MSG::APPM_REQUEST_TERMINATE)) {
        if (mProcessorsMgr != nullptr) {
            android::sp<AppMgrEventData> pAppMgrEventData {nullptr};
            message->getObject(pAppMgrEventData);
            if (pAppMgrEventData != nullptr) {
                LOG_I(DLT_STRING("requestTerminate, featurename = "), DLT_STRING(pAppMgrEventData->mFeatureName.c_str()));
                mProcessorsMgr->requestTerminate(pAppMgrEventData->mFeatureName);
            }

        }
        else { LOG_E(DLT_STRING("mProcessorsMgr is null"));}
    }
    else {}
    if (mProcessorsMgr != nullptr)  {
        mProcessorsMgr->handleMessage(static_cast<uint32_t>(what), message);
    }
    else {
        LOG_E(DLT_STRING("mProcessorsMgr is null"));
    }
}

void ECallApp2::onPostReceived(const sp<Post>& post) {
    LOG_I(DLT_STRING("onPostReceived, post.what = "), DLT_UINT(post->what));
    if (post->what > 5000) {
        if (mIsBootCompleted && mHandler!=nullptr) {
            mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::APPM_SLDD_SEND_POST), post->what, post->arg1, post->arg2)->sendToTarget();
        }
    }
}

} /* End: namespace ECALLDCMNONREG */
