#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <dlfcn.h>

#include <dlt/dlt.h>

#undef private
#define private public
#undef protected
#define protected public

#ifdef DLT_LOG
#undef DLT_LOG
#endif
#define DLT_LOG(...) std::cerr <<"LOG: "<<__LINE__<< ": " << __FUNCTION__<< std::endl;

#include "tsp_mock/header_mock/logger_Base.h"
#include "tspecall/core/services/TelephoneManagerAdapter.cpp"

using namespace ecallapp;
using namespace android;
using namespace telephony;
using namespace sl;
#include "tsp_mock/src/ECallAppHandler_mock.h"
#include "tsp_mock/include/utils/Android_mock.h"
#include "tsp_mock/include/utils/Binder_mock.h"
#include "tsp_mock/include/service/TelephonyManager_mock.h"
#include "tsp_mock/include/utils/Message_mock.h"

using ::testing::_;
using ::testing::A;
using ::testing::AllOf;
using ::testing::Args;
using ::testing::AtLeast;
using ::testing::Combine;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::SaveArg;
using ::testing::SaveArgPointee;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;
using ::testing::Values;
using ::testing::Invoke;

class TelephoneManagerAdapterTest : public ::testing::Test {
protected:
    std::unique_ptr<TelephoneManagerAdapter> testObj;
    std::unique_ptr<ECallPhoneStateListener> objListener;
    TelephoneManagerAdapterTest() {
    }

    virtual ~TelephoneManagerAdapterTest() {
    }

    virtual void SetUp() {
        testObj = std::make_unique<TelephoneManagerAdapter>();
        objListener = std::make_unique<ECallPhoneStateListener>(*testObj);
        M_MockAndroidService = new MockAndroidService();
        M_Binder = new MockBinder();
        M_ECallAppHandler = new MockECallAppHandler(mLooper, *new IECallAppIfMock());
        M_Message = new MockMessage();
        M_TelephonyManager = new Mock_TelephonyManager();
    }

    virtual void TearDown() {
        testObj = nullptr;
        objListener = nullptr;
        delete M_MockAndroidService;
        delete M_Binder;
        delete M_ECallAppHandler;
        delete M_Message;
        delete M_TelephonyManager;
    }
};

TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_TelephoneManagerAdapter_TC001){
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_getInstance function
 *   @classID TelephoneManagerAdapter
 *   @methodID getInstance
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_getInstance_TC001
 *   @test_type functionality
 *   @test_objective To test the getInstance of TelephoneManagerAdapter
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result Proper get instance of TelephoneManagerAdapter
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_getInstance_TC001){
    EXPECT_NE(TelephoneManagerAdapter::getInstance(), nullptr);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_getInstance function
 *   @classID TelephoneManagerAdapter
 *   @methodID getInstance
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_getInstance_TC002
 *   @test_type functionality
 *   @test_objective To test the singleton property of getInstance of TelephoneManagerAdapter
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result Same instance is returned on multiple calls
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_getInstance_TC002){
    TelephoneManagerAdapter* instance1 = TelephoneManagerAdapter::getInstance();
    TelephoneManagerAdapter* instance2 = TelephoneManagerAdapter::getInstance();
    EXPECT_EQ(instance1, instance2);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_getService function
 *   @classID TelephoneManagerAdapter
 *   @methodID getService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_getService_TC001
 *   @test_type functionality
 *   @test_objective To test the getService of TelephoneManagerAdapter
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result Proper get service of TelephoneManagerAdapter
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_getService_TC001){
    ITest_ServiceManager* testServiceManager = new ITest_ServiceManager();
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).WillOnce(Return(testServiceManager));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).WillOnce(Return(new MockBinder()));
    EXPECT_NE(testObj->getService(), nullptr);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_getService function
 *   @classID TelephoneManagerAdapter
 *   @methodID getService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_getService_TC002
 *   @test_type functionality
 *   @test_objective To test the singleton property of getService of TelephoneManagerAdapter
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result Same service instance is returned on multiple calls
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_getService_TC002){
    ITest_ServiceManager* testServiceManager = new ITest_ServiceManager();
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).WillRepeatedly(Return(testServiceManager));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).WillRepeatedly(Return(new MockBinder()));
    auto service1 = testObj->getService();
    auto service2 = testObj->getService();
    EXPECT_EQ(service1, service2);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_registerService function
 *   @classID TelephoneManagerAdapter
 *   @methodID registerService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_registerService_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_registerService_TC001){
    ITest_ServiceManager* testServiceManager = new ITest_ServiceManager();
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).Times(1).WillRepeatedly(Return(testServiceManager));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).Times(1).WillRepeatedly(Return(new MockBinder()));
    TelephoneManagerAdapter testObj2;
    android::wp<android::IBinder> who = new MockBinder();
    EXPECT_CALL(*M_Binder, asBinder(_)).Times(1).WillOnce(Invoke([&](const sp<IInterface>& a){
        testObj2.mServiceDeathRecipient->mOnBinderDied(who);
        return new MockBinder();
    }));
    testObj2.mTelephonyRegistry = android::sp<ITelephonyRegistry>(new Test_TelephonyRegistry());
    testObj2.mPhoneStateListener = std::make_shared<ECallPhoneStateListener>(testObj2);
    testObj2.registerService();
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_registerService function
 *   @classID TelephoneManagerAdapter
 *   @methodID registerService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_registerService_TC002
 *   @test_type functionality
 *   @test_objective To test the registration of the telephony service and the handling of service death
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result The telephony service is registered successfully and the service death is handled correctly
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_registerService_TC002){
    ITest_ServiceManager* testServiceManager = new ITest_ServiceManager();
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).Times(1).WillRepeatedly(Return(testServiceManager));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).Times(1).WillRepeatedly(Return(new MockBinder()));
    TelephoneManagerAdapter testObj2;
    android::wp<android::IBinder> who = new MockBinder();
    EXPECT_CALL(*M_Binder, asBinder(_)).Times(1).WillOnce(Invoke([&](const sp<IInterface>& a){
        testObj2.mServiceDeathRecipient->mOnBinderDied(who);
        return new MockBinder();
    }));
    testObj2.mTelephonyRegistry = nullptr;
    testObj2.mPhoneStateListener = std::make_shared<ECallPhoneStateListener>(testObj2);
    testObj2.registerService();
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_registerService function
 *   @classID TelephoneManagerAdapter
 *   @methodID registerService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_registerService_TC003
 *   @test_type functionality
 *   @test_objective To test the registration of the telephony service when mTelephonyRegistry is not null and mPhoneStateListener is null
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result The telephony service is registered successfully and the service death is handled correctly
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_registerService_TC003){
    ITest_ServiceManager* testServiceManager = new ITest_ServiceManager();
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).Times(1).WillRepeatedly(Return(testServiceManager));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).Times(1).WillRepeatedly(Return(new MockBinder()));
    TelephoneManagerAdapter testObj2;
    android::wp<android::IBinder> who = new MockBinder();
    EXPECT_CALL(*M_Binder, asBinder(_)).Times(1).WillOnce(Invoke([&](const sp<IInterface>& a){
        testObj2.mServiceDeathRecipient->mOnBinderDied(who);
        return new MockBinder();
    }));
    testObj2.mTelephonyRegistry = android::sp<ITelephonyRegistry>(new Test_TelephonyRegistry());
    testObj2.mPhoneStateListener = nullptr;
    testObj2.registerService();
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_registerService function
 *   @classID TelephoneManagerAdapter
 *   @methodID registerService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_registerService_TC004
 *   @test_type functionality
 *   @test_objective To test the registration of the telephony service when getService returns nullptr
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result The telephony service registration fails and the retry mechanism is triggered
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_registerService_TC004){
    ITest_ServiceManager* testServiceManager = new ITest_ServiceManager();
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).Times(1).WillRepeatedly(Return(testServiceManager));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).Times(1).WillRepeatedly(Return(nullptr));
    TelephoneManagerAdapter testObj2;
    android::wp<android::IBinder> who = new MockBinder();
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(4000)).Times(1).WillOnce(Return(new sl::Message()));
    EXPECT_CALL(*M_ECallAppHandler,sendMessageDelayed(_,500)).Times(1).WillOnce(Return(true));
    testObj2.mTelephonyRegistry = nullptr;
    testObj2.mPhoneStateListener = nullptr;
    testObj2.registerService();
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_registerService function
 *   @classID TelephoneManagerAdapter
 *   @methodID registerService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_registerService_TC005
 *   @test_type functionality
 *   @test_objective To test the registration of the telephony service when getService returns nullptr and obtainMessage returns nullptr
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result The telephony service registration fails and no retry mechanism is triggered due to obtainMessage returning nullptr
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_registerService_TC005){
    ITest_ServiceManager* testServiceManager = new ITest_ServiceManager();
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).Times(1).WillRepeatedly(Return(testServiceManager));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).Times(1).WillRepeatedly(Return(nullptr));
    TelephoneManagerAdapter testObj2;
    android::wp<android::IBinder> who = new MockBinder();
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(4000)).Times(1).WillOnce(Return(nullptr));
    testObj2.mTelephonyRegistry = nullptr;
    testObj2.mPhoneStateListener = nullptr;
    testObj2.registerService();
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_registerService function
 *   @classID TelephoneManagerAdapter
 *   @methodID registerService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_registerService_TC006
 *   @test_type functionality
 *   @test_objective To test the registration of the telephony service when getService returns nullptr and mServiceDeathRecipient is null
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result The telephony service registration fails and no retry mechanism is triggered due to obtainMessage returning nullptr
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_registerService_TC006){
    ITest_ServiceManager* testServiceManager = new ITest_ServiceManager();
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).Times(1).WillRepeatedly(Return(testServiceManager));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).Times(1).WillRepeatedly(Return(nullptr));
    TelephoneManagerAdapter testObj2;
    android::wp<android::IBinder> who = new MockBinder();
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(4000)).Times(1).WillOnce(Return(nullptr));
    testObj2.mServiceDeathRecipient = nullptr;
    testObj2.mTelephonyRegistry = android::sp<ITelephonyRegistry>(new Test_TelephonyRegistry());;
    testObj2.mPhoneStateListener = nullptr;
    testObj2.registerService();
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_onPreciseCallStateChanged function
 *   @classID TelephoneManagerAdapter
 *   @methodID onPreciseCallStateChanged
 *   @paramList int32_t state
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_onPreciseCallStateChanged_TC001
 *   @test_type functionality
 *   @test_objective To test the behavior of onPreciseCallStateChanged when mHandler is null
 *   @test_precon None
 *   @test_input state = 0
 *   @test_expected_result No action is taken when mHandler is null
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest,TelephoneManagerAdapterTest_onPreciseCallStateChanged_TC001){
    testObj->mHandler = nullptr;
    testObj->onPreciseCallStateChanged(0);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_onPreciseCallStateChanged function
 *   @classID TelephoneManagerAdapter
 *   @methodID onPreciseCallStateChanged
 *   @paramList int32_t state
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_onPreciseCallStateChanged_TC002
 *   @test_type functionality
 *   @test_objective To test the behavior of onPreciseCallStateChanged when mHandler is not null
 *   @test_precon None
 *   @test_input state = 0
 *   @test_expected_result The handler obtains a message with the correct state and sends it to the target
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest,TelephoneManagerAdapterTest_onPreciseCallStateChanged_TC002){
    testObj->mHandler = new MockECallAppHandler(mLooper, *new IECallAppIfMock());
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).Times(1).WillOnce(Return(new sl::Message()));
    testObj->onPreciseCallStateChanged(telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_onPreciseCallStateChanged function
 *   @classID TelephoneManagerAdapter
 *   @methodID onPreciseCallStateChanged
 *   @paramList int32_t state
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_onPreciseCallStateChanged_TC003
 *   @test_type functionality
 *   @test_objective To test the behavior of onPreciseCallStateChanged when obtainMessage returns nullptr
 *   @test_precon None
 *   @test_input state = 0
 *   @test_expected_result No action is taken when obtainMessage returns nullptr
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest,TelephoneManagerAdapterTest_onPreciseCallStateChanged_TC003){
    testObj->mHandler = new MockECallAppHandler(mLooper, *new IECallAppIfMock());
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).Times(1).WillOnce(Return(nullptr));
    testObj->onPreciseCallStateChanged(telephony::TelephonyManager::PRECISE_CALL_STATE_ACTIVE);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_onPreciseCallStateChanged function
 *   @classID TelephoneManagerAdapter
 *   @methodID onPreciseCallStateChanged
 *   @paramList int32_t state
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_onPreciseCallStateChanged_TC004
 *   @test_type functionality
 *   @test_objective To test the behavior of onPreciseCallStateChanged when mHandler is not null and obtainMessage returns a valid message
 *   @test_precon None
 *   @test_input state = telephony::TelephonyManager::PRECISE_CALL_STATE_ACTIVE
 *   @test_expected_result The handler obtains a message with the correct state and sends it to the target
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest,TelephoneManagerAdapterTest_onPreciseCallStateChanged_TC004){
    testObj->mHandler = new MockECallAppHandler(mLooper, *new IECallAppIfMock());
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).Times(1).WillOnce(Return(nullptr));
    testObj->onPreciseCallStateChanged(telephony::TelephonyManager::PRECISE_CALL_STATE_INCOMING);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_onPreciseCallStateChanged function
 *   @classID TelephoneManagerAdapter
 *   @methodID onPreciseCallStateChanged
 *   @paramList int32_t state
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_onPreciseCallStateChanged_TC005
 *   @test_type functionality
 *   @test_objective To test the behavior of onPreciseCallStateChanged when mHandler is not null and obtainMessage returns a valid message
 *   @test_precon None
 *   @test_input state = telephony::TelephonyManager::PRECISE_CALL_STATE_OUTGOING
 *   @test_expected_result The handler obtains a message with the correct state and sends it to the target
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest,TelephoneManagerAdapterTest_onPreciseCallStateChanged_TC005){
    testObj->mHandler = new MockECallAppHandler(mLooper, *new IECallAppIfMock());
    testObj->onPreciseCallStateChanged(-1);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_getIncomingNumber function
 *   @classID TelephoneManagerAdapter
 *   @methodID getIncomingNumber
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_getIncomingNumber_TC001
 *   @test_type functionality
 *   @test_objective To test the retrieval of the incoming number
 *   @test_precon None
 *   @test_input mIncomingNumber = "0987654321"
 *   @test_expected_result The returned incoming number is not an empty string
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_getIncomingNumber_TC001){
    testObj->mIncomingNumber = "0987654321";
    EXPECT_NE(testObj->getIncomingNumber(),"");
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_getIncomingNumber function
 *   @classID TelephoneManagerAdapter
 *   @methodID getIncomingNumber
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_getIncomingNumber_TC002
 *   @test_type functionality
 *   @test_objective To test the retrieval of the incoming number when it is not set
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result The returned incoming number is an empty string
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_getIncomingNumber_TC002){
    EXPECT_EQ(testObj->getIncomingNumber(),"");
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_onDisconnectCause function
 *   @classID TelephoneManagerAdapter
 *   @methodID onDisconnectCause
 *   @paramList int32_t cause
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_onDisconnectCause_TC001
 *   @test_type functionality
 *   @test_objective To test the handling of the disconnect cause
 *   @test_precon None
 *   @test_input cause = 0
 *   @test_expected_result The disconnect cause is logged and stored correctly
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_onDisconnectCause_TC001){
    testObj->onDisconnectCause(0);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_getDisconnectCause function
 *   @classID TelephoneManagerAdapter
 *   @methodID getDisconnectCause
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_getDisconnectCause_TC001
 *   @test_type functionality
 *   @test_objective To test the retrieval of the disconnect cause
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result The correct disconnect cause is returned
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_getDisconnectCause_TC001){
    testObj->getDisconnectCause();
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_isCallDisconnectedNormal function
 *   @classID TelephoneManagerAdapter
 *   @methodID isCallDisconnectedNormal
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_isCallDisconnectedNormal_TC001
 *   @test_type functionality
 *   @test_objective To test the determination of whether the call was disconnected normally
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result The function returns true if the disconnect cause is NORMAL or LOCAL, otherwise false
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_isCallDisconnectedNormal_TC001){
    testObj->isCallDisconnectedNormal();
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_isCallDisconnectedNormal function
 *   @classID TelephoneManagerAdapter
 *   @methodID isCallDisconnectedNormal
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_isCallDisconnectedNormal_TC002
 *   @test_type functionality
 *   @test_objective To test the determination of whether the call was disconnected normally with a specific disconnect cause
 *   @test_precon None
 *   @test_input mDisconnectCause = 2
 *   @test_expected_result The function returns false as the disconnect cause is not NORMAL or LOCAL
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_isCallDisconnectedNormal_TC002){
    testObj->mDisconnectCause = 2;
    testObj->isCallDisconnectedNormal();
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_isCallDisconnectedNormal function
 *   @classID TelephoneManagerAdapter
 *   @methodID isCallDisconnectedNormal
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_isCallDisconnectedNormal_TC003
 *   @test_type functionality
 *   @test_objective To test the determination of whether the call was disconnected normally with a specific disconnect cause
 *   @test_precon None
 *   @test_input mDisconnectCause = 3
 *   @test_expected_result The function returns false as the disconnect cause is not NORMAL or LOCAL
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_isCallDisconnectedNormal_TC003){
    testObj->mDisconnectCause = 3;
    testObj->isCallDisconnectedNormal();
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_dial function
 *   @classID TelephoneManagerAdapter
 *   @methodID dial
 *   @paramList std::string number, bool isECall
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_dial_TC001
 *   @test_type functionality
 *   @test_objective To test the dialing of an emergency call
 *   @test_precon None
 *   @test_input number = "0981144552", isECall = true
 *   @test_expected_result The number is dialed as an emergency call
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_dial_TC001){
    testObj->dial("0981144552", true);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_dial function
 *   @classID TelephoneManagerAdapter
 *   @methodID dial
 *   @paramList std::string number, bool isECall
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_dial_TC002
 *   @test_type functionality
 *   @test_objective To test the dialing of a regular call
 *   @test_precon None
 *   @test_input number = "0981144552", isECall = false
 *   @test_expected_result The number is dialed as a regular voice call
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_dial_TC002){
    testObj->dial("0981144552", false);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_endCall function
 *   @classID TelephoneManagerAdapter
 *   @methodID endCall
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_endCall_TC001
 *   @test_type functionality
 *   @test_objective To test the ending of a call
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result The call is ended successfully
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_endCall_TC001){
    testObj->endCall();
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_rejectCall function
 *   @classID TelephoneManagerAdapter
 *   @methodID rejectCall
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_rejectCall_TC001
 *   @test_type functionality
 *   @test_objective To test the rejection of a call
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result The call is rejected successfully
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_rejectCall_TC001){
    testObj->rejectCall();
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_answerRingingCall function
 * @classID TelephoneManagerAdapter
 * @methodID answerRingingCall
 * @paramList None
 * @priority P1
 * @resolution_Method Direct Invocation
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID TelephoneManagerAdapterTest_answerRingingCall_TC001
 * @test_type functionality
 * @test_objective To verify that the answerRingingCall method calls the corresponding method in TelephonyManager
 * @test_precon None
 * @test_input None
 * @test_expected_result The answerRingingCall method in TelephonyManager is invoked
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_answerRingingCall_TC001){
    testObj->answerRingingCall();
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_getMsisdn function
 * @classID TelephoneManagerAdapter
 * @methodID getMsisdn
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_getMsisdn_TC001
 * @test_type functionality
 * @test_objective To verify that getMsisdn returns an empty string when TelephonyManager returns an empty string
 * @test_precon None
 * @test_input None
 * @test_expected_result getMsisdn returns an empty string
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_getMsisdn_TC001){
    EXPECT_CALL(*M_TelephonyManager, getMsisdn()).WillOnce(Return(""));
    EXPECT_EQ(testObj->getMsisdn(), "");
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_getMsisdn function
 * @classID TelephoneManagerAdapter
 * @methodID getMsisdn
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_getMsisdn_TC002
 * @test_type functionality
 * @test_objective To verify that getMsisdn returns the correct string when TelephonyManager returns a specific string
 * @test_precon None
 * @test_input None
 * @test_expected_result getMsisdn returns "van"
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_getMsisdn_TC002){
    EXPECT_CALL(*M_TelephonyManager, getMsisdn()).WillOnce(Return("van"));
    EXPECT_EQ(testObj->getMsisdn(), "van");
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_getCallState function
 * @classID TelephoneManagerAdapter
 * @methodID getCallState
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_getCallState_TC001
 * @test_type functionality
 * @test_objective To verify that getCallState can be called without any exceptions
 * @test_precon None
 * @test_input None
 * @test_expected_result getCallState executes without throwing any exceptions
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_getCallState_TC001){
    testObj->getCallState();
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_clearingCurrentCall function
 * @classID TelephoneManagerAdapter
 * @methodID clearingCurrentCall
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_clearingCurrentCall_TC001
 * @test_type functionality
 * @test_objective To verify that clearingCurrentCall sets mIsClearingCurrentCall to true when getCallState returns a non-idle state
 * @test_precon mIsClearingCurrentCall is set to false
 * @test_input getCallState returns 5
 * @test_expected_result mIsClearingCurrentCall is set to true
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_clearingCurrentCall_TC001){
    testObj->mIsClearingCurrentCall = false;
    EXPECT_CALL(*M_TelephonyManager, getCallState()).WillOnce(Return(5));
    testObj->clearingCurrentCall();
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_clearingCurrentCall function
 * @classID TelephoneManagerAdapter
 * @methodID clearingCurrentCall
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_clearingCurrentCall_TC002
 * @test_type functionality
 * @test_objective To verify that clearingCurrentCall sets mIsClearingCurrentCall to true when getCallState returns a non-idle state
 * @test_precon mIsClearingCurrentCall is set to false
 * @test_input getCallState returns 3
 * @test_expected_result mIsClearingCurrentCall is set to true
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_clearingCurrentCall_TC002){
    testObj->mIsClearingCurrentCall = false;
    EXPECT_CALL(*M_TelephonyManager, getCallState()).WillOnce(Return(3));
    testObj->clearingCurrentCall();
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_clearingCurrentCall function
 * @classID TelephoneManagerAdapter
 * @methodID clearingCurrentCall
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_clearingCurrentCall_TC003
 * @test_type functionality
 * @test_objective To verify that clearingCurrentCall sets mIsClearingCurrentCall to false when getCallState returns an idle state
 * @test_precon mIsClearingCurrentCall is set to false
 * @test_input getCallState returns 0
 * @test_expected_result mIsClearingCurrentCall is set to false
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_clearingCurrentCall_TC003){
    testObj->mIsClearingCurrentCall = false;
    EXPECT_CALL(*M_TelephonyManager, getCallState()).WillOnce(Return(0));
    testObj->clearingCurrentCall();
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_clearingCurrentCall function
 * @classID TelephoneManagerAdapter
 * @methodID clearingCurrentCall
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_clearingCurrentCall_TC004
 * @test_type functionality
 * @test_objective To verify that clearingCurrentCall does not change mIsClearingCurrentCall when it is already true
 * @test_precon mIsClearingCurrentCall is set to true
 * @test_input None
 * @test_expected_result mIsClearingCurrentCall remains true
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_clearingCurrentCall_TC004){
    testObj->mIsClearingCurrentCall = true;
    testObj->clearingCurrentCall();
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_stopClearingCurrentCall function
 * @classID TelephoneManagerAdapter
 * @methodID stopClearingCurrentCall
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_stopClearingCurrentCall_TC001
 * @test_type functionality
 * @test_objective To verify that stopClearingCurrentCall sets mIsClearingCurrentCall to false
 * @test_precon None
 * @test_input None
 * @test_expected_result mIsClearingCurrentCall is set to false
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_stopClearingCurrentCall_TC001){
    testObj->stopClearingCurrentCall();
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_isClearingCurrentCall function
 * @classID TelephoneManagerAdapter
 * @methodID isClearingCurrentCall
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_isClearingCurrentCall_TC001
 * @test_type functionality
 * @test_objective To verify that isClearingCurrentCall can be called without any exceptions
 * @test_precon None
 * @test_input None
 * @test_expected_result isClearingCurrentCall executes without throwing any exceptions
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_isClearingCurrentCall_TC001){
    testObj->isClearingCurrentCall();
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_getNetworkOperator function
 * @classID TelephoneManagerAdapter
 * @methodID getNetworkOperator
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_getNetworkOperator_TC001
 * @test_type functionality
 * @test_objective To verify that getNetworkOperator can be called without any exceptions
 * @test_precon None
 * @test_input None
 * @test_expected_result getNetworkOperator executes without throwing any exceptions
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_getNetworkOperator_TC001){
    testObj->getNetworkOperator();
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_getSimOperator function
 * @classID TelephoneManagerAdapter
 * @methodID getSimOperator
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_getSimOperator_TC001
 * @test_type functionality
 * @test_objective To verify that getSimOperator can be called without any exceptions
 * @test_precon None
 * @test_input None
 * @test_expected_result getSimOperator executes without throwing any exceptions
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_getSimOperator_TC001){
    testObj->getSimOperator();
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_getVoiceServiceState function
 * @classID TelephoneManagerAdapter
 * @methodID getVoiceServiceState
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_getVoiceServiceState_TC001
 * @test_type functionality
 * @test_objective To verify that getVoiceServiceState can be called without any exceptions
 * @test_precon None
 * @test_input None
 * @test_expected_result getVoiceServiceState executes without throwing any exceptions
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_getVoiceServiceState_TC001){
    testObj->getVoiceServiceState();
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_isDataOutOfService function
 * @classID TelephoneManagerAdapter
 * @methodID isDataOutOfService
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_isDataOutOfService_TC001
 * @test_type functionality
 * @test_objective To verify that isDataOutOfService can be called without any exceptions
 * @test_precon None
 * @test_input None
 * @test_expected_result isDataOutOfService executes without throwing any exceptions
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_isDataOutOfService_TC001){
    testObj->isDataOutOfService();
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_onBinderDied function
 * @classID TelephoneManagerAdapter
 * @methodID onBinderDied
 * @paramList const android::wp<android::IBinder>& who
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_onBinderDied_TC001
 * @test_type functionality
 * @test_objective To verify that onBinderDied sends a delayed message to reconnect to the telephony manager
 * @test_precon None
 * @test_input MockBinder instance
 * @test_expected_result sendMessageDelayed is called with the correct parameters
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_onBinderDied_TC001){
    testObj->instance = new TelephoneManagerAdapter();
    EXPECT_CALL(*M_ECallAppHandler,sendMessageDelayed(_,500)).WillOnce(Return(true));
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).WillOnce(Return(new sl::Message()));
    testObj->onBinderDied(new MockBinder());
}

/**
 * @brief This is a test script for the TelephoneManagerAdapterTest_getImei function
 * @classID TelephoneManagerAdapter
 * @methodID getImei
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID TelephoneManagerAdapterTest_getImei_TC001
 * @test_type functionality
 * @test_objective To verify that getImei returns the correct IMEI string when TelephonyManager returns a specific string
 * @test_precon None
 * @test_input getImei returns "van"
 * @test_expected_result getImei returns "van"
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_getImei_TC001){
    TelephoneManagerAdapter testObj2;
    EXPECT_CALL(*M_TelephonyManager, getImei()).WillOnce(Return("van"));
    EXPECT_EQ(testObj2.getImei(), "van");
}

/**
 * @brief This is a test script for the ECallPhoneStateListenerTest_onPreciseCallStateChanged function
 * @classID ECallPhoneStateListener
 * @methodID onPreciseCallStateChanged
 * @paramList const int32_t state
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID ECallPhoneStateListenerTest_onPreciseCallStateChanged_TC001
 * @test_type functionality
 * @test_objective To verify that onPreciseCallStateChanged can be called without any exceptions
 * @test_precon None
 * @test_input state is 0
 * @test_expected_result onPreciseCallStateChanged executes without throwing any exceptions
 * @test_module
 * @design_id
 */
TEST_F(TelephoneManagerAdapterTest, ECallPhoneStateListenerTest_onPreciseCallStateChanged_TC001){
    objListener->onPreciseCallStateChanged(0);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_onCallStateChanged function
 *   @classID TelephoneManagerAdapter
 *   @methodID onCallStateChanged
 *   @paramList int32_t state, const std::string &incomingNumber
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_onCallStateChanged_TC001
 *   @test_type functionality
 *   @test_objective To test the onCallStateChanged method of TelephoneManagerAdapter
 *   @test_precon None
 *   @test_input int32_t state = 0, std::string incomingNumber = "0987654321"
 *   @test_expected_result Proper handling of call state change in TelephoneManagerAdapter
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_onCallStateChanged_TC001){
    int32_t state = 0;
    std::string incomingNumber = "0987654321";
    testObj->onCallStateChanged(state, incomingNumber);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_onCallStateChanged function
 *   @classID TelephoneManagerAdapter
 *   @methodID onCallStateChanged
 *   @paramList int32_t state, const std::string &incomingNumber
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_onCallStateChanged_TC002
 *   @test_type functionality
 *   @test_objective To test the onCallStateChanged method of TelephoneManagerAdapter
 *   @test_precon None
 *   @test_input int32_t state = 0, std::string incomingNumber = ""
 *   @test_expected_result Proper handling of call state change in TelephoneManagerAdapter
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_onCallStateChanged_TC002){
    int32_t state = 0;
    std::string incomingNumber = "";
    testObj->onCallStateChanged(state, incomingNumber);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_onCallStateChanged function
 *   @classID TelephoneManagerAdapter
 *   @methodID onCallStateChanged
 *   @paramList int32_t state, const std::string &incomingNumber
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_onCallStateChanged_TC003
 *   @test_type functionality
 *   @test_objective To test the onCallStateChanged method of TelephoneManagerAdapter
 *   @test_precon None
 *   @test_input int32_t state = 1, std::string incomingNumber = "0987654321"
 *   @test_expected_result Proper handling of call state change in TelephoneManagerAdapter
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_onCallStateChanged_TC003){
    int32_t state = 1;
    std::string incomingNumber = "0987654321";
    testObj->onCallStateChanged(state, incomingNumber);
}

/**
 *   @brief This is a test script for the TelephoneManagerAdapterTest_onCallStateChanged function
 *   @classID TelephoneManagerAdapter
 *   @methodID onCallStateChanged
 *   @paramList int32_t state, const std::string &incomingNumber
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID TelephoneManagerAdapterTest_onCallStateChanged_TC004
 *   @test_type functionality
 *   @test_objective To test the onCallStateChanged method of TelephoneManagerAdapter
 *   @test_precon None
 *   @test_input int32_t state = 1, std::string incomingNumber = ""
 *   @test_expected_result Proper handling of call state change in TelephoneManagerAdapter
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, TelephoneManagerAdapterTest_onCallStateChanged_TC004){
    int32_t state = 1;
    std::string incomingNumber = "";
    testObj->onCallStateChanged(state, incomingNumber);
}

/**
 *   @brief This is a test script for the ECallPhoneStateListenerTest_onCallStateChanged function
 *   @classID ECallPhoneStateListener
 *   @methodID onCallStateChanged
 *   @paramList int32_t state, std::string incomingNumber
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ECallPhoneStateListenerTest_onCallStateChanged_TC001
 *   @test_type functionality
 *   @test_objective To test the onCallStateChanged method of ECallPhoneStateListener
 *   @test_precon None
 *   @test_input state = 0, incomingNumber = "0981144552"
 *   @test_expected_result Proper handling of call state change with given parameters
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, ECallPhoneStateListenerTest_onCallStateChanged_TC001){
    objListener->onCallStateChanged(0, "0981144552");
}

/**
 *   @brief This is a test script for the ECallPhoneStateListenerTest_onDisconnectCause function
 *   @classID ECallPhoneStateListener
 *   @methodID onDisconnectCause
 *   @paramList int32_t cause
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ECallPhoneStateListenerTest_onDisconnectCause_TC001
 *   @test_type functionality
 *   @test_objective To test the onDisconnectCause method of ECallPhoneStateListener
 *   @test_precon None
 *   @test_input cause = 0
 *   @test_expected_result Proper handling of disconnect cause with given parameter
 *   @test_module
 *   @design_id
 */
TEST_F(TelephoneManagerAdapterTest, ECallPhoneStateListenerTest_onDisconnectCause_TC001){
    objListener->onDisconnectCause(0);
}