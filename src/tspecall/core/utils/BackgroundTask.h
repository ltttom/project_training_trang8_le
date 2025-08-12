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
 * @file        BackgroundTask.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef _BACKGROUND_TASK_H
#define _BACKGROUND_TASK_H

#include <functional>
#include <string>
#include <utils/Thread.h>

using android::Thread;

typedef std::function<void (std::string strTaskName)> TaskWorker;

namespace ecallapp {
class BackgroundTask : public Thread
{
public:
    BackgroundTask() = delete;
    BackgroundTask(std::string strTaskName, TaskWorker taskWorker);
    virtual ~BackgroundTask();

    void run();

private:
    virtual bool threadLoop();

private: 
    TaskWorker  mTaskWorker;
    std::string mTaskName;
};
}
#endif // _BACKGROUND_TASK_H
