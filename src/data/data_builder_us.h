#ifndef DATA_BUILDER_US_H
#define DATA_BUILDER_US_H
#include "i_data_builder.h"

namespace oc {

class DataBuilderUS : IDataBuilder {
public:

    DataBuilderUS();

    std::string build() override;
    IDataBuilder &setCDS(bool isNormalCDS) override;
    IDataBuilder &setGPS() override;
    IDataBuilder &setUtil(std::string ignition) override;

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

}

#endif // DATA_BUILDER_US_H
