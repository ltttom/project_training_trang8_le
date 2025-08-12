
/**
 * @copyright
 * Copyright (c) 2025 by LG Electronics Inc.
 * This program or software including the accompanying associated documentation
 * ("Software") is the proprietary software of LG Electronics Inc. and or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate written license agreement between you
 * and LG Electronics Inc. ("Authorized License"). Except as set forth in an
 * Authorized License, LG Electronics Inc. grants no license (express or implied),
 * rights to use, or waiver of any kind with respect to the Software, and LG
 * Electronics Inc. expressly reserves all rights in and to the Software and all
 * intellectual property therein. If you have no Authorized License, then you have
 * no rights to use the Software in any ways, and should immediately notify LG
 * Electronics Inc. and discontinue all use of the Software.
 * @file        NAECallStates.h
 * @author      eunbi.lee@lge.com
 * @version     1.0.00
*/
#ifndef FLAG_NA_ECALL_STATES
#define FLAG_NA_ECALL_STATES
#include <cstdint>
// Forward declaration to avoid circular dependency
namespace ecallapp {
    class NAECallStateMachine;
}
namespace ecallapp {
/**
 * @brief Abstract base class for all eCall states
 *
 * This class defines the interface for all concrete state classes.
 * Each state implements its own behavior for handling events.
 */
class ECallStateBase {
public:
    virtual ~ECallStateBase() = default;
    /**
     * @brief Called when entering this state
     *
     * @param context Reference to the state machine context
     */
    virtual void enter(NAECallStateMachine* context) {}
    /**
     * @brief Called when exiting this state
     *
     * @param context Reference to the state machine context
     */
    virtual void exit(NAECallStateMachine* context) {}
    /**
     * @brief Handle an event in this state
     *
     * @param context Reference to the state machine context
     * @param event Event to handle
     * @param eventParam1 First event parameter
     * @param eventParam2 Second event parameter
     */
    virtual void process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1 = 0, const uint32_t eventParam2 = 0) = 0;
};
/**
 * @brief Undefined state implementation
 */
class BootupState : public ECallStateBase {
public:
    void enter(NAECallStateMachine* context) override;
    void exit(NAECallStateMachine* context) override;
    void process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1 = 0, const uint32_t eventParam2 = 0) override;
};
/**
 * @brief Recovery state implementation
 */
class RecoveryState : public ECallStateBase {
public:
    void enter(NAECallStateMachine* context) override;
    void exit(NAECallStateMachine* context) override;
    void process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1 = 0, const uint32_t eventParam2 = 0) override;
private:
    uint32_t mRecoveryECallState = 0;
};

/**
 * @brief Power Off state implementation
 */
class PowerOffState : public ECallStateBase {
public:
    void enter(NAECallStateMachine* context) override;
    void exit(NAECallStateMachine* context) override;
    void process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1 = 0, const uint32_t eventParam2 = 0) override;
};
/**
 * @brief Standby state implementation
 */
class StandbyState : public ECallStateBase {
public:
    void enter(NAECallStateMachine* context) override;
    void exit(NAECallStateMachine* context) override;
    void process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1 = 0, const uint32_t eventParam2 = 0) override;
};
/**
 * @brief Cancel Period state implementation
 */
class CancelPeriodState : public ECallStateBase {
public:
    void enter(NAECallStateMachine* context) override;
    void exit(NAECallStateMachine* context) override;
    void process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1 = 0, const uint32_t eventParam2 = 0) override;
};
/**
 * @brief MSD Preparing state implementation
 */
class MsdPreparingState : public ECallStateBase {
public:
    void enter(NAECallStateMachine* context) override;
    void exit(NAECallStateMachine* context) override;
    void process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1 = 0, const uint32_t eventParam2 = 0) override;
};
/**
 * @brief MSD Transmission state implementation
 */
class MsdTransmissionState : public ECallStateBase {
public:
    void enter(NAECallStateMachine* context) override;
    void exit(NAECallStateMachine* context) override;
    void process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1 = 0, const uint32_t eventParam2 = 0) override;
};
/**
 * @brief Clear Current Call state implementation
 */
class ClearCurrentCallState : public ECallStateBase {
public:
    void enter(NAECallStateMachine* context) override;
    void exit(NAECallStateMachine* context) override;
    void process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1 = 0, const uint32_t eventParam2 = 0) override;
};
/**
 * @brief Call Connecting state implementation
 */
class CallConnectingState : public ECallStateBase {
public:
    void enter(NAECallStateMachine* context) override;
    void exit(NAECallStateMachine* context) override;
    void process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1 = 0, const uint32_t eventParam2 = 0) override;
};
/**
 * @brief Call Connected state implementation
 */
class CallConnectedState : public ECallStateBase {
public:
    void enter(NAECallStateMachine* context) override;
    void exit(NAECallStateMachine* context) override;
    void process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1 = 0, const uint32_t eventParam2 = 0) override;
};
/**
 * @brief Callback Waiting state implementation
 */
class CallbackWaitingState : public ECallStateBase {
public:
    void enter(NAECallStateMachine* context) override;
    void exit(NAECallStateMachine* context) override;
    void process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1 = 0, const uint32_t eventParam2 = 0) override;
private:
    int32_t eCallFeature = -1;
};
/**
 * @brief Callback Running state implementation
 */
class CallbackRunningState : public ECallStateBase {
public:
    void enter(NAECallStateMachine* context) override;
    void exit(NAECallStateMachine* context) override;
    void process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1 = 0, const uint32_t eventParam2 = 0) override;
private:
    bool isCallbackModeExpired = false;     // To check callback mode time is expired during callback running
};
} // namespace ecallapp
#endif // FLAG_NA_ECALL_STATES
