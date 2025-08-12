#include <utils/Timer.h>
#include "processor/processor_base.h"

namespace oc {

class OEMCallTimer: public TimerTimeoutHandler {
private :
    std::shared_ptr<ProcessorBase> spProcessor_;
public:
    OEMCallTimer(std::shared_ptr<ProcessorBase> spProcessor) : spProcessor_(spProcessor){};
    virtual ~OEMCallTimer(){};
    void handlerFunction(int timerId) override;
};

} // end namespaceoemcallapplication
