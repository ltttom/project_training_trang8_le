#include <servicelayer_log.h>
#include <Log.h>

#ifdef __cplusplus

#ifndef SERVICE_LAYER_LOG2
#define SERVICE_LAYER_LOG2

namespace LGEATP {

enum class ATPTLogLevel {
    ATPTLOG_VERBOSE,
    ATPTLOG_DEBUG,
    ATPTLOG_INFO,
    ATPTLOG_WARN,
    ATPTLOG_ERROR,
    ATPTLOG_EVENT,
};

class ATPTLog {
public:
    template <typename... Args>
    static void logMessageEx(ATPTLogLevel level, const char *tag, Args... args) {
        // Stub for unit testing
    }

    template <typename... Args>
    static void logMessagefunc(ATPTLogLevel level, const char *tag, const std::string& direction, const std::string& funcname, void* ptr, Args... args) {
        // Stub for unit testing
    }

    template <typename T, std::size_t N>
    static std::string logArray(const T(&array)[N]) {
        return "[Stub for unit testing]";
    }
};

class ATPTLogFunction {
public:
    template<typename... Args>
    ATPTLogFunction(ATPTLogLevel level, const char *tag, int limittime, const std::string& functionName, Args... args) {
        // Stub for unit testing
    }

    ~ATPTLogFunction() {
        // Stub for unit testing
    }
};

} // namespace LGEATP

#define ATPLOGV(...) ((void)0)
#define ATPLOGD(...) ((void)0)
#define ATPLOGI(...) ((void)0)
#define ATPLOGW(...) ((void)0)
#define ATPLOGE(...) ((void)0)
#define ATPELOGE(...) ((void)0)
#define ATPLOGFUNCTION(logLevel, limittime, ...) ((void)0)
#define ATPLOG_ARRAY_STR(native_array) "[Stub for unit testing]"

#endif // SERVICE_LAYER_LOG2

#endif
