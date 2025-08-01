/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "rte_base/c/common.h"
#include "rte_base/c/track/local_track.h"
#include "rte_base/c/track/audio_track.h"
#include "rte_base/c/utils/frame.h"
#include "rte_base/c/utils/string.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct RteLocalAudioTrackConfig {
  RteAudioTrackConfig audio_track_config;

  uint32_t publish_volume;
  bool _publish_volume_is_set;

  uint32_t loopback_volume;
  bool _loopback_volume_is_set;

  bool enable_loopback_filter;
  bool _enable_loopback_filter_is_set;

  bool enable_publish_filter;
  bool _enable_publish_filter_is_set;

  RteString *json_parameter;
  bool _json_parameter_is_set;
} RteLocalAudioTrackConfig;

typedef struct RteLocalAudioTrackObserver RteLocalAudioTrackObserver;
struct RteLocalAudioTrackObserver {
  RteAudioTrackObserver audio_track_observer;

  void (*on_frame)(RteLocalAudioTrackObserver *self,
                   RteAudioFrame *audio_frame);
};

typedef struct RteLocalAudioTrackInfo {
  RteAudioTrackInfo audio_track_info;
} RteLocalAudioTrackInfo;

// @{
// Config
AGORA_RTE_API_C void RteLocalAudioTrackConfigInit(
    RteLocalAudioTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteLocalAudioTrackConfigDeinit(
    RteLocalAudioTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteLocalAudioTrackConfigCopy(RteLocalAudioTrackConfig *dest, const RteLocalAudioTrackConfig *src, RteError *err);

AGORA_RTE_API_C void RteLocalAudioTrackConfigSetPublishVolume(
    RteLocalAudioTrackConfig *self, uint32_t volume, RteError *err);
AGORA_RTE_API_C void RteLocalAudioTrackConfigGetPublishVolume(
    RteLocalAudioTrackConfig *self, uint32_t *volume, RteError *err);

AGORA_RTE_API_C void RteLocalAudioTrackConfigSetLoopbackVolume(
    RteLocalAudioTrackConfig *self, uint32_t volume, RteError *err);
AGORA_RTE_API_C void RteLocalAudioTrackConfigGetLoopbackVolume(
    RteLocalAudioTrackConfig *self, uint32_t *volume, RteError *err);

AGORA_RTE_API_C void RteLocalAudioTrackConfigSetEnableLoopbackFilter(
    RteLocalAudioTrackConfig *self, bool enable_loopback_filter, RteError *err);
AGORA_RTE_API_C void RteLocalAudioTrackConfigGetEnableLoopbackFilter(
    RteLocalAudioTrackConfig *self, bool *enable_loopback_filter,
    RteError *err);

AGORA_RTE_API_C void RteLocalAudioTrackConfigSetEnablePublishFilter(
    RteLocalAudioTrackConfig *self, bool enable_publish_filter, RteError *err);
AGORA_RTE_API_C void RteLocalAudioTrackConfigGetEnablePublishFilter(
    RteLocalAudioTrackConfig *self, bool *enable_publish_filter, RteError *err);

AGORA_RTE_API_C void RteLocalAudioTrackConfigSetJsonParameter(
    RteLocalAudioTrackConfig *self, RteString *json_parameter, RteError *err);
AGORA_RTE_API_C void RteLocalAudioTrackConfigGetJsonParameter(
    RteLocalAudioTrackConfig *self, RteString *json_parameter, RteError *err);
// @}


// @{ 
// Info
AGORA_RTE_API_C void RteLocalAudioTrackInfoInit(RteLocalAudioTrackInfo *info, RteError *err);
AGORA_RTE_API_C void RteLocalAudioTrackInfoDeinit(RteLocalAudioTrackInfo *info, RteError *err);
AGORA_RTE_API_C void RteLocalAudioTrackInfoCopy(RteLocalAudioTrackInfo *dest, const RteLocalAudioTrackInfo *src, RteError *err);

// @}

// @{
// Track observer
AGORA_RTE_API_C RteLocalAudioTrackObserver *RteLocalAudioTrackObserverCreate(
    RteError *err);
AGORA_RTE_API_C void RteLocalAudioTrackObserverDestroy(
    RteLocalAudioTrackObserver *self, RteError *err);
//}

AGORA_RTE_API_C void RteLocalAudioTrackInit(RteLocalAudioTrack *self,
                                           RteError *err);
AGORA_RTE_API_C void RteLocalAudioTrackDeinit(RteLocalAudioTrack *self,
                                             RteError *err);

AGORA_RTE_API_C void RteLocalAudioTrackStart(RteLocalAudioTrack *self,
                                       void (*cb)(RteLocalAudioTrack *self,
                                                  void *cb_data, RteError *err),
                                       void *cb_data);
AGORA_RTE_API_C void RteLocalAudioTrackStop(RteLocalAudioTrack *self,
                                      void (*cb)(RteLocalAudioTrack *self,
                                                 void *cb_data, RteError *err),
                                      void *cb_data);

AGORA_RTE_API_C bool RteLocalAudioTrackGetConfigs(
    RteLocalAudioTrack *self, RteLocalAudioTrackConfig *config, RteError *err);
AGORA_RTE_API_C bool RteLocalAudioTrackSetConfigs(
    RteLocalAudioTrack *self, RteLocalAudioTrackConfig *config, RteError *err);

AGORA_RTE_API_C bool RteLocalAudioTrackEnableLoopback(RteLocalAudioTrack *self,
                                                     RteError *err);

AGORA_RTE_API_C bool RteLocalAudioTrackRegisterTrackObserver(
    RteLocalAudioTrack *self, RteLocalAudioTrackObserver *observer,
    RteError *err);
AGORA_RTE_API_C bool RteLocalAudioTrackUnregisterTrackObserver(
    RteLocalAudioTrack *self, RteLocalAudioTrackObserver *observer,
    RteError *err);

#ifdef __cplusplus
}
#endif  // __cplusplus
