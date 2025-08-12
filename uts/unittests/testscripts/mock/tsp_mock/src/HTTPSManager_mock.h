#ifndef HTTPS_MANAGER_MOCK_H
#define HTTPS_MANAGER_MOCK_H

// typedef std::function<void (std::string strTaskName)> TaskWorker;
// typedef struct _HttpData_t
// {
//     std::map<std::string, std::string> mRespHeader;
//     std::string respHeader;
//     std::string respBody;
// }
// HttpData_t;

class Mock_HTTPSManager{
public:
    // MOCK_METHOD0(getCallState,int32_t());
};

class Mock_BackgroundTask{
public:
    MOCK_METHOD0(run, void());
    MOCK_METHOD0(threadLoop, bool());
    MOCK_METHOD0(readyToRun, void());
};
Mock_BackgroundTask* M_BackgroundTask;

void BackgroundTask::run(){M_BackgroundTask->run();}
BackgroundTask::BackgroundTask(std::string strTaskName, TaskWorker taskWorker) : mTaskWorker(taskWorker), mTaskName(strTaskName){
    std::cout << "BackgroundTask constructor\n";
}
BackgroundTask::~BackgroundTask(){
    std::cout << "BackgroundTask destructor\n";
}
bool BackgroundTask::threadLoop(){return M_BackgroundTask->threadLoop();}

#endif //HTTPS_MANAGER_MOCK_H