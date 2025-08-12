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
 * @file        r_call_processor.cpp
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/

#include <chrono>
#include <thread>
#include <json-for-modern-cpp/json.hpp>

#include "r_call_jp_processor.h"
#include "../oem_call_app_constants.h"

//#include <services/TelephonyManagerService/utils/ByteArray.h>

#include "../oem_call_timer.h" // Timer
#include "../oem_call_handler.h"

#include "../logger.h"
#include "../data/data_builder_us.h"
#include "../data/data_builder_rcall.h"
#include "../services_adapter/rim_adapter.h"
#include <hal_system_tsu.h>
#include "../utils/utils_common.h"
#include <services/PowerManagerService/PowerIndexEnum.h>
#include <services/OperationModeManagerService/OperationModeManager.h>

namespace {
enum class State {
    NOT_INIT,
    OFF,
    DIAG,
    STANDBY,
    PREPARING_DATA,
    COMMUNICATING_DATA,
    VOICE_COMMUNICATION,
    TERMINATING,
    TURNING_OFF
};
std::unordered_map< State, std::string > statePrinter {
    {State::NOT_INIT           , "NOT_INIT"    },
    {State::OFF                , "OFF"    },
    {State::DIAG               , "DIAG"    },
    {State::STANDBY            , "STANDBY"        },
    {State::PREPARING_DATA     , "PREPARING_DATA"   },
    {State::COMMUNICATING_DATA , "COMMUNICATING_DATA"   },
    {State::VOICE_COMMUNICATION, "VOICE_COMMUNICATION"   },
    {State::TERMINATING        , "TERMINATING" },
    {State::TURNING_OFF        , "TURNING_OFF" }
};


enum class CleanupAction {
    WAIT_FOR_AUDIO_INDICATE_FINISHED,
    RESET_ALL_MODULE_IMMIDIATELY
};

std::unordered_map< CleanupAction, std::string > CleanupActionPrinter{
    {CleanupAction::RESET_ALL_MODULE_IMMIDIATELY     , "RESET_ALL_MODULE_IMMIDIATELY"    },
    {CleanupAction::WAIT_FOR_AUDIO_INDICATE_FINISHED , "WAIT_FOR_AUDIO_INDICATE_FINISHED"}
};

}

using namespace oc::self;
using namespace oc::msg;

namespace oc {

/**
     * @brief This contains the private properties of ACNPhoneProcessor class instance
     *
     */
struct RCallProcessor::Impl {
    State state = State::NOT_INIT;
    uint32_t callState = telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE;

    std::string payload = "";
    DataBuilderRCall builder;

    android::sp<OemCallHandler> spHandler;
    std::shared_ptr<AdapterLocator> spAdapters;
    int retryCallCounter = 0;
    int retryDataCounter = 0;
    bool isRequestedAudioRetry = false;
    bool isIndicatingError = false;
    uint32_t country = 0;
    uint32_t powerMode = 0;
	bool isEtherReady = false;
	bool isNeededToSendTelmuteAgain = false;
	bool currentMuteVal = false;

    /**
         * @brief This is the array that store all the pointer to instance of timer
         *
         */
    std::vector<android::sp<Timer>> timers;

    OEMCallTimer* timerHandler = nullptr;

    /**
         * @brief This is the method to update the state of processor. Prefer use
         *        this function instead of dirrecty modify the "state" for better
         *        debugging.
         *
         * @param newState
         */
    void updateState(State newState) {
        LOG_I(DLT_STRING("Processor in state "), DLT_STRING(statePrinter[state].c_str()), DLT_STRING(" and changed to "), DLT_STRING(statePrinter[newState].c_str()));
        LOG_M(DLT_STRING("State = "), DLT_STRING(statePrinter[newState].c_str()));
        state = newState;
        switch (state) {
        case State::OFF : // fall through
        case State::STANDBY:
            subscriptionCheck();
            break;
        case State::NOT_INIT :
        case State::DIAG :
        case State::PREPARING_DATA:
        case State::COMMUNICATING_DATA:
        case State::VOICE_COMMUNICATION:
        case State::TERMINATING:
        case State::TURNING_OFF:
            break;
        }
    }

    error_t getOperationMode(EOperationMode &op_mode)
    {
        return OperationModeManager::instance()->getOperationMode(op_mode);
    }

    /**
         * @brief tryPlayBeepTone This is the fucntion to request play specific beeptone till success
         * @param type
         */
    void requestPlayAudio(AudioManagerAdapter::AudioFile name) {
        if (spAdapters->spAudioManager->requestBPlayAudio(name)) {
            LOG_I(DLT_STRING("Request play audio Successfuly"));
            isRequestedAudioRetry = false;
        }
        else {
            LOG_I(DLT_STRING("Currently could not request play audio, try again after 500ms"));
            LOG_M(DLT_STRING("try again after 500ms"));
            isRequestedAudioRetry = true;
            spHandler->sendMessageDelayed(spHandler->obtainMessage(msg::MSC_CM_US_REQUEST_RETRY_PLAY_AUDIO, (uint32_t)name), 500);
        }
    }

    void indicateError(AudioManagerAdapter::AudioFile file, const std::string reason) {
        LOG_E(DLT_STRING(reason.c_str()));
        isIndicatingError = true;
        updateState(State::TERMINATING);
        if (
                spAdapters->spAudioManager->isAudioPlaying() &&
                !spAdapters->spAudioManager->requestBStopAudio()
                ){
            cleanUp(CleanupAction::RESET_ALL_MODULE_IMMIDIATELY, "Cannot stop play audio");
            return;
        }
        requestPlayAudio(file); // TODO: find audio file
        spAdapters->spHmiManager->requestPattern(HmiManagerAdapter::Pattern::JP_RCALL_RUNNING, false);
        spAdapters->spHmiManager->requestPattern(HmiManagerAdapter::Pattern::JP_RCALL_ERROR, true);
        spHandler->sendMessageDelayed(spHandler->obtainMessage(msg::RCALL_END_ERROR_LED), 5000);
    }

    void requestScreen(const std::string screenName) {
        LOG_I(DLT_STRING("Request screen to NAVI: "), DLT_STRING(screenName.c_str()));
    }

    void cleanUp(CleanupAction action, std::string reason) {
        updateState(State::TURNING_OFF);
        LOG_I(DLT_STRING("START CLEANUP, REASON = "), DLT_STRING(reason.c_str()), DLT_STRING(", ACTION = "), DLT_STRING(CleanupActionPrinter.at(action).c_str()));
        spAdapters->spHmiManager->requestPattern(HmiManagerAdapter::Pattern::JP_RCALL_RUNNING, false);
        if (action == CleanupAction::RESET_ALL_MODULE_IMMIDIATELY) {
            cleanUpCalling();
            cleanUpAudio();
            cleanUpOtherModule();
            spHandler->obtainMessage(msg::RCALL_TURNOFF, (uint32_t)action)->sendToTarget();
        }
    }

    void cleanUpBySomeIP(CleanupAction action, std::string reason) {
        LOG_I(DLT_STRING("cleanUpBySomeIP"));
        if (state != State::OFF && state != State::STANDBY) {
            updateState(State::TURNING_OFF);
            LOG_I(DLT_STRING("START CLEANUP, REASON = "), DLT_STRING(reason.c_str()), DLT_STRING(", ACTION = "), DLT_STRING(CleanupActionPrinter.at(action).c_str()));
            spAdapters->spHmiManager->requestPattern(HmiManagerAdapter::Pattern::JP_RCALL_RUNNING, false);
            if (action == CleanupAction::RESET_ALL_MODULE_IMMIDIATELY) {
                cleanUpCalling();
                cleanUpAudio();
                cleanUpOtherModule();
                spHandler->obtainMessage(msg::RCALL_TURNOFF, (uint32_t)action)->sendToTarget();
            }
        } else {
            LOG_I(DLT_STRING("state = ??"));
        }
    }

    void cleanUpCalling() {
        if (spAdapters->spTelephoneManager != nullptr) spAdapters->spTelephoneManager->stop();
        spAdapters->spAudioManager->requestCloseCallAudioPath();
    }
    void cleanUpAudio() {
        if (spAdapters->spAudioManager->isAudioPlaying()) {
            if (!spAdapters->spAudioManager->requestBStopAudio())
            {
                LOG_E(DLT_STRING("requestBStopAudio fail"));
            }
        }
        if(cv::Common::getConfig("Rcall_Mute_By_MuteLine") == "TRUE"){
            LOG_E(DLT_STRING("TEL MUTE Nomal"));
            if (!spAdapters->spAudioManager->requestTelMute(false)) {
                LOG_E(DLT_STRING("Cannot request TEL MUTE"));
            }
        }
        LOG_E(DLT_STRING("Request TEL MUTE by SomeIP"));
		currentMuteVal = false;

		isNeededToSendTelmuteAgain = !isEtherReady;
		if (!spAdapters->spAudioManager->requestMuteBySomeIP(false)) {
            LOG_E(DLT_STRING("Cannot request TEL MUTE by SomeIP"));
			isNeededToSendTelmuteAgain = true;
        }

    }
    void cleanUpOtherModule() {
        requestScreen("CANCEL TEL MUTE");
        //            cv::Common::sendToICB(EnumSomeipTxMsgID::kTxCallStatusChangeRequest, );
        //            cv::Common::sendToICB(EnumSomeipConTxMsgID, self::IcbCallStatus_endRcall);
        retryCallCounter = 0;
        retryDataCounter = 0;
        isRequestedAudioRetry = false;
        stopTimer(self::JapanTimerId::DIALING_TIME);
        stopTimer(self::JapanTimerId::SEND_DATA_TIME);
    }

    void turnOffFeature(const sp<sl::Message>& msg) {
        LOG_I(DLT_STRING("TURNING OFF FEATURE"));
        LOG_M(DLT_STRING("TURNING OFF FEATURE"));
        CleanupAction action = (CleanupAction)msg->arg1;
        bool isRetry = false;
        if (
                spAdapters->spAudioManager->isAudioPlaying() ||
                (spAdapters->spTelephoneManager->getCallState() != telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE)
                ) {
            isRetry = true;
        }

        if (!isRetry) {
            if (spAdapters->spAppManagerAdapter!= nullptr) spAdapters->spAppManagerAdapter->requestDeactiveFeature(self::feature_rcall_jp);
            if (spAdapters->spPowerManagerAdapter->isIGON()) {
                updateState(State::STANDBY);
            }
            else {
                updateState(State::OFF);
            }
            LOG_I(DLT_STRING("TURN OFF FEATURE SUCCESSFULLY"));
        }
        else {
            LOG_W(DLT_STRING("Failed to Finish Feature, try again after 500ms"));
            spHandler->sendMessageDelayed(spHandler->obtainMessage( msg::RCALL_TURNOFF, (uint32_t)action), 500);
        }
    }

    void subscriptionCheck() {
        if (powerMode == POWER_MODE_VALUE::POWER_MODE_LOW_POWER) {
            spAdapters->spHmiManager->requestPattern(HmiManagerAdapter::Pattern::LED_IDLE, false);
            spAdapters->spHmiManager->requestPattern(HmiManagerAdapter::Pattern::LED_OFF_OTHER, true);
        }
        else if (state == State::STANDBY){
            auto isSub = spAdapters->spConfigManager->isSubscripted(self::subscription_rcall_jp_id)&&
                         (spAdapters->spConfigManager->isSubscripted(self::subscription_acn_jp_id)||
                         spAdapters->spConfigManager->isSubscripted(self::subscription_ecall_id)||
                         spAdapters->spConfigManager->isSubscripted(self::subscription_dess_jp_id));
            if (isSub) {
                spAdapters->spHmiManager->requestPattern(HmiManagerAdapter::Pattern::LED_OFF_NO_SUBS, false);
                spAdapters->spHmiManager->requestPattern(HmiManagerAdapter::Pattern::LED_IDLE, true);
            }
            else {
                spAdapters->spHmiManager->requestPattern(HmiManagerAdapter::Pattern::LED_IDLE, false);
                spAdapters->spHmiManager->requestPattern(HmiManagerAdapter::Pattern::LED_OFF_NO_SUBS, true);
            }
        }
    }


    /**
         * @brief This is the method to start the timer
         *
         * @param timerId
         * @param firstTimeout
         * @param cyclicTimeout
         * @return true
         * @return false
         */
    bool startTimer(uint32_t timerId, uint32_t firstTimeout, uint32_t cyclicTimeout = 0) {
        timers[timerId]->setDuration(firstTimeout, cyclicTimeout);
        timers[timerId]->start();
        return true;
    }

    /**
         * @brief This is the method to stop the timer
         *
         * @param timerId
         * @return true
         * @return false
         */
    bool stopTimer(uint32_t timerId) {
        // LOGV("[USProcessor][%s] End %s", __func__, usTimerInfo[timerId].second.c_str());
        timers[timerId]->stop();
        return true;
    }

    /**
         * @brief This is the method to start specific feature
         *
         * @param type
         * @param featureName
         */
    void startFeature(std::string featureName) {
        LOG_I(DLT_STRING("Start RCall processing"));
        LOG_M(DLT_STRING("Start RCall processing"));
        if (!spAdapters->spAppManagerAdapter->requestActiveFeature(featureName)) {
            cleanUp(CleanupAction::RESET_ALL_MODULE_IMMIDIATELY, "Cannot Active Feature");
            return;
        }

        bool isTrigger = true;
        auto isSub = spAdapters->spConfigManager->isSubscripted(self::subscription_rcall_jp_id);

        if (!spAdapters->spConfigManager->getPrivacy(self::privacy_rcall_jp_id)) {
            LOG_W(DLT_STRING("Rcall privacy is OFF"));
            isTrigger = false;
        }

        if (
                !isSub
                ) {
            LOG_W(DLT_STRING("RCall contract is OFF"));
            isTrigger = false;
        }


        else{
            LOG_I(DLT_STRING("TSU in Normal Mode, excute eCall process"));
        }

        if(cv::Common::getConfig("Rcall_Mute_By_MuteLine") == "TRUE"){
            LOG_E(DLT_STRING("TEL MUTE Nomal"));
            if (!spAdapters->spAudioManager->requestTelMute(true)) {
                LOG_E(DLT_STRING("Cannot request TEL MUTE"));
            }
        }
        LOG_E(DLT_STRING("Request TEL MUTE by SomeIP"));
		currentMuteVal = true;
		isNeededToSendTelmuteAgain = !isEtherReady;
		if (!spAdapters->spAudioManager->requestMuteBySomeIP(true)) {
            LOG_E(DLT_STRING("Cannot request TEL MUTE by SomeIP"));
			isNeededToSendTelmuteAgain = true;
        }

        requestScreen("MUTE");

        if (isTrigger) {
            updateState(State::PREPARING_DATA);
            spAdapters->spHmiManager->requestPattern(HmiManagerAdapter::Pattern::JP_RCALL_RUNNING, true);
            requestPlayAudio(AudioManagerAdapter::AudioFile::RCall_init);
            payload = builder.setGPS()
                    .setUtil(spAdapters->spPowerManagerAdapter->getIgnition())
                    .build(); // TODO: check again
            LOG_I(DLT_STRING("Payload: "), DLT_STRING(payload.c_str()));
        }
        else {
            requestScreen("UNContract");
            indicateError(AudioManagerAdapter::AudioFile::RCall_nocontract, "no contract");
        }
    }

    void startCallProcess(const std::string& telNo) {

        LOG_I(DLT_STRING("Starting call progress with telNo: "), DLT_STRING(telNo.c_str()));
        updateState(State::VOICE_COMMUNICATION);
        startTimer(self::JapanTimerId::DIALING_TIME, self::RCallJpTimeoutTime);
        spAdapters->spTelephoneManager->dial(telNo, false);
    }

    /******************************************************************************/
    /**                               HANDLE METHOD                              **/
    /******************************************************************************/
    /**
         * @brief This is the method to handle cancel trigger from user, we should
         *        end the call if on callling and play appropriate led pattern.
         *
         * @param msg
         */
    void doHandleHmiTrigger(const android::sp<sl::Message>& msg) {
        LOG_I(DLT_STRING("Driver pressed RCall button"));
        LOG_M(DLT_STRING("Driver pressed RCall button"));
        EOperationMode op_mode;
        getOperationMode(op_mode);
        std::string operationMode = "";
        operationMode = op_mode.getStringEnum();
        LOG_I(DLT_STRING("Operation mode is"), DLT_STRING(operationMode.c_str()));
        if (operationMode.compare("EOperationMode::OEMFactoryMode_ProtectionMode") == 0) {
            LOG_I(DLT_STRING("Protected Mode"));
            return;
        }
        if (!spAdapters->spPowerManagerAdapter->isIGON()) {
            LOG_I(DLT_STRING("IG OFF"));
            return;
        }
        switch (state) {
        case State::STANDBY:
            spAdapters->spAppManagerAdapter->queryActionForFeature(self::feature_rcall_jp);
            break;
        case State::TERMINATING:
        case State::PREPARING_DATA:
        case State::COMMUNICATING_DATA:
        case State::VOICE_COMMUNICATION:
            cleanUp(CleanupAction::RESET_ALL_MODULE_IMMIDIATELY, "User cancel");
            break;
        case State::DIAG:
        case State::OFF:
        default:
            break;
        }
    }

    /**
         * @brief This is the method to handle the timer event
         *
         * @param timerId
         */
    void doHandleTimerEvent(uint32_t timerId) {
        switch (timerId) {
        case self::JapanTimerId::DIALING_TIME :
            //                indicateError()
            break;
        case self::JapanTimerId::SEND_DATA_TIME :
            //            case self::JapanTimerId::PERDIODIC_LED_TIME:
            //                prediodicCheck();
            //                startTimer(self::JapanTimerId::PERDIODIC_LED_TIME, self::RCallJpPeriodicLedTime);
            break;
        }
    }

    /**
         * @brief This is the function to make sure our application aware of services died.
         *
         * @param who
         */
    void doHandleBinderDied(uint32_t who) {
        LOG_I(DLT_STRING("ServiceId: "), DLT_UINT(who));
        if (
                who == self::ServiceId::AUDIO ||
                who == self::ServiceId::TELEPHONY ||
                who == self::ServiceId::RIM
                ) {
            if (state != State::OFF && state != State::STANDBY) {
                cleanUp(CleanupAction::RESET_ALL_MODULE_IMMIDIATELY, "Binder died");
            }
        }
        // TODO: Diag died
    }

    /**
         * @brief handleConfigChanged This is the function would call whenever Provisioning config
         *        had changed. We need to check the start condition and terminate the service if it's
         *        currenty running.
         */
    void handleConfigChanged() {
    }


    /**
         * @brief This is the method which wil be called when AppManager decided specific feature should run normaly
         *
         * @param featureHandler This is the pointer to handle object of specific feature.
         */
    bool onStart(const std::string type, const std::string name) {
        LOG_I(DLT_STRING("Name: "), DLT_STRING(name.c_str()));
        LOG_I(DLT_STRING("Type: "), DLT_STRING(type.c_str()));
        if ( state == State::STANDBY) {
            startFeature(name);
        }
        else {
            LOG_E(DLT_STRING("Processor busy"));
            LOG_M(DLT_STRING("Processor busy"));
        }
        return true;
    }

    /**
         * @brief This is the method which wil be called when AppManager decided specific feature should run after terminated
         *        other feature
         *
         * @param featureHandler This is the pointer to handle object of specific feature.
         */
    bool onUpgrade(const std::string type, const std::string name) {
        onStart(type, name);
        return true;
    }

    bool onTrigger(const std::string type, const std::string name) {
        spHandler->obtainMessage(msg::RCALL_TRIGGER)->sendToTarget();
        return true;
    }

    /**
         * @brief This is the method which will be called when AppManager decided specific feature should not run after it
         *        was requested.
         *
         * @param featureHandler This is the pointer to handle object of specific feature.
         */
    bool onTerminate(const std::string type, const std::string name) {
        LOG_I(DLT_STRING("Name: "), DLT_STRING(name.c_str()));
        LOG_I(DLT_STRING("Type: "), DLT_STRING(type.c_str()));
        return true;
    }

    bool onPostponed(const std::string type, const std::string name) {
        LOG_I(DLT_STRING("Name: "), DLT_STRING(name.c_str()));
        LOG_I(DLT_STRING("Type: "), DLT_STRING(type.c_str()));
        return true;
    }

    /**
         * @brief This is the method which will be called when AppManager decided that specific feature should stop.
         *
         * @param featureHandler This is the pointer to handle object of specific feature.
         */
    bool onStop(const std::string type, const std::string name) {
        LOG_I(DLT_STRING("Name: "), DLT_STRING(name.c_str()));
        LOG_I(DLT_STRING("Type: "), DLT_STRING(type.c_str()));
        if (
                (state != State::STANDBY) &&
                (state != State::OFF) &&
                (state != State::DIAG)
                ) {
            cleanUp(CleanupAction::RESET_ALL_MODULE_IMMIDIATELY, "AppManager request Stop");
        }
        return true;
    }

    void doStop(const std::string type, const std::string name) {
        LOG_I(DLT_STRING("Name: "), DLT_STRING(name.c_str()));
        LOG_I(DLT_STRING("Type: "), DLT_STRING(type.c_str()));

    }

    /**
         * @brief This is the method which will be called when audio stop play
         *
         */
    void onStopAudio() {
        LOG_I(DLT_STRING("onStopAudio"));
        switch (state) {
        case State::VOICE_COMMUNICATION: {
            break;
        }
        case State::PREPARING_DATA: {
            LOG_I(DLT_STRING("Finished voice indicate, start send data to GTC server"));
            updateState(State::COMMUNICATING_DATA);
            //                    spAdapters->spHttp->requestSend(payload);
            spAdapters->spDataManager->send(payload, ESendType::ROADSIDE_CALL);
            break;
        }
        case State::TERMINATING: {
            cleanUp(CleanupAction::RESET_ALL_MODULE_IMMIDIATELY, "Finish audio indicate");
            break;
        }
        case State::OFF:
        case State::DIAG:
        case State::COMMUNICATING_DATA:
        default:
            break;
        }
    }

    /**
         * @brief This is the method which will be called when audio start play
         *
         */
    void onPlayAudio() {
        LOG_I(DLT_STRING("onPlayAudio"));
    }

    void onSeverResponse(const android::sp<sl::Message>& msg) {
        switch (state) {
        case State::COMMUNICATING_DATA: {
            if(cv::Common::getConfig("isEnableTestRcall") == "Y"){
                if(cv::Common::getConfig("Rcall_Judge") == "UD"){
                    requestScreen("Data communication error");
                    indicateError(AudioManagerAdapter::AudioFile::RCall_error, "Judge = UD");
                    return;
                }
                if(cv::Common::getConfig("Rcall_Judge") == "NG"){
                    requestScreen("Data communication error");
                    indicateError(AudioManagerAdapter::AudioFile::RCall_error, "Judge = NG");
                    return;
                }
            }
            LOG_I(DLT_STRING("Received response from HELPNET, checking response data ... "));
            bool isErr = false;
            nlohmann::json body;
            std::string telNo;

            android::sp<RIMResponseType> response = nullptr;
            msg->getObject(response);

            if (response == nullptr) {
                isErr = true;
                LOG_E(DLT_STRING("Message error"));
            }
            else {
                try {
                    body = nlohmann::json::parse(response->responseBody.c_str());
                    telNo = body["phoneNumber"];
                } catch (...) {
                    isErr = true;
                }
            }

            if (!isErr) {
                auto result = response->serverResponse;
                if (result ==  serviceinfo::DataResResult::SUCCESS) {
                    LOG_I(DLT_STRING("Send data to server success"));
                    if (cv::Common::getConfig("isEnableTestRcall") == "Y") {
                        startCallProcess(cv::Common::getConfig("PrimaryNumber"));
                    }
                    else {
                        startCallProcess(telNo);
                    }
                    LOG_M(DLT_STRING("Success"));
                }
                else if (result == serviceinfo::DataResResult::FAILURE){
                    LOG_W(DLT_STRING("Send data to server failure"));
                    isErr = true;
                    LOG_M(DLT_STRING("Failure"));
                }
                else if (result == serviceinfo::DataResResult::FAILURE_RETRY){
                    LOG_W(DLT_STRING("Send data to server timeout"));
                    LOG_M(DLT_STRING("Time out"));
                    isErr = true;
                }
                else {
                    isErr = true;
                }
            }

            if (isErr) {
                requestScreen("Data communication error");
                LOG_M(DLT_STRING("Error"));
                indicateError(AudioManagerAdapter::AudioFile::RCall_error, "Data error");
            }
            break;
        }
        case State::VOICE_COMMUNICATION:
        case State::PREPARING_DATA:
        case State::OFF:
        case State::DIAG:
        case State::TERMINATING:
        default:
            LOG_W(DLT_STRING("Received response from server but R-Call feature is in the "), DLT_STRING(statePrinter[state].c_str()), DLT_STRING("State"));
            break;
        }
    }

    void onSendDataError() {
        LOG_M(DLT_STRING("Error"));
        switch (state) {
        case State::COMMUNICATING_DATA: {
            requestScreen("Data communication error");
            indicateError(AudioManagerAdapter::AudioFile::RCall_error, "Judge = UD");
            break;
        }
        case State::VOICE_COMMUNICATION:
        case State::PREPARING_DATA:
        case State::OFF:
        case State::DIAG:
        case State::TERMINATING:
        default:
            LOG_W(DLT_STRING("Received send error signla but R-Call feature in in state: "), DLT_STRING(statePrinter[state].c_str()), DLT_STRING("State"));
            break;
        }
    }

    void onPowerStateChanged(const android::sp<sl::Message>& msg) {
        if (
                (spAdapters->spPowerManagerAdapter->getACCStatus() != MCU_VALUE_STATUS::MCU_STATUS_ON
                 || spAdapters->spPowerManagerAdapter->getIGStatus() != MCU_VALUE_STATUS::MCU_STATUS_ON) &&
                state != State::OFF
                ) {
            if (state == State::STANDBY) {
                updateState(State::OFF);
            }
            else {
                cleanUp(CleanupAction::RESET_ALL_MODULE_IMMIDIATELY, "IG-OFF");
            }
        }
        else if (
                 (spAdapters->spPowerManagerAdapter->isIGON()) &&
                 state == State::OFF
                 ) {
            updateState(State::STANDBY);
        }
    }

    void onCallStateChanged(const android::sp<sl::Message> & msg) {
        if (state != State::OFF && state != State::DIAG && state != State::STANDBY && state != State::NOT_INIT) {
            android::sp<TelephoneManager::StateChangedInfo> info = nullptr;
            msg->getObject(info);
            if (info != nullptr) {
                switch (info->state) {
                case telephony::TelephonyManager::PRECISE_CALL_STATE_IDLE:
                    if (state == State::VOICE_COMMUNICATION) {
                        if (info->isNormal) {
                            cleanUp(CleanupAction::RESET_ALL_MODULE_IMMIDIATELY, "Call disconnected by Normal reason");
                        }
                        else {
                            indicateError(AudioManagerAdapter::AudioFile::RCall_error, "Call disconnected abnormal");
                        }
                    }
                    break;
                case telephony::TelephonyManager::PRECISE_CALL_STATE_ACTIVE:
                    stopTimer(self::JapanTimerId::DIALING_TIME);
                    spAdapters->spAudioManager->requestOpenCallAudioPath();
                    // TODO: NAVI screen request
                    break;
                case telephony::TelephonyManager::PRECISE_CALL_STATE_DIALING:
                    break;
                case telephony::TelephonyManager::PRECISE_CALL_STATE_HOLDING:
                    break;
                case telephony::TelephonyManager::PRECISE_CALL_STATE_WAITING:
                    break;
                case telephony::TelephonyManager::PRECISE_CALL_STATE_ALERTING:
                    break;
                case telephony::TelephonyManager::PRECISE_CALL_STATE_INCOMING:
                    break;
                case telephony::TelephonyManager::PRECISE_CALL_STATE_DISCONNECTED:
                    break;
                case telephony::TelephonyManager::PRECISE_CALL_STATE_DISCONNECTING:
                    break;
                default:
                    return;
                    break;
                }
            }
        }

    }

    /******************************************************************************/
    /**                               END HANDLE METHOD                          **/
    /******************************************************************************/

    /******************************************************************************/
    /**                               HANDLE ERROR **/
    /******************************************************************************/

    /**
         * @brief This is the helper function to handle the timeout error from call
         *
         */
    void handleCallTimeOut() {
        stopTimer(self::JapanTimerId::DIALING_TIME);
        indicateError(AudioManagerAdapter::AudioFile::RCall_error, "Call timeout");
        requestScreen("Voice communication error");
    }

    /******************************************************************************/
    /**                               END HANDLE ERROR                           **/
    /******************************************************************************/

};

// USProcessor::USProcessor(USProcessor&&) noexcept = default;
// USProcessor& USProcessor::operator=(USProcessor&&) noexcept = default;

std::unique_ptr<RCallProcessor::Impl> RCallProcessor::impl_ = std::make_unique<RCallProcessor::Impl>();
RCallProcessor::RCallProcessor()
{
    impl_->state = State::NOT_INIT;
    impl_->country = hal_system_get_region_number();
}

RCallProcessor::~RCallProcessor() {
    impl_->cleanUp(CleanupAction::RESET_ALL_MODULE_IMMIDIATELY, "Terminate cleaning");
    //        impl_->stopTimer(self::JapanTimerId::PERDIODIC_LED_TIME);
    if (impl_->timerHandler != nullptr) {
        delete impl_->timerHandler;
    }
}

void RCallProcessor::init(
        android::sp<OemCallHandler> spHandler,
        std::shared_ptr< AdapterLocator > adapters
        )
    {

    try
    {
        impl_->timerHandler = new OEMCallTimer (shared_from_this());
    }
    catch(...)
    {
        LOG_E(DLT_STRING("exception when init"));
    }

    impl_->timers.reserve(0);
    for (auto i =0; i < self::JapanTimerId::MAX_ID; i++) {
        if (impl_->timerHandler != nullptr) {
            android::sp<Timer> t = new Timer(impl_->timerHandler, i);
            impl_->timers.push_back(std::move(t));
        }
    }
    impl_->spHandler = spHandler;
    impl_->spAdapters = adapters;

    std::shared_ptr<AppManagerAdapter::FeatureHandler> rcallHandler = std::make_shared<AppManagerAdapter::FeatureHandler> ();
    rcallHandler->featureName = self::feature_rcall_jp;
    //LCOV_EXCL_START
    rcallHandler->onStart      = [&](const std::string type, const std::string featureName){ return impl_->onStart(type, featureName);     };
    rcallHandler->onStop       = [&](const std::string type, const std::string featureName){ return impl_->onStop(type, featureName);      };
    rcallHandler->onPostponed  = [&](const std::string type, const std::string featureName){ return impl_->onPostponed(type, featureName); };
    rcallHandler->onTrigger    = [&](const std::string type, const std::string featureName){ return impl_->onTrigger(type, featureName);   };
    rcallHandler->onUpgrade    = [&](const std::string type, const std::string featureName){ return impl_->onUpgrade(type, featureName);   };
    rcallHandler->onIgnore     = [&](const std::string type, const std::string featureName){ return true; };
    //LCOV_EXCL_STOP
    impl_->spAdapters->spAppManagerAdapter->addFeature(rcallHandler);

    if (isAvailable()) {
        if (adapters->spPowerManagerAdapter->isIGON()) {
            impl_->updateState(State::STANDBY);
        }
        else {
            impl_->updateState(State::OFF);
        }
    }
}

/**
 * Check at the CSV file, that the feature r-call for japan
 * is available or not, in this current region number
 */
bool RCallProcessor::isAvailable() {
    return ecallapp::VehicleVariationAdapter::getInstance()
                                  ->isFeatureAvailableInRegion(ecallapp::feature_name::rcall_japan);
}

void RCallProcessor::doProcessorHandler(uint32_t what,const sp<sl::Message>& msg) {
    switch(what) {
    case msg::RCALL_TRIGGER: {
        impl_->doHandleHmiTrigger(msg);
        break;
    }
    case msg::RCALL_TIMER_EXPIRED: {
        impl_->doHandleTimerEvent(msg->arg1);
        break;
    }
    case msg::MSG_CM_BINDER_DIED: {
        impl_->doHandleBinderDied(msg->arg1);
        break;
    }
    case msg::MSG_CM_TEAR_DOWN: {
        impl_->cleanUp(CleanupAction::RESET_ALL_MODULE_IMMIDIATELY, "Tear Down");
        break;
    }
    case msg::RCALL_TURNOFF: {
        impl_->turnOffFeature(msg);
        break;
    }
    case msg::MSC_CM_US_REQUEST_RETRY_PLAY_AUDIO: {
        if (impl_->isRequestedAudioRetry) {
            auto name = (AudioManagerAdapter::AudioFile)msg->arg1;
            impl_->requestPlayAudio(name);
        }
        else {
            // discard message
        }
        break;
    }
    case msg::MSG_AUDIO_PLAY_STATE_CHANGED: {
        auto state = msg->arg1;
        if (state == serviceinfo::AudioState::IDLE) {
            impl_->onStopAudio();
        }
        else if (state == serviceinfo::AudioState::PLAYING) {
            impl_->onPlayAudio();
        }
        break;
    }
    case msg::MSG_CONFIG_CHANGED: {
        impl_->handleConfigChanged();
        break;
    }
    case msg::MSG_POWER_STATE_CHANGED:{
        impl_->onPowerStateChanged(msg);
        break;
    }
    case msg::RCALL_END_ERROR_LED:{
        impl_->spAdapters->spHmiManager->requestPattern(HmiManagerAdapter::Pattern::JP_RCALL_ERROR, false);
        break;
    }
    case msg::MSG_RIM_GTC_RESPONSE:{
        impl_->onSeverResponse(msg);
        break;
    }
    case msg::MSG_POWER_MODE_CHANGED:{
        impl_->powerMode = msg->arg1;
        impl_->subscriptionCheck();
        break;
    }
    case msg::MSG_TELEM_CALL_PRECISE_STATE_CHANGED:{
        impl_->onCallStateChanged(msg);
        break;
    }
	case msg::MSG_SOMEIP_PROVIDER_ETH_READY : {
		impl_->isEtherReady = msg->arg1 == 1;

		if (impl_->isEtherReady && impl_->isNeededToSendTelmuteAgain) {
            if (!impl_->spAdapters->spAudioManager->requestMuteBySomeIP(impl_->currentMuteVal )) {
                LOG_E(DLT_STRING("Cannot request TEL MUTE by SomeIP"));
            }
			impl_->isNeededToSendTelmuteAgain = false;
		}
        break;
	}

    case msg::MSG_SOMEIP_REQUEST_CLEARUP_R_CALL:{
        LOG_I(DLT_STRING("msg::MSG_SOMEIP_REQUEST_CLEARUP_R_CALL"));
        sp<Buffer> buf = nullptr;
        msg->getObject(buf);
        LOG_I(DLT_STRING("pass getObject"));
        if(buf != nullptr) {
            if(buf->data() == nullptr || buf->size() == 0) {
                LOG_I(DLT_STRING("buf is null or buf size is zero!"));
                break;
            }
            if(buf->data()[0] == 0x00) {
                LOG_I(DLT_STRING("buf->data()[0] == 0x00"));
                impl_->cleanUpBySomeIP(CleanupAction::RESET_ALL_MODULE_IMMIDIATELY, "Tear Down");
                LOG_I(DLT_STRING("cleanUpBySomeIP susscess"));
                uint8_t data = 0x00;
                (void)oc::someIP_manager::sendToICB(EnumSomeipTxMsgID::kTxCallStatusChangeResponse, data);
                LOG_I(DLT_STRING("EnumSomeipTxMsgID::kTxCallStatusChangeResponse"));
            } else if(buf->data()[0] == 0x01) {
                LOG_I(DLT_STRING("buf->data()[0] == 0x01"));
                impl_->doHandleHmiTrigger(msg);
                uint8_t data = 0x00;
                (void)oc::someIP_manager::sendToICB(EnumSomeipTxMsgID::kTxCallStatusChangeResponse, data);
            }
        } else {
            LOG_I(DLT_STRING("buf is nullptr"));
        }
        break;
    }

    default : {
    }
    }
}

void RCallProcessor::onTimerEvent(uint32_t timerId) {
    impl_->spHandler->obtainMessage(
                msg::RCALL_TIMER_EXPIRED,
                timerId
                )->sendToTarget();
}

} // namespace oc
