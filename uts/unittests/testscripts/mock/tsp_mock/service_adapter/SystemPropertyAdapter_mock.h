#ifndef SYSTEMPROPERTYADAPTER_MOCK_H
#define SYSTEMPROPERTYADAPTER_MOCK_H

class MockSystemPropertyAdapter : public SystemPropertyAdapter {
public:
    MOCK_METHOD2(saveDataToProperty, void(const std::string &dataField, const std::string &value));
    MOCK_METHOD2(getDataFromProperty, std::string(const std::string &dataField, std::string defaultValue));
};

MockSystemPropertyAdapter *M_SystemPropertyAdapter;

SystemPropertyAdapter::SystemPropertyAdapter() {}

std::shared_ptr<SystemPropertyAdapter>& SystemPropertyAdapter::getInstance() {
    static std::shared_ptr<SystemPropertyAdapter> sSystemPropertyAdapterInstance(new SystemPropertyAdapter());
    return sSystemPropertyAdapterInstance;
}

void SystemPropertyAdapter::saveDataToProperty(const std::string &dataField, const std::string &value) {
    M_SystemPropertyAdapter->saveDataToProperty(dataField, value);
}

std::string SystemPropertyAdapter::getDataFromProperty(const std::string &dataField, std::string defaultValue) {
    return M_SystemPropertyAdapter->getDataFromProperty(dataField, defaultValue);
}

#endif // SYSTEMPROPERTYADAPTER_MOCK_H