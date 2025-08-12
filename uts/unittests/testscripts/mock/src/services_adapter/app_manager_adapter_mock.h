namespace oc {

class MockAppManagerAdapter : public AppManagerAdapter {
    public:
        MockAppManagerAdapter() : AppManagerAdapter() {}
        MOCK_METHOD1(addFeature, void(const std::shared_ptr<FeatureHandler> featureHandler));
        MOCK_METHOD2(queryActionForFeature, void(const std::string name, const std::string type));
        MOCK_METHOD1(requestActiveFeature, bool(const std::string name));
        MOCK_METHOD1(requestDeactiveFeature, bool(const std::string name));
        MOCK_METHOD1(getFeatureState, bool(const std::string featureName));
        MOCK_METHOD1(doHandleAppMgrResponse, void(const sp<sl::Message>& msg));
        MOCK_METHOD1(doHandlePostFeatureStatusChanged, void(const sp<sl::Message>& msg));
        MOCK_METHOD1(doHandlePostActionDelivered, void(const sp<sl::Message>& msg));
        MOCK_METHOD1(sendPostACNUS_ON_OF, void(int onOff));
        MOCK_METHOD1(isAvailableInRegion, bool(const std::string feature));
};

MockAppManagerAdapter *M_AppManagerAdapter;

AppManagerAdapter::AppManagerAdapter()
    :impl_(nullptr){
}

void AppManagerAdapter::addFeature(const std::shared_ptr<FeatureHandler> featureHandler) {
    return M_AppManagerAdapter->addFeature(featureHandler);
}

void AppManagerAdapter::queryActionForFeature(const std::string name, const std::string type) {
    return M_AppManagerAdapter->queryActionForFeature(name, type);
}

bool AppManagerAdapter::requestActiveFeature(const std::string name) {
    return M_AppManagerAdapter->requestActiveFeature(name);
}

bool AppManagerAdapter::requestDeactiveFeature(const std::string name) {
    return M_AppManagerAdapter->requestDeactiveFeature(name);
}

bool AppManagerAdapter::getFeatureState(const std::string featureName) {
    return M_AppManagerAdapter->getFeatureState(featureName);
}

void AppManagerAdapter::doHandleAppMgrResponse(const sp<sl::Message>& msg) {
    return M_AppManagerAdapter->doHandleAppMgrResponse(msg);
}

void AppManagerAdapter::doHandlePostFeatureStatusChanged(const sp<sl::Message>& msg) {
    return M_AppManagerAdapter->doHandlePostFeatureStatusChanged(msg);
}

void AppManagerAdapter::doHandlePostActionDelivered(const sp<sl::Message>& msg) {
    return M_AppManagerAdapter->doHandlePostActionDelivered(msg);
}

void AppManagerAdapter::sendPostACNUS_ON_OF(int onOff) {
    return M_AppManagerAdapter->sendPostACNUS_ON_OF(onOff);
}

bool AppManagerAdapter::isAvailableInRegion(const std::string feature) {
    return M_AppManagerAdapter->isAvailableInRegion(feature);
}

}  // namespace oc

