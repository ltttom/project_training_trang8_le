namespace oc {
namespace cv {

class MockCommon : public Common {
    public:
        MOCK_METHOD2(saveToNVM, error_t(const uint8_t& data, const std::string& name));
        MOCK_METHOD2(readFromNVM, error_t(uint8_t& out, const std::string& name));
        MOCK_METHOD1(createNewDirectory, error_t(const std::string& path));
        MOCK_METHOD1(removeFromNVM, error_t(const std::string& name));
        MOCK_METHOD0(getUtcTime, std::string());
        MOCK_METHOD1(convertTime, std::string(uint64_t timeStamp));
        MOCK_METHOD1(getConfig, std::string(const std::string& name));
        MOCK_METHOD1(trimWhiteSpace, std::string(std::string s));
        MOCK_METHOD1(isFileExist, bool(const std::string& path));
        // MOCK_METHOD1(isAvailableInRegion, bool(const std::string feature));
        // MOCK_METHOD2(sendToICB, error_t(EnumSomeipTxMsgID id, uint8_t data));
};

MockCommon *M_Common;

error_t Common::saveToNVM(const uint8_t& data, const std::string& name) {
    return M_Common->saveToNVM(data, name);
}

error_t Common::readFromNVM(uint8_t& out, const std::string& name) {
    return M_Common->readFromNVM(out, name);
}

error_t Common::createNewDirectory(const std::string& path) {
    return M_Common->createNewDirectory(path);
}

error_t Common::removeFromNVM(const std::string& name) {
    return M_Common->removeFromNVM(name);
}

std::string Common::getUtcTime() {
    return M_Common->getUtcTime();
}

std::string Common::convertTime(uint64_t timeStamp) {
    return M_Common->convertTime(timeStamp);
}

std::string Common::getConfig(const std::string& name) {
    return M_Common->getConfig(name);
}

std::string Common::trimWhiteSpace(std::string s) {
    return M_Common->trimWhiteSpace(s);
}

bool Common::isFileExist(const std::string& path) {
    return M_Common->isFileExist(path);
}

// bool Common::isAvailableInRegion(const std::string feature) {
//     return M_Common->isAvailableInRegion(feature);
// }

// error_t Common::sendToICB(EnumSomeipTxMsgID id, uint8_t data) {
//     return M_Common->sendToICB(id, data);
// }

}  // namespace cv
}  // namespace oc

