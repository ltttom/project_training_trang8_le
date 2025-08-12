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
 * @file        TimeManagerAdapter.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef _TIME_MANAGER_ADAPTER_H_
#define _TIME_MANAGER_ADAPTER_H_

#include <services/TimeManagerService/ITimeChangedReceiver.h>
#include <memory>
namespace ecallapp {

class TimeManagerAdapter {

    class TimeReceiver: public BnTimeChangedReceiver {
        /**
         * @brief Check if the TSU receives a valid time to recalculate the remaining time after recovery
         * @param time_type 
         * @param GMT_msec 
         * @param diff_time_msec 
         */
        void onTimeReceived(const int32_t time_type, const int64_t GMT_msec, const int64_t diff_time_msec) override;
        void onTimeOffsetReceived(const int32_t time_type, const int32_t offset_min, const int32_t dst) override;
    };
public:
    TimeManagerAdapter(TimeManagerAdapter const&) = delete;
    TimeManagerAdapter& operator=(TimeManagerAdapter const&) = delete;
    ~TimeManagerAdapter(){}
    static std::shared_ptr<TimeManagerAdapter>& getInstance();
    /**
     * @brief get current timestamp from TimeManager
     * @return Correct the timestamp if it is a valid time type; otherwise, return 0
     */
    static int64_t getCurrentTimestamp();
    /**
     * @brief Register the TimeManager service
     */
    void registerService();
private:
    explicit TimeManagerAdapter();
    android::sp<ITimeChangedReceiver> mTimeReceiver;
};
} //namespace ecallapp

#endif // _TIME_MANAGER_ADAPTER_H_