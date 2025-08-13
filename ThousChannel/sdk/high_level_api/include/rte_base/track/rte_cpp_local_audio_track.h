/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include "rte_base/c/track/local_audio_track.h"
#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"
#include "rte_base/track/rte_cpp_audio_track.h"


namespace rte {

using AudioFrameType = ::RteAudioFrameType;

/**
 * @brief Audio frame class for managing audio frame data
 * @technical preview
 */
class AudioFrame {
public:
    /**
     * @brief Constructor for AudioFrame
     * @technical preview
     * @param frame Pointer to RteAudioFrame
     */
    AudioFrame(const RteAudioFrame* frame) {
        if (frame != nullptr) {
            c_frame = *frame;
        }
    }

    /**
     * @brief Get audio frame type
     * @technical preview
     * @return Audio frame type
     */
    AudioFrameType GetType() const { return static_cast<AudioFrameType>(c_frame.type); }
    
    /**
     * @brief Get samples per channel
     * @technical preview
     * @return Number of samples per channel
     */
    int GetSamplesPerChannel() const { return c_frame.samples_per_channel; }
    
    /**
     * @brief Get bytes per sample
     * @technical preview
     * @return Number of bytes per sample
     */
    int GetBytesPerSample() const { return c_frame.bytes_per_sample; }
    
    /**
     * @brief Get number of channels
     * @technical preview
     * @return Number of audio channels
     */
    int GetChannels() const { return c_frame.channels; }
    
    /**
     * @brief Get samples per second
     * @technical preview
     * @return Sample rate in samples per second
     */
    int GetSamplesPerSec() const { return c_frame.samples_per_sec; }
    
    /**
     * @brief Get audio buffer pointer
     * @technical preview
     * @return Pointer to audio buffer
     */
    void* GetBuffer() const { return c_frame.buffer; }
    
    /**
     * @brief Get render time in milliseconds
     * @technical preview
     * @return Render time in milliseconds
     */
    int64_t GetRenderTimeInMs() const { return c_frame.render_time_in_ms; }
    
    /**
     * @brief Get AV sync type
     * @technical preview
     * @return AV sync type
     */
    int GetAvsyncType() const { return c_frame.avsync_type; }
    
    /**
     * @brief Get presentation time in milliseconds
     * @technical preview
     * @return Presentation time in milliseconds
     */
    int64_t GetPresentationInMs() const { return c_frame.presentation_in_ms; }
    
    /**
     * @brief Get audio track number
     * @technical preview
     * @return Audio track number
     */
    size_t GetAudioTrackNumber() const { return c_frame.audio_track_number; }

private:
    RteAudioFrame c_frame;
};

/**
 * @brief Configuration class for local audio track
 * @technical preview
 */
class LocalAudioTrackConfig : public AudioTrackConfig {
public:
    /**
     * @brief Default constructor for LocalAudioTrackConfig
     * @technical preview
     */
    LocalAudioTrackConfig()
        : AudioTrackConfig(reinterpret_cast<RteAudioTrackConfig*>(new RteLocalAudioTrackConfig())) {
        c_local_audio_track_config_ = reinterpret_cast<RteLocalAudioTrackConfig*>(AudioTrackConfig::get_underlying_impl());
        RteLocalAudioTrackConfigInit(c_local_audio_track_config_, nullptr);
        is_owner_ = true;
    }

    /**
     * @brief Constructor with existing RteLocalAudioTrackConfig
     * @technical preview
     * @param config Pointer to existing RteLocalAudioTrackConfig
     */
    explicit LocalAudioTrackConfig(RteLocalAudioTrackConfig* config)
        : AudioTrackConfig(reinterpret_cast<RteAudioTrackConfig*>(config)), c_local_audio_track_config_(config), is_owner_(false) { 
    }
    
    /**
     * @brief Destructor for LocalAudioTrackConfig
     * @technical preview
     */
    virtual ~LocalAudioTrackConfig() { 
        if(is_owner_ && c_local_audio_track_config_ != nullptr){
            RteLocalAudioTrackConfigDeinit(c_local_audio_track_config_, nullptr);
            delete c_local_audio_track_config_;
            is_owner_ = false;
        }
        c_local_audio_track_config_ = nullptr;
    }

    /**
     * @brief Set publish volume for local audio track
     * @technical preview
     * @param volume Volume level to set
     * @param err Pointer to Error object (optional)
     */
    void SetPublishVolume(uint32_t volume, Error* err = nullptr) {
        RteLocalAudioTrackConfigSetPublishVolume(c_local_audio_track_config_, volume, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get publish volume from local audio track configuration
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return Current publish volume level
     */
    uint32_t GetPublishVolume(Error* err = nullptr) {
        uint32_t volume;
        RteLocalAudioTrackConfigGetPublishVolume(c_local_audio_track_config_, &volume, err != nullptr ? err->get_underlying_impl() : nullptr);
        return volume;
    }

    /**
     * @brief Set loopback volume for local audio track
     * @technical preview
     * @param volume Volume level to set
     * @param err Pointer to Error object (optional)
     */
    void SetLoopbackVolume(uint32_t volume, Error* err = nullptr) {
        RteLocalAudioTrackConfigSetLoopbackVolume(c_local_audio_track_config_, volume, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get loopback volume from local audio track configuration
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return Current loopback volume level
     */
    uint32_t GetLoopbackVolume(Error* err = nullptr) {
        uint32_t volume;
        RteLocalAudioTrackConfigGetLoopbackVolume(c_local_audio_track_config_, &volume, err != nullptr ? err->get_underlying_impl() : nullptr);
        return volume;
    }

    /**
     * @brief Enable or disable loopback filter
     * @technical preview
     * @param enable_loopback_filter true to enable, false to disable
     * @param err Pointer to Error object (optional)
     */
    void SetEnableLoopbackFilter(bool enable_loopback_filter, Error* err = nullptr) {
        RteLocalAudioTrackConfigSetEnableLoopbackFilter(c_local_audio_track_config_, enable_loopback_filter, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get loopback filter setting
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return true if loopback filter is enabled, false otherwise
     */
    bool GetEnableLoopbackFilter(Error* err = nullptr) {
        bool enable_loopback_filter;
        RteLocalAudioTrackConfigGetEnableLoopbackFilter(c_local_audio_track_config_, &enable_loopback_filter, err != nullptr ? err->get_underlying_impl() : nullptr);
        return enable_loopback_filter;
    }

    /**
     * @brief Enable or disable publish filter
     * @technical preview
     * @param enable_publish_filter true to enable, false to disable
     * @param err Pointer to Error object (optional)
     */
    void SetEnablePublishFilter(bool enable_publish_filter, Error* err = nullptr) {
        RteLocalAudioTrackConfigSetEnablePublishFilter(c_local_audio_track_config_, enable_publish_filter, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get publish filter setting
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return true if publish filter is enabled, false otherwise
     */
    bool GetEnablePublishFilter(Error* err = nullptr) {
        bool enable_publish_filter;
        RteLocalAudioTrackConfigGetEnablePublishFilter(c_local_audio_track_config_, &enable_publish_filter, err != nullptr ? err->get_underlying_impl() : nullptr);
        return enable_publish_filter;
    }

    /**
     * @brief Set JSON parameter for local audio track configuration
     * @technical preview
     * @param json_parameter JSON string parameter
     * @param err Pointer to Error object (optional)
     */
    void SetJsonParameter(const std::string& json_parameter, Error* err = nullptr) {
        String str(json_parameter.c_str());
        RteLocalAudioTrackConfigSetJsonParameter(c_local_audio_track_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get JSON parameter from local audio track configuration
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return JSON string parameter
     */
    std::string GetJsonParameter(Error* err = nullptr) {
        String str;
        RteLocalAudioTrackConfigGetJsonParameter(c_local_audio_track_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
        return std::string(str.CStr());
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteLocalAudioTrackConfig
     */
    RteLocalAudioTrackConfig* get_underlying_impl() {
        return c_local_audio_track_config_;
    }

private:
    RteLocalAudioTrackConfig* c_local_audio_track_config_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Information class for local audio track
 * @technical preview
 */
class LocalAudioTrackInfo : public AudioTrackInfo {
public:
    /**
     * @brief Default constructor for LocalAudioTrackInfo
     * @technical preview
     */
    LocalAudioTrackInfo() : AudioTrackInfo(reinterpret_cast<RteAudioTrackInfo*>(new RteLocalAudioTrackInfo())){
        c_local_audio_track_info_ = reinterpret_cast<RteLocalAudioTrackInfo*>(AudioTrackInfo::get_underlying_impl());
        RteLocalAudioTrackInfoInit(c_local_audio_track_info_, nullptr);
        is_owner_ = true;
    }

    /**
     * @brief Constructor with existing RteLocalAudioTrackInfo
     * @technical preview
     * @param info Pointer to existing RteLocalAudioTrackInfo
     */
    explicit LocalAudioTrackInfo(RteLocalAudioTrackInfo* info)
        : AudioTrackInfo(reinterpret_cast<RteAudioTrackInfo*>(info)), c_local_audio_track_info_(info), is_owner_(false) { 
    }
    
    /**
     * @brief Destructor for LocalAudioTrackInfo
     * @technical preview
     */
    virtual ~LocalAudioTrackInfo() { 
        if(is_owner_ && c_local_audio_track_info_ != nullptr){
            RteLocalAudioTrackInfoDeinit(c_local_audio_track_info_, nullptr);
            delete c_local_audio_track_info_;
            is_owner_ = false;
        }
        c_local_audio_track_info_ = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteLocalAudioTrackInfo
     */
    RteLocalAudioTrackInfo* get_underlying_impl() {
        return c_local_audio_track_info_;
    }

private:
    RteLocalAudioTrackInfo* c_local_audio_track_info_ = nullptr;
    bool is_owner_ = false;
};

// Static callback function declaration
static void OnLocalAudioTrackFrame(RteLocalAudioTrackObserver *self, RteAudioFrame *audio_frame);

/**
 * @brief Observer class for local audio track events
 * @technical preview
 */
class LocalAudioTrackObserver : public AudioTrackObserver {
public:
    /**
     * @brief Default constructor for LocalAudioTrackObserver
     * @technical preview
     */
    LocalAudioTrackObserver() {
        c_observer = RteLocalAudioTrackObserverCreate(nullptr);
        if (c_observer != nullptr) {
            // Note: RteLocalTrackObserver is just a placeholder in the C interface and does not have a base_observer member
            // Temporarily commented out here, will be enabled after the C interface is improved
            // c_observer->local_track_observer.base_observer.me_in_target_lang = this;
            c_observer->on_frame = rte::OnLocalAudioTrackFrame;
        }
    }
    
    /**
     * @brief Destructor for LocalAudioTrackObserver
     * @technical preview
     */
    ~LocalAudioTrackObserver() { 
        RteLocalAudioTrackObserverDestroy(c_observer, nullptr); 
    }

    /**
     * @brief Called when audio frame is received
     * @technical preview
     * @param audio_frame The audio frame data
     */
    virtual void OnFrame(const AudioFrame& audio_frame) {
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteLocalAudioTrackObserver
     */
    RteLocalAudioTrackObserver* get_underlying_impl() {
        return c_observer;
    }

private:
    RteLocalAudioTrackObserver* c_observer;
};

// Static callback function implementation
void OnLocalAudioTrackFrame(RteLocalAudioTrackObserver *self, RteAudioFrame *audio_frame) {
    if (self != nullptr) {
        // Note: Since RteLocalTrackObserver is just a placeholder, it is temporarily impossible to obtain the C++ object pointer here
        // The complete callback mechanism will be implemented after the C interface is improved
        // LocalAudioTrackObserver* observer = static_cast<LocalAudioTrackObserver*>(self->local_track_observer.base_observer.me_in_target_lang);
        // AudioFrame frame(audio_frame);
        // observer->OnFrame(frame);
    }
}

/**
 * @brief Local audio track class for managing local audio streams
 * @technical preview
 */
class LocalAudioTrack : public AudioTrack {
public:
    LocalAudioTrack() = default;

    explicit LocalAudioTrack(const RteHandle& local_audio_track_handle) : AudioTrack(local_audio_track_handle) {
        c_local_audio_track.handle = local_audio_track_handle;
    }

    explicit LocalAudioTrack(const RteLocalAudioTrack &&local_audio_track) : AudioTrack(RteHandle(local_audio_track.handle)) {
        c_local_audio_track = local_audio_track;
    }

    virtual ~LocalAudioTrack() = default;

    /**
     * @brief Start the local audio track
     * @technical preview
     * @param cb Callback function called when operation completes
     */
    void Start(std::function<void(Error* err)> cb) {
        CallbackContext<LocalAudioTrack>* callbackCtx = new CallbackContext<LocalAudioTrack>(this, cb);
        RteLocalAudioTrackStart(&c_local_audio_track, &CallbackFunc<::RteLocalAudioTrack, LocalAudioTrack>, callbackCtx);
    }

    /**
     * @brief Stop the local audio track
     * @technical preview
     * @param cb Callback function called when operation completes
     */
    void Stop(std::function<void(Error* err)> cb) {
        CallbackContext<LocalAudioTrack>* callbackCtx = new CallbackContext<LocalAudioTrack>(this, cb);
        RteLocalAudioTrackStop(&c_local_audio_track, &CallbackFunc<::RteLocalAudioTrack, LocalAudioTrack>, callbackCtx);
    }

    /**
     * @brief Get local audio track configurations
     * @technical preview
     * @param config Pointer to LocalAudioTrackConfig to store the configurations
     * @param err Pointer to Error object (optional)
     * @return true if successful, false otherwise
     */
    bool GetConfigs(LocalAudioTrackConfig* config, Error* err = nullptr) {
        return RteLocalAudioTrackGetConfigs(&c_local_audio_track, config != nullptr ? config->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Set local audio track configurations
     * @technical preview
     * @param config Pointer to LocalAudioTrackConfig containing the configurations
     * @param err Pointer to Error object (optional)
     * @return true if successful, false otherwise
     */
    bool SetConfigs(LocalAudioTrackConfig* config, Error* err = nullptr) {
        return RteLocalAudioTrackSetConfigs(&c_local_audio_track, config != nullptr ? config->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Enable loopback for local audio track
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return true if successful, false otherwise
     */
    bool EnableLoopback(Error* err = nullptr) {
        return RteLocalAudioTrackEnableLoopback(&c_local_audio_track, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Register an observer for local audio track events
     * @technical preview
     * @param observer Pointer to LocalAudioTrackObserver
     * @param err Pointer to Error object (optional)
     * @return true if successful, false otherwise
     */
    bool RegisterTrackObserver(LocalAudioTrackObserver* observer, Error* err = nullptr) {
        return RteLocalAudioTrackRegisterTrackObserver(&c_local_audio_track, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Unregister an observer from local audio track events
     * @technical preview
     * @param observer Pointer to LocalAudioTrackObserver
     * @param err Pointer to Error object (optional)
     * @return true if successful, false otherwise
     */
    bool UnregisterTrackObserver(LocalAudioTrackObserver* observer, Error* err = nullptr) {
        return RteLocalAudioTrackUnregisterTrackObserver(&c_local_audio_track, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

  protected:
    RteLocalAudioTrack c_local_audio_track = {0};
};

}  // namespace rte
