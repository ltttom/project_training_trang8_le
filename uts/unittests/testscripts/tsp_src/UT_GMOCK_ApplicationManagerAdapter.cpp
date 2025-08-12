#define TC_trimWhiteSpace
#define TC_onSystemPostReceived
#define TC_getInstance
#define TC_registerService
#define TC_getBootComplete
#define TC_queryActionForFeature
#define TC_setFeatureStatus
#define TC_getFeatureStatus
#define TC_isAvailableInRegion
#define TC_doFeatureActionPerformed
#define TC_doHandlePostActionDelivered
#define TC_doHandlePostFeatureActionDelivered
#define TC_doHandlePostFeatureStatusChanged
#define TC_handleQueryActionForFeatureTimerTimeOut
#define TC_getService
#define TC_onBinderDied


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
#include "tsp_mock/header_mock/Message_Base.h"
#include "tsp_mock/header_mock/IApplicationManagerService_Base.h"
#include "tspecall/core/services/ApplicationManagerAdapter.cpp"

using namespace ecallapp;
using namespace android;

#include "tsp_mock/src/ECallAppHandler_mock.h"
#include "tsp_mock/include/utils/Android_mock.h"
#include "tsp_mock/include/utils/Binder_mock.h"
#include "tsp_mock/include/utils/Message_mock.h"
#include "tsp_mock/include/utils/Buffer_mock.h"
#include "tsp_mock/service_adapter/IApplicationManagerService_mock.h"
#include "tsp_mock/include/utils/Timer_mock.h"

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
using ::testing::Throw;

class ApplicationManagerAdapterTest : public ::testing::Test {
protected:
    std::unique_ptr<ApplicationManagerAdapter> testObj;

    virtual void SetUp() {
        testObj = std::make_unique<ApplicationManagerAdapter>();
        M_ECallAppHandler = new MockECallAppHandler(mLooper, *new IECallAppIfMock());
        M_MockAndroidService = new MockAndroidService();
        M_Binder = new MockBinder();
        M_Message = new MockMessage();
        M_Buffer = new MockBuffer();
        M_IApplicationManagerService = new MockIApplicationManagerService();
        M_Timer = new MockTimer();
    }

    virtual void TearDown() {
        testObj = nullptr;
        delete M_ECallAppHandler;
        delete M_MockAndroidService;
        delete M_Binder;
        delete M_Message;
        delete M_Buffer;
        delete M_IApplicationManagerService;
        delete M_Timer;
    }
};



#ifdef TC_trimWhiteSpace
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_trimWhiteSpace function
 *   @classID ApplicationManagerAdapter
 *   @methodID trimWhiteSpace
 *   @paramList std::string s
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_trimWhiteSpace_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_trimWhiteSpace_TC001){
    std::string s = " a ";
    EXPECT_EQ(trimWhiteSpace(s), "a");
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_trimWhiteSpace function
 *   @classID ApplicationManagerAdapter
 *   @methodID trimWhiteSpace
 *   @paramList std::string s
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_trimWhiteSpace_TC002
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_trimWhiteSpace_TC002){
    std::string s = " ";
    EXPECT_EQ(trimWhiteSpace(s), " ");
}
#endif // TC_trimWhiteSpace



#ifdef TC_onSystemPostReceived
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_onSystemPostReceived function
 *   @classID ApplicationManagerAdapter
 *   @methodID onSystemPostReceived
 *   @paramList const android::sp<Post> &systemPost
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_onSystemPostReceived_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_onSystemPostReceived_TC001){
    sp<Post> systemPost = sp<Post>( new Post() );

    systemPost->what = SYS_POST_BOOT_COMPLETED;
    testObj->mIsBootCompleted = false;
    testObj->mHandler = sp<ECallAppHandler>( new ECallAppHandler(mLooper, *new IECallAppIfMock()) );

    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).WillOnce(Return(new sl::Message()));
    EXPECT_CALL(*M_Message, sendToTarget()).WillOnce(Return(true));

    testObj->onSystemPostReceived(systemPost);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_onSystemPostReceived function
 *   @classID ApplicationManagerAdapter
 *   @methodID onSystemPostReceived
 *   @paramList const android::sp<Post> &systemPost
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_onSystemPostReceived_TC002
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_onSystemPostReceived_TC002){
    sp<Post> systemPost = sp<Post>( new Post() );

    systemPost->what = SYS_POST_BOOT_COMPLETED;
    testObj->mIsBootCompleted = false;
    testObj->mHandler = nullptr;

    testObj->onSystemPostReceived(systemPost);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_onSystemPostReceived function
 *   @classID ApplicationManagerAdapter
 *   @methodID onSystemPostReceived
 *   @paramList const android::sp<Post> &systemPost
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_onSystemPostReceived_TC003
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_onSystemPostReceived_TC003){
    sp<Post> systemPost = sp<Post>( new Post() );

    systemPost->what = SYS_POST_BOOT_COMPLETED;
    testObj->mIsBootCompleted = true;

    testObj->onSystemPostReceived(systemPost);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_onSystemPostReceived function
 *   @classID ApplicationManagerAdapter
 *   @methodID onSystemPostReceived
 *   @paramList const android::sp<Post> &systemPost
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_onSystemPostReceived_TC004
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_onSystemPostReceived_TC004){
    sp<Post> systemPost = sp<Post>( new Post() );

    systemPost->what = SYS_POST_FEATURE_STATUS_CHANGED;
    testObj->mIsBootCompleted = true;
    testObj->mHandler = sp<ECallAppHandler>( new ECallAppHandler(mLooper, *new IECallAppIfMock()) );

    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).WillOnce(Return(new sl::Message()));
    EXPECT_CALL(*M_Message, sendToTarget()).WillOnce(Return(true));

    testObj->onSystemPostReceived(systemPost);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_onSystemPostReceived function
 *   @classID ApplicationManagerAdapter
 *   @methodID onSystemPostReceived
 *   @paramList const android::sp<Post> &systemPost
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_onSystemPostReceived_TC005
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_onSystemPostReceived_TC005){
    sp<Post> systemPost = sp<Post>( new Post() );

    systemPost->what = SYS_POST_FEATURE_STATUS_CHANGED;
    testObj->mIsBootCompleted = true;
    testObj->mHandler = nullptr;

    testObj->onSystemPostReceived(systemPost);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_onSystemPostReceived function
 *   @classID ApplicationManagerAdapter
 *   @methodID onSystemPostReceived
 *   @paramList const android::sp<Post> &systemPost
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_onSystemPostReceived_TC006
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_onSystemPostReceived_TC006){
    sp<Post> systemPost = sp<Post>( new Post() );

    systemPost->what = SYS_POST_FEATURE_STATUS_CHANGED;
    testObj->mIsBootCompleted = false;
    testObj->mHandler = sp<ECallAppHandler>( new ECallAppHandler(mLooper, *new IECallAppIfMock()) );

    testObj->onSystemPostReceived(systemPost);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_onSystemPostReceived function
 *   @classID ApplicationManagerAdapter
 *   @methodID onSystemPostReceived
 *   @paramList const android::sp<Post> &systemPost
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_onSystemPostReceived_TC007
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_onSystemPostReceived_TC007){
    sp<Post> systemPost = sp<Post>( new Post() );

    systemPost->what = SYS_POST_FEATURE_ACTION_DELIVERED;
    testObj->mIsBootCompleted = true;
    testObj->mHandler = sp<ECallAppHandler>( new ECallAppHandler(mLooper, *new IECallAppIfMock()) );

    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).WillOnce(Return(new sl::Message()));
    EXPECT_CALL(*M_Message, sendToTarget()).WillOnce(Return(true));

    testObj->onSystemPostReceived(systemPost);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_onSystemPostReceived function
 *   @classID ApplicationManagerAdapter
 *   @methodID onSystemPostReceived
 *   @paramList const android::sp<Post> &systemPost
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_onSystemPostReceived_TC008
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_onSystemPostReceived_TC008){
    sp<Post> systemPost = sp<Post>( new Post() );

    systemPost->what = SYS_POST_FEATURE_ACTION_DELIVERED;
    testObj->mIsBootCompleted = true;
    testObj->mHandler = nullptr;

    testObj->onSystemPostReceived(systemPost);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_onSystemPostReceived function
 *   @classID ApplicationManagerAdapter
 *   @methodID onSystemPostReceived
 *   @paramList const android::sp<Post> &systemPost
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_onSystemPostReceived_TC009
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_onSystemPostReceived_TC009){
    sp<Post> systemPost = sp<Post>( new Post() );

    systemPost->what = SYS_POST_FEATURE_ACTION_DELIVERED;
    testObj->mIsBootCompleted = false;
    testObj->mHandler = sp<ECallAppHandler>( new ECallAppHandler(mLooper, *new IECallAppIfMock()) );

    testObj->onSystemPostReceived(systemPost);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_onSystemPostReceived function
 *   @classID ApplicationManagerAdapter
 *   @methodID onSystemPostReceived
 *   @paramList const android::sp<Post> &systemPost
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_onSystemPostReceived_TC010
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_onSystemPostReceived_TC010){
    sp<Post> systemPost = sp<Post>( new Post() );

    systemPost->what = 0;

    testObj->onSystemPostReceived(systemPost);
}
#endif // TC_onSystemPostReceived



#ifdef TC_getInstance
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_getInstance function
 *   @classID ApplicationManagerAdapter
 *   @methodID getInstance
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_getInstance_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_getInstance_TC001){
    ApplicationManagerAdapter::instance = nullptr;
    ApplicationManagerAdapter::getInstance();
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_getInstance function
 *   @classID ApplicationManagerAdapter
 *   @methodID getInstance
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_getInstance_TC002
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_getInstance_TC002){
    ApplicationManagerAdapter::instance = new ApplicationManagerAdapter();
    ApplicationManagerAdapter::getInstance();
}
#endif // TC_getInstance



#ifdef TC_registerService
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_registerService function
 *   @classID ApplicationManagerAdapter
 *   @methodID registerService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_registerService_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_registerService_TC001){
    MockBinder* tBinder = new MockBinder();

    mockGetInstance = true;
    testObj->mAppManager = new Test_IApplicationManagerService();
    testObj->mIsBootCompleted = false;

    EXPECT_CALL(*M_ECallAppHandler, getInstance()).WillOnce(Return(new ECallAppHandler(mLooper, *new IECallAppIfMock())));
    EXPECT_CALL(*M_Timer, setDurationMs(_,_)).Times(1);
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).WillOnce(Return(new ITest_ServiceManager()));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).WillOnce(Return(new Test_IApplicationManagerService()));
    EXPECT_CALL(*M_Binder, asBinder(_)).WillOnce(Return(tBinder));
    EXPECT_CALL(*tBinder, linkToDeath(_,_,_)).WillOnce(Return(OK));
    EXPECT_CALL(*M_IApplicationManagerService, getBootCompleted(_)).WillOnce(Return(true));
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).WillOnce(Return(new sl::Message()));
    EXPECT_CALL(*M_Message, sendToTarget()).WillOnce(Return(true));
    EXPECT_CALL(*M_IApplicationManagerService, registerSystemPostReceiver(_,_)).WillOnce(Return(true));

    testObj->registerService();
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_registerService function
 *   @classID ApplicationManagerAdapter
 *   @methodID registerService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_registerService_TC002
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_registerService_TC002){
    MockBinder* tBinder = new MockBinder();

    mockGetInstance = true;
    testObj->mAppManager = nullptr;
    testObj->mIsBootCompleted = false;

    EXPECT_CALL(*M_ECallAppHandler, getInstance()).WillOnce(Return(nullptr));
    EXPECT_CALL(*M_Timer, setDurationMs(_,_)).Times(1);
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).WillOnce(Return(new ITest_ServiceManager()));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).WillOnce(Return(new Test_IApplicationManagerService()));
    EXPECT_CALL(*M_Binder, asBinder(_)).WillOnce(Return(tBinder));
    EXPECT_CALL(*tBinder, linkToDeath(_,_,_)).WillOnce(Return(OK));
    EXPECT_CALL(*M_IApplicationManagerService, getBootCompleted(_)).WillOnce(Return(true));
    EXPECT_CALL(*M_IApplicationManagerService, registerSystemPostReceiver(_,_)).WillOnce(Return(true));

    testObj->registerService();
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_registerService function
 *   @classID ApplicationManagerAdapter
 *   @methodID registerService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_registerService_TC003
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_registerService_TC003){
    MockBinder* tBinder = new MockBinder();

    mockGetInstance = true;
    testObj->mAppManager = nullptr;
    testObj->mIsBootCompleted = true;

    EXPECT_CALL(*M_ECallAppHandler, getInstance()).WillOnce(Return(nullptr));
    EXPECT_CALL(*M_Timer, setDurationMs(_,_)).Times(1);
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).WillOnce(Return(new ITest_ServiceManager()));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).WillOnce(Return(new Test_IApplicationManagerService()));
    EXPECT_CALL(*M_Binder, asBinder(_)).WillOnce(Return(tBinder));
    EXPECT_CALL(*tBinder, linkToDeath(_,_,_)).WillOnce(Return(OK));
    EXPECT_CALL(*M_IApplicationManagerService, getBootCompleted(_)).WillOnce(Return(true));
    EXPECT_CALL(*M_IApplicationManagerService, registerSystemPostReceiver(_,_)).WillOnce(Return(true));

    testObj->registerService();
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_registerService function
 *   @classID ApplicationManagerAdapter
 *   @methodID registerService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_registerService_TC004
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_registerService_TC004){
    MockBinder* tBinder = new MockBinder();

    mockGetInstance = true;
    testObj->mAppManager = nullptr;

    EXPECT_CALL(*M_ECallAppHandler, getInstance()).WillOnce(Return(nullptr));
    EXPECT_CALL(*M_Timer, setDurationMs(_,_)).Times(1);
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).WillOnce(Return(new ITest_ServiceManager()));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).WillOnce(Return(new Test_IApplicationManagerService()));
    EXPECT_CALL(*M_Binder, asBinder(_)).WillOnce(Return(tBinder));
    EXPECT_CALL(*tBinder, linkToDeath(_,_,_)).WillOnce(Return(OK));
    EXPECT_CALL(*M_IApplicationManagerService, getBootCompleted(_)).WillOnce(Return(false));
    EXPECT_CALL(*M_IApplicationManagerService, registerSystemPostReceiver(_,_)).WillOnce(Return(true));

    testObj->registerService();
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_registerService function
 *   @classID ApplicationManagerAdapter
 *   @methodID registerService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_registerService_TC005
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_registerService_TC005){
    MockBinder* tBinder = new MockBinder();

    mockGetInstance = true;
    testObj->mAppManager = nullptr;

    EXPECT_CALL(*M_ECallAppHandler, getInstance()).WillOnce(Return(nullptr));
    EXPECT_CALL(*M_Timer, setDurationMs(_,_)).Times(1);
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).WillOnce(Return(new ITest_ServiceManager()));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).WillOnce(Return(new Test_IApplicationManagerService()));
    EXPECT_CALL(*M_Binder, asBinder(_)).WillOnce(Return(tBinder));
    EXPECT_CALL(*tBinder, linkToDeath(_,_,_)).WillOnce(Return(99));

    testObj->registerService();
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_registerService function
 *   @classID ApplicationManagerAdapter
 *   @methodID registerService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_registerService_TC006
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_registerService_TC006){
    mockGetInstance = true;
    testObj->mAppManager = nullptr;

    EXPECT_CALL(*M_ECallAppHandler, getInstance()).WillOnce(Return(new ECallAppHandler(mLooper, *new IECallAppIfMock())));
    EXPECT_CALL(*M_Timer, setDurationMs(_,_)).Times(1);
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).WillOnce(Return(new ITest_ServiceManager()));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).WillOnce(Return(nullptr));
    EXPECT_CALL(*M_ECallAppHandler, sendMessageDelayed(_,_)).Times(1);
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).WillOnce(Return(new sl::Message()));

    testObj->registerService();
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_registerService function
 *   @classID ApplicationManagerAdapter
 *   @methodID registerService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_registerService_TC007
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_registerService_TC007){
    mockGetInstance = true;
    testObj->mAppManager = nullptr;

    EXPECT_CALL(*M_ECallAppHandler, getInstance()).WillOnce(Return(nullptr));
    EXPECT_CALL(*M_Timer, setDurationMs(_,_)).Times(1);
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).WillOnce(Return(new ITest_ServiceManager()));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).WillOnce(Return(nullptr));

    testObj->registerService();
}
#endif // TC_registerService



#ifdef TC_getBootComplete
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_getBootComplete function
 *   @classID ApplicationManagerAdapter
 *   @methodID getBootComplete
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_getBootComplete_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_getBootComplete_TC001){
    testObj->getBootComplete();
}
#endif // TC_getBootComplete



#ifdef TC_queryActionForFeature
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_queryActionForFeature function
 *   @classID ApplicationManagerAdapter
 *   @methodID queryActionForFeature
 *   @paramList const std::string& feature, const bool timeOutLimit
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_queryActionForFeature_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_queryActionForFeature_TC001){
    testObj->mAppManager = nullptr;

    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).WillOnce(Return(new ITest_ServiceManager()));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).WillOnce(Return(nullptr));

    testObj->queryActionForFeature("", true);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_queryActionForFeature function
 *   @classID ApplicationManagerAdapter
 *   @methodID queryActionForFeature
 *   @paramList const std::string& feature, const bool timeOutLimit
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_queryActionForFeature_TC002
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_queryActionForFeature_TC002){
    testObj->mAppManager = new Test_IApplicationManagerService();

    EXPECT_CALL(*M_IApplicationManagerService, queryActionForFeature(_)).WillOnce(Return(FeatureAction::LAUNCH));
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).WillOnce(Return(new sl::Message()));

    testObj->queryActionForFeature("", true);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_queryActionForFeature function
 *   @classID ApplicationManagerAdapter
 *   @methodID queryActionForFeature
 *   @paramList const std::string& feature, const bool timeOutLimit
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_queryActionForFeature_TC003
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_queryActionForFeature_TC003){
    testObj->mAppManager = new Test_IApplicationManagerService();

    EXPECT_CALL(*M_IApplicationManagerService, queryActionForFeature(_)).WillOnce(Return(FeatureAction::IGNORE));
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).WillOnce(Return(new sl::Message()));

    testObj->queryActionForFeature("", true);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_queryActionForFeature function
 *   @classID ApplicationManagerAdapter
 *   @methodID queryActionForFeature
 *   @paramList const std::string& feature, const bool timeOutLimit
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_queryActionForFeature_TC004
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_queryActionForFeature_TC004){
    testObj->mAppManager = new Test_IApplicationManagerService();

    EXPECT_CALL(*M_IApplicationManagerService, queryActionForFeature(_)).WillOnce(Return(FeatureAction::POSTPONE));

    testObj->queryActionForFeature("", true);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_queryActionForFeature function
 *   @classID ApplicationManagerAdapter
 *   @methodID queryActionForFeature
 *   @paramList const std::string& feature, const bool timeOutLimit
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_queryActionForFeature_TC005
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_queryActionForFeature_TC005){
    testObj->mAppManager = new Test_IApplicationManagerService();
    testObj->mTimer2sQuerryActionForFeature = sp<Timer>(new Timer(nullptr, 0));

    EXPECT_CALL(*M_IApplicationManagerService, queryActionForFeature(_)).WillOnce(Return(FeatureAction::UPDATE));
    EXPECT_CALL(*M_Timer, start()).Times(1);
    EXPECT_CALL(*M_Timer, stop()).Times(1);

    testObj->queryActionForFeature("", true);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_queryActionForFeature function
 *   @classID ApplicationManagerAdapter
 *   @methodID queryActionForFeature
 *   @paramList const std::string& feature, const bool timeOutLimit
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_queryActionForFeature_TC006
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_queryActionForFeature_TC006){
    testObj->mAppManager = new Test_IApplicationManagerService();

    EXPECT_CALL(*M_IApplicationManagerService, queryActionForFeature(_)).WillOnce(Return(FeatureAction::UPDATE));

    testObj->queryActionForFeature("", false);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_queryActionForFeature function
 *   @classID ApplicationManagerAdapter
 *   @methodID queryActionForFeature
 *   @paramList const std::string& feature, const bool timeOutLimit
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_queryActionForFeature_TC007
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_queryActionForFeature_TC007){
    testObj->mAppManager = new Test_IApplicationManagerService();

    EXPECT_CALL(*M_IApplicationManagerService, queryActionForFeature(_)).WillOnce(Return(FeatureAction::TRIGGER));

    testObj->queryActionForFeature("", true);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_queryActionForFeature function
 *   @classID ApplicationManagerAdapter
 *   @methodID queryActionForFeature
 *   @paramList const std::string& feature, const bool timeOutLimit
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_queryActionForFeature_TC008
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_queryActionForFeature_TC008){
    testObj->mAppManager = new Test_IApplicationManagerService();

    EXPECT_CALL(*M_IApplicationManagerService, queryActionForFeature(_)).WillOnce(Return(99));

    testObj->queryActionForFeature("", true);
}
#endif // TC_queryActionForFeature



#ifdef TC_setFeatureStatus
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_setFeatureStatus function
 *   @classID ApplicationManagerAdapter
 *   @methodID setFeatureStatus
 *   @paramList const std::string& name, const bool& onOf
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_setFeatureStatus_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_setFeatureStatus_TC001){
    testObj->mAppManager = new Test_IApplicationManagerService();

    EXPECT_CALL(*M_IApplicationManagerService, setFeatureStatus(_,_,_)).Times(1);

    testObj->setFeatureStatus("", true);
}
#endif // TC_setFeatureStatus



#ifdef TC_getFeatureStatus
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_getFeatureStatus function
 *   @classID ApplicationManagerAdapter
 *   @methodID getFeatureStatus
 *   @paramList const std::string& name
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_getFeatureStatus_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_getFeatureStatus_TC001){
    testObj->mAppManager = new Test_IApplicationManagerService();

    EXPECT_CALL(*M_IApplicationManagerService, getFeatureStatus(_)).Times(1);

    testObj->getFeatureStatus("");
}
#endif // TC_getFeatureStatus



#ifdef TC_isAvailableInRegion
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_isAvailableInRegion function
 *   @classID ApplicationManagerAdapter
 *   @methodID isAvailableInRegion
 *   @paramList const std::string feature
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_isAvailableInRegion_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_isAvailableInRegion_TC001){
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).WillOnce(Return(new ITest_ServiceManager()));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).WillOnce(Return(nullptr));

    ApplicationManagerAdapter::isAvailableInRegion("");
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_isAvailableInRegion function
 *   @classID ApplicationManagerAdapter
 *   @methodID isAvailableInRegion
 *   @paramList const std::string feature
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_isAvailableInRegion_TC002
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_isAvailableInRegion_TC002){
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).WillOnce(Return(new ITest_ServiceManager()));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).WillOnce(Return(new Test_IApplicationManagerService()));
    EXPECT_CALL(*M_IApplicationManagerService, isAvailableInRegion(_)).Times(1);

    ApplicationManagerAdapter::isAvailableInRegion("");
}
#endif // TC_isAvailableInRegion



#ifdef TC_doFeatureActionPerformed
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_doFeatureActionPerformed function
 *   @classID ApplicationManagerAdapter
 *   @methodID doFeatureActionPerformed
 *   @paramList const sp<sl::Message>& msg
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_doFeatureActionPerformed_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_doFeatureActionPerformed_TC001){
    sp<sl::Message> msg = sp<sl::Message>( new sl::Message() );

    EXPECT_CALL(*M_Buffer, empty()).WillOnce(Return(false));
    EXPECT_CALL(*M_Buffer, data()).WillRepeatedly(Return("ab"));
    EXPECT_CALL(*M_Buffer, size()).WillOnce(Return(2));

    testObj->doFeatureActionPerformed(msg);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_doFeatureActionPerformed function
 *   @classID ApplicationManagerAdapter
 *   @methodID doFeatureActionPerformed
 *   @paramList const sp<sl::Message>& msg
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_doFeatureActionPerformed_TC002
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_doFeatureActionPerformed_TC002){
    sp<sl::Message> msg = sp<sl::Message>( new sl::Message() );

    msg->arg1 = FeatureAction::UPDATE;

    EXPECT_CALL(*M_Buffer, empty()).WillOnce(Return(true));
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).WillOnce(Return(new sl::Message()));

    testObj->doFeatureActionPerformed(msg);
}
#endif // TC_doFeatureActionPerformed



#ifdef TC_doHandlePostActionDelivered
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_doHandlePostActionDelivered function
 *   @classID ApplicationManagerAdapter
 *   @methodID doHandlePostActionDelivered
 *   @paramList const sp<sl::Message>& msg
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_doHandlePostActionDelivered_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_doHandlePostActionDelivered_TC001){
    sp<sl::Message> msg = sp<sl::Message>( new sl::Message() );
    testObj->doHandlePostActionDelivered(msg);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_doHandlePostActionDelivered function
 *   @classID ApplicationManagerAdapter
 *   @methodID doHandlePostActionDelivered
 *   @paramList const sp<sl::Message>& msg
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_doHandlePostActionDelivered_TC002
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_doHandlePostActionDelivered_TC002){
    sp<sl::Message> msg = sp<sl::Message>( new sl::Message() );

    msg->spRef = sp<Post>( new Post() );

    EXPECT_CALL(*M_Buffer, data()).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(*M_Buffer, size()).WillRepeatedly(Return(0));

    testObj->doHandlePostActionDelivered(msg);
}
#endif // TC_doHandlePostActionDelivered



#ifdef TC_doHandlePostFeatureActionDelivered
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_doHandlePostFeatureActionDelivered function
 *   @classID ApplicationManagerAdapter
 *   @methodID doHandlePostFeatureActionDelivered
 *   @paramList const sp<sl::Message>& msg
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_doHandlePostFeatureActionDelivered_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_doHandlePostFeatureActionDelivered_TC001){
    sp<sl::Message> msg = sp<sl::Message>( new sl::Message() );
    testObj->doHandlePostFeatureActionDelivered(msg);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_doHandlePostFeatureActionDelivered function
 *   @classID ApplicationManagerAdapter
 *   @methodID doHandlePostFeatureActionDelivered
 *   @paramList const sp<sl::Message>& msg
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_doHandlePostFeatureActionDelivered_TC002
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_doHandlePostFeatureActionDelivered_TC002){
    sp<sl::Message> msg = sp<sl::Message>( new sl::Message() );

    msg->spRef = sp<AppMgrEventData>( new AppMgrEventData("", "") );

    testObj->doHandlePostFeatureActionDelivered(msg);
}
#endif // TC_doHandlePostFeatureActionDelivered



#ifdef TC_doHandlePostFeatureStatusChanged
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged function
 *   @classID ApplicationManagerAdapter
 *   @methodID doHandlePostFeatureStatusChanged
 *   @paramList const sp<sl::Message>& msg
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged_TC001){
    sp<sl::Message> msg = sp<sl::Message>( new sl::Message() );

    msg->what = 0x7FFFFFFF; // simulate exception thrown case for getObject function

    testObj->doHandlePostFeatureStatusChanged(msg);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged function
 *   @classID ApplicationManagerAdapter
 *   @methodID doHandlePostFeatureStatusChanged
 *   @paramList const sp<sl::Message>& msg
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged_TC002
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged_TC002){
    sp<sl::Message> msg = sp<sl::Message>( new sl::Message() );
    sp<Post> post( new Post() );

    post->arg1 = FeatureStatus::ON;
    msg->spRef = post;

    EXPECT_CALL(*M_Buffer, data()).WillRepeatedly(Throw(std::exception()));
    EXPECT_CALL(*M_Buffer, size()).WillRepeatedly(Return(0));

    testObj->doHandlePostFeatureStatusChanged(msg);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged function
 *   @classID ApplicationManagerAdapter
 *   @methodID doHandlePostFeatureStatusChanged
 *   @paramList const sp<sl::Message>& msg
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged_TC003
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged_TC003){
    sp<sl::Message> msg = sp<sl::Message>( new sl::Message() );
    sp<Post> post = sp<Post>( new Post() );

    post->arg1 = FeatureStatus::OFF;
    msg->spRef = post;
    testObj->mWaitingList[""] = "";

    EXPECT_CALL(*M_Buffer, data()).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(*M_Buffer, size()).WillRepeatedly(Return(0));

    testObj->doHandlePostFeatureStatusChanged(msg);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged function
 *   @classID ApplicationManagerAdapter
 *   @methodID doHandlePostFeatureStatusChanged
 *   @paramList const sp<sl::Message>& msg
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged_TC004
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged_TC004){
    sp<sl::Message> msg = sp<sl::Message>( new sl::Message() );
    sp<Post> post = sp<Post>( new Post() );

    post->arg1 = FeatureStatus::OFF;
    msg->spRef = post;
    testObj->mWaitingList[""] = "2";

    EXPECT_CALL(*M_Buffer, data()).WillRepeatedly(Return(""));
    EXPECT_CALL(*M_Buffer, size()).WillRepeatedly(Return(1));

    testObj->doHandlePostFeatureStatusChanged(msg);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged function
 *   @classID ApplicationManagerAdapter
 *   @methodID doHandlePostFeatureStatusChanged
 *   @paramList const sp<sl::Message>& msg
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged_TC005
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged_TC005){
    sp<sl::Message> msg = sp<sl::Message>( new sl::Message() );
    sp<Post> post = sp<Post>( new Post() );

    post->arg1 = FeatureStatus::OFF;
    msg->spRef = post;
    testObj->mWaitingList["1"] = "2";
    testObj->mWaitingFeature = "1";
    testObj->mIsWaitingFeatureLaunched = true;

    EXPECT_CALL(*M_Buffer, data()).WillRepeatedly(Return("2"));
    EXPECT_CALL(*M_Buffer, size()).WillRepeatedly(Return(1));

    testObj->doHandlePostFeatureStatusChanged(msg);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged function
 *   @classID ApplicationManagerAdapter
 *   @methodID doHandlePostFeatureStatusChanged
 *   @paramList const sp<sl::Message>& msg
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged_TC006
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged_TC006){
    sp<sl::Message> msg = sp<sl::Message>( new sl::Message() );
    sp<Post> post = sp<Post>( new Post() );

    post->arg1 = FeatureStatus::OFF;
    msg->spRef = post;
    testObj->mWaitingList["1"] = "2";
    testObj->mWaitingFeature = "1";
    testObj->mIsWaitingFeatureLaunched = false;

    EXPECT_CALL(*M_Buffer, data()).WillRepeatedly(Return("2"));
    EXPECT_CALL(*M_Buffer, size()).WillRepeatedly(Return(1));
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).WillRepeatedly(Return(new sl::Message()));
    EXPECT_CALL(*M_ECallAppHandler, handleMessageInternal(_)).WillRepeatedly(Return());

    testObj->doHandlePostFeatureStatusChanged(msg);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged function
 *   @classID ApplicationManagerAdapter
 *   @methodID doHandlePostFeatureStatusChanged
 *   @paramList const sp<sl::Message>& msg
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged_TC007
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_doHandlePostFeatureStatusChanged_TC007){
    sp<sl::Message> msg = sp<sl::Message>( new sl::Message() );
    sp<Post> post = sp<Post>( new Post() );

    post->arg1 = FeatureStatus::OFF;
    msg->spRef = post;
    testObj->mWaitingList["1"] = "2";
    testObj->mWaitingFeature = "";
    testObj->mIsWaitingFeatureLaunched = true;

    EXPECT_CALL(*M_Buffer, data()).WillRepeatedly(Return("2"));
    EXPECT_CALL(*M_Buffer, size()).WillRepeatedly(Return(1));
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).WillRepeatedly(Return(new sl::Message()));
    EXPECT_CALL(*M_ECallAppHandler, handleMessageInternal(_)).WillRepeatedly(Return());

    testObj->doHandlePostFeatureStatusChanged(msg);
}
#endif // TC_doHandlePostFeatureStatusChanged



#ifdef TC_handleQueryActionForFeatureTimerTimeOut
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_handleQueryActionForFeatureTimerTimeOut function
 *   @classID ApplicationManagerAdapter
 *   @methodID handleQueryActionForFeatureTimerTimeOut
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_handleQueryActionForFeatureTimerTimeOut_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_handleQueryActionForFeatureTimerTimeOut_TC001){
    testObj->mIsWaitingFeatureLaunched = true;
    testObj->handleQueryActionForFeatureTimerTimeOut();
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_handleQueryActionForFeatureTimerTimeOut function
 *   @classID ApplicationManagerAdapter
 *   @methodID handleQueryActionForFeatureTimerTimeOut
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_handleQueryActionForFeatureTimerTimeOut_TC002
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_handleQueryActionForFeatureTimerTimeOut_TC002){
    testObj->mIsWaitingFeatureLaunched = false;

    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).WillOnce(Return(new sl::Message()));

    testObj->handleQueryActionForFeatureTimerTimeOut();
}
#endif // TC_handleQueryActionForFeatureTimerTimeOut



#ifdef TC_getService
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_getService function
 *   @classID ApplicationManagerAdapter
 *   @methodID getService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_getService_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_getService_TC001){
    testObj->mAppManager = nullptr;

    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).WillOnce(Return(new ITest_ServiceManager()));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).WillOnce(Return(new MockBinder()));

    testObj->getService();
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_getService function
 *   @classID ApplicationManagerAdapter
 *   @methodID getService
 *   @paramList None
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_getService_TC002
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_getService_TC002){
    testObj->mAppManager = interface_cast<IApplicationManagerService> (new MockBinder());
    testObj->getService();
}
#endif // TC_getService



#ifdef TC_onBinderDied
/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_onBinderDied function
 *   @classID ApplicationManagerAdapter
 *   @methodID onBinderDied
 *   @paramList const android::wp<android::IBinder>& who
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_onBinderDied_TC001
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_onBinderDied_TC001){
    EXPECT_CALL(*M_ECallAppHandler, sendMessageDelayed(_,_)).WillOnce(Return(true));
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_)).WillOnce(Return(new sl::Message()));

    testObj->onBinderDied(nullptr);
}


/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_sendPostACNUS_ON_OFF function
 *  @classID ApplicationManagerAdapter
 *  @methodID sendPostACNUS_ON_OFF
 *  @paramList const bool onOff
 *  @priority P2
 *  @resolution_Method Equivalence Partitioning
 *  @test_condition None
 *  @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID ApplicationManagerAdapterTest_sendPostACNUS_ON_OFF_TC001
 * @test_type functionality
 * @test_objective
 * @test_precon None
 * @test_input None
 * @test_expected_result
 * @test_module
 * @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_sendPostACNUS_ON_OFF_TC001){
    testObj->mAppManager = new Test_IApplicationManagerService();
    testObj->sendPostACNUS_ON_OFF(true);
}

/**
 *   @brief This is a test script for the ApplicationManagerAdapterTest_sendPostACNUS_ON_OFF function
 *   @classID ApplicationManagerAdapter
 *   @methodID sendPostACNUS_ON_OFF
 *   @paramList const bool onOff
 *   @priority P2
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition None
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID ApplicationManagerAdapterTest_sendPostACNUS_ON_OFF_TC002
 *   @test_type functionality
 *   @test_objective
 *   @test_precon None
 *   @test_input None
 *   @test_expected_result
 *   @test_module
 *   @design_id
 */
TEST_F(ApplicationManagerAdapterTest, ApplicationManagerAdapterTest_sendPostACNUS_ON_OFF_TC002){
    EXPECT_CALL(*M_MockAndroidService, defaultServiceManager()).WillOnce(Return(new ITest_ServiceManager()));
    EXPECT_CALL(*M_MockAndroidService, getService(_)).WillOnce(Return(nullptr));
    testObj->sendPostACNUS_ON_OFF(false);
}
#endif // TC_onBinderDied
