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
 * @file        oem_call_app_if.h
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#ifndef __OEM_CALL_APPLICATION_INTERFACE__
#define __OEM_CALL_APPLICATION_INTERFACE__
#include <binder/BinderService.h>
#include <utils/Handler.h>
#include <cstdlib>

namespace oc {

class OEMCallAppIF {
public:
    virtual void doOemCallHandler(uint32_t what, const android::sp<sl::Message>& msg) = 0;
    virtual bool isProcessorNullPtr() = 0;
};

}

#endif
