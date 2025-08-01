/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include "rte_base/c/track/video_track.h"
#include "rte_base/c/utils/frame.h"
#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_canvas.h"
#include "rte_base/track/rte_cpp_track.h"
#include "rte_base/rte_cpp_callback_utils.h"

namespace rte {

using VideoRenderMode = ::RteVideoRenderMode;
using VideoTrackEvent = ::RteVideoTrackEvent;
using VideoPipelinePosition = ::RteVideoPipelinePosition;

class VideoFrame {
public:
    VideoFrame(const RteVideoFrame* frame) {
        if (frame != nullptr) {
            c_frame = *frame;
        }
    }

    // int GetWidth() const { return c_frame.width; }
    // int GetHeight() const { return c_frame.height; }
    // int GetYStride() const { return c_frame.y_stride; }
    // int GetUStride() const { return c_frame.u_stride; }
    // int GetVStride() const { return c_frame.v_stride; }
    // void* GetYBuffer() const { return c_frame.y_buffer; }
    // void* GetUBuffer() const { return c_frame.u_buffer; }
    // void* GetVBuffer() const { return c_frame.v_buffer; }
    // int64_t GetRenderTimeInMs() const { return c_frame.render_time_in_ms; }
    // int GetRotation() const { return c_frame.rotation; }
    // int GetTextureId() const { return c_frame.texture_id; }
    // float GetMatrix() const { return c_frame.matrix; }
    // int64_t GetPresentationInMs() const { return c_frame.presentation_in_ms; }
    // int GetAvsyncType() const { return c_frame.avsync_type; }
    // int GetMetadataBuffer() const { return c_frame.metadata_buffer; }
    // int GetMetadataSize() const { return c_frame.metadata_size; }
    // int GetTextureSink() const { return c_frame.texture_sink; }

private:
    RteVideoFrame c_frame;
};

class VideoTrackConfig : public TrackConfig {
public:
    VideoTrackConfig()
        : TrackConfig(reinterpret_cast<RteTrackConfig*>(new RteVideoTrackConfig())) {
        c_video_track_config_ = reinterpret_cast<RteVideoTrackConfig*>(TrackConfig::get_underlying_impl());
        RteVideoTrackConfigInit(c_video_track_config_, nullptr);
        is_owner_ = true;
    }

    explicit VideoTrackConfig(RteVideoTrackConfig* config)
        : TrackConfig(reinterpret_cast<RteTrackConfig*>(config)), c_video_track_config_(config), is_owner_(false) { 
    }
    
    virtual ~VideoTrackConfig() { 
        if(is_owner_ && c_video_track_config_ != nullptr){
            RteVideoTrackConfigDeinit(c_video_track_config_, nullptr);
            delete c_video_track_config_;
            is_owner_ = false;
        }
        c_video_track_config_ = nullptr;
    }

    RteVideoTrackConfig* get_underlying_impl() {
        return c_video_track_config_;
    }

private:
    RteVideoTrackConfig* c_video_track_config_ = nullptr;
    bool is_owner_ = false;
};

class VideoTrackInfo : public TrackInfo {
public:
    VideoTrackInfo() : TrackInfo(reinterpret_cast<RteTrackInfo*>(new RteVideoTrackInfo())){
        c_video_track_info_ = reinterpret_cast<RteVideoTrackInfo*>(TrackInfo::get_underlying_impl());
        RteVideoTrackInfoInit(c_video_track_info_, nullptr);
        is_owner_ = true;
    }

    explicit VideoTrackInfo(RteVideoTrackInfo* info)
        : TrackInfo(reinterpret_cast<RteTrackInfo*>(info)), c_video_track_info_(info), is_owner_(false) { 
    }
    
    virtual ~VideoTrackInfo() { 
        if(is_owner_ && c_video_track_info_ != nullptr){
            RteVideoTrackInfoDeinit(c_video_track_info_, nullptr);
            delete c_video_track_info_;
            is_owner_ = false;
        }
        c_video_track_info_ = nullptr;
    }

    RteVideoTrackInfo* get_underlying_impl() {
        return c_video_track_info_;
    }

private:
    RteVideoTrackInfo* c_video_track_info_ = nullptr;
    bool is_owner_ = false;
};

// 静态回调函数声明
static void OnVideoTrackEvent(RteVideoTrackObserver *self, RteVideoTrackEvent event);
static void OnVideoTrackFrame(RteVideoTrackObserver *self, RteVideoFrame *video_frame);

class VideoTrackObserver : public TrackObserver {
public:
    VideoTrackObserver() {
        c_observer = RteVideoTrackObserverCreate(nullptr);
        if (c_observer != nullptr) {
            c_observer->base_observer.me_in_target_lang = this;
            // 注意：C接口中这些回调函数被注释掉了，暂时无法设置
            // c_observer->on_event = rte::OnVideoTrackEvent;
            // c_observer->on_frame = rte::OnVideoTrackFrame;
        }
    }
    
    ~VideoTrackObserver() { 
        RteVideoTrackObserverDestroy(c_observer, nullptr); 
    }

    /**
     * @brief Called when video track event occurs
     * @technical preview
     * @param event The video track event
     */
    virtual void OnEvent(VideoTrackEvent event) {
    }

    /**
     * @brief Called when video frame is received
     * @technical preview
     * @param video_frame The video frame data
     */
    virtual void OnFrame(const VideoFrame& video_frame) {
    }

    RteVideoTrackObserver* get_underlying_impl() {
        return c_observer;
    }

private:
    RteVideoTrackObserver* c_observer;
};

// 静态回调函数实现
void OnVideoTrackEvent(RteVideoTrackObserver *self, RteVideoTrackEvent event) {
    if (self != nullptr) {
        VideoTrackObserver* observer = static_cast<VideoTrackObserver*>(self->base_observer.me_in_target_lang);
        observer->OnEvent(static_cast<VideoTrackEvent>(event));
    }
}

void OnVideoTrackFrame(RteVideoTrackObserver *self, RteVideoFrame *video_frame) {
    if (self != nullptr) {
        VideoTrackObserver* observer = static_cast<VideoTrackObserver*>(self->base_observer.me_in_target_lang);
        VideoFrame frame(video_frame);
        observer->OnFrame(frame);
    }
}

class VideoTrack : public Track {
public:
    VideoTrack() = default;

    explicit VideoTrack(const RteHandle& video_track_handle) : Track(video_track_handle) {
        c_video_track.handle = video_track_handle;
    }

    explicit VideoTrack(const RteVideoTrack &&video_track) : Track(RteHandle(video_track.handle)) {
        c_video_track = video_track;
    }

    virtual ~VideoTrack() = default;



    void SetCanvas(Canvas* canvas, VideoPipelinePosition position, std::function<void(Error* err)> cb) {

      std::function<void(RteCanvas* canvas, Error *err)> cb_func = [cb](RteCanvas* canvas, Error* err) {
      };

      CallbackContextWithArgs<VideoTrack, RteCanvas*>* callbackCtx = new CallbackContextWithArgs<VideoTrack, RteCanvas*>(this, cb_func); 

      RteVideoTrackSetCanvas(&c_video_track, canvas != nullptr ? canvas->get_underlying_impl() : nullptr, 
                              static_cast<RteVideoPipelinePosition>(position), &CallbackFuncWithArgs<::RteVideoTrack, VideoTrack, RteCanvas*>, callbackCtx);
    }

    bool RegisterTrackObserver(VideoTrackObserver* observer, Error* err = nullptr) {
        return RteVideoTrackRegisterTrackObserver(&c_video_track, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    bool UnregisterTrackObserver(VideoTrackObserver* observer, Error* err = nullptr) {
        return RteVideoTrackUnregisterTrackObserver(&c_video_track, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    RteVideoTrack* get_underlying_impl() {
        return &c_video_track;
    }

 protected:
    RteVideoTrack c_video_track;
};

}  // namespace rte
