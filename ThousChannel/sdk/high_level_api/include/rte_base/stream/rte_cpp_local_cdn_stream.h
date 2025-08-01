/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include <string>
#include "rte_base/c/stream/local_cdn_stream.h"
#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"
#include "rte_base/stream/rte_cpp_local_stream.h"

namespace rte {

/**
 * @brief Configuration class for local CDN stream
 * @technical preview
 */
class LocalCdnStreamConfig : public LocalStreamConfig {
public:
    /**
     * @brief Default constructor for LocalCdnStreamConfig
     * @technical preview
     */
    LocalCdnStreamConfig()
        : LocalStreamConfig(reinterpret_cast<RteLocalStreamConfig*>(new RteLocalCdnStreamConfig())) {
        c_local_cdn_stream_config_ = reinterpret_cast<RteLocalCdnStreamConfig*>(LocalStreamConfig::get_underlying_impl());
        RteLocalCdnStreamConfigInit(c_local_cdn_stream_config_, nullptr);
        is_owner_ = true;
    }

    /**
     * @brief Constructor with existing RteLocalCdnStreamConfig
     * @technical preview
     * @param config Pointer to existing RteLocalCdnStreamConfig
     */
    explicit LocalCdnStreamConfig(RteLocalCdnStreamConfig* config)
        : LocalStreamConfig(reinterpret_cast<RteLocalStreamConfig*>(config)), c_local_cdn_stream_config_(config), is_owner_(false) {
    }
    
    /**
     * @brief Destructor for LocalCdnStreamConfig
     * @technical preview
     */
    virtual ~LocalCdnStreamConfig() {
        if (is_owner_ && c_local_cdn_stream_config_ != nullptr) {
            RteLocalCdnStreamConfigDeinit(c_local_cdn_stream_config_, nullptr);
            delete c_local_cdn_stream_config_;
            is_owner_ = false;
        }
        c_local_cdn_stream_config_ = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteLocalCdnStreamConfig
     */
    RteLocalCdnStreamConfig* get_underlying_impl() {
        return c_local_cdn_stream_config_;
    }

private:
    RteLocalCdnStreamConfig* c_local_cdn_stream_config_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Information class for local CDN stream
 * @technical preview
 */
class LocalCdnStreamInfo : public LocalStreamInfo {
public:
    /**
     * @brief Default constructor for LocalCdnStreamInfo
     * @technical preview
     */
    LocalCdnStreamInfo() : LocalStreamInfo(reinterpret_cast<RteLocalStreamInfo*>(new RteLocalCdnStreamInfo())){
        c_local_cdn_stream_info = reinterpret_cast<RteLocalCdnStreamInfo*>(LocalStreamInfo::get_underlying_impl());
        RteLocalCdnStreamInfoInit(c_local_cdn_stream_info, nullptr);
        is_owner_ = true;
    }

    explicit LocalCdnStreamInfo(RteLocalCdnStreamInfo* info)
        : LocalStreamInfo(reinterpret_cast<RteLocalStreamInfo*>(info)), c_local_cdn_stream_info(info), is_owner_(false) {
    }
    
    /**
     * @brief Destructor for LocalCdnStreamInfo
     * @technical preview
     */
    ~LocalCdnStreamInfo() {
        if (is_owner_ && c_local_cdn_stream_info != nullptr) {
            RteLocalCdnStreamInfoDeinit(c_local_cdn_stream_info, nullptr);
            delete c_local_cdn_stream_info;
            is_owner_ = false;
        }
        c_local_cdn_stream_info = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteLocalCdnStreamInfo
     */
    RteLocalCdnStreamInfo* get_underlying_impl() {
        return c_local_cdn_stream_info;
    }

private:
    RteLocalCdnStreamInfo* c_local_cdn_stream_info = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Local CDN stream class for managing local CDN streams
 * @technical preview
 */
class LocalCdnStream : public LocalStream {
public:
    /**
     * @brief Constructor for LocalCdnStream
     * @technical preview
     * @param rte Pointer to RTE instance
     * @param config Pointer to LocalCdnStreamConfig (optional)
     */
    explicit LocalCdnStream(Rte* rte, LocalCdnStreamConfig* config = nullptr): 
    c_local_cdn_stream(RteLocalCdnStreamCreate(&rte->c_rte, config != nullptr ? config->get_underlying_impl() : nullptr, nullptr)), 
    is_owner_(true) {
        c_local_stream.handle = c_local_cdn_stream.handle;
        c_stream.handle = c_local_cdn_stream.handle;
    }

    explicit LocalCdnStream(const RteHandle& local_cdn_stream_handle) : LocalStream(local_cdn_stream_handle) {
        c_local_cdn_stream.handle = local_cdn_stream_handle;
    }

    explicit LocalCdnStream(const RteLocalCdnStream &local_cdn_stream) : LocalStream(RteHandle(local_cdn_stream.handle)) {
        c_local_cdn_stream = local_cdn_stream;
    }
    
    /**
     * @brief Destructor for LocalCdnStream
     * @technical preview
     */
    virtual ~LocalCdnStream() { 
        if(is_owner_){
            RteLocalCdnStreamDestroy(&c_local_cdn_stream, nullptr); 
        }
    }


    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteLocalCdnStream
     */
    RteLocalCdnStream* get_underlying_impl() {
        return &c_local_cdn_stream;
    }

private:
    RteLocalCdnStream c_local_cdn_stream = {0};
    bool is_owner_ = false;
};

}  // namespace rte 