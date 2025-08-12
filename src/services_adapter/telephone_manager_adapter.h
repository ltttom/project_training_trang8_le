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
 * @file        telephone_manager_adapter.h
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#ifndef __OEM_CALL_TELEPHONE_MANAGER_H__
#define __OEM_CALL_TELEPHONE_MANAGER_H__
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../services_adapter/config_manager_adapter.h"
#include <utils/Message.h>
#include "../oem_call_handler.h"

namespace oc {

/**
 * @brief
 * This is the class which implement the method to manage telephone
 *
 */
class TelephoneManager{
public:
    class StateChangedInfo: public RefBase {
    public:
       uint32_t state;
       /**
        * @brief isNormal This field is used for indicate does State Changed was
        *                 caused by normal reason or not.
        */
       bool isNormal = true;
    };

    TelephoneManager(android::sp<OemCallHandler> spHandler);
    void dial(const std::string& number, bool isEcall);
    void endCall();
    void answer();
    void stop();
    void rejectCall();
    void exitCBMmode();
    uint32_t getCallState();
    std::string requestGetImsi();

    uint32_t getVoiceServiceState();

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
