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
 * @file        CommonFunctions.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
/* Define the USE_LGEFILEIO and add header */
#include "CommonFunctions.h"
#include <sys/stat.h>
#include <spawn.h>
#include <limits>
#include "logger.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <services/TimeManagerService/TimeManager.h>
#include <services/ConfigurationManagerService/ConfigManager.h>
// #include <services/ApplicationManagerService/ApplicationManager.h>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <utils/PosixCmd.h>
#include <fstream>
#include <errno.h>

#define USE_LGEFILEIO // It must be defined before the header file.
#include <lgefileio.h>
#ifdef __UNITTEST__
#define try      if (false)
#define catch(X) if (false)
#define __throw_exception_again
#define __throw
#undef LOG_I
#define LOG_I(...)
#undef LOG_E
#define LOG_E(...)
#undef LOG_W
#define LOG_W(...)
#define USE_DEFAULT_PREPATH ""
#define USE_DEFAULT_PREPATH_1 "../build/"
#else
#define USE_DEFAULT_PREPATH "/data/systemrw/oemcall/"
#endif

#define MIN_UINT8 0U
#define MAX_UINT8 255U
#define MIN_INT32 -2147483648
#define MAX_INT32 2147483647

namespace ecallapp {

void CommonFunctions::createDefaultPathIfNeeded() {
    /**
    * Create a directory USE_DEFAULT_PREPATH if it does not exist
    * */
    int isMakeDirectoryOK = mkdir(USE_DEFAULT_PREPATH, 0755);
    if(isMakeDirectoryOK == -1) { // Fail to make a new dir
        if(errno == EEXIST) {
            LOG_I(DLT_STRING("The directory has been already created:"), DLT_STRING(USE_DEFAULT_PREPATH));
        }
        else {
            LOG_I(DLT_STRING("Failed to create the directory:"), DLT_STRING(USE_DEFAULT_PREPATH));
        }
    }
    else {
        LOG_I(DLT_STRING("Default directory create succesfully:"), DLT_STRING(USE_DEFAULT_PREPATH));
    }
};


error_t CommonFunctions::saveToNVM(const uint8_t &data, const std::string &name) {
    error_t res = E_ERROR;
    std::string filePath = std::string(USE_DEFAULT_PREPATH) + name;
    std::ofstream outFile(filePath.c_str());
    if(outFile.is_open()) {
        outFile << data;
        outFile.flush();
        outFile.close();
        int32_t fd = open(filePath.c_str(), O_WRONLY);
        if(fd != -1) { // open file successfully
            (void) fsync(fd);
            close(fd);
            res = E_OK;
            LOG_I(DLT_STRING("Save data success to"), DLT_STRING(filePath.c_str()));
        }
        else {
            LOG_E(DLT_STRING("Failed to open file descriptor for"), DLT_STRING(filePath.c_str()));
        }
    }
    else {
        LOG_E(DLT_STRING("Cannot save"), DLT_STRING(name.c_str()), DLT_STRING("to NVM"));
    }
    return res;
}

error_t CommonFunctions::readFromNVM(uint8_t& out, const std::string &name) {
    error_t res = E_ERROR;
    std::string filePath = std::string(USE_DEFAULT_PREPATH) + name;
    std::ifstream inFile(filePath.c_str());
    if(inFile.is_open()) {
        uint8_t value{0U};
        int32_t buff = inFile.get(); //may return -1(EOF)
        if(buff != EOF) {
            value = static_cast<uint8_t>(buff);
            if(value == ECALL_ON) {
                res = E_OK;
                out = value;
                LOG_I(DLT_STRING("Read file "), DLT_STRING(name.c_str()), DLT_STRING(" from NVM success"));
            }
            else {
                LOG_E(DLT_STRING("Invalid value detected while reading data from NVM:"), DLT_STRING(name.c_str()));
            }
        }
        else {
            LOG_E(DLT_STRING("Read data from NVM fail, EOF"));
        }
        inFile.close();
    }
    else {
        LOG_E(DLT_STRING("Cannot open file "), DLT_STRING(filePath.c_str()));
    }
    return res;
}

error_t CommonFunctions::removeFromNVM(const std::string &name) {
    error_t res = E_ERROR;
    std::string filePath = std::string(USE_DEFAULT_PREPATH) + name;
    int32_t fd = open(filePath.c_str(), O_WRONLY | O_TRUNC);
    if(fd != -1) {//https://man7.org/linux/man-pages/man2/open.2.html - On success, open(), openat(), and creat() return the new file descriptor (a nonnegative integer).  On error, -1 is returned anderrno is set to indicate the error.
        (void) fsync(fd);
        close(fd);
    }
    else {
        LOG_E(DLT_STRING("Failed to open file descriptor for"), DLT_STRING(filePath.c_str()));
    }
    res = std::remove(filePath.c_str());
    if (res != E_OK) {
        LOG_E(DLT_STRING("Can not remove file:"), DLT_STRING(name.c_str()));
    }
    else {
        LOG_I(DLT_STRING("remove file from NVM, "), DLT_STRING(name.c_str()));
    }
    return res;
}

std::string CommonFunctions::getUtcTime() {
    // Get UTC time & Timezone base on RFC3339 standard format: "YYYY-MM-DDTHH:MM:SS+00:00"
    tm now = TimeManager::getInstance().getCurrentTime();
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(4) << (1900 + now.tm_year)
    << "-"
    << std::setfill('0') << std::setw(2) << (1 + now.tm_mon)
    << "-"
    << std::setfill('0') << std::setw(2) << now.tm_mday
    << "T"
    << std::setfill('0') << std::setw(2) << now.tm_hour
    << ":"
    << std::setfill('0') << std::setw(2) << now.tm_min
    << ":"
    << std::setfill('0') << std::setw(2) << now.tm_sec << "+00:00";

    return ss.str();
}

std::string CommonFunctions::convertTime(uint64_t timeStamp) {
    using namespace std::chrono;
    LOG_I(DLT_STRING("TimeStamp: "), DLT_UINT64(timeStamp));

    //        struct gbt_time_t gnssDateTime;
    // convert to time_t
    const auto durationSinceEpoch = std::chrono::milliseconds(timeStamp);
    const time_point<system_clock> tp_after_duration(durationSinceEpoch);
    time_t time_after_duration = system_clock::to_time_t(tp_after_duration);

    std::tm* timeData = std::gmtime(&time_after_duration);

    std::ostringstream sstr;
    if(timeData == nullptr || timeStamp == 0) {
        std::string dtTime = "1900-01-01T00:00:00+00:00";
        LOG_I(DLT_STRING(dtTime.c_str()));
        return dtTime;
    } else {
        sstr << std::setfill('0') << std::setw(4) << timeData->tm_year + 1900
             << "-"
             << std::setfill('0') << std::setw(2) << timeData->tm_mon + 1
             << "-"
             << std::setfill('0') << std::setw(2) << timeData->tm_mday
             << "T"
             << std::setfill('0') << std::setw(2) << timeData->tm_hour
             << ":"
             << std::setfill('0') << std::setw(2) << timeData->tm_min
             << ":"
             << std::setfill('0') << std::setw(2) << timeData->tm_sec
             << "+00:00";
        LOG_I(DLT_STRING(sstr.str().c_str()));
        return sstr.str(); // TODO: get time
    }


}

std::string CommonFunctions::getConfig(const std::string &name) {
    bool isError = false;
    std::string res = "";
    ConfigResponse response;

    try {
        response = ConfigManager::instance()->getData(DataFrom::General, name);
    } catch (...) {
        LOG_E(DLT_STRING("throw exception getConfig"));
        isError = true;
    }

    if (!isError) {
        if (response.result == ConfigResult::NoError) {
            LOG_E(DLT_STRING("ConfigResult::NoError"));
            res = response.read();
        }
        else {
            isError = true;
        }
    }

    if (isError) {
        LOG_E(DLT_STRING("Get Config Error"));
    }
    return res;
}

uint32_t CommonFunctions::convertInt32ToUint32(const int32_t inValue) {
    uint32_t outValue{0U};
    if(inValue >= 0) {
        outValue = static_cast<uint32_t>(inValue);
    }
    return outValue;
}

int32_t CommonFunctions::convertUint32ToInt32(const uint32_t inValue) {
    int32_t outValue{0};
    if(inValue <= 2147483647U) {//2147483647U int max value
        outValue = static_cast<int32_t>(inValue);
    }
    else {
        LOG_E(DLT_STRING("out of range"));
    }
    return outValue;
}

uint8_t CommonFunctions::uint32ToUint8(const uint32_t& val, const uint8_t defaultVal) {
    uint8_t outValue {defaultVal};
    if (val <= MAX_UINT8) {
        outValue = (static_cast<uint8_t>(val));
    }
    return outValue;
}

int32_t CommonFunctions::int64ToInt32(const int64_t& val, const int32_t defaultVal) {
    int32_t outValue {defaultVal};
    if ((val >= MIN_INT32) && (val <= MAX_INT32)) {
        outValue = (static_cast<int32_t>(val));
    }
    return outValue;
}

int32_t CommonFunctions::stringToInt32(const std::string& str, bool& isValid, const int32_t defaultVal) {
    int32_t res {defaultVal};
    isValid = true;
    try {
        res = std::stoi(str);
    }
    catch(...) {
        res = defaultVal;
        isValid = false;
    }
    LOG_D(DLT_STRING("Input string: "),
        DLT_STRING(str.c_str()),
        DLT_STRING(", defaultVal: "),
        DLT_INT32(defaultVal),
        DLT_STRING(", checkValid: "),
        DLT_BOOL(isValid));
    return res;
}

int64_t CommonFunctions::stringToInt64(const std::string& str, bool& isValid, const int64_t defaultVal) {
    int64_t res {defaultVal};
    isValid = true;
    try {
        res = std::stoll(str);
    }
    catch(...) {
        res = defaultVal;
        isValid = false;
    }
    LOG_D(DLT_STRING("Input string: "),
        DLT_STRING(str.c_str()),
        DLT_STRING(", defaultVal: "),
        DLT_INT64(defaultVal),
        DLT_STRING(", checkValid: "),
        DLT_BOOL(isValid));
    return res;
}

void CommonFunctions::saveTSPMSD(const std::string& msdData, bool& hasError) {
    std::string filePath = std::string(USE_DEFAULT_PREPATH) + "tspmsd.txt";
    std::ofstream outFile(filePath.c_str());
    if(outFile.is_open()) {
        outFile << msdData;
        outFile.flush();
        outFile.close();
        int32_t fd = open(filePath.c_str(), O_WRONLY);
        if(fd != -1) { // create file successfully
            (void) fsync(fd);
            close(fd);
            hasError = false;
            LOG_I(DLT_STRING("SAVE MSD file success"), DLT_STRING(" to "), DLT_STRING(filePath.c_str()));
        }
        else {
            LOG_E(DLT_STRING("Failed to open file descriptor for"), DLT_STRING(filePath.c_str()));
        }
    }
    else {
        hasError = true;
        LOG_E(DLT_STRING("SAVE MSD file fail"));
    }
}

std::string CommonFunctions::readTSPMSD() {
    std::string filePath = std::string(USE_DEFAULT_PREPATH) + "tspmsd.txt";
    std::string res = "";
    std::ifstream inFile(filePath);
    if(!inFile){
        LOG_E(DLT_STRING("Read MSD file fail"));
    }
    else{
        std::string line;
        while(std::getline(inFile, line)){
            res += line + '\n';
        }
        inFile.close();
        LOG_I(DLT_STRING("Read MSD file success, path:"), DLT_STRING(filePath.c_str()));
    }
    return res;
}
nlohmann::json CommonFunctions::stringToJson(const std::string& msdData, bool& hasError) {
    nlohmann::json res;
    try{
        res = nlohmann::json::parse(msdData);
        hasError = false;
    }
    catch(...){
        LOG_I(DLT_STRING("An error occurred during JSON parsing."));
        hasError = true;
    }
    return res;
}
std::string CommonFunctions::jsonToString(const nlohmann::json& msdJson, bool& hasError){
    std::string res = "";
    try{
        res = msdJson.dump(4); //Each level of indentation uses 4 spaces as a rule for readability and clarity.
        hasError = false;
    }
    catch(...){
        LOG_I(DLT_STRING("An error occurred during JSON dump."));
        hasError = true;
    }
    return res;
}
void CommonFunctions::printLongString(const std::string& message){
    std::string mess;
    uint32_t index = 0U;
    while((index + 1000U) < message.length()){
        try{
            mess = message.substr(index, 1000U);
            LOG_I(DLT_STRING(mess.c_str()));
            index += 1000U;
        }
        catch(...){
            LOG_I(DLT_STRING("Error while using substr"));
        }
    }
    try{
        mess = message.substr(index, message.length());
        LOG_I(DLT_STRING(mess.c_str()));
    }
    catch(...){
        LOG_I(DLT_STRING("Error while using substr"));
    }
}
}
