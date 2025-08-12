namespace oc {

class MockOEMCallAppIF : public OEMCallAppIF {
    public:
        MOCK_METHOD2(doOemCallHandler, void(uint32_t what, const android::sp<sl::Message>& msg));
        MOCK_METHOD0(isProcessorNullPtr, bool());
};

MockOEMCallAppIF *M_OEMCallAppIF;

void OEMCallAppIF::doOemCallHandler(uint32_t what, const android::sp<sl::Message>& msg) {
    return M_OEMCallAppIF->doOemCallHandler(what, msg);
}
bool OEMCallAppIF::isProcessorNullPtr() {
}

}  // namespace oc

