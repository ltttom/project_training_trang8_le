#ifndef ECALLAPP_ECALLSTATE_MOCK_H
#define ECALLAPP_ECALLSTATE_MOCK_H

namespace ecallapp {
class ECallStateMock :public ECallState {
public:
    MOCK_METHOD1(setName, void(const std::string name));
    MOCK_METHOD0(getCurrentState, int32_t());
    MOCK_METHOD0(getPreviousState, int32_t());
    MOCK_METHOD1(changeState, void(int32_t newState));
};

ECallStateMock* M_ECallState;

ECallState::ECallState(){}
void ECallState::setName(const std::string name) {
    M_ECallState->setName(name);
}
int32_t ECallState::getCurrentState () const{
    return M_ECallState->getCurrentState();
}
int32_t ECallState::getPreviousState () const{
    return M_ECallState->getPreviousState();
}
void ECallState::changeState(int32_t newState) {
    M_ECallState->changeState(newState);
}
} // namespace ecallapp
#endif // ECALLAPP_ECALLSTATE_MOCK_H