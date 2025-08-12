#include "data_collector_can.h"
#include "../logger.h"
#include <utils/Buffer.h>
// Service manager
#include <binder/IServiceManager.h>

// VCM
#include <services/VehicleManagerService/IVehicleManagerService.h>
#include <services/VehicleManagerService/IVehicleManagerServiceType.h>
#include <services/VehicleManagerService/IVehicleReceiver.h>
#include <services/VehicleManagerService/TSUDataIndex.h>
#include <services/VehicleManagerService/VehicleData.h>
#include <iomanip>


namespace oc {

/**
 * @brief This contains the private properties of DataBuilderUS class instance
 *
 */
struct CanDataCollector::Impl {
    Json::Value root;

    /**
     * @brief getFirstByte This is a helper function to get first byte of data from VehicleData type.
     * @param vcmData Pointer to VehicleData
     * @return value of first byte in the Vehicle Data
     */
    uint8_t getFirstByte(android::sp<VehicleData> vcmData, uint32_t frameId) {
        LOG_I(DLT_STRING("Type: " ), DLT_UINT(vcmData->valType));
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

//            value = vcmData->valFloat & 0xFF;
            break;
        }
        case vehicle_data_type::TYPE_DOUBLE: {
//            value = vcmData->valDouble & 0xFF;
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
            LOG_E(DLT_STRING("Cannot get vehicle data of frame ID: "), DLT_UINT(frameId));
            break;
        }
        default: {
            LOG_E(DLT_STRING("Wrong value type"));
            break;
        }
        }
        return value;
    }

    int getInt(const android::sp<IVehicleManagerService> spVCM, SigInd_Rx id) {
        auto vcmData= spVCM->getVehicleValue(id,GET_CAN_OPTION::CHANGED);
        if (vcmData == nullptr || vcmData->valType == vehicle_data_type::TYPE_ERROR) {
            LOG_E(DLT_STRING("Cannot get VCM data of id = "), DLT_UINT(id));
            return 0;
        }
        dumpData(vcmData.get());

        auto value = getFirstByte(vcmData, id);
        if (value <= 127) {
            return (int)value;
        }
        return (int)(value - 256);
    }

    std::string getString(const android::sp<IVehicleManagerService> spVCM, SigInd_Rx id, std::string defaultValue = "unknown") {
        auto vcmData = spVCM->getVehicleValue(id,GET_CAN_OPTION::CHANGED);
        if (vcmData == nullptr || vcmData->valType == vehicle_data_type::TYPE_ERROR) {
            LOG_E(DLT_STRING("Cannot get VCM data of id = "), DLT_UINT(id));
            return defaultValue;
        }
        dumpData(vcmData.get());


        auto value = getFirstByte(vcmData, id);
        switch(value) {
            case 0:
                return "notActivated";
            case 1:
                return "activated";
            case 2:
                return "notApplicable";
            case 3:
                return "unknown";
            default: {
                LOG_E(DLT_STRING("Wrong value"));
                break;
			}
            }
        return "unknown";
    }

    std::string decodeBelt(const android::sp< IVehicleManagerService> spVCM, SigInd_Rx id) {
        std::string ans = "unknown";
        auto vcmData = spVCM->getVehicleValue(id,GET_CAN_OPTION::CHANGED);
        if (vcmData == nullptr || vcmData->valType == vehicle_data_type::TYPE_ERROR) {
            LOG_E(DLT_STRING("Cannot get VCM data of id = "), DLT_UINT(id));
            return "unknown";
        }
        auto value = getFirstByte(vcmData, id);
        switch(value) {
        case 0:
            return "notActivated";
        case 1:
            return "activated";
        case 2:
            return "notApplicable";
        case 3:
            return "unknown";
        default: {
            LOG_E(DLT_STRING("Wrong value"));
            break;
		}
        }
        return "unknown";

    }

    void dumpData(VehicleData* data) {
        try {
            LOG_I(DLT_STRING("frame= "), DLT_UINT(data->sigID),
                  DLT_STRING(", valueType= "), DLT_UINT(data->valType),
                  DLT_STRING(", valueBuffer="), DLT_STRING(toByteArrayString(data->buffer->data(), data->buffer->size()).c_str()),
                  DLT_STRING(", valueInt32="), DLT_INT(data->valInt32),
                  DLT_STRING(", valueUInt32="), DLT_UINT(data->valUint32),
                  DLT_STRING(", valueInt64="), DLT_INT64(data->valInt64),
                  DLT_STRING(", valueUInt64="), DLT_UINT64(data->valUint64));
        } catch (const std::exception& ex) {
            LOG_E(DLT_STRING("Received invalid signal: "), DLT_UINT(data->sigID));
        }
    }

    std::string toByteArrayString(uint8_t* data, uint32_t size) {
        std::stringstream ss;
        for (uint32_t i = 0; i < size; i++) {
            ss << setfill('0') << setw(2) << data[i] << " ";
        }
        return ss.str();
    }

};

CanDataCollector::CanDataCollector()
    :impl_(new Impl()) {
}

void CanDataCollector::collect() {
    LOG_I(DLT_STRING("Collecting Telemetry data"));
    auto spVehicleMgrService = android::interface_cast<IVehicleManagerService>( android::defaultServiceManager()->getService(
         android::String16(VEHICLE_SRV_NAME)
     )
    );

    if (spVehicleMgrService == nullptr) {
        LOG_E(DLT_STRING("Cannot get instance of VCM service"));
        return;
    }

    impl_->root.clear();

    Json::Value crash_location;
    Json::Value belts;
    Json::Value delta_vmax_x1;
    Json::Value delta_vmax_y1;
    Json::Value delta_v;
    Json::Value occupacy;

    crash_location["frontalCrashSensor"] = impl_->getString(spVehicleMgrService, SigInd_CanRxSRS_CDS_FRONT,"notActivated");
    crash_location["sideCrashSensorL"]   = impl_->getString(spVehicleMgrService, SigInd_CanRxSRS_CDS_SIDE,"notActivated");
    crash_location["sideCrashSensorR"]   = impl_->getString(spVehicleMgrService, SigInd_CanRxSRS_CDS_SIDE,"notActivated");
    crash_location["rearEndCrashSensor"]= impl_->getString(spVehicleMgrService, SigInd_CanRxSRS_CDS_REAR,"notActivated");
#if defined(CAN_TYPE_GEN3)||defined(CAN_TYPE_GEN2P)
    LOG_I(DLT_STRING("CAN_TYPE_GEN3 or CAN_TYPE_GEN2P"));

    belts["firstRowDriver"]              = impl_->decodeBelt(spVehicleMgrService, SigInd_CanRxSRS_DR_BELT_STATUS_FD);
    belts["firstRowPassenger"]           = impl_->decodeBelt(spVehicleMgrService, SigInd_CanRxSRS_PS_BELT_STATUS_FD);
    delta_vmax_x1 ["value"]              = impl_->getInt(spVehicleMgrService, SigInd_CanRxSRS_EDR_DELTA_VMAX_X_1_FD);
    delta_vmax_y1["value"]               = impl_->getInt(spVehicleMgrService, SigInd_CanRxSRS_EDR_DELTA_VMAX_Y_1_FD);
    occupacy["firstRowPassenger"]        = impl_->getString(spVehicleMgrService, SigInd_CanRxSRS_PS_SITTING_FD);

#else
    belts["firstRowDriver"]              = impl_->decodeBelt(spVehicleMgrService, SigInd_CanRxSRS_DR_BELT_STATUS);
    belts["firstRowPassenger"]           = impl_->decodeBelt(spVehicleMgrService, SigInd_CanRxSRS_PS1_BELT_STATUS);
    delta_vmax_x1 ["value"]              = impl_->getInt(spVehicleMgrService, SigInd_CanRxSRS_EDR_DELTA_VMAX_X_1);
    delta_vmax_y1["value"]               = impl_->getInt(spVehicleMgrService, SigInd_CanRxSRS_EDR_DELTA_VMAX_Y_1);
    occupacy["firstRowPassenger"]        = impl_->getString(spVehicleMgrService, SigInd_CanRxSRS_PS_OCCUPIED);
#endif
    delta_vmax_x1 ["unit"]               = "kph";
    delta_vmax_y1 ["unit"]               = "kph";
    delta_v["deltaVmaxX1"]               = delta_vmax_x1;
    delta_v["deltaVmaxY1"]               = delta_vmax_y1;
    occupacy["firstRowDriver"]           = "notApplicable";


    impl_->root["crashLocation"]         = crash_location;
#ifdef CAN_TYPE_GEN3
    LOG_I(DLT_STRING("CAN_TYPE_GEN3"));
    impl_->root["airbagDeploymentIndicator"]= impl_->getString(spVehicleMgrService, SigInd_CanRxSRS_AIRBAG_DEPLOY,"notActivated");
#else
    impl_->root["airbagDeploymentIndicator"]= impl_->getString(spVehicleMgrService, SigInd_CanRxSRS_AIRBAG_DEPLOY,"notActivated");
#endif
    impl_->root["rollover"]              = impl_->getString(spVehicleMgrService, SigInd_CanRxSRS_CDS_ROLLOVER,"notActivated");
    impl_->root["occupancy"]             = occupacy;
    impl_->root["belts"]                 = belts;
    impl_->root["deltaV"]                = delta_v;
//        /* Telemetry Data*/
}

Json::Value CanDataCollector::get() {
    return impl_->root;
}

}
