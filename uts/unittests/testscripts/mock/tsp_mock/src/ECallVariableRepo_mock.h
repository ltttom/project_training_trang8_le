#ifndef _ECALL_VARIABLE_REPO_MOCK_H_
#define _ECALL_VARIABLE_REPO_MOCK_H_

namespace ecallapp {

class MockECallVariableRepo : public ECallVariableRepo {
public:
    static ECallVariableRepo& getInstance() {
        static MockECallVariableRepo instance;
        return instance;
    }
    MockECallVariableRepo() : ECallVariableRepo() {}

    MOCK_METHOD1(setECallType, void(int32_t ecallType));
    MOCK_METHOD0(getECallType, int32_t());
    MOCK_METHOD0(updateIgAccCondition, void());
    MOCK_METHOD0(getIgCondition, bool());
    MOCK_METHOD0(getAccCondition, bool());
    MOCK_METHOD0(getIgAccCondition, bool());
    MOCK_METHOD1(setECallClearAudioState, void(int32_t state));
    MOCK_METHOD0(getECallClearAudioState, int32_t());
};

MockECallVariableRepo* M_ECallVariableRepo;

ECallVariableRepo::ECallVariableRepo() {}
ECallVariableRepo& ECallVariableRepo::getInstance() {
    static ECallVariableRepo instance;
    return instance;
}
int32_t ECallVariableRepo::getECallType() const {
    return M_ECallVariableRepo->getECallType();
}
void ECallVariableRepo::setECallType(int32_t ecallType) {
    M_ECallVariableRepo->setECallType(ecallType);
}
void ECallVariableRepo::updateIgAccCondition() {
    M_ECallVariableRepo->updateIgAccCondition();
}
bool ECallVariableRepo::getIgCondition() const {
    return M_ECallVariableRepo->getIgCondition();
}   
bool ECallVariableRepo::getAccCondition() const {
    return M_ECallVariableRepo->getAccCondition();
}
bool ECallVariableRepo::getIgAccCondition() const {
    return M_ECallVariableRepo->getIgAccCondition();
}
void ECallVariableRepo::setECallClearAudioState(int32_t state) {
    M_ECallVariableRepo->setECallClearAudioState(state);
}
int32_t ECallVariableRepo::getECallClearAudioState() const {
    return M_ECallVariableRepo->getECallClearAudioState();
}
} // namespace ecallapp
#endif // _ECALL_VARIABLE_REPO_MOCK_H_