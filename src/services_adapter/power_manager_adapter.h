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
 * @file        power_manager_adapter.h
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#ifndef __OEM_CALL_POWER_MANAGER_ADAPTER_H__
#define __OEM_CALL_POWER_MANAGER_ADAPTER_H__
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <utils/Message.h>
#include "../oem_call_handler.h"

namespace oc {

/**
 * @brief
 * This is the class which implement the method to manage led.
 *
 */
class PowerManagerAdapter{
public:

    PowerManagerAdapter(android::sp<OemCallHandler> spHandler);
    virtual ~PowerManagerAdapter();

    /**
     * @brief This is the function to indicate the power manager services about
     *      the state of ACN processing.
     *
     */
    bool requestBUBModeACNPhone(bool isActive);
    bool requestBUBModeACN(bool isActive);
    bool requestBUBModeECall(bool isActive);
    int32_t getACCStatus();
    int32_t getIGStatus();
    void acquirePowerLock();
    void resetIsLocked();
    void releasePowerLock();

    /**
     * @brief This is the function to get Ignition value
     *
     */
    std::string getIgnition();

    bool isAccOn();
    bool isIGON();

    /**
     * @brief PowerManagerAdapter::handlePowerModeChanged This is the fucntion to handle PowerMode changed Signal from service
     */
    void handlePowerStateChanged(const uint32_t index, const uint32_t value);

    void onPowerLockRelease();

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
