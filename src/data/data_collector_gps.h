#ifndef GPS_COLLECTOR_H
#define GPS_COLLECTOR_H
#include <json/json.h>
#define GPS_CMD_LOC_PREFERRED 8
namespace oc {

class GPSCollector {
public:
    enum Type {
        ACN,
        RCALL
    };

    enum GPS_SUBCMD_SET_LOC_PREFERRED

    {     SUBCMD_SET_TSU_PREFERED = 0,
          SUBCMD_SET_ICB_PREFERED = 1,
          SUBCMD_GET_PREFERED_MODE = 2
    };

    enum PREFERRED_MODE

    {     PREFER_TSU_POS = 0,
          PREFER_ICB_POS = 1,
          PREFER_MODE_MAX
    };

    GPSCollector();
    void collect(Type type) ;
    Json::Value get();
    std::string getString();
private:
    /**
     * @brief This is the type of structure that contains the private
     *        properties of the instance.  It is defined in the implementation
     *        and declared here to ensure that it is scoped inside the class.
     */
    struct Impl;

    /**
     * @brief This contain private data of the instance
     *
     */
    std::shared_ptr<Impl> impl_;
};

};

#endif // GPS_COLLECTOR_H
