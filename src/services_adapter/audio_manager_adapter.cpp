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
 * @file        audio_manager_adapter.cpp
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#include <binder/IServiceManager.h>

#include "../logger.h"
#include "audio_manager_adapter.h"
#include "../oem_call_app_constants.h"
#include <map>

// Audio manager service
#include <services/AudioManagerService/IAudioManagerService.h>
#include <services/AudioManagerService/IAudioManagerServiceType.h>
#include <services/AudioManagerService/AudioManager.h>
#include <mutex>
#include <condition_variable>
#include <services/SomeipConsumerManagerService/SomeipConsumerManager.h>
#include "someip_manager.h"

namespace {
    enum class TelMuteState{
        kON,
        kOFF
    };

    enum class SpeakerState{
        kObtained,
        kNotObtained
    };

    enum class AudioMode {
        IDLE,
        PROMPT_PLAYING,
        TONE_PLAYING,
        TONE_PLAYING_REPEAT
    };

}

namespace oc{

class AudioReceiver: public BnAudioManagerReceiver{
    public:
        AudioReceiver(IAudioManagerAdapter* parent) : parent_(parent) {}
        virtual ~AudioReceiver() {}
        virtual void onAudioPlayStateChanged(uint8_t state) {
            parent_->onPlayStateChanged(state);
        }
    private:
        IAudioManagerAdapter* parent_;
};

/**
 * @brief This contains the private properties of ConfigManager class instance
 *
 */
struct AudioManagerAdapter::Impl {
    std::map < AudioFile, std::string > audioFiles{
        {AudioFile::Tone_1,                "/audio/tone_1.raw"},
        {AudioFile::Tone_2,                "/audio/tone_2.raw"},
        {AudioFile::Tone_3,                "/audio/tone_3.raw"},
        {AudioFile::RCall_init,            "/audio/voice_prompt_JP_1.raw"},
        {AudioFile::RCall_error,           "/audio/voice_prompt_JP_9.raw"},
        {AudioFile::RCall_nocontract,      "/audio/voice_prompt_JP_4.raw"}
    };

    android::sp<OemCallHandler> spHandler;
    android::sp<ServiceDeathRecipient>         spServiceDeathRecipient = nullptr;


    std::condition_variable cvAudioStart;
    std::condition_variable cvAudioStop;
    std::mutex mutex;


    /**
     * @brief This is the state of audio
     *
     */
    uint8_t audioState = serviceinfo::AudioState::IDLE;

    /**
     * @brief This is the pointer to Audio manager service
     *
     */
    android::sp< IAudioManagerService > spAudioManagerService;

    /**
     * @brief spAudioReceiver This is the receiver which provide the method to listen to
     *        signal from AudioManager service
     */
    android::sp<IAudioManagerReceiver> spAudioReceiver = nullptr;
    /**
     * @brief This is the method to get the audio manager service pointer from
     *        service layer.
     */
    android::sp<IAudioManagerService> getAudioMgrService() {
        if (spAudioManagerService == nullptr) {
            spAudioManagerService = android::interface_cast<IAudioManagerService>(
                                    android::defaultServiceManager()->getService(
                                        android::String16(AUDIO_SRV_NAME)
                                )
            );
        }
        return spAudioManagerService;
    }

    void registerAudioService() {
        spAudioManagerService= android::interface_cast<IAudioManagerService>(
            android::defaultServiceManager()->getService(
                android::String16(AUDIO_SRV_NAME)
            )
        );
        bool error = true;

        if(spAudioManagerService != nullptr) {
            auto result = android::IInterface::asBinder(spAudioManagerService)->linkToDeath(spServiceDeathRecipient);
            if (
                result == android::OK &&
                spAudioManagerService->registerReceiver(0, spAudioReceiver) == TIGER_ERR::E_OK
            ) {
                error = false;
                LOG_I(
                    DLT_STRING(std::string(__func__).c_str()),
                    DLT_STRING(", "),
                    DLT_STRING("Registed Audio manager Service")
                );
            }
        }

        if (error) {
            LOG_E(
                DLT_STRING(std::string(__func__).c_str()),
                DLT_STRING(", "),
                DLT_STRING("Cannot register Audio manager Service, try again after"),
                DLT_UINT32(self::SEND_RETRY_DELAY)
            );
            spHandler->sendMessageDelayed(
                spHandler->obtainMessage(
                    msg::MSG_CM_REQUEST_RETRY_REGISTER_RECEIVER,
                    self::ServiceId::AUDIO
                ),
                self::SEND_RETRY_DELAY
            );
        }
    }
    
    void onBinderDied(const android::wp<android::IBinder> &who) 
    {
        audioState = serviceinfo::AudioState::IDLE;
        spAudioManagerService = nullptr;
        spHandler->sendMessageDelayed(
            spHandler->obtainMessage(
                msg::MSG_CM_RETRY_REGISTER_AUDIO,
                self::ServiceId::AUDIO
            ),
            self::SEND_RETRY_DELAY
        );
        spHandler->obtainMessage(msg::MSG_CM_BINDER_DIED, self::ServiceId::AUDIO)->sendToTarget();
    }

};

std::unique_ptr<AudioManagerAdapter::Impl>  AudioManagerAdapter::impl_ = std::make_unique<AudioManagerAdapter::Impl>();
AudioManagerAdapter::AudioManagerAdapter(android::sp<OemCallHandler> handler)
{
    LOG_I(DLT_STRING("Creating AudioManager Adapter ..."));
    impl_->spHandler = handler;
    impl_->audioState = serviceinfo::AudioState::IDLE;
    impl_->spServiceDeathRecipient = new ServiceDeathRecipient();
    impl_->spServiceDeathRecipient->registerHandler(
        [&] (const android::wp<android::IBinder>& who) -> void
        {
            impl_->onBinderDied(who);
        });

    impl_->spAudioReceiver         = new AudioReceiver(this);
    impl_->registerAudioService();
}

bool AudioManagerAdapter::requestTelMute(bool isMute) {
    LOG_I(DLT_STRING("Request Send TEL MUTE signal"),DLT_BOOL(isMute));
    if (impl_->getAudioMgrService() == nullptr) {
        return false;
    }
    if (isMute && impl_->getAudioMgrService()->requestAudioArbitration(0) != TIGER_ERR::E_OK) {
        return false;
    }
    else if (!isMute && impl_->getAudioMgrService()->releaseAudioArbitration() != TIGER_ERR::E_OK) {
        return false;
    }
    return true;
}

bool AudioManagerAdapter::requestMuteBySomeIP(bool isMute)
{
    if (isMute && impl_->getAudioMgrService()->requestAudioArbitration(1) != TIGER_ERR::E_OK) {
        LOG_E(DLT_STRING(" unmute fail"));
    }
    else if (!isMute && impl_->getAudioMgrService()->releaseAudioArbitration() != TIGER_ERR::E_OK) {
        LOG_E(DLT_STRING(" mute fail"));
    }
    LOG_I(DLT_STRING("Send TEL MUTE signal via ICB"),DLT_BOOL(isMute));
    if (isMute) {
        LOG_I(DLT_STRING("Handle TestCall mute Audio"));
        uint8_t data = 0x03;
        error_t checkSend = oc::someIP_manager::sendToICB(EnumSomeipTxMsgID::kTxTSUCallStatusInfo, data);
        if(checkSend == TIGER_ERR::E_ERROR)
        {
            LOG_E(DLT_STRING(" Send error, please check!!!"));
        } else {
            LOG_I(DLT_STRING("Send to ICB successfully!"));
			return true;
        }
        return false;
    }
    else {
        LOG_I(DLT_STRING("Handle TestCall mute Audio"));
        uint8_t data = 0x00;
        error_t checkSend = oc::someIP_manager::sendToICB(EnumSomeipTxMsgID::kTxTSUCallStatusInfo, data);
        if(checkSend == TIGER_ERR::E_ERROR)
        {
            LOG_E(DLT_STRING(" Send error, please check!!!"));
        } else {
            LOG_I(DLT_STRING("Send to ICB successfully!"));
			return true;
        }
        return false;
    }
}


bool AudioManagerAdapter::requestOpenCallAudioPath() {
    if (AudioManager::instance()->openCallAudioPath(0) != E_OK){
        LOG_E(DLT_STRING("Cannot Open Call Audio Path"));
        return false;
    }
    return true;
}

bool AudioManagerAdapter::requestCloseCallAudioPath() {
    if (AudioManager::instance()->closeCallAudioPath() != E_OK){
        LOG_E(DLT_STRING("Cannot Close Call Audio Path"));
        return false;
    }
    return true;

}

bool AudioManagerAdapter::requestBStopAudio() {
    std::unique_lock<std::mutex> lk(impl_->mutex);
    if (impl_->audioState == serviceinfo::AudioState::PLAYING) {
        if (AudioManager::instance()->AudioFileStop() != E_OK) {
            return false;
        }
        impl_->cvAudioStop.wait(lk, [&] { return (impl_->audioState == serviceinfo::AudioState::IDLE);} );
    }
    else {
        LOG_W(DLT_STRING("Audio not playing"));
    }
    return true;

}

bool AudioManagerAdapter::requestNBStopAudio() {
    if (impl_->audioState == serviceinfo::AudioState::PLAYING) {
        if (AudioManager::instance()->AudioFileStop() != E_OK) {
            return false;
        }
    }
    return true;
}

bool AudioManagerAdapter::requestBPlayAudio(AudioManagerAdapter::AudioFile fileName) {
    std::unique_lock<std::mutex> lk(impl_->mutex);
    LOG_I(DLT_STRING("Requesting play audio"), DLT_STRING(impl_->audioFiles[fileName].c_str()));
    if (impl_->audioState == serviceinfo::AudioState::PLAYING) {
        LOG_W(DLT_STRING("Audio is playing, cannot request play file : "), DLT_STRING(impl_->audioFiles[fileName].c_str()) );
        return true;
    }

    if (impl_->spAudioManagerService->AudioFilePlay(
                impl_->audioFiles[fileName].length(),
                (uint8_t*)impl_->audioFiles[fileName].c_str()
            ) != TIGER_ERR::E_OK) {
        LOG_E(DLT_STRING("AudioMgr currently cannot play file : "), DLT_STRING(impl_->audioFiles[fileName].c_str()) );
        return false;
    }

    LOG_I(DLT_STRING("waiting till audio play"));
    impl_->cvAudioStart.wait(lk, [&] { return (impl_->audioState == serviceinfo::AudioState::PLAYING);} );
    LOG_I(DLT_STRING("audio playing, continue"));
    return true;
}

bool AudioManagerAdapter::requestNBPlayAudio(AudioManagerAdapter::AudioFile fileName) {
    if (impl_->spAudioManagerService->AudioFilePlay(
                impl_->audioFiles[fileName].length(),
                (uint8_t*)impl_->audioFiles[fileName].c_str()
            ) != TIGER_ERR::E_OK) {
        LOG_E(DLT_STRING("Cannot request play file : "), DLT_STRING(impl_->audioFiles[fileName].c_str()) );
        return false;
    }
    return true;
}

void AudioManagerAdapter::onPlayStateChanged(uint8_t state) 
{
    std::lock_guard<std::mutex> lk(impl_->mutex);
    LOG_I(DLT_STRING("current state: "), DLT_UINT8(impl_->audioState), DLT_STRING("new state: "), DLT_UINT8(state));
    auto previousState = impl_->audioState;
    impl_->audioState = state;
    
    if (
            previousState     == serviceinfo::AudioState::PLAYING&&
            impl_->audioState == serviceinfo::AudioState::IDLE
    ) {
        impl_->cvAudioStop.notify_all();
    }
    else if (
             previousState     == serviceinfo::AudioState::IDLE &&
             impl_->audioState == serviceinfo::AudioState::PLAYING
    ) {
        impl_->cvAudioStart.notify_all();
    }
    impl_->spHandler->obtainMessage(msg::MSG_AUDIO_PLAY_STATE_CHANGED, state)->sendToTarget();
}


bool AudioManagerAdapter::isAudioPlaying() {
    std::lock_guard<std::mutex> lk(impl_->mutex);
    return (impl_->audioState == serviceinfo::AudioState::PLAYING);
}

void AudioManagerAdapter::requestBRetryRegister() {
    impl_->registerAudioService();
}

} // namespace oc
