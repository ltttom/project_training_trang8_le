/**
* \file Message.h
* \brief    Declaration of Message class.
*
* \details
*   This software is copyright protected and proprietary to
*   LG electronics. LGE grants to you only those rights as
*   set out in the license conditions. All other rights remain
*   with LG electronics.
* \author   sungwoo.oh
* \date     2015.06.08
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/
#ifndef SERVICE_LAYER_UTILS_MESSAGE_H
#define SERVICE_LAYER_UTILS_MESSAGE_H

#include "Typedef.h"
#include <binder/Parcel.h>
#include <utils/RefBase.h>
#include "SLDefine.h"
#include "utils/Buffer.h"
#include "utils/Post.h"

using android::Parcel;
using android::sp;

namespace sl {

class Handler;

/**
 * This is Container data and object which can be send to Handler via MessageSender or
 * enqueue into MessageQueue directly.
 * You can choice the way you want.
 *
 * Message support converting to Parcel so that Message can be posted via IPC as binder
 * so that remote instance can get same Message and take information .
 *
 * Message Ctor is public but you can use Message::obtain() to create.
 *
 * Currently, Service layer is providing two kind of Message.
 * One is this and the other is android::Message.
 * android::Message only can have one int32_t command and ::Message can have various type of
 * message such as what, cmd1, cmd2, cmd3 and parcel.
 */
class Message : public android::RefBase
{
public:
    Message();
    Message(Message& other);
    virtual ~Message();

    static sp<Message> obtain();
    static sp<Message> obtain(const Message* message);
    static sp<Message> obtain(const sp<Handler>& handler);
    static sp<Message> obtain(const sp<Handler>& handler, int32_t obtain_what);
    static sp<Message> obtain(const sp<Handler>& handler, int32_t obtain_what,
                                 int32_t obtain_arg1);
    static sp<Message> obtain(const sp<Handler>& handler, int32_t obtain_what,
                                 void* obtain_obj);
    static sp<Message> obtain(const sp<Handler>& handler, int32_t obtain_what,
                                 int32_t obtain_arg1, int32_t obtain_arg2);
    static sp<Message> obtain(const sp<Handler>& handler, int32_t obtain_what,
                                 int32_t obtain_arg1, int32_t obtain_arg2, void* obtain_obj);
    static sp<Message> obtain(const sp<Handler>& handler, int32_t obtain_what,
                                 int32_t obtain_arg1, int32_t obtain_arg2, int32_t obtain_arg3);
    static sp<Message> obtain(const sp<Handler>& handler, int32_t obtain_what, sp<RefBase> obtain_spRef);
    bool sendToTarget();

    sp<Message> dup() const;

    template<typename T>
    void getObject(android::sp<T>& obj) {
        if (what != 0x7FFFFFFF) {
            obj = static_cast<T*>(spRef.get());
        } else {
            throw std::exception();
        }
    }

    error_t writeToParcel(Parcel* parcel);
    error_t readFromParcel(Parcel& parcel);

    int32_t what;
    int32_t arg1;
    int32_t arg2;
    int32_t arg3;

    void* obj;
    ssize_t obj_size;

    Buffer buffer;
    Buffer buffer2;
    sp<Post> post;
    sp<RefBase> spRef;

    void dump();
protected:
    Message& operator=(const Message& other);
    void clear();

private:
    void setTo(const Message& other);

    sp<Handler> mHandler;
    sp<Message> mNextMessage;
    sp<Message> mPrevMessage;
    uint64_t whenUs;

    friend class Handler;
    friend class MessageQueue;
    friend class SLLooper;
};

} //namespace sl
#endif /* _SERVICE_LAYER_UTILS_MESSAGE_H_ */