#ifndef CANDATACOLLECTOR_H
#define CANDATACOLLECTOR_H
#include <json/json.h>

namespace oc {
class CanDataCollector {
public:
    CanDataCollector();
    void collect();
    Json::Value get();

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

#endif // CANDATACOLLECTOR_H
