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
 * @file        acn_phone_processor.cpp
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/

#include <chrono>
#include <thread>

#include "acn_phone_processor.h"
#include "../oem_call_app_constants.h"


#include "../oem_call_timer.h" // Timer
#include "../oem_call_handler.h"

#include "../logger.h"
#include "data/data_builder_us.h"
#include "../message_defs.h"
#include "../utils/utils_common.h"
#include <hal_system_tsu.h>
namespace {
    enum class State {
        NOT_INIT,
        IDLE,
        RUNNING
    };
    std::unordered_map< State, std::string > statePrinter{
        {State::NOT_INIT       , "NOT_INIT"               },
        {State::IDLE           , "IDLE"                   },
        {State::RUNNING        , "RUNNING"                }
    };

    struct WatingInfo {
        bool isWaiting = false;
        std::string condition;
        std::function<void()> delegate;
    };
}

namespace oc {

    /**
     * @brief This contains the private properties of ACNPhoneProcessor class instance
     *
     */
    struct ACNPhoneProcessor::Impl {
        DataBuilderUS builder;
        State state;
        android::sp<OemCallHandler> spHandler;
        std::shared_ptr<AdapterLocator> spAdapters;

        /**
         * @brief This is the atribute which will store the information related waiting state,
         *        This atribute should update whenever AppManager decided that some feature in this
         *        process should wait. This atribute should manage and update by AppMgr signal only.
         */
        WatingInfo waitingInfo;
        std::string currentRunningFeature = "";
        int retryDataCounter = 0;
        std::string currentRunningType = "";
        uint32_t country = 0;
        bool needResume = false;

        /**
         * @brief This is the method to update the state of processor. Prefer use
         *        this function instead of dirrecty modify the "state" for better
         *        debugging.
         *
         * @param newState
         */
        void updateState(State newState) {
            LOG_I(DLT_STRING("Processor in state "), DLT_STRING(statePrinter[state].c_str()),
            DLT_STRING(" and changed to "), DLT_STRING(statePrinter[newState].c_str()));
            state = newState;
        }

        /**
         * @brief This is the method to force stop the process. After this method,
         *        all state of components become normal state.
         *
         */
        void reset() {
            needResume = false;
            retryDataCounter = 0;
            spAdapters->spPowerManagerAdapter->requestBUBModeACNPhone(false);

            if(
                    !spAdapters->spAppManagerAdapter->getFeatureState(self::feature_ecall_us) &&
                    !spAdapters->spAppManagerAdapter->getFeatureState(self::feature_ecall_us_cbwt) &&
                    !spAdapters->spAppManagerAdapter->getFeatureState(self::feature_ecall_us_incomming) &&
                    !spAdapters->spAppManagerAdapter->getFeatureState(self::feature_acn_us) &&
                    !spAdapters->spAppManagerAdapter->getFeatureState(self::feature_acn_us_cbwt) &&
                    !spAdapters->spAppManagerAdapter->getFeatureState(self::feature_acn_us_incomming)
                    ) {
                spAdapters->spPowerManagerAdapter->releasePowerLock();
            }
            spAdapters->spAppManagerAdapter->requestDeactiveFeature(self::feature_acn_phone);
            spAdapters->spDataManager->stop();
            updateState(State::IDLE);
        }


        /**
         * @brief This is the method to start perfoming the ACN with phone
         *
         */
        bool startAcnPhone(std::string requestType) {
            LOG_I(DLT_STRING(requestType.c_str()));
            LOG_M(DLT_STRING(requestType.c_str()));
            needResume = false;
            if (!spAdapters->spPowerManagerAdapter->requestBUBModeACNPhone(true)) return false;
            spAdapters->spPowerManagerAdapter->acquirePowerLock();
            updateState(State::RUNNING);
            if (!spAdapters->spConfigManager->getPrivacy(self::privacy_acn_wphone_id)){
                LOG_W(DLT_STRING("ACN with phone privacy OFF"));
                return false;
            }

            if (!checkSubcription(requestType)) {
                LOG_W(DLT_STRING("ACN with phone subcrption OFF"));
                return false;
            }

            if (spAdapters->spPowerManagerAdapter->getACCStatus() != MCU_VALUE_STATUS::MCU_STATUS_ON) {
                LOG_W(DLT_STRING("ACC OFF"));
                return false;
            }

            if (spAdapters->spPowerManagerAdapter->getIGStatus() != MCU_VALUE_STATUS::MCU_STATUS_ON) {
                LOG_W(DLT_STRING("IG OFF"));
                return false;
            }
//            spAdapters->spDataManager->setCollisionType((requestType == self::acn_phone_type_1));
            currentRunningType = requestType;
            sendData();
            return true;
        }

        /**
         * @brief This is the method which wil be called when AppManager decided specific feature should run normaly
         *
         * @param featureHandler This is the pointer to handle object of specific feature.
         */
        bool onStart(const std::string type, const std::string name) {
            LOG_I(DLT_STRING(name.c_str()));
            bool error = false;
            if (
                state == State::IDLE &&
                name == self::feature_acn_phone
                ) {
                if (!spAdapters->spAppManagerAdapter->requestActiveFeature(name)) { error = true; }
                if (!startAcnPhone(type)) {
                    error = true;
                    reset();
                }
            }
            else {
                error = true;
                LOG_E(DLT_STRING("Processor busy"));
                LOG_M(DLT_STRING("Processor busy"));
            }

            if (error) return false;
            return true;
        }

        bool onTrigger(const std::string type, const std::string name) {
            LOG_I(DLT_STRING(name.c_str()));
            if (type == self::acn_phone_type_1) {
                spHandler->obtainMessage(msg::MSG_CDS_TRIGGER_AFTER_WAITING)->sendToTarget();
            }
            else if (type == self::acn_phone_type_2) {
                spHandler->obtainMessage(msg::MSG_COMM_WPHONE_CDS_LIGHT_TRIGGER)->sendToTarget();
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
            LOG_I(DLT_STRING("Name: "), DLT_STRING(name.c_str()));
            LOG_I(DLT_STRING("Type: "), DLT_STRING(type.c_str()));
            if (
                state == State::IDLE &&
                name == self::feature_acn_phone
                ) {
                if (!spAdapters->spAppManagerAdapter->requestActiveFeature(name)) { return false; }
                if (!startAcnPhone(type)) {
                    reset();
                    return false;
                }
            }
            else {
                LOG_E(DLT_STRING("Processor busy"));
            }
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

        /**
         * @brief onPostponed
         * @param type
         * @param name
         * @return
         */
        bool onPostponed(const std::string type, const std::string name) {
            LOG_I(DLT_STRING("Name: "), DLT_STRING(name.c_str()));
            LOG_I(DLT_STRING("Type: "), DLT_STRING(type.c_str()));
//            if (name != self::feature_acn_phone) {
//                return true;
//            }
//            if(type == self::acn_phone_postponse) {
//                if(state == State::RUNNING){
//                    reset();
//                    needResume = true;
//                }
//            }
//            if(type == self::acn_phone_resumse) {
//                if(needResume == true) {
//                    onTrigger(currentRunningType,self::feature_acn_phone);
//                }
//            }
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
            if (!(state == State::IDLE)) {
                reset();
                return false;
            }
            return true;
        }

        bool onIgnore(const std::string type, const std::string name) {
            LOG_I(DLT_STRING("Feature trigger was ignored"));
            return true;
        }

        void onSeverResponse(const android::sp<RIMResponseType>& response) {
            if (state != State::IDLE && state != State::NOT_INIT) {
                auto result = response->serverResponse;

                if (result ==  serviceinfo::DataResResult::SUCCESS) {
                    LOG_M(DLT_STRING("onSeverResponse success"));
                    LOG_I(DLT_STRING("Send data to server success"));
                    reset();
                }
                else if (result == serviceinfo::DataResResult::FAILURE){
                    LOG_M(DLT_STRING("onSeverResponse failure"));
                    LOG_W(DLT_STRING("Send data to server failure"));
                    handleDataSendingError();
                }
                else if (result == serviceinfo::DataResResult::FAILURE_RETRY){
                    LOG_M(DLT_STRING("onSeverResponse failure retry"));
                    LOG_W(DLT_STRING("Send data to server timeout"));
                    handleDataSendingError();
                }

            }
            else {
                LOG_W(DLT_STRING("Received response when feature is in state: "), DLT_STRING(statePrinter[state].c_str()));
            }
        }

        /**
         * @brief This is the method to check the Subcription info, we need to consider the matching condition
         *        between actual subcription info and type of condition which happened.
         *
         */
        bool checkSubcription(std::string requestType) {
            LOG_I(DLT_STRING("Checking Subcription info"));
            if ( spAdapters->spConfigManager->isSubscripted(self::subscription_acn_phone_heavy_id)&& requestType == self::acn_phone_type_1) {
                return true;
            }
            else if ( spAdapters->spConfigManager->isSubscripted(self::subscription_acn_phone_light_id)&& requestType == self::acn_phone_type_2) {
                return true;
            }
            LOG_I(DLT_STRING("Subcription info does not match, terminating ACN with phone process"));
            return false;
        }

        void sendData() {
            auto payload = builder
                              .setCDS(currentRunningType == self::acn_phone_type_1)
                              .setGPS()
                              .setUtil(spAdapters->spPowerManagerAdapter->getIgnition())
                              .build();
            LOG_I(DLT_STRING("Payload = "), DLT_STRING(payload.c_str()), DLT_STRING(", Length = "), DLT_UINT(payload.length()));
            spAdapters->spDataManager->send(payload, ESendType::ACN);
        }


        /******************************************************************************/
        /**                               HANDLE METHOD                              **/
        /******************************************************************************/
        /**
         * @brief This is the function to handle cds trigger
         *
         */
        void doHandleCdsTrigger(const android::sp<sl::Message>& msg) {
            LOG_I(DLT_STRING("Start handle cds trigger"));
            if ( !spAdapters->spAppManagerAdapter->getFeatureState(self::feature_acn_phone)) {
                (void)spAdapters->spAppManagerAdapter->queryActionForFeature(self::feature_acn_phone, self::acn_phone_type_1);
            }
            else {
                 LOG_I(DLT_STRING("ACN Phone is currently Running"));
            }

        }

        /**
         * @brief This is the function to handle cds trigger
         *
         */
        void doHandleLightCds(const android::sp<sl::Message>& msg) {
            LOG_I(DLT_STRING("Start handle light cds trigger"));
            if ( !spAdapters->spAppManagerAdapter->getFeatureState(self::feature_acn_phone)) {
                (void)spAdapters->spAppManagerAdapter->queryActionForFeature(self::feature_acn_phone, self::acn_phone_type_2);
            }
            else {
                 LOG_I(DLT_STRING("ACN Phone is currently Running"));
            }
        }

        /**
         * @brief This is the function to make sure our application aware of services died.
         *
         * @param who
         */
        void doHandleBinderDied(uint32_t who) {
            LOG_I(DLT_UINT32(who));
            if (
                who == self::ServiceId::RIM
            ) {
                    if (state != State::IDLE && state != State::NOT_INIT) {
                        reset();
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
             * @brief This is the method to receive all the error signal
             *
             */
        void startHandleError(uint32_t type) {
            LOG_I(DLT_UINT32(type));
            LOG_M(DLT_UINT32(type));
            //TODO: implement logger for save error log
            if (type == self::ErrorType::memory) {
                LOG_E(DLT_STRING("Memory Error, terminating ACN with phone process"));
                reset();
            }
            else if (
                type == self::ErrorType::ethernet ||
                type == self::ErrorType::led ||
                type == self::ErrorType::location
                ) {
                // TODO: logging
            }
            else if (
                type == self::ErrorType::notificationStatus ||
                type == self::ErrorType::dataSendingTimeout
                ) {
                handleDataSendingError();
            }
            else if (type == self::ErrorType::termination) { // TODO: confirm remove this because of changed spec
            }
        }

        void handleDataSendingError() {
            reset();
        }

        /******************************************************************************/
        /**                               END HANDLE ERROR                           **/
        /******************************************************************************/

    };

// ACNPhoneProcessor::~ACNPhoneProcessor() noexcept = default;
// ACNPhoneProcessor::ACNPhoneProcessor(ACNPhoneProcessor&&) noexcept = default;
// ACNPhoneProcessor& ACNPhoneProcessor::operator=(ACNPhoneProcessor&&) noexcept = default;

std::unique_ptr<ACNPhoneProcessor::Impl>  ACNPhoneProcessor::impl_ = std::make_unique<ACNPhoneProcessor::Impl>();
ACNPhoneProcessor::ACNPhoneProcessor()
{
    impl_->state = State::NOT_INIT;
    impl_->country = hal_system_get_region_number();
}
ACNPhoneProcessor::~ACNPhoneProcessor() {
}

void ACNPhoneProcessor::init(
    android::sp<OemCallHandler> spHandler,
    std::shared_ptr< AdapterLocator > adapters
) {
    impl_->spHandler = spHandler;
    impl_->spAdapters = adapters;

    std::shared_ptr<AppManagerAdapter::FeatureHandler> acnNormalHandle = std::make_shared<AppManagerAdapter::FeatureHandler> ();
    acnNormalHandle->featureName = self::feature_acn_phone;
    //LCOV_EXCL_START
    acnNormalHandle->onStart      = [&](const std::string type, const std::string featureName){ return impl_->onStart(type, featureName);     };
    acnNormalHandle->onStop       = [&](const std::string type, const std::string featureName){ return impl_->onStop(type, featureName);      };
    acnNormalHandle->onPostponed  = [&](const std::string type, const std::string featureName){ return impl_->onPostponed(type, featureName); };
    acnNormalHandle->onTrigger    = [&](const std::string type, const std::string featureName){ return impl_->onTrigger(type, featureName);   };
    acnNormalHandle->onUpgrade    = [&](const std::string type, const std::string featureName){ return impl_->onUpgrade(type, featureName);   };
    acnNormalHandle->onIgnore     = [&](const std::string type, const std::string featureName){ return impl_->onIgnore(type, featureName);  };
    //LCOV_EXCL_STOP
    impl_->spAdapters->spAppManagerAdapter->addFeature(acnNormalHandle);

    impl_->updateState(State::IDLE);
}

/**
 * Check at the CSV file, that the feature acn-with-gtc
 * is available or not, in this current region number
 */

bool ACNPhoneProcessor::isAvailable() {
    return ecallapp::VehicleVariationAdapter::getInstance()
                                    ->isFeatureAvailableInRegion(ecallapp::feature_name::acn_phone);
}

void ACNPhoneProcessor::doProcessorHandler(uint32_t what, const sp<sl::Message>& msg) {
    if (impl_->state == State::NOT_INIT) {
    }
    else {
        switch (what) {
        case msg::MSG_CDS_TRIGGER_AFTER_WAITING: {
            // Heavy ACN W Phone
            if(impl_->country != 1
			&& impl_->country != 2
			&& impl_->country != 3
			&& impl_->country != 101
			&& impl_->country != 102
			&& impl_->country != 103
			&& impl_->country != 104
			&& impl_->country != 105
			&& impl_->country != 106
			&& impl_->country != 107
			&& impl_->country != 108
			&& impl_->country != 109
			&& impl_->country != 124
			&& impl_->country != 125
			&& impl_->country != 121
			&& impl_->country != 122
			&& impl_->country != 123
			&& impl_->country != 58
			&& impl_->country != 59
			&& impl_->country != 60
			&& impl_->country != 61
    && impl_->country != 34 && impl_->country != 112) {
                impl_->doHandleCdsTrigger(msg);
            }
            break;
        }
        case msg::MSG_COMM_WPHONE_CDS_LIGHT_TRIGGER: {
            // Light ACN W Phone
            if(impl_->country != 5 && impl_->country != 6 && impl_->country != 53) {
                impl_->doHandleLightCds(msg);
            }
            break;
        }
        case msg::MSG_CM_BINDER_DIED: {
            impl_->doHandleBinderDied(msg->arg1);
            break;
        }
        case msg::MSG_CM_TEAR_DOWN: {
            impl_->reset();
            break;
        }
        case msg::MSG_POWER_LOCK_CALLBACK: {
            if (impl_->state == State::RUNNING) {
                impl_->spAdapters->spPowerManagerAdapter->resetIsLocked();
                impl_->spAdapters->spPowerManagerAdapter->acquirePowerLock();
            }
            break;
        }
        case msg::MSG_RIM_GTC_RESPONSE: {
            android::sp<RIMResponseType> response = nullptr;
            msg->getObject(response);
            if (response != nullptr) {
                impl_->onSeverResponse(response);
            }
        }
        default: {
        }
        }
    }
}

void ACNPhoneProcessor::onTimerEvent(uint32_t timerId) {

}
} // namespace oc
