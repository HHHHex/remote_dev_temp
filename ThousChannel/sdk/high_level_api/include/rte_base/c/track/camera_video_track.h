/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "rte_base/c/handle.h"
#include "rte_base/c/common.h"
#include "rte_base/c/track/local_video_track.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct Rte Rte;

typedef enum RteCameraType {
  kRteCameraTypeBack,
  kRteCameraTypeFront
} RteCameraType;

typedef enum RteOrientation {
  kRteOrientation0 = 0,
  kRteOrientation90 = 90,
  kRteOrientation180 = 180,
  kRteOrientation270 = 270
} RteOrientation;

typedef struct RteCameraVideoTrackInitialConfig {
  RteLocalVideoTrackInitialConfig local_video_track_initial_config;
} RteCameraVideoTrackInitialConfig;

typedef struct RteCameraVideoTrackConfig {
  RteLocalVideoTrackConfig local_video_track_config;
  
  // Camera type (Android/iOS platforms only)
  RteCameraType type;
  bool _type_is_set;
  
  // Device ID (Windows/Mac/Linux platforms only)
  RteString* device_id;
  bool _device_id_is_set;
  
  // Camera zoom level
  float zoom;
  bool _zoom_is_set;
  
  // Focus coordinates
  float focus_x;
  bool _focus_x_is_set;
  float focus_y;
  bool _focus_y_is_set;
  
  // Auto face focus feature
  bool enable_auto_face_focus;
  bool _enable_auto_face_focus_is_set;
  
  // Face detection feature
  bool enable_face_detection;
  bool _enable_face_detection_is_set;
  
  // Video capture device orientation
  RteOrientation orientation;
  bool _orientation_is_set;
  
  // Capture output delay in milliseconds
  int32_t rtc_video_hw_capture_delay;
  bool _rtc_video_hw_capture_delay_is_set;
  
  // Camera capture method for Android platform
  int32_t rtc_video_android_camera_select;
  bool _rtc_video_android_camera_select_is_set;
  
  // SDK parameter as JSON string
  RteString* json_parameter;
  bool _json_parameter_is_set;
} RteCameraVideoTrackConfig;


typedef struct RteCameraVideoTrackInfo {
  RteLocalVideoTrackInfo local_video_track_info;
  // ... more fields.
} RteCameraVideoTrackInfo;


AGORA_RTE_API_C void RteCameraVideoTrackConfigInit(RteCameraVideoTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackConfigDeinit(RteCameraVideoTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackConfigCopy(RteCameraVideoTrackConfig *dst, const RteCameraVideoTrackConfig *src, RteError *err);


AGORA_RTE_API_C void RteCameraVideoTrackConfigSetCameraType(RteCameraVideoTrackConfig *self, RteCameraType type, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackConfigGetCameraType(RteCameraVideoTrackConfig *self, RteCameraType *type, RteError *err);

AGORA_RTE_API_C void RteCameraVideoTrackConfigSetDeviceId(RteCameraVideoTrackConfig *self, RteString *device_id, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackConfigGetDeviceId(RteCameraVideoTrackConfig *self, RteString *device_id, RteError *err);

AGORA_RTE_API_C void RteCameraVideoTrackConfigSetZoom(RteCameraVideoTrackConfig *self, float zoom, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackConfigGetZoom(RteCameraVideoTrackConfig *self, float *zoom, RteError *err);

AGORA_RTE_API_C void RteCameraVideoTrackConfigSetFocus(RteCameraVideoTrackConfig *self, float x, float y, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackConfigGetFocus(RteCameraVideoTrackConfig *self, float *x, float *y, RteError *err);

AGORA_RTE_API_C void RteCameraVideoTrackConfigSetEnableAutoFaceFocus(RteCameraVideoTrackConfig *self, bool enable_auto_face_focus, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackConfigGetEnableAutoFaceFocus(RteCameraVideoTrackConfig *self, bool *enable_auto_face_focus, RteError *err);

AGORA_RTE_API_C void RteCameraVideoTrackConfigSetEnableFaceDetection(RteCameraVideoTrackConfig *self, bool enable_face_detection, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackConfigGetEnableFaceDetection(RteCameraVideoTrackConfig *self, bool *enable_face_detection, RteError *err);

AGORA_RTE_API_C void RteCameraVideoTrackConfigSetOrientation(RteCameraVideoTrackConfig *self, RteOrientation orientation, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackConfigGetOrientation(RteCameraVideoTrackConfig *self, RteOrientation *orientation, RteError *err);

AGORA_RTE_API_C void RteCameraVideoTrackConfigSetRtcVideoHwCaptureDelay(RteCameraVideoTrackConfig *self, int32_t rtc_video_hw_capture_delay, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackConfigGetRtcVideoHwCaptureDelay(RteCameraVideoTrackConfig *self, int32_t *rtc_video_hw_capture_delay, RteError *err);

AGORA_RTE_API_C void RteCameraVideoTrackConfigSetRtcVideoAndroidCameraSelect(RteCameraVideoTrackConfig *self, int32_t rtc_video_android_camera_select, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackConfigGetRtcVideoAndroidCameraSelect(RteCameraVideoTrackConfig *self, int32_t *rtc_video_android_camera_select, RteError *err);

AGORA_RTE_API_C void RteCameraVideoTrackConfigSetJsonParameter(RteCameraVideoTrackConfig *self, RteString *json_parameter, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackConfigGetJsonParameter(RteCameraVideoTrackConfig *self, RteString *json_parameter, RteError *err);


AGORA_RTE_API_C void RteCameraVideoTrackInfoInit(RteCameraVideoTrackInfo *info, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackInfoDeinit(RteCameraVideoTrackInfo *info, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackInfoCopy(RteCameraVideoTrackInfo *dst, const RteCameraVideoTrackInfo *src, RteError *err);



AGORA_RTE_API_C RteCameraVideoTrack RteCameraVideoTrackCreate(
    Rte *rte, RteCameraVideoTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteCameraVideoTrackDestroy(RteCameraVideoTrack *self,
                                       RteError *err);
AGORA_RTE_API_C bool RteCameraVideoTrackGetConfigs(RteCameraVideoTrack *self, RteCameraVideoTrackConfig *config, RteError *err);
AGORA_RTE_API_C bool RteCameraVideoTrackSetConfigs(RteCameraVideoTrack *self, RteCameraVideoTrackConfig *config, RteError *err);

#ifdef __cplusplus
}
#endif  // __cplusplus
