namespace oc {

class MockIDataBuilder : public IDataBuilder {
    public:
        MOCK_METHOD0(build, std::string());
        MOCK_METHOD0(setGPS, IDataBuilder &());
        MOCK_METHOD1(setCDS, IDataBuilder &(bool isNormalCDS));
        MOCK_METHOD1(setUtil, IDataBuilder &(std::string ignition));
};

MockIDataBuilder *M_IDataBuilder;

class ITest_DataBuilder : public IDataBuilder {
    ~ITest_DataBuilder() {}
    std::string build() {
        return M_IDataBuilder->build();
    }

    IDataBuilder & setGPS() {
        return M_IDataBuilder->setGPS();
    }

    IDataBuilder & setCDS(bool isNormalCDS) {
        return M_IDataBuilder->setCDS(isNormalCDS);
    }

    IDataBuilder & setUtil(std::string ignition) {
        return M_IDataBuilder->setUtil(ignition);
    }

};

}  // namespace oc


