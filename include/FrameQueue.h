#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

class AVBase;
typedef uint8_t GWVideoFrame;
typedef void (*ReleaseHandle)(std::shared_ptr<GWVideoFrame>& frame, AVBase* avBase);

using namespace std;

class FrameQueue{
public:
    FrameQueue();
    ~FrameQueue();

public:
    void enQueue(std::shared_ptr<GWVideoFrame> newFrame);
    bool deQueue(std::shared_ptr<GWVideoFrame>&value);
    void setEnable(bool enable);
    bool empty();
    size_t size();
    void clear();
    void setReleaseHandle(ReleaseHandle handle, AVBase *avBase);
    const int MAX_NUM = 4; //In sync with MAX_BUFFERS of GWAVUtils.java

private:
    //It is expected that will get tips of compile error when using the following  default methods.
    FrameQueue(const FrameQueue& fq) {}
    FrameQueue& operator=(const FrameQueue& fq){return *this;}

private:
    std::mutex m_mutex_cond;
    std::condition_variable m_condition;

    queue<std::shared_ptr<GWVideoFrame>> m_frame_queue;
    bool m_queue_enable;
    ReleaseHandle m_release_handle;
    AVBase* m_avBase_cb;
};
