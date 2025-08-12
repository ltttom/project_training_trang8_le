#ifndef __DATA_BUIDER_RECALL_H__
#define __DATA_BUIDER_RECALL_H__
namespace oc {

class MockDataBuilderRCall : public DataBuilderRCall {
    public:
        MOCK_METHOD0(build, std::string());
        MOCK_METHOD1(setCDS, IDataBuilder &(bool isNormalCDS));
        MOCK_METHOD0(setGPS, IDataBuilder &());
        MOCK_METHOD1(setUtil, IDataBuilder &(std::string ignition));
};

MockDataBuilderRCall *M_DataBuilderRCall;

DataBuilderRCall::DataBuilderRCall()
    //:impl_(new Impl())
 {
}

std::string DataBuilderRCall::build() {
    return M_DataBuilderRCall->build();
}

IDataBuilder & DataBuilderRCall::setGPS() {
    return M_DataBuilderRCall->setGPS();
}

IDataBuilder & DataBuilderRCall::setUtil(std::string ignition) {
    return M_DataBuilderRCall->setUtil(ignition);
}

}  // namespace oc

#endif