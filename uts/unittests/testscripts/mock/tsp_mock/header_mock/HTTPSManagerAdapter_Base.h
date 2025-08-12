#ifndef ADAPTER_HTTPSMANAGERADAPTER_H_
#define ADAPTER_HTTPSMANAGERADAPTER_H_

#include <iostream>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <functional>
#include <map>
#include <curl/curl.h>

#include <utils/Handler.h>
#include <utils/SLLooper.h>
#include <utils/LooperThread.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include "../utils/EventDataBase.h"
#include "../utils/BackgroundTask.h"
#include <thread>

using namespace std;

namespace ecallapp {

constexpr const char *const HTTPS_RESP_JUDGE_OK = "OK";
constexpr const char *const HTTPS_RESP_JUDGE_NG = "NG";
constexpr const char *const HTTPS_RESP_REASON_NORMAL = "Normal";
constexpr const char *const HTTPS_RESP_REASON_DATAERR = "Data Err";
constexpr const char *const HTTPS_RESP_REASON_NODATA = "No Data";
constexpr const char *const HTTPS_RESP_REASON_SYSERR = "System Err";
constexpr const char *const REGISTERED_FALSE = "0";
constexpr const char *const REGISTERED_TRUE = "1";
constexpr const char *const RESP_JUDGE_OK = "OK";
constexpr const char *const RESP_JUDGE_NG = "NG";
constexpr const char *const RESP_JUDGE_UD = "UD";
constexpr const char *const RESP_REASON_NORMAL = "Normal";
constexpr const char *const RESP_REASON_NO_DATA = "No Data";
constexpr const char *const RESP_REASON_SYSTEM_ERR = "System Err";
constexpr const char *const RESP_REASON_DATA_ERR = "Data Err";
constexpr const char *const RESP_TELNO = "116";

// String For LGE MOCK SERVER.
constexpr const char *const STRING_REGISTERED = "Registered";
constexpr const char *const STRING_RESP_JUDGE = "Resp_Judge";
constexpr const char *const STRING_RESP_REASON = "Resp_Reason";
constexpr const char *const STRING_RESP_TELNO = "Resp_TelNo";

const std::string TEST_CENTER_URI = "http://hondatcusf.us-e2.cloudhub.io/v1/emergency-notification";
const std::string REAL_CENTER_URI = "http://hondatcusf.us-e2.cloudhub.io/v1/emergency-notification";

const uint32_t HTTP_RES_OK = 1U;
const uint32_t HTTP_RES_NOK = 0U;

constexpr const char *const SERVICE_FILE_PATH = "/usr/bin/app_proc";

typedef struct _HttpData_t
{
    std::map<std::string, std::string> mRespHeader;
    std::string respHeader;
    std::string respBody;
}
HttpData_t;

typedef struct _RespInfo_t
{
    std::string respHeader;
    std::string respBody;
}
RespInfo_t;

typedef struct MSDResp
{
    std::string Judge; //OK NG UD
    std::string TelNo;
} MSDResp_t;

class HttpEventData : public EventDataBase {
public:
    HttpEventData() {};
    ~HttpEventData() {};

public:
    HttpData_t mReceivedData;
};

class ResponseData : public android::RefBase {
public:
    ResponseData(const std::string& strHeader, const std::string& strBody)
        : RefBase()
    {
        mHeader = strHeader;
        mBody = strBody;
        ret = HTTP_RES_NOK;
    }
public:
    std::string mHeader;
    std::string mBody;
    uint32_t ret;
};

class HTTPSManagerAdapter
{
public:
    HTTPSManagerAdapter();
    virtual ~HTTPSManagerAdapter();
    static HTTPSManagerAdapter* getInstance();

    virtual void onDestroy(); // Application is destroyed.
    virtual void sendRequestAsync1(string strURL, string dataBody, string strHttpMethod = "POST");
    virtual void sendRequestAsync2(string strURL, string dataBody, bool needAuthenServer = false, string strHttpMethod = "POST");
    virtual void registerService();
    virtual RespInfo_t* getRespData();
    virtual void clearRspData();
    virtual std::string getTelNo();
    virtual void setTelNo(std::string telNo);
    virtual void onReceivedData(const android::sp<sl::Message>& message);

private:
    virtual uint32_t sendRequest(string strURL, string dataBody, HttpData_t& respData,bool needAuthenServer = false, string strHttpMethod = "POST");
    virtual void sendRecvThreadFunc(string strURL, string strPayload, bool needAuthenServer = false, string strHttpMethod = "POST");
#ifdef REGION_PCB26
    void setSecurityEvent(const CURLcode& iResultCode);
#endif
private:
    virtual void initHttpData(HttpData_t *httpData);

    /* callback function */
    static size_t writeFunc(void *ptr, size_t size, size_t nmemb, HttpData_t *httpData);
    static size_t headerFunc(void *ptr, size_t size, size_t nmemb, HttpData_t *httpData);


private:
    static HTTPSManagerAdapter *instance;
    RespInfo_t mRspData{};
    MSDResp_t mMsdData{};
    std::unique_ptr<BackgroundTask> mBackgroundTask;
    std::unique_ptr<std::thread> mSendRecvThread;
};
}
#endif /* HTTPSADAPTER_H_ */
