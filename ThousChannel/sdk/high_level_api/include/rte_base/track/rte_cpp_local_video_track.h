/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include "rte_base/c/track/local_video_track.h"
#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"
#include "rte_base/track/rte_cpp_video_track.h"

namespace rte {

using LocalVideoTrackEvent = ::RteLocalVideoTrackEvent;

/**
 * @brief Configuration class for local video track
 * @technical preview
 */
class LocalVideoTrackConfig : public VideoTrackConfig {
public:
    /**
     * @brief Default constructor for LocalVideoTrackConfig
     * @technical preview
     */
    LocalVideoTrackConfig()
        : VideoTrackConfig(reinterpret_cast<RteVideoTrackConfig*>(new RteLocalVideoTrackConfig())) {
        c_local_video_track_config_ = reinterpret_cast<RteLocalVideoTrackConfig*>(VideoTrackConfig::get_underlying_impl());
        RteLocalVideoTrackConfigInit(c_local_video_track_config_, nullptr);
        is_owner_ = true;
    }

    /**
     * @brief Constructor with existing RteLocalVideoTrackConfig
     * @technical preview
     * @param config Pointer to existing RteLocalVideoTrackConfig
     */
    explicit LocalVideoTrackConfig(RteLocalVideoTrackConfig* config)
        : VideoTrackConfig(reinterpret_cast<RteVideoTrackConfig*>(config)), c_local_video_track_config_(config), is_owner_(false) { 
    }
    
    /**
     * @brief Destructor for LocalVideoTrackConfig
     * @technical preview
     */
    virtual ~LocalVideoTrackConfig() { 
        if(is_owner_ && c_local_video_track_config_ != nullptr){
            RteLocalVideoTrackConfigDeinit(c_local_video_track_config_, nullptr);
            delete c_local_video_track_config_;
            is_owner_ = false;
        }
        c_local_video_track_config_ = nullptr;
    }

    /**
     * @brief Set JSON parameter for local video track configuration
     * @technical preview
     * @param json_parameter JSON string parameter
     * @param err Pointer to Error object (optional)
     */
    void SetJsonParameter(const std::string& json_parameter, Error* err = nullptr) {
        String str(json_parameter.c_str());
        RteLocalVideoTrackConfigSetJsonParameter(c_local_video_track_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get JSON parameter from local video track configuration
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return JSON string parameter
     */
    std::string GetJsonParameter(Error* err = nullptr) {
        String str;
        RteLocalVideoTrackConfigGetJsonParameter(c_local_video_track_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
        return std::string(str.CStr());
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteLocalVideoTrackConfig
     */
    RteLocalVideoTrackConfig* get_underlying_impl() {
        return c_local_video_track_config_;
    }

private:
    RteLocalVideoTrackConfig* c_local_video_track_config_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Information class for local video track
 * @technical preview
 */
class LocalVideoTrackInfo : public VideoTrackInfo {
public:
    /**
     * @brief Default constructor for LocalVideoTrackInfo
     * @technical preview
     */
    LocalVideoTrackInfo() : VideoTrackInfo(reinterpret_cast<RteVideoTrackInfo*>(new RteLocalVideoTrackInfo())){
        c_local_video_track_info_ = reinterpret_cast<RteLocalVideoTrackInfo*>(VideoTrackInfo::get_underlying_impl());
        RteLocalVideoTrackInfoInit(c_local_video_track_info_, nullptr);
        is_owner_ = true;
    }

    /**
     * @brief Constructor with existing RteLocalVideoTrackInfo
     * @technical preview
     * @param info Pointer to existing RteLocalVideoTrackInfo
     */
    explicit LocalVideoTrackInfo(RteLocalVideoTrackInfo* info)
        : VideoTrackInfo(reinterpret_cast<RteVideoTrackInfo*>(info)), c_local_video_track_info_(info), is_owner_(false) { 
    }
    
    /**
     * @brief Destructor for LocalVideoTrackInfo
     * @technical preview
     */
    virtual ~LocalVideoTrackInfo() { 
        if(is_owner_ && c_local_video_track_info_ != nullptr){
            RteLocalVideoTrackInfoDeinit(c_local_video_track_info_, nullptr);
            delete c_local_video_track_info_;
            is_owner_ = false;
        }
        c_local_video_track_info_ = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteLocalVideoTrackInfo
     */
    RteLocalVideoTrackInfo* get_underlying_impl() {
        return c_local_video_track_info_;
    }

private:
    RteLocalVideoTrackInfo* c_local_video_track_info_ = nullptr;
    bool is_owner_ = false;
};

static void OnLocalVideoTrackEvent(RteLocalVideoTrackObserver *self, RteLocalVideoTrackEvent event);
static void OnLocalVideoTrackFrame(RteLocalVideoTrackObserver *self, RteVideoFrame *video_frame);

/**
 * @brief Observer class for local video track events
 * @technical preview
 */
class LocalVideoTrackObserver : public VideoTrackObserver {
public:
    /**
     * @brief Default constructor for LocalVideoTrackObserver
     * @technical preview
     */
    LocalVideoTrackObserver() {
        c_observer = RteLocalVideoTrackObserverCreate(nullptr);
        if (c_observer != nullptr) {
            c_observer->video_track_observer.base_observer.me_in_target_lang = this;
            c_observer->on_event = rte::OnLocalVideoTrackEvent;
            c_observer->on_frame = rte::OnLocalVideoTrackFrame;
        }
    }
    
    /**
     * @brief Destructor for LocalVideoTrackObserver
     * @technical preview
     */
    ~LocalVideoTrackObserver() { 
        RteLocalVideoTrackObserverDestroy(c_observer, nullptr); 
    }

    /**
     * @brief Called when local video track event occurs
     * @technical preview
     * @param event The local video track event
     */
    virtual void OnEvent(LocalVideoTrackEvent event) {
    }

    /**
     * @brief Called when local video frame is received
     * @technical preview
     * @param video_frame The video frame data
     */
    virtual void OnFrame(const VideoFrame& video_frame) {
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteLocalVideoTrackObserver
     */
    RteLocalVideoTrackObserver* get_underlying_impl() {
        return c_observer;
    }

private:
    RteLocalVideoTrackObserver* c_observer;
};


void OnLocalVideoTrackEvent(RteLocalVideoTrackObserver *self, RteLocalVideoTrackEvent event) {
    if (self != nullptr) {
        LocalVideoTrackObserver* observer = static_cast<LocalVideoTrackObserver*>(self->video_track_observer.base_observer.me_in_target_lang);
        observer->OnEvent(static_cast<LocalVideoTrackEvent>(event));
    }
}

void OnLocalVideoTrackFrame(RteLocalVideoTrackObserver *self, RteVideoFrame *video_frame) {
    if (self != nullptr) {
        LocalVideoTrackObserver* observer = static_cast<LocalVideoTrackObserver*>(self->video_track_observer.base_observer.me_in_target_lang);
        VideoFrame frame(video_frame);
        observer->OnFrame(frame);
    }
}

/**
 * @brief Local video track class for managing local video streams
 * @technical preview
 */
class LocalVideoTrack : public VideoTrack {
public:

    LocalVideoTrack() = default;

    explicit LocalVideoTrack(const RteHandle& local_video_track_handle) : VideoTrack(local_video_track_handle) {
        c_local_video_track.handle = local_video_track_handle;
    }

    explicit LocalVideoTrack(const RteLocalVideoTrack &&local_video_track) : VideoTrack(RteHandle(local_video_track.handle)) {
        c_local_video_track = local_video_track;
    }

    virtual ~LocalVideoTrack() = default;

    /**
     * @brief Start the local video track
     * @technical preview
     * @param cb Callback function called when operation completes
     */
    void Start(std::function<void(Error* err)> cb) {
        CallbackContext<LocalVideoTrack>* callbackCtx = new CallbackContext<LocalVideoTrack>(this, cb);
        RteLocalVideoTrackStart(&c_local_video_track, &CallbackFunc<::RteLocalVideoTrack, LocalVideoTrack>, callbackCtx);
    }

    /**
     * @brief Stop the local video track
     * @technical preview
     * @param cb Callback function called when operation completes
     */
    void Stop(std::function<void(Error* err)> cb) {
        CallbackContext<LocalVideoTrack>* callbackCtx = new CallbackContext<LocalVideoTrack>(this, cb);
        RteLocalVideoTrackStop(&c_local_video_track, &CallbackFunc<::RteLocalVideoTrack, LocalVideoTrack>, callbackCtx);
    }

    /**
     * @brief Register an observer for local video track events
     * @technical preview
     * @param observer Pointer to LocalVideoTrackObserver
     * @param err Pointer to Error object (optional)
     * @return true if successful, false otherwise
     */
    bool RegisterTrackObserver(LocalVideoTrackObserver* observer, Error* err = nullptr) {
        return RteLocalVideoTrackRegisterTrackObserver(&c_local_video_track, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Unregister an observer from local video track events
     * @technical preview
     * @param observer Pointer to LocalVideoTrackObserver
     * @param err Pointer to Error object (optional)
     * @return true if successful, false otherwise
     */
    bool UnregisterTrackObserver(LocalVideoTrackObserver* observer, Error* err = nullptr) {
        return RteLocalVideoTrackUnregisterTrackObserver(&c_local_video_track, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

  protected:
    RteLocalVideoTrack c_local_video_track = {0};
};

}  // namespace rte
