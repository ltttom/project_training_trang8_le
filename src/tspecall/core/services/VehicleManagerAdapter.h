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
 * @file        VehicleManagerAdapter.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef ECALLNONREG_VEHICLEMANAGERADAPTER_H
#define ECALLNONREG_VEHICLEMANAGERADAPTER_H

#include <json/json.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <utils/Message.h>
#include <services/VehicleManagerService/IVehicleManagerService.h>
#include <services/VehicleManagerService/IVehicleManagerServiceType.h>
#include <services/VehicleManagerService/IVehicleReceiver.h>
#include <binder/IServiceManager.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>

#include "../utils/ECallAppHandler.h"
#include "../utils/ECallTimerHandler.h"
#include "logger.h"
#include "../utils/ServiceDeathRecipient.h"
#include "../utils/ParamsDef.h"

namespace ecallapp {

class VehicleManagerAdapter {
public:
    VehicleManagerAdapter();
    ~VehicleManagerAdapter();

    VehicleManagerAdapter(const VehicleManagerAdapter& ) = delete;
    VehicleManagerAdapter& operator=(const VehicleManagerAdapter& ) = delete;

    VehicleManagerAdapter(VehicleManagerAdapter&& ) = delete;
    VehicleManagerAdapter& operator=(VehicleManagerAdapter&& ) = delete;

    static VehicleManagerAdapter* getInstance();
    void registerService();
private:
    static std::shared_ptr<VehicleManagerAdapter> instance;
    android::sp<ECallAppHandler> mHandler;
    android::sp<ServiceDeathRecipient> mServiceDeathRecipient = nullptr;
    android::sp<IVehicleReceiver> mVehicleReceiver = nullptr;
    android::sp<IVehicleManagerService> mVCMService = nullptr;

    ECallTimerHandler* mTimerHandlerSRS{ nullptr };
    android::sp<Timer> mTimerSRSFront{nullptr};
    android::sp<Timer> mTimerSRSSide{nullptr};
    android::sp<Timer> mTimerSRSRear{nullptr};
    android::sp<Timer> mTimerSRSRollover{nullptr};

    bool mIsTimerSRSFrontRunning{false};
    bool mIsTimerSRSSideRunning{false};
    bool mIsTimerSRSRearRunning{false};
    bool mIsTimerSRSRolloverRunning{false};

    Json::Value mCrashTelemetry;
    Json::Value mEventDetail;
    std::map<SigInd_Rx, std::string> mCanData;

private:
    static uint8_t getFirstByte(const android::sp<VehicleData>& vcmData, const uint32_t frameId);
    static int32_t getInt(const android::sp<IVehicleManagerService>& spVCM, const SigInd_Rx id);
    std::string getString(const SigInd_Rx id, std::string defaultValue = "unknown");
    std::string getVehicleData(const SigInd_Rx id,const std::string& defaultStringValue);

public:
    void handleSRSSignal(const uint32_t sigID);
    void sendVehicleData(const uint32_t channel, android::sp<VehicleData> data);
    uint8_t getCanData(const SigInd_Rx id);
    void onReceivedCanSignal(const uint32_t channel, const android::sp<VehicleData>& vehicleData);
    void onBinderDied(const android::wp<android::IBinder>& who);
    void onTimerTimeoutEvent(const int32_t& msgId);

    void send_BCAN_CANCEL_DISP(const uint32_t value);
    void sendBCAN_CANCEL_DISP(const uint32_t value);
    void sendACKToRVU(const uint32_t value);
    void notifySOSPressedToRVU(const uint32_t value);
    void collectCanData();
    void collectIntialData();
    void clearCanData();
    void setInitialStringData(const SigInd_Rx id, std::string& val);
    void restoreAtSignalDetectingTimePartOfMSD();
    Json::Value getCrashTelemetry();
    Json::Value getEventDetails();
    std::string getOccupantsCount();
};

class VCMReceiver : public BnVehicleReceiver {
public:
    explicit VCMReceiver(VehicleManagerAdapter& vcm) noexcept : vcmRC(vcm) {}
    ~VCMReceiver() override = default;

    VCMReceiver(const VCMReceiver& ) = default;
    VCMReceiver& operator=(const VCMReceiver& ) = default;

    VCMReceiver(VCMReceiver&& ) = default;
    VCMReceiver& operator=(VCMReceiver&& ) = default;

    void onReceived(const uint32_t channel, const sp<VehicleData>& vehicleData) override;
    void onTimerTimeoutEvent(const int32_t& msgId);

private:
    VehicleManagerAdapter& vcmRC;
};

} /* End: namespace ECALLDCMNONREG */
#endif // ECALLNONREG_VEHICLEMANAGERADAPTER_H
