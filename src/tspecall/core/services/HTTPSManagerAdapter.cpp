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
 * @file        HTTPSManagerAdapter.cpp
 * @author      lam.pham@lge.com
 * @version     1.0.00
*/

#ifdef REGION_PCB26
#include "services/IdsibinderManagerService/IdsibinderManager.h"
#endif
#include <sys/syscall.h>
#include <json/json.h>

#include "logger.h"
#include "../utils/ParamsDef.h"
#include "HTTPSManagerAdapter.h"
#include "../utils/ECallAppHandler.h"
#include "tspecall/core/utils/CommonFunctions.h"

#undef NLOG__TAG
#define NLOG__TAG "eCallM_HTTPMA"

#define EMPTY_RESPONSE  ""

namespace ecallapp {
static const int HTTPS_RESP_OK = 200;
static const int HTTPS_TIMEOUT = 15L;          /* 15s */

HTTPSManagerAdapter::HTTPSManagerAdapter() {
    LOG_D(DLT_STRING("HTTPSManagerAdapter constructor"));
    mSendRecvThread = nullptr;
}

HTTPSManagerAdapter::~HTTPSManagerAdapter() {
    onDestroy();
}

HTTPSManagerAdapter* HTTPSManagerAdapter::instance = nullptr;
HTTPSManagerAdapter* HTTPSManagerAdapter::getInstance()
{
    if (instance == nullptr) {
        instance = new HTTPSManagerAdapter();
    }
    return instance;
}

void HTTPSManagerAdapter::registerService() {
    LOG_I(DLT_STRING("registerService"));
}

void HTTPSManagerAdapter::onDestroy() {
    if (mSendRecvThread != nullptr) {
        mSendRecvThread->detach();
        mSendRecvThread = nullptr;
    }
}


void HTTPSManagerAdapter::sendRequestAsync1(string strURL, string dataBody, string strHttpMethod)
{
    LOG_I(DLT_STRING("sendRequestAsync1"));
    HttpData_t respData;
    /* Create a background task */
    std::string strTaskName {"HttpsTask"};
    RespInfo_t& tRspData = mRspData;
    auto func = [&tRspData, &strURL, &dataBody, &respData, &strHttpMethod](std::string strTaskName) {
        /* Send request */
        uint32_t retCode = HTTPSManagerAdapter::getInstance()->sendRequest(strURL, dataBody, respData, false, strHttpMethod);
        /* Check response */
        if(retCode == HTTP_RES_OK)
        {
            // Copy data from respData -> tRspData
            tRspData.respHeader = respData.respHeader;
            tRspData.respBody = respData.respBody;
        }
        LOG_I(DLT_STRING("sendRequestAsync1 2"));
        sp<sl::Message> message =  ECallAppHandler::getInstance()->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_RECEIVED_DATA_NOTIFICATION), retCode);
        message->sendToTarget();
        LOG_I(DLT_STRING("sendRequestAsync1 3"));
    };
    mBackgroundTask = std::make_unique<BackgroundTask>(strTaskName, func);
    /* Run task */
    mBackgroundTask->readyToRun();
    mBackgroundTask->run();
}


void HTTPSManagerAdapter::sendRequestAsync2(string strURL, string dataBody, bool needAuthenServer, string strHttpMethod)
{
    LOG_I(DLT_STRING("sendRequestAsync2"));
    LOG_I(DLT_STRING("before clean thread"));
    onDestroy(); // clean thread before making new
    LOG_I(DLT_STRING("after clean thread"));
    mSendRecvThread = std::make_unique<std::thread>(&HTTPSManagerAdapter::sendRecvThreadFunc, this, strURL, dataBody, needAuthenServer, strHttpMethod);
}


void HTTPSManagerAdapter::sendRecvThreadFunc(string strURL, string strPayload, bool needAuthenServer, string strHttpMethod)
{
    LOG_I(DLT_STRING("sendRecvThreadFunc"));
    HttpData_t respData;
    CommonFunctions::printLongString(strPayload);
    uint32_t ret = sendRequest(strURL, strPayload, respData, needAuthenServer, strHttpMethod);

    android::sp<ResponseData> spReponseData{nullptr};
    spReponseData = new ResponseData("", "");
    spReponseData->ret = ret;
    if (ret == HTTP_RES_OK)
    {
        LOG_I(DLT_STRING("HTTP_RES_OK"));
        // Copy data from respData -> mRspData at the main thread
        // didn't coppy data to member object in here: mRspData.respHeader = respData.respHeader;
        // didn't coppy data to member object : mRspData.respBody = respData.respBody;
        spReponseData->mHeader = respData.respHeader;
        spReponseData->mBody = respData.respBody;
    }
    else {
        LOG_I(DLT_STRING("HTTP_RES_NOK"));
    }

    // Send msg to App to stop the task. Processor need to consider current callstate is standby then start stand by task.
    sp<sl::Message> message =  ECallAppHandler::getInstance()->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_RECEIVED_DATA_NOTIFICATION), spReponseData);
    message->sendToTarget();
}

void HTTPSManagerAdapter::onReceivedData(const android::sp<sl::Message>& message) {
    LOG_I(DLT_STRING("onReceivedData!!!"));

    android::sp<ResponseData> spReponseData {nullptr};
    if(message != nullptr) {
        message->getObject(spReponseData);
    }
    uint32_t retCode{HTTP_RES_NOK};
    if(spReponseData != nullptr) {
        retCode = spReponseData->ret;
    }

    LOG_I(DLT_STRING("onReceivedData, retCode = "), DLT_UINT(retCode));
    if (retCode == HTTP_RES_OK) {
        ECallAppHandler::getInstance()->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_MSD_SUCCESS))->sendToTarget();
    }
    else {
        ECallAppHandler::getInstance()->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_MSD_FAILURE))->sendToTarget();
    }
}

std::string HTTPSManagerAdapter::getTelNo() {
    return mMsdData.TelNo;
}

void HTTPSManagerAdapter::setTelNo(std::string telNo) {
    mMsdData.TelNo = telNo;
}

void HTTPSManagerAdapter::clearRspData()
{
    mRspData.respHeader.clear();
    mRspData.respBody.clear();
}
RespInfo_t *HTTPSManagerAdapter::getRespData()
{
    return &mRspData; // delete info by caller
}

uint32_t HTTPSManagerAdapter::sendRequest(string strURL, string strPayload, HttpData_t& respData, bool needAuthenServer, string strHttpMethod) {
	LOG_I(DLT_STRING("HTTPSManagerAdapter::ECALLHTTPSReq strPayload length : "), DLT_UINT(strPayload.length()));
    CURL *iCurl = NULL;
    CURLcode res = CURLE_FAILED_INIT;
    /* Initialize response data */
    initHttpData(&respData);
    iCurl = curl_easy_init();
    if (!iCurl) {
        LOG_E(DLT_STRING("Failed to initialize cURL"));
        curl_easy_cleanup(iCurl);
        return res;
    }
    curl_slist *curlList = NULL;
    res = curl_easy_setopt(iCurl, CURLOPT_URL, strURL.c_str());
    if (needAuthenServer) {
        res = curl_easy_setopt(iCurl, CURLOPT_CAPATH, "/data/remoteif/");
        res = curl_easy_setopt(iCurl, CURLOPT_SSL_SESSIONID_CACHE, 0L);
        res = curl_easy_setopt(iCurl, CURLOPT_SSL_VERIFYPEER, 1L);
        res = curl_easy_setopt(iCurl, CURLOPT_SSL_VERIFYHOST, 1L);
        curlList = curl_slist_append(curlList, "method: POST");
        curlList = curl_slist_append(curlList, "scheme: https");
        curlList = curl_slist_append(curlList, "path: /v1/emergency-notification");
        curlList = curl_slist_append(curlList, "authority: example-api.example.com");
        curlList = curl_slist_append(curlList, "accept: application/json");
        curlList = curl_slist_append(curlList, "content-type: application/json");
        curlList = curl_slist_append(curlList, "user-agent: EmergencyNotificationClient/1.0");
    }
    else {
        res = curl_easy_setopt(iCurl, CURLOPT_SSL_VERIFYPEER, 0L);      //For test without certificate verification
        res = curl_easy_setopt(iCurl, CURLOPT_SSL_VERIFYHOST, 0L);      //For test without certificate verification
        res = curl_easy_setopt(iCurl, CURLOPT_TIMEOUT, HTTPS_TIMEOUT);  //timeout 30sec
        res = curl_easy_setopt(iCurl, CURLOPT_VERBOSE, 1L);

        curlList = curl_slist_append(curlList, "Content-Type: application/json");
    }
    std::ostringstream oss;
    oss << "Content-Length: " << strPayload.length();
    std::string len_hdr = oss.str();
    // list = curl_slist_append(list, "Content-Length: 15000");
    curlList = curl_slist_append(curlList, len_hdr.c_str());
    res = curl_easy_setopt(iCurl, CURLOPT_HTTPHEADER, curlList);

    //body
    res = curl_easy_setopt(iCurl, CURLOPT_POSTFIELDS, strPayload.c_str());
    // curl_easy_setopt(iCurl, CURLOPT_POSTFIELDSIZE, (long)strlen(strPayload.c_str()));
    res = curl_easy_setopt(iCurl, CURLOPT_CUSTOMREQUEST, strHttpMethod.c_str());

    // callback function
    res = curl_easy_setopt(iCurl, CURLOPT_HEADERFUNCTION, headerFunc);
    res = curl_easy_setopt(iCurl, CURLOPT_HEADERDATA, &respData);
    res = curl_easy_setopt(iCurl, CURLOPT_WRITEFUNCTION, writeFunc);
    res = curl_easy_setopt(iCurl, CURLOPT_WRITEDATA, &respData);
    CURLcode iResultCode = CURLE_FAILED_INIT;
    try {
        iResultCode = curl_easy_perform(iCurl);
    }
    catch (...) {
        LOG_I(DLT_STRING("error"));
    }
    uint32_t retCode = HTTP_RES_NOK;
    if (iResultCode != CURLE_OK)
    {
        LOG_I(DLT_STRING("HTTPSAdapter::ECALLHTTPSReq - not ok ["), DLT_STRING(curl_easy_strerror(iResultCode)), DLT_STRING("]:["), DLT_UINT(static_cast<uint32_t>(iResultCode)), DLT_STRING("]"));
        retCode = HTTP_RES_NOK;
#ifdef REGION_PCB26
        setSecurityEvent(iResultCode);
#endif
    } else
    {
        LOG_D(DLT_STRING("HTTPSAdapter::ECALLHTTPSReq - Response Header ["), DLT_STRING(respData.respHeader.c_str()), DLT_STRING("]"));
        LOG_D(DLT_STRING("HTTPSAdapter::ECALLHTTPSReq - Response Body ["), DLT_STRING(respData.respBody.c_str()),DLT_STRING("]"));
        std::map<std::string, std::string>::const_iterator itt;
        for (itt = respData.mRespHeader.begin(); itt != respData.mRespHeader.end(); itt++)
        {
            LOG_I(DLT_STRING("HTTPSAdapter::ECALLHTTPSReq - Map Response Header ["), DLT_STRING(itt->first.c_str()),DLT_STRING("]:["), DLT_STRING(itt->second.c_str()),DLT_STRING("]"));
        }

        long respCode;
        curl_easy_getinfo(iCurl, CURLINFO_RESPONSE_CODE, &respCode);

        if (respCode != HTTPS_RESP_OK)
        {
            LOG_E(DLT_STRING("HTTPSAdapter::ECALLHTTPSReq - Response Code is "), DLT_INT64(respCode));
            retCode = HTTP_RES_NOK;
        } else
        {
            LOG_I(DLT_STRING("HTTPSAdapter::ECALLHTTPSReq - Response Code is 200"));
            retCode = HTTP_RES_OK;
        }
    }

    curl_slist_free_all(curlList);
    curl_easy_cleanup(iCurl);

    return retCode;
}

size_t HTTPSManagerAdapter::writeFunc(void *ptr, size_t size, size_t nmemb, HttpData_t *httpData) {
    size_t oldLen = httpData->respBody.length();
    size_t newLen = oldLen + size * nmemb; // add more size * nmemb element to httpData when receiving each chunk of data
    try {
        httpData->respBody.resize(newLen); // re-allocate httpData to new bigger size
    }
    catch (...) {
        LOG_E(DLT_STRING("memmory erro"));
    }
    newLen = httpData->respBody.length();
    if (newLen >= oldLen) {
        try {
            memcpy(&httpData->respBody[oldLen], ptr, newLen - oldLen);  // coppy data from ptr to respHeader[oldLen, ..., newLen]
        }
        catch (...) {
            LOG_E(DLT_STRING("memmory erro"));
        }
    }
    return size * nmemb;
}

size_t HTTPSManagerAdapter::headerFunc(void *ptr, size_t size, size_t nmemb, HttpData_t *httpData) {
    size_t oldLen = httpData->respHeader.length();
    size_t newLen = httpData->respHeader.length() + size * nmemb; // add more size * nmemb element to httpData when receiving each chunk of data
    try {
        httpData->respHeader.resize(newLen); // re-allocate httpData to new bigger size
    }
    catch (...) {
        LOG_E(DLT_STRING("memmory erro"));
    }
    newLen = httpData->respHeader.length();
    if (newLen >= oldLen) {
        try {
            memcpy(&httpData->respHeader[oldLen], ptr, newLen - oldLen); // coppy data from ptr to respHeader[oldLen, ..., newLen]
        }
        catch (...) {
            LOG_E(DLT_STRING("memmory erro"));
        }
    }

    //To parsing Header as map (to log the response header only)
    size_t index = 0; // init index at 0
    while (index < newLen) { // loop from 0 to index < newLen - 1
        unsigned char *temp = (unsigned char *)ptr + index; // access ptr at "index"
        if ((temp[0] == '\r') || (temp[0] == '\n')) { // temp[0] char value of ptr at "index"
            break; // found '\r', '\n' in ptr and break
        }
        index++;
    }

    std::string str((unsigned char *)ptr, (unsigned char *)ptr + index); // string of ptr from 0 to index - 1
    size_t pos = str.find(": "); // separat keyword and value
    if (pos != std::string::npos)
    {
        // separat keyword and value 0 to before ": ", and after ": "
        httpData->mRespHeader.insert(std::pair<std::string, std::string>(str.substr(0, pos), str.substr(pos + 2)));
    }

    return size * nmemb;
}

void HTTPSManagerAdapter::initHttpData(HttpData_t *httpData) {
    httpData->respHeader.clear();
    httpData->respBody.clear();
}

#ifdef REGION_PCB26
void HTTPSManagerAdapter::setSecurityEvent(const CURLcode& iResultCode) {
    int32_t securityEventID = 40; //refer HMTSUBEV-1021
    int32_t errorType = 0;
    switch (iResultCode){
        case CURLE_SSL_CONNECT_ERROR:
        case CURLE_SSL_ENGINE_NOTFOUND:
        case CURLE_SSL_ENGINE_SETFAILED:
        case CURLE_SSL_ENGINE_INITFAILED:
        case CURLE_SSL_CERTPROBLEM:
        case CURLE_SSL_CIPHER:
        case CURLE_PEER_FAILED_VERIFICATION:
        case CURLE_SSL_CACERT_BADFILE:
        case CURLE_USE_SSL_FAILED:
        case CURLE_SSL_CRL_BADFILE:
        case CURLE_SSL_ISSUER_ERROR:
        case CURLE_SSL_PINNEDPUBKEYNOTMATCH:
        case CURLE_SSL_INVALIDCERTSTATUS:
        case CURLE_SSL_CLIENTCERT:
            errorType = 1;
            break;
        case CURLE_SSL_SHUTDOWN_FAILED:
        case CURLE_COULDNT_RESOLVE_HOST:
            errorType = 2;
            break;
        default:
            LOG_I(DLT_STRING("Don't set security event, errorCode = "), DLT_UINT(static_cast<uint32_t>(iResultCode)));
            break;
    }
    securityEventID += errorType;
    auto idsManagerService = android::interface_cast<IIdsibinderManagerService>(
            android::defaultServiceManager()-> getService(android::String16(IDSIBINDER_SRV_NAME)));
    if ( (errorType != 0U) && (idsManagerService != nullptr) ) {
        string contextData = "error_type=" + to_string(errorType) + ", software_name=OemCallApp, id="
                            + to_string(getpid()) + ", file_path=" + SERVICE_FILE_PATH;
        size_t contextDataLen = contextData.length()+1U; // add null terminator
        uint8_t contextDataBuffer[contextDataLen] = {'\0'};
        memcpy(contextDataBuffer, contextData.c_str(), contextDataLen-1U); // add null terminator
        error_t ret = idsManagerService->IdsM_SetSecurityEvent(securityEventID, contextDataLen, contextDataBuffer);
        LOG_I(DLT_STRING("IdsM_SetSecurityEvent, ret = "), DLT_INT(static_cast<int32_t>(ret)));
    }
}
#endif
}