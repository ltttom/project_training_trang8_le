#include "processor_manager.h"
#include "acn_phone_processor.h"
#include "r_call_jp_processor.h"

oc::ProcessorManager::ProcessorManager() {
    processors_.push_back(std::make_shared<ACNPhoneProcessor>());
    processors_.push_back(std::make_shared<RCallProcessor>());
}

oc::ProcessorManager::~ProcessorManager() {
    for (auto& processor : processors_) {
        processor.reset();
    }
}

void oc::ProcessorManager::init(android::sp<oc::OemCallHandler> spHandler, std::shared_ptr<oc::AdapterLocator> adapters) {
    for (auto& processor : processors_) {
        processor->init(spHandler, adapters);
    }
}

void oc::ProcessorManager::handleMessage(const uint32_t what, const android::sp<sl::Message> &msg) {
    for (auto& processor : processors_) {
        if (processor->isAvailable()) {
            processor->doProcessorHandler(what, msg);
        }
    }
}
