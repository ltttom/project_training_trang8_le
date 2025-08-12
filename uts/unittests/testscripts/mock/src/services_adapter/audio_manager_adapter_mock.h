namespace oc {

class MockIAudioManagerAdapter : public IAudioManagerAdapter {
    public:
        MockIAudioManagerAdapter() : IAudioManagerAdapter() {}
        MOCK_METHOD1(requestTelMute, bool(bool isMute));
        MOCK_METHOD1(requestMuteBySomeIP, bool(bool isMute));
        MOCK_METHOD0(requestOpenCallAudioPath, bool());
        MOCK_METHOD0(requestCloseCallAudioPath, bool());
        MOCK_METHOD0(requestBStopAudio, bool());
        MOCK_METHOD0(requestNBStopAudio, bool());
        MOCK_METHOD1(requestBPlayAudio, bool(AudioFile fileName));
        MOCK_METHOD1(requestNBPlayAudio, bool(AudioFile fileName));
        MOCK_METHOD0(requestBRetryRegister, void());
        MOCK_METHOD0(isAudioPlaying, bool());
};

MockIAudioManagerAdapter *M_IAudioManagerAdapter;

bool IAudioManagerAdapter::requestTelMute(bool isMute) {
    return M_IAudioManagerAdapter->requestTelMute(isMute);
}

bool IAudioManagerAdapter::requestMuteBySomeIP(bool isMute) {
    return M_IAudioManagerAdapter->requestMuteBySomeIP(isMute);
}

bool IAudioManagerAdapter::requestOpenCallAudioPath() {
    return M_IAudioManagerAdapter->requestOpenCallAudioPath();
}

bool IAudioManagerAdapter::requestCloseCallAudioPath() {
    return M_IAudioManagerAdapter->requestCloseCallAudioPath();
}

bool IAudioManagerAdapter::requestBStopAudio() {
    return M_IAudioManagerAdapter->requestBStopAudio();
}

bool IAudioManagerAdapter::requestNBStopAudio() {
    return M_IAudioManagerAdapter->requestNBStopAudio();
}

bool IAudioManagerAdapter::requestBPlayAudio(AudioFile fileName) {
    return M_IAudioManagerAdapter->requestBPlayAudio(fileName);
}

bool IAudioManagerAdapter::requestNBPlayAudio(AudioFile fileName) {
    return M_IAudioManagerAdapter->requestNBPlayAudio(fileName);
}

void IAudioManagerAdapter::requestBRetryRegister() {
    return M_IAudioManagerAdapter->requestBRetryRegister();
}

void IAudioManagerAdapter::onPlayStateChanged(uint8_t state) {
    //return M_IAudioManagerAdapter->onPlayStateChanged(state);
}

bool IAudioManagerAdapter::isAudioPlaying() {
    return M_IAudioManagerAdapter->isAudioPlaying();
}

}  // namespace oc

namespace oc {

class MockAudioManagerAdapter : public AudioManagerAdapter {
    public:
        MockAudioManagerAdapter() : AudioManagerAdapter(nullptr){}
        MOCK_METHOD1(requestTelMute, bool(bool isMute));
        MOCK_METHOD1(requestMuteBySomeIP, bool(bool isMute));
        MOCK_METHOD0(requestOpenCallAudioPath, bool());
        MOCK_METHOD0(requestCloseCallAudioPath, bool());
        MOCK_METHOD0(requestBStopAudio, bool());
        MOCK_METHOD0(requestNBStopAudio, bool());
        MOCK_METHOD1(requestBPlayAudio, bool(AudioFile fileName));
        MOCK_METHOD1(requestNBPlayAudio, bool(AudioFile fileName));
        MOCK_METHOD0(requestBRetryRegister, void());
        MOCK_METHOD1(onPlayStateChanged, void(uint8_t state));
        MOCK_METHOD0(isAudioPlaying, bool());
};

MockAudioManagerAdapter *M_AudioManagerAdapter;

AudioManagerAdapter::AudioManagerAdapter(android::sp<OemCallHandler> handler)
//    :impl_(new Impl())
{
}

bool AudioManagerAdapter::requestTelMute(bool isMute) {
    return M_AudioManagerAdapter->requestTelMute(isMute);
}

bool AudioManagerAdapter::requestMuteBySomeIP(bool isMute) {
    return M_AudioManagerAdapter->requestMuteBySomeIP(isMute);
}

bool AudioManagerAdapter::requestOpenCallAudioPath() {
    return M_AudioManagerAdapter->requestOpenCallAudioPath();
}

bool AudioManagerAdapter::requestCloseCallAudioPath() {
    return M_AudioManagerAdapter->requestCloseCallAudioPath();
}

bool AudioManagerAdapter::requestBStopAudio() {
    return M_AudioManagerAdapter->requestBStopAudio();
}

bool AudioManagerAdapter::requestNBStopAudio() {
    return M_AudioManagerAdapter->requestNBStopAudio();
}

bool AudioManagerAdapter::requestBPlayAudio(AudioFile fileName) {
    return M_AudioManagerAdapter->requestBPlayAudio(fileName);
}

bool AudioManagerAdapter::requestNBPlayAudio(AudioFile fileName) {
    return M_AudioManagerAdapter->requestNBPlayAudio(fileName);
}

void AudioManagerAdapter::requestBRetryRegister() {
    return M_AudioManagerAdapter->requestBRetryRegister();
}

void AudioManagerAdapter::onPlayStateChanged(uint8_t state) {
    return M_AudioManagerAdapter->onPlayStateChanged(state);
}

bool AudioManagerAdapter::isAudioPlaying() {
    return M_AudioManagerAdapter->isAudioPlaying();
}

}  // namespace oc

