class MockLogger {
    public:
        //MOCK_METHOD0(initDLTLog, void());
};

MockLogger *M_Logger;

void initDLTLog() {
    //return M_Logger->initDLTLog();
}

DLT_DECLARE_CONTEXT(ctxOEMCall);
DLT_DECLARE_CONTEXT(ctxOEMCallMASS);
