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
 * @file        vehicle_variation_info_adapter.h
 * @author      quang8.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef ADAPTER_VEHICLEVARIATIONINFO_H
#define ADAPTER_VEHICLEVARIATIONINFO_H
#ifdef REGION_PCB26
#include <VehicleVariationInfo.h>
#include <vector>
#include <string>
using namespace std;
#endif
#include <memory>

namespace ecallapp {

const std::string FEATURE_APPLIED = "O";
const std::string FEATURE_APPLIED_WITH_NOTATION = "O*";

namespace feature_name {
#ifdef REGION_PCB26
const std::string acn_phone = Column3::ACN_WITH_GTC;
const std::string ecall_us = Column3::ACN_E_CALL_FOR_US;
const std::string ecall_us_pf3 = Column3::ACN_ECALL_FOR_US;
const std::string rcall_japan = Column3::ROADSIDE_ASSISTANCE_CALL_FOR_JAPAN;
const std::string cpd_smartphone_allert = Column3::CHILD_PRESENCE_DETECTION_CPD;
const std::string na_dess_ecall = Column3::DRIVER_EMERGENCY_SUPPORT_SYSTEM_DESS;
#else
const std::string acn_phone = "acn-phone";
const std::string ecall_us = "ecall-us";
const std::string ecall_us_pf3 = "ecall-us";
const std::string rcall_japan = "rcall-japan";
const std::string cpd_smartphone_allert = "cpd_smartphone";
const std::string na_dess_ecall = "dess-us";
#endif
}

class VehicleVariationAdapter{
private:
    static VehicleVariationAdapter *instance;
#ifdef REGION_PCB26
    std::unique_ptr<VehicleVariationInfo> mVehicleVariationInfo;
    std::vector<VariationList::VariationInfo> mVariationData;
#endif
public:
    VehicleVariationAdapter();
    virtual ~VehicleVariationAdapter();
    static VehicleVariationAdapter* getInstance();

    /**
     * @brief check if the feature is available or not.
     */
    bool isFeatureAvailableInRegion(const std::string FeatureName);

    /**
     * Initialize the vector mVariationData by reading the appropriate part
     * of the CSV files.
     */
    void getAllInformation();
};

} /* namespace oc */
#endif /* ADAPTER_VEHICLEVARIATION_H */
