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
 * @file        AudioManagerAdapter.cpp
 * @author      tien.nguyen@lge.com
 * @version     1.0.00
*/
#include <unordered_map>
#include <sstream>
#include "AudioManagerAdapter.h"
#include <fstream>

namespace ecallapp {

AudioManagerAdapter::AudioManagerAdapter() {

}

AudioManagerAdapter* AudioManagerAdapter::instance {nullptr};
AudioManagerAdapter* AudioManagerAdapter::getInstance() {
    if (instance == nullptr) {
        instance = new AudioManagerAdapter();
    }
    return instance;
}

android::sp<IAudioManagerService> AudioManagerAdapter::getService() {
    if (mAudioMgr != nullptr) {
        mAudioMgr = android::interface_cast<IAudioManagerService>(android::defaultServiceManager()->getService(android::String16(AUDIO_SRV_NAME)));
    }
    return mAudioMgr;
}

void AudioManagerAdapter::registerService() {
    LOG_D(DLT_STRING("registerService"));
    mHandler = ECallAppHandler::getInstance();
    if (mAudioMgr != nullptr) {
        mAudioMgr = nullptr;
    }
    mServiceDeathRecipient = android::sp<ServiceDeathRecipient>{new ServiceDeathRecipient( [&] ( const android::wp<android::IBinder>& who ){
        AudioManagerAdapter::getInstance()->onBinderDied(who);
    })};

    mAudioMgr = android::interface_cast<IAudioManagerService>(android::defaultServiceManager()->getService(android::String16(AUDIO_SRV_NAME)));

    if (mAudioMgr != nullptr) {
        if (android::OK == android::IInterface::asBinder(mAudioMgr)->linkToDeath(mServiceDeathRecipient)) {
            //finally register receiver
            mAudioManagerReceiver = new AudioReceiver(this);
            const uint32_t moduleId {static_cast<uint32_t>(getpid())};
            (void)mAudioMgr->registerReceiver(static_cast<serviceid_t>(moduleId), mAudioManagerReceiver);
        }
    } else {
        LOG_E(DLT_STRING("mAudioMgr is nullptr"));
        if (mHandler != nullptr) {
            (void)(mHandler->sendMessageDelayed(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_AUDIOMGR)), timeMsParams::TIME_SEND_RETRY_DELAY_MS));
        } else {
            LOG_E(DLT_STRING("mHandler = nullptr"));
        }
    }
}

void AudioManagerAdapter::openCallAudioPath(const int32_t type) {
    LOG_I(DLT_STRING("openCallAudioPath"));
    if(mAudioMgr != nullptr) {
        (void)mAudioMgr->openCallAudioPath(type);
    }
    else {
        LOG_E(DLT_STRING("mAudioMgr is nullptr"));
    }
}

void AudioManagerAdapter::closeCallAudioPath() {
    LOG_I(DLT_STRING("closeCallAudioPath"));
    if(mAudioMgr != nullptr) {
        (void)mAudioMgr->closeCallAudioPath();
    }
    else {
        LOG_E(DLT_STRING("mAudioMgr is nullptr"));
    }
}

void AudioManagerAdapter::onBinderDied(const android::wp<android::IBinder>& who) {
    LOG_D(DLT_STRING("onBinderDied"));
    NOTUSED(who);
    if(mHandler != nullptr)
    {
        (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_CONNECT_TO_AUDIOMGR))->sendToTarget());
    }
    else
    {
        LOG_E(DLT_STRING("mHandler is nullptr"));
    }
}
bool AudioManagerAdapter::isPlaying() const {
    return mCurrentPlayState == audio::STATE_PLAYING;
}
void AudioManagerAdapter::onPlayStateChanged(const uint8_t state) {
	LOG_I(DLT_STRING("onPlayStateChanged, "), DLT_UINT8(state));
    (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_AUDIO_PLAY_STATE_CHANGED), static_cast<int32_t>(state))->sendToTarget());
}

void AudioManagerAdapter::handlePlayStateChanged(const uint8_t state) {
	LOG_I(DLT_STRING("handlePlayStateChanged, "), DLT_UINT8(state));
    if (state != mCurrentPlayState) {
        mCurrentPlayState = state;
        if (mCurrentPlayState == audio::STATE_PLAYING) {
            (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_AUDIO_PLAYING_STATE))->sendToTarget());
        }
        if (mCurrentPlayState == audio::STATE_STOPPED) {
            (void)(mHandler->obtainMessage(ENUM_TO_INT(msg::MSG::RECV_MSG_AUDIO_STOPPED_STATE))->sendToTarget());
        }
    }
}

bool AudioManagerAdapter::playAudioFile(const std::string& audioFile) {
	LOG_I(DLT_STRING("playAudioFile, "), DLT_STRING(audioFile.c_str()));
    bool ret{false};
    if (mAudioMgr->AudioFilePlay(audioFile.length(), (uint8_t*)audioFile.c_str()) == TIGER_ERR::E_OK) {
        mCurrentAudioFile = audioFile;
        ret = true;
    }
    return ret;
}

bool AudioManagerAdapter::stopAudioFile() {
    LOG_I(DLT_STRING("stopAudioFile"));
    bool ret{false};
    ret = mAudioMgr->AudioFileStop() == TIGER_ERR::E_OK;
    LOG_I(DLT_STRING("stopAudioFile"), DLT_STRING(", results = "), DLT_BOOL(ret));
    return ret;
}

bool AudioManagerAdapter::setMute(bool isMute) {
    bool result {false};
    if (mAudioMgr != nullptr) {
        if (isMute) {
            result = mAudioMgr->requestAudioArbitration(0) == TIGER_ERR::E_OK;
        }
        else {
            result = mAudioMgr->releaseAudioArbitration() == TIGER_ERR::E_OK;
        }
    }
    LOG_I(DLT_STRING("Request Send TEL MUTE signal "), DLT_STRING(isMute ? "TRUE" : "FALSE"), DLT_STRING(", results = "), DLT_BOOL(result));
    return result;
}

} /* End: namespace ECALLDCMNONREG */
