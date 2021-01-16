#pragma once

#include <string>
#include <unordered_map>

#include "FrameQueue.h"
enum av_player_states {
    AV_PLAYER_STATE_ERROR       = 0,
    AV_PLAYER_IDLE              = 1 << 0,
    AV_PLAYER_INITIALIZED       = 1 << 1,
    AV_PLAYER_PREPARING         = 1 << 2,
    AV_PLAYER_PREPARED          = 1 << 3,
    AV_PLAYER_STARTED           = 1 << 4,
    AV_PLAYER_PAUSED            = 1 << 5,
    AV_PLAYER_STOPPED           = 1 << 6,
    AV_PLAYER_PLAYBACK_COMPLETE = 1 << 7
};

//typedef void (*UpdateFramePtr)(uint8_t* frame, int width, int height);
class AVBase
{
public:
    AVBase();
    virtual ~AVBase() {}
    virtual void Unload() = 0;
    virtual void destoryInstance() = 0;

public:
    virtual void SetUrl(const std::string& url) = 0;
    virtual void GetData(std::shared_ptr<GWVideoFrame>& data, const std::string& url, int& width, int& height) = 0;
    virtual void FreeFrame() = 0;
    virtual void Stop(const std::string& url) = 0;
    virtual void Start(const std::string& url) = 0;
    virtual bool IsReady(const std::string& url) = 0;
    virtual void updateVideoFrame(char* src_, char* url, int width, int height, bool flag) = 0;
    virtual void releaseVideoFrameImpl(std::shared_ptr<GWVideoFrame>& frame) = 0;
    static void releaseVideoFrame(std::shared_ptr<GWVideoFrame>& frame, AVBase *avBase);

    //    void onVideoFrameAvailable(uint8_t* frame, int width, int height);
//    void setVideoFrameAvailableCallback(UpdateFramePtr callback);

private:
    //It is expected that will get tips of compile error when using the following  default methods.
    AVBase(const AVBase& fq) {}
    AVBase& operator=(const AVBase& fq) {return *this;}

public:
    int m_video_width;
    int m_video_height;
    int m_data_size{ 0 };
    bool m_ready_flag{ false };
    FrameQueue m_video_frame_queue;
    //    static UpdateFramePtr frameAvailableCallback;
private:
    AVBase* m_avBase = nullptr;
};