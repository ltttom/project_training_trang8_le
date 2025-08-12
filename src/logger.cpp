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
 * @file        logger.cpp
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#include "logger.h"
#include <application/Application.h>

DLT_DECLARE_CONTEXT(ctxOEMCall);
DLT_DECLARE_CONTEXT(ctxOEMCallMASS);

extern android::sp<Application> gApp;
#ifdef __UNITTEST__
#undef LOG_I
#define LOG_I(...)
#else
extern android::sp<Application> gApp;
#endif
int32_t dltInjectionCallback(uint32_t service_id, void *data, uint32_t length)
{
    (void) length;
    if(service_id == OEM_CALL_DLT_SERVICE_ID && length >= 1) {
        uint8_t* dataPtr = (uint8_t*) data;
        int32_t what = dataPtr[0];
        LOG_I(DLT_STRING("Perform test cmd: "), DLT_INT32(what));
#ifdef __UNITTEST__
#else
        gApp->onHMIReceived(what, 0);
#endif
    } else {
        LOG_I(DLT_STRING("Invalid service id or test data"));
    }
    return 0;
}

void initDLTLog()
{
    DLT_REGISTER_APP(OEM_CALL_APP_NAME ,"DLT log for OEM Call application");
    DLT_REGISTER_CONTEXT(ctxOEMCall, OEM_CALL_CTX_NAME,"DLT LOG context for OEM Call");
    DLT_REGISTER_CONTEXT(ctxOEMCallMASS, OEM_CALL_CTX_MASS_NAME,"DLT LOG MASS context for OEM Call");
    DLT_REGISTER_INJECTION_CALLBACK(ctxOEMCall, OEM_CALL_DLT_SERVICE_ID, dltInjectionCallback);
}
