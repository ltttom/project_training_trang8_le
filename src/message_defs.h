#ifndef __MESSAGEDEFS_H__
#define __MESSAGEDEFS_H__

#include <binder/Parcel.h>
#include <utils/RefBase.h>
#include <utils/Buffer.h>
#include <services/RemoteIfManagerService/EOrderType.h>
#include <services/RemoteIfManagerService/ESendType.h>
#include <services/RemoteIfManagerService/EServerRespType.h>

namespace oc {
    using namespace android;

    class RIMResponseType : public RefBase {
    public:
        ESendType sendType;
        EServerRespType serverResponse;
        std::string responseBody;
        std::string errorCode;
        std::string errorMessage;
    };

    class ConfigNotiData : public RefBase {
    public:
        std::string name;
        std::string value;
    };

}

#endif
