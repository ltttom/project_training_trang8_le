#ifndef IAPPLICATION_MANAGER_SERVICE_H
#define IAPPLICATION_MANAGER_SERVICE_H

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <string>

#include "SystemPost.h"
#include "Typedef.h"
#include "utils/Post.h"

#include "services/ApplicationManagerService/ISystemPostReceiver.h"
#include "services/ApplicationManagerService/IApplicationManagerServiceType.h"

class IApplicationManagerService : public android::IInterface {
public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    DECLARE_META_INTERFACE(ApplicationManagerService);
    // virtual error_t enumerateAvailableApplication(bool bReset=false)=0;
    // virtual error_t disableAllApplication(void)=0;
    // virtual error_t attachApplication(const char* app, android::sp<android::IBinder> proxy)=0;
    // virtual error_t enableApplication(const char* app, int32_t reason, bool bIgnoreMode=false)=0;
    // virtual void broadcastHMI(const uint32_t type, const uint32_t action) =0;
    // virtual error_t ReadyToDestroy(const char* app) = 0;
    // virtual error_t readyToRun(const char* app) = 0;
#endif

    virtual error_t sendPost(const char* app, const android::sp<Post>& post)=0;
    // virtual error_t broadcastPost(const android::sp<Post>& post)=0;
    // virtual error_t broadcastSystemPost(const android::sp<Post>& systemPost)=0;
    virtual error_t registerSystemPostReceiver(const android::sp<ISystemPostReceiver>& receiver, const int32_t post) = 0;
    // virtual error_t unregisterSystemPostReceiver(const android::sp<ISystemPostReceiver>& receiver) = 0;
    // virtual error_t disableApplication(const char* app, int32_t reason)=0;
    // virtual bool isEnabled(const char* app) = 0;

// #ifndef DOXYGEN_SHOULD_SKIP_THIS
    // virtual void setBootCompleted(bool complete) = 0;
// #endif

    virtual bool getBootCompleted(bool bPreComplete = false) = 0;
    // virtual error_t requestActive(std::string app, const int32_t param = -1, const std::string param2 = "") = 0;
    // virtual error_t requestInactive(std::string app, const int32_t param = -1, const std::string param2 = "") = 0;
    // virtual bool isActive(std::string app) = 0;
    // virtual error_t triggerEvent(std::string event, const int32_t param = -1, const std::string param2 = "") = 0;
    virtual error_t setFeatureStatus(const std::string app, const std::string feature, const FeatureStatus status) = 0;
    virtual FeatureStatus getFeatureStatus(const std::string feature) = 0;
    virtual FeatureAction queryActionForFeature(const std::string feature) = 0;
    // virtual std::string getAllFeaturesStatus(void) = 0;
    // virtual std::string getActionTableForFeature(std::string feature) = 0;
    virtual bool isAvailableInRegion(const std::string feature) = 0;
    // virtual int32_t getHighestPriority() = 0;
    // virtual int32_t getPriority(std::string feature) = 0;
    // virtual std::string getFeatureOfPriority(uint32_t priority) = 0;
    // virtual std::string getAllFeaturesByPriority() = 0;
    // virtual error_t updateServiceStatus(android::sp<android::IBinder> token, std::uintptr_t handle, int32_t presence, int32_t priority, int32_t properties) = 0;
    // virtual int32_t getPriorityStatus() = 0;
    // virtual error_t enableAllApplication(void)=0;
};


class BnApplicationManagerService : public android::BnInterface<IApplicationManagerService> {
public:
    virtual android::status_t onTransact(uint32_t code, const android::Parcel& data, android::Parcel* reply, uint32_t flags);
};
#endif /* IAPPLICATION_MANAGER_SERVICE_H */