
class MockConfigurationManagerService {
public:
    MOCK_METHOD3(dbStatusQuery, dbStatus(const DataFrom sourceType, const Query queryReason, const std::string value));    
    MOCK_METHOD2(getData, ProvisioningResponse(const DataFrom sourceType, const std::string& name));
    MOCK_METHOD3(registerReceiver, error_t(const DataFrom sourceType, const std::string& name, android::sp<IConfigurationManagerReceiver>& receiver));
};

MockConfigurationManagerService *M_MockConfigurationManagerService;

class ITestConfigurationManagerService : public BnConfigurationManagerService {
public:
    ITestConfigurationManagerService() {};
    virtual ~ITestConfigurationManagerService() {};

    virtual ProvisioningResponse getData(const DataFrom sourceType, const std::string& name)
    {
        return M_MockConfigurationManagerService->getData(sourceType, name);
    };
    virtual int32_t setData(const DataFrom sourceType, const std::vector<std::pair<std::string, std::string>>& storeData){};
    virtual error_t registerReceiver(const DataFrom sourceType, const std::string& name, android::sp<IConfigurationManagerReceiver>& receiver){
        return M_MockConfigurationManagerService->registerReceiver(sourceType, name, receiver);
    };
    virtual error_t unregisterReceiver(const DataFrom sourceType, const std::string& name, android::sp<IConfigurationManagerReceiver>& receiver)
    {
        return E_ERROR;
    };
    virtual ConfigResult defaultDBSelect(const DataFrom sourceType, const int32_t dbIndex){};
    virtual ConfigResult dbReloadNeeded(const DataFrom sourceType, const Reload reloadReason, const std::string& location){};
    virtual dbStatus dbStatusQuery(const DataFrom sourceType, const Query queryReason, const std::string value)
    {
        return M_MockConfigurationManagerService->dbStatusQuery(sourceType,queryReason,value);
    };
    virtual dbStatus debugCmd(std::string& name, std::string& value){};
    virtual ConfigResult getConfigData(const DataFrom sourceType, const std::string& name, std::string& value){};
    virtual int32_t setConfigData(const DataFrom sourceType, const std::string name, const std::string value){};
    virtual bool isProvisioned(void){};
    
    virtual int32_t read(const std::string &pattern, config::data_t &data) {};
    virtual int32_t read(const std::string &pattern, bool &value) {};
    virtual int32_t read(const std::string &pattern, int64_t &value) {};
    virtual int32_t read(const std::string &pattern, double &value) {};
    virtual int32_t read(const std::string &pattern, std::string &value) {};
    virtual int32_t read(const std::string &pattern, std::vector<bool> &values) {};
    virtual int32_t read(const std::string &pattern, std::vector<int64_t> &values) {};
    virtual int32_t read(const std::string &pattern, std::vector<double> &values) {};
    virtual int32_t read(const std::string &pattern, std::vector<std::string> &values) {};
    virtual int32_t write(const std::string &pattern, const config::data_t &data) {};
    virtual int32_t write(const std::string &pattern, bool value) {};
    virtual int32_t write(const std::string &pattern, int64_t value) {};
    virtual int32_t write(const std::string &pattern, double value) {};
    virtual int32_t write(const std::string &pattern, const std::string &value) {};
    virtual int32_t write(const std::string &pattern, const std::vector<bool> &values) {};
    virtual int32_t write(const std::string &pattern, const std::vector<int64_t> &values) {};
    virtual int32_t write(const std::string &pattern, const std::vector<double> &values) {};
    virtual int32_t write(const std::string &pattern, const std::vector<std::string> &values) {};
    virtual ConfigResponse getJsonData(const DataFrom sourceType, const std::string& name){};
    virtual int32_t erase() {};
    virtual error_t registerDataChangedEvent(const android::sp<IConfigurationManagerReceiver> &receiver) {};
    virtual error_t registerDataChangedEvent(const android::sp<IConfigurationManagerReceiver> &receiver, const std::vector<std::string> &patterns) {};
    virtual error_t unregisterDataChangedEvent(const android::sp<IConfigurationManagerReceiver> &receiver) {};
    virtual error_t unregisterDataChangedEvent(const android::sp<IConfigurationManagerReceiver> &receiver, const std::vector<std::string> &patterns) {};
    virtual error_t registerUserInfoDeletedEvent(const android::sp<IConfigurationManagerReceiver> &receiver) {};
    virtual error_t unregisterUserInfoDeletedEvent(const android::sp<IConfigurationManagerReceiver> &receiver) {};
};




