namespace oc {

class MockTelephoneManager :  TelephoneManager {
    public:
        MockTelephoneManager() : TelephoneManager(nullptr) {}
        MOCK_METHOD2(dial, void(const std::string& number, bool isEcall));
        MOCK_METHOD0(endCall, void());
        MOCK_METHOD0(answer, void());
        MOCK_METHOD0(stop, void());
        MOCK_METHOD0(rejectCall, void());
        MOCK_METHOD0(exitCBMmode, void());
        MOCK_METHOD0(getCallState, uint32_t());
        MOCK_METHOD0(getVoiceServiceState, uint32_t());
};

MockTelephoneManager *M_TelephoneManager;

TelephoneManager::TelephoneManager(android::sp<OemCallHandler> spHandler)
{
}

void TelephoneManager::dial(const std::string& number, bool isEcall) {
    return M_TelephoneManager->dial(number, isEcall);
}

void TelephoneManager::endCall() {
    return M_TelephoneManager->endCall();
}

void TelephoneManager::answer() {
    return M_TelephoneManager->answer();
}

void TelephoneManager::stop() {
    return M_TelephoneManager->stop();
}

void TelephoneManager::rejectCall() {
	return M_TelephoneManager->rejectCall();
}

void TelephoneManager::exitCBMmode() {

    return M_TelephoneManager->exitCBMmode();
}

uint32_t TelephoneManager::getCallState() {
    return M_TelephoneManager->getCallState();
}

uint32_t TelephoneManager::getVoiceServiceState() {
    return M_TelephoneManager->getVoiceServiceState();
}

}  // namespace oc
