#ifndef APPLICATION_MANAGER_ADAPTER_MOCK_H
#define APPLICATION_MANAGER_ADAPTER_MOCK_H

#include "tspecall/core/services/ApplicationManagerAdapter.h"

// using namespace ecallapp;

class MockApplicationManagerAdapter : public ApplicationManagerAdapter {
    public:
        MockApplicationManagerAdapter() : ApplicationManagerAdapter() {}
        MOCK_METHOD0(registerService, void());
        MOCK_CONST_METHOD0(getBootComplete, bool());
        MOCK_METHOD2(queryActionForFeature, void(const std::string& feature, const bool timeOutLimit));
        MOCK_METHOD1(getFeatureStatus, int32_t(const std::string& name));
        MOCK_METHOD1(isAvailableInRegion, bool(const std::string feature));
        MOCK_METHOD1(doFeatureActionPerformed, void(const sp<sl::Message>& msg));
        MOCK_METHOD1(doHandlePostActionDelivered, void(const sp<sl::Message>& msg));
        MOCK_METHOD1(doHandlePostFeatureActionDelivered, void(const sp<sl::Message>& msg));
        MOCK_METHOD1(doHandlePostFeatureStatusChanged, void(const sp<sl::Message>& msg));
        MOCK_METHOD0(handleQueryActionForFeatureTimerTimeOut, void());
        MOCK_METHOD2(setFeatureStatus, int32_t(const std::string& name, const bool& onOf));
        MOCK_METHOD1(sendPostACNUS_ON_OFF, void(int32_t));
};

MockApplicationManagerAdapter *M_ApplicationManagerAdapter;

ApplicationManagerAdapter* ApplicationManagerAdapter::instance = nullptr;
ApplicationManagerAdapter* ApplicationManagerAdapter::getInstance() 
{
    if (instance == nullptr) {
        instance = new ApplicationManagerAdapter();
    }
    return instance;
}

ApplicationManagerAdapter::ApplicationManagerAdapter() {}

void ApplicationManagerAdapter::registerService() {
    M_ApplicationManagerAdapter->registerService();
}
bool ApplicationManagerAdapter::getBootComplete() const noexcept{
    return M_ApplicationManagerAdapter->getBootComplete();
}

void ApplicationManagerAdapter::queryActionForFeature(const std::string& feature, const bool timeOutLimit) {
    M_ApplicationManagerAdapter->queryActionForFeature(feature, timeOutLimit);
}

int32_t ApplicationManagerAdapter::setFeatureStatus(const std::string& name, const bool& onOf) {
    return M_ApplicationManagerAdapter->setFeatureStatus(name, onOf);
}

int32_t ApplicationManagerAdapter::getFeatureStatus(const std::string& name) {
    return M_ApplicationManagerAdapter->getFeatureStatus(name);
}

bool ApplicationManagerAdapter::isAvailableInRegion(const std::string feature) {
    return M_ApplicationManagerAdapter->isAvailableInRegion(feature);
}

void ApplicationManagerAdapter::doFeatureActionPerformed(const sp<sl::Message>& msg) {
    M_ApplicationManagerAdapter->doFeatureActionPerformed(msg);
}

void ApplicationManagerAdapter::doHandlePostActionDelivered(const sp<sl::Message>& msg) {
    M_ApplicationManagerAdapter->doHandlePostActionDelivered(msg);
}

void ApplicationManagerAdapter::doHandlePostFeatureActionDelivered(const sp<sl::Message>& msg) {
    M_ApplicationManagerAdapter->doHandlePostFeatureActionDelivered(msg);
}

void ApplicationManagerAdapter::doHandlePostFeatureStatusChanged(const sp<sl::Message>& msg) {
    M_ApplicationManagerAdapter->doHandlePostFeatureStatusChanged(msg);
}

void ApplicationManagerAdapter::handleQueryActionForFeatureTimerTimeOut() {
    M_ApplicationManagerAdapter->handleQueryActionForFeatureTimerTimeOut();
}

void ApplicationManagerAdapter::sendPostACNUS_ON_OFF(int32_t post) {
    M_ApplicationManagerAdapter->sendPostACNUS_ON_OFF(post);
}

#endif // APPLICATION_MANAGER_ADAPTER_MOCK_H


