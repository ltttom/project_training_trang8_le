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
 * @file        AudioManagerAdapter.h
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#ifndef ADAPTER_AUDIOMANAGERADAPTER_H_
#define ADAPTER_AUDIOMANAGERADAPTER_H_

#include <iostream>
#include <utils/RefBase.h>
#include <binder/IServiceManager.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>

#include <services/AudioManagerService/IAudioManagerService.h>
#include <services/AudioManagerService/IAudioManagerReceiver.h>
#include <services/AudioManagerService/IAudioManagerServiceType.h>
#include <services/AudioManagerService/AudioManager.h>
#include <services/AudioManagerService/IAudioManagerService.h>

#include "../utils/ECallAppHandler.h"
#include "logger.h"
#include "../utils/ServiceDeathRecipient.h"
#include "../utils/ParamsDef.h"
#include "../utils/ECallTimerHandler.h"

namespace ecallapp {

// Honda voice file
namespace audio {
static const uint8_t STATE_PLAYING{1U};
static const uint8_t STATE_STOPPED{0U};

static const std::string NONE = "";
static const std::string TONE_1 = "/audio/tone_1.raw";
static const std::string TONE_2 = "/audio/tone_2.raw";
static const std::string TONE_3 = "/audio/tone_3.raw";
static const std::string RCALL_INIT = "/audio/voice_prompt_JP_1.raw";
static const std::string RCALL_ERROR = "/audio/voice_prompt_JP_9.raw";
static const std::string RCALL_NO_CONTRACT = "/audio/voice_prompt_JP_4.raw";

static const std::string ECALL_EVENT_CALL_DIALING = "/audio/tone_1.raw";
static const std::string ECALL_EVENT_MSD_SENDING = "/audio/tone_2.raw";
static const std::string ECALL_EVENT_CALL_CONNECTED =  "/audio/tone_3.raw";
static const std::string ACN_INIT = "/audio/voice_prompt_US_1.raw";
static const std::string ECALL_INIT = "/audio/voice_prompt_US_2.raw";
static const std::string ECALL_UPGRADE = "/audio/voice_prompt_US_3.raw";
static const std::string ECALL_DROPPED = "/audio/voice_prompt_US_4.raw";
static const std::string ECALL_FAILED = "/audio/voice_prompt_US_5.raw";
static const std::string ECALL_NOT_ACTIVE = "/audio/voice_prompt_US_6.raw";
static const std::string NO_DTC = "/audio/voice_prompt_US_7.raw";
static const std::string DTCS = "/audio/voice_prompt_US_8.raw";
static const std::string JP_INIT = "/audio/voice_prompt_JP_1.raw";
static const std::string JP_SENDING = "/audio/voice_prompt_JP_2.raw";
static const std::string JP_VOICE_CONNECT = "/audio/voice_prompt_JP_3.raw";
static const std::string JP_UNREGISTERED = "/audio/voice_prompt_JP_4.raw";
static const std::string JP_DATA_FAILURE = "/audio/voice_prompt_JP_5.raw";
static const std::string JP_VOICE_FAILURE = "/audio/voice_prompt_JP_6.raw";
static const std::string JP_SUCCEEDED = "/audio/voice_prompt_JP_7.raw";
static const std::string JP_TEST_CONNECT = "/audio/voice_prompt_JP_8.raw";
static const std::string JP_TEST_FAILED = "/audio/voice_prompt_JP_9.raw";
}

class AudioManagerAdapter {
public:
    AudioManagerAdapter();
    static AudioManagerAdapter* getInstance();
    void registerService();
    void openCallAudioPath(const int32_t type = 0);
    void closeCallAudioPath();

    bool playAudioFile(const std::string& audioFile);
    bool stopAudioFile();
    bool isPlaying() const; 
    /* Handle play state changed event */
    void onPlayStateChanged(const uint8_t state);
    void handlePlayStateChanged(const uint8_t state);
    bool setMute(bool isMute);
private:
    android::sp<IAudioManagerService> getService();
    void onBinderDied(const android::wp<android::IBinder>& who);

private:
    static AudioManagerAdapter*        instance;
    android::sp<IAudioManagerService>  mAudioMgr{nullptr};
    android::sp<ECallAppHandler>    mHandler{nullptr};
    android::sp<ServiceDeathRecipient> mServiceDeathRecipient{nullptr};
    android::sp<IAudioManagerReceiver> mAudioManagerReceiver{nullptr};
    uint8_t                            mCurrentPlayState{audio::STATE_STOPPED};
    std::string mCurrentAudioFile {audio::NONE};
};

class AudioReceiver: public BnAudioManagerReceiver{
    public:
        AudioReceiver(AudioManagerAdapter* const rv) noexcept 
            : BnAudioManagerReceiver(),
            rvAudio(rv) {}

        virtual void onAudioPlayStateChanged(const uint8_t state) override {
            rvAudio->onPlayStateChanged(state);
        }
    private:
        AudioManagerAdapter* rvAudio;
};
} /* End: namespace ECALLDCMNONREG */
#endif /* ADAPTER_AUDIOMANAGERADAPTER_H_ */
