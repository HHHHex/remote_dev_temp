/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "rte_base/c/stream/local_stream.h"
#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"
#include "rte_base/stream/rte_cpp_stream.h"

namespace rte {

/**
 * @brief Configuration class for local stream
 * @technical preview
 */
class LocalStreamConfig : public StreamConfig {
public:
    /**
     * @brief Default constructor for LocalStreamConfig
     * @technical preview
     */
    LocalStreamConfig() : c_local_stream_config_(new RteLocalStreamConfig()), is_owner_(true) {
        RteLocalStreamConfigInit(c_local_stream_config_, nullptr);
    }

    /**
     * @brief Constructor with existing RteLocalStreamConfig
     * @technical preview
     * @param config Pointer to existing RteLocalStreamConfig
     */
    explicit LocalStreamConfig(RteLocalStreamConfig* config)
        : StreamConfig(reinterpret_cast<RteStreamConfig*>(config)), c_local_stream_config_(config), is_owner_(false) {
    }

    /**
     * @brief Destructor for LocalStreamConfig
     * @technical preview
     */
    virtual ~LocalStreamConfig() {
        if (is_owner_ && c_local_stream_config_ != nullptr) {
            RteLocalStreamConfigDeinit(c_local_stream_config_, nullptr);
            delete c_local_stream_config_;
            is_owner_ = false;
        }
        c_local_stream_config_ = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteLocalStreamConfig
     */
    RteLocalStreamConfig* get_underlying_impl() {
        return c_local_stream_config_;
    }

private:
    RteLocalStreamConfig* c_local_stream_config_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Information class for local stream
 * @technical preview
 */
class LocalStreamInfo : public StreamInfo {
public:
    /**
     * @brief Default constructor for LocalStreamInfo
     * @technical preview
     */
    LocalStreamInfo() : StreamInfo(reinterpret_cast<RteStreamInfo*>(new RteLocalStreamInfo())){
        c_local_stream_info = reinterpret_cast<RteLocalStreamInfo*>(StreamInfo::get_underlying_impl());
        RteLocalStreamInfoInit(c_local_stream_info, nullptr);
        is_owner_ = true;
    }

    explicit LocalStreamInfo(RteLocalStreamInfo* info)
        : StreamInfo(reinterpret_cast<RteStreamInfo*>(info)), c_local_stream_info(info), is_owner_(false) {
    }
    
    /**
     * @brief Destructor for LocalStreamInfo
     * @technical preview
     */
    ~LocalStreamInfo() {
        if (is_owner_ && c_local_stream_info != nullptr) {
            RteLocalStreamInfoDeinit(c_local_stream_info, nullptr);
            delete c_local_stream_info;
            is_owner_ = false;
        }
        c_local_stream_info = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteLocalStreamInfo
     */
    RteLocalStreamInfo* get_underlying_impl() {
        return c_local_stream_info;
    }

private:
    RteLocalStreamInfo* c_local_stream_info = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Local stream class for managing local streams
 * @technical preview
 */
class LocalStream : public Stream {
public:

    LocalStream() = default;

    explicit LocalStream(const RteHandle& local_stream_handle) : Stream(local_stream_handle) {
        c_local_stream.handle = local_stream_handle;
    }

    explicit LocalStream(const RteLocalStream &local_stream) : Stream(RteHandle(local_stream.handle)) {
        c_local_stream = local_stream;
    }
    
    virtual ~LocalStream() = default;

    /**
     * @brief Get information about the local stream
     * @technical preview
     * @param info Pointer to LocalStreamInfo to receive the information
     * @param err Pointer to Error to receive error information (optional)
     * @return true if successful, false otherwise
     */
    bool GetInfo(LocalStreamInfo* info, Error* err = nullptr) {
        return RteLocalStreamGetInfo(&c_local_stream, info != nullptr ? info->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteLocalStream
     */
    RteLocalStream* get_underlying_impl(){
        return &c_local_stream;
    }

 protected:
    RteLocalStream c_local_stream = {0};
};

}  // namespace rte 