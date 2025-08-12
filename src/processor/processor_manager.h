#ifndef PROCESSOR_MANAGER_H
#define PROCESSOR_MANAGER_H
#include "processor/processor_base.h"

namespace oc {

class ProcessorManager {
public:
    ProcessorManager();
    ~ProcessorManager();

    /**
     * @brief init : This is the method to init the Processor Manager
     * @param spHandler
     * @param adapters
     */
    void init(
        android::sp<OemCallHandler> spHandler,
        std::shared_ptr< AdapterLocator > adapters
    );

    /**
     * @brief handleMessage : This is the method to forward message to processor.
     * @param what
     * @param msg
     */
    void handleMessage(const uint32_t what, const android::sp<sl::Message>& msg);

private:
    /**
     * @brief processors_ This is the data structure to store instances of processor.
     */
    std::vector<std::shared_ptr<ProcessorBase>> processors_;
};

}


#endif // PROCESSOR_MANAGER_H
