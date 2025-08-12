/**
 * @copyright
 * Copyright (c) 2025 by LG Electronics Inc.
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
 * @file        NAECallStates.cpp
 * @author      eunbi.lee@lge.com
 * @version     1.0.00
*/
#include "NAECallStates.h"
#include "ECallState.h"
#include "NAECallProcessor.h"
#include "ECallVariableRepo.h"
#include "InternalParamsDef.h"
#include "../../../core/utils/ParamsDef.h"
#include "../../../core/services/AudioManagerAdapter.h"
#include "../../../core/services/PowerManagerAdapter.h"
#include "../../../core/services/HMIManagerAdapter.h"
#include "../../../core/services/TelephoneManagerAdapter.h"
#include "../../../core/services/ConfigurationManagerAdapter.h"
#include "../../../core/services/ApplicationManagerAdapter.h"
#include "../../../core/services/VehicleManagerAdapter.h"
namespace ecallapp {


void BootupState::enter(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Entering BOOTUP state"));
}
void BootupState::exit(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Exiting BOOTUP state"));
}
void BootupState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    switch (event) {
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_BOOT_COMPLETED):
        {
            LOG_I(DLT_STRING("BOOTUP:: RECV_MSG_BOOT_COMPLETED"));
            auto& repo = ECallVariableRepo::getInstance();
            repo.updateIgAccCondition();
            context->doNotifySOSNoRequest();
            bool isRecover = context->doECallRecovery();
            if (isRecover == false) {
                context->clearSignalReceived();
                if (repo.getIgCondition()) {
                    context->requestLedEcallReady();
                    context->changeState(ecall_state::STANDBY);
                }
                else {
                    context->turnOffCurrentLed();
                    context->changeState(ecall_state::POWER_OFF);
                }
            }
            else {
                context->changeState(ecall_state::RECOVERY);
            }
        }
        break;

        default:
            LOG_I(DLT_STRING("BOOTUP:: Unhandled event: "), DLT_UINT(event));
        break;
    }
}


void RecoveryState::enter(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Entering RECOVERY state"));
}
void RecoveryState::exit(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Exiting RECOVERY state"));
    mRecoveryECallState = 0;
}
void RecoveryState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    auto& repo = ECallVariableRepo::getInstance();

    switch (event) {
        case ENUM_TO_UINT32(msg::AppMSG::RECOVER_ECALL_TO_CANCEL_PERIOD) :
        {
            LOG_I(DLT_STRING("RECOVERY:: RECOVER_ECALL_TO_CANCEL_PERIOD"), DLT_UINT(event));
            repo.setECallType(eventParam1);
            context->doRecoveringAtSignalDetectingTimePartOfMSD();
            context->queryActionForFeature(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], true);
            context->changeState(ecall_state::CANCEL_PERIOD);
        }
        break;

        case ENUM_TO_UINT32(msg::AppMSG::RECOVER_ECALL_TO_DATA_TRANSMISSION) :
        {
            LOG_I(DLT_STRING("RECOVERY:: RECOVER_ECALL_TO_DATA_TRANSMISSION"), DLT_UINT(event));
            repo.setECallType(eventParam1);
            context->doRecoveringMSD();
            context->queryActionForFeature(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], true);
            context->performActionsAtLaunchingTime(true); //isRecover = true
            if (repo.getECallType() == ecall_type::ACN || repo.getECallType() == ecall_type::DESS) {
                context->requestBUBModeACNRunning();
            }
            else  {
                context->doNotifySOSRequest();
            }
            mRecoveryECallState = ecall_state::MSD_TRANSMISSION;
        }
        break;

        case ENUM_TO_UINT32(msg::AppMSG::RECOVER_ECALL_TO_CALL_CONNECT) :
        {
            LOG_I(DLT_STRING("RECOVERY:: RECOVER_ECALL_TO_CALL_CONNECT (skip MSD)"), DLT_UINT(event));
            repo.setECallType(eventParam1);
            context->doRecoveringTelNo();
            context->queryActionForFeature(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], true);
            context->performActionsAtLaunchingTime(true); //isRecover = true
            if (repo.getECallType() == ecall_type::ACN || repo.getECallType() == ecall_type::DESS) {
                context->requestBUBModeACNRunning();
            }
            else  {
                context->doNotifySOSRequest();
            }
            mRecoveryECallState = ecall_state::CALL_CONNECTING;
        }
        break;

        case ENUM_TO_UINT32(msg::AppMSG::RECOVER_ECALL_TO_CALLBACK_WAITING) :
        {
            LOG_I(DLT_STRING("RECOVERY:: RECOVER_ECALL_TO_CALLBACK_WAITING"), DLT_UINT(event));
            repo.setECallType(eventParam1);
            context->calculateT9RemainingTime(ecalltimer::CBWT);
            int64_t cbwtCountdown = context->getRemainingTime(ecalltimer::CBWT);
            if (cbwtCountdown == 0) {
                context->clearSignalReceived();
                if (repo.getIgCondition()) {
                    context->requestLedEcallReady();
                    context->changeState(ecall_state::STANDBY);
                }
                else {
                    context->turnOffCurrentLed();
                    context->changeState(ecall_state::POWER_OFF);
                }
            }
            else {
                context->queryActionForFeature(ecall_feature::appfeature[ecall_feature::CALLBACK_WAITING][repo.getECallType()], true);
                if (repo.getECallType() == ecall_type::ACN) {
                    context->sendPostACNUS_ON_OFF(ecallapp::ACNUS_ON);
                    context->requestBUBModeACNRunning();
                }
                else if (repo.getECallType() == ecall_type::DESS) {
                    context->requestBUBModeACNRunning();
                }
                context->performsetCallBackWaitingTimer();
                mRecoveryECallState = ecall_state::CALLBACK_WAITING;
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::APPM_REQUEST_LAUNCH):
        {
            LOG_I(DLT_STRING("RECOVERY::APPM_REQUEST_LAUNCH"));
            if (mRecoveryECallState == ecall_state::MSD_TRANSMISSION) {
                context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], true);
                context->changeState(ecall_state::MSD_TRANSMISSION);
            }
            else if (mRecoveryECallState == ecall_state::CALL_CONNECTING) {
                context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], true);
                bool isNeedWaiting = context->clearCurrentCall();
                if (isNeedWaiting) {
                    context->changeState(ecall_state::CLEAR_CURRENT_CALL);
                } else {
                    context->changeState(ecall_state::CALL_CONNECTING);
                }
            }
            else if (mRecoveryECallState == ecall_state::CALLBACK_WAITING) {
                context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::CALLBACK_WAITING][repo.getECallType()], true);
                context->changeState(ecall_state::CALLBACK_WAITING);
            }
            else {
                LOG_I(DLT_STRING("RECOVERY:: APPM_REQUEST_LAUNCH received w/o RECOVER_ECALL_TO event"));
            }
        }
        break;

        default:
            LOG_I(DLT_STRING("RECOVERY:: Unhandled event: "), DLT_UINT(event));
            break;
    }

}


void PowerOffState::enter(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Entering POWER_OFF state"));
}
void PowerOffState::exit(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Exiting POWER_OFF state"));
}
void PowerOffState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    switch (event) {
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_HMI_BUTTON_SHORT_PRESS):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER):
        {
            LOG_I(DLT_STRING("POWER_OFF:: sendAck only : "), DLT_UINT(event));
            context->doSignalAckOnActiveCall(event, eventParam1, eventParam2);
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::MSG_POWER_STATE_CHANGED):
        {
            LOG_I(DLT_STRING("POWER_OFF:: MSG_POWER_STATE_CHANGED"));
            auto& repo = ECallVariableRepo::getInstance();
            if (repo.getIgCondition()) {
                context->requestLedEcallReady();
                context->changeState(ecall_state::STANDBY);
            }
            else {
                context->turnOffCurrentLed();
            }
        }
        break;

        default:
            LOG_I(DLT_STRING("POWER_OFF:: Unhandled event: "), DLT_UINT(event));
            break;
    }
}


void StandbyState::enter(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Entering STANDBY state"));
}
void StandbyState::exit(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Exiting STANDBY state"));
}
void StandbyState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    auto& repo = ECallVariableRepo::getInstance();
    switch (event) {
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH):
        {
            LOG_I(DLT_STRING("STANDBY:: RECV_MSG_CDS_HEAVY_CRASH"));
            if (repo.getAccCondition()) {
                LOG_I(DLT_STRING("STANDBY:: eCall is triggered"));
                repo.setECallType(ecall_type::ACN);
                context->queryActionForFeature(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], true);
                if (context->confirmSignalReceived(ecall_type::ACN) == true) {
                    context->sendCDSAck(eventParam1, eventParam2);
                }
                context->changeState(ecall_state::CANCEL_PERIOD);
            }
            else {
                context->sendCDSAck(eventParam1, eventParam2);
                LOG_I(DLT_STRING("STANDBY::eCall is triggered, but acc off"));
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER):
        {
            LOG_I(DLT_STRING("STANDBY:: RECV_MSG_SRS_ECALL_TRIGGER"));
            if (repo.getAccCondition()) {
                LOG_I(DLT_STRING("STANDBY:: eCall is triggered"));
                repo.setECallType(ecall_type::ACN);
                context->queryActionForFeature(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], true);
                (void)context->confirmSignalReceived(ecall_type::ACN);
                context->changeState(ecall_state::CANCEL_PERIOD);
            }
            else {
                LOG_I(DLT_STRING("STANDBY::eCall is triggered, but acc off"));
            }
        }
        break;

        case ENUM_TO_UINT32(msg::AppMSG::DESS_ECALL_REQUEST_MSG):
        {
            LOG_I(DLT_STRING("STANDBY:: DESS_ECALL_REQUEST_MSG"));
            if (repo.getAccCondition()) {
                LOG_I(DLT_STRING("STANDBY:: eCall(DESS) is triggered"));
                repo.setECallType(ecall_type::DESS);
                context->queryActionForFeature(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], true);
                (void)context->confirmSignalReceived(ecall_type::DESS);
                context->changeState(ecall_state::CANCEL_PERIOD);
            }
            else {
                LOG_I(DLT_STRING("STANDBY:: eCall(DESS) is triggered, but acc off"));
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_HMI_BUTTON_SHORT_PRESS):
        {
            LOG_I(DLT_STRING("STANDBY:: RECV_MSG_HMI_BUTTON_SHORT_PRESS"));
            auto& repo = ECallVariableRepo::getInstance();
            if (repo.getAccCondition() && (repo.getProtectionMode() == false)) {
                LOG_I(DLT_STRING("STANDBY::manual eCall is triggered"));
                repo.setECallType(ecall_type::SOS);
                context->queryActionForFeature(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], true);
                if (context->confirmSignalReceived(ecall_type::SOS) == true) {
                    context->sendSOSAck(eventParam1, eventParam2);
                }
                context->doNotifySOSRequest();
                context->changeState(ecall_state::CANCEL_PERIOD);
            }
            else {
                context->sendSOSAck(eventParam1, eventParam2);
                LOG_I(DLT_STRING("STANDBY::manual eCall is triggered, but acc off"));
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::MSG_POWER_STATE_CHANGED):
        {
            LOG_I(DLT_STRING("STANDBY:: MSG_POWER_STATE_CHANGED"));
            if (!repo.getIgCondition()) {
                context->turnOffCurrentLed();
                context->changeState(ecall_state::POWER_OFF);
            }
        }
        break;

        default:
            LOG_I(DLT_STRING("STANDBY:: Unhandled event: "), DLT_UINT(event));
            break;
    }
}


void CancelPeriodState::enter(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Entering CANCEL_PERIOD state"));
    context->doResetCBWTFlags();
    auto& repo = ECallVariableRepo::getInstance();
    context->startCancellingTimer();
    context->sendBCANCancelDisplay(1U);
    if (repo.getECallType() == ecall_type::ACN || repo.getECallType() == ecall_type::DESS) {
        context->requestBUBModeACNRunning();
    }
    else {
        context->requestBUBModeECallRunning();
    }
    context->performActionsAtLaunchingTime();
    context->collectIntialData();
}
void CancelPeriodState::exit(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Exiting CANCEL_PERIOD state"));
    context->stopCancellingTimer();
    context->sendBCANCancelDisplay(0U);
}
void CancelPeriodState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    auto& repo = ECallVariableRepo::getInstance();
    switch (event) {
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_HMI_BUTTON_SHORT_PRESS):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER):
        {
            LOG_I(DLT_STRING("CANCEL_PERIOD:: sendAck only : "), DLT_UINT(event));
            context->doSignalAckOnActiveCall(event, eventParam1, eventParam2);
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_HMI_BUTTON_LONG_PRESS):
        {
            LOG_I(DLT_STRING("CANCEL_PERIOD:: HMI_BUTTON_LONG_PRESS"));
            context->stopCancellingTimer();
            context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], false);
            context->performECallTermination();

            if (repo.getECallType() == ecall_type::SOS || repo.getECallType() == ecall_type::DESS) {
                context->doNotifySOSCancel();
            }
            if (repo.getIgCondition()) {
                context->changeState(ecall_state::STANDBY);
            }
            else {
                context->turnOffCurrentLed();
                context->changeState(ecall_state::POWER_OFF);
            }
        }
        break;

        case ENUM_TO_UINT32(msg::AppMSG::DESS_ECALL_CANCEL_MSG) :
        {
            LOG_I(DLT_STRING("CANCEL_PERIOD:: DESS_ECALL_CANCEL_MSG"));
            if (repo.getECallType() == ecall_type::DESS) {
                context->stopCancellingTimer();
                context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], false);
                context->performECallTermination();
                if (repo.getIgCondition()) {
                    context->changeState(ecall_state::STANDBY);
                }
                else {
                    context->turnOffCurrentLed();
                    context->changeState(ecall_state::POWER_OFF);
                }
            }
            else {
                LOG_I(DLT_STRING("CANCEL_PERIOD:: DESS_ECALL_CANCEL_MSG, but not DESS call"));
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::MSG_TIMER_SRV):
        {
            if(eventParam1 == ecall_na_timer::CANCELLING_TIMER_ID) {
                LOG_I(DLT_STRING("CANCEL_PERIOD:: CANCELLING_TIMER_ID expired"));
                context->changeState(ecall_state::MSD_TRANSMISSION);
            }
            else{
                LOG_I(DLT_STRING("CANCEL_PERIOD:: MSG_TIMER_SRV, eventParam1 = "), DLT_UINT32(eventParam1));
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::APPM_REQUEST_LAUNCH):
        {
            LOG_I(DLT_STRING("CANCEL_PERIOD:: APPM_REQUEST_LAUNCH"));
            context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], true);
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::APPM_REQUEST_IGNORE):
        {
            LOG_I(DLT_STRING("CANCEL_PERIOD:: APPM_REQUEST_IGNORE"));
            context->stopCancellingTimer();
            context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], false);
            context->performECallTermination();
            if (repo.getECallType() == ecall_type::SOS) {
                context->doNotifySOSCancel();
            }

            if (repo.getIgCondition()) {
                context->changeState(ecall_state::STANDBY);
            }
            else {
                context->turnOffCurrentLed();
                context->changeState(ecall_state::POWER_OFF);
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::APPM_REQUEST_TERMINATE):
        {
            LOG_I(DLT_STRING("CANCEL_PERIOD:: APPM_REQUEST_TERMINATE"));
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED):
            LOG_I(DLT_STRING("CANCEL_PERIOD:: RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED"));
            context->acquirePowerLock();
        break;

        default:
            LOG_I(DLT_STRING("CANCEL_PERIOD:: Unhandled event: "), DLT_UINT(event));
            break;
    }
}


void MsdPreparingState::enter(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Entering MSD_PREPARING state"));
}
void MsdPreparingState::exit(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Exiting MSD_PREPARING state"));
}
void MsdPreparingState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    switch (event) {
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_HMI_BUTTON_SHORT_PRESS):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER):
        {
            LOG_I(DLT_STRING("MSD_PREPARING:: sendAck only : "), DLT_UINT(event));
            context->doSignalAckOnActiveCall(event, eventParam1, eventParam2);
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_AUDIO_STOPPED_STATE):
            break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED):
            LOG_I(DLT_STRING("MSD_PREPARING:: RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED"));
            context->acquirePowerLock();
        break;

        default:
            LOG_I(DLT_STRING("MSD_PREPARING:: Unhandled event: "), DLT_UINT(event));
            break;
        }
}


void MsdTransmissionState::enter(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Entering MSD_TRANSMISSION state"));
    context->setAttemptDataCount(0);

    context->prepareMSD();

    context->performMSDTransmission();  // HTTP Request
}
void MsdTransmissionState::exit(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Exiting MSD_TRANSMISSION state"));
    context->performClearMSD();
    // Data send retry timer stop
    context->setAttemptDataCount(0);
}
void MsdTransmissionState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    auto& repo = ECallVariableRepo::getInstance();
    switch (event) {
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_HMI_BUTTON_SHORT_PRESS):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER):
        {
            LOG_I(DLT_STRING("MSD_TRANSMISSION:: sendAck only : "), DLT_UINT(event));
            context->doSignalAckOnActiveCall(event, eventParam1, eventParam2);
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_MSD_SUCCESS):
        {
            LOG_I(DLT_STRING("MSD_TRANSMISSION:: RECV_MSG_MSD_SUCCESS"));
            if(context->IsSLDDTest() && context->IsSLDDTest_DataTransmitFAIL()) {
                LOG_I(DLT_STRING("MSD_TRANSMISSION(SLDD):: attempt data count = "), DLT_UINT(context->getAttemptDataCount()));
                if (context->shouldRetryDataTransmission()) {
                    LOG_I(DLT_STRING("MSD_TRANSMISSION(SLDD):: retry data transmission"));
                } else {
                    LOG_I(DLT_STRING("MSD_TRANSMISSION(SLDD):: data transmission failed"));
                    context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], false);
                    context->clearSignalReceived();
                    if (repo.getIgCondition()) {
                        context->changeState(ecall_state::STANDBY);
                    } else {
                        context->changeState(ecall_state::POWER_OFF);
                    }
                }
            }
            else {
                bool isNeedWaiting = context->clearCurrentCall();
                if (isNeedWaiting) {
                    context->changeState(ecall_state::CLEAR_CURRENT_CALL);
                } else {
                    context->changeState(ecall_state::CALL_CONNECTING);
                }
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_MSD_FAILURE):
        {
            LOG_I(DLT_STRING("MSD_TRANSMISSION:: RECV_MSG_MSD_FAILURE"));
            if(context->IsSLDDTest() && context->IsSLDDTest_DataTransmitSUCCESS()) {
                LOG_I(DLT_STRING("MSD_TRANSMISSION(SLDD):: success data transmission"));
                context->changeState(ecall_state::CALL_CONNECTING);
            }
            else {
                LOG_I(DLT_STRING("MSD_TRANSMISSION:: attempt data count = "), DLT_UINT(context->getAttemptDataCount()));
                if (context->shouldRetryDataTransmission()) {
                    LOG_I(DLT_STRING("MSD_TRANSMISSION: MSD doesn't reach the server. Retrying MSD transmission..."));
                    context->performMSDTransmission();
                } else {
                    LOG_I(DLT_STRING("MSD_TRANSMISSION: MSD doesn't reach the server. Make the voice call now."));
                    bool isNeedWaiting = context->clearCurrentCall();
                    if (isNeedWaiting) {
                        LOG_I(DLT_STRING("MSD_TRANSMISSION: need to clear call"));
                        context->changeState(ecall_state::CLEAR_CURRENT_CALL);
                    } else {
                        LOG_I(DLT_STRING("MSD_TRANSMISSION: move to CallConnectingState"));
                        context->changeState(ecall_state::CALL_CONNECTING);
                    }
                }
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_AUDIO_STOPPED_STATE):
        {
            LOG_I(DLT_STRING("MSD_TRANSMISSION:: RECV_MSG_AUDIO_STOPPED_STATE"));
            auto& repo = ECallVariableRepo::getInstance();
            if (repo.getECallClearAudioState() == ecall_audio_state::CLEAR_TONE_2) {
                context->processAudioStopped(ecall_audio_state::CLEAR_TONE_2);
                repo.setECallClearAudioState(ecall_audio_state::UNCLEAR_TONE);
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED):
        {
            LOG_I(DLT_STRING("MSD_TRANSMISSION:: RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED"));
            context->acquirePowerLock();
        }
        break;

        default:
            LOG_I(DLT_STRING("MSD_TRANSMISSION:: Unhandled event: "), DLT_UINT(event));
            break;
    }
}


void ClearCurrentCallState::enter(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Entering CLEAR_CURRENT_CALL state"));
}
void ClearCurrentCallState::exit(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Exiting CLEAR_CURRENT_CALL state"));
}
void ClearCurrentCallState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    switch (event) {
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_HMI_BUTTON_SHORT_PRESS):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER):
        {
            LOG_I(DLT_STRING("CLEAR_CURRENT_CALL:: sendAck only : "), DLT_UINT(event));
            context->doSignalAckOnActiveCall(event, eventParam1, eventParam2);
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CALL_DISCONNECTED):
        {
            LOG_I(DLT_STRING("CLEAR_CURRENT_CALL:: RECV_MSG_CALL_DISCONNECTED"));
            context->stopClearingCurrentCall();
            context->changeState(ecall_state::CALL_CONNECTING);
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_AUDIO_STOPPED_STATE):
        {
            LOG_I(DLT_STRING("CLEAR_CURRENT_CALL:: RECV_MSG_AUDIO_STOPPED_STATE"));
            auto& repo = ECallVariableRepo::getInstance();
            if (repo.getECallClearAudioState() == ecall_audio_state::CLEAR_TONE_1) {
                context->processAudioStopped(ecall_audio_state::CLEAR_TONE_1);
                repo.setECallClearAudioState(ecall_audio_state::UNCLEAR_TONE);
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED):
        {
            LOG_I(DLT_STRING("CLEAR_CURRENT_CALL:: RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED"));
            context->acquirePowerLock();
        }
        break;

        default:
            LOG_I(DLT_STRING("CLEAR_CURRENT_CALL:: Unhandled event: "), DLT_UINT(event));
            break;
    }
}


void CallConnectingState::enter(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Entering CALL_CONNECTING state"));
    // Set up voice prompt to notify the user that a call is being attempted
    if (context->isAudioPlaying()) {
        auto& repo = ECallVariableRepo::getInstance();
        repo.setECallClearAudioState(ecall_audio_state::CLEAR_TONE_1);
        context->stopAudioFile();
    }
    else {
        context->playAudioFile(audio::TONE_1);
    }
    context->turnOffCurrentLed();
    // Set up an LED to notify the user that a call is being attempted
    context->requestLedPatternWithState(LED_ECALL_DIALING, true);
    context->setAttemptVoiceCount(0);
    // call dialling
    context->startCallDiallingTimer();
    context->performCallDialling();
}
void CallConnectingState::exit(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Exiting CALL_CONNECTING state"));
}
void CallConnectingState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    auto& repo = ECallVariableRepo::getInstance();
    switch (event) {
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_HMI_BUTTON_SHORT_PRESS):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER):
        {
            LOG_I(DLT_STRING("CALL_CONNECTING:: sendAck only : "), DLT_UINT(event));
            context->doSignalAckOnActiveCall(event, eventParam1, eventParam2);
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CALL_DISCONNECTED):
        {
            LOG_I(DLT_STRING("CALL_CONNECTING:: RECV_MSG_CALL_DISCONNECTED"));
            context->stopCallDiallingTimer();
            context->playLedPatternInMs(ecallapp::LED_ECALL_CONN_FAIL, 3000);
            if(context->shouldRetryVoiceCall()) {
                LOG_I(DLT_STRING("CALL_CONNECTING:: retry call connecting"));
                context->startCallRetryTimer();
            } else {
                LOG_I(DLT_STRING("CALL_CONNECTING:: call connecting failed"));
                context->performsetCallBackWaitingTimer();
                context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], false);
                context->changeState(ecall_state::CALLBACK_WAITING);
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CALL_CONNECTED):
        {
            LOG_I(DLT_STRING("CALL_CONNECTING:: RECV_MSG_CALL_CONNECTED"));
            context->stopCallDiallingTimer();
            context->changeState(ecall_state::CALL_CONNECTED);
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_AUDIO_STOPPED_STATE):
        {
            LOG_I(DLT_STRING("CALL_CONNECTING:: RECV_MSG_AUDIO_STOPPED_STATE"));
            if (repo.getECallClearAudioState() == ecall_audio_state::CLEAR_TONE_1) {
                context->processAudioStopped(ecall_audio_state::CLEAR_TONE_1);
                repo.setECallClearAudioState(ecall_audio_state::UNCLEAR_TONE);
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::MSG_TIMER_SRV):
        {
            if(eventParam1 == ecall_na_timer::CALL_RETRY_TIMER_ID) {
                LOG_I(DLT_STRING("CALL_CONNECTING:: CALL_RETRY_TIMER_ID expired"));
                bool isNeedWaiting = context->clearCurrentCall();
                if (isNeedWaiting) {
                    context->changeState(ecall_state::CLEAR_CURRENT_CALL);
                } else {
                    context->startCallDiallingTimer();
                    context->performCallDialling();
                    context->requestLedPatternWithState(ecallapp::LED_ECALL_DIALING, true);
                }
            }
            else if(eventParam1 == ecall_na_timer::CALL_DIALLING_TIMER_ID) {
                LOG_I(DLT_STRING("CALL_CONNECTING:: CALL_DIALLING_TIMER_ID expired"));
                context->checkAndEndCall();
            }
            else {
                LOG_I(DLT_STRING("CALL_CONNECTING:: unhandle timer id : "), DLT_UINT32(eventParam1));
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED):
        {
            LOG_I(DLT_STRING("CALL_CONNECTING:: RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED"));
            context->acquirePowerLock();
        }
        break;

        default:
            LOG_I(DLT_STRING("CALL_CONNECTING:: Unhandled event: "), DLT_UINT(event));
            break;
    }
}


void CallConnectedState::enter(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Entering CALL_CONNECTED state : doECallConnected "));
    context->turnOffCurrentLed();
    context->requestLedPatternWithState(ecallapp::LED_ECALL_VOICE_CONNECTING, true);
    if(context->isAudioPlaying()) {
        auto& repo = ECallVariableRepo::getInstance();
        repo.setECallClearAudioState(ecall_audio_state::CLEAR_TONE_3);
        (void)context->stopAudioFile();
    }
    else {
        (void)context->playAudioFile(audio::TONE_3);
    }
    context->openCallAudioPath();
}
void CallConnectedState::exit(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Exiting CALL_CONNECTED state"));
}
void CallConnectedState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    auto& repo = ECallVariableRepo::getInstance();
    switch (event) {
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_HMI_BUTTON_SHORT_PRESS):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER):
        {
            LOG_I(DLT_STRING("CALL_CONNECTED:: sendAck only : "), DLT_UINT(event));
            context->doSignalAckOnActiveCall(event, eventParam1, eventParam2);
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CALL_DISCONNECTED):
        {
            LOG_I(DLT_STRING("CALL_CONNECTED:: RECV_MSG_CALL_DISCONNECTED"));
            if(context->isCallDisconnectedNormal()) {
                LOG_I(DLT_STRING("CALL_CONNECTED:: normal disconnection"));
                context->performsetCallBackWaitingTimer();
                context->turnOffCurrentLed();
                context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], false);
                context->changeState(ecall_state::CALLBACK_WAITING);
            }
            else {
                LOG_I(DLT_STRING("CALL_CONNECTED: abnormal disconnection"));
                bool isNeedWaiting = context->clearCurrentCall();
                if (isNeedWaiting) {
                    LOG_I(DLT_STRING("CALL_CONNECTED: need to clear call"));
                    context->changeState(ecall_state::CLEAR_CURRENT_CALL);
                } else {
                    LOG_I(DLT_STRING("CALL_CONNECTED: move to CallConnectingState to retry voice dialing"));
                    context->changeState(ecall_state::CALL_CONNECTING);
                }
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_AUDIO_STOPPED_STATE):
        {
            LOG_I(DLT_STRING("CALL_CONNECTED:: RECV_MSG_AUDIO_STOPPED_STATE"));
            if (repo.getECallClearAudioState() == ecall_audio_state::CLEAR_TONE_3) {
                context->processAudioStopped(ecall_audio_state::CLEAR_TONE_3);
                repo.setECallClearAudioState(ecall_audio_state::UNCLEAR_TONE);
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED):
        {
            LOG_I(DLT_STRING("CALL_CONNECTED:: RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED"));
            context->acquirePowerLock();
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::MSG_TIMER_SRV):
        {
            LOG_I(DLT_STRING("CALL_CONNECTED:: No handled timeout : "), DLT_UINT32(eventParam1));
        }
        break;

        default:
            LOG_I(DLT_STRING("CALL_CONNECTED:: Unhandled event: "), DLT_UINT(event));
        break;
    }
}



void CallbackWaitingState::enter(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Entering CALLBACK_WAITING state"));
    context->transitionToCBWTMode();
}
void CallbackWaitingState::exit(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Exiting CALLBACK_WAITING state"));
}
void CallbackWaitingState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    auto& repo = ECallVariableRepo::getInstance();
    switch (event) {
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH):
        {
            LOG_I(DLT_STRING("CALLBACK_WAITING:: RECV_MSG_CDS_HEAVY_CRASH"));
            if (repo.getIgAccCondition()) {
                repo.setECallType(ecall_type::ACN);
                context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::CALLBACK_WAITING][repo.getECallType()], false);
                context->queryActionForFeature(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], true);
                if (context->confirmSignalReceived(ecall_type::ACN) == true) {
                    context->sendCDSAck(eventParam1, eventParam2);
                }
                context->changeState(ecall_state::CANCEL_PERIOD);
            }
            else {
                context->sendCDSAck(eventParam1, eventParam2);
                LOG_I(DLT_STRING("CALLBACK_WAITING:: can't ecall trigger due to IgAcc condition is false"));
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER):
        {
            LOG_I(DLT_STRING("CALLBACK_WAITING:: RECV_MSG_SRS_ECALL_TRIGGER"));
            if (repo.getIgAccCondition()) {
                repo.setECallType(ecall_type::ACN);
                context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::CALLBACK_WAITING][repo.getECallType()], false);
                context->queryActionForFeature(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], true);
                (void)context->confirmSignalReceived(ecall_type::ACN);
                context->changeState(ecall_state::CANCEL_PERIOD);
            }
            else {
                LOG_I(DLT_STRING("CALLBACK_WAITING:: can't ecall trigger due to IgAcc condition is false"));
            }
        }
        break;

        case ENUM_TO_UINT32(msg::AppMSG::DESS_ECALL_REQUEST_MSG):
        {
            LOG_I(DLT_STRING("CALLBACK_WAITING:: DESS_ECALL_REQUEST_MSG"));
            if (repo.getIgAccCondition()) {
                repo.setECallType(ecall_type::DESS);
                context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::CALLBACK_WAITING][repo.getECallType()], false);
                context->queryActionForFeature(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], true);
                (void)context->confirmSignalReceived(ecall_type::DESS);
                context->changeState(ecall_state::CANCEL_PERIOD);
            }
            else {
                LOG_I(DLT_STRING("CALLBACK_WAITING:: can't ecall(DESS) trigger due to IgAcc condition is false"));
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_HMI_BUTTON_SHORT_PRESS):
        {
            LOG_I(DLT_STRING("CALLBACK_WAITING:: RECV_MSG_HMI_BUTTON_SHORT_PRESS"));
            if (repo.getIgAccCondition() && (repo.getProtectionMode() == false)) {
                repo.setECallType(ecall_type::SOS);
                context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::CALLBACK_WAITING][repo.getECallType()], false);
                context->queryActionForFeature(ecall_feature::appfeature[ecall_feature::OUTGOING_CALL][repo.getECallType()], true);
                if (context->confirmSignalReceived(ecall_type::SOS) == true) {
                    context->sendSOSAck(eventParam1, eventParam2);
                }
                context->changeState(ecall_state::CANCEL_PERIOD);
            }
            else {
                context->sendSOSAck(eventParam1, eventParam2);
                LOG_I(DLT_STRING("CALLBACK_WAITING:: can't ecall trigger due to IgAcc condition is false"));
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_INCOMING_CALL_TRIGGER):
        {
            LOG_I(DLT_STRING("CALLBACK_WAITING:: RECV_MSG_INCOMING_CALL_TRIGGER"));
            context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::CALLBACK_WAITING][repo.getECallType()], false);
            context->queryActionForFeature(ecall_feature::appfeature[ecall_feature::INCOMING_CALL][repo.getECallType()], true);
            context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::INCOMING_CALL][repo.getECallType()], true);
            context->acquirePowerLock();
            context->requestLedPatternWithState(ecallapp::LED_ECALL_VOICE_CONNECTING, true);
            context->setAudioMute(true);
            context->answerRingingCall();
            context->changeState(ecall_state::CALLBACK_RUNNING);
        }
        break;


        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CBWT_TIMER_EXPIRED):
        {
            LOG_I(DLT_STRING("CALLBACK_WAITING:: RECV_MSG_CBWT_TIMER_EXPIRED"));
            context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::CALLBACK_WAITING][repo.getECallType()], false);
            context->performECallTermination();
            if (repo.getIgCondition()) {
                context->changeState(ecall_state::STANDBY);
            }
            else {
                context->turnOffCurrentLed();
                context->changeState(ecall_state::POWER_OFF);
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::APPM_REQUEST_IGNORE):
        {
            LOG_I(DLT_STRING("CALLBACK_WAITING:: APPM_REQUEST_IGNORE"));
        }
        break;

        default:
            LOG_I(DLT_STRING("CALLBACK_WAITING:: Unhandled event: "), DLT_UINT(event));
            break;
    }
}


void CallbackRunningState::enter(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Entering CALLBACK_RUNNING state"));
    if (context->isAudioPlaying()) {
        auto& repo = ECallVariableRepo::getInstance();
        repo.setECallClearAudioState(ecall_audio_state::CLEAR_TONE_3);
        context->stopAudioFile();
    }
    else {
        context->playAudioFile(audio::TONE_3); // Notification of Voice connection sound tone
    }
    context->openCallAudioPath();
    isCallbackModeExpired = false;
}
void CallbackRunningState::exit(NAECallStateMachine* context) {
    LOG_I(DLT_STRING("Exiting CALLBACK_RUNNING state"));
}
void CallbackRunningState::process(NAECallStateMachine* context, const uint32_t& event, const uint32_t eventParam1, const uint32_t eventParam2) {
    auto& repo = ECallVariableRepo::getInstance();
    switch (event) {
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_HMI_BUTTON_SHORT_PRESS):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CDS_HEAVY_CRASH):
        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_SRS_ECALL_TRIGGER):
        {
            LOG_I(DLT_STRING("CALLBACK_RUNNING:: sendAck only : "), DLT_UINT(event));
            context->doSignalAckOnActiveCall(event, eventParam1, eventParam2);
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CALL_DISCONNECTED):
        {
            LOG_I(DLT_STRING("CALLBACK_RUNNING:: RECV_MSG_CALL_DISCONNECTED"));
            if(isCallbackModeExpired == true) {
                context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::INCOMING_CALL][repo.getECallType()], false);
                context->performECallTermination();
                auto& repo = ECallVariableRepo::getInstance();
                if (repo.getIgCondition()) {
                    context->changeState(ecall_state::STANDBY);
                }
                else {
                    context->turnOffCurrentLed();
                    context->changeState(ecall_state::POWER_OFF);
                }
            }
            else {
                context->turnOffCurrentLed();
                context->setFeatureStatus(ecall_feature::appfeature[ecall_feature::INCOMING_CALL][repo.getECallType()], false);
                context->changeState(ecall_state::CALLBACK_WAITING);
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_CBWT_TIMER_EXPIRED):
        {
            LOG_I(DLT_STRING("CALLBACK_RUNNING:: RECV_MSG_CBWT_TIMER_EXPIRED"));
            isCallbackModeExpired = true;
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::MSG_TIMER_SRV):
        {
            LOG_I(DLT_STRING("CALLBACK_RUNNING:: No handled timeout : "), DLT_UINT32(eventParam1));
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_AUDIO_STOPPED_STATE):
        {
            if (repo.getECallClearAudioState() == ecall_audio_state::CLEAR_TONE_3) {
                context->processAudioStopped(ecall_audio_state::CLEAR_TONE_3);
                repo.setECallClearAudioState(ecall_audio_state::UNCLEAR_TONE);
            }
        }
        break;

        case ENUM_TO_UINT32(msg::MSG::RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED):
        {
            LOG_I(DLT_STRING("CALLBACK_RUNNING:: RECV_MSG_POWER_LOCK_ACQUISITION_EXPIRED"));
            context->acquirePowerLock();
        }
        break;

        default:
            LOG_I(DLT_STRING("CALLBACK_RUNNING:: Unhandled event: "), DLT_UINT(event));
            break;
    }
}


} // namespace ecallapp