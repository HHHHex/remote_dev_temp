/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include "rte_base/c/track/track.h"
#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"

namespace rte {

using TrackMediaType = ::RteTrackMediaType;
using TrackSrc = ::RteTrackSrc;
using TrackState = ::RteTrackState;
using TrackStateChangedReason = ::RteTrackStateChangedReason;

class TrackConfig {
public:
    TrackConfig()
        : c_track_config_(new RteTrackConfig()), is_owner_(true) {
        RteTrackConfigInit(c_track_config_, nullptr);
    }

    explicit TrackConfig(RteTrackConfig* config)
        : c_track_config_(config), is_owner_(false) { 
    }
    
    virtual ~TrackConfig() {
        if(is_owner_ && c_track_config_ != nullptr){
            RteTrackConfigDeinit(c_track_config_, nullptr);
            delete c_track_config_;
            is_owner_ = false;
        }
        c_track_config_ = nullptr;
    }

    void SetJsonParameter(const std::string& json_parameter, Error* err = nullptr) {
        String str(json_parameter.c_str());
        // RteTrackConfigSetJsonParameter(&c_track_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    std::string GetJsonParameter(Error* err = nullptr) {
        String str;
        // RteTrackConfigGetJsonParameter(&c_track_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
        return std::string(str.CStr());
    }

    RteTrackConfig* get_underlying_impl() {
        return c_track_config_;
    }
    
private:
    RteTrackConfig* c_track_config_ = nullptr;
    bool is_owner_ = false;
};

class TrackInfo {
public:
    TrackInfo()
        : c_track_info_(new RteTrackInfo()), is_owner_(true) {
        RteTrackInfoInit(c_track_info_, nullptr);
    }

    explicit TrackInfo(RteTrackInfo* info)
        : c_track_info_(info), is_owner_(false) { 
    }
    
    virtual ~TrackInfo() { 
        if(is_owner_ && c_track_info_ != nullptr){
            RteTrackInfoDeinit(c_track_info_, nullptr);
            delete c_track_info_;
            is_owner_ = false;
        }
        c_track_info_ = nullptr;
    }

    RteTrackInfo* get_underlying_impl() {
        return c_track_info_;
    }

private:
    bool is_owner_ = false;
    RteTrackInfo* c_track_info_ = nullptr;
};

class TrackStats {
public:
    TrackStats() { RteTrackStatsInit(&c_stats_, nullptr); }
    ~TrackStats() { RteTrackStatsDeinit(&c_stats_, nullptr); }

    TrackSrc GetTrackSource() const { return c_stats_.track_source; }
    TrackMediaType GetTrackMediaType() const { return c_stats_.track_media_type; }

    RteTrackStats* get_underlying_impl() {
        return &c_stats_;
    }

private:
    RteTrackStats c_stats_;
};

// 静态回调函数声明
static void OnTrackStateChanged(RteTrackObserver *self, RteTrackState old_state, RteTrackState new_state, RteTrackStateChangedReason reason, RteError *err);

class TrackObserver {
public:
    TrackObserver() {
    }
    
    ~TrackObserver() { 
    }


    virtual void OnTrackStateChanged(TrackState old_state, TrackState new_state, TrackStateChangedReason reason, Error* err) {
    }

    RteTrackObserver* get_underlying_impl() {
        return c_observer;
    }

private:
    RteTrackObserver* c_observer;
};

// 静态回调函数实现
void OnTrackStateChanged(RteTrackObserver *self, RteTrackState old_state, RteTrackState new_state, RteTrackStateChangedReason reason, RteError *err) {
    if (self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
        TrackObserver* observer = static_cast<TrackObserver*>(self->base_observer.me_in_target_lang);
        Error cpp_err(err);
        observer->OnTrackStateChanged(static_cast<TrackState>(old_state), static_cast<TrackState>(new_state), static_cast<TrackStateChangedReason>(reason), &cpp_err);
    }
}

class Track {
  public:
    Track() = default;

    explicit Track(const RteHandle& track_handle){
        c_track.handle = track_handle;
    }

    explicit Track(RteTrack* track) {
        if (track != nullptr) {
            c_track.handle = track->handle;
        }
    }
    
    virtual ~Track() = default;

    void GetStats(std::function<void(TrackStats* stats, Error* err)> cb) {
        // CallbackContextWithArgs<Track, TrackStats*>* callbackCtx = new CallbackContextWithArgs<Track, TrackStats*>(this, cb);
        // RteTrackGetStats(&c_track, &CallbackFuncWithArgs<::RteTrack, Track, TrackStats*>, callbackCtx);
    }

    bool GetInfo(TrackInfo* info, Error* err = nullptr) {
        return RteTrackGetInfo(&c_track, info != nullptr ? info->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    bool RegisterObserver(TrackObserver* observer, Error* err = nullptr) {
        return RteTrackRegisterObserver(&c_track, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    bool UnregisterObserver(TrackObserver* observer, Error* err = nullptr) {
        return RteTrackUnregisterObserver(&c_track, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    RteTrack* get_underlying_impl() {
        return &c_track;
    }

  protected:
    RteTrack c_track = {0};
};

}  // namespace rte
