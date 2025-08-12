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
 * @file        r_call_jp_processor.h
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#ifndef __OEM_CALL_RCALL_PROCESSOR_BASE_H__
#define __OEM_CALL_RCALL_PROCESSOR_BASE_H__
#include "processor_base.h"

namespace oc {

/**
 * @brief
 * TODO: document
 *
 */
class RCallProcessor: public ProcessorBase {

public:
    RCallProcessor();
    ~RCallProcessor();

    /**
     * @brief This is the method to handle input event
     *
     */
    virtual void doProcessorHandler(uint32_t what,const sp<sl::Message>& msg) override;

    /**
     * @brief This is the method which will be call by Timer manager when timer event occur
     *
     * @param timerId
     */
    virtual void onTimerEvent(uint32_t timerId) override;

    /**
     * @brief This this the method to create runtime connection between Processor
     *        instance and other instance of components.
     *
     */
    virtual void init(
        android::sp<OemCallHandler> spHandler,
        std::shared_ptr< AdapterLocator > adapters
    )  override;

    virtual bool isAvailable() override;
    // virtual void runTest(uint32_t testId) override;
private:
    /**
     * @brief This is the type of structure that contains the private
     *        properties of the instance.  It is defined in the implementation
     *        and declared here to ensure that it is scoped inside the class.
     */
    struct Impl;

    /**
     * @brief This contain private data of the instance
     *
     */
    static std::unique_ptr<Impl> impl_;
};

} // namespace OEMCall App
#endif
