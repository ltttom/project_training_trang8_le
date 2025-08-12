#ifndef TELEPHONY_MANAGER_MOCK_H
#define TELEPHONY_MANAGER_MOCK_H

class Test_TelephonyRegistry: public telephony::ITelephonyRegistry {
public:
    Test_TelephonyRegistry():telephony::ITelephonyRegistry(){}
    IBinder* onAsBinder(){return nullptr;}
    void listen(const android::sp<IPhoneStateListener> &callback, uint64_t events, bool notifyNow, const TelephonyManager::SlotIdType slotId = TelephonyManager::SlotIdType::DEFAULT_ID){}
};

class Mock_TelephonyManager{
public:
    MOCK_METHOD0(getCallState,int32_t());
    MOCK_METHOD0(getImei,std::string());
    MOCK_METHOD0(getMsisdn, std::string());
};

Mock_TelephonyManager* M_TelephonyManager;

void telephony::TelephonyManager::listen(const std::shared_ptr<PhoneStateListener> &listener,
            uint64_t events,
            bool notifyNow = false,
            const TelephonyManager::SlotIdType slotId) {}
void telephony::TelephonyManager::dial(const std::string &number,
         const int32_t callType,
         const TelephonyManager::SlotIdType slotId) {}
void telephony::TelephonyManager::endCall(const TelephonyManager::SlotIdType slotId) {}
void telephony::TelephonyManager::rejectCall(const TelephonyManager::SlotIdType slotId) {}
void telephony::TelephonyManager::answerRingingCall(const TelephonyManager::SlotIdType slotId) {}
std::string telephony::TelephonyManager::getMsisdn(const TelephonyManager::SlotIdType slotId) { return M_TelephonyManager->getMsisdn();}
std::string telephony::TelephonyManager::getNetworkOperator(const TelephonyManager::SlotIdType slotId) {return "van";}
std::string telephony::TelephonyManager::getSimOperator(const TelephonyManager::SlotIdType slotId) {return "van";}
int32_t telephony::TelephonyManager::getServiceState(const int32_t type,
                       const TelephonyManager::SlotIdType slotId) {return 0;}
std::string telephony::TelephonyManager::getImei() {return M_TelephonyManager->getImei();}
int32_t telephony::TelephonyManager::getCallState(const TelephonyManager::SlotIdType slotId) {return M_TelephonyManager->getCallState();}

#endif //TELEPHONY_MANAGER_MOCK_H