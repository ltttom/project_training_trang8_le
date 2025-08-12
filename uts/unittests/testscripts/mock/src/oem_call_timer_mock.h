namespace oc {

class MockOEMCallTimer : public OEMCallTimer {
    public:
        MockOEMCallTimer() : OEMCallTimer(nullptr){}
        MOCK_METHOD1(handlerFunction, void(int timerId));
};

MockOEMCallTimer *M_OEMCallTimer;

void OEMCallTimer::handlerFunction(int timerId) {
    return M_OEMCallTimer->handlerFunction(timerId);
}

}  // namespace oc

