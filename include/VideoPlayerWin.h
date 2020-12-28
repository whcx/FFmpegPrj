#pragma once

#include "AVBase.h"

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
//#ifndef _CRT_SECURE_NO_WARNINGS
//#define _CRT_SECURE_NO_WARNINGS
//#endif // !_CRT_SECURE_NO_WARNINGS

# pragma warning (disable:4819)

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

#include "WriteYuv.h"

class VideoPlayerWin : public AVBase
{
public:
    VideoPlayerWin();
    virtual ~VideoPlayerWin();

public:
    virtual void Unload();
    virtual void destoryInstance();

public:
    virtual void SetUrl(const std::string& url);
    virtual void GetData(std::shared_ptr<GWVideoFrame>& data, const std::string& url, int& width, int& height);
    virtual void FreeFrame();
    virtual void Stop(const std::string& url);
    virtual void Start(const std::string& url);
    virtual bool IsReady(const std::string& url);
    virtual void updateVideoFrame(char* src_, char* url, int width, int height, bool flag);
    virtual void releaseVideoFrameImpl(std::shared_ptr<GWVideoFrame>& frame);

private:
    VideoPlayerWin(const VideoPlayerWin& fq) {}
    VideoPlayerWin& operator=(const VideoPlayerWin& fq) {}

private:
    int const MAX_THREADS_SIZE{2};
    void DecodeVideo();
    void GetYUV(const AVFrame* avFrame);
    //static std::shared_ptr<SlaveMaster> m_asset_async_loader_;

    bool prepare_success{false};
    string media_url;
    AVFormatContext* p_format_context = nullptr;
    int64_t i_duration;
    AVCodecContext* p_codec_context = nullptr;
    std::shared_ptr<GWVideoFrame> yuv_buf = nullptr;
    double f_fps = 30.0f;
    int i_video_index = -1;
    int i_audio_index = -1;
    int count_frame = 0;
    //std::mutex m_mutex_l_yuv;
    //std::condition_variable m_condition_yuv;
    bool loop_playing{false};
    std::shared_ptr<WriteYuv> p_WriteYuv = nullptr;;
};