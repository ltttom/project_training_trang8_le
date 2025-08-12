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
 * @file        VehicleManagerAdapter.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include "VehicleManagerAdapter.h"
#include "ServiceAdapterTimerId.h"
#include "../utils/CommonFunctions.h"
#include <iomanip>
#include <memory>

#include "tspecall/core/services/SystemPropertyAdapter.h"
#include "tspecall/core/utils/ParamsDef.h"

namespace ecallapp {

VehicleManagerAdapter::VehicleManagerAdapter() {
    mVehicleReceiver = std::move(android::sp<IVehicleReceiver>{new VCMReceiver(*this)});
}

VehicleManagerAdapter::~VehicleManagerAdapter() {
    mVCMService = nullptr;
    mVehicleReceiver = nullptr;
    mServiceDeathRecipient = nullptr;
}

std::shared_ptr<VehicleManagerAdapter> VehicleManagerAdapter::instance {};
VehicleManagerAdapter* VehicleManagerAdapter::getInstance() {
    if (instance == nullptr) {
        instance = std::make_shared<VehicleManagerAdapter>();
    }
    return instance.get();
}

void VehicleManagerAdapter::registerService() {
    LOG_I(DLT_STRING("registerService"));
    mHandler = ECallAppHandler::getInstance();

    // Declare a callback function to be invoked when the service is dead
    mServiceDeathRecipient =
              android::sp<ServiceDeathRecipient>{
                                              new ServiceDeathRecipient( [&] ( const android::wp<android::IBinder>& who){
                                                VehicleManagerAdapter::getInstance()->onBinderDied(who);
                                              })
                                            };

    // Initialize timers
    mTimerHandlerSRS = new ECallTimerHandler();
    mTimerSRSFront = new Timer(mTimerHandlerSRS, static_cast<int32_t>(vehicle::TIMER_SRS_FRONT_ID));
    mTimerSRSFront->setDurationMs(timeMsParams::SRS_SEPARATING_DURATION_MS, 0U);
    mTimerSRSSide = new Timer(mTimerHandlerSRS, static_cast<int32_t>(vehicle::TIMER_SRS_SIDE_ID));
    mTimerSRSSide->setDurationMs(timeMsParams::SRS_SEPARATING_DURATION_MS, 0U);
    mTimerSRSRear = new Timer(mTimerHandlerSRS, static_cast<int32_t>(vehicle::TIMER_SRS_REAR_ID));
    mTimerSRSRear->setDurationMs(timeMsParams::SRS_SEPARATING_DURATION_MS, 0U);
    mTimerSRSRollover = new Timer(mTimerHandlerSRS, static_cast<int32_t>(vehicle::TIMER_SRS_ROLLOVER_ID));
    mTimerSRSRollover->setDurationMs(timeMsParams::SRS_SEPARATING_DURATION_MS, 0U);

    if (mVCMService != nullptr) {
        mVCMService = nullptr;
    }

    if (mVehicleReceiver == nullptr) {
        mVehicleReceiver = std::move(android::sp<IVehicleReceiver>{new VCMReceiver(*this)});
    }

    // Get service provided by the VehicleManagement
    mVCMService = android::interface_cast<IVehicleManagerService> (
                android::defaultServiceManager()->getService(
                    android::String16(VEHICLE_SRV_NAME)
                    )
                );

    bool error{true};
    if(mVCMService != nullptr) {
        // Register the callback function to be invoked when the service is dead
        if (android::OK == android::IInterface::asBinder(mVCMService)->linkToDeath(mServiceDeathRecipient)) {
            const uint32_t moduleId {static_cast<uint32_t>(getpid())}; // Change to getpid to avoid dupplicate with another module
            // Register the service with VehicleManager:
            // Let VehicleManager knows that upon any change of signals, it should call the registered stub method
            if (mVCMService->registerVehicleReceiver(moduleId, mVehicleReceiver) == android::OK)
            {
                error = false;
                if (mVCMService != nullptr) {
                    mVCMService->addSignalIndex(moduleId, SigInd_CanRxSRS_CDS_FRONT);
                    mVCMService->addSignalIndex(moduleId, SigInd_CanRxSRS_CDS_REAR);
                    mVCMService->addSignalIndex(moduleId, SigInd_CanRxSRS_CDS_SIDE);
                    mVCMService->addSignalIndex(moduleId, SigInd_CanRxSRS_CDS_ROLLOVER);
#ifdef CAN_TYPE_GEN3
                    mVCMService->addSignalIndex(moduleId, SigInd_CanRxMES_STATUS_NOTIFICATION);
                    mVCMService->addSignalIndex(moduleId, SigInd_CanRxECALL_RECEIVED_VALUE);
                    mVCMService->addSignalIndex(moduleId, SigInd_CanTx_ECALL_STATUS_NOTIFICATION_TEL_0CD9AAC7);
                    mVCMService->addSignalIndex(moduleId, SigInd_CanTx_MES_RECEIVED_VALUE_TEL_0CD9AAC7);
#endif
#ifdef CAN_TYPE_GEN2P
                    mVCMService->addSignalIndex(moduleId, SigInd_CanRxMES_STATUS_NOTIFICATION);
                    mVCMService->addSignalIndex(moduleId, SigInd_CanRxECALL_RECEIVED_VALUE);
                    mVCMService->addSignalIndex(moduleId, SigInd_CanRxRODS_REQ_CPA_SMARTPHONE_ALERT);
                    mVCMService->addSignalIndex(moduleId, SigInd_CanTx_ECALL_STATUS_NOTIFICATION);
                    mVCMService->addSignalIndex(moduleId, SigInd_CanTx_MES_RECEIVED_VALUE);

                    // TODO: so far non _RES_ required, just _REQ_
                    // however, in near future we could have it
                    // mVCMService->addSignalIndex(moduleId,SigInd_CanTx_TEL_RES_CPA_SMARTPHONE_ALERT);
#endif
                }
            }
        }
    }

    // If the service is not available, try to register it again after a delay
    if (error) {
        LOG_E(DLT_STRING("Cannot register VCM Service, try again after ms: "), DLT_UINT(timeMsParams::TIME_SEND_RETRY_DELAY_MS));
        (void)(mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_VCMMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS));
    }
}

/**
 * onReceivedCanSignal:
 * This method is call by VehicleManagerAdapter::onReceived
 * It handles the received signal from VehicleManager application
 */
void VehicleManagerAdapter::onReceivedCanSignal(const uint32_t channel, const android::sp<VehicleData>& vehicleData){
    mHandler = ECallAppHandler::getInstance();
    if (mHandler == nullptr) {
        LOG_E(DLT_STRING("mHandler nullptr"));
    }else {
        if (channel == data_channel::CAN && vehicleData->valUint32 == 1) {
            if (
                vehicleData->sigID == SigInd_CanRxSRS_CDS_FRONT ||
                vehicleData->sigID == SigInd_CanRxSRS_CDS_REAR ||
                vehicleData->sigID == SigInd_CanRxSRS_CDS_SIDE ||
                vehicleData->sigID == SigInd_CanRxSRS_CDS_ROLLOVER

            ) {
                LOG_I(DLT_STRING("onReceivedCanSignal, "), DLT_UINT(vehicleData->sigID));
                (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_SRS_SIGNAL), vehicleData->sigID)->sendToTarget());
            }
        }
        if (channel == data_channel::CAN) {
            switch (vehicleData->sigID) {
#ifdef CAN_TYPE_GEN3
            case SigInd_CanRxMES_STATUS_NOTIFICATION:
            if(vehicleData->valType == static_cast<uint32_t>(vehicle_data_type::TYPE_UINT32)) {
                LOG_I(DLT_STRING("onReceivedCanSignal, SigInd_CanRxMES_STATUS_NOTIFICATION"));
                mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::MES_STATUS_NOTIFICATION), vehicleData->valUint32));
            }
            break;

            case SigInd_CanTx_MES_RECEIVED_VALUE_TEL_0CD9AAC7:
            if(vehicleData->valType == static_cast<uint32_t>(vehicle_data_type::TYPE_UINT32)) {
                LOG_I(DLT_STRING("onReceivedCanSignal, SigInd_CanTx_MES_RECEIVED_VALUE_TEL_0CD9AAC7"));
                mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::MES_RECEIVED_VALUE), vehicleData->valUint32));
            }
            break;

            case SigInd_CanTx_ECALL_STATUS_NOTIFICATION_TEL_0CD9AAC7:
            if(vehicleData->valType == static_cast<uint32_t>(vehicle_data_type::TYPE_UINT32)) {
                LOG_I(DLT_STRING("onReceivedCanSignal, SigInd_CanTx_ECALL_STATUS_NOTIFICATION_TEL_0CD9AAC7"));
                mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::MES_ECALL_STATUS_NOTIFICATION), vehicleData->valUint32));
            }
            break;
            case SigInd_CanRxECALL_RECEIVED_VALUE:
            if(vehicleData->valType == static_cast<uint32_t>(vehicle_data_type::TYPE_UINT32)) {
                LOG_I(DLT_STRING("onReceivedCanSignal, SigInd_CanRxECALL_RECEIVED_VALUE"));
                mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::MES_ECALL_RECEIVED_VALUE), vehicleData->valUint32));
            }
            break;
#endif

#ifdef CAN_TYPE_GEN2P
            case SigInd_CanRxMES_STATUS_NOTIFICATION:
            if(vehicleData->valType == static_cast<uint32_t>(vehicle_data_type::TYPE_UINT32)) {
                LOG_I(DLT_STRING("GEN2P, onReceivedCanSignal, SigInd_CanRxMES_STATUS_NOTIFICATION"));
                mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::MES_STATUS_NOTIFICATION), vehicleData->valUint32));
            }
            break;

            case SigInd_CanTx_MES_RECEIVED_VALUE:
            if(vehicleData->valType == static_cast<uint32_t>(vehicle_data_type::TYPE_UINT32)) {
                LOG_I(DLT_STRING("GEN2P, onReceivedCanSignal, SigInd_CanTx_MES_RECEIVED_VALUE_TEL_0CD9AAC7"));
                mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::MES_RECEIVED_VALUE), vehicleData->valUint32));
            }
            break;

            case SigInd_CanRxRODS_REQ_CPA_SMARTPHONE_ALERT:
            if(vehicleData->valType == static_cast<uint32_t>(vehicle_data_type::TYPE_UINT32)) {
                LOG_I(DLT_STRING("GEN2P, onReceivedCanSignal, SigInd_CanRxRODS_REQ_CPA_SMARTPHONE_ALERT"));
                mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::REQ_CPA_SMARTPHONE_ALERT), vehicleData->valUint32));
            }
            break;

            case SigInd_CanTx_ECALL_STATUS_NOTIFICATION:
            if(vehicleData->valType == static_cast<uint32_t>(vehicle_data_type::TYPE_UINT32)) {
                LOG_I(DLT_STRING("GEN2P, onReceivedCanSignal, SigInd_CanTx_ECALL_STATUS_NOTIFICATION_TEL_0CD9AAC7"));
                mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::MES_ECALL_STATUS_NOTIFICATION), vehicleData->valUint32));
            }
            break;
            case SigInd_CanRxECALL_RECEIVED_VALUE:
            if(vehicleData->valType == static_cast<uint32_t>(vehicle_data_type::TYPE_UINT32)) {
                LOG_I(DLT_STRING("GEN2P, onReceivedCanSignal, SigInd_CanRxECALL_RECEIVED_VALUE"));
                mHandler->sendMessage(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::MES_ECALL_RECEIVED_VALUE), vehicleData->valUint32));
            }
            break;
#endif
            default:
            break;
            }
        }
    }
}

/*
handleSRSSignal:
This method is responsible for handling the SRS signal.
It is call when the message RECV_MSG_SRS_SIGNAL is received at ECallAppHandler.
The message RECV_MSG_SRS_SIGNAL is added to the message queue by upon receiving 1 out of the following SRS signal:
  1. SigInd_CanRxSRS_CDS_FRONT
  2. SigInd_CanRxSRS_CDS_REAR
  3. SigInd_CanRxSRS_CDS_SIDE
  4. SigInd_CanRxSRS_CDS_ROLLOVER
*/
void VehicleManagerAdapter::handleSRSSignal(const uint32_t sigID) {
    LOG_I(DLT_STRING("handleSRSSignal, "), DLT_UINT(sigID));
    if (sigID == SigInd_CanRxSRS_CDS_FRONT) {
        if (!mIsTimerSRSFrontRunning) {
            mTimerSRSFront->stop();
            mTimerSRSFront->start();
            mIsTimerSRSFrontRunning = true;
            (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER), sigID)->sendToTarget());
        }
    }
    else if (sigID == SigInd_CanRxSRS_CDS_REAR) {
        if (!mIsTimerSRSRearRunning) {
            mTimerSRSRear->stop();
            mTimerSRSRear->start();
            mIsTimerSRSRearRunning = true;
            (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER), sigID)->sendToTarget());
        }
    }
    else if (sigID == SigInd_CanRxSRS_CDS_SIDE) {
        if (!mIsTimerSRSSideRunning) {
            mTimerSRSSide->stop();
            mTimerSRSSide->start();
            mIsTimerSRSSideRunning = true;
            (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER), sigID)->sendToTarget());
        }
    }
    else if (sigID == SigInd_CanRxSRS_CDS_ROLLOVER) {
        if (!mIsTimerSRSFrontRunning) {
            mTimerSRSRollover->stop();
            mTimerSRSRollover->start();
            mIsTimerSRSRolloverRunning = true;
            (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER), sigID)->sendToTarget());
        }
    }
    else {

    }
}

void VehicleManagerAdapter::sendVehicleData(const uint32_t channel, android::sp<VehicleData> data) {
    if (mVCMService != nullptr) {
        (void)(mVCMService->sendVehicleData(channel, data));
    }
}

void VehicleManagerAdapter::sendBCAN_CANCEL_DISP(const uint32_t value) {
    LOG_I(DLT_STRING("sendBCAN_CANCEL_DISP, value = "), DLT_UINT(value));
    if (value != 0) {
        send_BCAN_CANCEL_DISP(value);
    }
    else {
        mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::REQ_SEND_BCAN_CANCEL_DISP), value), 500);
    }
}

void VehicleManagerAdapter::send_BCAN_CANCEL_DISP(const uint32_t value) {
    android::sp < VehicleData > data = new VehicleData();
#ifdef CAN_TYPE_GEN3
    LOG_E(DLT_STRING("[VCM] CAN_TYPE_GEN3"));
    data->sigID = SigInd_CanTx_TEL_C_ECL_CANCELDISP_REQ_TEL_11F9CFAE;
#elif defined(CAN_TYPE_GEN2P)
    data->sigID = SigInd_CanTx_TEL_C_ECL_CANCELDISP_REQ;
#else
    data->sigID = SigInd_CanTx_C_ECL_CANCELDISP_REQ;
#endif
    data->valUint32 = value;
    data->valType= TYPE_UINT32;
    sendVehicleData(data_channel::CAN, data);
}

void VehicleManagerAdapter::sendACKToRVU(const uint32_t value) {
    LOG_I(DLT_STRING("value: "), DLT_UINT(value));
#ifdef CAN_TYPE_GEN3
    LOG_I(DLT_STRING("GEN3"));
    android::sp < VehicleData > data = new VehicleData();
    data->sigID = SigInd_CanTx_MES_RECEIVED_VALUE_TEL_0CD9AAC7;
    data->valUint32 = value;
    data->valType= TYPE_UINT32;
    sendVehicleData(data_channel::CAN, data);
#endif
#ifdef CAN_TYPE_GEN2P
    LOG_I(DLT_STRING("GEN2P"));
    android::sp < VehicleData > data = new VehicleData();
    data->sigID = SigInd_CanTx_MES_RECEIVED_VALUE;
    data->valUint32 = value;
    data->valType= TYPE_UINT32;
    sendVehicleData(data_channel::CAN, data);
#endif
}


void VehicleManagerAdapter::notifySOSPressedToRVU(const uint32_t value){
    LOG_I(DLT_STRING("value: "), DLT_UINT(value));
#ifdef CAN_TYPE_GEN3
    LOG_I(DLT_STRING("GEN3"));
    android::sp < VehicleData > data = new VehicleData();
    data->sigID = SigInd_CanTx_ECALL_STATUS_NOTIFICATION_TEL_0CD9AAC7;
    data->valUint32 = value;
    data->valType= TYPE_UINT32;
    sendVehicleData(data_channel::CAN, data);
#endif
#ifdef CAN_TYPE_GEN2P
    LOG_I(DLT_STRING("GEN2P"));
    android::sp < VehicleData > data = new VehicleData();
    data->sigID = SigInd_CanTx_ECALL_STATUS_NOTIFICATION;
    data->valUint32 = value;
    data->valType= TYPE_UINT32;
    sendVehicleData(data_channel::CAN, data);
#endif
}


uint8_t VehicleManagerAdapter::getCanData(const SigInd_Rx id) {
    auto vcmData = mVCMService->getVehicleValue(id,GET_CAN_OPTION::CHANGED);
    if (vcmData == nullptr || vcmData->valType == vehicle_data_type::TYPE_ERROR) {
        return 0U;
    }

    uint8_t value{0U}; // just 1 byte
    switch (vcmData->valType) {
    case vehicle_data_type::TYPE_INT32: {
        value = vcmData->valInt32 & 0xFF;
        break;
    }
    case vehicle_data_type::TYPE_INT64: {
        value = vcmData->valInt64 & 0xFF;
        break;
    }
    case vehicle_data_type::TYPE_UINT32: {
        value = vcmData->valUint32 & 0xFF;
        break;
    }
    case vehicle_data_type::TYPE_UINT64: {
        value = vcmData->valUint64 & 0xFF;
        break;
    }
    case vehicle_data_type::TYPE_FLOAT: {
        break;
    }
    case vehicle_data_type::TYPE_DOUBLE: {
        break;
    }
    case vehicle_data_type::TYPE_STREAM: {
        auto data =  vcmData->buffer->data();
        if (data) {
            value = (*data) & 0xFF;
        }
        else {
            LOG_E(DLT_STRING("data is null"));
        }
        break;
    }
    case vehicle_data_type::TYPE_ERROR: {
        LOG_E(DLT_STRING("Cannot get vehicle data of frame ID: "));
         break;
    }
    default: {
        LOG_E(DLT_STRING("Wrong value type"));
        break;
    }
    }
    return value;
}


/**
 * getVehicleData:
 * This method is used handle when the binder is dead. It would try to re-register the service once more.
 */
void VehicleManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who){
    LOG_I(DLT_STRING("onBinderDied"));
    NOTUSED(who);
    (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_VCMMGR))->sendToTarget());
}



/**
 * onReceived:
 * This method plays a role of stub/server method at the server side in the Binder IPC communication.
 * It is trigger by the application VehicleManager application when it receives the data from the vehicle
 * and needs to send it to the OEMCallapp.
 */

void VCMReceiver::onReceived(const uint32_t channel, const sp<VehicleData>& vehicleData) {
    LOG_I(DLT_STRING("onReceived"));
    if ((channel == data_channel::CAN) ||  (channel == data_channel::ETHERNET)) {
        vcmRC.onReceivedCanSignal(channel, vehicleData);
    }
}


/**
 * onTimerTimeoutEvent:
 * This method plays a role of stub/server method at the server side in the Binder IPC communication.
 * It is trigger by the application VehicleManager application when it receives the data from the vehicle
 * and needs to send it to the OEMCallapp.
 */
void VehicleManagerAdapter::onTimerTimeoutEvent(const int32_t& msgId) {
    LOG_I(DLT_STRING("onTimerTimeoutEvent, "), DLT_INT(msgId));
    if (msgId == vehicle::TIMER_SRS_FRONT_ID) {
        mIsTimerSRSFrontRunning = false;
    }
    else if (msgId == vehicle::TIMER_SRS_SIDE_ID) {
        mIsTimerSRSSideRunning = false;
    }
    else if (msgId == vehicle::TIMER_SRS_REAR_ID) {
        mIsTimerSRSRearRunning = false;
    }
    else if (msgId == vehicle::TIMER_SRS_ROLLOVER_ID) {
        mIsTimerSRSRolloverRunning = false;
    }
}

void VehicleManagerAdapter::collectCanData(){
    LOG_I(DLT_STRING("Collecting Telemetry data"));

    mCrashTelemetry.clear();

    Json::Value crash_location;
    Json::Value belts;
    Json::Value delta_vmax_x1;
    Json::Value delta_vmax_y1;
    Json::Value delta_v;
    Json::Value occupacy;

    std::string frontalCrashSensor = getVehicleData(SigInd_CanRxSRS_CDS_FRONT, "unknown");
    std::string sideCrashSensor = getVehicleData(SigInd_CanRxSRS_CDS_SIDE,"unknown");
    std::string rearEndCrashSensor = getVehicleData(SigInd_CanRxSRS_CDS_REAR,"unknown");
    crash_location["frontalCrashSensor"] = frontalCrashSensor;
    crash_location["sideCrashSensor"]   = sideCrashSensor;
    crash_location["rearEndCrashSensor"]= rearEndCrashSensor;
    std::string driverBeltStatus;
#if defined(CAN_TYPE_GEN3)||defined(CAN_TYPE_GEN2P)
    LOG_E(DLT_STRING("CAN_TYPE_GEN3 | 2P"));
    driverBeltStatus              = getVehicleData(SigInd_CanRxSRS_DR_BELT_STATUS_FD, "unknown");
    belts["firstRowPassenger"]    = getVehicleData(SigInd_CanRxSRS_PS_BELT_STATUS_FD, "unknown");
    delta_vmax_x1 ["value"]       = getInt(mVCMService, SigInd_CanRxSRS_EDR_DELTA_VMAX_X_1_FD);
    delta_vmax_y1["value"]        = getInt(mVCMService, SigInd_CanRxSRS_EDR_DELTA_VMAX_Y_1_FD);
    occupacy["firstRowPassenger"] = getVehicleData(SigInd_CanRxSRS_PS_SITTING_FD, "unknown");
#else
    driverBeltStatus              = getVehicleData(SigInd_CanRxSRS_DR_BELT_STATUS, "unknown");
    belts["firstRowPassenger"]    = getVehicleData(SigInd_CanRxSRS_PS1_BELT_STATUS, "unknown");
    delta_vmax_x1 ["value"]       = getInt(mVCMService, SigInd_CanRxSRS_EDR_DELTA_VMAX_X_1);
    delta_vmax_y1["value"]        = getInt(mVCMService, SigInd_CanRxSRS_EDR_DELTA_VMAX_Y_1);
    occupacy["firstRowPassenger"] = getVehicleData(SigInd_CanRxSRS_PS_OCCUPIED, "unknown");
#endif
    belts["firstRowDriver"]       = driverBeltStatus;
    delta_vmax_x1 ["unit"]        = "kph";
    delta_vmax_y1 ["unit"]        = "kph";
    delta_v["deltaVmaxX1"]        = delta_vmax_x1;
    delta_v["deltaVmaxY1"]        = delta_vmax_y1;
    occupacy["firstRowDriver"]    = "notApplicable";
    mCrashTelemetry["crashLocation"]             = crash_location;
    std::string airbagDeploy                     = getVehicleData(SigInd_CanRxSRS_AIRBAG_DEPLOY,"unknown");
    mCrashTelemetry["airbagDeploymentIndicator"] = airbagDeploy;
    std::string rollOverDetect                   = getVehicleData(SigInd_CanRxSRS_CDS_ROLLOVER,"unknown");
    mCrashTelemetry["rollover"]                  = rollOverDetect;
    mCrashTelemetry["occupancy"]                 = occupacy;
    mCrashTelemetry["belts"]                     = belts;
    mCrashTelemetry["deltaV"]                    = delta_v;


    SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::CRASH_LOCATION_FRONTAL_CRASH_SENSOR, frontalCrashSensor);
    SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::CRASH_LOCATION_SIDE_CRASH_SENSOR, sideCrashSensor);
    SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::CRASH_LOCATION_REAR_END_CRASH_SENSOR, rearEndCrashSensor);
}

Json::Value VehicleManagerAdapter::getCrashTelemetry(){
    return mCrashTelemetry;
}

Json::Value VehicleManagerAdapter::getEventDetails() {
    return mEventDetail;
}

/**
 * getFirstByte:
 * This method is used to get the first byte of the vcmData.
 * For example: if the vcmData is 0x12345678, the first byte is 0x78
 */
uint8_t VehicleManagerAdapter::getFirstByte(const android::sp<VehicleData>& vcmData, const uint32_t frameId) {
    uint8_t value = 2; // just 1 byte
    switch (vcmData->valType) {
    case vehicle_data_type::TYPE_INT32: {
        value = vcmData->valInt32 & 0xFF;
        break;
    }
    case vehicle_data_type::TYPE_INT64: {
        value = vcmData->valInt64 & 0xFF;
        break;
    }
    case vehicle_data_type::TYPE_UINT32: {
        value = vcmData->valUint32 & 0xFF;
        break;
    }
    case vehicle_data_type::TYPE_UINT64: {
        value = vcmData->valUint64 & 0xFF;
        break;
    }
    case vehicle_data_type::TYPE_FLOAT: {
        break;
    }
    case vehicle_data_type::TYPE_DOUBLE: {
        break;
    }
    case vehicle_data_type::TYPE_STREAM: {
        auto data =  vcmData->buffer->data();
        if (data) {
            value = (*data) & 0xFF;
        }
        else {
            LOG_E(DLT_STRING("data is null"));
        }
        break;
    }
    case vehicle_data_type::TYPE_ERROR: {
        LOG_E(DLT_STRING("Cannot get vehicle data of frame ID:"), DLT_UINT(frameId));
        break;
    }
    default: {
        LOG_E(DLT_STRING("Wrong value type"));
        break;
    }
    }
    return value;
}

int32_t VehicleManagerAdapter::getInt(const android::sp<IVehicleManagerService>& spVCM, const SigInd_Rx id) {
    auto vcmData= spVCM->getVehicleValue(id,GET_CAN_OPTION::CHANGED);
    int32_t ret = 0;
    if ( (vcmData == nullptr) || (vcmData->valType == vehicle_data_type::TYPE_ERROR) ) {
        LOG_E(DLT_STRING("Cannot get VCM data of id ="), DLT_UINT(static_cast<uint32_t>(id)));
    }
    else {
        auto value = getFirstByte(vcmData, id);
        if (value <= 127) {
            ret = CommonFunctions::convertUint32ToInt32(static_cast<uint32_t>(value));
        }
        else {
            ret = static_cast<int32_t>(value) - 256;
        }
    }
    return ret;
}


std::string VehicleManagerAdapter::getString(const SigInd_Rx id, std::string defaultValue) {
    std::string ret = defaultValue;
    if (mVCMService == nullptr) {
        LOG_E(DLT_STRING("VehicleService is nullptr"));
    } else {
        auto vcmData = mVCMService->getVehicleValue(id,GET_CAN_OPTION::CHANGED);
        if (vcmData == nullptr || vcmData->valType == vehicle_data_type::TYPE_ERROR) {
            LOG_E(DLT_STRING("Cannot get VCM data of id ="), DLT_UINT(static_cast<uint32_t>(id)));
        }
        else {
            auto value = getFirstByte(vcmData, id);
#if defined(CAN_TYPE_GEN3)||defined(CAN_TYPE_GEN2P)
            if (id == SigInd_CanRxSRS_DR_BELT_STATUS_FD || id == SigInd_CanRxSRS_PS_BELT_STATUS_FD) {
#else
            if (id == SigInd_CanRxSRS_DR_BELT_STATUS || SigInd_CanRxSRS_PS1_BELT_STATUS) {
#endif
                switch(value) {
                    case 0:
                        ret = "unknown";
                        break;
                    case 1:
                        ret = "notActivated";
                        break;
                    case 2:
                        ret = "activated";
                        break;
                    case 3:
                        ret = "failure";
                        break;
                    default: {
                        LOG_E(DLT_STRING("Wrong value"));
                        break;
                    }
                }
            }
            else {
                switch(value) {
                    case 0:
                        ret = "notActivated";
                        break;
                    case 1:
                        ret = "activated";
                        break;
                    default: {
                        LOG_E(DLT_STRING("Wrong value"));
                        break;
                    }
                }
            }
        }
    }
    return ret;
}

void VehicleManagerAdapter::clearCanData() {
    LOG_I(DLT_STRING("Clear CAN data"));
    mCanData.clear();
}

std::string VehicleManagerAdapter::getVehicleData(const SigInd_Rx id,const std::string& defaultStringValue) {
    if (mCanData[id].empty()) {
        mCanData[id] = getString(id, defaultStringValue);
        LOG_I(DLT_STRING("CanId["), DLT_INT(static_cast<uint32_t>(id)), DLT_STRING("]:"), DLT_STRING(mCanData[id].c_str()));
    }
    else {
        LOG_I(DLT_STRING("CanId["), DLT_INT(static_cast<uint32_t>(id)), DLT_STRING("] already exists, value: "), DLT_STRING(mCanData[id].c_str()));
    }
    return mCanData[id];
}

void VehicleManagerAdapter::collectIntialData() {
    LOG_I(DLT_STRING("Collecting initial data"));
#if defined(CAN_TYPE_GEN3)||defined(CAN_TYPE_GEN2P)
    getVehicleData(SigInd_CanRxSRS_DR_BELT_STATUS_FD, "unknown");
    getVehicleData(SigInd_CanRxSRS_PS_BELT_STATUS_FD, "unknown");
    getVehicleData(SigInd_CanRxSRS_PS_SITTING_FD, "unknown");
    //save SRS_PS_OCCUPIED, SRS_DR_BELT_STATUS, SRS_PS1_BELT_STATUS
    SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::CRASH_VEHICLE_SRS_DR_BELT_STATUS, mCanData[SigInd_CanRxSRS_DR_BELT_STATUS_FD]);
    SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::CRASH_VEHICLE_SRS_PS1_BELT_STATUS, mCanData[SigInd_CanRxSRS_PS_BELT_STATUS_FD]);
    SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::CRASH_VEHICLE_SRS_PS_OCCUPIED, mCanData[SigInd_CanRxSRS_PS_SITTING_FD]);
#else
    getVehicleData(SigInd_CanRxSRS_DR_BELT_STATUS, "unknown");
    getVehicleData(SigInd_CanRxSRS_PS1_BELT_STATUS, "unknown");
    getVehicleData(SigInd_CanRxSRS_PS_OCCUPIED, "unknown");
    //save SRS_PS_OCCUPIED, SRS_DR_BELT_STATUS, SRS_PS1_BELT_STATUS
    SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::CRASH_VEHICLE_SRS_DR_BELT_STATUS, mCanData[SigInd_CanRxSRS_DR_BELT_STATUS]);
    SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::CRASH_VEHICLE_SRS_PS1_BELT_STATUS, mCanData[SigInd_CanRxSRS_PS1_BELT_STATUS]);
    SystemPropertyAdapter::getInstance()->saveDataToProperty(SystemProperty::CRASH_VEHICLE_SRS_PS_OCCUPIED, mCanData[SigInd_CanRxSRS_PS_OCCUPIED]);
#endif
}

void VehicleManagerAdapter::restoreAtSignalDetectingTimePartOfMSD() {
    #if defined(CAN_TYPE_GEN3)||defined(CAN_TYPE_GEN2P)
    std::string defaultValue = getString(SigInd_CanRxSRS_DR_BELT_STATUS_FD, "");
    mCanData[SigInd_CanRxSRS_DR_BELT_STATUS_FD] = SystemPropertyAdapter::getInstance()->getDataFromProperty
       (SystemProperty::CRASH_VEHICLE_SRS_DR_BELT_STATUS, defaultValue);
    LOG_D(DLT_STRING("id: "), DLT_INT(static_cast<uint32_t>(SigInd_CanRxSRS_DR_BELT_STATUS_FD)),
        DLT_STRING("value: "), DLT_STRING(mCanData[SigInd_CanRxSRS_DR_BELT_STATUS_FD].c_str()));

    defaultValue = getString(SigInd_CanRxSRS_PS_BELT_STATUS_FD, "");
    mCanData[SigInd_CanRxSRS_PS_BELT_STATUS_FD] = SystemPropertyAdapter::getInstance()->getDataFromProperty
       (SystemProperty::CRASH_VEHICLE_SRS_PS1_BELT_STATUS, defaultValue);
    LOG_D(DLT_STRING("id: "), DLT_INT(static_cast<uint32_t>(SigInd_CanRxSRS_PS_BELT_STATUS_FD)),
        DLT_STRING("value: "), DLT_STRING(mCanData[SigInd_CanRxSRS_PS_BELT_STATUS_FD].c_str()));

    defaultValue = getString(SigInd_CanRxSRS_PS_SITTING_FD, "");
    mCanData[SigInd_CanRxSRS_PS_SITTING_FD] = SystemPropertyAdapter::getInstance()->getDataFromProperty
       (SystemProperty::CRASH_VEHICLE_SRS_PS_OCCUPIED, defaultValue);
    LOG_D(DLT_STRING("id: "), DLT_INT(static_cast<uint32_t>(SigInd_CanRxSRS_PS_SITTING_FD)),
        DLT_STRING("value: "), DLT_STRING(mCanData[SigInd_CanRxSRS_PS_SITTING_FD].c_str()));

    #else
    std::string defaultValue = getString(SigInd_CanRxSRS_DR_BELT_STATUS, "");
    mCanData[SigInd_CanRxSRS_DR_BELT_STATUS] =
            SystemPropertyAdapter::getInstance()->getDataFromProperty(SystemProperty::CRASH_VEHICLE_SRS_DR_BELT_STATUS, defaultValue);

    LOG_D(DLT_STRING("id: "),
          DLT_INT(static_cast<uint32_t>(SigInd_CanRxSRS_DR_BELT_STATUS)),
          DLT_STRING("value: "),
          DLT_STRING(mCanData[SigInd_CanRxSRS_DR_BELT_STATUS].c_str()));

    defaultValue = getString(SigInd_CanRxSRS_PS1_BELT_STATUS, "");
    mCanData[SigInd_CanRxSRS_PS1_BELT_STATUS] =
            SystemPropertyAdapter::getInstance()->getDataFromProperty(SystemProperty::CRASH_VEHICLE_SRS_PS1_BELT_STATUS, defaultValue);

    LOG_D(DLT_STRING("id: "),
          DLT_INT(static_cast<uint32_t>(SigInd_CanRxSRS_PS1_BELT_STATUS)),
          DLT_STRING("value: "),
          DLT_STRING(mCanData[SigInd_CanRxSRS_PS1_BELT_STATUS].c_str()));

    defaultValue = getString(SigInd_CanRxSRS_PS_OCCUPIED, "");
    mCanData[SigInd_CanRxSRS_PS_OCCUPIED] =
            SystemPropertyAdapter::getInstance()->getDataFromProperty(SystemProperty::CRASH_VEHICLE_SRS_PS_OCCUPIED, defaultValue);

    LOG_D(DLT_STRING("id: "),
          DLT_INT(static_cast<uint32_t>(SigInd_CanRxSRS_PS_OCCUPIED)),
          DLT_STRING("value: "),
          DLT_STRING(mCanData[SigInd_CanRxSRS_PS_OCCUPIED].c_str()));
    #endif
}

void VehicleManagerAdapter::setInitialStringData(const SigInd_Rx id, std::string& val) {
    LOG_D(DLT_STRING("id: "),
          DLT_INT(static_cast<uint32_t>(id)),
          DLT_STRING("value: "),
          DLT_STRING(val.c_str()));
    mCanData[id] = val;
}

std::string VehicleManagerAdapter::getOccupantsCount() {
    uint8_t count{1U}; // driver
    std::vector<SigInd_Rx> passengerSensorList;
#if defined(CAN_TYPE_GEN3)||defined(CAN_TYPE_GEN2P)
    passengerSensorList.push_back(SigInd_CanRxSRS_PS_SITTING_FD);
    passengerSensorList.push_back(SigInd_CanRxSRS_2ND_ROW_C_SITTING_FD);
    passengerSensorList.push_back(SigInd_CanRxSRS_2ND_ROW_L_SITTING_FD);
    passengerSensorList.push_back(SigInd_CanRxSRS_2ND_ROW_R_SITTING_FD);
    passengerSensorList.push_back(SigInd_CanRxSRS_3RD_ROW_C_SITTING_FD);
    passengerSensorList.push_back(SigInd_CanRxSRS_3RD_ROW_L_SITTING_FD);
    passengerSensorList.push_back(SigInd_CanRxSRS_3RD_ROW_R_SITTING_FD);
#else
    passengerSensorList.push_back(SigInd_CanRxSRS_PS_SITTING);
    passengerSensorList.push_back(SigInd_CanRxSRS_2ND_ROW_C_SITTING);
    passengerSensorList.push_back(SigInd_CanRxSRS_2ND_ROW_L_SITTING);
    passengerSensorList.push_back(SigInd_CanRxSRS_2ND_ROW_R_SITTING);
#endif
    for (auto& sensor : passengerSensorList) {
        std::string isOccupied = getVehicleData(sensor, "unknown");
        if (isOccupied == "activated") {
            count++;
        }
    }
    LOG_D(DLT_STRING("Occupants count: "), DLT_UINT(count));
    return std::to_string(count);
}

} /* End: namespace ECALLDCMNONREG */
