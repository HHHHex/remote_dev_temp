/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include "rte_base/c/track/mic_audio_track.h"
#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"
#include "rte_base/track/rte_cpp_local_audio_track.h"

namespace rte {

using VoiceBeautifierPreset = ::RteVoiceBeautifierPreset;
using AudioEffectPreset = ::RteAudioEffectPreset;
using VoiceConversionPreset = ::RteVoiceConversionPreset;

class MicAudioTrackConfig: public LocalAudioTrackConfig {
public:
    MicAudioTrackConfig()
        : LocalAudioTrackConfig(reinterpret_cast<RteLocalAudioTrackConfig*>(new RteMicAudioTrackConfig())) {
        c_mic_audio_track_config_ = reinterpret_cast<RteMicAudioTrackConfig*>(LocalAudioTrackConfig::get_underlying_impl());
        RteMicAudioTrackConfigInit(c_mic_audio_track_config_, nullptr);
        is_owner_ = true;
    }

    explicit MicAudioTrackConfig(RteMicAudioTrackConfig* config)
        : LocalAudioTrackConfig(reinterpret_cast<RteLocalAudioTrackConfig*>(config)), c_mic_audio_track_config_(config), is_owner_(false) {
    }

    virtual ~MicAudioTrackConfig() {
        if(is_owner_ && c_mic_audio_track_config_ != nullptr){
            RteMicAudioTrackConfigDeinit(c_mic_audio_track_config_, nullptr); 
            delete c_mic_audio_track_config_;
            is_owner_ = false;
        }
        c_mic_audio_track_config_ = nullptr;
    }

    void SetRecordingVolume(uint32_t volume, Error* err = nullptr) {
        RteMicAudioTrackConfigSetRecordingVolume(c_mic_audio_track_config_, volume, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    uint32_t GetRecordingVolume(Error* err = nullptr) {
        uint32_t volume;
        RteMicAudioTrackConfigGetRecordingVolume(c_mic_audio_track_config_, &volume, err != nullptr ? err->get_underlying_impl() : nullptr);
        return volume;
    }

    void SetJsonParameter(const std::string& json_parameter, Error* err = nullptr) {
        String str(json_parameter.c_str());
        RteMicAudioTrackConfigSetJsonParameter(c_mic_audio_track_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    std::string GetJsonParameter(Error* err = nullptr) {
        String str;
        RteMicAudioTrackConfigGetJsonParameter(c_mic_audio_track_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
        return std::string(str.CStr());
    }

    RteMicAudioTrackConfig* get_underlying_impl() {
        return c_mic_audio_track_config_;
    }

private:    
    RteMicAudioTrackConfig* c_mic_audio_track_config_ = nullptr;
    bool is_owner_ = false;
};

class MicAudioTrackInfo: public LocalAudioTrackInfo {
public:
    MicAudioTrackInfo() : LocalAudioTrackInfo(reinterpret_cast<RteLocalAudioTrackInfo*>(new RteMicAudioTrackInfo())){
        c_mic_audio_track_info_ = reinterpret_cast<RteMicAudioTrackInfo*>(LocalAudioTrackInfo::get_underlying_impl());
        RteMicAudioTrackInfoInit(c_mic_audio_track_info_, nullptr);
        is_owner_ = true;
    }
    
    explicit MicAudioTrackInfo(RteMicAudioTrackInfo* info)
        : LocalAudioTrackInfo(reinterpret_cast<RteLocalAudioTrackInfo*>(info)), c_mic_audio_track_info_(info), is_owner_(false) {
    }

    virtual ~MicAudioTrackInfo() { 
        if(is_owner_ && c_mic_audio_track_info_ != nullptr){
            RteMicAudioTrackInfoDeinit(c_mic_audio_track_info_, nullptr);
            delete c_mic_audio_track_info_;
            is_owner_ = false;
        }
        c_mic_audio_track_info_ = nullptr;
    }

    RteMicAudioTrackInfo* get_underlying_impl() {
        return c_mic_audio_track_info_;
    }

private:
    RteMicAudioTrackInfo* c_mic_audio_track_info_ = nullptr;
    bool is_owner_ = false;
};

class MicAudioTrack: public LocalAudioTrack {
public:
    MicAudioTrack(Rte* rte, MicAudioTrackConfig* config = nullptr) 
        : c_mic_audio_track(RteMicAudioTrackCreate(&rte->c_rte, config != nullptr ? config->get_underlying_impl() : nullptr, nullptr)), is_owner_(true) {
        c_local_audio_track.handle = c_mic_audio_track.handle;
        c_audio_track.handle = c_mic_audio_track.handle;
        c_track.handle = c_mic_audio_track.handle;
    }

    explicit MicAudioTrack(const RteHandle& mic_audio_track_handle) : LocalAudioTrack(mic_audio_track_handle) {
        c_mic_audio_track.handle = mic_audio_track_handle;
    }

    explicit MicAudioTrack(const RteMicAudioTrack &&mic_audio_track) : LocalAudioTrack(RteHandle(mic_audio_track.handle)) {
        c_mic_audio_track = mic_audio_track;
    }
    
    virtual ~MicAudioTrack() { 
        if(is_owner_){
            RteMicAudioTrackDestroy(&c_mic_audio_track, nullptr);
            is_owner_ = false;
        }
    }

    void GetConfigs(MicAudioTrackConfig* config, Error* err = nullptr) {
        RteMicAudioTrackGetConfigs(&c_mic_audio_track, config != nullptr ? config->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    void SetConfigs(MicAudioTrackConfig* config, Error* err = nullptr) {
        RteMicAudioTrackSetConfigs(&c_mic_audio_track, config != nullptr ? config->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    void SetVoiceBeautifierPreset(VoiceBeautifierPreset preset, std::function<void(Error* err)> cb) {
        CallbackContextWithArgs<MicAudioTrack>* callbackCtx = new CallbackContextWithArgs<MicAudioTrack>(this, cb);
        RteMicAudioTrackSetVoiceBeautifierPreset(&c_mic_audio_track, preset, &CallbackFuncWithArgs<::RteMicAudioTrack, MicAudioTrack>, callbackCtx);
    }

    void SetAudioEffectPreset(AudioEffectPreset preset, std::function<void(Error* err)> cb) {
        CallbackContextWithArgs<MicAudioTrack>* callbackCtx = new CallbackContextWithArgs<MicAudioTrack>(this, cb);
        RteMicAudioTrackSetAudioEffectPreset(&c_mic_audio_track, preset, &CallbackFuncWithArgs<::RteMicAudioTrack, MicAudioTrack>, callbackCtx);  
    }

    void SetVoiceConversionPreset(VoiceConversionPreset preset, std::function<void(Error* err)> cb) {
        CallbackContextWithArgs<MicAudioTrack>* callbackCtx = new CallbackContextWithArgs<MicAudioTrack>(this, cb);
        RteMicAudioTrackSetVoiceConversionPreset(&c_mic_audio_track, preset, &CallbackFuncWithArgs<::RteMicAudioTrack, MicAudioTrack>, callbackCtx);
    }

    void GetInfo(MicAudioTrackInfo* info, Error* err = nullptr) {
        RteMicAudioTrackGetInfo(&c_mic_audio_track, info != nullptr ? info->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

private:
    RteMicAudioTrack c_mic_audio_track = {0};
    bool is_owner_ = false;
};

}  // namespace rte
