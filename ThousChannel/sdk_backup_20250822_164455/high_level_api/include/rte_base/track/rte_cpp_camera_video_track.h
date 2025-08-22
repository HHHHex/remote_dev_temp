/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include "rte_base/c/track/camera_video_track.h"
#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"
#include "rte_base/rte_cpp_callback_utils.h"
#include "rte_base/rte_cpp_rte.h"
#include "rte_base/track/rte_cpp_local_video_track.h"

namespace rte {

using CameraType = ::RteCameraType;
using Orientation = ::RteOrientation;

/**
 * @brief Configuration class for camera video track
 * @technical preview
 */
class CameraVideoTrackConfig : public LocalVideoTrackConfig {
public:
    /**
     * @brief Default constructor for CameraVideoTrackConfig
     * @technical preview
     */
    CameraVideoTrackConfig()
        : LocalVideoTrackConfig(reinterpret_cast<RteLocalVideoTrackConfig*>(new RteCameraVideoTrackConfig())) {
        c_camera_video_track_config_ = reinterpret_cast<RteCameraVideoTrackConfig*>(LocalVideoTrackConfig::get_underlying_impl());
        RteCameraVideoTrackConfigInit(c_camera_video_track_config_, nullptr);
        is_owner_ = true;
    }

    /**
     * @brief Constructor with existing RteCameraVideoTrackConfig
     * @technical preview
     * @param config Pointer to existing RteCameraVideoTrackConfig
     */
    explicit CameraVideoTrackConfig(RteCameraVideoTrackConfig* config)
        : LocalVideoTrackConfig(reinterpret_cast<RteLocalVideoTrackConfig*>(config)), c_camera_video_track_config_(config), is_owner_(false) { 
    }
    
    /**
     * @brief Destructor for CameraVideoTrackConfig
     * @technical preview
     */
    virtual ~CameraVideoTrackConfig() { 
        if(is_owner_ && c_camera_video_track_config_ != nullptr){
            RteCameraVideoTrackConfigDeinit(c_camera_video_track_config_, nullptr);
            delete c_camera_video_track_config_;
            is_owner_ = false;
        }
        c_camera_video_track_config_ = nullptr;
    }
    
    /**
     * @brief Set camera type for camera video track
     * @technical preview
     * @param type Camera type to set
     * @param err Pointer to Error object (optional)
     */
    void SetCameraType(CameraType type, Error* err = nullptr) {
        RteCameraVideoTrackConfigSetCameraType(c_camera_video_track_config_, static_cast<RteCameraType>(type), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get camera type from camera video track configuration
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return Current camera type
     */
    CameraType GetCameraType(Error* err = nullptr) {
        RteCameraType type;
        RteCameraVideoTrackConfigGetCameraType(c_camera_video_track_config_, &type, err != nullptr ? err->get_underlying_impl() : nullptr);
        return static_cast<CameraType>(type);
    }

    /**
     * @brief Set device ID for camera video track
     * @technical preview
     * @param device_id Device ID string
     * @param err Pointer to Error object (optional)
     */
    void SetDeviceId(const std::string& device_id, Error* err = nullptr) {
        String str(device_id.c_str());
        RteCameraVideoTrackConfigSetDeviceId(c_camera_video_track_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get device ID from camera video track configuration
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return Device ID string
     */
    std::string GetDeviceId(Error* err = nullptr) {
        String str;
        RteCameraVideoTrackConfigGetDeviceId(c_camera_video_track_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
        return std::string(str.CStr());
    }

    /**
     * @brief Set zoom level for camera video track
     * @technical preview
     * @param zoom Zoom level value
     * @param err Pointer to Error object (optional)
     */
    void SetZoom(float zoom, Error* err = nullptr) {
        RteCameraVideoTrackConfigSetZoom(c_camera_video_track_config_, zoom, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get zoom level from camera video track configuration
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return Current zoom level
     */
    float GetZoom(Error* err = nullptr) {
        float zoom;
        RteCameraVideoTrackConfigGetZoom(c_camera_video_track_config_, &zoom, err != nullptr ? err->get_underlying_impl() : nullptr);
        return zoom;
    }

    /**
     * @brief Set focus point for camera video track
     * @technical preview
     * @param x X coordinate of focus point
     * @param y Y coordinate of focus point
     * @param err Pointer to Error object (optional)
     */
    void SetFocus(float x, float y, Error* err = nullptr) {
        RteCameraVideoTrackConfigSetFocus(c_camera_video_track_config_, x, y, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get focus point from camera video track configuration
     * @technical preview
     * @param x Pointer to store X coordinate of focus point
     * @param y Pointer to store Y coordinate of focus point
     * @param err Pointer to Error object (optional)
     */
    void GetFocus(float* x, float* y, Error* err = nullptr) {
        RteCameraVideoTrackConfigGetFocus(c_camera_video_track_config_, x, y, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Enable or disable auto face focus
     * @technical preview
     * @param enable_auto_face_focus true to enable, false to disable
     * @param err Pointer to Error object (optional)
     */
    void SetEnableAutoFaceFocus(bool enable_auto_face_focus, Error* err = nullptr) {
        RteCameraVideoTrackConfigSetEnableAutoFaceFocus(c_camera_video_track_config_, enable_auto_face_focus, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get auto face focus setting
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return true if auto face focus is enabled, false otherwise
     */
    bool GetEnableAutoFaceFocus(Error* err = nullptr) {
        bool enable_auto_face_focus;
        RteCameraVideoTrackConfigGetEnableAutoFaceFocus(c_camera_video_track_config_, &enable_auto_face_focus, err != nullptr ? err->get_underlying_impl() : nullptr);
        return enable_auto_face_focus;
    }

    /**
     * @brief Enable or disable face detection
     * @technical preview
     * @param enable_face_detection true to enable, false to disable
     * @param err Pointer to Error object (optional)
     */
    void SetEnableFaceDetection(bool enable_face_detection, Error* err = nullptr) {
        RteCameraVideoTrackConfigSetEnableFaceDetection(c_camera_video_track_config_, enable_face_detection, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get face detection setting
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return true if face detection is enabled, false otherwise
     */
    bool GetEnableFaceDetection(Error* err = nullptr) {
        bool enable_face_detection;
        RteCameraVideoTrackConfigGetEnableFaceDetection(c_camera_video_track_config_, &enable_face_detection, err != nullptr ? err->get_underlying_impl() : nullptr);
        return enable_face_detection;
    }

    /**
     * @brief Set orientation for camera video track
     * @technical preview
     * @param orientation Camera orientation
     * @param err Pointer to Error object (optional)
     */
    void SetOrientation(Orientation orientation, Error* err = nullptr) {
        RteCameraVideoTrackConfigSetOrientation(c_camera_video_track_config_, static_cast<RteOrientation>(orientation), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get orientation from camera video track configuration
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return Current camera orientation
     */
    Orientation GetOrientation(Error* err = nullptr) {
        RteOrientation orientation;
        RteCameraVideoTrackConfigGetOrientation(c_camera_video_track_config_, &orientation, err != nullptr ? err->get_underlying_impl() : nullptr);
        return static_cast<Orientation>(orientation);
    }

    /**
     * @brief Set RTC video hardware capture delay
     * @technical preview
     * @param rtc_video_hw_capture_delay Hardware capture delay value
     * @param err Pointer to Error object (optional)
     */
    void SetRtcVideoHwCaptureDelay(int32_t rtc_video_hw_capture_delay, Error* err = nullptr) {
        RteCameraVideoTrackConfigSetRtcVideoHwCaptureDelay(c_camera_video_track_config_, rtc_video_hw_capture_delay, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get RTC video hardware capture delay
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return Current hardware capture delay value
     */
    int32_t GetRtcVideoHwCaptureDelay(Error* err = nullptr) {
        int32_t rtc_video_hw_capture_delay;
        RteCameraVideoTrackConfigGetRtcVideoHwCaptureDelay(c_camera_video_track_config_, &rtc_video_hw_capture_delay, err != nullptr ? err->get_underlying_impl() : nullptr);
        return rtc_video_hw_capture_delay;
    }

    /**
     * @brief Set RTC video Android camera selection
     * @technical preview
     * @param rtc_video_android_camera_select Android camera selection value
     * @param err Pointer to Error object (optional)
     */
    void SetRtcVideoAndroidCameraSelect(int32_t rtc_video_android_camera_select, Error* err = nullptr) {
        RteCameraVideoTrackConfigSetRtcVideoAndroidCameraSelect(c_camera_video_track_config_, rtc_video_android_camera_select, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get RTC video Android camera selection
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return Current Android camera selection value
     */
    int32_t GetRtcVideoAndroidCameraSelect(Error* err = nullptr) {
        int32_t rtc_video_android_camera_select;
        RteCameraVideoTrackConfigGetRtcVideoAndroidCameraSelect(c_camera_video_track_config_, &rtc_video_android_camera_select, err != nullptr ? err->get_underlying_impl() : nullptr);
        return rtc_video_android_camera_select;
    }

    /**
     * @brief Set JSON parameter for camera video track configuration
     * @technical preview
     * @param json_parameter JSON string parameter
     * @param err Pointer to Error object (optional)
     */
    void SetJsonParameter(const std::string& json_parameter, Error* err = nullptr) {
        String str(json_parameter.c_str());
        RteCameraVideoTrackConfigSetJsonParameter(c_camera_video_track_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get JSON parameter from camera video track configuration
     * @technical preview
     * @param err Pointer to Error object (optional)
     * @return JSON string parameter
     */
    std::string GetJsonParameter(Error* err = nullptr) {
        String str;
        RteCameraVideoTrackConfigGetJsonParameter(c_camera_video_track_config_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
        return std::string(str.CStr());
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteCameraVideoTrackConfig
     */
    RteCameraVideoTrackConfig* get_underlying_impl() {
        return c_camera_video_track_config_;
    }

private:    
    RteCameraVideoTrackConfig* c_camera_video_track_config_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Information class for camera video track
 * @technical preview
 */
class CameraVideoTrackInfo : public LocalVideoTrackInfo {
public:
    /**
     * @brief Default constructor for CameraVideoTrackInfo
     * @technical preview
     */
    CameraVideoTrackInfo()
        : LocalVideoTrackInfo(reinterpret_cast<RteLocalVideoTrackInfo*>(new RteCameraVideoTrackInfo())) {
        c_camera_video_track_info_ = reinterpret_cast<RteCameraVideoTrackInfo*>(LocalVideoTrackInfo::get_underlying_impl());
        RteCameraVideoTrackInfoInit(c_camera_video_track_info_, nullptr);
        is_owner_ = true;
    }
    
    /**
     * @brief Constructor with existing RteCameraVideoTrackInfo
     * @technical preview
     * @param info Pointer to existing RteCameraVideoTrackInfo
     */
    explicit CameraVideoTrackInfo(RteCameraVideoTrackInfo* info)
        : LocalVideoTrackInfo(reinterpret_cast<RteLocalVideoTrackInfo*>(info)), c_camera_video_track_info_(info), is_owner_(false) {
    }

    /**
     * @brief Destructor for CameraVideoTrackInfo
     * @technical preview
     */
    virtual ~CameraVideoTrackInfo() { 
        if(is_owner_ && c_camera_video_track_info_ != nullptr){
            RteCameraVideoTrackInfoDeinit(c_camera_video_track_info_, nullptr);
            delete c_camera_video_track_info_;
            is_owner_ = false;
        }
        c_camera_video_track_info_ = nullptr;
    }

    /**
     * @brief Get the underlying C implementation
     * @technical preview
     * @return Pointer to RteCameraVideoTrackInfo
     */
    RteCameraVideoTrackInfo* get_underlying_impl() {
        return c_camera_video_track_info_;
    }

private:
    RteCameraVideoTrackInfo* c_camera_video_track_info_ = nullptr;
    bool is_owner_ = false;
};

/**
 * @brief Camera video track class for managing camera video streams
 * @technical preview
 */
class CameraVideoTrack : public LocalVideoTrack {
public:

    explicit CameraVideoTrack(Rte* rte, CameraVideoTrackConfig* config = nullptr) 
        : c_camera_video_track(RteCameraVideoTrackCreate(rte != nullptr ? &rte->c_rte : nullptr, config != nullptr ? config->get_underlying_impl() : nullptr, nullptr)), is_owner_(true) {
        c_local_video_track.handle = c_camera_video_track.handle;
        c_video_track.handle = c_camera_video_track.handle;
        c_track.handle = c_camera_video_track.handle;
    }

    explicit CameraVideoTrack(const RteHandle& camera_video_track_handle) : LocalVideoTrack(camera_video_track_handle) {
        c_camera_video_track.handle = camera_video_track_handle;
    }

    explicit CameraVideoTrack(const RteCameraVideoTrack &&camera_video_track) : LocalVideoTrack(RteHandle(camera_video_track.handle)) {
        c_camera_video_track = camera_video_track;
    }

    /**
     * @brief Destructor for CameraVideoTrack
     * @technical preview
     */
    virtual ~CameraVideoTrack() { 
        if(is_owner_){
            RteCameraVideoTrackDestroy(&c_camera_video_track, nullptr); 
            is_owner_ = false;
        }
    }

    /**
     * @brief Get camera video track configurations
     * @technical preview
     * @param config Pointer to CameraVideoTrackConfig to store the configurations
     * @param err Pointer to Error object (optional)
     * @return true if successful, false otherwise
     */
    bool GetConfigs(CameraVideoTrackConfig* config, Error* err = nullptr) {
        return RteCameraVideoTrackGetConfigs(&c_camera_video_track, config != nullptr ? config->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Set camera video track configurations
     * @technical preview
     * @param config Pointer to CameraVideoTrackConfig containing the configurations
     * @param err Pointer to Error object (optional)
     * @return true if successful, false otherwise
     */
    bool SetConfigs(CameraVideoTrackConfig* config, Error* err = nullptr) {
        return RteCameraVideoTrackSetConfigs(&c_camera_video_track, config != nullptr ? config->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

private:
    RteCameraVideoTrack c_camera_video_track = {0};
    bool is_owner_ = false;
};

}  // namespace rte
