#ifndef TELEPHONEMANAGERADAPTER_MOCK_H
#define TELEPHONEMANAGERADAPTER_MOCK_H
// using namespace ecallapp;
class MockTelephoneManagerAdapter : public TelephoneManagerAdapter {
public:
    MOCK_METHOD0(getInstance, TelephoneManagerAdapter*());
    MOCK_METHOD0(registerService, void());
    MOCK_CONST_METHOD2(dial, void(const std::string number, const bool isECall));
    MOCK_CONST_METHOD0(endCall, void());
    MOCK_CONST_METHOD0(rejectCall, void());
    MOCK_CONST_METHOD0(answerRingingCall, void());
    MOCK_METHOD0(clearingCurrentCall, bool());
    MOCK_METHOD0(stopClearingCurrentCall, void());
    MOCK_METHOD0(isClearingCurrentCall, bool());
    MOCK_METHOD0(getIncomingNumber, std::string());
    MOCK_CONST_METHOD0(getMsisdn, std::string());
    MOCK_CONST_METHOD0(getNetworkOperator, std::string());
    MOCK_CONST_METHOD0(getSimOperator, std::string());
    MOCK_CONST_METHOD0(getImei, std::string());
    MOCK_METHOD0(getDisconnectCause, int32_t());
    MOCK_CONST_METHOD0(getCallState, int32_t());
    MOCK_METHOD0(getVoiceServiceState, uint32_t());
    MOCK_METHOD0(getDataServiceState, int32_t());
    MOCK_METHOD0(isCallDisconnectedNormal, bool());
    MOCK_METHOD0(isDataOutOfService, bool());
    MOCK_METHOD1(onPreciseCallStateChanged, void(const int32_t state));
    MOCK_METHOD2(onCallStateChanged, void(const int32_t state, const std::string &incomingNumber));
    MOCK_METHOD1(onDisconnectCause, void(const int32_t cause));
    MOCK_METHOD0(getService, android::sp<telephony::ITelephonyRegistry>());
    MOCK_CONST_METHOD1(generateMsgSeq, void(char_t* const));
    MOCK_METHOD0(retryMOSMS, void());
    MOCK_METHOD1(onBinderDied, void(const android::wp<android::IBinder>& who));
    MOCK_METHOD1(onTimerTimeoutEvent, void(const int32_t timerId));
};

MockTelephoneManagerAdapter* M_TelephoneManagerAdapter;

TelephoneManagerAdapter* TelephoneManagerAdapter::instance = nullptr;

TelephoneManagerAdapter* TelephoneManagerAdapter::getInstance() 
{
    if (instance == nullptr) {
        instance = new TelephoneManagerAdapter();
    }
    return instance;
}

TelephoneManagerAdapter::TelephoneManagerAdapter() {}

void TelephoneManagerAdapter::registerService() {
    M_TelephoneManagerAdapter->registerService();
}

void TelephoneManagerAdapter::dial(const std::string number, const bool isECall) const {
    M_TelephoneManagerAdapter->dial(number, isECall);
}

void TelephoneManagerAdapter::endCall() const {
    M_TelephoneManagerAdapter->endCall();
}

void TelephoneManagerAdapter::rejectCall() const {
    M_TelephoneManagerAdapter->rejectCall();
}

void TelephoneManagerAdapter::answerRingingCall() const {
    M_TelephoneManagerAdapter->answerRingingCall();
}

bool TelephoneManagerAdapter::clearingCurrentCall() {
    return M_TelephoneManagerAdapter->clearingCurrentCall();
}

void TelephoneManagerAdapter::stopClearingCurrentCall() {
    M_TelephoneManagerAdapter->stopClearingCurrentCall();
}

bool TelephoneManagerAdapter::isClearingCurrentCall() {
    return M_TelephoneManagerAdapter->isClearingCurrentCall();
}

std::string TelephoneManagerAdapter::getIncomingNumber() noexcept{
    return M_TelephoneManagerAdapter->getIncomingNumber();
}

std::string TelephoneManagerAdapter::getMsisdn() const {
    return M_TelephoneManagerAdapter->getMsisdn();
}

std::string TelephoneManagerAdapter::getNetworkOperator() const {
    return M_TelephoneManagerAdapter->getNetworkOperator();
}

std::string TelephoneManagerAdapter::getSimOperator() const {
    return M_TelephoneManagerAdapter->getSimOperator();
}

std::string TelephoneManagerAdapter::getImei() const {
    return M_TelephoneManagerAdapter->getImei();
}

int32_t TelephoneManagerAdapter::getDisconnectCause() {
    return M_TelephoneManagerAdapter->getDisconnectCause();
}

int32_t TelephoneManagerAdapter::getCallState() const {
    return M_TelephoneManagerAdapter->getCallState();
}

uint32_t TelephoneManagerAdapter::getVoiceServiceState() noexcept {
    return M_TelephoneManagerAdapter->getVoiceServiceState();
}

int32_t TelephoneManagerAdapter::getDataServiceState() noexcept {
    return M_TelephoneManagerAdapter->getDataServiceState();
}

bool TelephoneManagerAdapter::isCallDisconnectedNormal() {
    return M_TelephoneManagerAdapter->isCallDisconnectedNormal();
}

bool TelephoneManagerAdapter::isDataOutOfService() {
    return M_TelephoneManagerAdapter->isDataOutOfService();
}

void TelephoneManagerAdapter::onPreciseCallStateChanged(const int32_t state) {
    M_TelephoneManagerAdapter->onPreciseCallStateChanged(state);
}

void TelephoneManagerAdapter::onCallStateChanged(const int32_t state, const std::string &incomingNumber) {
    M_TelephoneManagerAdapter->onCallStateChanged(state, incomingNumber);
}

void TelephoneManagerAdapter::onDisconnectCause(const int32_t cause) {
    M_TelephoneManagerAdapter->onDisconnectCause(cause);
}

android::sp<telephony::ITelephonyRegistry> TelephoneManagerAdapter::getService() {
    return M_TelephoneManagerAdapter->getService();
}

void TelephoneManagerAdapter::generateMsgSeq(char_t* const seq) const noexcept {
    M_TelephoneManagerAdapter->generateMsgSeq(seq);
}

void TelephoneManagerAdapter::retryMOSMS() {
    M_TelephoneManagerAdapter->retryMOSMS();
}

void TelephoneManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who) {
    M_TelephoneManagerAdapter->onBinderDied(who);
}

void TelephoneManagerAdapter::onTimerTimeoutEvent(const int32_t timerId) {
    M_TelephoneManagerAdapter->onTimerTimeoutEvent(timerId);
}

class MockECallPhoneStateListener : public ECallPhoneStateListener {
public:
    MockECallPhoneStateListener(TelephoneManagerAdapter& pr) noexcept : ECallPhoneStateListener(pr) {}

    MOCK_METHOD1(onPreciseCallStateChanged, void(const int32_t state));
    MOCK_METHOD2(onCallStateChanged, void(const int32_t state, const std::string &incomingNumber));
    MOCK_METHOD1(onDisconnectCause, void(const int32_t cause));
};

MockECallPhoneStateListener* M_ECallPhoneStateListener;

void ECallPhoneStateListener::onPreciseCallStateChanged(const int32_t state) {
    M_ECallPhoneStateListener->onPreciseCallStateChanged(state);
}

void ECallPhoneStateListener::onCallStateChanged(const int32_t state, const std::string &incomingNumber) {
    M_ECallPhoneStateListener->onCallStateChanged(state, incomingNumber);
}

void ECallPhoneStateListener::onDisconnectCause(const int32_t cause) {
    M_ECallPhoneStateListener->onDisconnectCause(cause);
}
#endif // TELEPHONEMANAGERADAPTER_MOCK_H