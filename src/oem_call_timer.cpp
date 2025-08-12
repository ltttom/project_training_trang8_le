#include "oem_call_timer.h"
#include <cstdint>
#include "logger.h"

namespace oc {

void OEMCallTimer::handlerFunction(int timerId) {
    // LOG_I(DLT_INT32(timerId));
    spProcessor_->onTimerEvent((uint32_t)timerId);
}

} // end namespace oc
