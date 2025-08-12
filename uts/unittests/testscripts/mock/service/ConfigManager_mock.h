ConfigManager* ConfigManager::mInstance = nullptr;

ConfigManager::ConfigManager()
{
}

ConfigManager::~ConfigManager()
{
}

class MockConfigManager {
public:
    MOCK_METHOD0(instance, ConfigManager*());
    MOCK_METHOD2(getData, ConfigResponse(const DataFrom sourceType, const std::string& name));
};

MockConfigManager * M_ConfigManager;
ConfigManager* ConfigManager::instance()
{
    return M_ConfigManager->instance();
}

ConfigResponse ConfigManager::getData(const DataFrom sourceType, const std::string& name)
{
    return M_ConfigManager->getData(sourceType, name);
}