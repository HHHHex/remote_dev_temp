/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include <stdint.h>

#include "rte_base/c/common.h"
#include "rte_base/c/track/remote_track.h"
#include "rte_base/c/track/audio_track.h"
#include "rte_base/c/utils/frame.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct RteRemoteAudioTrackConfig {
  RteAudioTrackConfig audio_track_config;

  uint32_t playback_volume;
  bool has_playback_volume;
  RteString* json_parameter;
  bool has_json_parameter;
} RteRemoteAudioTrackConfig;

typedef struct RteRemoteAudioTrackObserver RteRemoteAudioTrackObserver;
struct RteRemoteAudioTrackObserver {
  RteAudioTrackObserver audio_track_observer;

  void (*on_frame)(RteRemoteAudioTrackObserver *self,
                   RteAudioFrame *audio_frame);
};

typedef struct RteRemoteAudioTrackInfo {
  RteAudioTrackInfo audio_track_info;
} RteRemoteAudioTrackInfo;

// @{
// Config
AGORA_RTE_API_C void RteRemoteAudioTrackConfigInit(
    RteRemoteAudioTrackConfig *config, RteError *err);
    
AGORA_RTE_API_C void RteRemoteAudioTrackConfigDeinit(
    RteRemoteAudioTrackConfig *config, RteError *err);

AGORA_RTE_API_C void RteRemoteAudioTrackConfigSetPlaybackVolume(
    RteRemoteAudioTrackConfig *self, uint32_t volume, RteError *err);
AGORA_RTE_API_C void RteRemoteAudioTrackConfigGetPlaybackVolume(
    RteRemoteAudioTrackConfig *self, uint32_t *volume, RteError *err);

AGORA_RTE_API_C void RteRemoteAudioTrackConfigSetJsonParameter(
    RteRemoteAudioTrackConfig *self, RteString *json_parameter, RteError *err);
AGORA_RTE_API_C void RteRemoteAudioTrackConfigGetJsonParameter(
    RteRemoteAudioTrackConfig *self, RteString *json_parameter, RteError *err);
// @}

// @{
// Info
AGORA_RTE_API_C void RteRemoteAudioTrackInfoInit(RteRemoteAudioTrackInfo *info, RteError *err);
AGORA_RTE_API_C void RteRemoteAudioTrackInfoDeinit(RteRemoteAudioTrackInfo *info, RteError *err);
//}

// @{
// Track observer
AGORA_RTE_API_C RteRemoteAudioTrackObserver *RteRemoteAudioTrackObserverCreate(
    RteError *err);
AGORA_RTE_API_C void RteRemoteAudioTrackObserverDestroy(
    RteRemoteAudioTrackObserver *self, RteError *err);
//}

AGORA_RTE_API_C void RteRemoteAudioTrackInit(RteRemoteAudioTrack *self,
                                            RteError *err);
AGORA_RTE_API_C void RteRemoteAudioTrackDeinit(RteRemoteAudioTrack *self,
                                              RteError *err);

AGORA_RTE_API_C bool RteRemoteAudioTrackGetConfigs(
    RteRemoteAudioTrack *self, RteRemoteAudioTrackConfig *config,
    RteError *err);
    
AGORA_RTE_API_C bool RteRemoteAudioTrackSetConfigs(
    RteRemoteAudioTrack *self, RteRemoteAudioTrackConfig *config,
    RteError *err);

AGORA_RTE_API_C bool RteRemoteAudioTrackEnableLoopback(RteRemoteAudioTrack *self, RteError *err);

AGORA_RTE_API_C bool RteRemoteAudioTrackRegisterTrackObserver(
    RteRemoteAudioTrack *self, RteRemoteAudioTrackObserver *observer,
    void (*destroyer)(RteRemoteAudioTrackObserver *self, RteError *err),
    RteError *err);
AGORA_RTE_API_C bool RteRemoteAudioTrackUnregisterTrackObserver(
    RteRemoteAudioTrack *self, RteRemoteAudioTrackObserver *observer,
    RteError *err);

#ifdef __cplusplus
}
#endif  // __cplusplus
