
#include "VideoPlayerWin.h"

#define __STDC_CONSTANT_MACROS

#include <thread>
#include "VideoPlayerWin.h"

using namespace std;

#pragma comment (lib,"avcodec.lib")
#pragma comment (lib,"avformat.lib")
#pragma comment (lib,"avutil.lib")
#pragma comment (lib,"swresample.lib")

extern "C"
{
#include "libavutil/imgutils.h"
#include "libavutil/timestamp.h"
#include "libavutil/log.h"
#include "libavutil/error.h"
#include "libavutil/time.h"
}

VideoPlayerWin::VideoPlayerWin() {
    m_video_frame_queue.setReleaseHandle(AVBase::releaseVideoFrame, this);
    avformat_network_init();
    UpdateCurrentState(AV_PLAYER_IDLE);
}

VideoPlayerWin::~VideoPlayerWin() {
    m_video_frame_queue.setReleaseHandle(nullptr, nullptr);
    avformat_network_deinit();
    avformat_close_input(&p_format_context);
    avformat_free_context(p_format_context);
    avcodec_free_context(&p_codec_context);
}

void VideoPlayerWin::Unload()
{
}
void VideoPlayerWin::destoryInstance() {
}

void VideoPlayerWin::releaseVideoFrameImpl(std::shared_ptr<GWVideoFrame>& frame) {
    //        if(nullptr != frame) {
    //            free(frame);
    //            frame = nullptr;
    //        }
}

void VideoPlayerWin::FreeFrame() {
}

void VideoPlayerWin::GetYUV(const AVFrame* avFrame)
{
    int index = 0;
    m_video_width = avFrame->width;
    m_video_height = avFrame->height;
    if (!yuv_buf)
    {
        int size = ((m_video_width * m_video_height * 3) >> 1);
        //yuv_buf = std::make_shared<uint8_t>(new uint8_t[size]);// (uint8_t*)malloc((m_video_width * m_video_height * 3) >> 1);
        yuv_buf = std::shared_ptr<GWVideoFrame>(new GWVideoFrame[size], [](GWVideoFrame* ptr) {delete[] ptr;});
    }
    m_video_frame_queue.setEnable(true);
    if (p_codec_context->pix_fmt == AV_PIX_FMT_YUV420P)
    {
        for (index = 0; index < m_video_height; index++)
        {
            memcpy(yuv_buf.get() + m_video_width * index, avFrame->data[0] + index * avFrame->linesize[0], m_video_width);
        }
        int u_step = m_video_width * m_video_height;
        int half_width = m_video_width / 2;
        int half_height = m_video_height / 2;
        for (index = 0; index < half_height; index++)
        {
            memcpy(yuv_buf.get() + u_step + half_width * index, avFrame->data[1] + index * avFrame->linesize[1], half_width);
        }
        int v_step = m_video_width * m_video_height + m_video_width / 2 * m_video_height / 2;
        for (index = 0; index < half_height; index++)
        {
            memcpy(yuv_buf.get() + v_step + half_width * index, avFrame->data[2] + index * avFrame->linesize[2], half_width);
        }
    }
    else
    {
    }

    if (!p_WriteYuv)
    {
        p_WriteYuv = std::make_shared<WriteYuv>();
    }
    p_WriteYuv.get()->StoreYuvData(yuv_buf.get(), m_video_width, m_video_height);

    m_video_frame_queue.enQueue(yuv_buf);
    m_ready_flag = true;
}

void VideoPlayerWin::DecodeVideo() {
    m_video_frame_queue.setEnable(true);
    AVPacket* p_packet = av_packet_alloc();
    AVFrame* p_frame = av_frame_alloc();

    while (m_current_state & AV_PLAYER_STARTED)
    {
        if (av_read_frame(p_format_context, p_packet) == AVERROR_EOF && loop_playing)
        {
            avformat_seek_file(p_format_context, -1, INT64_MIN, p_format_context->start_time, p_format_context->start_time, 0);
            continue;
        }

        if (p_packet->stream_index == i_video_index)
        {
            int result;
            result = avcodec_send_packet(p_codec_context, p_packet);

            if (result < 0)
            {
                break;
            }

            while (result >= 0)
            {
                result = avcodec_receive_frame(p_codec_context, p_frame);
                //Not enought data to decode a frame ,so continue looping.
                if (result == EAGAIN)
                {
                    break;
                }

                if (p_frame->data[0] == NULL)
                {
                    break;
                }

                double extra_delay = p_frame->repeat_pict / (2 * f_fps);
                //av_usleep((extra_delay + 1.0 / fps) * 1000000);
                std::this_thread::sleep_for(std::chrono::milliseconds(long((extra_delay + 1.0 / f_fps) * 1000)));

                GetYUV(p_frame);
                av_frame_unref(p_frame);
                av_packet_unref(p_packet);
            }
        }
        else if (p_packet->stream_index == i_audio_index)
        {}
    }
    m_ready_flag = false;
    av_frame_free(&p_frame);
    av_packet_free(&p_packet);

    UpdateCurrentState(AV_PLAYER_PLAYBACK_COMPLETE);
}

void VideoPlayerWin::Start(const std::string& url) {
    if (m_current_state & AV_PLAYER_STARTED)
    {
        //LOGD("VideoPlayerWin.cpp, Start ,ongoing...");
    }
    else if (m_current_state & (AV_PLAYER_PREPARING | AV_PLAYER_STATE_ERROR))
    {
        //LOGD("VideoPlayerWin.cpp, Start ,prepare failed...");
    }
    else if (m_current_state & (AV_PLAYER_PREPARED | AV_PLAYER_PLAYBACK_COMPLETE | AV_PLAYER_STOPPED))
    {
        UpdateCurrentState(AV_PLAYER_STARTED);
        std::thread decode_thread(&VideoPlayerWin::DecodeVideo, this);
        decode_thread.detach();
    }
    //if (!m_asset_async_loader_)
    //{
    //    m_asset_async_loader_ = std::make_shared<SlaveMaster>(MAX_THREADS_SIZE);
    //}
    //auto decodeJob = [&]() {
    //    DecodeVideo();
    //};

    //m_asset_async_loader_->PushJob(decodeJob);
}

void VideoPlayerWin::Stop(const std::string& url) {
    if (m_current_state & AV_PLAYER_STOPPED)
    {
        return;
    }
    else if (m_current_state & (AV_PLAYER_STARTED | AV_PLAYER_PREPARED | AV_PLAYER_PLAYBACK_COMPLETE))
    {
        UpdateCurrentState(AV_PLAYER_STOPPED);
    }
}

/**
*  @brief Get the video frames.
*  @details Get the video frames,from a Queue.
*
*  @param data The data of video frame, As a out parameter.
*/
void VideoPlayerWin::GetData(std::shared_ptr<GWVideoFrame>& data, const std::string& url, int& width, int& height) {
    width = m_video_width;
    height = m_video_height;
    m_video_frame_queue.deQueue(data);
    //std::unique_lock<std::mutex> uni_lock(m_mutex_l_yuv);
    //m_condition_yuv.notify_all();
    //m_mutex_l_yuv.unlock();
    //LOGD("VideoPlayerWin.cpp,GetData width = %d, height = %d, data= %p", width, height, data.get());
}
bool VideoPlayerWin::IsReady(const std::string& url) {
    //LOGD("VideoPlayerWin.cpp,IsReady,GWMediaPlayer::Instance()->m_ready_flag = %d.", m_ready_flag);
    return m_ready_flag;
}

void VideoPlayerWin::SetUrl(const std::string& url) {
    if (m_current_state & (AV_PLAYER_IDLE | AV_PLAYER_STOPPED))
    {
        UpdateCurrentState(AV_PLAYER_PREPARING);
        media_url = url;
        //av_log_set_level(AV_LOG_INFO); //AV_LOG_INFO,AV_LOG_DEBUG

        AVCodecParameters* p_codec_parameters = nullptr;
        AVStream* p_avStream = nullptr;

        p_format_context = avformat_alloc_context();
        if (avformat_open_input(&p_format_context, media_url.c_str(), NULL, NULL) != 0)
        {
            //LOGD("VideoPlayerWin.cpp,avformat_open_input failed.");
            UpdateCurrentState(AV_PLAYER_STATE_ERROR);
            return;
        }

        if (avformat_find_stream_info(p_format_context, 0) < 0)
        {
            //LOGD("VideoPlayerWin.cpp,avformat_find_stream_info failed.");
            UpdateCurrentState(AV_PLAYER_STATE_ERROR);
            return;
        }
        i_duration = p_format_context->duration;

        for (int i = 0; i < p_format_context->nb_streams; i++)
        {
            p_avStream = p_format_context->streams[i];
            p_codec_parameters = p_avStream->codecpar;

            if (p_codec_parameters->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                i_video_index = i;
                break;
            }
        }
        if (i_video_index == -1)
        {
            //LOGD("VideoPlayerWin.cpp,Didn't find a video stream.");
            UpdateCurrentState(AV_PLAYER_STATE_ERROR);
            return;
        }

        f_fps = av_q2d(p_avStream->avg_frame_rate);

        AVCodec* p_avCodec = avcodec_find_decoder(p_codec_parameters->codec_id);
        if (!p_avCodec)
        {
            //LOGD("VideoPlayerWin.cpp,Couldn't open codec.");
            UpdateCurrentState(AV_PLAYER_STATE_ERROR);
            return;
        }
        p_codec_context = avcodec_alloc_context3(p_avCodec);
        if (avcodec_parameters_to_context(p_codec_context, p_codec_parameters) < 0)
        {
            //LOGD("VideoPlayerWin.cpp,Fill the codec context failed.");
            UpdateCurrentState(AV_PLAYER_STATE_ERROR);
            return;
        }

        if (avcodec_open2(p_codec_context, p_avCodec, NULL) != 0)
        {
            //LOGD("VideoPlayerWin.cpp,Could't open codec.");
            UpdateCurrentState(AV_PLAYER_STATE_ERROR);
            return;
        }

        UpdateCurrentState(AV_PLAYER_PREPARED);
    }
    else
    {
        //LOGD("VideoPlayerWin.cpp,setUrl called in state %d.", m_current_state);
    }
}

/**
*  @brief Update the video frames.
*  @details Update the video frames, at a specific frame rate. Save the video frame to a Queue.
*
*  @param src_ The data of video frame.
*  @param width The width of video frame.
*  @param height The height of video frame.
*/
void VideoPlayerWin::updateVideoFrame(char* src_, char* url, int width, int height, bool flag)
{
}

void VideoPlayerWin::UpdateCurrentState(av_player_states state)
{
    std::unique_lock<std::mutex> state_lock(m_mutex_state);
    m_current_state = state;
}

