#pragma once

#include <string>

#include <portaudio.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

struct PaData {
    AVFormatContext* formatContext;
    AVCodecContext* codecContext;
    AVFrame* frame;
    SwrContext* swrContext;
    int audioStreamIndex;
    bool isPlaying;
    float volume;
};

struct SongInfo {
    std::string title;
    std::string artist;
    std::string album;
    long duration; // in seconds
};

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    void play(const std::string& filename);
    void pause();
    void stop();
    void setVolume(float volume);
    float getVolume() const;
    SongInfo getCurrentSongInfo() const;

private:
    PaStream* _stream;
    PaData* _paData;
    SongInfo _currentSongInfo;
};