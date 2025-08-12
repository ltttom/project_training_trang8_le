namespace oc {

class MockConfigManager : public ConfigManager {
    public:
        MOCK_METHOD1(update, bool(const uint32_t type));
        MOCK_METHOD1(getConfig, std::string(std::string name));
        MOCK_METHOD1(doHandleConfigChanged, void(const android::sp<ConfigNotiData>& data));
        MOCK_METHOD2(getPrivacy, bool(const string& id, bool isACNUS));
        MOCK_METHOD1(isSubscripted, bool(const string& id));
};

MockConfigManager *M_ConfigManager;

ConfigManager::ConfigManager()
//    :impl_(new Impl()) 
    {
}

bool ConfigManager::update(const uint32_t type) {
    return M_ConfigManager->update(type);
}

std::string ConfigManager::getConfig(std::string name) {
    return M_ConfigManager->getConfig(name);
}

void ConfigManager::doHandleConfigChanged(const android::sp<ConfigNotiData>& data) {
    return M_ConfigManager->doHandleConfigChanged(data);
}

bool ConfigManager::getPrivacy(const string& id, bool isACNUS) {
    return M_ConfigManager->getPrivacy(id, isACNUS);
}

bool ConfigManager::isSubscripted(const string& id) {
    return M_ConfigManager->isSubscripted(id);
}

}  // namespace oc

