namespace oc {

class MockOEMCallApp2 : public ecallapp::ECallApp2 {
    public:
        MOCK_METHOD2(onFeatureStatusChanged, error_t(const std::string feature, const FeatureStatus status));
        MOCK_METHOD2(onFeatureActionPerformed, error_t(const FeatureAction action, const std::string feature));
};

MockOEMCallApp2 *M_OEMCallApp2;

ecallapp::ECallApp2::ECallApp2()
{
}

ecallapp::ECallApp2::~ECallApp2() {
}

void ecallapp::ECallApp2::onCreate() {
}

void ecallapp::ECallApp2::onDestroy() {
}

void ecallapp::ECallApp2::onActive(const int32_t param, std::string param2) noexcept {
}

void ecallapp::ECallApp2::onInactive(const int32_t param, std::string param2) noexcept {
}

void ecallapp::ECallApp2::onPostReceived(const sp<Post>& post) {
}

void ecallapp::ECallApp2::doECallhandler(const int32_t& what, const android::sp<sl::Message>& message) {
}
void ecallapp::ECallApp2::doECallhandlerInternal(const int32_t& what, const android::sp<sl::Message>& message)
{

}
error_t ecallapp::ECallApp2::onFeatureActionDelivered(const std::string feature, const FeatureAction action, const std::string causedBy) {
    return M_OEMCallApp2->onFeatureActionDelivered(feature, action, causedBy);
}

void ecallapp::ECallApp2::setParrent(oc::OEMCallAppIF * parrent)
{
    
}

error_t ecallapp::ECallApp2::onFeatureStatusChanged(const std::string feature, const FeatureStatus status) {
    return M_OEMCallApp2->onFeatureStatusChanged(feature, status);
}

error_t ecallapp::ECallApp2::onFeatureActionPerformed(const FeatureAction action, const std::string feature) {
    return M_OEMCallApp2->onFeatureActionPerformed(action, feature);
}


}
