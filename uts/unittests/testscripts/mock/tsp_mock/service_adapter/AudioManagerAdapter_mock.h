#ifndef AUDIO_MANAGER_ADAPTER_MOCK_H
#define AUDIO_MANAGER_ADAPTER_MOCK_H

// using namespace ecallapp;

class AudioManagerAdapterMock : public AudioManagerAdapter {
public:
    MOCK_METHOD0(registerService, void());
    MOCK_METHOD1(openCallAudioPath, void(const int32_t type));
    MOCK_METHOD0(closeCallAudioPath, void());
    MOCK_METHOD1(playAudioFile, bool(const std::string& audioFile));
    MOCK_METHOD0(stopAudioFile, bool());
    MOCK_CONST_METHOD0(isPlaying, bool()); 
    MOCK_METHOD1(onPlayStateChanged, void(const uint8_t state));
    MOCK_METHOD1(handlePlayStateChanged, void(const uint8_t state));
    MOCK_METHOD1(setMute, bool(bool isMute));  
};

AudioManagerAdapterMock *M_AudioManagerAdapter;

AudioManagerAdapter* AudioManagerAdapter::instance = nullptr;
AudioManagerAdapter* AudioManagerAdapter::getInstance() 
{
    if (instance == nullptr) {
        instance = new AudioManagerAdapter();
    }
    return instance;
}

AudioManagerAdapter::AudioManagerAdapter() {}

void AudioManagerAdapter::registerService() {
    M_AudioManagerAdapter->registerService();
}

void AudioManagerAdapter::openCallAudioPath(const int32_t type) {
    M_AudioManagerAdapter->openCallAudioPath(type);
}

void AudioManagerAdapter::closeCallAudioPath() {
    M_AudioManagerAdapter->closeCallAudioPath();
}

bool AudioManagerAdapter::playAudioFile(const std::string& audioFile) {
    return M_AudioManagerAdapter->playAudioFile(audioFile);
}

bool AudioManagerAdapter::stopAudioFile() {
    return M_AudioManagerAdapter->stopAudioFile();
}

bool AudioManagerAdapter::isPlaying() const {
    return M_AudioManagerAdapter->isPlaying();
}

void AudioManagerAdapter::onPlayStateChanged(const uint8_t state) {
    M_AudioManagerAdapter->onPlayStateChanged(state);
}

void AudioManagerAdapter::handlePlayStateChanged(const uint8_t state) {
    M_AudioManagerAdapter->handlePlayStateChanged(state);
}

bool AudioManagerAdapter::setMute(bool isMute) {
    return M_AudioManagerAdapter->setMute(isMute);
}
#endif // AUDIO_MANAGER_ADAPTER_MOCK_H