#ifndef NA_ECALL_RECOVERY_MOCK_H
#define NA_ECALL_RECOVERY_MOCK_H

namespace ecallapp {
class MockNAECallRecovery : public NAECallRecovery {
public:
    MOCK_METHOD0(doECallRecovery, bool());
    MOCK_METHOD0(doRecoveringAtSignalDetectingTimePartOfMSD, void());
    MOCK_METHOD0(doRecoveringTelNo, void());
    MOCK_METHOD0(getPreviousECallState, int32_t());
    MOCK_METHOD0(getPreviousACNFlag, bool());
    MOCK_METHOD0(getPreviousSOSFlag, bool());
    MOCK_METHOD0(getPreviousDESSFlag, bool());
    MOCK_METHOD0(getRecoveredMSDJson, nlohmann::json());
    MOCK_METHOD0(getRecoveredMSDStr, std::string());
    MOCK_METHOD1(doSetConditionToTestRecoverECall, void(uint32_t recoveryCase));
    MOCK_METHOD0(isRestoreMSDSuccessful, bool());
    MOCK_METHOD1(calculateT9RemainingTime, void(std::string timeType));
    MOCK_METHOD1(getRemainingTime, int64_t(std::string timeType));
    MOCK_METHOD0(getCBWTFlag, bool());
    MOCK_METHOD0(resetCBWTFlags, void());
    MOCK_METHOD0(recoverMSD, string());
};

MockNAECallRecovery* M_NAECallRecovery = nullptr;

NAECallRecovery::NAECallRecovery(){};

bool NAECallRecovery::doECallRecovery() {
    return M_NAECallRecovery->doECallRecovery();
}

void NAECallRecovery::doRecoveringAtSignalDetectingTimePartOfMSD() {
    M_NAECallRecovery->doRecoveringAtSignalDetectingTimePartOfMSD();
}

void NAECallRecovery::doRecoveringTelNo() {
    M_NAECallRecovery->doRecoveringTelNo();
}

void NAECallRecovery::doSetConditionToTestRecoverECall(uint32_t recoveryCase) {
    M_NAECallRecovery->doSetConditionToTestRecoverECall(recoveryCase);
}

void NAECallRecovery::calculateT9RemainingTime(std::string timeType) {
    M_NAECallRecovery->calculateT9RemainingTime(timeType);
}
int64_t NAECallRecovery::getRemainingTime(std::string timeType) {
    return M_NAECallRecovery->getRemainingTime(timeType);
}
bool NAECallRecovery::getCBWTFlag() const {
    return M_NAECallRecovery->getCBWTFlag();
}
void NAECallRecovery::resetCBWTFlags() {
    M_NAECallRecovery->resetCBWTFlags();
}
string NAECallRecovery::recoverMSD() {
    return M_NAECallRecovery->recoverMSD();
}

} // namespace ecallapp
#endif // NAE_CALL_RECOVERY_MOCK_H