namespace oc {

class MockCanDataCollector : public CanDataCollector {
    public:
        MOCK_METHOD0(collect, void());
        MOCK_METHOD0(get, Json::Value());
};

MockCanDataCollector *M_CanDataCollector;

CanDataCollector::CanDataCollector()
//    :impl_(new Impl())
{
}


void CanDataCollector::collect() {
    return M_CanDataCollector->collect();
}

Json::Value CanDataCollector::get() {
    return M_CanDataCollector->get();
}

}  // namespace oc

