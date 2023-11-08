#include "audiooutput.h"
#include "log.h"
AudioOutput::AudioOutput(const AudioParams &audio_params,AVFrameQueue *frame_queue)
    :src_tgt(audio_params),frame_queue_(frame_queue)
{

}

AudioOutput::~AudioOutput()
{

}

void fill_audio_pcm(void *udata, Uint8 *stream, int len){

    // 1.从frame queue读取解码后的pcm数据填充到stream中
    // 2.
    AudioOutput *is = (AudioOutput *)udata;
    int len1 = 0;
    int audio_size = 0;

    while(len > 0){
        if(is->audio_buf_index == is->audio_buf_size) {
            is->audio_buf_index = 0;
            AVFrame *frame = is->frame_queue_->Pop(10);
            if(frame) {
                //读到解码后的数据
                //判断是否做重采样
                if( ((frame->format != is->dst_tgt.fmt)
                    || (frame->sample_rate != is->dst_tgt.freq)
                    || (frame->channel_layout != is->dst_tgt.channel_layout))
                    && (!is->swr_ctx_) ){
                    is->swr_ctx_ = swr_alloc_set_opts(NULL,
                                                      is->dst_tgt.channel_layout,
                                                      (enum AVSampleFormat)is->dst_tgt.fmt,
                                                      is->dst_tgt.freq,
                                                      frame->channel_layout,
                                                      (enum AVSampleFormat)frame->format,
                                                      frame->sample_rate,
                                                      0, NULL);
                    if(!is->swr_ctx_ || swr_init(is->swr_ctx_) < 0) {
                        LogError(
                            "Cannot create sample rate",
                            frame->sample_rate,
                            av_get_sample_fmt_name((enum AVSampleFormat)frame->format),
                            frame->channels,
                            is->dst_tgt.freq,
                            //av_set_sample_fmt_name((enum AVSampleFormat)is->dst_tgt.fmt),
                            is->dst_tgt.channels);
                        swr_free((SwrContext **)(&is->swr_ctx_));
                        return ;
                    }
                }
                if(!is->swr_ctx_) {
                    uint8_t **in = (uint8_t **)frame->extended_data;
                    uint8_t **out = &is->audio_buf1_;

                    int out_samples = frame->nb_samples * is->dst_tgt.freq/frame->sample_rate + 256;
                    int out_bytes = av_samples_get_buffer_size(NULL, is->dst_tgt.channels, out_samples, is->dst_tgt.fmt,0);
                    if(out_bytes < 0){
                        LogError("av_samples_get_buffer_size failed");
                        return ;
                    }
                    av_fast_malloc(&is->audio_buf1_, &is->audio_buf1_size, audio_size);

                    int len2 = swr_convert(is->swr_ctx_, out, out_samples, (const Uint8 **)in , frame->nb_samples);
                    if(len2 < 0){
                        LogError("swr_convert failed");
                        return ;
                    }
                    is->audio_buf_ = is->audio_buf1_;
                    is->audio_buf_size = av_samples_get_buffer_size(NULL, is->dst_tgt.channels, len2, is->dst_tgt.fmt, 1);;


                }else { //没有重采样
                    audio_size = av_samples_get_buffer_size(NULL, frame->channels, frame->nb_samples, (enum AVSampleFormat)frame->format, 1);
                    av_fast_malloc(&is->audio_buf1_, &is->audio_buf1_size, audio_size);
                    is->audio_buf_ = is->audio_buf1_;
                    is->audio_buf_size = is->audio_buf1_size;
                    memcpy(is->audio_buf_, frame->data[0],audio_size);
                }


                av_frame_free(&frame);
            }else {
                //没有读到解码后的数据

                is->audio_buf_ = NULL;
                is->audio_buf1_size = 512;
            }
        }
        len1 = is->audio_buf1_size - is->audio_buf_index;
        if(len1 > len)
            len1 = len;

        if(!is->audio_buf_){
            memset(stream, 0 ,len1);
        } else {
            //真真拷贝有效数据
            memcpy(stream, is->audio_buf_ + is->audio_buf_index , len1);

        }
        len -= len1;
        stream += len1;
        is->audio_buf_index += len1;
    }
}

int AudioOutput::Init()
{
    if(SDL_Init(SDL_INIT_AUDIO) !=0 ){
        LogError("SDL_Init failed");
        return -1;
    }

    SDL_AudioSpec wanted_spec, spec;
    wanted_spec.channels = src_tgt.channels;
    wanted_spec.freq = src_tgt.freq;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.silence = 0;
    wanted_spec.callback =fill_audio_pcm();
    wanted_spec.userdata = this;
    wanted_spec.samples = src_tgt.frame_size;//采样数量



    int ret = SDL_OpenAudio(&wanted_spec, &spec);
    if(ret < 0){
        LogError("SDL_OpenAudio failed");
        return -1;
    }

    dst_tgt.channels = spec.channels;
    dst_tgt.fmt = AV_SAMPLE_FMT_S16;
    dst_tgt.freq = spec.freq;
    dst_tgt.channel_layout = av_get_default_channel_layout(src_tgt.channels);
    dst_tgt.frame_size = src_tgt.frame_size;
    SDL_PauseAudio(0);

    LogInfo("AudioOutput::Init() leave");
}

int AudioOutput::DeInit()
{
    SDL_PauseAudio(1);
}
