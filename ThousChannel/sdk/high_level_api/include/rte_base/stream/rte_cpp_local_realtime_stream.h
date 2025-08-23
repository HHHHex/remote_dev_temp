/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include <string>
#include "rte_base/c/stream/local_realtime_stream.h"

#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"
#include "rte_base/stream/rte_cpp_local_stream.h"

namespace rte {

/**
 * @brief Configuration class for local real-time stream
 * @technical preview
 */
class LocalRealTimeStreamConfig : public LocalStreamConfig {
public:
    /**
     * @brief Default constructor for LocalRealTimeStreamConfig
     * @technical preview
     */
    LocalRealTimeStreamConfig()
        : LocalStreamConfig(reinterpret_cast<RteLocalStreamConfig*>(new RteLocalRealTimeStreamConfig())) {
        c_local_realtime_stream_config_ = reinterpret_cast<RteLocalRealTimeStreamConfig*>(LocalStreamConfig::get_underlying_impl());
        RteLocalRealTimeStreamConfigInit(c_local_realtime_stream_config_, nullptr);
        is_owner_ = true;
    }

    /**
     * @brief Constructor with existing RteLocalRealTimeStreamConfig
     * @technical preview
     * @param config Pointer to existing RteLocalRealTimeStreamConfig
     */
    explicit LocalRealTimeStreamConfig(RteLocalRealTimeStreamConfig* config)
        : LocalStreamConfig(reinterpret_cast<RteLocalStreamConfig*>(config)), c_local_realtime_stream_config_(config), is_owner_(false) {
    }

    /**
     * @brief Destructor for LocalRealTimeStreamConfig
     * @technical preview
     */
    virtual ~LocalRealTimeStreamConfig() {
        if (is_owner_ && c_local_realtime_stream_config_ != nullptr) {
            RteLocalRealTimeStreamConfigDeinit(c_local_realtime_stream_config_, nullptr);
            delete c_local_realtime_stream_config_;
            is_owner_ = false;
        }
        c_local_realtime_stream_config_ = nullptr;
    }

    /**
     * @brief Set the stream token for the local real-time stream
     * @technical preview
     * @param stream_token The stream token to set
     */
    void SetStreamToken(const std::string& stream_token, Error* err = nullptr) {
        String rte_stream_token(stream_token.c_str());
        RteLocalRealTimeStreamConfigSetStreamToken(
            c_local_realtime_stream_config_,
            rte_stream_token.get_underlying_impl(),
            err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the stream token for the local real-time stream
     * @technical preview
     * @param err Optional pointer to Error for receiving error information
     * @return The stream token as a std::string
     */
    std::string GetStreamToken(Error* err = nullptr) {
        String rte_stream_token;
        RteLocalRealTimeStreamConfigGetStreamToken(
            c_local_realtime_stream_config_,
            rte_stream_token.get_underlying_impl(),
            err != nullptr ? err->get_underlying_impl() : nullptr);
        return std::string(rte_stream_token.CStr());
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteLocalRealTimeStreamConfig
     */
    RteLocalRealTimeStreamConfig* get_underlying_impl() {
        return c_local_realtime_stream_config_;
    }

private:
    RteLocalRealTimeStreamConfig* c_local_realtime_stream_config_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Information class for local real-time stream
 * @technical preview
 */
class LocalRealTimeStreamInfo : public LocalStreamInfo {
public:
    /**
     * @brief Default constructor for LocalRealTimeStreamInfo
     * @technical preview
     */
    LocalRealTimeStreamInfo() : LocalStreamInfo(reinterpret_cast<RteLocalStreamInfo*>(new RteLocalRealTimeStreamInfo())){
        c_local_realtime_stream_info = reinterpret_cast<RteLocalRealTimeStreamInfo*>(LocalStreamInfo::get_underlying_impl());
        RteLocalRealTimeStreamInfoInit(c_local_realtime_stream_info, nullptr);
        is_owner_ = true;
    }

    explicit LocalRealTimeStreamInfo(RteLocalRealTimeStreamInfo* info)
        : LocalStreamInfo(reinterpret_cast<RteLocalStreamInfo*>(info)), c_local_realtime_stream_info(info), is_owner_(false) {
    }

    /**
     * @brief Destructor for LocalRealTimeStreamInfo
     * @technical preview
     */
    ~LocalRealTimeStreamInfo() {
        if (is_owner_ && c_local_realtime_stream_info != nullptr) {
            RteLocalRealTimeStreamInfoDeinit(c_local_realtime_stream_info, nullptr);
            delete c_local_realtime_stream_info;
            is_owner_ = false;
        }
        c_local_realtime_stream_info = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteLocalRealTimeStreamInfo
     */
    RteLocalRealTimeStreamInfo* get_underlying_impl() {
        return c_local_realtime_stream_info;
    }

private:
    RteLocalRealTimeStreamInfo* c_local_realtime_stream_info = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Local real-time stream class for managing local real-time streams
 * @technical preview
 */
class LocalRealTimeStream : public LocalStream {
public:

    explicit LocalRealTimeStream(Rte* rte, LocalRealTimeStreamConfig* config = nullptr): 
    c_local_realtime_stream(RteLocalRealTimeStreamCreate(&rte->c_rte, config != nullptr ? config->get_underlying_impl() : nullptr, nullptr)), 
    is_owner_(true) {
        c_local_stream.handle = c_local_realtime_stream.handle;
        c_stream.handle = c_local_realtime_stream.handle;
    }

    explicit LocalRealTimeStream(const RteHandle& local_realtime_stream_handle) : LocalStream(local_realtime_stream_handle) {
        c_local_realtime_stream.handle = local_realtime_stream_handle;
    }

    explicit LocalRealTimeStream(const RteLocalRealTimeStream &local_realtime_stream) : LocalStream(RteHandle(local_realtime_stream.handle)) {
        c_local_realtime_stream = local_realtime_stream;
    }
    
    virtual ~LocalRealTimeStream() { 
        if(is_owner_){
            RteLocalRealTimeStreamDestroy(&c_local_realtime_stream, nullptr); 
        }
    }

    /**
     * @brief Get the configuration of the local real-time stream
     * @technical preview
     * @param config Pointer to LocalRealTimeStreamConfig to receive the configuration
     * @param err Optional pointer to Error to receive error information
     * @return true if successful, false otherwise
     */
    bool GetConfigs(LocalRealTimeStreamConfig* config, Error* err = nullptr) {
        return RteLocalRealTimeStreamGetConfigs(&c_local_realtime_stream, 
                                                config != nullptr ? config->get_underlying_impl() : nullptr, 
                                                err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Set the configuration of the local real-time stream
     * @technical preview
     * @param config Pointer to LocalRealTimeStreamConfig to set the configuration
     * @param err Optional pointer to Error to receive error information
     * @return true if successful, false otherwise
     */
    bool SetConfigs(LocalRealTimeStreamConfig* config, Error* err = nullptr) {
        return RteLocalRealTimeStreamSetConfigs(&c_local_realtime_stream, 
                                                config != nullptr ? config->get_underlying_impl() : nullptr, 
                                                err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get information about the local real-time stream
     * @technical preview
     * @param info Pointer to LocalRealTimeStreamInfo to receive the information
     * @param err Pointer to Error to receive error information (optional)
     * @return true if successful, false otherwise
     */
    bool GetInfo(LocalRealTimeStreamInfo* info, Error* err = nullptr) {
        return RteLocalRealTimeStreamGetInfo(&c_local_realtime_stream, info != nullptr ? info->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }


    RteLocalRealTimeStream* get_underlying_impl() {
        return &c_local_realtime_stream;
    }

private:
    RteLocalRealTimeStream c_local_realtime_stream = {0};
    bool is_owner_ = false;
};

}  // namespace rte 