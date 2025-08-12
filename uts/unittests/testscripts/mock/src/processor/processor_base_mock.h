namespace oc {

class MockProcessorBase : public ProcessorBase {
    public:
        MOCK_METHOD2(doProcessorHandler, void(uint32_t what, const sp<sl::Message>& msg));
        MOCK_METHOD1(onTimerEvent, void(uint32_t timerId));
        MOCK_METHOD2(init, void(android::sp<OemCallHandler> spHandler, std::shared_ptr< AdapterLocator > adapters));
        MOCK_METHOD0(isAvailable, bool());
};

MockProcessorBase *M_ProcessorBase;

void ProcessorBase::doProcessorHandler(uint32_t what, const sp<sl::Message>& msg) {
    return M_ProcessorBase->doProcessorHandler(what, msg);
}

void ProcessorBase::onTimerEvent(uint32_t timerId) {
    return M_ProcessorBase->onTimerEvent(timerId);
}

void ProcessorBase::init(android::sp<OemCallHandler> spHandler, std::shared_ptr< AdapterLocator > adapters) {
    return M_ProcessorBase->init(spHandler, adapters);
}

bool ProcessorBase::isAvailable() {
    return M_ProcessorBase->isAvailable();
}

}  // namespace oc

