#ifndef __OEM_CALL_APP_CONSTANTS_H__
#define __OEM_CALL_APP_CONSTANTS_H__
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <TelephonyManager.hpp>
//#include <services/TelephonyManagerService/base/CallManager.h>
//#include <services/TelephonyManagerService/utils/EnumUtils.h>
#include "services/ApplicationManagerService/IApplicationManagerServiceType.h"
#include <services/ApplicationManagerService/ISystemPostReceiver.h>
#include <services/RemoteIfManagerService/EServerRespType.h>
#include <services/PowerManagerService/PowerIndexEnum.h>
#include <services/PowerManagerService/PowerManager.h>
//#include <services/SomeipConsumerManagerService/SomeipConsumerManager.h>

namespace oc {
    #define SLDD_TEST_MSG_ECALL_THRESHOLD 5000
    namespace self {
        const std::string feature_ecall_us = "ecall-us";
        const std::string feature_ecall_us_incomming = "ecall-us-incomming";
        const std::string feature_ecall_us_cbwt ="ecall-us-cbwt";
        const std::string feature_acn_us   = "acn-us";
        const std::string feature_acn_us_incomming = "acn-us-incomming";
        const std::string feature_acn_us_cbwt ="acn-us-cbwt";
        const std::string feature_acn_phone= "acn-phone";
        const std::string feature_rcall_jp = "rcall-japan";
        const std::string acn_phone_type_1   = "Type1";
        const std::string acn_phone_type_2   = "Type2";
        const std::string acn_phone_postponse   = "postponse";
        const std::string acn_phone_resumse   = "resumse";
        const std::string feature_acn_uae   = "acn_ke_kg";

        const uint32_t us_processor_callback_id = 1;
        const uint32_t acn_processor_callback_id = 2;
        const uint32_t japan_processor_callback_id = 3;

        const std::string privacy_ecall_id = "010038";
        const std::string privacy_acn_us_id = "010037";
        const std::string privacy_acn_wphone_id = "010036";
        const std::string privacy_rcall_jp_id = "010040";

        const std::string subscription_acn_jp_id = "7";
        const std::string subscription_ecall_id = "8";
        const std::string subscription_acn_phone_heavy_id = "30";
        const std::string subscription_acn_phone_light_id = "31";
        const std::string subscription_rcall_jp_id = "9";
        const std::string subscription_dess_jp_id = "10";



        const uint32_t RCallJpTimeoutTime = 60;
        const uint32_t RCallJpPeriodicLedTime= 1;
        const uint32_t UsPreparingTime = 10;
        const uint32_t UsCyclicTime    = 500; // ms
        const uint32_t UsDelayIndicateTime = 1;
        const uint32_t UsEC_CANCEL_TIME    = 10500; // ms
		const uint32_t ECALL_STATUS_30S = 30000; //ms
        const uint32_t UsReconnectingTime= 10;
        const uint32_t UsRetryWaitTime   = 3;
        const uint32_t US_CONNECTION_TIME = 120;
        const uint32_t UsCallBackTime = 3600;
        const uint32_t UsCallBackAlarmId = 5634;
        const uint32_t eCBMTimer = 300000;
        const uint32_t DESS_CONFIRM_2S = 2000; //ms

        const uint8_t IcbCallStatus_endRcall= 0x00;
        const uint8_t IcbCallStatus_RcallEstablished= 0x01;

        const uint32_t DESS_ECALL_NO_REQUEST = 0U;
        const uint32_t DESS_ECALL_REQUEST  = 1U;
        const uint32_t DESS_CANCEL_ECALL_TRIGGER  = 2U;

//        const uint32_t acn_with_phone_light_id = 1;
//        const uint32_t acn_with_phone_normal_id = 2;
//        const uint32_t acn_us_id = 3;
//        const uint32_t ecall_us_id = 4;

        /**
         * This is the Update matrix for priority scenario.
         * The first value of each element will be the feature which is wanting to run.
         * The second value will be the feature which should terminate immidiately.
        **/
        static std::unordered_map< std::string, bool> updateMatrix{
            {"acn-phone;ecall-us", true},
            {"ecall_kj;rcall-japan", true},
            {"acn_kj;rcall-japan", true},
			{"dess_kj;rcall-japan", true}
        };

        constexpr static uint32_t SEND_RETRY_DELAY            = 500; // ms
        constexpr static uint32_t SEND_TEL_MUTE_DELAY         = 500; // ms

        struct ErrorType {
            constexpr static uint32_t memory             = 301;
            constexpr static uint32_t ethernet           = 302;
            constexpr static uint32_t led                = 303;
            constexpr static uint32_t location           = 304;
            constexpr static uint32_t callPrimaryNumber  = 305;
            constexpr static uint32_t callSecondaryNumber= 306;
            constexpr static uint32_t voiceInterupted    = 307;
            constexpr static uint32_t longPress          = 308;
            constexpr static uint32_t termination        = 309;// TODO: this will be removed due to change spec
            constexpr static uint32_t notificationStatus = 310;
            constexpr static uint32_t dataSendingTimeout = 311;
            constexpr static uint32_t mic                = 312;
            constexpr static uint32_t audio              = 313;
        };

        struct UsTimerId {
            constexpr static int PREPARING_TIME       = 1;
            constexpr static int DELAY_INDICATE       = 2;
            constexpr static int DELAY_INDICATE_ACN   = 2;
            constexpr static int C_EC_CANCELDISP_REQ  = 3;
            constexpr static int CONNECTION_TIME      = 4;
            constexpr static int RECONNECTING_TIME    = 5;
            constexpr static int CALLBACK_WAITING     = 6;
            constexpr static int RETRY_WAIT_TIME      = 7;

            constexpr static int SEND_DATA_TIME       = 8;
            constexpr static int CANCEL_DISP_TIME     = 9;
            constexpr static int DIALING_TIME         = 10;
			constexpr static int ECALL_STATUS_30S     = 11;
			constexpr static int ECALL_ECBM_TIMER     = 12;

            constexpr static int DESS_CONFIRM_TIMER_NO_REQUEST          = 13;
            constexpr static int DESS_CONFIRM_TIMER_REQUEST             = 14;
            constexpr static int ECALL_NOTIFICATION_TIMER_REQUEST       = 15;
            constexpr static int DESS_ECALL_STATUS_30S                  = 16;
            constexpr static int ECALL_NOTIFICATION_TIMER_NO_REQUEST    = 17;
            constexpr static int DESS_NOTIFICATION_CANCEL_ECALL_TRIGGER = 18;
            constexpr static int ECALL_NOTIFICATION_CANCEL_ECALL_TRIGGER = 19;
            constexpr static int MAX_ID               = 20; // Do not use

        };

        struct JapanTimerId {
            constexpr static int DIALING_TIME   = 0;
            constexpr static int SEND_DATA_TIME = 1;
            constexpr static int PERDIODIC_LED_TIME= 2;
            constexpr static int MAX_ID         = 3; // Do not use
        };

        struct AcnPhoneTimerId {
            constexpr static uint32_t SEND_DATA_TIME = 0;
            constexpr static uint32_t MAX_ID         = 1; // Do not use
        };

        struct ServiceId {
            constexpr static uint32_t POWER     = 101;
            constexpr static uint32_t TELEPHONY = 102;
            constexpr static uint32_t AUDIO     = 103;
            constexpr static uint32_t HMI       = 104;
            constexpr static uint32_t VCM       = 105;
            constexpr static uint32_t RIM       = 106;
            constexpr static uint32_t CONFIG    = 107;
            constexpr static uint32_t COMM      = 108;
            constexpr static uint32_t ALARM     = 109;
            constexpr static uint32_t SOMEIP     = 110;
        };

        struct ethernetState {
            constexpr static uint32_t ETHERNET_NOT_READY  = 0x30;
            constexpr static uint32_t ETHERNET_READY      = 0x31;
        };

        struct SLDDTestId {
            constexpr static uint32_t ECALL_TRIGGER             = 5001;
            constexpr static uint32_t ACN_TRIGGER               = 5002;
            constexpr static uint32_t ACN_LIGHT_TRIGGER         = 5003;
            constexpr static uint32_t ACN_WITH_PHONE_TRIGGER    = 5004;
            constexpr static uint32_t ENABLE_TEST_DROP_CALL     = 5005;
            constexpr static uint32_t DISABLE_TEST_DROP_CALL    = 5006;
            constexpr static uint32_t LONG_PRESS_SIGNAL         = 5007;

            constexpr static uint32_t RCALL_TRIGGER             = 5008;
            constexpr static uint32_t TEAR_DOWN                 = 5099;
            constexpr static uint32_t SOMEIP_RCALL_TRIGER       = 5100;

            constexpr static uint32_t FRONT_REC_CAN_CDS_TEST           = 5200;
            constexpr static uint32_t SIDE_REC_CAN_CDS_TEST            = 5201;
            constexpr static uint32_t REAR_REC_CAN_CDS_TEST            = 5202;
            constexpr static uint32_t ROLLOVER_REC_CAN_CDS_TEST        = 5203;

			constexpr static uint32_t ECALL_STATUS_ACK_0_TEST                  		   = 5600;
			constexpr static uint32_t ECALL_STATUS_ACK_1_TEST                  		   = 5601;
			constexpr static uint32_t ECALL_STATUS_ACK_2_TEST                  		   = 5602;
			constexpr static uint32_t MESS_STATUS_REQUEST_0_TEST                   	   = 5610;
			constexpr static uint32_t MESS_STATUS_REQUEST_1_TEST                   	   = 5611;
			constexpr static uint32_t MESS_STATUS_REQUEST_2_TEST                   	   = 5612;


        };
    } // namespace name

    namespace msg{
        // 100 -> APP manager messages
        constexpr static uint32_t APPM_BOOT_COMPLETED                              = 100;
        constexpr static uint32_t APPM_RESPONSE_QUERY                              = 101;
        constexpr static uint32_t APPM_FEATURE_STATUS_CHANGED                      = 102;
        constexpr static uint32_t APPM_REQUEST_ACTIONS                             = 103;

        // 200 -> HMI manager messages
        constexpr static uint32_t MSG_HMIM_ECALL_TRIGGER                               = 105;
        constexpr static uint32_t MSG_HMIM_LONG_PRESS_TRIGGER                          = 106;
        constexpr static uint32_t MSG_RECV_HMI                                         = 107;
        constexpr static uint32_t MSG_HMI_RETRY_SAVE_TRIGGER                           = 108;

        constexpr static uint32_t MSG_COMM_CDS                                         = 401;
        constexpr static uint32_t MSG_CDS_TRIGGER                                      = 402;
        constexpr static uint32_t MSG_COMM_WPHONE_CDS_LIGHT_TRIGGER                    = 403;
        constexpr static uint32_t MSG_WPHONE_CDS_TRIGGER_NORMAL                        = 404;
        constexpr static uint32_t MSG_COMM_RETRY_SAVE_CDS                              = 405;
        constexpr static uint32_t MSG_CDS_TRIGGER_AFTER_WAITING                        = 406;

        constexpr static uint32_t MSG_POWER_STATE_CHANGED                              = 110;
        constexpr static uint32_t MSG_POWER_MODE_CHANGED                               = 111;
        constexpr static uint32_t MSG_POWER_LOCK_CALLBACK                              = 112;
        constexpr static uint32_t MSG_CONFIG_CHANGED                                   = 113;
        constexpr static uint32_t MSG_TELEM_CALL_PRECISE_STATE_CHANGED                 = 114;
        constexpr static uint32_t MSG_TELEM_CALL_DISCONECTED                           = 115;
        constexpr static uint32_t MSG_AUDIO_PLAY_STATE_CHANGED                         = 116;
        constexpr static uint32_t MSG_RIM_GTC_RESPONSE                                 = 117;

        constexpr static uint32_t MSG_CM_WPHONE_TIMER_EXPIRED                          = 211;
        constexpr static uint32_t MSG_CM_US_TIMER_EXPIRED                              = 212;
        constexpr static uint32_t MSG_CM_US_REQUEST_TERMINATE_PROCESSOR                = 213;
        constexpr static uint32_t MSG_CM_US_REQUEST_TERMINATE_KEEP_VOICE               = 224;
        constexpr static uint32_t MSG_CM_US_REQUEST_QUERY_ACN                          = 225;
        constexpr static uint32_t MSG_CM_US_REQUEST_QUERY_ECALL                        = 226;
        constexpr static uint32_t MSC_CM_US_REQUEST_RETRY_PLAY_AUDIO                   = 227;
        constexpr static uint32_t US_END_ERROR_LED                                     = 228;

        constexpr static uint32_t RCALL_TURNOFF                                        = 500;
        constexpr static uint32_t RCALL_TRIGGER                                        = 501;
        constexpr static uint32_t RCALL_CANCEL                                         = 502;
        constexpr static uint32_t RCALL_TIMER_EXPIRED                                  = 503;
        constexpr static uint32_t RCALL_END_ERROR_LED                                  = 504;

        constexpr static uint32_t HTTP_SEND_ERROR                                      = 505;
        constexpr static uint32_t HTTP_RECV_RESPONSE                                   = 506;

        constexpr static uint32_t MSG_CM_BINDER_DIED                                   = 231;
        constexpr static uint32_t MSG_CM_REQUEST_RETRY_REGISTER_RECEIVER               = 232;
        constexpr static uint32_t MSG_CM_RETRY_REGISTER_AUDIO                          = 233;
        constexpr static uint32_t MSG_CM_TEST_DROP_CALL                                = 234;
        constexpr static uint32_t MSG_CM_TEAR_DOWN                                     = 235;
        constexpr static uint32_t MSG_CM_RETRY_REGISTER_ALARM                          = 236;
        constexpr static uint32_t MSG_CM_RETRY_REGISTER_SOMEIP                         = 237;
        constexpr static uint32_t MSG_SOMEIP_PROVIDER_ETH_READY                        = 238;
        constexpr static uint32_t MSG_SOMEIP_REQUEST_CLEARUP_R_CALL                    = 239;
        constexpr static uint32_t MSG_CM_REQUEST_SEND_TEL_MUTE_SIGNAL                  = 240;

        constexpr static uint32_t MSG_FRONT_REC_CAN_CDS_TEST                  		   = 300;
        constexpr static uint32_t MSG_SIDE_REC_CAN_CDS_TEST                   		   = 301;
        constexpr static uint32_t MSG_REAR_REC_CAN_CDS_TEST                   		   = 302;
        constexpr static uint32_t MSG_ROLLOVER_REC_CAN_CDS_TEST               		   = 303;

#ifdef CAN_TYPE_GEN3
        constexpr static uint32_t MES_STATUS_NOTIFICATION                            = 410;
        constexpr static uint32_t MES_RECEIVED_VALUE                                 = 411;
        constexpr static uint32_t MES_ECALL_STATUS_NOTIFICATION                      = 412;
        constexpr static uint32_t MES_ECALL_RECEIVED_VALUE                           = 413;

        constexpr static uint32_t MSG_DESS_TRIGGER                                   = 414;
        constexpr static uint32_t MSG_DESS_TRIGGER_WAITING                           = 415;
#endif



        static std::unordered_map<uint32_t, std::string> name {
            {APPM_BOOT_COMPLETED                            ,"APPM_BOOT_COMPLETED"                           },
            {APPM_RESPONSE_QUERY                            ,"APPM_RESPONSE_QUERY"                           },
            {APPM_FEATURE_STATUS_CHANGED                    ,"APPM_FEATURE_STATUS_CHANGED"                   },
            {APPM_REQUEST_ACTIONS                           ,"APPM_REQUEST_ACTIONS"                          },
            {MSG_HMIM_ECALL_TRIGGER                         ,"MSG_HMIM_ECALL_TRIGGER"                        },
            {MSG_HMIM_LONG_PRESS_TRIGGER                    ,"MSG_HMIM_LONG_PRESS_TRIGGER"                   },
            {MSG_RECV_HMI                                   ,"MSG_RECV_HMI"                                  },
            {MSG_HMI_RETRY_SAVE_TRIGGER                     ,"MSG_HMI_RETRY_SAVE_TRIGGER"                    },
            {MSG_COMM_CDS                                   ,"MSG_COMM_CDS"                                  },
            {MSG_CDS_TRIGGER                                ,"MSG_CDS_TRIGGER"                               },
            {MSG_COMM_WPHONE_CDS_LIGHT_TRIGGER              ,"MSG_COMM_WPHONE_CDS_LIGHT_TRIGGER"             },
            {MSG_WPHONE_CDS_TRIGGER_NORMAL                  ,"MSG_WPHONE_CDS_TRIGGER_NORMAL"                 },
            {MSG_COMM_RETRY_SAVE_CDS                        ,"MSG_COMM_RETRY_SAVE_CDS"                       },
            {MSG_CDS_TRIGGER_AFTER_WAITING                  ,"MSG_CDS_TRIGGER_AFTER_WAITING"                 },
            {MSG_POWER_STATE_CHANGED                        ,"MSG_POWER_STATE_CHANGED"                       },
            {MSG_POWER_MODE_CHANGED                         ,"MSG_POWER_MODE_CHANGED"                        },
            {MSG_POWER_LOCK_CALLBACK                        ,"MSG_POWER_LOCK_CALLBACK"                       },
            {MSG_CONFIG_CHANGED                             ,"MSG_CONFIG_CHANGED"                            },
            {MSG_TELEM_CALL_PRECISE_STATE_CHANGED           ,"MSG_TELEM_CALL_PRECISE_STATE_CHANGED"          },
            {MSG_TELEM_CALL_DISCONECTED                     ,"MSG_TELEM_CALL_DISCONECTED"                    },
            {MSG_AUDIO_PLAY_STATE_CHANGED                   ,"MSG_AUDIO_PLAY_STATE_CHANGED"                  },
            {MSG_RIM_GTC_RESPONSE                           ,"MSG_RIM_GTC_RESPONSE"                          },
            {MSG_CM_WPHONE_TIMER_EXPIRED                    ,"MSG_CM_WPHONE_TIMER_EXPIRED"                   },
            {MSG_CM_US_TIMER_EXPIRED                        ,"MSG_CM_US_TIMER_EXPIRED"                       },
            {MSG_CM_US_REQUEST_TERMINATE_PROCESSOR          ,"MSG_CM_US_REQUEST_TERMINATE_PROCESSOR"         },
            {MSG_CM_US_REQUEST_TERMINATE_KEEP_VOICE         ,"MSG_CM_US_REQUEST_TERMINATE_KEEP_VOICE"        },
            {MSG_CM_US_REQUEST_QUERY_ACN                    ,"MSG_CM_US_REQUEST_QUERY_ACN"                   },
            {MSG_CM_US_REQUEST_QUERY_ECALL                  ,"MSG_CM_US_REQUEST_QUERY_ECALL"                 },
            {MSC_CM_US_REQUEST_RETRY_PLAY_AUDIO             ,"MSC_CM_US_REQUEST_RETRY_PLAY_AUDIO"            },
            {US_END_ERROR_LED                               ,"US_END_ERROR_LED"                              },
            {RCALL_TURNOFF                                  ,"RCALL_TURNOFF"                                 },
            {RCALL_TRIGGER                                  ,"RCALL_TRIGGER"                                 },
            {RCALL_CANCEL                                   ,"RCALL_CANCEL"                                  },
            {RCALL_TIMER_EXPIRED                            ,"RCALL_TIMER_EXPIRED"                           },
            {RCALL_END_ERROR_LED                            ,"RCALL_END_ERROR_LED"                           },
            {HTTP_SEND_ERROR                                ,"HTTP_SEND_ERROR"                               },
            {HTTP_RECV_RESPONSE                             ,"HTTP_RECV_RESPONSE"                            },
            {MSG_CM_BINDER_DIED                             ,"MSG_CM_BINDER_DIED"                            },
            {MSG_CM_REQUEST_RETRY_REGISTER_RECEIVER         ,"MSG_CM_REQUEST_RETRY_REGISTER_RECEIVER"        },
            {MSG_CM_RETRY_REGISTER_AUDIO                    ,"MSG_CM_RETRY_REGISTER_AUDIO"                   },
            {MSG_CM_TEST_DROP_CALL                          ,"MSG_CM_TEST_DROP_CALL"                         },
            {MSG_CM_TEAR_DOWN                               ,"MSG_CM_TEAR_DOWN"                              },
            {MSG_CM_RETRY_REGISTER_ALARM                    ,"MSG_CM_RETRY_REGISTER_ALARM"                   },
            {MSG_CM_RETRY_REGISTER_SOMEIP                   ,"MSG_CM_RETRY_REGISTER_SOMEIP"                  },
            {MSG_SOMEIP_PROVIDER_ETH_READY                  ,"MSG_SOMEIP_PROVIDER_ETH_READY"                 },
            {MSG_SOMEIP_REQUEST_CLEARUP_R_CALL              ,"MSG_SOMEIP_REQUEST_CLEARUP_R_CALL"             },
            {MSG_CM_REQUEST_SEND_TEL_MUTE_SIGNAL            ,"MSG_CM_REQUEST_SEND_TEL_MUTE_SIGNAL"           },
#ifdef CAN_TYPE_GEN3
            {MES_STATUS_NOTIFICATION                        ,"MES_STATUS_NOTIFICATION"                       },
            {MES_RECEIVED_VALUE                             ,"MES_RECEIVED_VALUE"                            },
            {MES_ECALL_STATUS_NOTIFICATION                  ,"MES_ECALL_STATUS_NOTIFICATION"                  },


            {MSG_DESS_TRIGGER                               ,"MSG_DESS_TRIGGER"                              },
            {MSG_DESS_TRIGGER_WAITING                       ,"MSG_DESS_TRIGGER_WAITING"                      },
#endif
            {MSG_FRONT_REC_CAN_CDS_TEST            			,"MSG_FRONT_REC_CAN_CDS_TEST"           		 },
			{MSG_SIDE_REC_CAN_CDS_TEST            			,"MSG_SIDE_REC_CAN_CDS_TEST"           			 },
			{MSG_REAR_REC_CAN_CDS_TEST            			,"MSG_REAR_REC_CAN_CDS_TEST"           			 },
			{MSG_ROLLOVER_REC_CAN_CDS_TEST            		,"MSG_ROLLOVER_REC_CAN_CDS_TEST"           		 }

        };
    }

    namespace serviceinfo{
        static std::unordered_set<uint32_t> us_market_name        = {1,2,3};
        static std::unordered_set<uint32_t> acn_phone_market_name = {5, 6,7,9, 10, 11, 14,15,16,17,18, 20};
        static std::unordered_set<uint32_t> rcall_market_name     = {4};
        struct AudioState {
            constexpr static uint8_t IDLE      = 0;
            constexpr static uint8_t PLAYING   = 1;
        };
        static std::unordered_map<uint32_t, std::string> audioStatePrinter = {
            {AudioState::IDLE   , "IDLE"},
            {AudioState::PLAYING, "PLAYING"}
        };

        struct CallDisconectedReason{ // TODO: check "services/ApplicationManagerService/IApplicationManagerServiceType.h"
            constexpr static uint32_t NORMAL      = 2;
            constexpr static uint32_t LOCAL       = 3;
            constexpr static uint32_t INVALID_NUMBER = 7;
        };

        static std::unordered_map<uint32_t, std::string> disconnectedReasonPrinter= {
            {CallDisconectedReason::NORMAL,"LAUNCH"  },
            {CallDisconectedReason::LOCAL ,"LOCAL"  },
            {CallDisconectedReason::INVALID_NUMBER ,"INVALID_NUMBER"  }
        };

        struct DataResResult{
            constexpr static uint32_t SUCCESS = EServerRespType::SUCCESS;
            constexpr static uint32_t FAILURE = EServerRespType::FAILURE;
            constexpr static uint32_t FAILURE_RETRY = EServerRespType::FAILURE_RETRY;
        };

        struct AppMgrRequestResult {
            constexpr static uint32_t LAUNCH   = FeatureAction::LAUNCH;
            constexpr static uint32_t IGNORE   = FeatureAction::IGNORE;
            constexpr static uint32_t UPDATE   = FeatureAction::UPDATE;
            constexpr static uint32_t POSTPONE = FeatureAction::POSTPONE;
            constexpr static uint32_t TRIGGER  = FeatureAction::TRIGGER;
        };

        static std::unordered_map<uint32_t, std::string> appMgrRsPrinter= {
            {AppMgrRequestResult::LAUNCH   ,"LAUNCH"  },
            {AppMgrRequestResult::IGNORE   ,"IGNORE"  },
            {AppMgrRequestResult::UPDATE   ,"UPDATE"  },
            {AppMgrRequestResult::POSTPONE ,"POSTPONE"},
            {AppMgrRequestResult::TRIGGER  ,"TRIGGER" }
        };

        struct AppMgrFeatureStatus {
            constexpr static uint32_t ON  = FeatureStatus::ON;
            constexpr static uint32_t OFF = FeatureStatus::OFF;
        };

        struct SysPostFeatureStatus {
            constexpr static uint32_t ON  = SYS_POST_FEATURE_STATUS_ON;
            constexpr static uint32_t OFF = SYS_POST_FEATURE_STATUS_OFF;
        };

        // ACC, IG1, Status
        struct PowerStatus{
            constexpr static int32_t MCU_STATUS_UNKNOWN = MCU_VALUE_STATUS::MCU_STATUS_UNKNOWN;
            constexpr static int32_t MCU_STATUS_OFF     = MCU_VALUE_STATUS::MCU_STATUS_OFF;
            constexpr static int32_t MCU_STATUS_ON      = MCU_VALUE_STATUS::MCU_STATUS_ON;
            constexpr static int32_t MCU_STATUS_MAX     = MCU_VALUE_STATUS::MCU_STATUS_MAX;
        };



    }
}

#endif
