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
 * @file        EventDataBase.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef _EVENT_DATA_BASE_H
#define _EVENT_DATA_BASE_H

class EventDataBase : public android::RefBase
{
public: 
    EventDataBase() {};
    ~EventDataBase() {};
};

#endif // _EVENT_DATA_BASE_H