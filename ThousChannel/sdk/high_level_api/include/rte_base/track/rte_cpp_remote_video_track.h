/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include "rte_base/c/track/remote_video_track.h"
#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"
#include "rte_base/track/rte_cpp_video_track.h"

namespace rte {

/**
 * @brief Configuration class for remote video track
 * @technical preview
 */
class RemoteVideoTrackConfig : public VideoTrackConfig {
public:
    /**
     * @brief Default constructor for RemoteVideoTrackConfig
     * @technical preview
     */
    RemoteVideoTrackConfig()
        : VideoTrackConfig(reinterpret_cast<RteVideoTrackConfig*>(new RteRemoteVideoTrackConfig())) {
        c_remote_video_track_config_ = reinterpret_cast<RteRemoteVideoTrackConfig*>(VideoTrackConfig::get_underlying_impl());
        RteRemoteVideoTrackConfigInit(c_remote_video_track_config_, nullptr);
        is_owner_ = true;
    }

    /**
     * @brief Constructor with existing RteRemoteVideoTrackConfig
     * @technical preview
     * @param config Pointer to existing RteRemoteVideoTrackConfig
     */
    explicit RemoteVideoTrackConfig(RteRemoteVideoTrackConfig* config)
        : VideoTrackConfig(reinterpret_cast<RteVideoTrackConfig*>(config)), c_remote_video_track_config_(config), is_owner_(false) { 
    }
    
    /**
     * @brief Destructor for RemoteVideoTrackConfig
     * @technical preview
     */
    virtual ~RemoteVideoTrackConfig() { 
        if(is_owner_ && c_remote_video_track_config_ != nullptr){
            RteRemoteVideoTrackConfigDeinit(c_remote_video_track_config_, nullptr);
            delete c_remote_video_track_config_;
            is_owner_ = false;
        }
        c_remote_video_track_config_ = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteRemoteVideoTrackConfig
     */
    RteRemoteVideoTrackConfig* get_underlying_impl() {
        return c_remote_video_track_config_;
    }

private:
    RteRemoteVideoTrackConfig* c_remote_video_track_config_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Information class for remote video track
 * @technical preview
 */
class RemoteVideoTrackInfo : public VideoTrackInfo {
public:
    /**
     * @brief Default constructor for RemoteVideoTrackInfo
     * @technical preview
     */
    RemoteVideoTrackInfo() : VideoTrackInfo(reinterpret_cast<RteVideoTrackInfo*>(new RteRemoteVideoTrackInfo())){
        c_remote_video_track_info_ = reinterpret_cast<RteRemoteVideoTrackInfo*>(VideoTrackInfo::get_underlying_impl());
        RteRemoteVideoTrackInfoInit(c_remote_video_track_info_, nullptr);
        is_owner_ = true;
    }

    /**
     * @brief Constructor with existing RteRemoteVideoTrackInfo
     * @technical preview
     * @param info Pointer to existing RteRemoteVideoTrackInfo
     */
    explicit RemoteVideoTrackInfo(RteRemoteVideoTrackInfo* info)
        : VideoTrackInfo(reinterpret_cast<RteVideoTrackInfo*>(info)), c_remote_video_track_info_(info), is_owner_(false) { 
    }
    
    /**
     * @brief Destructor for RemoteVideoTrackInfo
     * @technical preview
     */
    virtual ~RemoteVideoTrackInfo() { 
        if(is_owner_ && c_remote_video_track_info_ != nullptr){
            RteRemoteVideoTrackInfoDeinit(c_remote_video_track_info_, nullptr);
            delete c_remote_video_track_info_;
            is_owner_ = false;
        }
        c_remote_video_track_info_ = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteRemoteVideoTrackInfo
     */
    RteRemoteVideoTrackInfo* get_underlying_impl() {
        return c_remote_video_track_info_;
    }

private:
    RteRemoteVideoTrackInfo* c_remote_video_track_info_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Observer class for remote video track events
 * @technical preview
 */
class RemoteVideoTrackObserver : public VideoTrackObserver {
public:
    /**
     * @brief Default constructor for RemoteVideoTrackObserver
     * @technical preview
     */
    RemoteVideoTrackObserver() {
        c_observer = RteRemoteVideoTrackObserverCreate(nullptr);
        if (c_observer != nullptr) {
            c_observer->video_track_observer.base_observer.me_in_target_lang = this;
            // Note: These callback functions are commented out in the C interface and cannot be set for now
            // c_observer->on_event = rte::OnRemoteVideoTrackEvent;
            // c_observer->on_frame = rte::OnRemoteVideoTrackFrame;
        }
    }
    
    /**
     * @brief Destructor for RemoteVideoTrackObserver
     * @technical preview
     */
    ~RemoteVideoTrackObserver() { 
        RteRemoteVideoTrackObserverDestroy(c_observer, nullptr); 
    }

    /**
     * @brief Called when remote video track event occurs
     * @technical preview
     * @param event The remote video track event
     */
    virtual void OnEvent(VideoTrackEvent event) {
    }

    /**
     * @brief Called when remote video frame is received
     * @technical preview
     * @param video_frame The video frame data
     */
    virtual void OnFrame(const VideoFrame& video_frame) {
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteRemoteVideoTrackObserver
     */
    RteRemoteVideoTrackObserver* get_underlying_impl() {
        return c_observer;
    }

private:
    RteRemoteVideoTrackObserver* c_observer;
};

/**
 * @brief Remote video track class for managing remote video streams
 * @technical preview
 */
class RemoteVideoTrack : public VideoTrack {
public:

    RemoteVideoTrack() = default;

    explicit RemoteVideoTrack(const RteHandle& remote_video_track_handle) : VideoTrack(remote_video_track_handle) {
        c_remote_video_track.handle = remote_video_track_handle;
    }

    explicit RemoteVideoTrack(const RteRemoteVideoTrack &&remote_video_track) : VideoTrack(RteHandle(remote_video_track.handle)) {
        c_remote_video_track = remote_video_track;
    }

    virtual ~RemoteVideoTrack() = default;

    /**
     * @brief Register an observer for remote video track events
     * @technical preview
     * @param observer Pointer to RemoteVideoTrackObserver
     * @param destroyer Function to destroy the observer
     * @param err Pointer to Error object (optional)
     */
    void RegisterTrackObserver(RemoteVideoTrackObserver* observer, std::function<void(RemoteVideoTrackObserver* self)> destroyer, Error* err = nullptr) {
        // Note: The callback function signatures in the C interface are different from C++, adaptation is required
        // A simplified implementation is provided here
        RteRemoteVideoTrackRegisterTrackObserver(&c_remote_video_track, observer != nullptr ? observer->get_underlying_impl() : nullptr, nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Unregister an observer from remote video track events
     * @technical preview
     * @param observer Pointer to RemoteVideoTrackObserver
     * @param err Pointer to Error object (optional)
     */
    void UnregisterTrackObserver(RemoteVideoTrackObserver* observer, Error* err = nullptr) {
        RteRemoteVideoTrackUnregisterTrackObserver(&c_remote_video_track, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

private:
    RteRemoteVideoTrack c_remote_video_track = {0};
};

}  // namespace rte
