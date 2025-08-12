/**
 * @copyright
 * Copyright (c) 2020 by LG Electronics Inc.
 * This program or software including the accompanying associated documentation
 * ("Software") is the proprietary software of LG Electronics Inc. and or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate written license agreement between you
 * and LG Electronics Inc. ("Authorized License"). Except as set forth in an
 * Authorized License, LG Electronics Inc. grants no license (express or implied),
 * rights to use, or waiver of any kind with respect to the Software, and LG
 * Electronics Inc. expressly reserves all rights in and to the Software and all
 * intellectual property therein. If you have no Authorized License, then you have
 * no rights to use the Software in any ways, and should immediately notify LG
 * Electronics Inc. and discontinue all use of the Software.
 * @file        telephone_manager_adapter.cpp
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/

#include <binder/IServiceManager.h> // for get default services manager

// #include <services/TelephonyManagerService/base/ITelephonyService.h>
//#include <services/TelephonyManagerService/base/CallManager.h>
//#include <services/TelephonyManagerService/utils/ByteArray.h>
#include <TelephonyManager.hpp>


#include "telephone_manager_adapter.h"
#include "../logger.h"
#include "../oem_call_app_constants.h"
#include <map>
#include <TelephonyManager.hpp>
#include <ITelephonyRegistry.hpp>
#include <mutex>
//#include "../utils/utils_common.h"


namespace {

    std::unordered_map<uint32_t, std::string> callStateName {
        {telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE         , "IDLE"         },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_ACTIVE       , "ACTIVE"       },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_HOLDING      , "HOLDING"      },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_DIALING      , "DIALING"      },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_ALERTING     , "ALERTING"     },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_INCOMING     , "INCOMING"     },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_WAITING      , "WAITING"      },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_DISCONNECTED , "DISCONNECTED" },
        {telephony::TelephonyManager::PRECISE_CALL_STATE_DISCONNECTING, "DISCONNECTING"}
    };

    constexpr uint32_t REQ_REGISTER                = 10;
    constexpr uint32_t REQ_DIAL                    = 11;
    constexpr uint32_t REQ_ANSWER                  = 12;
    constexpr uint32_t REQ_END_CALL                = 13;
    constexpr uint32_t REQ_STOP                    = 14;

    constexpr uint32_t REC_PRECICSE_STATE_CHANGED  = 21;
    constexpr uint32_t REC_DISCONNECTED            = 22;
    constexpr uint32_t REC_BINDER_DIED             = 23;

    std::unordered_map <uint32_t, std::string> msgName{
        { REQ_REGISTER               ,  "REQ_REGISTER"             },
        { REQ_DIAL                   ,  "REQ_DIAL"                 },
        { REQ_ANSWER                 ,  "REQ_ANSWER"               },
        { REQ_END_CALL               ,  "REQ_END_CALL"             },
        { REQ_STOP                   ,  "REQ_STOP"                 },
        { REC_PRECICSE_STATE_CHANGED ,  "REC_PRECISE_STATE_CHANGED"},
        { REC_DISCONNECTED           ,  "REC_DISCONNECTED"         },
        { REC_BINDER_DIED            ,  "REC_BINDER_DIED"          }
    };

    constexpr uint32_t NOT_DISCONNECTED              =0; /** has not yet disconnected */
    constexpr uint32_t INCOMING_MISSED               =1;/** an incoming call that was missed and never answered */
    constexpr uint32_t NORMAL                        =2; /** normal; remote */
    constexpr uint32_t LOCAL                         =3;/** normal; local hangup */
    constexpr uint32_t BUSY                          =4;/** outgoing call to busy line */
    constexpr uint32_t CONGESTION                    =5;/** outgoing call to congested network */
    constexpr uint32_t MMI                           =6;/** not presently used; dial() returns null */
    constexpr uint32_t INVALID_NUMBER                =7;/** invalid dial string */
    constexpr uint32_t NUMBER_UNREACHABLE            =8; /** cannot reach the peer */
    constexpr uint32_t SERVER_UNREACHABLE            =9;/** cannot reach the server */
    constexpr uint32_t INVALID_CREDENTIALS           =10;/** invalid credentials */
    constexpr uint32_t OUT_OF_NETWORK                =11;/** calling from out of network is not allowed */
    constexpr uint32_t SERVER_ERROR                  =12;/** server error */
    constexpr uint32_t TIMED_OUT                     =13;/** client timed out */
    constexpr uint32_t LOST_SIGNAL                   = 14;
    constexpr uint32_t LIMIT_EXCEEDED                = 15;/** eg GSM ACM limit exceeded */
    constexpr uint32_t INCOMING_REJECTED             =16;/** an incoming call that was rejected */
    constexpr uint32_t POWER_OFF                     =17;/** radio is turned off explicitly */
    constexpr uint32_t OUT_OF_SERVICE                =18;/** out of service */
    constexpr uint32_t ICC_ERROR                     = 19;/** No ICC, ICC locked, or other ICC error */
    constexpr uint32_t CALL_BARRED                   = 20;/** call was blocked by call barring */
    constexpr uint32_t FDN_BLOCKED                   =21;/** call was blocked by fixed dial number */
    constexpr uint32_t CS_RESTRICTED                 =22;/** call was blocked by restricted all voice access */
    constexpr uint32_t CS_RESTRICTED_NORMAL          =23;/** call was blocked by restricted normal voice access */
    constexpr uint32_t CS_RESTRICTED_EMERGENCY       =24;/** call was blocked by restricted emergency voice access */
    constexpr uint32_t UNOBTAINABLE_NUMBER           =25;/** Unassigned number (3GPP TS 24.008 table 10.5.123) */
    constexpr uint32_t CDMA_LOCKED_UNTIL_POWER_CYCLE =26;/** MS is locked until next power cycle */
    constexpr uint32_t CDMA_DROP                     = 27;
    constexpr uint32_t CDMA_INTERCEPT                =28;/** INTERCEPT order received, MS state idle entered */
    constexpr uint32_t CDMA_REORDER                  =29;/** MS has been redirected, call is cancelled */
    constexpr uint32_t CDMA_SO_REJECT                =30;/** service option rejection */
    constexpr uint32_t CDMA_RETRY_ORDER              =31;/** requested service is rejected, retry delay is set */
    constexpr uint32_t CDMA_ACCESS_FAILURE           = 32;
    constexpr uint32_t CDMA_PREEMPTED                = 33;
    constexpr uint32_t CDMA_NOT_EMERGENCY            = 34;/** not an emergency call */
    constexpr uint32_t CDMA_ACCESS_BLOCKED           = 35;/** Access Blocked by CDMA network */
    constexpr uint32_t ERROR_UNSPECIFIED             = 36;
    constexpr uint32_t LOW_BATTERY                   = 37;/** From OOS code base where this value is 61 . The call has ended (mid-call) because the device's battery is too low.*/
    constexpr uint32_t DIAL_LOW_BATTERY              =38;/** From OOS code base where this value is 62 . A call was not dialed because the device's battery is too low.*/

    std::unordered_map< uint32_t, std::string > disconnectReasonName{
        {NOT_DISCONNECTED             , "NOT_DISCONNECTED"              },
        {INCOMING_MISSED              , "INCOMING_MISSED"               },
        {NORMAL                       , "NORMAL"                        },
        {LOCAL                        , "LOCAL"                         },
        {BUSY                         , "BUSY"                          },
        {CONGESTION                   , "CONGESTION"                    },
        {MMI                          , "MMI"                           },
        {INVALID_NUMBER               , "INVALID_NUMBER"                },
        {NUMBER_UNREACHABLE           , "NUMBER_UNREACHABLE"            },
        {SERVER_UNREACHABLE           , "SERVER_UNREACHABLE"            },
        {INVALID_CREDENTIALS          , "INVALID_CREDENTIALS"           },
        {OUT_OF_NETWORK               , "OUT_OF_NETWORK"                },
        {SERVER_ERROR                 , "SERVER_ERROR"                  },
        {TIMED_OUT                    , "TIMED_OUT"                     },
        {LOST_SIGNAL                  , "LOST_SIGNAL"                   },
        {LIMIT_EXCEEDED               , "LIMIT_EXCEEDED"                },
        {INCOMING_REJECTED            , "INCOMING_REJECTED"             },
        {POWER_OFF                    , "POWER_OFF"                     },
        {OUT_OF_SERVICE               , "OUT_OF_SERVICE"                },
        {ICC_ERROR                    , "ICC_ERROR"                     },
        {CALL_BARRED                  , "CALL_BARRED"                   },
        {FDN_BLOCKED                  , "FDN_BLOCKED"                   },
        {CS_RESTRICTED                , "CS_RESTRICTED"                 },
        {CS_RESTRICTED_NORMAL         , "CS_RESTRICTED_NORMAL"          },
        {CS_RESTRICTED_EMERGENCY      , "CS_RESTRICTED_EMERGENCY"       },
        {UNOBTAINABLE_NUMBER          , "UNOBTAINABLE_NUMBER"           },
        {CDMA_LOCKED_UNTIL_POWER_CYCLE, "CDMA_LOCKED_UNTIL_POWER_CYCLE" },
        {CDMA_DROP                    , "CDMA_DROP"                     },
        {CDMA_INTERCEPT               , "CDMA_INTERCEPT"                },
        {CDMA_REORDER                 , "CDMA_REORDER"                  },
        {CDMA_SO_REJECT               , "CDMA_SO_REJECT"                },
        {CDMA_RETRY_ORDER             , "CDMA_RETRY_ORDER"              },
        {CDMA_ACCESS_FAILURE          , "CDMA_ACCESS_FAILURE"           },
        {CDMA_PREEMPTED               , "CDMA_PREEMPTED"                },
        {CDMA_NOT_EMERGENCY           , "CDMA_NOT_EMERGENCY"            },
        {CDMA_ACCESS_BLOCKED          , "CDMA_ACCESS_BLOCKED"           },
        {ERROR_UNSPECIFIED            , "ERROR_UNSPECIFIED"             },
        {LOW_BATTERY                  , "LOW_BATTERY"                   },
        {DIAL_LOW_BATTERY             , "DIAL_LOW_BATTERY"              }
    };
}

namespace oc {

    class RequestCallType: public RefBase {
    public:
        std::string number;
        bool isEcall; // 1 = ecall; 0 = rcall
    };

    class OEMPhoneStateListener : public telephony::PhoneStateListener
    {
        public:
            OEMPhoneStateListener(const android::sp<AdapterHandler> handler) : handler_(handler) {}
            virtual ~OEMPhoneStateListener() {}

            virtual void onPreciseCallStateChanged(const int32_t state) override {
            	LOG_I(DLT_STRING("notify Precise Call State Changed: "), DLT_UINT32(state));
            	auto msg = handler_->obtainMessage(REC_PRECICSE_STATE_CHANGED, state);
            	msg->sendToTarget();
            }

            virtual void onDisconnectCause(const int32_t cause) override {
                handler_->obtainMessage( REC_DISCONNECTED, cause)->sendToTarget();
            }
        private:
            android::sp<AdapterHandler> handler_;
    };



    /**
     * @brief This contains the private properties of TelephoneManager class instance
     *
     */
    struct TelephoneManager::Impl {
        std::shared_ptr<telephony::PhoneStateListener>            spPhoneStateListener = nullptr;
        android::sp<telephony::ITelephonyRegistry> spTelephonyRegistry = nullptr;
        android::sp<ServiceDeathRecipient>         spServiceDeathRecipient = nullptr;
        android::sp<AdapterHandler>               spTelephonyHandler = nullptr;
        android::sp<OemCallHandler>                spOemCallHandler = nullptr; // handler for sending message to main thread
        std::mutex mutex;

        /**
         * @brief This is the state of telephone
         *
         */
        uint32_t state;
        uint32_t disconnectedReason;

        void registerTelephonyService() {
            LOG_I(DLT_STRING(std::string(__func__).c_str()));
            spTelephonyRegistry = android::interface_cast<telephony::ITelephonyRegistry>(
                                        android::defaultServiceManager()->getService(
                                            android::String16("service_layer.TelephonyRegistry")));
            bool error = true;

            if(spTelephonyRegistry != nullptr) {
                auto result = android::IInterface::asBinder(spTelephonyRegistry)->linkToDeath(spServiceDeathRecipient);
                if (
                    result == android::OK
                ) {
                    uint64_t events = telephony::PhoneStateListener::LISTEN_PRECISE_CALL_STATE
                                                            | telephony::PhoneStateListener::LISTEN_DISCONNECT_CAUSE;
                    telephony::TelephonyManager::listen(spPhoneStateListener, events);
                    error = false;
                    LOG_I(DLT_STRING("[TP] Registed Telephony manager Service") );
                }
            }

            if (error) {
                LOG_E(
                    DLT_STRING("[TP] Cannot register Telephony manager Service, try again after"),
                    DLT_UINT32(self::SEND_RETRY_DELAY)
                );
                spTelephonyHandler->sendMessageDelayed(
                    spTelephonyHandler->obtainMessage(REQ_REGISTER),
                    self::SEND_RETRY_DELAY
                );
            }
        }

        void onMsg(const sp<sl::Message> &msg)
        {
            try {
                LOG_I(DLT_STRING("[TP] Received msg =  "), DLT_STRING(msgName[msg->what].c_str()));
            } catch (...) {
            }

            switch (msg->what)
            {
                case REQ_REGISTER:
                {
                    registerTelephonyService();
                }
                break;

                case REC_PRECICSE_STATE_CHANGED: 
                {        
                    std::lock_guard<std::mutex> lock(mutex);

                    try {
                        LOG_I(
                            DLT_STRING("[TP] New state = "), DLT_STRING(callStateName[msg->arg1].c_str()),
                            DLT_STRING(", Current state = "), DLT_STRING(callStateName[impl_->state].c_str())
                        );
                    } catch (...) {
                    }

                    state = msg->arg1;
                    android::sp<StateChangedInfo> info = new StateChangedInfo();
                    info->state = state;
                    info->isNormal = true;
                    switch (state)
                    {
                        case telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE:
                        
                            try {
                                LOG_I(
                                    DLT_STRING("[TP] reason = "), DLT_STRING(disconnectReasonName[impl_->disconnectedReason].c_str())
                                );
                            } catch (...) {
                                LOG_E(DLT_STRING("[TP] reason not defined "), DLT_UINT32(impl_->disconnectedReason));
                            }
                            if (disconnectedReason != serviceinfo::CallDisconectedReason::NORMAL &&
                                disconnectedReason != serviceinfo::CallDisconectedReason::LOCAL) 
                            {
                                info->isNormal = false;
                            }
                        break;

                        case telephony::TelephonyManager::PRECISE_CALL_STATE_ACTIVE:
                        case telephony::TelephonyManager::PRECISE_CALL_STATE_DIALING:
                        case telephony::TelephonyManager::PRECISE_CALL_STATE_HOLDING:
                        case telephony::TelephonyManager::PRECISE_CALL_STATE_WAITING:
                        case telephony::TelephonyManager::PRECISE_CALL_STATE_ALERTING:
                        case telephony::TelephonyManager::PRECISE_CALL_STATE_INCOMING:
                        case telephony::TelephonyManager::PRECISE_CALL_STATE_DISCONNECTED:
                        case telephony::TelephonyManager::PRECISE_CALL_STATE_DISCONNECTING:
                        break;

                        default:
                        {
                            LOG_E(DLT_STRING("[TP] state not defined, stateID = "), DLT_UINT32(msg->arg1));
                            return;
                        }
                    }

                    spOemCallHandler->obtainMessage(msg::MSG_TELEM_CALL_PRECISE_STATE_CHANGED, info)->sendToTarget();
                }
                break;

            case REC_DISCONNECTED :
            {
                // When the call is disconnected, the call "disconnected" signal will be received before "stateChanged".
                // Therefore, when received the "call Disconnected" signal, we still possibly can't start a new call.
                // We have to wait for the state actually changed to IDLE.
                disconnectedReason = msg->arg1;
            }
            break;

            case REC_BINDER_DIED:
            {
            }
            break;

            default :
            {
                LOG_E(DLT_STRING("[TP] Message not defined, MSGID = "), DLT_UINT32(msg->what));
            }
            break;
            }
        }

        void onBinderDied(const android::wp<IBinder> &who)
        {
            LOG_E(DLT_STRING("Telephony died"));
            state = telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE;
            spTelephonyHandler->obtainMessage(REQ_REGISTER)->sendToTarget();
            spOemCallHandler->obtainMessage(msg::MSG_CM_BINDER_DIED, self::ServiceId::TELEPHONY)->sendToTarget();
        }
    };

std::unique_ptr<TelephoneManager::Impl> TelephoneManager::impl_ = std::make_unique<TelephoneManager::Impl>();
TelephoneManager::TelephoneManager(android::sp<OemCallHandler> spHandler)
{
    LOG_E(DLT_STRING("Telephony died"));
    impl_->spOemCallHandler = spHandler;

    static sp<sl::SLLooper> looper = new sl::SLLooper();
    looper->prepare();
    looper->start(false);

    impl_->spTelephonyHandler = new AdapterHandler(looper);
    impl_->spTelephonyHandler->registerHandler(
        [&](const sp<sl::Message> & msg) -> void
        {
            impl_->onMsg(msg);
        });

    impl_->spPhoneStateListener    = std::make_shared<OEMPhoneStateListener>(impl_->spTelephonyHandler);

    impl_->spServiceDeathRecipient = new ServiceDeathRecipient();
    impl_->spServiceDeathRecipient->registerHandler(
        [&] (const android::wp<android::IBinder>& who) -> void
        {
            impl_->onBinderDied(who);
        });

    impl_->registerTelephonyService();
    impl_->state = telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE;
}

void TelephoneManager::dial(const std::string& number, bool isEcall) {
	if (isEcall) {
		LOG_I(DLT_STRING("request dial with calltype = CALL_TYPE_EMERGENCY_CALL, requestInfo->number = "), DLT_STRING(number.c_str()));
		telephony::TelephonyManager::dial(number, telephony::TelephonyManager::CALL_TYPE_EMERGENCY_CALL);
	}
	else { // rcall
		LOG_I(DLT_STRING("request dial with calltype = CALL_TYPE_VOICE_CALL, requestInfo->number = "),  DLT_STRING(number.c_str()));
		telephony::TelephonyManager::dial(number, telephony::TelephonyManager::CALL_TYPE_VOICE_CALL);
	}
}

void TelephoneManager::answer() {
	telephony::TelephonyManager::answerRingingCall();
}

std::string TelephoneManager::requestGetImsi() {
	return telephony::TelephonyManager::getImsi();
}

void TelephoneManager::endCall() {
	telephony::TelephonyManager::endCall();
}

void TelephoneManager::stop() {
	if (impl_->state != telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE) {
		telephony::TelephonyManager::endCall();
	}
}

void TelephoneManager::rejectCall() {
	telephony::TelephonyManager::rejectCall();
}

void TelephoneManager::exitCBMmode() {
	LOG_I(DLT_STRING("Exit Call back waiting mode"));

}

uint32_t TelephoneManager::getCallState() {
    std::lock_guard<std::mutex> lk(impl_->mutex);
    return impl_->state;
}

uint32_t TelephoneManager::getVoiceServiceState() {
    return telephony::TelephonyManager::getServiceState(telephony::TelephonyManager::VOICE_NETWORK);
//    telephony::TelephonyManager::dial(requestInfo->number, telephony::TelephonyManager::CALL_TYPE_EMERGENCY_CALL);

}

}
