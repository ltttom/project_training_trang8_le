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
 * @file        vehicle_variation_info_adapter.cpp
 * @author      quang8.nguyen@lge.com
 * @version     1.0.00
*/
#include "VehicleVariationAdapter.h"
#include "logger.h"
#include "ApplicationManagerAdapter.h"

namespace ecallapp {
VehicleVariationAdapter::VehicleVariationAdapter() {
#ifdef REGION_PCB26
    mVehicleVariationInfo = std::make_unique<VehicleVariationInfo>();
    getAllInformation();
#endif
}

VehicleVariationAdapter::~VehicleVariationAdapter() {
}

VehicleVariationAdapter* VehicleVariationAdapter::instance = nullptr;
VehicleVariationAdapter* VehicleVariationAdapter::getInstance() {
    if (instance == nullptr) {
        instance = new VehicleVariationAdapter();
    } else {
    }
    return instance;
}


void VehicleVariationAdapter::getAllInformation()
{
#ifdef REGION_PCB26
    error_t return_val;
    // bool isAvailable = false;
    return_val = mVehicleVariationInfo->getVariationInfo(mVariationData, Column1::TSU_SW_VARIATION, Column2::CALL_FUNCTIONS);
    if (return_val != E_OK || mVariationData.empty()) {
        LOG_I(DLT_STRING("[VehicleInfo] Feature not available. Error ="), DLT_UINT(return_val));
        /**
         * By default, the features are available
         */
        mVariationData.push_back({Column1::TSU_SW_VARIATION, Column2::CALL_FUNCTIONS, Column3::ACN_E_CALL_FOR_US, FEATURE_APPLIED});
        mVariationData.push_back({Column1::TSU_SW_VARIATION, Column2::CALL_FUNCTIONS, Column3::ACN_ECALL_FOR_US, FEATURE_APPLIED});
        mVariationData.push_back({Column1::TSU_SW_VARIATION, Column2::CALL_FUNCTIONS, Column3::DRIVER_EMERGENCY_SUPPORT_SYSTEM_DESS, FEATURE_APPLIED});
        mVariationData.push_back({Column1::TSU_SW_VARIATION, Column2::CALL_FUNCTIONS, Column3::ROADSIDE_ASSISTANCE_CALL_FOR_JAPAN, FEATURE_APPLIED});
        mVariationData.push_back({Column1::TSU_SW_VARIATION, Column2::CALL_FUNCTIONS, Column3::ACN_WITH_GTC, FEATURE_APPLIED});
        mVariationData.push_back({Column1::TSU_SW_VARIATION, Column2::CALL_FUNCTIONS, Column3::CHILD_PRESENCE_DETECTION_CPD, FEATURE_APPLIED});
    }
    else {

    }
#endif
}

bool VehicleVariationAdapter::isFeatureAvailableInRegion(const std::string FeatureName)
{
    bool isAvailable = true;
#ifdef REGION_PCB26
    bool isFound = false;
    for(auto feature: mVariationData)
    {
      if(feature.column3 == FeatureName)
      {
          isFound = true;
          if(feature.value != FEATURE_APPLIED && feature.value != FEATURE_APPLIED_WITH_NOTATION)
              isAvailable = false;
          else isAvailable = true;
          break;
      } else {}
    }
    if(!isFound)
        isAvailable = false;

    LOG_I(DLT_STRING("[VehicleInfo] Feature availability: "),
          DLT_STRING(FeatureName.c_str()),
          DLT_STRING(" - "),
          DLT_BOOL(isAvailable));
#else
    isAvailable = ApplicationManagerAdapter::isAvailableInRegion(FeatureName);
#endif
    return isAvailable;
}
} /* namespace eCall */
