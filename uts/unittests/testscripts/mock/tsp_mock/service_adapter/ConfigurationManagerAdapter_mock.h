#ifndef CONFIGURATION_MANAGER_ADAPTER_MOCK_H
#define CONFIGURATION_MANAGER_ADAPTER_MOCK_H

// using namespace ecallapp;

class MockConfigurationManagerAdapter : public ConfigurationManagerAdapter {
public:
    MOCK_METHOD0(registerService, void());
    MOCK_METHOD2(handleConfigDataChanged, void(const std::string& name, const std::string& value));
    MOCK_CONST_METHOD2(getConfig, std::string(const std::string &name, const DataFrom& sourceType));
    MOCK_METHOD2(setConfig, void(const std::string &name, const std::string &value));
    MOCK_CONST_METHOD1(getConfigIntValue, int32_t(const std::string &name));
    MOCK_METHOD2(setConfig, void(const std::string &name, const int32_t &value));
    MOCK_CONST_METHOD2(getPrivacy, bool(const std::string &funcId, const bool& isACNUS));
    MOCK_CONST_METHOD1(isSubscripted, bool(const std::string& id));
    MOCK_METHOD0(getService, android::sp<IConfigurationManagerService>());
    MOCK_METHOD1(onBinderDied, void(const android::wp<android::IBinder>& who));
};

MockConfigurationManagerAdapter* M_ConfigurationManagerAdapter;

ConfigurationManagerAdapter::ConfigurationManagerAdapter() {}

std::shared_ptr<ConfigurationManagerAdapter> ConfigurationManagerAdapter::instance = nullptr;

ConfigurationManagerAdapter* ConfigurationManagerAdapter::getInstance() 
{
    if (instance == nullptr) {
        instance = std::make_shared<ConfigurationManagerAdapter>();
    }
    return instance.get();
}

void ConfigurationManagerAdapter::registerService() {
    M_ConfigurationManagerAdapter->registerService();
}

void ConfigurationManagerAdapter::handleConfigDataChanged(const std::string& name, const std::string& value) noexcept {
    M_ConfigurationManagerAdapter->handleConfigDataChanged(name, value);
}

std::string ConfigurationManagerAdapter::getConfig(const std::string &name, const DataFrom& sourceType) const noexcept {
    return M_ConfigurationManagerAdapter->getConfig(name, sourceType);
}

void ConfigurationManagerAdapter::setConfig(const std::string &name, const std::string &value) noexcept {
    M_ConfigurationManagerAdapter->setConfig(name, value);
}

int32_t ConfigurationManagerAdapter::getConfigIntValue(const std::string &name) const noexcept {
    return M_ConfigurationManagerAdapter->getConfigIntValue(name);
}

void ConfigurationManagerAdapter::setConfig(const std::string &name, const int32_t &value) noexcept {
    M_ConfigurationManagerAdapter->setConfig(name, value);
}

bool ConfigurationManagerAdapter::getPrivacy(const std::string &funcId, const bool& isACNUS) const {
    return M_ConfigurationManagerAdapter->getPrivacy(funcId, isACNUS);
}

bool ConfigurationManagerAdapter::isSubscripted(const std::string& id) const {
    return M_ConfigurationManagerAdapter->isSubscripted(id);
}

android::sp<IConfigurationManagerService> ConfigurationManagerAdapter::getService() {
    return M_ConfigurationManagerAdapter->getService();
}

void ConfigurationManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who) {
    M_ConfigurationManagerAdapter->onBinderDied(who);
}

class MockConfigReceiver : public ConfigReceiver {
public:
    MockConfigReceiver(ConfigurationManagerAdapter& pr) noexcept : ConfigReceiver(pr) {}

    MOCK_METHOD2(onConfigDataChanged, void(std::string& name, std::string& value));
    // MOCK_METHOD2(onDataChanged, void(const std::string &pattern, const config::data_t &value));
};

MockConfigReceiver* M_ConfigReceiver;

void ConfigReceiver::onConfigDataChanged(std::string& name, std::string& value) {
    M_ConfigReceiver->onConfigDataChanged(name, value);
}

void ConfigReceiver::onDataChanged(const std::string &pattern, const config::data_t &value) noexcept {
    // M_ConfigReceiver->onDataChanged(pattern, value);
}


#endif // CONFIGURATION_MANAGER_ADAPTER_MOCK_H