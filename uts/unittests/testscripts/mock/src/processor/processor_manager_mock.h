namespace oc {

class MockProcessorManager : public ProcessorManager {
    public:
        MOCK_METHOD2(init, void(android::sp<OemCallHandler> spHandler, std::shared_ptr< AdapterLocator > adapters));
        MOCK_METHOD2(handleMessage, void(const uint32_t what, const android::sp<sl::Message>& msg));
};

MockProcessorManager *M_ProcessorManager;

ProcessorManager::ProcessorManager(){}
ProcessorManager::~ProcessorManager() {
}

void ProcessorManager::init(android::sp<OemCallHandler> spHandler, std::shared_ptr< AdapterLocator > adapters) {
    return M_ProcessorManager->init(spHandler, adapters);
}

void ProcessorManager::handleMessage(const uint32_t what, const android::sp<sl::Message>& msg) {
    return M_ProcessorManager->handleMessage(what, msg);
}

}  // namespace oc

