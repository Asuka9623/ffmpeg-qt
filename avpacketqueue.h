#ifndef AVPACKETQUEUE_H
#define AVPACKETQUEUE_H
#include"queue.h"

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
}

class AVPacketQueue
{
public:
    AVPacketQueue();
    ~AVPacketQueue();
    void Abort();
    void release();
    int Size();
    int Push(AVPacket *val);
    AVPacket *Pop(const int timeout);
    AVPacket *Front();
private:
    Queue<AVPacket *> queue_;
};

#endif // AVPACKETQUEUE_H
