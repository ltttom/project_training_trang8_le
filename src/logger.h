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
 * @file        logger.h
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#ifndef __OEM_CALL_LOGGER_H__
#define __OEM_CALL_LOGGER_H__
#include <dlt/dlt.h>
#include <cxxabi.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <Log.h>

DLT_IMPORT_CONTEXT(ctxOEMCall);
DLT_IMPORT_CONTEXT(ctxOEMCallMASS);
//#define __UT_BUILD__
#define __OEM_CONFIG__
#define __TEST_REGION__
//#define __POWER_TEST__
//#define __REMOTE_IF__

#define OEM_CALL_APP_NAME "OEMC"
#define OEM_CALL_CTX_NAME "OECT"
#define OEM_CALL_CTX_MASS_NAME "MASS"
#define OEM_CALL_DLT_SERVICE_ID 5000
#if __linux__
#define OEM_CALL_THREAD_ID DLT_CSTRING("["), DLT_INT32(syscall(SYS_gettid)), DLT_CSTRING("]")
#elif __WIN32__
#define OEM_CALL_THREAD_ID DLT_CSTRING("["), DLT_CSTRING("]")
#endif

#define OEM_CALL_FILE_NAME DLT_CSTRING("["), DLT_CSTRING(__FILENAME__), DLT_CSTRING(":"), DLT_INT(__LINE__), DLT_CSTRING(":"), DLT_CSTRING("]")
#define OEM_CALL_FUNC_NAME DLT_CSTRING("["), DLT_CSTRING(__func__), DLT_CSTRING("]")


#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define OEM_CALL_LOG_HEADER OEM_CALL_THREAD_ID, OEM_CALL_FILE_NAME, OEM_CALL_FUNC_NAME

#ifdef __UNITTEST__
    #define LOG_E(...)
    #define LOG_W(...)
    #define LOG_I(...)
    #define LOG_D(...)
    #define LOG_V(...)
    #define LOG_M(...)
    #define try      if (true)
    #define catch(X) if (false)
#else
    #define LOG_E(...) DLT_LOG(ctxOEMCall, DLT_LOG_ERROR, OEM_CALL_LOG_HEADER, __VA_ARGS__)
    #define LOG_W(...) DLT_LOG(ctxOEMCall, DLT_LOG_WARN, OEM_CALL_LOG_HEADER, __VA_ARGS__)
    #define LOG_I(...) DLT_LOG(ctxOEMCall, DLT_LOG_INFO, OEM_CALL_LOG_HEADER, __VA_ARGS__)
    #define LOG_D(...) DLT_LOG(ctxOEMCall, DLT_LOG_DEBUG, OEM_CALL_LOG_HEADER, __VA_ARGS__)
    #define LOG_V(...) DLT_LOG(ctxOEMCall, DLT_LOG_VERBOSE, OEM_CALL_LOG_HEADER, __VA_ARGS__)
    #define LOG_M(...) DLT_LOG(ctxOEMCallMASS, DLT_LOG_INFO, OEM_CALL_LOG_HEADER, __VA_ARGS__)
#endif
/**
 * @brief initDLTLog init DLT app and context
 * This function should be called as soon as application started up
 */
void initDLTLog();
#endif // __OEM_CALL_LOGGER_H__
