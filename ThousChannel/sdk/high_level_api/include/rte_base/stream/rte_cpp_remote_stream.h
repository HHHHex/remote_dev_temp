/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include "rte_base/c/stream/remote_stream.h"
#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"
#include "rte_base/stream/rte_cpp_stream.h"

namespace rte {

/**
 * @brief Configuration class for remote stream
 * @technical preview
 */
class RemoteStreamConfig : public StreamConfig {
public:
    /**
     * @brief Default constructor for RemoteStreamConfig
     * @technical preview
     */
    RemoteStreamConfig() : StreamConfig(reinterpret_cast<RteStreamConfig*>(new RteRemoteStreamConfig())){
        c_remote_stream_config_ = reinterpret_cast<RteRemoteStreamConfig*>(StreamConfig::get_underlying_impl());
        RteRemoteStreamConfigInit(c_remote_stream_config_, nullptr);
        is_owner_ = true;
    }

    explicit RemoteStreamConfig(RteRemoteStreamConfig* config)
        : StreamConfig(reinterpret_cast<RteStreamConfig*>(config)), c_remote_stream_config_(config), is_owner_(false) {
    }
    
    /**
     * @brief Destructor for RemoteStreamConfig
     * @technical preview
     */
    ~RemoteStreamConfig() {
        if (is_owner_ && c_remote_stream_config_ != nullptr) {
            RteRemoteStreamConfigDeinit(c_remote_stream_config_, nullptr);
            delete c_remote_stream_config_;
            is_owner_ = false;
        }
        c_remote_stream_config_ = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteRemoteStreamConfig
     */
    RteRemoteStreamConfig* get_underlying_impl() {
        return c_remote_stream_config_;
    }

private:
    RteRemoteStreamConfig* c_remote_stream_config_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Information class for remote stream
 * @technical preview
 */
class RemoteStreamInfo : public StreamInfo {
public:
    /**
     * @brief Default constructor for RemoteStreamInfo
     * @technical preview   
     */
    RemoteStreamInfo() : StreamInfo(reinterpret_cast<RteStreamInfo*>(new RteRemoteStreamInfo())){
        c_remote_stream_info = reinterpret_cast<RteRemoteStreamInfo*>(StreamInfo::get_underlying_impl());
        RteRemoteStreamInfoInit(c_remote_stream_info, nullptr);
        is_owner_ = true;
    }

    explicit RemoteStreamInfo(RteRemoteStreamInfo* info)
        : StreamInfo(reinterpret_cast<RteStreamInfo*>(info)), c_remote_stream_info(info), is_owner_(false) {
    }

    explicit RemoteStreamInfo(const RemoteStreamInfo& other) : StreamInfo(reinterpret_cast<RteStreamInfo*>(new RteRemoteStreamInfo())) {
        c_remote_stream_info = reinterpret_cast<RteRemoteStreamInfo*>(StreamInfo::get_underlying_impl());
        RteRemoteStreamInfoInit(c_remote_stream_info, nullptr);
        RteRemoteStreamInfoCopy(c_remote_stream_info, other.c_remote_stream_info, nullptr);
        is_owner_ = true;
    }

    RemoteStreamInfo& operator=(const RemoteStreamInfo& other) {
        if (this != &other) {
            RteRemoteStreamInfoCopy(c_remote_stream_info, other.c_remote_stream_info, nullptr);
        }
        return *this;
    }
    
    /**
     * @brief Destructor for RemoteStreamInfo
     * @technical preview
     */
    ~RemoteStreamInfo() {
        if (is_owner_ && c_remote_stream_info != nullptr) {
            RteRemoteStreamInfoDeinit(c_remote_stream_info, nullptr);
            delete c_remote_stream_info;
            is_owner_ = false;
        }
        c_remote_stream_info = nullptr;
    }

    /**
     * @brief Get the stream ID
     * @technical preview
     * @return Stream ID as string
     */
    std::string GetStreamId() {
        String stream_id;
        RteRemoteStreamInfoGetStreamId(c_remote_stream_info, stream_id.get_underlying_impl(), nullptr);
        return std::string(stream_id.CStr());
    }

    void SetStreamId(const std::string& stream_id) {
        String stream_id_str(stream_id.c_str());
        RteRemoteStreamInfoSetStreamId(c_remote_stream_info, stream_id_str.get_underlying_impl(), nullptr);
    }

    /**
     * @brief Check if the stream has audio
     * @technical preview
     * @return true if stream has audio, false otherwise
     */
    bool GetHasAudio() {
        if (c_remote_stream_info == nullptr) {
            return false;
        }
        return c_remote_stream_info->has_audio;
    }

    /**
     * @brief Set the has audio flag
     * @technical preview
     * @param has_audio 
     */
    void SetHasAudio(bool has_audio) {
        if (c_remote_stream_info == nullptr) {
            return;
        }
        c_remote_stream_info->has_audio = has_audio;
    }

    /**
     * @brief Check if the stream has video
     * @technical preview
     * @return true if stream has video, false otherwise
     */
    bool GetHasVideo() {
        if (c_remote_stream_info == nullptr) {
            return false;
        }
        return c_remote_stream_info->has_video;
    }

    /**
     * @brief Set the has video flag
     * @technical preview
     * @param has_video
     */
    void SetHasVideo(bool has_video) {
        if (c_remote_stream_info == nullptr) {
            return;
        }
        c_remote_stream_info->has_video = has_video;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteRemoteStreamInfo
     */
    RteRemoteStreamInfo* get_underlying_impl() {
        return c_remote_stream_info;
    }

private:
    RteRemoteStreamInfo* c_remote_stream_info = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Remote stream class for managing remote streams
 * @technical preview
 */
class RemoteStream : public Stream {
public:

    RemoteStream() = default;

    explicit RemoteStream(const RteHandle& remote_stream_handle) : Stream(remote_stream_handle) {
        c_remote_stream.handle = remote_stream_handle;
    }

    explicit RemoteStream(RteRemoteStream& remote_stream) : Stream(RteHandle(remote_stream.handle)) {
        c_remote_stream = remote_stream;
    }

    explicit RemoteStream(const RemoteStream& other) : Stream(other.c_remote_stream.handle) {
        c_remote_stream = other.c_remote_stream;
    }

    
    virtual ~RemoteStream() = default;

    /**
     * @brief Get information about the remote stream
     * @technical preview
     * @param info Pointer to RemoteStreamInfo to receive the information
     * @param err Pointer to Error to receive error information (optional)
     * @return true if successful, false otherwise
     */
    bool GetInfo(RemoteStreamInfo* info, Error* err = nullptr) {
        return RteRemoteStreamGetInfo(&c_remote_stream, info != nullptr ? info->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    // bool RegisterObserver(RemoteStreamObserver* observer, Error* err = nullptr) {
    //     return RteRemoteStreamRegisterObserver(&c_remote_stream, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    // }

    // bool UnregisterObserver(RemoteStreamObserver* observer, Error* err = nullptr) {
    //     return RteRemoteStreamUnregisterObserver(&c_remote_stream, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    // }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteRemoteStream
     */
    RteRemoteStream* get_underlying_impl() {
        return &c_remote_stream;
    }

 private:
    RteRemoteStream c_remote_stream = {0};
};

}  // namespace rte 