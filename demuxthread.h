#ifndef DEMUXTHREAD_H
#define DEMUXTHREAD_H
#include "thread.h"
#include "avpacketqueue.h"
extern "C"{
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
}

class Demuxthread : public Thread
{
public:
    Demuxthread(AVPacketQueue *audio_queue, AVPacketQueue *video_queue);
    ~Demuxthread();
    int Init(const char *url);
    int Start();
    int Stop();
    void Run();

    AVCodecParameters *AudioCodecParameters();
    AVCodecParameters *VideoCodecParameters();

private:
    char err2str[256] = {0};
    std::string url_;
    AVPacketQueue *audio_queue_ = NULL;
    AVPacketQueue *video_queue_ = NULL;

    AVFormatContext *ifmt_ctx = NULL;
    int audio_index = -1;
    int video_index = -1;
};

#endif // DEMUXTHREAD_H
