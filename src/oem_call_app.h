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
 * @file        oem_call_app.h
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/

#ifndef __OEM_CALL_APPLICATION_H__
#define __OEM_CALL_APPLICATION_H__

#include <Error.h>
#include <binder/IServiceManager.h>
#include <utils/Handler.h>

#include <application/Application.h>
#include <services/ApplicationManagerService/ISystemPostReceiver.h>
#include <services/ApplicationManagerService/IApplicationManagerService.h>
#include <memory>
#include <deque>

#include "services_adapter/config_manager_adapter.h"
#include "services_adapter/hmi_manager_adapter.h"
#include "services_adapter/audio_manager_adapter.h"
#include "services_adapter/telephone_manager_adapter.h"
#include "services_adapter/app_manager_adapter.h"
#include "services_adapter/power_manager_adapter.h"
#include "services_adapter/rim_adapter.h"
#include "processor/acn_phone_processor.h"
#include "services_adapter/receiver_manager.h"
#include "oem_call_app_if.h"
#include "oem_call_handler.h"
#include "processor/processor_manager.h"
#include <map>
#include "services_adapter/someip_manager.h"
#include "tspecall/core/ECallApp2.h"

namespace oc {
class OEMCallApp : public Application, public OEMCallAppIF {
public:

    OEMCallApp();
    virtual ~OEMCallApp();

    /**
    * Application has two lifecycle method, onCreate() and onDestroy()
    */
    virtual void onCreate() override;
    virtual void onDestroy() override;
    void onActive(const int32_t param, std::string param2) override
    {
    }

    void onInactive(const int32_t param, std::string param2) override
    {
    }

    virtual error_t onFeatureStatusChanged(const std::string feature, const FeatureStatus status) override;
    virtual error_t onFeatureActionPerformed(const FeatureAction action, const std::string feature) override;
    virtual error_t onFeatureActionDelivered(const std::string feature, const FeatureAction action, const std::string causedBy) override;

    /**
    * Callback method
    */
    virtual void onPostReceived(const sp<Post>& post) override;//this function handle send_post command (sldd)
    void onAppSystemPostReceived(const sp<Post>& post);//this function handle system post event
    void onHMIReceived(const uint32_t type, const uint32_t action) override
    {
    }

    /**
     * @brief This is the method to actually do handle event from the handle
     *
     */
    void doOemCallHandler(uint32_t what, const android::sp<sl::Message>& msg) override;
    bool isProcessorNullPtr () override;
    void doHandleRetryRegisterReceiver(uint32_t who);

private:
    android::sp<sl::SLLooper> spLooper_;                   // looper for handler
    /**
     * @brief This is our only handler
     *
     */
    android::sp<OemCallHandler> spHandler_;

    bool isOEMCallCreated_;
    /**
     * @brief This is the pointer to spSystemReceiver_
     *
     */
    sp<ISystemPostReceiver> spSystemReceiver_;

    /**
     * @brief All manager we use
     *
     */
    sp<IApplicationManagerService> spAppManager_;

    std::shared_ptr<ReceiverManager> spReceiverManager_;
    std::shared_ptr<AdapterLocator> spAdapters_;
    std::unique_ptr<ProcessorManager> upProcessorManager_;
    ecallapp::ECallApp2 mECallApp2;

    void doBootCompleted();
//    void updateFeatureObject();
    void runTest(const uint32_t testId);

};

} // namespace oc

#endif // _TEMPLATE_APPLICATION_H_
