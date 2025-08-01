 /**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include "rte_base/c/stream/remote_realtime_stream.h"
#include "rte_base/c/handle.h"

#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"
#include "rte_base/stream/rte_cpp_remote_stream.h"

namespace rte {

/**
 * @brief Configuration class for remote real-time stream
 * @technical preview
 */
class RemoteRealTimeStreamConfig : public RemoteStreamConfig {
public:
    /**
     * @brief Default constructor for RemoteRealTimeStreamConfig
     * @technical preview
     */
    RemoteRealTimeStreamConfig() : RemoteStreamConfig(reinterpret_cast<RteRemoteStreamConfig*>(new RteRemoteRealTimeStreamConfig())){
        c_remote_real_time_stream_config_ = reinterpret_cast<RteRemoteRealTimeStreamConfig*>(RemoteStreamConfig::get_underlying_impl());
        RteRemoteRealTimeStreamConfigInit(c_remote_real_time_stream_config_, nullptr);
        is_owner_ = true;
    }

    explicit RemoteRealTimeStreamConfig(RteRemoteRealTimeStreamConfig* config)
        : RemoteStreamConfig(reinterpret_cast<RteRemoteStreamConfig*>(config)), c_remote_real_time_stream_config_(config), is_owner_(false) {
    }
    
    /**
     * @brief Destructor for RemoteRealTimeStreamConfig
     * @technical preview
     */
    ~RemoteRealTimeStreamConfig() {
        if (is_owner_ && c_remote_real_time_stream_config_ != nullptr) {
            RteRemoteRealTimeStreamConfigDeinit(c_remote_real_time_stream_config_, nullptr);
            delete c_remote_real_time_stream_config_;
            is_owner_ = false;
        }
        c_remote_real_time_stream_config_ = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteRemoteRealTimeStreamConfig
     */
    RteRemoteRealTimeStreamConfig* get_underlying_impl() {
        return c_remote_real_time_stream_config_;
    }

private:
    RteRemoteRealTimeStreamConfig* c_remote_real_time_stream_config_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Information class for remote real-time stream
 * @technical preview
 */
class RemoteRealTimeStreamInfo : public RemoteStreamInfo {
public:
    /**
     * @brief Default constructor for RemoteRealTimeStreamInfo
     * @technical preview
     */
    RemoteRealTimeStreamInfo() : RemoteStreamInfo(reinterpret_cast<RteRemoteStreamInfo*>(new RteRemoteRealTimeStreamInfo())){
        c_remote_realtime_stream_info = reinterpret_cast<RteRemoteRealTimeStreamInfo*>(RemoteStreamInfo::get_underlying_impl());
        RteRemoteRealTimeStreamInfoInit(c_remote_realtime_stream_info, nullptr);
        is_owner_ = true;
    }

    explicit RemoteRealTimeStreamInfo(RteRemoteRealTimeStreamInfo* info)
        : RemoteStreamInfo(reinterpret_cast<RteRemoteStreamInfo*>(info)), c_remote_realtime_stream_info(info), is_owner_(false) {
    }

    RemoteRealTimeStreamInfo& operator=(const RemoteRealTimeStreamInfo& other) {
        if (this != &other) {    
            RteRemoteRealTimeStreamInfoCopy(c_remote_realtime_stream_info, other.c_remote_realtime_stream_info, nullptr);
        }
        return *this;
    }
    
    /**
     * @brief Destructor for RemoteRealTimeStreamInfo
     * @technical preview
     */
    ~RemoteRealTimeStreamInfo() {
        if (is_owner_ && c_remote_realtime_stream_info != nullptr) {
            RteRemoteRealTimeStreamInfoDeinit(c_remote_realtime_stream_info, nullptr);
            delete c_remote_realtime_stream_info;
            is_owner_ = false;
        }
        c_remote_realtime_stream_info = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteRemoteRealTimeStreamInfo
     */
    RteRemoteRealTimeStreamInfo* get_underlying_impl() {
        return c_remote_realtime_stream_info;
    }

private:
    RteRemoteRealTimeStreamInfo* c_remote_realtime_stream_info = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Remote real-time stream class for managing remote real-time streams
 * @technical preview
 */
class RemoteRealTimeStream : public RemoteStream {
public:

    RemoteRealTimeStream() = default;

    explicit RemoteRealTimeStream(const RteHandle& remote_realtime_stream_handle) : RemoteStream(remote_realtime_stream_handle) {
        c_remote_realtime_stream.handle = remote_realtime_stream_handle;
    }

    explicit RemoteRealTimeStream(const RteRemoteRealTimeStream &remote_realtime_stream) : RemoteStream(RteHandle(remote_realtime_stream.handle)) {
        c_remote_realtime_stream = remote_realtime_stream;
    }

    virtual ~RemoteRealTimeStream() = default;

    /**
     * @brief Get information about the remote real-time stream
     * @technical preview
     * @param info Pointer to RemoteRealTimeStreamInfo to receive the information
     * @param err Pointer to Error to receive error information (optional)
     * @return true if successful, false otherwise
     */
    bool GetInfo(RemoteRealTimeStreamInfo* info, Error* err = nullptr) {
        return RteRemoteRealTimeStreamGetInfo(&c_remote_realtime_stream, info != nullptr ? info->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    // bool RegisterObserver(RemoteRealTimeStreamObserver* observer, Error* err = nullptr) {
    //     return RteRemoteRealTimeStreamRegisterObserver(&c_remote_realtime_stream, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    // }

    // bool UnregisterObserver(RemoteRealTimeStreamObserver* observer, Error* err = nullptr) {
    //     return RteRemoteRealTimeStreamUnregisterObserver(&c_remote_realtime_stream, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    // }

private:
    RteRemoteRealTimeStream c_remote_realtime_stream = {0};
};

}  // namespace rte