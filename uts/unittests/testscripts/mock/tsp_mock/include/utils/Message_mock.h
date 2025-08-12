#ifndef MESSAGE_MOCK_H
#define MESSAGE_MOCK_H
#include "tsp_mock/header_mock/HTTPSManagerAdapter_Base.h"

class MockMessage : public sl::Message {
public:
    MockMessage() : sl::Message() {}
    MockMessage(sl::Message& other) : sl::Message(other) {}
    virtual ~MockMessage() {}

    MOCK_METHOD0(obtain, android::sp<sl::Message>());
    MOCK_METHOD1(obtain, android::sp<sl::Message>(const Message* message));
    MOCK_METHOD1(obtain, android::sp<sl::Message>(const sp<sl::Handler>& handler));
    MOCK_METHOD2(obtain, android::sp<sl::Message>(const sp<sl::Handler>& handler, int32_t obtain_what));
    MOCK_METHOD3(obtain, android::sp<sl::Message>(const sp<sl::Handler>& handler, int32_t obtain_what, int32_t obtain_arg1));
    MOCK_METHOD3(obtain, android::sp<sl::Message>(const sp<sl::Handler>& handler, int32_t obtain_what, void* obtain_obj));
    MOCK_METHOD4(obtain, android::sp<sl::Message>(const sp<sl::Handler>& handler, int32_t obtain_what, int32_t obtain_arg1, int32_t obtain_arg2));
    MOCK_METHOD5(obtain, android::sp<sl::Message>(const sp<sl::Handler>& handler, int32_t obtain_what, int32_t obtain_arg1, int32_t obtain_arg2, void* obtain_obj));
    MOCK_METHOD0(sendToTarget, bool());
    MOCK_METHOD1(getObject, void(android::sp<ResponseData>));
};
MockMessage* M_Message = nullptr;

bool sl::Message::sendToTarget() {
  return M_Message->sendToTarget();
}



#endif // MESSAGE_MOCK_H