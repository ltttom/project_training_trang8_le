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
 * @file        hmi_manager.h
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#ifndef __OEM_CALL_LED_MANAGER_H__
#define __OEM_CALL_LED_MANAGER_H__
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../oem_call_handler.h"

namespace oc {

/**
 * @brief
 * This is the class which implement the method to manage led.
 *
 */
class HmiManagerAdapter {
public:
    enum class Pattern {
        US_ECALL_CONNECTING,
        US_ECALL_CONNECTED,
        US_ECALL_ERROR,
        JP_RCALL_ERROR,
        JP_RCALL_RUNNING,
        LED_IDLE,
        LED_OFF_NO_SUBS,
        LED_OFF_OTHER,
        US_ACN_CONNECTING
    };

    HmiManagerAdapter(android::sp<OemCallHandler> handler);

    void doHandleHMIMessages(const android::sp<sl::Message>& message, bool savingFlash = false);
    void doHandleRetrySaveTrigger(const android::sp<sl::Message>& message);
    void doHandleSaveTrigger(bool savingFlash = true);

    /**
     * @brief requestPattern This is the method to request play/stop led pattern
     * @param pattern Pattern name
     * @param isOn request ON or OFF
     */
    void requestPattern(const Pattern pattern, bool isOn);
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
    std::shared_ptr<Impl> impl_;
};

} // namespace OEMCall App
#endif
