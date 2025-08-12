namespace oc {

class MockGPSCollector : public GPSCollector {
    public:
        MOCK_METHOD1(collect, void(Type type));
        MOCK_METHOD0(get, Json::Value());
        MOCK_METHOD0(getString, std::string());
};

MockGPSCollector *M_GPSCollector;

GPSCollector::GPSCollector()
    :impl_(nullptr)
{
}


void GPSCollector::collect(Type type) {
    return M_GPSCollector->collect(type);
}

Json::Value GPSCollector::get() {
    return M_GPSCollector->get();
}

std::string GPSCollector::getString() {
    return M_GPSCollector->getString();
}

}  // namespace oc

