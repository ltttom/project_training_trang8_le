/* Define the USE_LGEFILEIO and add header */
#include "utils_common.h"
#include <sys/stat.h>
#include <spawn.h>
#include <limits>
#include "../logger.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <services/TimeManagerService/TimeManager.h>
#include <services/ConfigurationManagerService/ConfigManager.h>
// #include <services/ApplicationManagerService/ApplicationManager.h>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <utils/PosixCmd.h>
#include <fstream>

#define USE_LGEFILEIO // It must be defined before the header file.
#include <lgefileio.h>
#ifdef __UNITTEST__
#define try      if (false)
#define catch(X) if (false)
#define __throw_exception_again
#define __throw
#undef LOG_I
#define LOG_I(...)
#undef LOG_E
#define LOG_E(...) 
#undef LOG_W
#define LOG_W(...) 
#define USE_DEFAULT_PREPATH ""
#define USE_DEFAULT_PREPATH_1 "../build/"
#else
#define USE_DEFAULT_PREPATH "/data/systemrw/oemcall/"
#endif


namespace oc{
namespace cv {
error_t Common::saveToNVM(const uint8_t &data, const std::string &name) {
    error_t res = 0;
    FILE *fp_absolute = NULL;
    uint8_t buffer[1];
    buffer[0] = data;
#ifdef __UNITTEST__
    if(data==0)
        res = createNewDirectory(USE_DEFAULT_PREPATH);
    else
        res = createNewDirectory(USE_DEFAULT_PREPATH_1);
#else
    res = createNewDirectory(USE_DEFAULT_PREPATH);
#endif
    auto path = USE_DEFAULT_PREPATH + name;

    if (isFileExist(path)) {
        LOG_E(DLT_STRING("File already exist"));
        return E_OK;
    }
    if (!res) {
        res = run_cmd("touch " + path);
    }
    if (!res) {
        fp_absolute = fopen(path.c_str(), "w+");
        //LCOV_EXCL_START
        if (fp_absolute) {
            auto count = fwrite(buffer, sizeof (uint8_t), sizeof (buffer), fp_absolute);
            (count == sizeof (buffer)) ? (res = E_OK) : (res = E_ERROR);
            fclose(fp_absolute);
        } else {
            res = E_ERROR;
        }
        //LCOV_EXCL_STOP
    }

    if (res != E_OK) {
        LOG_E(DLT_STRING("Cannot save data to NVM"));
    }
    return res;
}

error_t Common::readFromNVM(uint8_t& out, const std::string &name) {
    error_t res = 0;
    auto path = USE_DEFAULT_PREPATH + name;
    auto fp_relative = fopen(path.c_str(), "r");
    uint8_t buffer[1] = {0x99};

    if (!fp_relative) {
        LOG_E(DLT_STRING("Cannot open file"), DLT_STRING(path.c_str()));
        res = E_ERROR;
    }

    if (res != E_ERROR) {
        auto count = fread(buffer, sizeof (uint8_t), sizeof(buffer), fp_relative);
        //LCOV_EXCL_START
        (count == sizeof (buffer)) ? (res = E_OK) : (res = E_ERROR);
        //LCOV_EXCL_STOP
    }

    if (fp_relative) {
        fclose(fp_relative);
    }
    out = buffer[0];
    if (res != E_OK) {
        LOG_E(DLT_STRING("Cannot read data from NVM"));
    }
    return res;
}

error_t Common::removeFromNVM(const std::string &name) {
    error_t res = 0;

    auto path = "/data/systemrw/oemcall/" + name;
    auto cmd  = "rm -rf " + path;

    res = run_cmd(cmd);
    if (res != 0) {
        LOG_E(DLT_STRING("Can not remove file"));
    }
    return (error_t) res;
}

error_t Common::createNewDirectory(const std::string &path) {
#if defined __linux__
    struct stat info;
    error_t res = 0;
    auto status = stat(path.c_str(), &info); // check the directory are writetable
    //LCOV_EXCL_START
    bool isDirExist = bool(
        (status == 0U) && // path exist
        (info.st_mode & S_IFDIR) // it's a directory
    );
    //LCOV_EXCL_STOP
    if (!isDirExist) {
        LOG_W(DLT_STRING("Try to create dir! "), DLT_STRING(path.c_str()));
        std::string cmd = "mkdir -p " + path;
        res = run_cmd(cmd);
        if (res != 0) {
            LOG_E(DLT_STRING("Can not create dir!"));
        }
    }
    return (error_t)res;

#else
    LOG_E(DLT_STRING("Not in posix environment"));
    return E_ERROR;
#endif
}


std::string Common::getUtcTime() {
    // Get UTC time & Timezone base on RFC3339 standard format: "YYYY-MM-DDTHH:MM:SS+00:00"
    tm now = TimeManager::getInstance().getCurrentTime();
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(4) << (1900 + now.tm_year)
    << "-"
    << std::setfill('0') << std::setw(2) << (1 + now.tm_mon)
    << "-"
    << std::setfill('0') << std::setw(2) << now.tm_mday
    << "T"
    << std::setfill('0') << std::setw(2) << now.tm_hour
    << ":"
    << std::setfill('0') << std::setw(2) << now.tm_min
    << ":"
    << std::setfill('0') << std::setw(2) << now.tm_sec << "+00:00";

    return ss.str();
}

std::string Common::convertTime(uint64_t timeStamp) {
    using namespace std::chrono;
    LOG_I(DLT_STRING("TimeStamp: "), DLT_UINT64(timeStamp));

    //        struct gbt_time_t gnssDateTime;
    // convert to time_t
    const auto durationSinceEpoch = std::chrono::milliseconds(timeStamp);
    const time_point<system_clock> tp_after_duration(durationSinceEpoch);
    const time_t time_after_duration = system_clock::to_time_t(tp_after_duration);

    std::tm* timeData = std::gmtime(&time_after_duration);

    std::ostringstream sstr;
    if(timeStamp == 0) {
        std::string dtTime = "1900-01-01T00:00:00+00:00";
        LOG_I(DLT_STRING(dtTime.c_str()));
        return dtTime;
    } else {
        if(timeData != nullptr)
        {
            sstr << std::setfill('0') << std::setw(4) << (timeData->tm_year + 1900)
                << "-"
                << std::setfill('0') << std::setw(2) << (timeData->tm_mon + 1)
                << "-"
                << std::setfill('0') << std::setw(2) << timeData->tm_mday
                << "T"
                << std::setfill('0') << std::setw(2) << timeData->tm_hour
                << ":"
                << std::setfill('0') << std::setw(2) << timeData->tm_min
                << ":"
                << std::setfill('0') << std::setw(2) << timeData->tm_sec
                << "+00:00";
            LOG_I(DLT_STRING(sstr.str().c_str()));
            return sstr.str(); // TODO: get time
        }
        return "";
    }
}

std::string Common::getConfig(const std::string &name) {
    bool isError = false;
    std::string res = "";
    ConfigResponse response;

    try {
        response = ConfigManager::instance()->getData(DataFrom::General, name);
    } catch (...) {
        LOG_E(DLT_STRING("throw exception getConfig"));
        isError = true;
    }

    if (!isError) {
        if (response.result == ConfigResult::NoError){
            LOG_E(DLT_STRING("ConfigResult::NoError"));
            res = response.read();
        }
        else {
            isError = true;
        }
    }

    if (isError) {
        LOG_E(DLT_STRING("Get Config Error"));
    }
    return res;
}

std::string Common::trimWhiteSpace(std::string s) {
    size_t i = 0;
    while (
        (i < s.length())
        && (s[i] <= 32)
    ) {
        ++i;
    }
    size_t j = s.length();
    while (
        (j > 0)
        && (s[j - 1] <= 32)
    ) {
        --j;
    }
    if (j>=i)
        return s.substr(i, j - i);
    else
        return "";
}

bool Common::isFileExist(const std::string &path) {
    std::ifstream f(path.c_str());
    return f.good();
}

}
}
