#ifndef AVFRAMEQUEUE_H
#define AVFRAMEQUEUE_H
#include"queue.h"

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
}

class AVFrameQueue
{
public:
    AVFrameQueue();
    ~AVFrameQueue();
    void Abort();
    int Push(AVFrame *val);
    AVFrame *Pop(const int timeout);
    AVFrame *Front();
    int Size();
private:
    void release();
    Queue<AVFrame *>queue_;
};

#endif // AVFRAMEQUEUE_H
