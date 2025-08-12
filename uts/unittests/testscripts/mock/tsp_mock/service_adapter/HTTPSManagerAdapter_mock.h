#ifndef HTTPSMANAGERADAPTER_MOCK_H
#define HTTPSMANAGERADAPTER_MOCK_H

// using namespace ecallapp;

class MockHTTPSManagerAdapter : public HTTPSManagerAdapter {
public:
    MOCK_METHOD0(getInstance, HTTPSManagerAdapter*());
    MOCK_METHOD0(onDestroy, void());
    MOCK_METHOD3(sendRequestAsync1, void(std::string strURL, std::string dataBody, std::string strHttpMethod));
    MOCK_METHOD4(sendRequestAsync2, void(std::string strURL, std::string dataBody, bool needAuthenServer, std::string strHttpMethod));
    MOCK_METHOD0(registerService, void());
    MOCK_METHOD0(getRespData, RespInfo_t*());
    MOCK_METHOD0(clearRspData, void());
    MOCK_METHOD0(getTelNo, std::string());
    MOCK_METHOD1(onReceivedData, void(const android::sp<sl::Message>& message));
    MOCK_METHOD5(sendRequest, uint32_t(std::string strURL, std::string dataBody, HttpData_t& respData, bool needAuthenServer, std::string strHttpMethod));
    MOCK_METHOD4(sendRecvThreadFunc, void(std::string strURL, std::string strPayload, bool needAuthenServer, std::string strHttpMethod));
#ifdef REGION_PCB26
    MOCK_METHOD1(setSecurityEvent, void(const CURLcode& iResultCode));
#endif
    MOCK_METHOD1(initHttpData, void(HttpData_t *s));
    MOCK_METHOD4(writeFunc, size_t(void *ptr, size_t size, size_t nmemb, HttpData_t *s));
    MOCK_METHOD4(headerFunc, size_t(void *ptr, size_t size, size_t nmemb, HttpData_t *s));
};

MockHTTPSManagerAdapter* M_HTTPSManagerAdapter;

HTTPSManagerAdapter::HTTPSManagerAdapter() {}

HTTPSManagerAdapter::~HTTPSManagerAdapter() {}

HTTPSManagerAdapter* HTTPSManagerAdapter::instance = nullptr;

HTTPSManagerAdapter* HTTPSManagerAdapter::getInstance()
{
    if (instance == nullptr) {
        instance = new HTTPSManagerAdapter();
    }
    return instance;
}

void HTTPSManagerAdapter::onDestroy()
{
  M_HTTPSManagerAdapter->onDestroy();
}

void HTTPSManagerAdapter::sendRequestAsync1(std::string strURL, std::string dataBody, std::string strHttpMethod) {
    M_HTTPSManagerAdapter->sendRequestAsync1(strURL, dataBody, strHttpMethod);
}

void HTTPSManagerAdapter::sendRequestAsync2(std::string strURL, std::string dataBody, bool needAuthenServer, std::string strHttpMethod) {
    M_HTTPSManagerAdapter->sendRequestAsync2(strURL, dataBody, needAuthenServer, strHttpMethod);
}

void HTTPSManagerAdapter::registerService() {
    M_HTTPSManagerAdapter->registerService();
}

RespInfo_t* HTTPSManagerAdapter::getRespData() {
    return M_HTTPSManagerAdapter->getRespData();
}

void HTTPSManagerAdapter::clearRspData() {
    M_HTTPSManagerAdapter->clearRspData();
}

std::string HTTPSManagerAdapter::getTelNo() {
    return M_HTTPSManagerAdapter->getTelNo();
}

void HTTPSManagerAdapter::onReceivedData(const android::sp<sl::Message>& message) {
    M_HTTPSManagerAdapter->onReceivedData(message);
}

uint32_t HTTPSManagerAdapter::sendRequest(std::string strURL, std::string dataBody, HttpData_t& respData, bool needAuthenServer, std::string strHttpMethod) {
    return M_HTTPSManagerAdapter->sendRequest(strURL, dataBody, respData, needAuthenServer, strHttpMethod);
}

void HTTPSManagerAdapter::sendRecvThreadFunc(std::string strURL, std::string strPayload, bool needAuthenServer, std::string strHttpMethod) {
    M_HTTPSManagerAdapter->sendRecvThreadFunc(strURL, strPayload, needAuthenServer, strHttpMethod);
}

#ifdef REGION_PCB26
void HTTPSManagerAdapter::setSecurityEvent(const CURLcode& iResultCode) {
    M_HTTPSManagerAdapter->setSecurityEvent(iResultCode);
}
#endif

void HTTPSManagerAdapter::initHttpData(HttpData_t *s) {
    M_HTTPSManagerAdapter->initHttpData(s);
}


size_t HTTPSManagerAdapter::writeFunc(void *ptr, size_t size, size_t nmemb, HttpData_t *s) {
    return M_HTTPSManagerAdapter->writeFunc(ptr, size, nmemb, s);
}

size_t HTTPSManagerAdapter::headerFunc(void *ptr, size_t size, size_t nmemb, HttpData_t *s) {
    return M_HTTPSManagerAdapter->headerFunc(ptr, size, nmemb, s);
}

class Mock_BackgroundTask{
public:
    MOCK_METHOD0(run, void());
    MOCK_METHOD0(threadLoop, bool());
    MOCK_METHOD0(readyToRun, void());
};

Mock_BackgroundTask* M_BackgroundTask;

void BackgroundTask::run(){
    return M_BackgroundTask->run();
}
BackgroundTask::BackgroundTask(std::string strTaskName, TaskWorker taskWorker) : mTaskWorker(taskWorker), mTaskName(strTaskName){}
BackgroundTask::~BackgroundTask(){}
bool BackgroundTask::threadLoop(){
    return M_BackgroundTask->threadLoop();
}

#endif // HTTPSMANAGERADAPTER_MOCK_H