#include "demuxthread.h"
#include "log.h"
Demuxthread::Demuxthread(AVPacketQueue *audio_queue, AVPacketQueue *video_queue)
    :audio_queue_(audio_queue), video_queue_(video_queue)
{
    LogInfo("Demuxthread");
}

Demuxthread::~Demuxthread()
{
    LogInfo("~Demuxthread");
    if(thread_){
        Stop();
    }
}

int Demuxthread::Init(const char *url)
{
    LogInfo("url:%s",url);
    int ret = 0;
    url_ = url;

    ifmt_ctx = avformat_alloc_context();

    ret = avformat_open_input(&ifmt_ctx,url_.c_str(),NULL,NULL);
    if(ret < 0){
        av_strerror(ret, err2str, sizeof(err2str));
        LogError("avformat_open_input failed, ret:%d, err2str:%s\n",ret, err2str);
        return -1;
    }
    av_dump_format(ifmt_ctx,0,url_.c_str(),0);

    ret = avformat_find_stream_info(ifmt_ctx,NULL);

    audio_index = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);
    video_index = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);

    LogInfo("audio_index_:%d, video_index_:%d",audio_index,video_index);
    if(audio_index < 0 || video_index < 0 ){
        LogError("no audio or no video");
        return -1;

    }

    LogInfo("Init leave");
}

int Demuxthread::Start()
{
    thread_ = new std::thread(&Demuxthread::Run,this);
    if(!thread_){
        LogError("new std::thread(&Demuxthread::Run,this) failed");
        return -1;
    }
    return 0;
}

int Demuxthread::Stop()
{
    Thread::Stop();
    avformat_close_input(&ifmt_ctx);
}

void Demuxthread::Run()
{
    int ret = 0;
    AVPacket pkt;

    while(abort_ !=1 ){

        if(audio_queue_->Size() > 100 ||video_queue_->Size() > 100){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        ret = av_read_frame(ifmt_ctx, &pkt);
        if(ret < 0){
            av_strerror(ret, err2str, sizeof(err2str));
            LogError("av_read_frame failed, ret:%d, err2str:%s\n",ret, err2str);
            break;
        }
        if(pkt.stream_index == audio_index){
            audio_queue_->Push(&pkt);
            LogInfo("audio pkt queue size :%d",audio_queue_->Size());
        }else if(pkt.stream_index == video_index){
            video_queue_->Push(&pkt);
            LogInfo("video pkt queue size :%d",video_queue_->Size());
        }else{
            av_packet_unref(&pkt);
        }

    }
    LogInfo("run finish");
}

AVCodecParameters *Demuxthread::AudioCodecParameters()
{
    if(audio_index != -1){
        return ifmt_ctx->streams[audio_index]->codecpar;
    }else{
        return NULL;
    }
}

AVCodecParameters *Demuxthread::VideoCodecParameters()
{
    if(video_index != -1){
        return ifmt_ctx->streams[video_index]->codecpar;
    }else{
        return NULL;
    }
}
