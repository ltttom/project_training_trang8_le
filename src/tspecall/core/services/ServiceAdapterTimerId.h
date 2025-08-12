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
 * @file        ServiceAdapterTimerId.h
 * @author      lam.pham@lge.com
 * @version     1.0.00
*/
#ifndef ECALLNONREG_SERIVCEADAPTERTIMERID_H
#define ECALLNONREG_SERIVCEADAPTERTIMERID_H
namespace ecallapp {
// service adapter timer id (1 - 100)
namespace vehicle {
    static const int32_t TIMER_SRS_FRONT_ID{1};
    static const int32_t TIMER_SRS_SIDE_ID{2};
    static const int32_t TIMER_SRS_REAR_ID{3};
    static const int32_t TIMER_SRS_ROLLOVER_ID{4};
}

namespace appmgr {
    static const int32_t TIMER_2S_QUERYACTIONFORFEATURE_ID{14};
}

}


#endif