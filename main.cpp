#include <iostream>
#include "log.h"
#include "demuxthread.h"
#include "avframequeue.h"
#include "decodethread.h"
#include "audiooutput.h"
extern "C"{
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
}

using namespace std;

int main(int argc, char *argv[])
{
    int ret = 0;

    LogInit();
    cout << "Hello World!" << endl;
    AVPacketQueue audio_packet_queue;
    AVPacketQueue video_packet_queue;

    AVFrameQueue audio_frame_queue;
    AVFrameQueue video_frame_queue;

    //1.解复用
    Demuxthread *demux_thread = new Demuxthread(&audio_packet_queue,&video_packet_queue);
    ret = demux_thread->Init(argv[1]);

    if(ret < 0 ){
        LogError("demux_thread.Init faild");
        return -1;
    }

    ret = demux_thread->Start();
    if(ret < 0){
        LogError("demux_thread.Start faild");
        return -1;
    }

    //解码线程初始化
    DecodeThread *audio_decode_thread = new DecodeThread(&audio_packet_queue,&audio_frame_queue);
    ret = audio_decode_thread->Init(demux_thread->AudioCodecParameters());
    if(ret < 0){
        LogError("audio_decode_thread->Init faild");
        return -1;
    }
    ret = audio_decode_thread->Start();
    if(ret < 0){
        LogError("audio_decode_thread->Start() faild");
        return -1;
    }

    DecodeThread *video_decode_thread = new DecodeThread(&video_packet_queue,&video_frame_queue);
    ret = video_decode_thread->Init(demux_thread->VideoCodecParameters());
    if(ret < 0){
        LogError("video_decode_thread->Init faild");
        return -1;
    }
    ret = video_decode_thread->Start();
    if(ret < 0){
        LogError("video_decode_thread->Start() faild");
        return -1;
    }
    //初始化audio输出
    AudioParams audio_params = {0};
    memset(&audio_params , 0, sizeof(AudioParams));
    audio_params.channels = demux_thread->AudioCodecParameters()->channels;
    audio_params.channel_layout = demux_thread->AudioCodecParameters()->channel_layout;
    audio_params.fmt = demux_thread->AudioCodecParameters()->format;
    audio_params.freq = demux_thread->AudioCodecParameters()->sample_rate;
    audio_params.frame_size = demux_thread->AudioCodecParameters()->frame_size;
    AudioOutput *audio_output = new AudioOutput(audio_params, audio_frame_queue);
    ret = audio_output->Init();
    if(ret < 0) {
        LogError("audio_output->Init() faild");
        return -1;
    }

    // 休眠2秒
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    demux_thread->Stop();
    delete demux_thread;

    audio_decode_thread->Stop();
    delete audio_decode_thread;

    video_decode_thread->Stop();
    delete video_decode_thread;

    LogInfo("main finsh");

    return 0;
}
