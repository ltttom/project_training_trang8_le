#ifndef I_DATA_BUILDER_H
#define I_DATA_BUILDER_H
#include <json/json.h>
#include <string>
#include "data_collector_can.h"
#include "data_collector_gps.h"
#include <memory>

namespace oc {

class IDataBuilder {
public:
    virtual std::string build() = 0;
    virtual IDataBuilder &setGPS() = 0;
    virtual IDataBuilder &setCDS(bool isNormalCDS) = 0;
    virtual IDataBuilder &setUtil(std::string ignition) = 0;
protected:
    std::unique_ptr<GPSCollector> upGPSCollector_;
    std::unique_ptr<CanDataCollector> upCanCollector_;
};

}


#endif // I_DATA_BUILDER_H
