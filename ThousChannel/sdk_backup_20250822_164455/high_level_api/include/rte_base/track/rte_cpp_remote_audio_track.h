/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include "rte_base/c/track/remote_audio_track.h"
#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"
#include "rte_base/track/rte_cpp_audio_track.h"

namespace rte {

class RemoteAudioTrackConfig : public AudioTrackConfig {
public:
    RemoteAudioTrackConfig()
        : AudioTrackConfig(reinterpret_cast<RteAudioTrackConfig*>(new RteRemoteAudioTrackConfig())) {
        c_remote_audio_track_config_ = reinterpret_cast<RteRemoteAudioTrackConfig*>(AudioTrackConfig::get_underlying_impl());
        RteRemoteAudioTrackConfigInit(c_remote_audio_track_config_, nullptr);
        is_owner_ = true;
    }

    explicit RemoteAudioTrackConfig(RteRemoteAudioTrackConfig* config)
        : AudioTrackConfig(reinterpret_cast<RteAudioTrackConfig*>(config)), c_remote_audio_track_config_(config), is_owner_(false) { 
    }
    
    virtual ~RemoteAudioTrackConfig() { 
        if(is_owner_ && c_remote_audio_track_config_ != nullptr){
            RteRemoteAudioTrackConfigDeinit(c_remote_audio_track_config_, nullptr);
            delete c_remote_audio_track_config_;
            is_owner_ = false;
        }
        c_remote_audio_track_config_ = nullptr;
    }

    void SetPlaybackVolume(uint32_t volume, Error* err = nullptr) {
        RteRemoteAudioTrackConfigSetPlaybackVolume(c_remote_audio_track_config_, volume, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    uint32_t GetPlaybackVolume(Error* err = nullptr) {
        uint32_t volume;
        RteRemoteAudioTrackConfigGetPlaybackVolume(c_remote_audio_track_config_, &volume, err != nullptr ? err->get_underlying_impl() : nullptr);
        return volume;
    }

    void SetJsonParameter(const std::string& json_parameter, Error* err = nullptr) {
        String str(json_parameter.c_str());
        RteRemoteAudioTrackConfigSetJsonParameter(c_remote_audio_track_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    std::string GetJsonParameter(Error* err = nullptr) {
        String str;
        RteRemoteAudioTrackConfigGetJsonParameter(c_remote_audio_track_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
        return std::string(str.CStr());
    }

    RteRemoteAudioTrackConfig* get_underlying_impl() {
        return c_remote_audio_track_config_;
    }

private:
    RteRemoteAudioTrackConfig* c_remote_audio_track_config_ = nullptr;
    bool is_owner_ = false;
};

class RemoteAudioTrackInfo : public AudioTrackInfo {
public:
    RemoteAudioTrackInfo() : AudioTrackInfo(reinterpret_cast<RteAudioTrackInfo*>(new RteRemoteAudioTrackInfo())){
        c_remote_audio_track_info_ = reinterpret_cast<RteRemoteAudioTrackInfo*>(AudioTrackInfo::get_underlying_impl());
        RteRemoteAudioTrackInfoInit(c_remote_audio_track_info_, nullptr);
        is_owner_ = true;
    }

    explicit RemoteAudioTrackInfo(RteRemoteAudioTrackInfo* info)
        : AudioTrackInfo(reinterpret_cast<RteAudioTrackInfo*>(info)), c_remote_audio_track_info_(info), is_owner_(false) { 
    }
    
    virtual ~RemoteAudioTrackInfo() { 
        if(is_owner_ && c_remote_audio_track_info_ != nullptr){
            RteRemoteAudioTrackInfoDeinit(c_remote_audio_track_info_, nullptr);
            delete c_remote_audio_track_info_;
            is_owner_ = false;
        }
        c_remote_audio_track_info_ = nullptr;
    }

    RteRemoteAudioTrackInfo* get_underlying_impl() {
        return c_remote_audio_track_info_;
    }

private:
    RteRemoteAudioTrackInfo* c_remote_audio_track_info_ = nullptr;
    bool is_owner_ = false;
};

class RemoteAudioTrack : public AudioTrack {
public:
    RemoteAudioTrack() = default;

    explicit RemoteAudioTrack(const RteHandle& remote_audio_track_handle) : AudioTrack(remote_audio_track_handle) {
        c_remote_audio_track.handle = remote_audio_track_handle;
    }

    explicit RemoteAudioTrack(const RteRemoteAudioTrack &&remote_audio_track) : AudioTrack(RteHandle(remote_audio_track.handle)) {
        c_remote_audio_track = remote_audio_track;
    }
    
    virtual ~RemoteAudioTrack() = default;

    bool GetConfigs(RemoteAudioTrackConfig* config, Error* err = nullptr) {
        return RteRemoteAudioTrackGetConfigs(&c_remote_audio_track, config != nullptr ? config->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    bool SetConfigs(RemoteAudioTrackConfig* config, Error* err = nullptr) {
        return RteRemoteAudioTrackSetConfigs(&c_remote_audio_track, config != nullptr ? config->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    bool EnableLoopback(Error* err = nullptr) {
        return RteRemoteAudioTrackEnableLoopback(&c_remote_audio_track, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    // bool RegisterTrackObserver(RemoteAudioTrackObserver* observer, Error* err = nullptr) {
    //     return RteRemoteAudioTrackRegisterTrackObserver(&c_remote_audio_track, observer != nullptr ? observer->get_underlying_impl() : nullptr, nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    // }

    // bool UnregisterTrackObserver(RemoteAudioTrackObserver* observer, Error* err = nullptr) {
    //     return RteRemoteAudioTrackUnregisterTrackObserver(&c_remote_audio_track, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    // }

private:
    RteRemoteAudioTrack c_remote_audio_track = {0};
};

}  // namespace rte
