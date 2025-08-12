#ifndef COMMONFUNCTIONS_MOCK_H
#define COMMONFUNCTIONS_MOCK_H

class MockCommonFunctions {
public:
MockCommonFunctions() {};
    virtual ~MockCommonFunctions() {};

    MOCK_METHOD2(saveToNVM, error_t(const uint8_t& data, const std::string& name));
    MOCK_METHOD2(readFromNVM, error_t(uint8_t& out, const std::string& name));
    MOCK_METHOD1(removeFromNVM, error_t(const std::string& name));
    MOCK_METHOD0(getUtcTime, std::string());
    MOCK_METHOD1(convertTime, std::string(uint64_t timeStamp));
    MOCK_METHOD1(getConfig, std::string(const std::string& name));
    MOCK_METHOD1(convertUint32ToInt32, int32_t(const uint32_t inValue));
    MOCK_METHOD1(convertInt32ToUint32, uint32_t(const int32_t inValue));
    MOCK_METHOD3(stringToInt32, int32_t(const std::string& str, bool& isValid, const int32_t defaultVal));
    MOCK_METHOD2(uint32ToUint8, uint8_t(const uint32_t& val, const uint8_t defaultVal));
    MOCK_METHOD2(int64ToInt32, int32_t(const int64_t& val, const int32_t defaultVal));
    MOCK_METHOD2(saveTSPMSD, void(const std::string& msdData, bool& hasError));
    MOCK_METHOD0(readTSPMSD, std::string());
    MOCK_METHOD2(stringToJson, nlohmann::json(const std::string& msdData, bool& hasError));
    MOCK_METHOD2(jsonToString, std::string(const nlohmann::json& msdJson, bool& hasError));
    MOCK_METHOD1(printLongString, void(const std::string& message));
};

MockCommonFunctions *M_CommonFunctions;

error_t CommonFunctions::saveToNVM(const uint8_t& data, const std::string& name) {
    return M_CommonFunctions->saveToNVM(data, name);
}

error_t CommonFunctions::readFromNVM(uint8_t& out, const std::string& name) {
    return M_CommonFunctions->readFromNVM(out, name);
}

error_t CommonFunctions::removeFromNVM(const std::string& name) {
    return M_CommonFunctions->removeFromNVM(name);
}

std::string CommonFunctions::getUtcTime() {
    return M_CommonFunctions->getUtcTime();
}

std::string CommonFunctions::convertTime(uint64_t timeStamp) {
    return M_CommonFunctions->convertTime(timeStamp);
}

std::string CommonFunctions::getConfig(const std::string& name) {
    return M_CommonFunctions->getConfig(name);
}

int32_t CommonFunctions::convertUint32ToInt32(const uint32_t inValue) {
    return M_CommonFunctions->convertUint32ToInt32(inValue);
}

uint32_t CommonFunctions::convertInt32ToUint32(const int32_t inValue) {
    return M_CommonFunctions->convertInt32ToUint32(inValue);
}

int32_t CommonFunctions::stringToInt32(const std::string& str, bool& isValid, const int32_t defaultVal) {
    return M_CommonFunctions->stringToInt32(str, isValid, defaultVal);
}

uint8_t CommonFunctions::uint32ToUint8(const uint32_t& val, const uint8_t defaultVal) {
    return M_CommonFunctions->uint32ToUint8(val, defaultVal);
}

int32_t CommonFunctions::int64ToInt32(const int64_t& val, const int32_t defaultVal) {
    return M_CommonFunctions->int64ToInt32(val, defaultVal);
}

void CommonFunctions::saveTSPMSD(const std::string& msdData, bool& hasError) {
    M_CommonFunctions->saveTSPMSD(msdData, hasError);
}

std::string CommonFunctions::readTSPMSD() {
    return M_CommonFunctions->readTSPMSD();
}

nlohmann::json CommonFunctions::stringToJson(const std::string& msdData, bool& hasError) {
    return M_CommonFunctions->stringToJson(msdData, hasError);
}

std::string CommonFunctions::jsonToString(const nlohmann::json& msdJson, bool& hasError) {
    return M_CommonFunctions->jsonToString(msdJson, hasError);
}

void CommonFunctions::printLongString(const std::string& message) {
    M_CommonFunctions->printLongString(message);
}

#endif // COMMONFUNCTIONS_MOCK_H