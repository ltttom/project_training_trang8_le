#include "CommunicationManagerAdapter.h"
namespace ecallapp {

class MockCommManagerAdapter : public CommunicationManagerAdapter {
    public:
        MockCommManagerAdapter() : CommunicationManagerAdapter() {}
        
        MOCK_METHOD0(getInstance, CommunicationManagerAdapter*());
        MOCK_METHOD0(registerService, void());
        MOCK_METHOD2(requestSendACKToMCU, void(const uint32_t& prev,const uint32_t& curr));

        MOCK_METHOD0(getService, android::sp<ICommunicationManagerService>());
        MOCK_METHOD1(onBinderDied, void(const android::wp<android::IBinder>& who));
        MOCK_METHOD1(onCommReceive, void (const sp<CommunicationData>& commData));

};

MockCommManagerAdapter *M_CommManagerAdapter;

CommunicationManagerAdapter::CommunicationManagerAdapter(){}

CommunicationManagerAdapter* CommunicationManagerAdapter::getInstance()
{
    return M_CommManagerAdapter->getInstance();
}

void CommunicationManagerAdapter::registerService()
{
    return M_CommManagerAdapter->registerService();
}

void CommunicationManagerAdapter::requestSendACKToMCU(const uint32_t& prev, const uint32_t& curr) 
{
     return M_CommManagerAdapter->requestSendACKToMCU(prev, curr);
}

android::sp<ICommunicationManagerService> CommunicationManagerAdapter::getService()
{
    return M_CommManagerAdapter->getService();
}

void CommunicationManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who)
{
    return M_CommManagerAdapter->onBinderDied(who);
}

void CommunicationManagerAdapter::onCommReceive(const sp<CommunicationData>& commData)
{
    return M_CommManagerAdapter->onCommReceive(commData);
}

}  // namespace oc

