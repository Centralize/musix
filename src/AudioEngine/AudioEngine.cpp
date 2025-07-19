#include "AudioEngine.hpp"

#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

#include <portaudio.h>

#define FRAMES_PER_BUFFER 512

static int paCallback(const void* inputBuffer, void* outputBuffer,
                      unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags, void* userData) {
    PaData* data = (PaData*)userData;
    float* out = (float*)outputBuffer;
    int ret;

    if (!data->isPlaying) {
        return paContinue;
    }

    while (true) {
        ret = av_read_frame(data->formatContext, nullptr);
        if (ret < 0) {
            return paComplete;
        }

        if (data->formatContext->streams[ret]->index == data->audioStreamIndex) {
            ret = avcodec_send_packet(data->codecContext, (const AVPacket*)data->formatContext->streams[ret]);
            if (ret < 0) {
                return paAbort;
            }

            ret = avcodec_receive_frame(data->codecContext, data->frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                continue;
            } else if (ret < 0) {
                return paAbort;
            }

            swr_convert(data->swrContext, (uint8_t**)&out, framesPerBuffer,
                        (const uint8_t**)data->frame->data, data->frame->nb_samples);
            // Apply volume
            for (unsigned int i = 0; i < framesPerBuffer * data->codecContext->ch_layout.nb_channels; ++i) {
                out[i] *= data->volume;
            }
            return paContinue;
        }
    }
}

AudioEngine::AudioEngine() : _stream(nullptr), _paData(new PaData()) {
    _paData->formatContext = nullptr;
    _paData->codecContext = nullptr;
    _paData->frame = nullptr;
    _paData->swrContext = nullptr;
    _paData->audioStreamIndex = -1;
    _paData->isPlaying = false;
    _paData->volume = 1.0f; // Default volume
    _currentSongInfo = {"", "", "", 0}; // Initialize song info
}

AudioEngine::~AudioEngine() {
    if (_stream) {
        Pa_StopStream(_stream);
        Pa_CloseStream(_stream);
    }
    if (_paData->frame) {
        av_frame_free(&_paData->frame);
    }
    if (_paData->swrContext) {
        swr_free(&_paData->swrContext);
    }
    if (_paData->codecContext) {
        avcodec_close(_paData->codecContext);
    }
    if (_paData->formatContext) {
        avformat_close_input(&_paData->formatContext);
    }
    avformat_network_deinit();
    Pa_Terminate();
    delete _paData;
}

SongInfo AudioEngine::getCurrentSongInfo() const {
    return _currentSongInfo;
}

void AudioEngine::setVolume(float volume) {
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    _paData->volume = volume;
    std::cout << "Volume set to: " << (int)(volume * 100) << "%" << std::endl;
}

float AudioEngine::getVolume() const {
    return _paData->volume;
}

void AudioEngine::play(const std::string& filename) {
    PaError err;

    if (_stream && _paData->isPlaying) {
        Pa_StopStream(_stream);
    }

    if (_paData->formatContext) {
        avformat_close_input(&_paData->formatContext);
        _paData->formatContext = nullptr;
    }
    if (_paData->codecContext) {
        avcodec_close(_paData->codecContext);
        _paData->codecContext = nullptr;
    }
    if (_paData->frame) {
        av_frame_free(&_paData->frame);
        _paData->frame = nullptr;
    }
    if (_paData->swrContext) {
        swr_free(&_paData->swrContext);
        _paData->swrContext = nullptr;
    }

    avformat_network_init();

    _paData->formatContext = avformat_alloc_context();
    if (avformat_open_input(&_paData->formatContext, filename.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "Error: could not open file " << filename << std::endl;
        return;
    }

    if (avformat_find_stream_info(_paData->formatContext, nullptr) < 0) {
        std::cerr << "Error: could not find stream info" << std::endl;
        return;
    }

    // Dummy metadata for testing
    _currentSongInfo.title = "Test Title";
    _currentSongInfo.artist = "Test Artist";
    _currentSongInfo.album = "Test Album";
    _currentSongInfo.duration = 180; // 3 minutes

    // Extract metadata
    AVDictionaryEntry *tag = nullptr;
    if ((tag = av_dict_get(_paData->formatContext->metadata, "title", tag, AV_DICT_IGNORE_SUFFIX))) {
        _currentSongInfo.title = tag->value;
    }
    if ((tag = av_dict_get(_paData->formatContext->metadata, "artist", tag, AV_DICT_IGNORE_SUFFIX))) {
        _currentSongInfo.artist = tag->value;
    }
    if ((tag = av_dict_get(_paData->formatContext->metadata, "album", tag, AV_DICT_IGNORE_SUFFIX))) {
        _currentSongInfo.album = tag->value;
    }
    if (_paData->formatContext->duration != AV_NOPTS_VALUE) {
        _currentSongInfo.duration = _paData->formatContext->duration / AV_TIME_BASE;
    }

    _paData->audioStreamIndex = -1;
    const AVCodec* codec = nullptr;
    AVCodecParameters* codecParams = nullptr;

    for (unsigned int i = 0; i < _paData->formatContext->nb_streams; i++) {
        if (_paData->formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            _paData->audioStreamIndex = i;
            codecParams = _paData->formatContext->streams[i]->codecpar;
            codec = avcodec_find_decoder(codecParams->codec_id);
            if (!codec) {
                std::cerr << "Error: unsupported codec" << std::endl;
                return;
            }
            break;
        }
    }

    if (_paData->audioStreamIndex == -1) {
        std::cerr << "Error: could not find audio stream" << std::endl;
        return;
    }

    _paData->codecContext = avcodec_alloc_context3(codec);
    if (avcodec_parameters_to_context(_paData->codecContext, codecParams) < 0) {
        std::cerr << "Error: could not copy codec parameters" << std::endl;
        return;
    }

    if (avcodec_open2(_paData->codecContext, codec, nullptr) < 0) {
        std::cerr << "Error: could not open codec" << std::endl;
        return;
    }

    _paData->frame = av_frame_alloc();

    _paData->swrContext = swr_alloc();
    AVChannelLayout out_ch_layout = AV_CHANNEL_LAYOUT_STEREO;
    av_opt_set_chlayout(_paData->swrContext, "in_chlayout", &_paData->codecContext->ch_layout, 0);
    av_opt_set_chlayout(_paData->swrContext, "out_chlayout", &out_ch_layout, 0);
    av_opt_set_int(_paData->swrContext, "in_sample_rate", _paData->codecContext->sample_rate, 0);
    av_opt_set_int(_paData->swrContext, "out_sample_rate", _paData->codecContext->sample_rate, 0);
    av_opt_set_sample_fmt(_paData->swrContext, "in_sample_fmt", _paData->codecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(_paData->swrContext, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
    swr_init(_paData->swrContext);

    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return;
    }

    err = Pa_OpenDefaultStream(&_stream, 0, 2, paFloat32, _paData->codecContext->sample_rate,
                               FRAMES_PER_BUFFER, paCallback, _paData);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return;
    }

    err = Pa_StartStream(_stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return;
    }

    _paData->isPlaying = true;
    std::cout << "Playing... Press enter to stop." << std::endl;
    std::cin.get();

    err = Pa_StopStream(_stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return;
    }

    _paData->isPlaying = false;
}

void AudioEngine::pause() {
    if (_stream && _paData->isPlaying) {
        Pa_StopStream(_stream);
        _paData->isPlaying = false;
        std::cout << "Paused." << std::endl;
    }
}

void AudioEngine::stop() {
    if (_stream) {
        Pa_StopStream(_stream);
        Pa_CloseStream(_stream);
        _stream = nullptr;
        _paData->isPlaying = false;
        std::cout << "Stopped." << std::endl;
    }
    // Reset FFmpeg contexts for next playback
    if (_paData->frame) {
        av_frame_free(&_paData->frame);
        _paData->frame = nullptr;
    }
    if (_paData->swrContext) {
        swr_free(&_paData->swrContext);
        _paData->swrContext = nullptr;
    }
    if (_paData->codecContext) {
        avcodec_close(_paData->codecContext);
        _paData->codecContext = nullptr;
    }
    if (_paData->formatContext) {
        avformat_close_input(&_paData->formatContext);
        _paData->formatContext = nullptr;
    }
} 

