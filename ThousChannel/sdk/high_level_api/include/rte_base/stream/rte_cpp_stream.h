/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include <functional>
#include "rte_base/c/stream/stream.h"
#include "rte_base/c/handle.h"
#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"
#include "rte_base/track/rte_cpp_track.h"
#include "rte_base/track/rte_cpp_audio_track.h"
#include "rte_base/track/rte_cpp_video_track.h"

namespace rte {

using StreamType = ::RteStreamType;
using EncryptionMode = ::RteEncryptionMode;
using AudioEncoderProfile = ::RteAudioEncoderProfile;
using OrientationMode = ::RteOrientationMode;
using VideoDegradationPreference = ::RteVideoDegradationPreference;
using VideoMirrorMode = ::RteVideoMirrorMode;

class PresenceState;

/**
 * @brief Configuration class for stream settings
 * @technical preview
 */
class StreamConfig {
public:
    /**
     * @brief Default constructor for StreamConfig
     * @technical preview
     */
    StreamConfig() : c_config_(new RteStreamConfig()), is_owner_(true) {
        RteStreamConfigInit(c_config_, nullptr);
    }

    explicit StreamConfig(RteStreamConfig* config)
        : c_config_(config), is_owner_(false) {
    }

    /**
     * @brief Virtual destructor for StreamConfig
     * @technical preview
     */
    virtual ~StreamConfig() {
        if (is_owner_ && c_config_ != nullptr) {
            RteStreamConfigDeinit(c_config_, nullptr);
            delete c_config_;
            is_owner_ = false;
        }
        c_config_ = nullptr;
    }

    /**
     * @brief Set the stream type
     * @technical preview
     * @param type The stream type to set
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetStreamType(StreamType type, Error* err = nullptr) {
        RteStreamConfigSetStreamType(c_config_, type, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the stream type
     * @technical preview
     * @param err Pointer to Error to receive error information (optional)
     * @return The current stream type
     */
    StreamType GetStreamType(Error* err = nullptr) {
        StreamType type;
        RteStreamConfigGetStreamType(c_config_, &type, err != nullptr ? err->get_underlying_impl() : nullptr);
        return type;
    }

    /**
     * @brief Set the stream ID
     * @technical preview
     * @param stream_id The stream ID to set
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetStreamId(const std::string& stream_id, Error* err = nullptr) {
        String str(stream_id.c_str());
        RteStreamConfigSetStreamId(c_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the stream ID
     * @technical preview
     * @param err Pointer to Error to receive error information (optional)
     * @return The current stream ID
     */
    std::string GetStreamId(Error* err = nullptr) {
        String str;
        RteStreamConfigGetStreamId(c_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
        return std::string(str.CStr());
    }

    /**
     * @brief Set the encryption mode
     * @technical preview
     * @param mode The encryption mode to set
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetEncryptionMode(EncryptionMode mode, Error* err = nullptr) {
        RteStreamConfigSetEncryptionMode(c_config_, mode, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the encryption mode
     * @technical preview
     * @param err Pointer to Error to receive error information (optional)
     * @return The current encryption mode
     */
    EncryptionMode GetEncryptionMode(Error* err = nullptr) {
        EncryptionMode mode;
        RteStreamConfigGetEncryptionMode(c_config_, &mode, err != nullptr ? err->get_underlying_impl() : nullptr);
        return mode;
    }

    /**
     * @brief Set the encryption key
     * @technical preview
     * @param key The encryption key to set
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetEncryptionKey(const std::string& key, Error* err = nullptr) {
        String str(key.c_str());
        RteStreamConfigSetEncryptionKey(c_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the encryption key
     * @technical preview
     * @param err Pointer to Error to receive error information (optional)
     * @return The current encryption key
     */
    std::string GetEncryptionKey(Error* err = nullptr) {
        String str;
        RteStreamConfigGetEncryptionKey(c_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
        return std::string(str.CStr());
    }

    /**
     * @brief Set the encryption KDF salt
     * @technical preview
     * @param salt The encryption KDF salt to set
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetEncryptionKdfSalt(const uint8_t* salt, Error* err = nullptr) {
        RteStreamConfigSetEncryptionKdfSalt(c_config_, const_cast<uint8_t*>(salt), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the encryption KDF salt
     * @technical preview
     * @param salt Pointer to receive the encryption KDF salt
     * @param err Pointer to Error to receive error information (optional)
     */
    void GetEncryptionKdfSalt(uint8_t* salt, Error* err = nullptr) {
        RteStreamConfigGetEncryptionKdfSalt(c_config_, salt, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Set the audio encoder profile
     * @technical preview
     * @param profile The audio encoder profile to set
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetAudioEncoderProfile(AudioEncoderProfile profile, Error* err = nullptr) {
        RteStreamConfigSetAudioEncoderProfile(c_config_, profile, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the audio encoder profile
     * @technical preview
     * @param err Pointer to Error to receive error information (optional)
     * @return The current audio encoder profile
     */
    AudioEncoderProfile GetAudioEncoderProfile(Error* err = nullptr) {
        AudioEncoderProfile profile;
        RteStreamConfigGetAudioEncoderProfile(c_config_, &profile, err != nullptr ? err->get_underlying_impl() : nullptr);
        return profile;
    }

    /**
     * @brief Set the video width
     * @technical preview
     * @param width The video width to set
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetWidth(uint32_t width, Error* err = nullptr) {
        RteStreamConfigSetWidth(c_config_, width, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the video width
     * @technical preview
     * @param err Pointer to Error to receive error information (optional)
     * @return The current video width
     */
    uint32_t GetWidth(Error* err = nullptr) {
        uint32_t width;
        RteStreamConfigGetWidth(c_config_, &width, err != nullptr ? err->get_underlying_impl() : nullptr);
        return width;
    }

    /**
     * @brief Set the video height
     * @technical preview
     * @param height The video height to set
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetHeight(uint32_t height, Error* err = nullptr) {
        RteStreamConfigSetHeight(c_config_, height, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the video height
     * @technical preview
     * @param err Pointer to Error to receive error information (optional)
     * @return The current video height
     */
    uint32_t GetHeight(Error* err = nullptr) {
        uint32_t height;
        RteStreamConfigGetHeight(c_config_, &height, err != nullptr ? err->get_underlying_impl() : nullptr);
        return height;
    }

    /**
     * @brief Set the frame rate
     * @technical preview
     * @param frame_rate The frame rate to set
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetFrameRate(uint32_t frame_rate, Error* err = nullptr) {
        RteStreamConfigSetFrameRate(c_config_, frame_rate, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the frame rate
     * @technical preview
     * @param err Pointer to Error to receive error information (optional)
     * @return The current frame rate
     */
    uint32_t GetFrameRate(Error* err = nullptr) {
        uint32_t frame_rate;
        RteStreamConfigGetFrameRate(c_config_, &frame_rate, err != nullptr ? err->get_underlying_impl() : nullptr);
        return frame_rate;
    }

    /**
     * @brief Set the minimum bit rate
     * @technical preview
     * @param min_bitrate The minimum bit rate to set
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetMinBitRate(uint32_t min_bitrate, Error* err = nullptr) {
        RteStreamConfigSetMinBitRate(c_config_, min_bitrate, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the minimum bit rate
     * @technical preview
     * @param err Pointer to Error to receive error information (optional)
     * @return The current minimum bit rate
     */
    uint32_t GetMinBitRate(Error* err = nullptr) {
        uint32_t min_bitrate;
        RteStreamConfigGetMinBitRate(c_config_, &min_bitrate, err != nullptr ? err->get_underlying_impl() : nullptr);
        return min_bitrate;
    }

    /**
     * @brief Set the orientation mode
     * @technical preview
     * @param mode The orientation mode to set
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetOrientationMode(OrientationMode mode, Error* err = nullptr) {
        RteStreamConfigSetOrientationMode(c_config_, mode, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the orientation mode
     * @technical preview
     * @param err Pointer to Error to receive error information (optional)
     * @return The current orientation mode
     */
    OrientationMode GetOrientationMode(Error* err = nullptr) {
        OrientationMode mode;
        RteStreamConfigGetOrientationMode(c_config_, &mode, err != nullptr ? err->get_underlying_impl() : nullptr);
        return mode;
    }

    /**
     * @brief Set the video degradation preference
     * @technical preview
     * @param preference The video degradation preference to set
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetVideoDegradationPreference(VideoDegradationPreference preference, Error* err = nullptr) {
        RteStreamConfigSetVideoDegradationPreference(c_config_, preference, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the video degradation preference
     * @technical preview
     * @param err Pointer to Error to receive error information (optional)
     * @return The current video degradation preference
     */
    VideoDegradationPreference GetVideoDegradationPreference(Error* err = nullptr) {
        VideoDegradationPreference preference;
        RteStreamConfigGetVideoDegradationPreference(c_config_, &preference, err != nullptr ? err->get_underlying_impl() : nullptr);
        return preference;
    }

    /**
     * @brief Set the video mirror mode
     * @technical preview
     * @param mode The video mirror mode to set
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetVideoMirrorMode(VideoMirrorMode mode, Error* err = nullptr) {
        RteStreamConfigSetVideoMirrorMode(c_config_, mode, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the video mirror mode
     * @technical preview
     * @param err Pointer to Error to receive error information (optional)
     * @return The current video mirror mode
     */
    VideoMirrorMode GetVideoMirrorMode(Error* err = nullptr) {
        VideoMirrorMode mode;
        RteStreamConfigGetVideoMirrorMode(c_config_, &mode, err != nullptr ? err->get_underlying_impl() : nullptr);
        return mode;
    }

    /**
     * @brief Set the CHE hardware decoding flag
     * @technical preview
     * @param enable Whether to enable CHE hardware decoding
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetCheHwDecoding(bool enable, Error* err = nullptr) {
        RteStreamConfigSetCheHwDecoding(c_config_, enable, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the CHE hardware decoding flag
     * @technical preview
     * @param err Pointer to Error to receive error information (optional)
     * @return Whether CHE hardware decoding is enabled
     */
    bool GetCheHwDecoding(Error* err = nullptr) {
        bool enable;
        RteStreamConfigGetCheHwDecoding(c_config_, &enable, err != nullptr ? err->get_underlying_impl() : nullptr);
        return enable;
    }

    /**
     * @brief Set the JSON parameter
     * @technical preview
     * @param json_parameter The JSON parameter to set
     * @param err Pointer to Error to receive error information (optional)
     */
    void SetJsonParameter(const std::string& json_parameter, Error* err = nullptr) {
        String str(json_parameter.c_str());
        RteStreamConfigSetJsonParameter(c_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the JSON parameter
     * @technical preview
     * @param err Pointer to Error to receive error information (optional)
     * @return The current JSON parameter
     */
    std::string GetJsonParameter(Error* err = nullptr) {
        String str;
        RteStreamConfigGetJsonParameter(c_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
        return std::string(str.CStr());
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteStreamConfig
     */
    RteStreamConfig* get_underlying_impl() {
        return c_config_;
    }

private:
    RteStreamConfig* c_config_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Information class for stream details
 * @technical preview
 */
class StreamInfo {
public:
    /**
     * @brief Default constructor for StreamInfo
     * @technical preview
     */
    StreamInfo() : c_stream_info(new RteStreamInfo()), is_owner_(true) {
        RteStreamInfoInit(c_stream_info, nullptr);
    }

    explicit StreamInfo(RteStreamInfo* info)
        : c_stream_info(info), is_owner_(false) {
    }

    StreamInfo& operator=(const StreamInfo& other) {
        if (this != &other) {
            RteStreamInfoCopy(c_stream_info, other.c_stream_info, nullptr);
        }
        return *this;
    }

    /**
     * @brief Destructor for StreamInfo
     * @technical preview
     */
    ~StreamInfo() {
        if (is_owner_ && c_stream_info != nullptr) {
            RteStreamInfoDeinit(c_stream_info, nullptr);
            delete c_stream_info;
            is_owner_ = false;
        }
        c_stream_info = nullptr;
    }

    // Channel GetChannel() const { return Channel(&c_info.channel); }
    // Rte GetRte() const { return Rte(&c_info.rte); }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteStreamInfo
     */
    RteStreamInfo* get_underlying_impl(){
        return c_stream_info;
    };

private:
    RteStreamInfo* c_stream_info = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Observer class for stream events
 * @technical preview
 */
class StreamObserver {
public:
    /**
     * @brief Default constructor for StreamObserver
     * @technical preview
     */
    StreamObserver() : c_observer(::RteStreamObserverCreate(nullptr)) {
        c_observer->base_observer.me_in_target_lang = this;
    }

    /**
     * @brief Destructor for StreamObserver
     * @technical preview
     */
    ~StreamObserver() { RteStreamObserverDestroy(c_observer, nullptr); }

    friend class Stream;

private:
    ::RteStreamObserver* c_observer;
};

/**
 * The Stream class is used to manage the stream.
 * @since v4.4.0
 * @technical preview
 */
class Stream {

 public:
  Stream() = default;

  explicit Stream(const RteHandle& stream_handle) {
    c_stream.handle = stream_handle;
  }

  explicit Stream(const RteStream& stream) : Stream(RteHandle(stream.handle)) {
    c_stream = stream;
  }

  virtual ~Stream() = default;

  /**
   * @brief Get the information of the stream.
   * @note technical preview
   * @param info The pointer to StreamInfo to receive the stream information.
   * @param err The pointer to Error to receive error information. Optional.
   */
  void GetInfo(StreamInfo* info, Error* err = nullptr) {
    RteStreamGetInfo(&c_stream, info != nullptr ? info->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Get the configuration of the stream.
   * @note technical preview
   * @param config The pointer to StreamConfig to receive the stream configuration.
   * @param err The pointer to Error to receive error information. Optional.
   * @return true if successful, false otherwise.
   */
  bool GetConfigs(StreamConfig* config, Error* err = nullptr) {
    return RteStreamGetConfigs(&c_stream, config != nullptr ? config->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Set the configuration of the stream.
   * @note technical preview
   * @param config The pointer to StreamConfig containing the configuration to set.
   * @param err The pointer to Error to receive error information. Optional.
   * @return true if successful, false otherwise.
   */
  bool SetConfigs(StreamConfig* config, Error* err = nullptr) {
    return RteStreamSetConfigs(&c_stream, config != nullptr ? config->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Register a stream observer.
   * @note technical preview
   * @param observer The pointer to StreamObserver to register.
   * @param err The pointer to Error to receive error information. Optional.
   * @return true if successful, false otherwise.
   */
  bool RegisterObserver(StreamObserver* observer, Error* err = nullptr) {
    return RteStreamRegisterObserver(&c_stream, observer != nullptr ? observer->c_observer : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Unregister a stream observer.
   * @note technical preview
   * @param observer The pointer to StreamObserver to unregister.
   * @param err The pointer to Error to receive error information. Optional.
   * @return true if successful, false otherwise.
   */
  bool UnregisterObserver(StreamObserver* observer, Error* err = nullptr) {
    return RteStreamUnregisterObserver(&c_stream, observer != nullptr ? observer->c_observer : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Get the audio track of the stream.
   * @note technical preview
   * @param err The pointer to Error to receive error information. Optional.
   * @return The AudioTrack object.
   */
  AudioTrack GetAudioTrack(Error* err = nullptr) {
    return AudioTrack(RteStreamGetAudioTrack(&c_stream, err != nullptr ? err->get_underlying_impl() : nullptr));
  }

  /**
   * @brief Add an audio track to the stream.
   * @note technical preview
   * @param audio_track The pointer to AudioTrack to add.
   * @param err The pointer to Error to receive error information. Optional.
   * @return true if successful, false otherwise.
   */
  bool AddAudioTrack(AudioTrack* audio_track, Error* err = nullptr) {
    return RteStreamAddAudioTrack(&c_stream, audio_track != nullptr ? audio_track->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Remove an audio track from the stream.
   * @note technical preview
   * @param audio_track The pointer to AudioTrack to remove.
   * @param err The pointer to Error to receive error information. Optional.
   * @return true if successful, false otherwise.
   */
  bool DelAudioTrack(AudioTrack* audio_track, Error* err = nullptr) {
    return RteStreamDelAudioTrack(&c_stream, audio_track != nullptr ? audio_track->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Get the video track of the stream.
   * @note technical preview
   * @param err The pointer to Error to receive error information. Optional.
   * @return The VideoTrack object.
   */
  VideoTrack GetVideoTrack(Error* err = nullptr) {
    RteVideoTrack video_track = RteStreamGetVideoTrack(&c_stream, err != nullptr ? err->get_underlying_impl() : nullptr);
    return VideoTrack(std::move(video_track));
  }

  /**
   * @brief Add a video track to the stream.
   * @note technical preview
   * @param video_track The pointer to VideoTrack to add.
   * @param err The pointer to Error to receive error information. Optional.
   * @return true if successful, false otherwise.
   */
  bool AddVideoTrack(VideoTrack* video_track, Error* err = nullptr) {
    return RteStreamAddVideoTrack(&c_stream, video_track != nullptr ? video_track->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Remove a video track from the stream.
   * @note technical preview
   * @param video_track The pointer to VideoTrack to remove.
   * @param err The pointer to Error to receive error information. Optional.
   * @return true if successful, false otherwise.
   */
  bool DelVideoTrack(VideoTrack* video_track, Error* err = nullptr) {
    return RteStreamDelVideoTrack(&c_stream, video_track != nullptr ? video_track->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Get the data track of the stream.
   * @note technical preview
   * @param err The pointer to Error to receive error information. Optional.
   * @return The RteDataTrack object.
   */
  RteDataTrack GetDataTrack(Error* err = nullptr) {
    return RteStreamGetDataTrack(&c_stream, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Add a data track to the stream.
   * @note technical preview
   * @param data_track The pointer to RteDataTrack to add.
   * @param err The pointer to Error to receive error information. Optional.
   * @return true if successful, false otherwise.
   */
  bool AddDataTrack(RteDataTrack* data_track, Error* err = nullptr) {
    return RteStreamAddDataTrack(&c_stream, data_track, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Remove a data track from the stream.
   * @note technical preview
   * @param data_track The pointer to RteDataTrack to remove.
   * @param err The pointer to Error to receive error information. Optional.
   * @return true if successful, false otherwise.
   */
  bool DelDataTrack(RteDataTrack* data_track, Error* err = nullptr) {
    return RteStreamDelDataTrack(&c_stream, data_track, err != nullptr ? err->get_underlying_impl() : nullptr);
  }

  /**
   * @brief Set the audio encoder configuration for the stream asynchronously.
   * @note technical preview
   * @param config The pointer to StreamConfig containing the audio encoder configuration.
   * @param cb The callback function to receive the result.
   */
  void SetAudioEncoderConfig(StreamConfig* config, std::function<void(Error* err)> cb) {
    CallbackContext<Stream>* context = new CallbackContext<Stream>(this, cb);
    RteStreamSetAudioEncoderConfig(&c_stream, config != nullptr ? config->get_underlying_impl() : nullptr, &CallbackFunc<::RteStream, Stream>, context);
  }

  /**
   * @brief Set the video encoder configuration for the stream asynchronously.
   * @note technical preview
   * @param config The pointer to StreamConfig containing the video encoder configuration.
   * @param cb The callback function to receive the result.
   */
  void SetVideoEncoderConfig(StreamConfig* config, std::function<void(Error* err)> cb) {
    CallbackContext<Stream>* context = new CallbackContext<Stream>(this, cb);
    RteStreamSetVideoEncoderConfig(&c_stream, config != nullptr ? config->get_underlying_impl() : nullptr, &CallbackFunc<::RteStream, Stream>, context);
  }

  /**
   * @brief Get the presence state of the stream asynchronously.
   * @note technical preview
   * @param cb The callback function to receive the presence state and error information.
   */
  void GetState(std::function<void(PresenceState* state, Error* err)> cb) {
    std::function<void(RtePresenceState* state, Error* err)> cb_func = [cb](RtePresenceState* state, Error* err) {

    };

    CallbackContextWithArgs<Stream, RtePresenceState*>* context = new CallbackContextWithArgs<Stream, RtePresenceState*>(this, cb_func);
    RteStreamGetState(&c_stream, &CallbackFuncWithArgs<RteStream, Stream, RtePresenceState*>, context);
  }

  /**
   * @brief Set the presence state of the stream asynchronously.
   * @note technical preview
   * @param cb The callback function to receive the result.
   */
  void SetState(std::function<void(Error* err)> cb) {
    CallbackContext<Stream>* context = new CallbackContext<Stream>(this, cb);
    RteStreamSetState(&c_stream, &CallbackFunc<::RteStream, Stream>, context);
  }

  /**
   * @brief Remove the presence state of the stream asynchronously by key.
   * @note technical preview
   * @param key The key of the state to remove.
   * @param cb The callback function to receive the result.
   */
  void RemoveState(const std::string& key, std::function<void(Error* err)> cb) {
    CallbackContext<Stream>* context = new CallbackContext<Stream>(this, cb);
    RteStreamRemoveState(&c_stream, key.c_str(), &CallbackFunc<::RteStream, Stream>, context);
  }

 protected:
  friend class Rte;
  friend class Player;

  ::RteStream c_stream = {};
};

}  // namespace rte
