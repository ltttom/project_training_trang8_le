namespace oc {

class MockDataBuilderUS : public DataBuilderUS {
    public:
        MOCK_METHOD0(build, std::string());
        MOCK_METHOD1(setCDS, IDataBuilder &(bool isNormalCDS));
        MOCK_METHOD0(setGPS, IDataBuilder &());
        MOCK_METHOD1(setUtil, IDataBuilder &(std::string ignition));
};

MockDataBuilderUS *M_DataBuilderUS;

DataBuilderUS::DataBuilderUS()
//    :impl_(new Impl())
{
}

std::string DataBuilderUS::build() {
    return M_DataBuilderUS->build();
}

IDataBuilder & DataBuilderUS::setCDS(bool isNormalCDS) {
    return M_DataBuilderUS->setCDS(isNormalCDS);
}

IDataBuilder & DataBuilderUS::setGPS() {
    return M_DataBuilderUS->setGPS();
}

IDataBuilder & DataBuilderUS::setUtil(std::string ignition) {
    return M_DataBuilderUS->setUtil(ignition);
}

}  // namespace oc

