#ifndef DATABUILDERUS_MOCK_H
#define DATABUILDERUS_MOCK_H

class MockDataBuilderUS : public DataBuilderUS {
public:
    MockDataBuilderUS() {}
    virtual ~MockDataBuilderUS() {}
    MOCK_METHOD0(build, std::string());
    MOCK_METHOD1(setCDS, IDataBuilder&(bool isNormalCDS));
    MOCK_METHOD0(setGPS, IDataBuilder&());
    MOCK_METHOD1(setUtil, IDataBuilder&(std::string ignition));
    MOCK_METHOD2(setTriggerType, IDataBuilder&(std::string triggerType, bool isDessCall));
    MOCK_METHOD2(getIgState, std::string(int32_t igState, int32_t accState));
};

MockDataBuilderUS *M_DataBuilderUS;

DataBuilderUS::DataBuilderUS(){}

std::string DataBuilderUS::build() {
    return M_DataBuilderUS->build();
}

IDataBuilder& DataBuilderUS::setCDS(bool isNormalCDS) {
    return M_DataBuilderUS->setCDS(isNormalCDS);
}

IDataBuilder& DataBuilderUS::setGPS() {
    return M_DataBuilderUS->setGPS();
}

IDataBuilder& DataBuilderUS::setUtil(std::string ignition) {
    return M_DataBuilderUS->setUtil(ignition);
}

IDataBuilder& DataBuilderUS::setTriggerType(std::string triggerType, bool isDessCall) {
    return M_DataBuilderUS->setTriggerType(triggerType, isDessCall);
}

std::string DataBuilderUS::getIgState(int32_t igState, int32_t accState) {
    return M_DataBuilderUS->getIgState(igState, accState);
}

#endif // DATABUILDERUS_MOCK_H