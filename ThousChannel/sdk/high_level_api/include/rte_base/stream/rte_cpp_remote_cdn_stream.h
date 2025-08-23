/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include "rte_base/c/stream/remote_cdn_stream.h"
#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"
#include "rte_base/stream/rte_cpp_remote_stream.h"

namespace rte {

/**
 * @brief Configuration class for remote CDN stream
 * @technical preview
 */
class RemoteCdnStreamConfig : public RemoteStreamConfig {
public:
    /**
     * @brief Default constructor for RemoteCdnStreamConfig
     * @technical preview
     */
    RemoteCdnStreamConfig() : c_remote_cdn_stream_config_(new RteRemoteCdnStreamConfig()), is_owner_(true) {
        RteRemoteCdnStreamConfigInit(c_remote_cdn_stream_config_, nullptr);
    }

    explicit RemoteCdnStreamConfig(RteRemoteCdnStreamConfig* config)
        : RemoteStreamConfig(reinterpret_cast<RteRemoteStreamConfig*>(config)), c_remote_cdn_stream_config_(config), is_owner_(false) {
    }
    
    /**
     * @brief Destructor for RemoteCdnStreamConfig
     * @technical preview
     */
    ~RemoteCdnStreamConfig() {
        if (is_owner_ && c_remote_cdn_stream_config_ != nullptr) {
            RteRemoteCdnStreamConfigDeinit(c_remote_cdn_stream_config_, nullptr);
            delete c_remote_cdn_stream_config_;
            is_owner_ = false;
        }
        c_remote_cdn_stream_config_ = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteRemoteCdnStreamConfig
     */
    RteRemoteCdnStreamConfig* get_underlying_impl() {
        return c_remote_cdn_stream_config_;
    }

private:
    RteRemoteCdnStreamConfig* c_remote_cdn_stream_config_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Information class for remote CDN stream
 * @technical preview
 */
class RemoteCdnStreamInfo : public RemoteStreamInfo {
public:
    /**
     * @brief Default constructor for RemoteCdnStreamInfo
     * @technical preview
     */
    RemoteCdnStreamInfo() { RteRemoteCdnStreamInfoInit(&c_info, nullptr); }
    
    /**
     * @brief Destructor for RemoteCdnStreamInfo
     * @technical preview
     */
    ~RemoteCdnStreamInfo() { RteRemoteCdnStreamInfoDeinit(&c_info, nullptr); }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteRemoteCdnStreamInfo
     */
    RteRemoteCdnStreamInfo* get_underlying_impl() {
        return &c_info;
    }

private:
    RteRemoteCdnStreamInfo c_info;
};

/**
 * @brief Remote CDN stream class for managing remote CDN streams
 * @technical preview
 */
class RemoteCdnStream : public RemoteStream {
public:
    /**
     * @brief Constructor for RemoteCdnStream
     * @technical preview
     * @param rte Pointer to RTE instance
     * @param config Pointer to RemoteCdnStreamConfig (optional)
     */
    RemoteCdnStream(Rte* rte, RemoteCdnStreamConfig* config = nullptr) 
        : RemoteStream(rte, nullptr) {
        RteRemoteCdnStreamInit(&c_remote_cdn_stream, &rte->c_rte, config != nullptr ? config->get_underlying_impl() : nullptr, nullptr);
    }
    
    /**
     * @brief Destructor for RemoteCdnStream
     * @technical preview
     */
    ~RemoteCdnStream() { RteRemoteCdnStreamDeinit(&c_remote_cdn_stream, nullptr); }

    /**
     * @brief Get information about the remote CDN stream
     * @technical preview
     * @param info Pointer to RemoteCdnStreamInfo to receive the information
     * @param err Pointer to Error to receive error information (optional)
     * @return true if successful, false otherwise
     */
    bool GetInfo(RemoteCdnStreamInfo* info, Error* err = nullptr) {
        return RteRemoteCdnStreamGetInfo(&c_remote_cdn_stream, info != nullptr ? info->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Register an observer for the remote CDN stream
     * @technical preview
     * @param observer Pointer to RemoteCdnStreamObserver to register
     * @param err Pointer to Error to receive error information (optional)
     * @return true if successful, false otherwise
     */
    bool RegisterObserver(RemoteCdnStreamObserver* observer, Error* err = nullptr) {
        return RteRemoteCdnStreamRegisterObserver(&c_remote_cdn_stream, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Unregister an observer from the remote CDN stream
     * @technical preview
     * @param observer Pointer to RemoteCdnStreamObserver to unregister
     * @param err Pointer to Error to receive error information (optional)
     * @return true if successful, false otherwise
     */
    bool UnregisterObserver(RemoteCdnStreamObserver* observer, Error* err = nullptr) {
        return RteRemoteCdnStreamUnregisterObserver(&c_remote_cdn_stream, observer != nullptr ? observer->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

private:
    RteRemoteCdnStream c_remote_cdn_stream;
};

}  // namespace rte