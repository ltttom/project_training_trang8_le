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
// overwrite external header file

// overwrite internal header file
#include "tsp_mock/header_mock/logger_Base.h"
#include "tsp_mock/header_mock/ApplicationManagerAdapter_Base.h"
// #include "tsp_mock/header_mock/NAECallProcessor_Base.h"
#include "variant/honda/processors/NAECallProcessor.cpp"

using namespace ecallapp;
using namespace sl;
#include "tsp_mock/service_adapter/AlarmManagerAdapter_mock.h"
#include "tsp_mock/service_adapter/ApplicationManagerAdapter_mock.h"
#include "tsp_mock/service_adapter/AudioManagerAdapter_mock.h"
#include "tsp_mock/service_adapter/CommunicationManagerAdapter_mock.h"
#include "tsp_mock/service_adapter/ConfigurationManagerAdapter_mock.h"
#include "tsp_mock/src/CommonFunctions_mock.h"
#include "tsp_mock/src/DataBuilderUS_mock.h"
#include "tsp_mock/src/ECallAppHandler_mock.h"
#include "tsp_mock/src/ECallState_mock.h"
#include "tsp_mock/service_adapter/HMIManagerAdapter_mock.h"
#include "tsp_mock/service_adapter/HTTPSManagerAdapter_mock.h"
#include "tsp_mock/include/utils/Message_mock.h"
#include "tsp_mock/src/NAECallRecovery_mock.h"
#include "tsp_mock/service_adapter/PowerManagerAdapter_mock.h"
#include "tsp_mock/service_adapter/SystemPropertyAdapter_mock.h"
#include "tsp_mock/service_adapter/TelephoneManagerAdapter_mock.h"
#include "tsp_mock/include/utils/Timer_mock.h"
#include "tsp_mock/service_adapter/TimeManagerAdapter_mock.h"
#include "tsp_mock/service_adapter/VehicleManagerAdapter_mock.h"
#include "tsp_mock/src/NAECallStateMachine_mock.h"
#include "tsp_mock/src/NAECallStates_mock.h"
#include "tsp_mock/src/ECallVariableRepo_mock.h"

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
using ::testing::SetArgReferee;


class NAECallProcessor_Test: public NAECallProcessor {
public:
    NAECallProcessor_Test(): NAECallProcessor() {
        
    }
    virtual ~NAECallProcessor_Test() {
    }

    MOCK_METHOD1(launchRecoveredECall, void(std::string strFeatureName));
    MOCK_METHOD1(doActionsAtLaunchingTime, void(bool isRecover));
    MOCK_METHOD0(doPreparingMSD, void());
    MOCK_METHOD0(doMSDTransmission, void());
    MOCK_METHOD0(doMSDSuccess, void());
    MOCK_METHOD0(doDataRetryStrategy, void());
    MOCK_METHOD0(doVoiceCallRetryStrategy, void());
    MOCK_METHOD0(doCallConnecting, void());
    MOCK_METHOD0(doCallDialling, void());
    MOCK_METHOD0(doECallConnected, void());
    MOCK_METHOD0(transitsToCBWTMode, void());
    MOCK_METHOD0(transitsToCBWTModeAgain, void());
    MOCK_METHOD0(doECallTermination, void());
    MOCK_METHOD2(playLedPatternInMs, void(const int32_t ledPartten, const int32_t timeMs));
    MOCK_METHOD0(doHandlePowerStateChanged, void());
    MOCK_METHOD1(doDESSECallTrigger, void(bool isAfterRecovery));
    MOCK_METHOD0(doDESSECallCancel, void());
    MOCK_METHOD0(doResetECall, void());
    MOCK_METHOD2(doHandleInternalAppMsg, void(const uint32_t& what, const android::sp<sl::Message>& msgData));
    MOCK_METHOD0(checkIgAccCondition, bool());
    MOCK_METHOD0(doOnTimeReceived, void());
    void test_doResetECall() {
        NAECallProcessor::doResetECall();
    }
    void test_doDataRetryStrategy() {
        NAECallProcessor::doDataRetryStrategy();
    }
    void test_doVoiceCallRetryStrategy() {
        NAECallProcessor::doVoiceCallRetryStrategy();
    }
};
class NAECallProcessorTest : public ::testing::Test {
protected:
    std::unique_ptr<NAECallProcessor> testObj;

    NAECallProcessorTest() {
    }

    virtual ~NAECallProcessorTest() {
    }

    virtual void SetUp() {
        mockGetInstance = false;
        testObj = std::make_unique<NAECallProcessor>();
        M_Timer = new MockTimer();
        M_ECallState = new ECallStateMock();
        M_ECallAppHandler = new MockECallAppHandler(mLooper, *new IECallAppIfMock());
        M_SystemPropertyAdapter = new MockSystemPropertyAdapter();
        M_PowerManagerAdapter = new MockPowerManagerAdapter();
        M_ApplicationManagerAdapter = new MockApplicationManagerAdapter();
        M_VehicleManagerAdapter = new MockVehicleManagerAdapter();
        M_TelephoneManagerAdapter = new MockTelephoneManagerAdapter();
        M_HMIManagerAdapter = new MockHMIManagerAdapter();
        M_AlarmManagerAdapter = new MockAlarmManagerAdapter();
        M_HTTPSManagerAdapter = new MockHTTPSManagerAdapter();
        M_ConfigurationManagerAdapter = new MockConfigurationManagerAdapter();
        M_CommonFunctions = new MockCommonFunctions();
        M_DataBuilderUS = new MockDataBuilderUS();
        M_CommunicationManagerAdapter = new MockCommunicationManagerAdapter();
        M_AudioManagerAdapter = new AudioManagerAdapterMock();
        M_NAECallRecovery = new MockNAECallRecovery();
        M_TimeManagerAdapter = new MockTimeManagerAdapter();
        M_Message = new MockMessage();
        M_NAECallStateMachine = new MockNAECallStateMachine(testObj.get());
        // M_ECallStateBase = new MockECallStateBase()
        M_ECallVariableRepo = new MockECallVariableRepo();
    }

    virtual void TearDown() {
        testObj = nullptr;
        delete M_Timer;
        delete M_ECallState;
        delete M_ECallAppHandler;
        delete M_SystemPropertyAdapter;
        delete M_PowerManagerAdapter;
        delete M_ApplicationManagerAdapter;
        delete M_AudioManagerAdapter;
        delete M_VehicleManagerAdapter;
        delete M_TelephoneManagerAdapter;
        delete M_HMIManagerAdapter;
        delete M_AlarmManagerAdapter;
        delete M_HTTPSManagerAdapter;
        delete M_ConfigurationManagerAdapter;
        delete M_CommonFunctions;
        delete M_DataBuilderUS;
        delete M_CommunicationManagerAdapter;
        delete M_NAECallRecovery;
        delete M_TimeManagerAdapter;
        delete M_Message;
        delete M_NAECallStateMachine;
        delete M_ECallStateBase;
        delete M_ECallVariableRepo;
    }
};
 
/**
 * @brief Test case for the initialize method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID initialize
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_initialize_TC001
 * @test_type functionality
 * @test_objective Verify the initialization process
 * @test_precon None
 * @test_input None
 * @test_expected_result Timer is set with correct duration
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_initialize_TC001){
    EXPECT_CALL(*M_Timer, setDurationMs(_, _)).Times(3);
    testObj->initialize();
}

/**
 * @brief Test case for the doNotifySOSRequest method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doNotifySOSRequest
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doNotifySOSRequest_TC001
 * @test_type functionality
 * @test_objective Verify SOS request notification handling
 * @test_precon None
 * @test_input None
 * @test_expected_result Message is handled internally
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doNotifySOSRequest_TC001){
    EXPECT_CALL(*M_ECallAppHandler, handleMessageInternal(_)).Times(1);
    testObj->doNotifySOSRequest();
}

/**
 * @brief Test case for the doNotifySOSNoRequest method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doNotifySOSNoRequest
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doNotifySOSNoRequest_TC001
 * @test_type functionality
 * @test_objective Verify SOS no request notification handling
 * @test_precon None
 * @test_input None
 * @test_expected_result Message is handled internally
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doNotifySOSNoRequest_TC001){
    EXPECT_CALL(*M_ECallAppHandler, handleMessageInternal(_)).Times(1);
    testObj->doNotifySOSNoRequest();
}

/**
 * @brief Test case for the doNotifySOSCancel method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doNotifySOSCancel
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doNotifySOSCancel_TC001
 * @test_type functionality
 * @test_objective Verify SOS cancel notification handling
 * @test_precon None
 * @test_input None
 * @test_expected_result Message is handled internally
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doNotifySOSCancel_TC001){
    EXPECT_CALL(*M_ECallAppHandler, handleMessageInternal(_)).Times(1);
    testObj->doNotifySOSCancel();
}

/**
 * @brief Test case for the doActionsAtLaunchingTime method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doActionsAtLaunchingTime
 * @paramList isRecover
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doActionsAtLaunchingTime_TC001
 * @test_type functionality
 * @test_objective Verify actions performed during launching time
 * @test_precon None
 * @test_input isRecover = false
 * @test_expected_result Mute audio, acquire power lock, and request LED pattern
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doActionsAtLaunchingTime_TC001){
    EXPECT_CALL(*M_AudioManagerAdapter, setMute(true)).Times(1);
    EXPECT_CALL(*M_PowerManagerAdapter, acquirePowerLock()).Times(1);
    EXPECT_CALL(*M_AudioManagerAdapter, isPlaying()).WillOnce(Return(false));
    EXPECT_CALL(*M_HMIManagerAdapter, requestLedPattern(_, _)).Times(1);
    EXPECT_CALL(*M_ECallVariableRepo, getECallType()).Times(::testing::AnyNumber()).WillRepeatedly(Return(ecall_type::SOS));
    testObj->doActionsAtLaunchingTime(false);
}

/**
 * @brief Test case for the doActionsAtLaunchingTime method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doActionsAtLaunchingTime
 * @paramList isRecover
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doActionsAtLaunchingTime_TC002
 * @test_type functionality
 * @test_objective Verify actions performed during launching time
 * @test_precon None
 * @test_input isRecover = true
 * @test_expected_result Mute audio, acquire power lock
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doActionsAtLaunchingTime_TC002){
    EXPECT_CALL(*M_AudioManagerAdapter, setMute(true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(*M_PowerManagerAdapter, acquirePowerLock()).Times(1);
    testObj->doActionsAtLaunchingTime(true);
}

/**
 * @brief Test case for the doActionsAtLaunchingTime method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doActionsAtLaunchingTime
 * @paramList isRecover
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doActionsAtLaunchingTime_TC003
 * @test_type functionality
 * @test_objective Verify actions performed during launching time
 * @test_precon None
 * @test_input isRecover = false
 * @test_expected_result Mute audio, acquire power lock, and request LED pattern
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doActionsAtLaunchingTime_TC003){
    EXPECT_CALL(*M_AudioManagerAdapter, setMute(true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(*M_PowerManagerAdapter, acquirePowerLock()).Times(1);
    EXPECT_CALL(*M_AudioManagerAdapter, isPlaying()).WillOnce(Return(true));
    EXPECT_CALL(*M_HMIManagerAdapter, requestLedPattern(_, _)).Times(1);
    testObj->doActionsAtLaunchingTime(false);
}

/**
 * @brief Test case for the launchFeature method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID launchFeature
 * @paramList featureName
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_launchFeature_TC001
 * @test_type functionality
 * @test_objective Verify feature launching
 * @test_precon None
 * @test_input featureName = FEATURE_NAMES::HONDA_ACN_NA
 * @test_expected_result Feature is launched
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_launchFeature_TC001){
    testObj->launchFeature(FEATURE_NAMES::HONDA_ACN_NA);
}

/**
 * @brief Test case for the ignoreFeature method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID ignoreFeature
 * @paramList featureName
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_ignoreFeature_TC001
 * @test_type functionality
 * @test_objective Verify feature ignoring
 * @test_precon None
 * @test_input featureName = FEATURE_NAMES::HONDA_ACN_NA
 * @test_expected_result Feature is ignored
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_ignoreFeature_TC001){
    testObj->ignoreFeature(FEATURE_NAMES::HONDA_ACN_NA);
}

/**
 * @brief Test case for the terminateFeature method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID terminateFeature
 * @paramList featureName
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_terminateFeature_TC001
 * @test_type functionality
 * @test_objective Verify feature termination
 * @test_precon None
 * @test_input featureName = FEATURE_NAMES::HONDA_SOS_NA_CBWT
 * @test_expected_result Feature is terminated
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_terminateFeature_TC001){
    testObj->terminateFeature(FEATURE_NAMES::HONDA_SOS_NA_CBWT);
}

/**
 * @brief Test case for the doPreparingMSD method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doPreparingMSD
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doPreparingMSD_TC001
 * @test_type functionality
 * @test_objective Verify MSD preparation
 * @test_precon None
 * @test_input None
 * @test_expected_result MSD is prepared and saved
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doPreparingMSD_TC001){
    DataBuilderUS builder;
    EXPECT_CALL(*M_DataBuilderUS, setCDS(true)).Times(1).WillOnce(ReturnRef(builder));
    EXPECT_CALL(*M_DataBuilderUS, setGPS()).Times(1).WillOnce(ReturnRef(builder));
    EXPECT_CALL(*M_DataBuilderUS, setUtil(_)).Times(1).WillOnce(ReturnRef(builder));
    EXPECT_CALL(*M_DataBuilderUS, setTriggerType(_,_)).Times(1).WillOnce(ReturnRef(builder));
    EXPECT_CALL(*M_DataBuilderUS, build()).WillOnce(Return(""));
    EXPECT_CALL(*M_PowerManagerAdapter, getIGStatus()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*M_PowerManagerAdapter, getACCStatus()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*M_DataBuilderUS, getIgState(0, 0)).WillOnce(Return(""));
    EXPECT_CALL(*M_HMIManagerAdapter, requestLedPattern(_, _)).Times(1);
    EXPECT_CALL(*M_AudioManagerAdapter, isPlaying()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(*M_AudioManagerAdapter, playAudioFile("/audio/tone_2.raw")).Times(1).WillOnce(Return(false));
    bool expectVal = true;
    EXPECT_CALL(*M_CommonFunctions, saveTSPMSD(_, _)).WillOnce(SetArgReferee<1>(expectVal));
    testObj->doPreparingMSD();
}

/**
 * @brief Test case for the doPreparingMSD method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doPreparingMSD
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doPreparingMSD_TC002
 * @test_type functionality
 * @test_objective Verify MSD preparation
 * @test_precon None
 * @test_input None
 * @test_expected_result MSD is prepared and saved
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doPreparingMSD_TC002){
    DataBuilderUS builder;
    EXPECT_CALL(*M_DataBuilderUS, setCDS(true)).Times(1).WillOnce(ReturnRef(builder));
    EXPECT_CALL(*M_DataBuilderUS, setGPS()).Times(1).WillOnce(ReturnRef(builder));
    EXPECT_CALL(*M_DataBuilderUS, setUtil(_)).Times(1).WillOnce(ReturnRef(builder));
    EXPECT_CALL(*M_DataBuilderUS, setTriggerType(_,_)).Times(1).WillOnce(ReturnRef(builder));
    EXPECT_CALL(*M_DataBuilderUS, build()).WillOnce(Return(""));
    EXPECT_CALL(*M_PowerManagerAdapter, getIGStatus()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*M_PowerManagerAdapter, getACCStatus()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*M_DataBuilderUS, getIgState(0, 0)).WillOnce(Return(""));
    EXPECT_CALL(*M_HMIManagerAdapter, requestLedPattern(_, _)).Times(1);
    EXPECT_CALL(*M_AudioManagerAdapter, isPlaying()).Times(1).WillOnce(Return(true));
    bool expectVal = false;
    EXPECT_CALL(*M_CommonFunctions, saveTSPMSD(_, _)).WillOnce(SetArgReferee<1>(expectVal));
    testObj->doPreparingMSD();
}

/**
 * @brief Test case for the doPreparingMSD method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doPreparingMSD
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doPreparingMSD_TC003
 * @test_type functionality
 * @test_objective Verify MSD preparation
 * @test_precon None
 * @test_input None
 * @test_expected_result MSD is prepared and saved
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doPreparingMSD_TC003){
    DataBuilderUS builder;
    EXPECT_CALL(*M_PowerManagerAdapter, getIGStatus()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*M_PowerManagerAdapter, getACCStatus()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*M_DataBuilderUS, getIgState(0, 0)).WillOnce(Return(""));
    EXPECT_CALL(*M_HMIManagerAdapter, requestLedPattern(_, _)).Times(1);
    EXPECT_CALL(*M_AudioManagerAdapter, isPlaying()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*M_DataBuilderUS, setCDS(true)).Times(1).WillOnce(ReturnRef(builder));
    EXPECT_CALL(*M_DataBuilderUS, setGPS()).Times(1).WillOnce(ReturnRef(builder));
    EXPECT_CALL(*M_DataBuilderUS, setUtil(_)).Times(1).WillOnce(ReturnRef(builder));
    EXPECT_CALL(*M_DataBuilderUS, setTriggerType(_,_)).Times(1).WillOnce(ReturnRef(builder));
    EXPECT_CALL(*M_DataBuilderUS, build()).WillOnce(Return(""));
    testObj->doPreparingMSD();
}

/**
 * @brief Test case for the doMSDTransmission method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doMSDTransmission
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doMSDTransmission_TC001
 * @test_type functionality
 * @test_objective Verify MSD transmission
 * @test_precon None
 * @test_input None
 * @test_expected_result MSD is transmitted
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doMSDTransmission_TC001){
    testObj->mTimerHandler = std::make_unique<ECallTimerHandler>();
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_DATA_TRANSMISSION_SKIP, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_ENABLE));
    EXPECT_CALL(*M_HMIManagerAdapter, turnOffCurrentLed()).Times(1);
    EXPECT_CALL(*M_HMIManagerAdapter, requestLedPattern(_, _)).Times(1);
    EXPECT_CALL(*M_AudioManagerAdapter, isPlaying()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(*M_AudioManagerAdapter, playAudioFile("/audio/tone_1.raw")).Times(1).WillOnce(Return(false));
    testObj->doMSDTransmission();
}

/**
 * @brief Test case for the doMSDTransmission method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doMSDTransmission
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doMSDTransmission_TC002
 * @test_type functionality
 * @test_objective Verify MSD transmission
 * @test_precon None
 * @test_input None
 * @test_expected_result MSD is transmitted
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doMSDTransmission_TC002){
    testObj->mTimerHandler = std::make_unique<ECallTimerHandler>();
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_DATA_TRANSMISSION_SKIP, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_DISABLE));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_SERVER_ULR_ENABLE_NAME, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_ENABLE));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_SERVER_ULR_NAME, DataFrom::General)).Times(1).WillOnce(Return("url"));
    EXPECT_CALL(*M_HTTPSManagerAdapter, sendRequestAsync2("url", "", false, "POST")).Times(1);
    testObj->doMSDTransmission();
}

/**
 * @brief Test case for the doMSDTransmission method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doMSDTransmission
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doMSDTransmission_TC003
 * @test_type functionality
 * @test_objective Verify MSD transmission
 * @test_precon None
 * @test_input None
 * @test_expected_result MSD is transmitted
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doMSDTransmission_TC003){
    testObj->mTimerHandler = std::make_unique<ECallTimerHandler>();
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_DATA_TRANSMISSION_SKIP, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_DISABLE));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_SERVER_ULR_ENABLE_NAME, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_DISABLE));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_TEST_SENDING_FUNCTION_NAME, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_ENABLE));
    EXPECT_CALL(*M_HTTPSManagerAdapter, sendRequestAsync1("http://hondatcusf.us-e2.cloudhub.io/v1/emergency-notification", "", "POST")).Times(1);
    testObj->doMSDTransmission();
}

/**
 * @brief Test case for the doMSDTransmission method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doMSDTransmission
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doMSDTransmission_TC004
 * @test_type functionality
 * @test_objective Verify MSD transmission
 * @test_precon None
 * @test_input None
 * @test_expected_result MSD is transmitted
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doMSDTransmission_TC004){
    testObj->mTimerHandler = std::make_unique<ECallTimerHandler>();
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_DATA_TRANSMISSION_SKIP, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_DISABLE));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_SERVER_ULR_ENABLE_NAME, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_DISABLE));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_TEST_SENDING_FUNCTION_NAME, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_DISABLE));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_SERVER_REAL_ENABLE_NAME, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_ENABLE));
    EXPECT_CALL(*M_HTTPSManagerAdapter, sendRequestAsync2("http://hondatcusf.us-e2.cloudhub.io/v1/emergency-notification", "", true, "POST")).Times(1);
    testObj->doMSDTransmission();
}

/**
 * @brief Test case for the doMSDTransmission method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doMSDTransmission
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doMSDTransmission_TC005
 * @test_type functionality
 * @test_objective Verify MSD transmission
 * @test_precon None
 * @test_input None
 * @test_expected_result MSD is transmitted
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doMSDTransmission_TC005){
    testObj->mTimerHandler = std::make_unique<ECallTimerHandler>();
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_DATA_TRANSMISSION_SKIP, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_DISABLE));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_SERVER_ULR_ENABLE_NAME, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_DISABLE));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_TEST_SENDING_FUNCTION_NAME, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_DISABLE));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_SERVER_REAL_ENABLE_NAME, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_DISABLE));
    EXPECT_CALL(*M_HTTPSManagerAdapter, sendRequestAsync2("http://hondatcusf.us-e2.cloudhub.io/v1/emergency-notification", "", false, "POST")).Times(1);
    testObj->doMSDTransmission();
}

/**
 * @brief Test case for the doMSDSuccess method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doMSDSuccess
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doMSDSuccess_TC001
 * @test_type functionality
 * @test_objective Verify MSD success handling
 * @test_precon None
 * @test_input None
 * @test_expected_result LED pattern is requested, audio is stopped
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doMSDSuccess_TC001){
    EXPECT_CALL(*M_HMIManagerAdapter, turnOffCurrentLed()).Times(1);
    EXPECT_CALL(*M_HMIManagerAdapter, requestLedPattern(_, _)).Times(1);
    EXPECT_CALL(*M_AudioManagerAdapter, isPlaying()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*M_AudioManagerAdapter, stopAudioFile()).Times(1);
    testObj->doMSDSuccess();
}

/**
 * @brief Test case for the doDataRetryStrategy method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doDataRetryStrategy
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doDataRetryStrategy_TC001
 * @test_type functionality
 * @test_objective Verify data retry strategy
 * @test_precon None
 * @test_input None
 * @test_expected_result Data retry strategy is executed
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doDataRetryStrategy_TC001){
    testObj->doDataRetryStrategy();
}

/**
 * @brief Test case for the doDataRetryStrategy method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doDataRetryStrategy
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doDataRetryStrategy_TC003
 * @test_type functionality
 * @test_objective Verify data retry strategy
 * @test_precon None
 * @test_input None
 * @test_expected_result Data retry strategy is executed
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doDataRetryStrategy_TC003){
    testObj->mAttemptDataCount = 3;
    testObj->mMaxAttemptData = 2;
    testObj->mTimerHandler = std::make_unique<ECallTimerHandler>();
    testObj->mCallDiallingTimer = android::sp<Timer>{new Timer(testObj->mTimerHandler.get(), 1)};
    testObj->mCallRetryTimer = android::sp<Timer>{new Timer(testObj->mTimerHandler.get(), 1)};
    testObj->doDataRetryStrategy();
}

/**
 * @brief Test case for the doDataRetryStrategy method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doDataRetryStrategy
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doDataRetryStrategy_TC004
 * @test_type functionality
 * @test_objective Verify data retry strategy
 * @test_precon None
 * @test_input None
 * @test_expected_result Data retry strategy is executed
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doDataRetryStrategy_TC004){
    testObj->mAttemptDataCount = 3;
    testObj->mMaxAttemptData = -1;
    testObj->doDataRetryStrategy();
}

/**
 * @brief Test case for the doDataRetryStrategy method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doDataRetryStrategy
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doDataRetryStrategy_TC006
 * @test_type functionality
 * @test_objective Verify data retry strategy
 * @test_precon None
 * @test_input None
 * @test_expected_result Data retry strategy is executed
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doDataRetryStrategy_TC006){
    testObj->mMaxAttemptData = 3;
    testObj->mAttemptDataCount = 2;
    testObj->doDataRetryStrategy();
}

/**
 * @brief Test case for the doDataRetryStrategy method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doDataRetryStrategy
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doDataRetryStrategy_TC007
 * @test_type functionality
 * @test_objective Verify data retry strategy
 * @test_precon None
 * @test_input None
 * @test_expected_result Data retry strategy is executed
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doDataRetryStrategy_TC007){
    testObj->mAttemptDataCount = 3;
    testObj->mMaxAttemptData = -1;
    testObj->doDataRetryStrategy();
}

/**
 * @brief Test case for the doDataRetryStrategy method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doDataRetryStrategy
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doDataRetryStrategy_TC008
 * @test_type functionality
 * @test_objective Verify data retry strategy
 * @test_precon None
 * @test_input None
 * @test_expected_result Data retry strategy is executed
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doDataRetryStrategy_TC008){
    testObj->mAttemptDataCount = -1;
    testObj->doDataRetryStrategy();
}

/**
 * @brief Test case for the doDataRetryStrategy method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doDataRetryStrategy
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doDataRetryStrategy_TC009
 * @test_type functionality
 * @test_objective Verify data retry strategy
 * @test_precon None
 * @test_input None
 * @test_expected_result Data retry strategy is executed
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doDataRetryStrategy_TC009){
    EXPECT_CALL(*M_ECallVariableRepo, getECallType()).Times(::testing::AnyNumber()).WillRepeatedly(Return(ecall_type::SOS));
    testObj->mAttemptDataCount = 4;
    testObj->mMaxAttemptData = 3;
    testObj->doDataRetryStrategy();
}

/**
 * @brief Test case for the doDataRetryStrategy method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doDataRetryStrategy
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doDataRetryStrategy_TC010
 * @test_type functionality
 * @test_objective Verify data retry strategy
 * @test_precon None
 * @test_input None
 * @test_expected_result Data retry strategy is executed
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doDataRetryStrategy_TC010){
    testObj->mAttemptDataCount = 3;
    testObj->mMaxAttemptData = 3;
    testObj->doDataRetryStrategy();
}

/**
 * @brief Test case for the doVoiceCallRetryStrategy method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doVoiceCallRetryStrategy
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doVoiceCallRetryStrategy_TC001
 * @test_type functionality
 * @test_objective Verify voice call retry strategy
 * @test_precon None
 * @test_input None
 * @test_expected_result Voice call retry strategy is executed
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doVoiceCallRetryStrategy_TC001) {
    testObj->mMaxAttemptVoice = -1;
    testObj->mAttemptVoiceCount = 3;
    testObj->doVoiceCallRetryStrategy();
}

/**
 * @brief Test case for the doVoiceCallRetryStrategy method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doVoiceCallRetryStrategy
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doVoiceCallRetryStrategy_TC002
 * @test_type functionality
 * @test_objective Verify voice call retry strategy
 * @test_precon None
 * @test_input None
 * @test_expected_result Voice call retry strategy is executed
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doVoiceCallRetryStrategy_TC002) {
    testObj->mMaxAttemptVoice = 3;
    testObj->mAttemptVoiceCount = 1;
    testObj->doVoiceCallRetryStrategy();
}

/**
 * @brief Test case for the doVoiceCallRetryStrategy method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doVoiceCallRetryStrategy
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID NAECallProcessorTest_doVoiceCallRetryStrategy_TC003
 * @test_type functionality
 * @test_objective Verify voice call retry strategy
 * @test_precon None
 * @test_input None
 * @test_expected_result Voice call retry strategy is executed
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doVoiceCallRetryStrategy_TC003) {
    testObj->mMaxAttemptVoice = 3;
    testObj->mAttemptVoiceCount = 3;
    testObj->doVoiceCallRetryStrategy();
}

/**
 * @brief Test case for the doCallDialling method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doCallDialling
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doCallDialling_TC001
 * @test_type functionality
 * @test_objective Verify call dialling
 * @test_precon None
 * @test_input None
 * @test_expected_result Call is dialled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doCallDialling_TC001) {
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_PHONE_NUMBER_ENABLE_NAME, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_ENABLE));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig("PHONE_NUMBER", DataFrom::General)).Times(1).WillOnce(Return("1234567890"));
    testObj->doCallDialling();
}

/**
 * @brief Test case for the doCallDialling method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doCallDialling
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doCallDialling_TC002
 * @test_type functionality
 * @test_objective Verify call dialling
 * @test_precon None
 * @test_input None
 * @test_expected_result Call is dialled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doCallDialling_TC002) {
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_PHONE_NUMBER_ENABLE_NAME, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_DISABLE));
    EXPECT_CALL(*M_HTTPSManagerAdapter, getTelNo()).Times(1).WillOnce(Return("1234567890"));
    testObj->doCallDialling();
}

/**
 * @brief Test case for the doCallDialling method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doCallDialling
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doCallDialling_TC003
 * @test_type functionality
 * @test_objective Verify call dialling
 * @test_precon None
 * @test_input None
 * @test_expected_result Call is dialled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doCallDialling_TC003) {
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_PHONE_NUMBER_ENABLE_NAME, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_DISABLE));
    EXPECT_CALL(*M_HTTPSManagerAdapter, getTelNo()).Times(1).WillOnce(Return(""));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_PHONE_NUMBER_NAME, DataFrom::General)).Times(1).WillOnce(Return("1234567890"));
    testObj->doCallDialling();
}

/**
 * @brief Test case for the setCallBackWaitingTimer method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID setCallBackWaitingTimer
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_setCallBackWaitingTimer_TC001
 * @test_type functionality
 * @test_objective Verify callback waiting timer setting
 * @test_precon None
 * @test_input None
 * @test_expected_result Callback waiting timer is set
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_setCallBackWaitingTimer_TC001) {
    EXPECT_CALL(*M_NAECallRecovery, getRemainingTime(ecalltimer::CBWT)).Times(1).WillOnce(Return(100));
    testObj->setCallBackWaitingTimer();
}

/**
 * @brief Test case for the setCallBackWaitingTimer method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID setCallBackWaitingTimer
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_setCallBackWaitingTimer_TC002
 * @test_type functionality
 * @test_objective Verify callback waiting timer setting
 * @test_precon None
 * @test_input None
 * @test_expected_result Callback waiting timer is set
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_setCallBackWaitingTimer_TC002) {
    EXPECT_CALL(*M_NAECallRecovery, getRemainingTime(ecalltimer::CBWT)).Times(1).WillOnce(Return(0));
    testObj->setCallBackWaitingTimer();
}

/**
 * @brief Test case for the setCallBackWaitingTimer method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID setCallBackWaitingTimer
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_setCallBackWaitingTimer_TC003
 * @test_type functionality
 * @test_objective Verify callback waiting timer setting
 * @test_precon None
 * @test_input None
 * @test_expected_result Callback waiting timer is set
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_setCallBackWaitingTimer_TC003) {
    EXPECT_CALL(*M_NAECallRecovery, getRemainingTime(ecalltimer::CBWT)).Times(1).WillOnce(Return(0));
    mockGetInstance = true;
    EXPECT_CALL(*M_ECallAppHandler, getInstance()).Times(1).WillOnce(Return(nullptr));
    testObj->setCallBackWaitingTimer();
}

/**
 * @brief Test case for the setCallBackWaitingTimer method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID setCallBackWaitingTimer
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_setCallBackWaitingTimer_TC004
 * @test_type functionality
 * @test_objective Verify callback waiting timer setting
 * @test_precon None
 * @test_input None
 * @test_expected_result Callback waiting timer is set
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_setCallBackWaitingTimer_TC004) {
    EXPECT_CALL(*M_NAECallRecovery, getRemainingTime(ecalltimer::CBWT)).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_CBWT_TIME_SHORTEN_ENABLE_NAME, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_ENABLE));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfigIntValue("CBWT_TIME_NAME")).Times(1).WillOnce(Return(100));
    testObj->setCallBackWaitingTimer();
}

/**
 * @brief Test case for the setCallBackWaitingTimer method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID setCallBackWaitingTimer
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_setCallBackWaitingTimer_TC005
 * @test_type functionality
 * @test_objective Verify callback waiting timer setting
 * @test_precon None
 * @test_input None
 * @test_expected_result Callback waiting timer is set
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_setCallBackWaitingTimer_TC005) {
    EXPECT_CALL(*M_NAECallRecovery, getRemainingTime(ecalltimer::CBWT)).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(*M_ConfigurationManagerAdapter, getConfig(ecall_config::CF_CBWT_TIME_SHORTEN_ENABLE_NAME, DataFrom::General)).Times(1).WillOnce(Return(ecall_config::CONFIG_DISABLE));
    testObj->setCallBackWaitingTimer();
}

/**
 * @brief Test case for the transitsToCBWTMode method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID transitsToCBWTMode
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_transitsToCBWTMode_TC001
 * @test_type functionality
 * @test_objective Verify transition to CBWT mode
 * @test_precon None
 * @test_input None
 * @test_expected_result Transition to CBWT mode is performed
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_transitsToCBWTMode_TC001) {
    testObj->transitsToCBWTMode();
}

/**
 * @brief Test case for the doECallTermination method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doECallTermination
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doECallTermination_TC001
 * @test_type functionality
 * @test_objective Verify eCall termination
 * @test_precon None
 * @test_input None
 * @test_expected_result eCall is terminated
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doECallTermination_TC001) {
    testObj->doECallTermination();
}

/**
 * @brief Test case for the playLedPatternInMs method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID playLedPatternInMs
 * @paramList ledPattern, timeMs
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_playLedPatternInMs_TC001
 * @test_type functionality
 * @test_objective Verify LED pattern playing
 * @test_precon None
 * @test_input ledPattern = 100, timeMs = 100
 * @test_expected_result LED pattern is played
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_playLedPatternInMs_TC001) {
    testObj->playLedPatternInMs(100, 100);
}

/**
 * @brief Test case for the doHandlePowerStateChanged method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doHandlePowerStateChanged
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doHandlePowerStateChanged_TC001
 * @test_type functionality
 * @test_objective Verify power state change handling
 * @test_precon None
 * @test_input None
 * @test_expected_result Power state change is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doHandlePowerStateChanged_TC001) {
    EXPECT_CALL(*M_NAECallStateMachine, getCurrentState()).Times(1).WillOnce(Return(ecall_state::STANDBY));
    EXPECT_CALL(*M_PowerManagerAdapter, isIgnitionOn()).Times(1).WillOnce(Return(true));
    testObj->doHandlePowerStateChanged();
}

/**
 * @brief Test case for the doHandlePowerStateChanged method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doHandlePowerStateChanged
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doHandlePowerStateChanged_TC002
 * @test_type functionality
 * @test_objective Verify power state change handling
 * @test_precon None
 * @test_input None
 * @test_expected_result Power state change is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doHandlePowerStateChanged_TC002) {
    EXPECT_CALL(*M_NAECallStateMachine, getCurrentState()).Times(1).WillOnce(Return(ecall_state::POWER_OFF));
    EXPECT_CALL(*M_PowerManagerAdapter, isIgnitionOn()).Times(1).WillOnce(Return(false));
    testObj->doHandlePowerStateChanged();
}

/**
 * @brief Test case for the doHandlePowerStateChanged method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doHandlePowerStateChanged
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doHandlePowerStateChanged_TC003
 * @test_type functionality
 * @test_objective Verify power state change handling
 * @test_precon None
 * @test_input None
 * @test_expected_result Power state change is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doHandlePowerStateChanged_TC003) {
    EXPECT_CALL(*M_NAECallStateMachine, getCurrentState()).Times(1).WillOnce(Return(ecall_state::MSD_TRANSMISSION));
    testObj->doHandlePowerStateChanged();
}

/**
 * @brief Test case for the processorHandleMessage method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID processorHandleMessage
 * @paramList what, msg
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_processorHandleMessage_TC001
 * @test_type functionality
 * @test_objective Verify message handling
 * @test_precon None
 * @test_input what = 7, msg = nullptr
 * @test_expected_result Message is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_processorHandleMessage_TC001) {
    testObj->processorHandleMessage(7, nullptr); //RECV_MSG_BOOT_COMPLETED
}

/**
 * @brief Test case for the processorHandleMessage method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID processorHandleMessage
 * @paramList what, msg
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_processorHandleMessage_TC002
 * @test_type functionality
 * @test_objective Verify message handling
 * @test_precon None
 * @test_input what = 7, msg = new sl::Message()
 * @test_expected_result Message is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_processorHandleMessage_TC002) {
    android::sp<sl::Message> msg = new sl::Message();
    testObj->processorHandleMessage(7, msg);
}

/**
 * @brief Test case for the processorHandleMessage method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID processorHandleMessage
 * @paramList what, msg
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_processorHandleMessage_TC003
 * @test_type functionality
 * @test_objective Verify message handling
 * @test_precon None
 * @test_input what = 3006, msg = new sl::Message()
 * @test_expected_result Message is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_processorHandleMessage_TC003) {
    android::sp<sl::Message> msg = new sl::Message();
    testObj->processorHandleMessage(3006, msg);
}

/**
 * @brief Test case for the processorHandleMessage method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID processorHandleMessage
 * @paramList what, msg
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_processorHandleMessage_TC004
 * @test_type functionality
 * @test_objective Verify message handling
 * @test_precon None
 * @test_input what = 6002, msg = new sl::Message()
 * @test_expected_result Message is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_processorHandleMessage_TC004) {
    android::sp<sl::Message> msg = new sl::Message();
    testObj->processorHandleMessage(6002, msg);
}

/**
 * @brief Test case for the processorHandleMessage method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID processorHandleMessage
 * @paramList what, msg
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_processorHandleMessage_TC005
 * @test_type functionality
 * @test_objective Verify message handling
 * @test_precon None
 * @test_input what = 6004, msg = new sl::Message()
 * @test_expected_result Message is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_processorHandleMessage_TC005) {
    android::sp<sl::Message> msg = new sl::Message();
    testObj->processorHandleMessage(6004, msg);
}

/**
 * @brief Test case for the processorHandleMessage method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID processorHandleMessage
 * @paramList what, msg
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_processorHandleMessage_TC006
 * @test_type functionality
 * @test_objective Verify message handling
 * @test_precon None
 * @test_input what = 5009, msg = new sl::Message()
 * @test_expected_result Message is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_processorHandleMessage_TC006) {
    android::sp<sl::Message> msg = new sl::Message();
    testObj->processorHandleMessage(5009, msg);
}

/**
 * @brief Test case for the processorHandleMessage method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID processorHandleMessage
 * @paramList what, msg
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_processorHandleMessage_TC007
 * @test_type functionality
 * @test_objective Verify message handling
 * @test_precon None
 * @test_input what = 60002, msg = new sl::Message()
 * @test_expected_result Message is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_processorHandleMessage_TC007) {
    android::sp<sl::Message> msg = new sl::Message();
    testObj->processorHandleMessage(60002, msg);
}

/**
 * @brief Test case for the processorHandleMessage method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID processorHandleMessage
 * @paramList what, msg
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_processorHandleMessage_TC008
 * @test_type functionality
 * @test_objective Verify message handling
 * @test_precon None
 * @test_input what = 60003, msg = new sl::Message()
 * @test_expected_result Message is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_processorHandleMessage_TC008) {
    android::sp<sl::Message> msg = new sl::Message();
    testObj->processorHandleMessage(60003, msg);
}

/**
 * @brief Test case for the processorHandleMessage method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID processorHandleMessage
 * @paramList what, msg
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_processorHandleMessage_TC009
 * @test_type functionality
 * @test_objective Verify message handling
 * @test_precon None
 * @test_input what = 50002, msg = new sl::Message()
 * @test_expected_result Message is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_processorHandleMessage_TC009) {
    android::sp<sl::Message> msg = new sl::Message();
    EXPECT_CALL(*M_NAECallRecovery, getCBWTFlag()).Times(1).WillOnce(Return(true));
    testObj->processorHandleMessage(50002, msg);
}

/**
 * @brief Test case for the processorHandleMessage method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID processorHandleMessage
 * @paramList what, msg
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_processorHandleMessage_TC010
 * @test_type functionality
 * @test_objective Verify message handling
 * @test_precon None
 * @test_input what = 5012, msg = new sl::Message()
 * @test_expected_result Message is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_processorHandleMessage_TC010) {
    android::sp<sl::Message> msg = new sl::Message();
    testObj->processorHandleMessage(5012, msg);
}

/**
 * @brief Test case for the processorHandleMessage method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID processorHandleMessage
 * @paramList what, msg
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_processorHandleMessage_TC011
 * @test_type functionality
 * @test_objective Verify message handling
 * @test_precon None
 * @test_input what = 5013, msg = new sl::Message()
 * @test_expected_result Message is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_processorHandleMessage_TC011) {
    android::sp<sl::Message> msg = new sl::Message();
    testObj->processorHandleMessage(5013, msg);
}

/**
 * @brief Test case for the processorHandleMessage method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID processorHandleMessage
 * @paramList what, msg
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_processorHandleMessage_TC012
 * @test_type functionality
 * @test_objective Verify message handling
 * @test_precon None
 * @test_input what = 0, msg = new sl::Message()
 * @test_expected_result Message is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_processorHandleMessage_TC012) {
    android::sp<sl::Message> msg = new sl::Message();
    testObj->processorHandleMessage(0, msg);
}

/**
 * @brief Test case for the processorHandleMessage method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID processorHandleMessage
 * @paramList what, msg
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_processorHandleMessage_TC013
 * @test_type functionality
 * @test_objective Verify message handling
 * @test_precon None
 * @test_input what = 50002, msg = new sl::Message()
 * @test_expected_result Message is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_processorHandleMessage_TC013) {
    android::sp<sl::Message> msg = new sl::Message();
    EXPECT_CALL(*M_NAECallRecovery, getCBWTFlag()).Times(1).WillOnce(Return(false));
    testObj->processorHandleMessage(50002, msg);
}

/**
 * @brief Test case for the processorHandleMessage method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID processorHandleMessage
 * @paramList what, msg
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_processorHandleMessage_TC014
 * @test_type functionality
 * @test_objective Verify message handling
 * @test_precon None
 * @test_input what = 50002, msg = new sl::Message()
 * @test_expected_result Message is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_processorHandleMessage_TC014) {
    android::sp<sl::Message> msg = new sl::Message();
    testObj->processorHandleMessage(ENUM_TO_INT(msg::MSG::APPM_REQUEST_LAUNCH), msg);
}

/**
 * @brief Test case for the onTimerTimeoutEvent method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID onTimerTimeoutEvent
 * @paramList timerId
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_onTimerTimeoutEvent_TC001
 * @test_type functionality
 * @test_objective Verify timer timeout event handling
 * @test_precon None
 * @test_input timerId = ecall_na_timer::DATA_RETRY_TIMER_ID
 * @test_expected_result Timer timeout event is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_onTimerTimeoutEvent_TC001) {
    testObj->onTimerTimeoutEvent(ecall_na_timer::DATA_RETRY_TIMER_ID);
}

/**
 * @brief Test case for the doResetECall method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doResetECall
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doResetECall_TC001
 * @test_type functionality
 * @test_objective Verify eCall reset
 * @test_precon None
 * @test_input None
 * @test_expected_result eCall is reset
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doResetECall_TC001) {
    EXPECT_CALL(*M_TelephoneManagerAdapter, getCallState()).WillOnce(Return(telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE));
    EXPECT_CALL(*M_PowerManagerAdapter, isIgnitionOn()).WillOnce(Return(false));
    testObj->doResetECall();
}

/**
 * @brief Test case for the doResetECall method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doResetECall
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doResetECall_TC002
 * @test_type functionality
 * @test_objective Verify eCall reset
 * @test_precon None
 * @test_input None
 * @test_expected_result eCall is reset
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doResetECall_TC002) {
    EXPECT_CALL(*M_TelephoneManagerAdapter, getCallState()).WillOnce(Return(telephony::TelephonyManager::CALL_STATE_RINGING));
    EXPECT_CALL(*M_PowerManagerAdapter, isIgnitionOn()).WillOnce(Return(true));
    testObj->doResetECall();
}

/**
 * @brief Test case for the doOnTimeReceived method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doOnTimeReceived
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doOnTimeReceived_TC001
 * @test_type functionality
 * @test_objective Verify time received handling
 * @test_precon None
 * @test_input None
 * @test_expected_result Time received is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doOnTimeReceived_TC001) {
    EXPECT_CALL(*M_NAECallStateMachine, getCurrentState()).Times(1).WillOnce(Return(ecall_state::STANDBY));
    testObj->doOnTimeReceived();
}

/**
 * @brief Test case for the doOnTimeReceived method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doOnTimeReceived
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doOnTimeReceived_TC002
 * @test_type functionality
 * @test_objective Verify time received handling
 * @test_precon None
 * @test_input None
 * @test_expected_result Time received is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doOnTimeReceived_TC002) {
    EXPECT_CALL(*M_NAECallStateMachine, getCurrentState()).Times(1).WillOnce(Return(ecall_state::CALLBACK_WAITING ));
    EXPECT_CALL(*M_NAECallRecovery, getRemainingTime(ecalltimer::CBWT)).Times(1).WillOnce(Return(-1));
    testObj->doOnTimeReceived();
}

/**
 * @brief Test case for the doOnTimeReceived method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doOnTimeReceived
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doOnTimeReceived_TC003
 * @test_type functionality
 * @test_objective Verify time received handling
 * @test_precon None
 * @test_input None
 * @test_expected_result Time received is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doOnTimeReceived_TC003) {
    EXPECT_CALL(*M_NAECallStateMachine, getCurrentState()).Times(1).WillOnce(Return(ecall_state::CALLBACK_RUNNING ));
    EXPECT_CALL(*M_NAECallRecovery, getRemainingTime(ecalltimer::CBWT)).Times(1).WillOnce(Return(0));
    mockGetInstance = true;
    EXPECT_CALL(*M_ECallAppHandler, getInstance()).Times(1).WillOnce(Return(nullptr));
    testObj->doOnTimeReceived();
}

/**
 * @brief Test case for the doOnTimeReceived method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doOnTimeReceived
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doOnTimeReceived_TC004
 * @test_type functionality
 * @test_objective Verify time received handling
 * @test_precon None
 * @test_input None
 * @test_expected_result Time received is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doOnTimeReceived_TC004) {
    EXPECT_CALL(*M_NAECallStateMachine, getCurrentState()).Times(1).WillOnce(Return(ecall_state::CALLBACK_RUNNING ));
    EXPECT_CALL(*M_NAECallRecovery, getRemainingTime(ecalltimer::CBWT)).Times(1).WillOnce(Return(100));
    testObj->doOnTimeReceived();
}

/**
 * @brief Test case for the doOnTimeReceived method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doOnTimeReceived
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doOnTimeReceived_TC005
 * @test_type functionality
 * @test_objective Verify time received handling
 * @test_precon None
 * @test_input None
 * @test_expected_result Time received is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doOnTimeReceived_TC005) {
    EXPECT_CALL(*M_NAECallStateMachine, getCurrentState()).Times(1).WillOnce(Return(ecall_state::CALLBACK_RUNNING ));
    EXPECT_CALL(*M_NAECallRecovery, getRemainingTime(ecalltimer::CBWT)).Times(1).WillOnce(Return(0));
    testObj->doOnTimeReceived();
}

/**
 * @brief Test case for the sendSOSAck method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID sendSOSAck
 * @paramList arg1, arg2
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_sendSOSAck_TC001
 * @test_type functionality
 * @test_objective Verify SOS acknowledgment sending
 * @test_precon None
 * @test_input arg1 = 0, arg2 = 0
 * @test_expected_result SOS acknowledgment is sent
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_sendSOSAck_TC001) {
    int32_t arg1 = 0;
    int32_t arg2 = 0;
    testObj->sendSOSAck(arg1, arg2);
}

/**
 * @brief Test case for the doSignalAckOnActiveCall method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doSignalAckOnActiveCall
 * @paramList event, arg1, arg2
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doSignalAckOnActiveCall_TC001
 * @test_type functionality
 * @test_objective Verify signal acknowledgment on active call
 * @test_precon None
 * @test_input event = RECV_MSG_CDS_HEAVY_CRASH, arg1 = 0, arg2 = 0
 * @test_expected_result Signal acknowledgment is performed on active call
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doSignalAckOnActiveCall_TC001) {
    int32_t event = ENUM_TO_UINT32(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH);
    int32_t arg1 = 0;
    int32_t arg2 = 0;
    testObj->doSignalAckOnActiveCall(event, arg1, arg2);
}

/**
 * @brief Test case for the doSignalAckOnActiveCall method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doSignalAckOnActiveCall
 * @paramList event, arg1, arg2
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doSignalAckOnActiveCall_TC002
 * @test_type functionality
 * @test_objective Verify signal acknowledgment on active call
 * @test_precon None
 * @test_input event = RECV_MSG_HMI_BUTTON_SHORT_PRESS, arg1 = 0, arg2 = 0
 * @test_expected_result Signal acknowledgment is performed on active call
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doSignalAckOnActiveCall_TC002) {
    int32_t event = ENUM_TO_UINT32(msg::MSG::RECV_MSG_HMI_BUTTON_SHORT_PRESS);
    int32_t arg1 = 0;
    int32_t arg2 = 0;
    testObj->doSignalAckOnActiveCall(event, arg1, arg2);
}

/**
 * @brief Test case for the doSignalAckOnActiveCall method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doSignalAckOnActiveCall
 * @paramList event, arg1, arg2
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doSignalAckOnActiveCall_TC003
 * @test_type functionality
 * @test_objective Verify signal acknowledgment on active call
 * @test_precon None
 * @test_input event = RECV_MSG_SRS_ECALL_TRIGGER, arg1 = 0, arg2 = 0
 * @test_expected_result Signal acknowledgment is performed on active call
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doSignalAckOnActiveCall_TC003) {
    int32_t event = ENUM_TO_UINT32(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER);
    int32_t arg1 = 0;
    int32_t arg2 = 0;
    testObj->doSignalAckOnActiveCall(event, arg1, arg2);
}

/**
 * @brief Test case for the setQueryFeatureName method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID setQueryFeatureName
 * @paramList featureName
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_setQueryFeatureName_TC001
 * @test_type functionality
 * @test_objective Verify setting query feature name
 * @test_precon None
 * @test_input featureName = "testFeature"
 * @test_expected_result Query feature name is set
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_setQueryFeatureName_TC001) {
    std::string featureName = "testFeature";
    testObj->setQueryFeatureName(featureName);    
}

/**
 * @brief Test case for the getQueryFeatureName method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID getQueryFeatureName
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_getQueryFeatureName_TC001
 * @test_type functionality
 * @test_objective Verify getting query feature name
 * @test_precon None
 * @test_input None
 * @test_expected_result Query feature name is returned
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_getQueryFeatureName_TC001) {
    std::string featureName = "testFeature";
    testObj->setQueryFeatureName(featureName);
    EXPECT_EQ(testObj->getQueryFeatureName(), featureName);
}

/**
 * @brief Test case for the doRecoveringMSD method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doRecoveringMSD
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doRecoveringMSD_TC001
 * @test_type functionality
 * @test_objective Verify MSD recovery handling
 * @test_precon None
 * @test_input None
 * @test_expected_result MSD recovery is handled
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doRecoveringMSD_TC001) {
    EXPECT_CALL(*M_NAECallRecovery, recoverMSD()).Times(1).WillOnce(Return(""));
    testObj->doRecoveringMSD();
}

/**
 * @brief Test case for the doRecoveringMSD method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID doRecoveringMSD
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_doRecoveringMSD_TC002
 * @test_type functionality
 * @test_objective Verify MSD recovery handling with specific return value
 * @test_precon None
 * @test_input None
 * @test_expected_result MSD recovery is handled and returns "msd"
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_doRecoveringMSD_TC002) {
    EXPECT_CALL(*M_NAECallRecovery, recoverMSD()).Times(1).WillOnce(Return("msd"));
    testObj->doRecoveringMSD();
}

/**
 * @brief Test case for the clearMSD method of NAECallProcessor
 * @classID NAECallProcessor
 * @methodID clearMSD
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage
 * @test_case_ID NAECallProcessorTest_clearMSD_TC001
 * @test_type functionality
 * @test_objective Verify clearing of MSD
 * @test_precon None
 * @test_input None
 * @test_expected_result MSD is cleared
 * @test_module
 * @design_id
 */
TEST_F(NAECallProcessorTest, NAECallProcessorTest_clearMSD_TC001) {
    testObj->clearMSD();
}
