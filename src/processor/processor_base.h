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
 * @file        processor_base.h
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#ifndef __OEM_CALL_PROCESSOR_BASE_H__
#define __OEM_CALL_PROCESSOR_BASE_H__
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <Error.h>
#include <utils/SLLooper.h>
#include <utils/Handler.h>
#include "../services_adapter/hmi_manager_adapter.h"
#include "../services_adapter/config_manager_adapter.h"
#include "../services_adapter/telephone_manager_adapter.h"
#include "../services_adapter/audio_manager_adapter.h"
#include "../services_adapter/app_manager_adapter.h"
#include "../services_adapter/power_manager_adapter.h"
#include "../services_adapter/rim_adapter.h"
#include "../oem_call_handler.h"
#include "../services_adapter/someip_manager.h"
#include "../tspecall/core/services/VehicleVariationAdapter.h"
namespace oc {

struct AdapterLocator
{
    std::shared_ptr<ConfigManager> spConfigManager = nullptr;
    std::shared_ptr<RIMAdapter> spDataManager = nullptr;
    std::shared_ptr<AppManagerAdapter> spAppManagerAdapter = nullptr;
    std::shared_ptr<PowerManagerAdapter> spPowerManagerAdapter = nullptr;
    std::shared_ptr<HmiManagerAdapter> spHmiManager = nullptr;
    std::shared_ptr<AudioManagerAdapter> spAudioManager = nullptr;
    std::shared_ptr<TelephoneManager> spTelephoneManager = nullptr;
    std::shared_ptr<someIP_manager> spSomeIPManger = nullptr;

    AdapterLocator(android::sp<OemCallHandler> spHandler)
    {
        spTelephoneManager    = std::make_shared<TelephoneManager> (spHandler);
        spConfigManager       = std::make_shared<ConfigManager> ();
        spDataManager         = std::make_shared<RIMAdapter> (spHandler);
        spHmiManager          = std::make_shared<HmiManagerAdapter> (spHandler);
        spAudioManager        = std::make_shared<AudioManagerAdapter> (spHandler);
        spAppManagerAdapter   = std::make_shared<AppManagerAdapter> ();
        spPowerManagerAdapter = std::make_shared<PowerManagerAdapter>(spHandler);
        spSomeIPManger        = std::make_shared<someIP_manager>();
        spSomeIPManger->initHandle(spHandler);
        spSomeIPManger->registerSomeipProviderReceiver();
    }
    ~AdapterLocator() {
    }
};


/**
 * @brief
 * TODO: document
 *
 */
class ProcessorBase : public std::enable_shared_from_this<ProcessorBase> {
public:
    /**
     * @brief This is the method to handle input event
     *
     */
    virtual void doProcessorHandler(uint32_t what,const sp<sl::Message>& msg) = 0;

    /**
     * @brief This is the function which will be call by timer when timer event occur
     *
     * @param timerId
     */
    virtual void onTimerEvent(uint32_t timerId) = 0;

    /**
     * @brief This this the method to create runtime connection between Processor
     *        instance and other instance of components.
     *
     */
    virtual void init(
        android::sp<OemCallHandler> spHandler,
        std::shared_ptr< AdapterLocator > adapters
    ) = 0;

    virtual bool isAvailable() = 0;

    // /**
    //  * @brief This is the function for testing only
    //  *
    //  * @param testName
    //  */
    // virtual void runTest(uint32_t testId) = 0;
};

} // namespace OEMCall App
#endif
