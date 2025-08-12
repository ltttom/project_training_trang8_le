namespace oc {

class MockHmiManagerAdapter : public HmiManagerAdapter {
    public:
        MockHmiManagerAdapter() :HmiManagerAdapter(nullptr){}
        MOCK_METHOD2(doHandleHMIMessages, void(const android::sp<sl::Message>& message, bool savingFlash));
        MOCK_METHOD1(doHandleRetrySaveTrigger, void(const android::sp<sl::Message>& message));
        MOCK_METHOD2(requestPattern, void(const Pattern pattern, bool isOn));
        MOCK_METHOD1(doHandleSaveTrigger, void(bool));
};

MockHmiManagerAdapter *M_HmiManagerAdapter;

HmiManagerAdapter::HmiManagerAdapter(android::sp<oc::OemCallHandler>){}

void HmiManagerAdapter::doHandleHMIMessages(const android::sp<sl::Message>& message, bool savingFlash) {
    return M_HmiManagerAdapter->doHandleHMIMessages(message, savingFlash);
}

void HmiManagerAdapter::doHandleRetrySaveTrigger(const android::sp<sl::Message>& message) {
    //return M_HmiManagerAdapter->doHandleRetrySaveTrigger(message);
}

void HmiManagerAdapter::requestPattern(const Pattern pattern, bool isOn) {
    return M_HmiManagerAdapter->requestPattern(pattern, isOn);
}

void HmiManagerAdapter::doHandleSaveTrigger(bool savingFlash)
{
    return M_HmiManagerAdapter->doHandleSaveTrigger(savingFlash);
}

}  // namespace oc

