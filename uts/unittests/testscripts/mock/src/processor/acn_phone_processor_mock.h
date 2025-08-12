
namespace oc {


class MockACNPhoneProcessor {
    public:
        MOCK_METHOD2(doProcessorHandler, void(uint32_t what, const sp<sl::Message>& msg));
        //MOCK_METHOD1(onTimerEvent, void(uint32_t timerId));
        //MOCK_METHOD2(init, void(android::sp<OemCallHandler> spHandler, std::shared_ptr< AdapterLocator > adapters));
        MOCK_METHOD0(isAvailable, bool());
};

MockACNPhoneProcessor *M_ACNPhoneProcessor;

ACNPhoneProcessor::ACNPhoneProcessor()
//    :impl_(new Impl()) 
{
}

ACNPhoneProcessor::~ACNPhoneProcessor() {
}

void ACNPhoneProcessor::doProcessorHandler(uint32_t what, const sp<sl::Message>& msg) {
    return M_ACNPhoneProcessor->doProcessorHandler(what, msg);
}

void ACNPhoneProcessor::onTimerEvent(uint32_t timerId) {
    //return M_ACNPhoneProcessor->onTimerEvent(timerId);
}

void ACNPhoneProcessor::init(android::sp<OemCallHandler> spHandler, std::shared_ptr< AdapterLocator > adapters) {
    //return M_ACNPhoneProcessor->init(spHandler, adapters);
}

bool ACNPhoneProcessor::isAvailable() {
    return M_ACNPhoneProcessor->isAvailable();
}

}  // namespace oc

