class MockApplication {
  public:
    MOCK_METHOD1(getFeatureStatus, FeatureStatus(const std::string feature));
    MOCK_METHOD2(setFeatureStatus, error_t(const std::string feature, FeatureStatus status));
    MOCK_METHOD1(queryActionForFeature, FeatureAction(const std::string feature));
};

MockApplication * M_Application;
FeatureStatus Application::getFeatureStatus(const std::string feature){
    return M_Application->getFeatureStatus(feature);
}

error_t Application::setFeatureStatus(const std::string feature, FeatureStatus status)
{
  return M_Application->setFeatureStatus(feature, status);
}

FeatureAction Application::queryActionForFeature(const std::string feature){
  return M_Application->queryActionForFeature(feature);
}