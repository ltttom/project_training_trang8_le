/**
 * @copyright
 * Copyright (c) 2020 by LG Electronics Inc.
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
 * @file        oem_call_app.cpp
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#include <stdio.h>
#include "oem_call_app.h"
#include "logger.h"
#include "oem_call_app_constants.h"
// For types
// #include <services/TelephonyManagerService/base/CallManager.h>

namespace {
    using namespace oc;
    /**
     * SystemPostReceiver to receive system post event
     *
     */
    class SystemPostReceiver : public BnSystemPostReceiver {
    public:
        SystemPostReceiver(OEMCallApp &app) : app(app) {}
        virtual bool onSystemPostReceived(const sp<Post>& systemPost){
            app.onAppSystemPostReceived(systemPost);
            return true;
        };
    private:
        OEMCallApp &app;
    };
}

namespace oc {
using namespace self;
using namespace msg;

    /******************************************************************************/
    /**                               START PRIVATE METHOD**/
    /******************************************************************************/
    /**
     * @brief This is the method to initiallize all the component of OEMCallApp
     *
     */
    void OEMCallApp::doBootCompleted() {
        LOG_I(DLT_STRING("onBootComplete"));
#ifdef CAN_TYPE_GEN3
        LOG_I(DLT_STRING("CAN_DB_TYPE : GEN3"));
#elif CAN_TYPE_GEN2P
        LOG_I(DLT_STRING("CAN_DB_TYPE : GEN2P"));
#elif CAN_TYPE_GEN2
        LOG_I(DLT_STRING("CAN_DB_TYPE : GEN2"));
#elif CAN_TYPE_GEN1
        LOG_I(DLT_STRING("CAN_DB_TYPE : GEN1"));
#else
        LOG_I(DLT_STRING("CAN_DB_TYPE : DEFAULT"));
#endif
        if (!isOEMCallCreated_) {
            isOEMCallCreated_ = true;
            LOG_I(DLT_STRING("Creating Adapter location"));
            spReceiverManager_     = std::make_shared<ReceiverManager> ();
            spReceiverManager_->init(spHandler_);

            spAdapters_ = std::make_shared<AdapterLocator>(spHandler_);
            spAdapters_->spConfigManager->update(ConfigManager::UpdateType::APP_MODE);

            upProcessorManager_ = std::make_unique<ProcessorManager> ();
            upProcessorManager_->init(spHandler_, spAdapters_);
        }
        else {
            LOG_I(DLT_STRING("OEMCall already created"));
        }
    };

    void OEMCallApp::doHandleRetryRegisterReceiver(uint32_t who) {
        if(spReceiverManager_ != nullptr) {
            spReceiverManager_->requestRetryRegisterReceiver(who);
        }
    }

    void OEMCallApp::runTest(const uint32_t testId) {
        switch (testId) {
        case self::SLDDTestId::RCALL_TRIGGER: {
            spHandler_->obtainMessage(msg::RCALL_TRIGGER)->sendToTarget();
            break;
        }
        case self::SLDDTestId::ACN_LIGHT_TRIGGER: {
            spHandler_->obtainMessage(msg::MSG_COMM_WPHONE_CDS_LIGHT_TRIGGER)->sendToTarget();
            break;
        }
        case self::SLDDTestId::ACN_WITH_PHONE_TRIGGER: {
            spHandler_->obtainMessage(msg::MSG_WPHONE_CDS_TRIGGER_NORMAL)->sendToTarget();
            break;
        }
        case self::SLDDTestId::SOMEIP_RCALL_TRIGER: {
            uint8_t data = 0x00;
            sp<Buffer> payload = new Buffer();
            payload->append(&data, 1);
            spHandler_->obtainMessage(msg::MSG_SOMEIP_REQUEST_CLEARUP_R_CALL, payload)->sendToTarget();
            break;
        }

        default:
            break;
        }

    }

    /******************************************************************************/
    /**                               END PRIVATE METHOD**/
    /******************************************************************************/


    /******************************************************************************/
    /**                               PUBLIC                                     **/
    /******************************************************************************/
    OEMCallApp::OEMCallApp() :
        isOEMCallCreated_(false)
    {
        initDLTLog();
        mECallApp2.setParrent(this);
    }

    OEMCallApp::~OEMCallApp() {
    }

    void OEMCallApp::onCreate() {
        LOG_I(DLT_STRING("onCreate(). 2025.Feb.21"));
#ifdef REGION_PCB26
        LOG_I(DLT_STRING("Build for TSU26"));
#endif
        mECallApp2.onCreate();
        spAppManager_ = interface_cast<IApplicationManagerService> (
            defaultServiceManager()->getService(
                String16("service_layer.ApplicationManagerService")
            )
        );

        if(spHandler_ == nullptr) {
            spLooper_ = sl::SLLooper::myLooper();
            spHandler_ = new OemCallHandler(spLooper_, *this);
        }

        if(spAppManager_ != nullptr) {
            if(spAppManager_->getBootCompleted()) {
                LOG_I(DLT_STRING("System boot completed, initialize application now"));
                if (spHandler_ != nullptr) spHandler_->obtainMessage(msg::APPM_BOOT_COMPLETED)->sendToTarget();
            } else {
                LOG_I(DLT_STRING("System has not yet boot completed, waiting for boot complete event"));
            }
            spSystemReceiver_ = new SystemPostReceiver(*this);
            spAppManager_->registerSystemPostReceiver(
                spSystemReceiver_,
                SYS_POST_ALL
            );
        }
    }

    void OEMCallApp::onDestroy() {
        LOG_I(DLT_STRING("onDestroy"));
        mECallApp2.onDestroy();
        // TODO: handle application destroy event
        if (spReceiverManager_) spReceiverManager_.reset();
        if (spAdapters_)spAdapters_.reset();
        if (upProcessorManager_)upProcessorManager_.reset();
        isOEMCallCreated_ = false;
    }

    void OEMCallApp::onPostReceived(const sp<Post>& post) {
        if(post != nullptr) {
            uint32_t what = post->what;
            if (isOEMCallCreated_) {
                if (what > SLDD_TEST_MSG_ECALL_THRESHOLD) { // oemcallcapp define sldd am send_post oemcall <<arg>> with arg > 5000
                    mECallApp2.onPostReceived(post); // handle sldd send_post command
                    runTest(what); // handle sldd send_post command
                }
            }
        }
        else {
            LOG_I(DLT_STRING("Can't handle this post"));
        }
    }

    void OEMCallApp::onAppSystemPostReceived(const sp<Post>& post) {
        if(post != nullptr) {
            uint32_t what = post->what;
            if (what == SYS_POST_BOOT_COMPLETED && !isOEMCallCreated_) {
                spHandler_->obtainMessage(msg::APPM_BOOT_COMPLETED)->sendToTarget();
            }
            else if (isOEMCallCreated_) {
                if (what == SYS_POST_FEATURE_STATUS_CHANGED) {
                    spHandler_->obtainMessage(msg::APPM_FEATURE_STATUS_CHANGED, post)->sendToTarget();
                }
                else if (what == SYS_POST_FEATURE_ACTION_DELIVERED) {
                    spHandler_->obtainMessage(msg::APPM_REQUEST_ACTIONS, post)->sendToTarget();
                }
            }
        }
        else {
            LOG_I(DLT_STRING("Can't handle this post"));
        }
    }

    error_t OEMCallApp::onFeatureStatusChanged(const std::string feature, const FeatureStatus status) {
        LOG_I(DLT_STRING(feature.c_str()), DLT_UINT(status));
        mECallApp2.onFeatureStatusChanged(feature, status);
        return TIGER_ERR::E_OK;
    }

    error_t OEMCallApp::onFeatureActionPerformed(const FeatureAction action, const std::string feature) {
        LOG_I(DLT_STRING(feature.c_str()), DLT_UINT(action));
        mECallApp2.onFeatureActionPerformed(action, feature);
        // TODO: check feature name defined in Json file
        auto msg = spHandler_->obtainMessage(msg::APPM_RESPONSE_QUERY, action);
        msg->buffer.setTo(const_cast<char*>(feature.c_str()), feature.size());
        msg->sendToTarget();
        return TIGER_ERR::E_OK;
    }

    error_t OEMCallApp::onFeatureActionDelivered(const std::string feature, const FeatureAction action, const std::string causedBy) {
        LOG_I(DLT_STRING(feature.c_str()), DLT_UINT(action), DLT_STRING(causedBy.c_str()));
        mECallApp2.onFeatureActionDelivered(feature, action, causedBy);
        return TIGER_ERR::E_OK;
    }

    /**
     * @brief This is the function will be call to handle all message which was
     *        pushed in the message queue.
     *
     * @param msg
     */
    void OEMCallApp::doOemCallHandler(uint32_t what, const sp<sl::Message>& msg) {
    	bool needHandleMessage = msg::name.find(msg->what) != msg::name.end();

    	if (needHandleMessage) {
    		LOG_I( DLT_STRING("Received message"), DLT_STRING(msg::name[msg->what].c_str()));
    	}
    	else {
    		return;
    	}

        if (what == msg::APPM_BOOT_COMPLETED) {
            // TODO: check application initiallized.
            doBootCompleted();
        }
        // HMI
        else if (what == msg::MSG_RECV_HMI) {
            bool shouldSavedECallFlag = spAdapters_->spPowerManagerAdapter->getACCStatus() == MCU_VALUE_STATUS::MCU_STATUS_ON &&
                    spAdapters_->spPowerManagerAdapter->getIGStatus() == MCU_VALUE_STATUS::MCU_STATUS_ON;
            spAdapters_->spHmiManager->doHandleHMIMessages(msg, shouldSavedECallFlag);
        }
        else if (what == msg::MSG_HMI_RETRY_SAVE_TRIGGER) {
            spAdapters_->spHmiManager->doHandleRetrySaveTrigger(msg);
        }
        else if (what == msg::MSG_CM_REQUEST_RETRY_REGISTER_RECEIVER) {
            doHandleRetryRegisterReceiver(msg->arg1);
        }
        else if (what == msg::MSG_CM_RETRY_REGISTER_AUDIO) {
            spAdapters_->spAudioManager->requestBRetryRegister();

        }

    /*AppMgr signals*/
        else if (what == msg::APPM_RESPONSE_QUERY) {
            spAdapters_->spAppManagerAdapter->doHandleAppMgrResponse(msg);
        }
        else if (what == msg::APPM_REQUEST_ACTIONS) {
            spAdapters_->spAppManagerAdapter->doHandlePostActionDelivered(msg);
        }
        else if (what == msg::APPM_FEATURE_STATUS_CHANGED) {
            spAdapters_->spAppManagerAdapter->doHandlePostFeatureStatusChanged(msg);
        }

    /*Config Signal*/
        else if (what == msg::MSG_CONFIG_CHANGED) {
            android::sp<ConfigNotiData> data = nullptr;
            msg->getObject(data);
            spAdapters_->spConfigManager->doHandleConfigChanged(data); // update config
            if (upProcessorManager_ != nullptr) {
                upProcessorManager_->handleMessage(what, msg);
            }
        }

    /*Power*/
        else if (what == msg::MSG_POWER_STATE_CHANGED) {
            const uint32_t index = msg->arg1;
            const uint32_t value = msg->arg2;
            spAdapters_->spPowerManagerAdapter->handlePowerStateChanged(index, value);
            if (upProcessorManager_ != nullptr) {
                upProcessorManager_->handleMessage(what, msg);
            }
        }

     /*Some IP*/
        else if (what == msg::MSG_CM_RETRY_REGISTER_SOMEIP) {
            spAdapters_->spSomeIPManger->registerSomeipProviderReceiver();
        }

    /*Feature control signals*/
        else {
            if (upProcessorManager_ != nullptr) {
                upProcessorManager_->handleMessage(what, msg);
            }
            else {

            }
        }
    }

    bool OEMCallApp::isProcessorNullPtr() {
        return upProcessorManager_ == nullptr;
    }


    /******************************************************************************/
    /**                          END PUBLIC                                      **/
    /******************************************************************************/
} // namespace

/******************************************************************************/
/**                          LIFE TIME                                       **/
/******************************************************************************/
android::sp<Application> gApp;
#ifdef __cplusplus
extern "C" class Application* createApplication() {
    printf("create OEMCallApp");
    gApp = new oc::OEMCallApp;
    return gApp.get();
}

extern "C" void destroyApplication(class Application* application) {
    delete (oc::OEMCallApp*)application;
}

#endif
/******************************************************************************/
/**                          END LIFE TIME                                        **/
/******************************************************************************/
