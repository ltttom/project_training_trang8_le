namespace oc {

class MockRCallProcessor {
    public:
        MOCK_METHOD2(doProcessorHandler, void(uint32_t what, const sp<sl::Message>& msg));
        //MOCK_METHOD1(onTimerEvent, void(uint32_t timerId));
        //MOCK_METHOD2(init, void(android::sp<OemCallHandler> spHandler, std::shared_ptr< AdapterLocator > adapters));
        MOCK_METHOD0(isAvailable, bool());
};

MockRCallProcessor *M_RCallProcessor;

RCallProcessor::RCallProcessor()
//    :impl_(new Impl()) 
{
}

RCallProcessor::~RCallProcessor() {
}

void RCallProcessor::doProcessorHandler(uint32_t what, const sp<sl::Message>& msg) {
    return M_RCallProcessor->doProcessorHandler(what, msg);
}

void RCallProcessor::onTimerEvent(uint32_t timerId) {
    //return M_RCallProcessor->onTimerEvent(timerId);
}

void RCallProcessor::init(android::sp<OemCallHandler> spHandler, std::shared_ptr< AdapterLocator > adapters) {
    //return M_RCallProcessor->init(spHandler, adapters);
}

bool RCallProcessor::isAvailable() {
    return M_RCallProcessor->isAvailable();
}

}  // namespace oc

