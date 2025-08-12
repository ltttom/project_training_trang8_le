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
 * @file        audio_manager_adapter.h
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#ifndef __OEM_CALL_AUDIO_MANAGER_H__
#define __OEM_CALL_AUDIO_MANAGER_H__
#include <memory>
#include <functional>
#include <utils/Message.h>
#include "../oem_call_handler.h"
#include "utils/utils_common.h"

namespace oc {


class IAudioManagerAdapter {
public:
    enum class AudioFile {
        Tone_1,
        Tone_2,
        Tone_3, // 1 sec once
        RCall_error,
        RCall_nocontract,
        RCall_init
    };
    /**
     * @brief requestTelMute This is the function to request/release Audio Arbitration.
     *        The purpose of this function is to MUTE audio of other block. After that, our TSU could
     *        able to play audio file.
     * @param isMute
     * @return
     */
    virtual bool requestTelMute(bool isMute) = 0;
    virtual bool requestMuteBySomeIP(bool isMute) = 0;

    /**
     * @brief requestOpenCallAudioPath wrapper function to
     * @return
     */
    virtual bool requestOpenCallAudioPath() = 0;
    virtual bool requestCloseCallAudioPath() = 0;


    virtual bool requestBStopAudio() = 0;
    virtual bool requestNBStopAudio() = 0;
    virtual bool requestBPlayAudio(AudioFile fileName) = 0;
    virtual bool requestNBPlayAudio(AudioFile fileName) = 0;
    virtual void requestBRetryRegister() = 0;
    virtual void onPlayStateChanged(uint8_t state) = 0;
    virtual bool isAudioPlaying() = 0;
};

/**
 * @brief
 * This is the class which implement method to manager communicate with audio manager
 *
 */
class AudioManagerAdapter: public IAudioManagerAdapter{
public:
    AudioManagerAdapter(android::sp<OemCallHandler> handler);

    bool requestTelMute(bool isMute) override;
    bool requestMuteBySomeIP(bool isMute) override;
    bool requestOpenCallAudioPath() override;
    bool requestCloseCallAudioPath() override;
    bool requestBStopAudio() override;
    bool requestNBStopAudio() override;
    bool requestBPlayAudio(AudioFile fileName) override;
    bool requestNBPlayAudio(AudioFile fileName) override;
    void requestBRetryRegister() override;
    void onPlayStateChanged(uint8_t state) override;
    bool isAudioPlaying() override;

private:
    /**
     * @brief This is the type of structure that contains the private
     *        properties of the instance.  It is defined in the implementation
     *        and declared here to ensure that it is scoped inside the class.
     */
    struct Impl;

    /**
     * @brief This contain private data of the instance
     *
     */
    static std::unique_ptr<Impl> impl_;
};

} // namespace OEMCall App
#endif
