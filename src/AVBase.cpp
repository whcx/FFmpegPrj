#include "AVBase.h"

//    UpdateFramePtr AVBase::frameAvailableCallback = nullptr;

AVBase::AVBase()
{

}

void AVBase::releaseVideoFrame(std::shared_ptr<GWVideoFrame>&frame, AVBase *avBase) {
    if (avBase)
    {
        avBase->releaseVideoFrameImpl(frame);
    }
}

//    void AVBase::onVideoFrameAvailable(uint8_t *frame, int width, int height) {
//        if (AVBase::frameAvailableCallback != nullptr) {
//            (AVBase::frameAvailableCallback)(frame, width, height);
//        }
//    }
//    void AVBase::setVideoFrameAvailableCallback(UpdateFramePtr callback) {
//        AVBase::frameAvailableCallback = callback;
//    }
