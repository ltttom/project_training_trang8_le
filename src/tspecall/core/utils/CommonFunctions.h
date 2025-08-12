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
 * @file        CommonFunctions.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef UTILS_COMMON_H
#define UTILS_COMMON_H
#include <servicelayer/Error.h>
#include <string>
#include <climits>
#include <unordered_map>
#include <services/SomeipProviderManagerService/SomeipProviderManager.h>
#include <json-for-modern-cpp/json.hpp>
#include "ParamsDef.h"

namespace ecallapp{
    class CommonFunctions {
    public:
        CommonFunctions(){};
        virtual ~CommonFunctions(){};
        static error_t saveToNVM(const uint8_t& data, const std::string& name);
        static error_t readFromNVM(uint8_t& out, const std::string& name);
        static error_t removeFromNVM(const std::string& name);
        static std::string getUtcTime();
        static std::string convertTime(uint64_t timeStamp);
        static std::string getConfig(const std::string& name);
        static int32_t convertUint32ToInt32(const uint32_t inValue);
        static uint32_t convertInt32ToUint32(const int32_t inValue);
        /* Convert string to int32 */
        static int32_t stringToInt32(const std::string& str, bool& isValid, const int32_t defaultVal = 0);
        static int64_t stringToInt64(const std::string& str, bool& isValid, const int64_t defaultVal = 0);
        static uint8_t uint32ToUint8(const uint32_t& val, const uint8_t defaultVal = 0U);
        static int32_t int64ToInt32(const int64_t& val, const int32_t defaultVal = 0);

        static void saveTSPMSD(const std::string& msdData, bool& hasError);
        static std::string readTSPMSD();
        static nlohmann::json stringToJson(const std::string& msdData, bool& hasError);
        static std::string jsonToString(const nlohmann::json& msdJson, bool& hasError);
        static void printLongString(const std::string& message);
        static void createDefaultPathIfNeeded();

        /**
         * @brief convert enum to string for debugging purpose
         * @param enumClassContainer a look up table
         * @param key given key
         * @return corresponding name, otherwise "Unsupported key: <id>"
         */
        template <typename T>
        static std::string enumToString(const std::unordered_map<T, std::string>& enumClassContainer, T key)
        {
            auto it = enumClassContainer.find(key);
            return it != enumClassContainer.end() ?
                            it->second :
                            "Unsupported key: " + std::to_string(static_cast<int32_t>(key));
        }
    };
}

#endif // UTILS_COMMON_H
