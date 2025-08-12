#ifndef UTILS_COMMON_H
#define UTILS_COMMON_H
#include <servicelayer/Error.h>
#include <string>
#include <services/SomeipConsumerManagerService/SomeipConsumerManager.h>
#include <services/SomeipProviderManagerService/SomeipProviderManager.h>

namespace oc {
    namespace cv{
        class Common {
        public:
            Common(){};
            virtual ~Common(){};
            static error_t saveToNVM(const uint8_t& data, const std::string& name);
            static error_t readFromNVM(uint8_t& out, const std::string& name);
            static error_t createNewDirectory(const std::string& path);
            static error_t removeFromNVM(const std::string& name);
            static std::string getUtcTime();
            static std::string convertTime(uint64_t timeStamp);
            static std::string getConfig(const std::string& name);
            static std::string trimWhiteSpace(std::string s);
            static bool isFileExist(const std::string& path);
        };
    }
}


#endif // UTILS_COMMON_H
