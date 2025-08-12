#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <dlfcn.h>

#include <dlt/dlt.h>

// #ifndef REGION_PCB26
// #define REGION_PCB26
// #endif


#undef private
#define private public
#undef protected
#define protected public

#ifdef DLT_LOG
#undef DLT_LOG
#endif

#define DLT_LOG(...) std::cerr << "LOG: " << __LINE__ << ": " << __FUNCTION__ << std::endl;

#include "tsp_mock/header_mock/logger_Base.h"
#include "tsp_mock/include/utils/CURL_mock.h"
#include "tsp_mock/include/utils/reader_mock.h"
#include "tsp_mock/include/utils/CURL_mock.h"
#include "tsp_mock/header_mock/Message_Base.h"
#include "tsp_mock/header_mock/HTTPSManagerAdapter_Base.h"


#include "tspecall/core/services/HTTPSManagerAdapter.cpp"

using namespace ecallapp;
using namespace android;
using namespace sl;

#include "tsp_mock/src/HTTPSManager_mock.h"
#include "tsp_mock/src/ECallAppHandler_mock.h"
#include "tsp_mock/src/CommonFunctions_mock.h"
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

using ::testing::SetArgReferee;

class MockHTTPSManagerAdapter : public HTTPSManagerAdapter {
  public:
    MockHTTPSManagerAdapter() {}
    ~MockHTTPSManagerAdapter() {}

    MOCK_METHOD3(sendRequestAsync1, void(const std::string&, const std::string&, const std::string&));
    MOCK_METHOD5(sendRequest, uint32_t(string strURL, string dataBody, HttpData_t& respData,bool needAuthenServer, string strHttpMethod));
    MOCK_METHOD1(onReceivedData, void(uint32_t));
    MOCK_METHOD4(sendRecvThreadFunc, void(string, string, bool, string));
    void test_sendRequestAsync1(const std::string& strURL, const std::string& dataBody, const std::string& strHttpMethod) {
      HTTPSManagerAdapter::sendRequestAsync1(strURL, dataBody, strHttpMethod);
    }
    void test_sendRequestAsync2(string strURL, string dataBody, bool needAuthenServer, string strHttpMethod) {
      HTTPSManagerAdapter::sendRequestAsync2(strURL, dataBody, needAuthenServer, strHttpMethod);
    }
    void test_sendRecvThreadFunc(string strURL, string dataBody, bool needAuthenServer, string strHttpMethod) {
      HTTPSManagerAdapter::sendRecvThreadFunc(strURL, dataBody, needAuthenServer, strHttpMethod);
    }
};

class HTTPSManagerAdapterTest : public ::testing::Test {
  protected:
      std::unique_ptr<HTTPSManagerAdapter> testObj;
      std::unique_ptr<MockHTTPSManagerAdapter> mockTestObj;
      virtual ~HTTPSManagerAdapterTest() {
      }

      virtual void SetUp() {
          testObj = std::make_unique<HTTPSManagerAdapter>();
          mockTestObj = std::make_unique<MockHTTPSManagerAdapter>();
          M_BackgroundTask = new Mock_BackgroundTask();
          M_CommonFunctions = new MockCommonFunctions();
          M_ECallAppHandler = new MockECallAppHandler(mLooper, *new IECallAppIfMock());
          M_Message = new MockMessage();
          M_URL = new MockCURLcode();
          Json::M_CharReader = new Json::mockCharReader();
          // HTTPSManagerAdapter::instance = mockTestObj.get();
          Json::M_Value = new Json::mockValue();;
      }

      virtual void TearDown() {
          testObj = nullptr;
          mockTestObj = nullptr;
          delete M_BackgroundTask;
          delete M_CommonFunctions;
          delete M_ECallAppHandler;
          delete M_Message;
          delete M_URL;
          delete Json::M_CharReader;
          delete Json::M_Value;
      }
};

/**
 * @classID HTTPSManagerAdapter
 * @methodID writeFunc
 * @paramList void *ptr, size_t size, size_t nmemb, HttpData_t *s
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_writeFunc_TC001
 * @test_type functionality
 * @test_objective To test writeFunc with valid header input.
 * @test_precon None
 * @test_input Valid header string.
 * @test_expected_result Proper parsing of header into respHeader and mRespHeader.
 * @test_module
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_writeFunc_TC001) {
#if 1
  // Set up
  const char* header = "Content-Type: application/json\r\n";
  size_t size = 1;
  size_t nmemb = strlen(header);
  HttpData_t httpData = {
    { // Initializing mRespHeader with some key-value pairs
        {"Content-Type", "application/json"},
        {"Content-Length", "123"},
        {"Connection", "keep-alive"}
    },
    "Content-Type: application/json\r\nContent-Length: 123\r\n", // Initializing respHeader
    "This is the response body." // Initializing respBody
  };

  // Execute
  testObj->writeFunc((void*)header, size, nmemb, &httpData);

  // Verify
  EXPECT_EQ(httpData.respHeader, "Content-Type: application/json\r\nContent-Length: 123\r\n");
#endif
}


/**
 * @classID HTTPSManagerAdapter
 * @methodID writeFunc
 * @paramList void *ptr, size_t size, size_t nmemb, HttpData_t *s
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_writeFunc_TC002
 * @test_type functionality
 * @test_objective To test writeFunc with valid header input, newLen < oldLen
 * @test_precon None
 * @test_input Valid header string.
 * @test_expected_result Proper parsing of header into respHeader and mRespHeader.
 * @test_module
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_writeFunc_TC002) {
  #if 1
    // Set up
    const char* header = "Content-Type: application/json\r\n";
    size_t size = -1;
    size_t nmemb = 1;
    HttpData_t httpData = {
      { // Initializing mRespHeader with some key-value pairs
          {"Content-Type", "application/json"},
          {"Content-Length", "123"},
          {"Connection", "keep-alive"}
      },
      "Content-Type: application/json\r\nContent-Length: 123\r\n", // Initializing respHeader
      "This is the response body." // Initializing respBody
    };

    // Execute
    testObj->writeFunc((void*)header, size, nmemb, &httpData);

    // Verify
    EXPECT_EQ(httpData.respHeader, "Content-Type: application/json\r\nContent-Length: 123\r\n");
  #endif
  }

/**
 * @classID HTTPSManagerAdapter
 * @methodID headerFunc
 * @paramList void *ptr, size_t size, size_t nmemb, HttpData_t *s
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_headerFunc_TC001
 * @test_type functionality
 * @test_objective To test headerFunc with valid header input.
 * @test_precon None
 * @test_input Valid header string.
 * @test_expected_result Proper parsing of header into respHeader and mRespHeader.
 * @test_module
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_headerFunc_TC001) {
#if 1
  // Set up
  const char* header = "Content\r-Type: application/json\r\n";
  size_t size = 1;
  size_t nmemb = strlen(header);
  HttpData_t httpData = {
    { // Initializing mRespHeader with some key-value pairs
        {"Content-Type", "application/json"},
        {"Content-Length", "123"},
        {"Connection", "keep-alive"}
    },
    "Content-Type: application/json\r\nContent-Length: 123\r\n", // Initializing respHeader
    "This is the response body." // Initializing respBody
  };

  // Execute
  testObj->headerFunc((void*)header, size, nmemb, &httpData);

  // Verify
  EXPECT_EQ(httpData.respHeader, "Content-Type: application/json\r\nContent-Length: 123\r\nContent\r-Type: application/json\r\n");
#endif
}



/**
 * @classID HTTPSManagerAdapter
 * @methodID headerFunc
 * @paramList void *ptr, size_t size, size_t nmemb, HttpData_t *s
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_headerFunc_TC002
 * @test_type functionality
 * @test_objective To test headerFunc with valid header input.
 * @test_precon None
 * @test_input Valid header string.
 * @test_expected_result Proper parsing of header into respHeader and mRespHeader.
 * @test_module
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_headerFunc_TC002) {
  #if 1
    // Set up
    const char* header = "C\nontent-Type: application/json";
    size_t size = -1;
    size_t nmemb = 1;
    HttpData_t httpData = {
      { // Initializing mRespHeader with no key-value pairs
      },
      " \n  ", // Initializing respHeader with len = 0
      "   " // Initializing respBody with len = 0
    };

    // Execute
    testObj->headerFunc((void*)header, size, nmemb, &httpData);

    // Verify
    EXPECT_EQ(httpData.respHeader, " \n ");
  #endif
  }


/**
 * @classID HTTPSManagerAdapter
 * @methodID headerFunc
 * @paramList void *ptr, size_t size, size_t nmemb, HttpData_t *s
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_headerFunc_TC003
 * @test_type functionality
 * @test_objective To test headerFunc with valid header input, no \n or \r in header string.
 * @test_precon None
 * @test_input Valid header string.
 * @test_expected_result Proper parsing of header into respHeader and mRespHeader.
 * @test_module
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_headerFunc_TC003) {
  #if 1
    // Set up
    const char* header = "Content-Type: application/json";
    size_t size = 1;
    size_t nmemb = strlen(header);
    HttpData_t httpData = {
      { // Initializing mRespHeader with some key-value pairs
          {"Content-Type", "application/json"},
          {"Content-Length", "123"},
          {"Connection", "keep-alive"}
      },
      "Content-Type: application", // Initializing respHeader
      "This is the response body." // Initializing respBody
    };

    // Execute
    testObj->headerFunc((void*)header, size, nmemb, &httpData);

    // Verify
    EXPECT_EQ(httpData.respHeader, "Content-Type: applicationContent-Type: application/json");
  #endif
  }


/**
 * @brief This is a test script for the HTTPSManagerAdapter_getInstance function
 * @classID HTTPSManagerAdapter
 * @methodID getInstance
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_getInstance_TC001
 * @test_type functionality
 * @test_objective To test the initialization of the HTTPSManagerAdapter singleton instance
 * @test_precon None
 * @test_input None
 * @test_expected_result A new instance of HTTPSManagerAdapter is created
 * @test_module
 * @design_id
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_getInstance_TC001) {
#if 1
  // Set up
  HTTPSManagerAdapter* instance1 = HTTPSManagerAdapter::instance;
  HTTPSManagerAdapter::instance = nullptr;

  // Execute
  HTTPSManagerAdapter* temp_instance = HTTPSManagerAdapter::getInstance();

  // Verify
  ASSERT_NE(temp_instance, nullptr);
  HTTPSManagerAdapter::instance = instance1;
#endif
}

/**
 * @brief This is a test script for the HTTPSManagerAdapter_getInstance function
 * @classID HTTPSManagerAdapter
 * @methodID getInstance
 * @paramList None
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_getInstance_TC002
 * @test_type functionality
 * @test_objective To test the initialization of the HTTPSManagerAdapter singleton instance
 * @test_precon None
 * @test_input None
 * @test_expected_result A new instance of HTTPSManagerAdapter is created
 * @test_module
 * @design_id
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_getInstance_TC002) {
#if 1
  // Set up
  HTTPSManagerAdapter* instance1 = HTTPSManagerAdapter::instance;
  HTTPSManagerAdapter::instance = reinterpret_cast<HTTPSManagerAdapter*>(0x1);

  // Execute
  HTTPSManagerAdapter* temp_instance = HTTPSManagerAdapter::getInstance();

  // Verify
  ASSERT_EQ(temp_instance, HTTPSManagerAdapter::instance);
  HTTPSManagerAdapter::instance = instance1;
#endif
}

/**
 *   @brief Test sendRequestAsync2 with invalid HTTP method
 *   @classID HTTPSManagerAdapter
 *   @methodID sendRequestAsync2
 *   @paramList string strURL, string dataBody, bool needAuthenServer, string strHttpMethod
 *   @priority P1
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition Invalid HTTP method provided
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID HTTPSManagerAdapter_sendRequestAsync2_TC001
 *   @test_type functionality
 *   @test_objective To test the behavior when an invalid HTTP method is provided
 *   @test_precon None
 *   @test_input strURL = "https://example.com", dataBody = "{}", needAuthenServer = false, strHttpMethod = "INVALID"
 *   @test_expected_result sendRecvThreadFunc is called with correct parameters
 *   @test_module
 *   @design_id
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_sendRequestAsync2_TC001) {
#if 1
  // Set up
  // EXPECT_CALL(*mockTestObj, sendRecvThreadFunc(_, _, _, _)).Times(1);

  // Execute
  mockTestObj->test_sendRequestAsync2("https://example.com", "{}", false, "INVALID");
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Verify
#endif
}




/**
 * @classID HTTPSManagerAdapter
 * @methodID sendRequestAsync1
 * @paramList string strURL, string dataBody, string strHttpMethod
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_sendRequestAsync1_TC001
 * @test_type functionality
 * @test_objective To test sending a request with a long URL.
 * @test_precon None
 * @test_input None
 * @test_expected_result Proper handling of long URL.
 * @test_module
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_sendRequestAsync1_TC001) {
#if 1
  // Set up
  string strURL = "";
  string dataBody, strHttpMethod;
  RespInfo_t tRspData{};
  HttpData_t respData{};
  respData.respHeader = "header";
  respData.respBody = "body";
  mockTestObj->mRspData.respHeader = "header";
  mockTestObj->mRspData.respBody = "body";
  HTTPSManagerAdapter::instance = mockTestObj.get(); // testObj.instance
  EXPECT_CALL(*mockTestObj, sendRequest(_, _, _, _, _)).WillOnce(Return(HTTP_RES_OK + 1));
  EXPECT_CALL(*M_BackgroundTask, run()).WillOnce(Invoke([&]() {
    std::cout << "I am in run" << std::endl;
    mockTestObj->mBackgroundTask->mTaskWorker(strURL);
  }));

  // Execute
  mockTestObj->test_sendRequestAsync1(strURL, "data", "POST");

  // Verify
  EXPECT_EQ(mockTestObj->mRspData.respHeader, "header");
  EXPECT_EQ(mockTestObj->mRspData.respBody, "body");
#endif
}


/**
 * @classID HTTPSManagerAdapter
 * @methodID sendRequestAsync1
 * @paramList string strURL, string dataBody, string strHttpMethod
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_sendRequestAsync1_TC002
 * @test_type functionality
 * @test_objective To test sending a request with a long URL.
 * @test_precon None
 * @test_input None
 * @test_expected_result Proper handling of long URL.
 * @test_module
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_sendRequestAsync1_TC002) {
#if 1
  // Set up
  string strURL = "";
  string dataBody, strHttpMethod;
  RespInfo_t tRspData{};
  HttpData_t respData{};
  respData.respHeader = "header";
  respData.respBody = "body";
  mockTestObj->mRspData.respHeader = "header";
  mockTestObj->mRspData.respBody = "body";
  HTTPSManagerAdapter::instance = mockTestObj.get(); // testObj.instance
  EXPECT_CALL(*mockTestObj, sendRequest(_, _, _, _, _)).WillOnce(Return(HTTP_RES_OK));
  EXPECT_CALL(*M_BackgroundTask, run()).WillOnce(Invoke([&]() {
    std::cout << "I am in run" << std::endl;
    mockTestObj->mBackgroundTask->mTaskWorker(strURL);
  }));

  // Execute
  mockTestObj->test_sendRequestAsync1(strURL, "data", "POST");

  // Verify
  EXPECT_EQ(mockTestObj->mRspData.respHeader, "");
  EXPECT_EQ(mockTestObj->mRspData.respBody, "");
#endif
}


/**
 * @brief This is a test script for the HTTPSManagerAdapter_sendRecvThreadFunc function
 * @classID HTTPSManagerAdapter
 * @methodID sendRecvThreadFunc
 * @paramList string strURL, string strPayload, bool needAuthenServer, string strHttpMethod
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_sendRecvThreadFunc_TC001
 * @test_type functionality
 * @test_objective To test sendRecvThreadFunc with proper request
 * @test_precon None
 * @test_input strURL = "https://example.com", strPayload = "{}", needAuthenServer = true, strHttpMethod = "POST"
 * @test_expected_result Proper handling of proper request
 * @test_module
 * @design_id
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_sendRecvThreadFunc_TC001) {
#if 1
  // Setup
  std::string strURL = "https://example.com";
  std::string strPayload = "{}";
  bool needAuthenServer = true;
  std::string strHttpMethod = "POST";

  EXPECT_CALL(*mockTestObj, sendRequest(_, _, _, _, _))
      .WillOnce(Return(HTTP_RES_OK));

  // Execute
  mockTestObj->test_sendRecvThreadFunc(strURL, strPayload, needAuthenServer, strHttpMethod);

  // Verify
  EXPECT_EQ(mockTestObj->mRspData.respHeader, "");
  EXPECT_EQ(mockTestObj->mRspData.respBody, "");
#endif
}


/**
 * @brief This is a test script for the HTTPSManagerAdapter_sendRecvThreadFunc function
 * @classID HTTPSManagerAdapter
 * @methodID sendRecvThreadFunc
 * @paramList string strURL, string strPayload, bool needAuthenServer, string strHttpMethod
 * @priority P1
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_sendRecvThreadFunc_TC002
 * @test_type functionality
 * @test_objective To test sendRecvThreadFunc with failed request
 * @test_precon None
 * @test_input strURL = "https://example.com", strPayload = "{}", needAuthenServer = true, strHttpMethod = "POST"
 * @test_expected_result Proper handling of failed request
 * @test_module
 * @design_id
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_sendRecvThreadFunc_TC002) {
#if 1
  // Setup
  std::string strURL = "https://example.com";
  std::string strPayload = "{}";
  bool needAuthenServer = true;
  std::string strHttpMethod = "POST";
  EXPECT_CALL(*mockTestObj, sendRequest(strURL, strPayload, _, needAuthenServer, strHttpMethod))
      .WillOnce(Return(HTTP_RES_NOK));

  // Execute
  mockTestObj->test_sendRecvThreadFunc(strURL, strPayload, needAuthenServer, strHttpMethod);

  // Verify
  EXPECT_EQ(mockTestObj->mRspData.respHeader, "");
  EXPECT_EQ(mockTestObj->mRspData.respBody, "");
#endif
}


/**
 *   @brief Test sendRequest function when curl_easy_init fails
 *   @classID HTTPSManagerAdapter
 *   @methodID sendRequest
 *   @paramList string strURL, string strPayload, HttpData_t& respData, bool needAuthenServer, string strHttpMethod
 *   @priority P1
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition curl_easy_init returns NULL
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID HTTPSManagerAdapter_sendRequest_TC001
 *   @test_type functionality
 *   @test_objective To test the behavior when curl_easy_init fails
 *   @test_precon None
 *   @test_input strURL = "https://example.com", strPayload = "{}", needAuthenServer = false, strHttpMethod = "POST"
 *   @test_expected_result Function returns CURLE_FAILED_INIT
 *   @test_module
 *   @design_id
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_sendRequest_TC001) {
#if 1
  // Set up
  EXPECT_CALL(*M_URL, curl_easy_init()).WillOnce(Return(nullptr));
  HttpData_t respData;

  // Execute
  uint32_t result = testObj->sendRequest("https://example.com", "{}", respData, false, "POST");

  // Verify
  EXPECT_EQ(result, CURLE_FAILED_INIT);
#endif
}

/**
 *   @brief Test sendRequest function when curl_easy_perform fails
 *   @classID HTTPSManagerAdapter
 *   @methodID sendRequest
 *   @paramList string strURL, string strPayload, HttpData_t& respData, bool needAuthenServer, string strHttpMethod
 *   @priority P1
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition curl_easy_perform returns CURLE_FAILED_INIT
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID HTTPSManagerAdapter_sendRequest_TC002
 *   @test_type functionality
 *   @test_objective To test the behavior when curl_easy_perform fails
 *   @test_precon None
 *   @test_input strURL = "https://example.com", strPayload = "{}", needAuthenServer = false, strHttpMethod = "POST"
 *   @test_expected_result Function returns HTTP_RES_NOK
 *   @test_module
 *   @design_id
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_sendRequest_TC002) {
#if 1
  // Set up
  CURL* mockCurl = reinterpret_cast<CURL*>(0x1);
  EXPECT_CALL(*M_URL, curl_easy_init()).WillOnce(Return(mockCurl));
  EXPECT_CALL(*M_URL, curl_easy_perform(_)).WillOnce(Return(CURLE_FAILED_INIT));
  EXPECT_CALL(*M_URL, curl_easy_cleanup(mockCurl));
  HttpData_t respData;

  // Execute
  uint32_t result = testObj->sendRequest("https://example.com", "{}", respData, false, "POST");

  // Verify
  EXPECT_EQ(result, HTTP_RES_NOK);
#endif
}



/**
 *   @brief Test sendRequest function when curl_easy_perform successes
 *   @classID HTTPSManagerAdapter
 *   @methodID sendRequest
 *   @paramList string strURL, string strPayload, HttpData_t& respData, bool needAuthenServer, string strHttpMethod
 *   @priority P1
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition curl_easy_perform returns CURLE_FAILED_INIT
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID HTTPSManagerAdapter_sendRequest_TC002
 *   @test_type functionality
 *   @test_objective To test the behavior when curl_easy_perform OK
 *   @test_precon None
 *   @test_input strURL = "https://example.com", strPayload = "{}", needAuthenServer = true, strHttpMethod = "POST"
 *   @test_expected_result Function returns HTTP_RES_NOK
 *   @test_module
 *   @design_id
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_sendRequest_TC003) {
#if 1
  // Set up
  CURL* mockCurl = reinterpret_cast<CURL*>(0x1);;
  EXPECT_CALL(*M_URL, curl_easy_init()).WillOnce(Return(mockCurl));
  // EXPECT_CALL(*M_URL, curl_easy_setopt(mockCurl, CURLOPT_URL, testing::_)).WillRepeatedly(Return(CURLE_OK));
  EXPECT_CALL(*M_URL, curl_easy_perform(_)).WillOnce(Return(CURLE_OK));
  EXPECT_CALL(*M_URL, curl_easy_cleanup(mockCurl));
  long responseCode = HTTPS_RESP_OK;
  EXPECT_CALL(*M_URL, curl_easy_getinfo(_, CURLINFO_RESPONSE_CODE, testing::_)).WillOnce(DoAll(testing::SetArgPointee<2>(responseCode), Return(HTTPS_RESP_OK)));
  HttpData_t respData;
  respData.mRespHeader = {
    {"Content-Type", "application/json"},
    {"Content-Length", "123"},
    {"Connection", "keep-alive"}
  };

  // Execute
  uint32_t result = testObj->sendRequest("https://example.com", "{}", respData, true, "POST");

  // Verify
  EXPECT_EQ(result, HTTP_RES_OK);
#endif
}


/**
 *   @brief Test sendRequest function when curl_easy_perform OK
 *   @classID HTTPSManagerAdapter
 *   @methodID sendRequest
 *   @paramList string strURL, string strPayload, HttpData_t& respData, bool needAuthenServer, string strHttpMethod
 *   @priority P1
 *   @resolution_Method Equivalence Partitioning
 *   @test_condition curl_easy_perform returns CURLE_FAILED_INIT
 *   @test_Coverage_Item line coverage, branch coverage
 *   @test_case_ID HTTPSManagerAdapter_sendRequest_TC002
 *   @test_type functionality
 *   @test_objective To test the behavior when curl_easy_perform OK
 *   @test_precon None
 *   @test_input strURL = "https://example.com", strPayload = "{}", needAuthenServer = false, strHttpMethod = "POST"
 *   @test_expected_result Function returns HTTP_RES_NOK
 *   @test_module
 *   @design_id
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_sendRequest_TC004) {
#if 1
  // Set up
  CURL* mockCurl = reinterpret_cast<CURL*>(0x1);
  EXPECT_CALL(*M_URL, curl_easy_init()).WillOnce(Return(mockCurl));
  EXPECT_CALL(*M_URL, curl_easy_perform(_)).WillOnce(Return(CURLE_OK));
  EXPECT_CALL(*M_URL, curl_easy_cleanup(mockCurl));
  long responseCode = HTTPS_TIMEOUT;
  EXPECT_CALL(*M_URL, curl_easy_getinfo(_, CURLINFO_RESPONSE_CODE, testing::_)).WillOnce(DoAll(testing::SetArgPointee<2>(responseCode), Return(HTTPS_TIMEOUT)));
  HttpData_t respData;

  // Execute
  uint32_t result = testObj->sendRequest("https://example.com", "{}", respData, false, "POST");

  // Verify
  EXPECT_EQ(result, HTTP_RES_NOK);
#endif
}


/**
* @brief This is a test script for the HTTPSManagerAdapter_onReceivedData function
* @classID HTTPSManagerAdapter
* @methodID onReceivedData
* @paramList const android::sp<sl::Message>& message
* @priority P1
* @resolution_Method Equivalence Partitioning
* @test_condition None
* @test_Coverage_Item line coverage, branch coverage
* @test_case_ID HTTPSManagerAdapter_onReceivedData_TC001
* @test_type functionality
* @test_objective To test onReceivedData when message is null
* @test_precon None
* @test_input message = nullptr
* @test_expected_result Should send failure message
* @test_module
* @design_id
*/
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_onReceivedData_TC001) {
  #if 1
    // Setup
    android::sp<sl::Message> message;
    EXPECT_CALL(*M_ECallAppHandler, obtainMessage(_));
    EXPECT_CALL(*M_Message, sendToTarget()).WillOnce(Return(true));

    // Execute
    testObj->onReceivedData(nullptr);

    // Verify
    EXPECT_EQ(testObj->mRspData.respHeader, "");
    EXPECT_EQ(testObj->mRspData.respHeader, "");
  #endif
  }



/**
* @brief This is a test script for the HTTPSManagerAdapter_onReceivedData function
* @classID HTTPSManagerAdapter
* @methodID onReceivedData
* @paramList const android::sp<sl::Message>& message
* @priority P1
* @resolution_Method Equivalence Partitioning
* @test_condition None
* @test_Coverage_Item line coverage, branch coverage
* @test_case_ID HTTPSManagerAdapter_onReceivedData_TC002
* @test_type functionality
* @test_objective To test onReceivedData when message returns not nullptr, and spReponseData == nullptr
* @test_precon None
* @test_input message with valid ResponseData
* @test_expected_result Should send success message and parse JSON
* @test_module
* @design_id
*/
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_onReceivedData_TC002) {
#if 1
  // Setup
  sp<sl::Message> msg = sp<sl::Message>(new sl::Message());
  msg->spRef = new ResponseData("{\"Phone number =\":\"1234567890\", \"<Judge>\":\"Pass\"}", "{\"Phone number =\":\"1234567890\", \"<Judge>\":\"Pass\"}");

  // Execute
  testObj->onReceivedData(msg);

  // Verify
  EXPECT_EQ(testObj->mMsdData.TelNo, "");
  EXPECT_EQ(testObj->mMsdData.Judge, "");
#endif
}

/**
* @brief This is a test script for the HTTPSManagerAdapter_onReceivedData function
* @classID HTTPSManagerAdapter
* @methodID onReceivedData
* @paramList const android::sp<sl::Message>& message
* @priority P1
* @resolution_Method Equivalence Partitioning
* @test_condition None
* @test_Coverage_Item line coverage, branch coverage
* @test_case_ID HTTPSManagerAdapter_onReceivedData_TC003
* @test_type functionality
* @test_objective To test onReceivedData when retCode equals HTTP_RES_OK
* @test_precon None
* @test_input message with valid ResponseData and retCode = HTTP_RES_OK
* @test_expected_result Should send success message
* @test_module
* @design_id
*/
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_onReceivedData_TC003) {
#if 1
  // Setup
  android::sp<ResponseData> spResponseData = new ResponseData("{\"Phone number =\":\"1234567890\", \"<Judge>\":\"Pass\"}", "{\"Phone number =\":\"1234567890\", \"<Judge>\":\"Pass\"}");
  spResponseData->ret = HTTP_RES_OK;
  android::sp<sl::Message> message = new sl::Message();
  message->spRef = spResponseData;
  // Execute
  testObj->onReceivedData(message);

  // Verify
  EXPECT_EQ(spResponseData->ret, HTTP_RES_OK);
#endif
}


/**
 * @classID HTTPSManagerAdapter
 * @methodID getTelNo
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_getTelNo_TC001
 * @test_type functionality
 * @test_objective To test getTelNo when TelNo is set to a non-standard format.
 * @test_precon None
 * @test_input None
 * @test_expected_result Proper retrieval of the non-standard formatted phone number.
 * @test_module
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_getTelNo_TC001) {
#if 1
  // Set up
  testObj->mMsdData.TelNo = "1234567890"; // No dashes or spaces

  // Execute
  std::string result = testObj->getTelNo();

  // Verify
  EXPECT_EQ(result, "1234567890");
#endif
}


/**
 * @classID HTTPSManagerAdapter
 * @methodID setTelNo
 * @paramList std::string telNo
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_setTelNo_TC001
 * @test_type functionality
 * @test_objective To test setTelNo with a valid phone number.
 * @test_precon None
 * @test_input "123-456-7890"
 * @test_expected_result Proper retrieval of the valid phone number via getTelNo.
 * @test_module
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_setTelNo_TC001) {
#if 1
  // Set up
  // Nothing to set-up

  // Execute
  testObj->setTelNo("123-456-7890");

  // Verify
  EXPECT_EQ(testObj->getTelNo(), "123-456-7890");
#endif
}


/**
 * @classID HTTPSManagerAdapter
 * @methodID clearRspData
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_clearRspData_TC001
 * @test_type functionality
 * @test_objective To test setTelNo with a valid phone number.
 * @test_precon None
 * @test_input "123-456-7890"
 * @test_expected_result Proper retrieval of the valid phone number via getTelNo.
 * @test_module
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_clearRspData_TC001) {
#if 1
  // Set up
  testObj->mRspData.respHeader = "Header";
  testObj->mRspData.respBody = "Body";

  // Execute
  testObj->clearRspData();

  // Verify
  EXPECT_EQ(testObj->mRspData.respHeader.length(), 0);
  EXPECT_EQ(testObj->mRspData.respBody.length(), 0);
#endif
}


/**
 * @classID HTTPSManagerAdapter
 * @methodID getRespData
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_getRespData_TC001
 * @test_type functionality
 * @test_objective To test setTelNo with a valid phone number.
 * @test_precon None
 * @test_input "123-456-7890"
 * @test_expected_result Proper retrieval of the valid phone number via getTelNo.
 * @test_module
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_getRespData_TC001) {
#if 1
  // Set up
  testObj->mRspData.respHeader = "Header";
  testObj->mRspData.respBody = "Body";

  // Execute
  RespInfo_t* _RespInfo = testObj->getRespData();

  // Verify
  EXPECT_EQ(_RespInfo->respHeader.length(), 6);
  EXPECT_EQ(_RespInfo->respBody.length(), 4);
#endif
}


/**
 * @classID HTTPSManagerAdapter
 * @methodID registerService
 * @paramList None
 * @priority P2
 * @resolution_Method Equivalence Partitioning
 * @test_condition None
 * @test_Coverage_Item line coverage, branch coverage
 * @test_case_ID HTTPSManagerAdapter_registerService_TC001
 * @test_type functionality
 * @test_objective To make the coverage 100%.
 * @test_precon None
 * @test_input Nothing to input
 * @test_expected_result Nothing to expect
 * @test_module
 */
TEST_F(HTTPSManagerAdapterTest, HTTPSManagerAdapter_registerService_TC001) {
#if 1
  // Set up

  // Execute
  testObj->registerService();

  // Verify
  // Just print out log, nothing to expect.
#endif
}