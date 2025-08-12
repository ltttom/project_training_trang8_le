#ifndef NAE_CALL_STATES_MOCK_H
#define NAE_CALL_STATES_MOCK_H

namespace ecallapp {

class MockECallStateBase : public ECallStateBase {
public:
    MockECallStateBase() : ECallStateBase() {}
    virtual ~MockECallStateBase() = default;

    MOCK_METHOD1(enter, void(NAECallStateMachine* context));
    MOCK_METHOD1(exit, void(NAECallStateMachine* context));
    MOCK_METHOD4(handleEvent, void(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2));
};

MockECallStateBase* M_ECallStateBase;

void BootupState::enter(NAECallStateMachine* context) {
    M_ECallStateBase->enter(context);
}
void BootupState::exit(NAECallStateMachine* context) {
    M_ECallStateBase->exit(context);
}
void BootupState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    M_ECallStateBase->handleEvent(context, event, eventParam1, eventParam2);
}

void RecoveryState::enter(NAECallStateMachine* context) {
    M_ECallStateBase->enter(context);
}
void RecoveryState::exit(NAECallStateMachine* context) {
    M_ECallStateBase->exit(context);
}
void RecoveryState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    M_ECallStateBase->handleEvent(context, event, eventParam1, eventParam2);
}

void StandbyState::enter(NAECallStateMachine* context) {
    M_ECallStateBase->enter(context);
}
void StandbyState::exit(NAECallStateMachine* context) {
    M_ECallStateBase->exit(context);
}
void StandbyState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    M_ECallStateBase->handleEvent(context, event, eventParam1, eventParam2);
}

void CancelPeriodState::enter(NAECallStateMachine* context) {
    M_ECallStateBase->enter(context);
}
void CancelPeriodState::exit(NAECallStateMachine* context) {
    M_ECallStateBase->exit(context);
}
void CancelPeriodState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    M_ECallStateBase->handleEvent(context, event, eventParam1, eventParam2);
}

void MsdPreparingState::enter(NAECallStateMachine* context) {
    M_ECallStateBase->enter(context);
}
void MsdPreparingState::exit(NAECallStateMachine* context) {
    M_ECallStateBase->exit(context);
}
void MsdPreparingState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    M_ECallStateBase->handleEvent(context, event, eventParam1, eventParam2);
}

void MsdTransmissionState::enter(NAECallStateMachine* context) {
    M_ECallStateBase->enter(context);
}
void MsdTransmissionState::exit(NAECallStateMachine* context) {
    M_ECallStateBase->exit(context);
}
void MsdTransmissionState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    M_ECallStateBase->handleEvent(context, event, eventParam1, eventParam2);
}

void CallConnectingState::enter(NAECallStateMachine* context) {
    M_ECallStateBase->enter(context);
}

void CallConnectingState::exit(NAECallStateMachine* context) {
    M_ECallStateBase->exit(context);
}

void CallConnectingState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    M_ECallStateBase->handleEvent(context, event, eventParam1, eventParam2);
}

void CallConnectedState::enter(NAECallStateMachine* context) {
    M_ECallStateBase->enter(context);
}
void CallConnectedState::exit(NAECallStateMachine* context) {
    M_ECallStateBase->exit(context);
}
void CallConnectedState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    M_ECallStateBase->handleEvent(context, event, eventParam1, eventParam2);
}

void ClearCurrentCallState::enter(NAECallStateMachine* context) {
    M_ECallStateBase->enter(context);
}
void ClearCurrentCallState::exit(NAECallStateMachine* context) {
    M_ECallStateBase->exit(context);
}
void ClearCurrentCallState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    M_ECallStateBase->handleEvent(context, event, eventParam1, eventParam2);
}

void PowerOffState::enter(NAECallStateMachine* context) {
    M_ECallStateBase->enter(context);
}
void PowerOffState::exit(NAECallStateMachine* context) {
    M_ECallStateBase->exit(context);
}
void PowerOffState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    M_ECallStateBase->handleEvent(context, event, eventParam1, eventParam2);
}

void CallbackWaitingState::enter(NAECallStateMachine* context) {
    M_ECallStateBase->enter(context);
}
void CallbackWaitingState::exit(NAECallStateMachine* context) {
    M_ECallStateBase->exit(context);
}
void CallbackWaitingState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    M_ECallStateBase->handleEvent(context, event, eventParam1, eventParam2);
}

void CallbackRunningState::enter(NAECallStateMachine* context) {
    M_ECallStateBase->enter(context);
}
void CallbackRunningState::exit(NAECallStateMachine* context) {
    M_ECallStateBase->exit(context);
}
void CallbackRunningState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    M_ECallStateBase->handleEvent(context, event, eventParam1, eventParam2);
}

} // namespace ecallapp
#endif // NAE_CALL_STATES_MOCK_H