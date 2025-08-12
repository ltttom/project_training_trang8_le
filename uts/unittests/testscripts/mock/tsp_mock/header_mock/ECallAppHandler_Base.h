#ifndef ECALLNONREG_UTILS_ECALLNONREGHANDLER_H
#define ECALLNONREG_UTILS_ECALLNONREGHANDLER_H

#include <utils/Handler.h>
#include <utils/Message.h>
#include "tspecall/core/utils/IECallAppIf.h"

namespace ecallapp {
class ECallAppHandler : public sl::Handler {
public:
    ECallAppHandler(sp<sl::SLLooper>& looper, IECallAppIf& app) noexcept :
        Handler(looper),
        mApplication(app) {}
    
    virtual void handleMessage(const sp<sl::Message>& handlemsg) override;
    void handleMessageInternal(const sp<sl::Message>& handlemsg);
    void init(ECallAppHandler* const) const;
    static ECallAppHandler* getInstance();

private:
    IECallAppIf& mApplication;
    static ECallAppHandler* instance;
};

} /* End: namespace ECALLDCMNONREG */
#endif // ECALLNONREG_UTILS_ECALLNONREGHANDLER_H
