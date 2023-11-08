#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include "avframequeue.h"

extern "C"{
#include "SDL2/SDL.h"
#include "libswresample/swresample.h"
}

typedef struct AudioParams
{
    int freq; //采样率
    int channels; //通道数
    int64_t channel_layout;
    enum AVSampleFormat fmt;
    int frame_size; //一桢样本数量
}AudioParams;

class AudioOutput
{
public:
    AudioOutput(const AudioParams &audio_params,AVFrameQueue *frame_queue);
    ~AudioOutput();
    int Init();
    int DeInit();
public:
    AudioParams src_tgt;    //解码后的参数
    AudioParams dst_tgt;    //SDL实际输出格式
    AVFrameQueue *frame_queue_ = NULL;

    struct SwrContext * swr_ctx_ = NULL;

    uint8_t *audio_buf_ = NULL;
    uint8_t *audio_buf1_ = NULL;
    uint32_t audio_buf_size = 0;
    uint32_t audio_buf1_size = 0;
    uint32_t audio_buf_index = 0;
};

#endif // AUDIOOUTPUT_H
