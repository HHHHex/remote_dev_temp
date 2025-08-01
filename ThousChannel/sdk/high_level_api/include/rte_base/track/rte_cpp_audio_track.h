/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include "rte_base/c/track/audio_track.h"
#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"
#include "rte_base/c/track/track.h"
#include "rte_base/track/rte_cpp_track.h"

namespace rte {

/**
 * @brief Configuration class for audio track
 * @technical preview
 */
class AudioTrackConfig : public TrackConfig {
public:
    /**
     * @brief Default constructor for AudioTrackConfig
     * @technical preview
     */
    AudioTrackConfig()
        : TrackConfig(reinterpret_cast<RteTrackConfig*>(new RteAudioTrackConfig())) {
        c_audio_track_config_ = reinterpret_cast<RteAudioTrackConfig*>(TrackConfig::get_underlying_impl());
        RteAudioTrackConfigInit(c_audio_track_config_, nullptr);
        is_owner_ = true;
    }

    /**
     * @brief Constructor with existing RteAudioTrackConfig
     * @technical preview
     * @param config Pointer to existing RteAudioTrackConfig
     */
    explicit AudioTrackConfig(RteAudioTrackConfig* config)
        : TrackConfig(reinterpret_cast<RteTrackConfig*>(config)), c_audio_track_config_(config), is_owner_(false) { 
    }
    
    /**
     * @brief Destructor for AudioTrackConfig
     * @technical preview
     */
    virtual ~AudioTrackConfig() { 
        if(is_owner_ && c_audio_track_config_ != nullptr){
            RteAudioTrackConfigDeinit(c_audio_track_config_, nullptr);
            delete c_audio_track_config_;
            is_owner_ = false;
        }
        c_audio_track_config_ = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteAudioTrackConfig
     */
    RteAudioTrackConfig* get_underlying_impl() {
        return c_audio_track_config_;
    }

private:
    RteAudioTrackConfig* c_audio_track_config_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Information class for audio track
 * @technical preview
 */
class AudioTrackInfo : public TrackInfo {
public:
    /**
     * @brief Default constructor for AudioTrackInfo
     * @technical preview
     */
    AudioTrackInfo() : TrackInfo(reinterpret_cast<RteTrackInfo*>(new RteAudioTrackInfo())){
        c_audio_track_info_ = reinterpret_cast<RteAudioTrackInfo*>(TrackInfo::get_underlying_impl());
        RteAudioTrackInfoInit(c_audio_track_info_, nullptr);
        is_owner_ = true;
    }

    /**
     * @brief Constructor with existing RteAudioTrackInfo
     * @technical preview
     * @param info Pointer to existing RteAudioTrackInfo
     */
    explicit AudioTrackInfo(RteAudioTrackInfo* info)
        : TrackInfo(reinterpret_cast<RteTrackInfo*>(info)), c_audio_track_info_(info), is_owner_(false) {
    }
    
    /**
     * @brief Destructor for AudioTrackInfo
     * @technical preview
     */
    virtual ~AudioTrackInfo() { 
        if(is_owner_ && c_audio_track_info_ != nullptr){
            RteAudioTrackInfoDeinit(c_audio_track_info_, nullptr);
            delete c_audio_track_info_;
            is_owner_ = false;
        }
        c_audio_track_info_ = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteAudioTrackInfo
     */
    RteAudioTrackInfo* get_underlying_impl() {
        return c_audio_track_info_;
    }

private:
    RteAudioTrackInfo* c_audio_track_info_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Observer class for audio track events
 * @technical preview
 */
class AudioTrackObserver : public TrackObserver {
public:
    /**
     * @brief Default constructor for AudioTrackObserver
     * @technical preview
     */
    AudioTrackObserver() { 
        c_observer = RteAudioTrackObserverCreate(nullptr);
    }
    
    /**
     * @brief Destructor for AudioTrackObserver
     * @technical preview
     */
    ~AudioTrackObserver() { 
        RteAudioTrackObserverDestroy(c_observer, nullptr); 
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteAudioTrackObserver
     */
    RteAudioTrackObserver* get_underlying_impl() {
        return c_observer;
    }

private:
    RteAudioTrackObserver* c_observer;
};

/**
 * @brief Audio track class for managing audio streams
 * @technical preview
 */
class AudioTrack : public Track {
public:

    AudioTrack() = default;

    explicit AudioTrack(const RteHandle &audio_track_handle) : Track(audio_track_handle) {
        c_audio_track.handle = audio_track_handle;
    }

    explicit AudioTrack(const RteAudioTrack &&audio_track) : Track(RteHandle(audio_track.handle)) {
        c_audio_track = audio_track;
    }

    virtual ~AudioTrack() = default;

    // 注意：RteAudioTrackGetInfo 在 C 接口中未定义，这里暂时注释掉
    // bool GetInfo(AudioTrackInfo* info, Error* err = nullptr) {
    //     return RteAudioTrackGetInfo(&c_audio_track, info != nullptr ? info->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    // }

    /**
     * @brief Register an observer for audio track events
     * @technical preview
     * @param observer Pointer to AudioTrackObserver
     * @param err Pointer to Error object (optional)
     * @return true if successful, false otherwise
     */
    bool RegisterTrackObserver(AudioTrackObserver* observer, Error* err = nullptr) {
        return RteAudioTrackRegisterTrackObserver(&c_audio_track, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Unregister an observer from audio track events
     * @technical preview
     * @param observer Pointer to AudioTrackObserver
     * @param err Pointer to Error object (optional)
     * @return true if successful, false otherwise
     */
    bool UnregisterTrackObserver(AudioTrackObserver* observer, Error* err = nullptr) {
        return RteAudioTrackUnregisterTrackObserver(&c_audio_track, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteAudioTrack
     */
    RteAudioTrack* get_underlying_impl() {
        return &c_audio_track;
    }

protected:
    RteAudioTrack c_audio_track = {0};
};

}  // namespace rte
