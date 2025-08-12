/**
 * @copyright
 * Copyright (c) 2025 by LG Electronics Inc.
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
 * @file        ECallApp2.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/

#ifndef ECALLAPP2_H
#define ECALLAPP2_H

#include <memory>
#include <deque>
#include <Error.h>
#include <binder/IServiceManager.h>
#include <utils/Handler.h>

#include <application/Application.h>
#include <services/ApplicationManagerService/ISystemPostReceiver.h>
#include <services/ApplicationManagerService/IApplicationManagerService.h>

#include "utils/ECallAppHandler.h"
#include "utils/IECallAppIf.h"
#include "oem_call_app_if.h"
#include "oem_call_app_constants.h"
// #include "utils/ECallEmulator.h"
#include "processors/ProcessorsManager.h"

namespace ecallapp {
class ECallApp2 : public Application, public IECallAppIf {
public:
    ECallApp2();
    ~ECallApp2() override;

public:
    void onCreate() override;
    void onDestroy() override;
    void onActive(const int32_t param, const std::string param2) noexcept override;
    void onInactive(const int32_t param, const std::string param2) noexcept override;
    error_t onFeatureStatusChanged(const std::string feature, const FeatureStatus status) override;
    error_t onFeatureActionPerformed(const FeatureAction action, const std::string feature) override;
    error_t onFeatureActionDelivered(const std::string feature, const FeatureAction action, const std::string causedBy) override;

    void doECallhandler(const int32_t& what, const android::sp<sl::Message>& message);// override;
    void doECallhandlerInternal(const int32_t& what, const android::sp<sl::Message>& message);
    void doBootCompleted(const android::sp<sl::Message>& message);
    void initHandler();
    void slddTest(uint32_t num, uint32_t arg1);
    void setParrent(oc::OEMCallAppIF * parrent);
    virtual void onPostReceived(const sp<Post>& post) override;

private:

    android::sp<sl::SLLooper>           mLooper;
    std::shared_ptr<ECallAppHandler> mHandler;

    // mProcessorsMgr is an object of ProcessorsManager class, which would be used
    // to manage all the processors to handle the functionalities: TSP, DESS and CPD.
    std::shared_ptr<ProcessorsManager>  mProcessorsMgr;
    // std::shared_ptr<ECallEmulator>      mECallEmulator;
    bool                                mIsBootCompleted;
    oc::OEMCallAppIF * mParrent;

    void sendACKToMCU(const int32_t arg1, const int32_t arg2);
};

} /* End: namespace ecallapp */
#endif // ECALLAPP_H
