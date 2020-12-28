
#include "FrameQueue.h"
#include "AVBase.h"


    FrameQueue::FrameQueue() {
 /*   std::unique_lock<std::mutex> uni_lock(m_mutex_cond);
    std::lock_guard<std::mutex> lock_g(m_mutex_cond);*/
    }

    FrameQueue::~FrameQueue() {
        clear();
        delete m_avBase_cb;
    }

/*    FrameQueue& FrameQueue::operator=(const FrameQueue& fq)
    {
        if(this != &fq)
        {
        }
        return *this;
    }*/

    void FrameQueue::enQueue(std::shared_ptr<GWVideoFrame> newFrame) {
        std::unique_lock<std::mutex> uni_lock(m_mutex_cond);
        if (m_queue_enable) {
            if(m_frame_queue.size() >= MAX_NUM) {
                m_frame_queue.pop();
                m_release_handle(newFrame, m_avBase_cb);
                //LOGD("FrameQueue.cpp,enQueue(),drop frame ---");
            }
            m_frame_queue.push(newFrame);
            //uni_lock.unlock();
        } else {
            m_release_handle(newFrame, m_avBase_cb);
        }
        //uni_lock.unlock();
    }

    bool FrameQueue::deQueue(std::shared_ptr<GWVideoFrame>&value){
        bool ret = false;
        std::unique_lock<std::mutex> uni_lock(m_mutex_cond);

        while (m_queue_enable && m_frame_queue.empty()){
            m_condition.wait(uni_lock);
        }
        if (!m_frame_queue.empty()){
            value = m_frame_queue.front();
            if (m_frame_queue.size() > 1)
                m_frame_queue.pop();
            ret = true;
        }
        //uni_lock.unlock();

        return ret;
    }

    void FrameQueue::setEnable(bool enable) {
        std::unique_lock<std::mutex> uni_lock(m_mutex_cond);
        this->m_queue_enable = enable;
        m_condition.notify_all();
        //uni_lock.unlock();
    }

    bool FrameQueue::empty() {
        return m_frame_queue.empty();
    }

    size_t FrameQueue::size() {
        return m_frame_queue.size();
    }

    void FrameQueue::clear() {
        std::lock_guard<std::mutex> lock_g(m_mutex_cond);
        queue<std::shared_ptr<GWVideoFrame>> empty;
        std::swap(m_frame_queue, empty);
    }

    void FrameQueue::setReleaseHandle(ReleaseHandle handle, AVBase *avBase) {
        m_release_handle = handle;
        m_avBase_cb = avBase;
    }

