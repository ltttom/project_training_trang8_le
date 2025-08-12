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
 * @file        DataBuilderUS.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef TSP_DATA_BUILDER_US_H
#define TSP_DATA_BUILDER_US_H
#include "IDataBuilder.h"

namespace ecallapp {

class DataBuilderUS : public IDataBuilder {
public:

    DataBuilderUS();

    virtual std::string build() override;
    virtual IDataBuilder &setCDS(bool isNormalCDS) override;
    virtual IDataBuilder &setGPS() override;
    virtual IDataBuilder &setUtil(std::string ignition) override;
    virtual IDataBuilder &setTriggerType(std::string triggerType, bool isDessCall) override;
    static std::string getIgState(int32_t igState, int32_t accState);

private:
    struct Impl;
    std::shared_ptr<Impl> impl_;
};

}

#endif // TSP_DATA_BUILDER_US_H
