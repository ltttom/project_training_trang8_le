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
 * @file        acn_phone_processor.h
 * @author
 *    copyright (c) 2025  Long Nhat Nguyen <nhatlong.nguyen@lgepartner.com>
 * @version     1.0.00
 */
#pragma once

#include "IDataBuilder.h"

namespace ecallapp
{

class DataBuilderCPDSmartphone: IDataBuilder
{
public:
    std::string build() override;
    IDataBuilder& setCDS(bool isNormalCDS) override;
    IDataBuilder& setGPS() override;
    IDataBuilder& setUtil(std::string eventName) override;
    IDataBuilder& setTriggerType(std::string triggerType, bool isDessCall) override;

private:
    Json::Value m_root;
    std::string m_eventName;
};

} // </ecallapp>


