/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "rte_base/c/track/video_track.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct RteRemoteVideoTrackConfig {
  RteVideoTrackConfig video_track_config;
} RteRemoteVideoTrackConfig;

typedef struct RteRemoteVideoTrackInfo {
  RteVideoTrackInfo video_track_info;
  // ... more fields.
} RteRemoteVideoTrackInfo;

typedef struct RteRemoteVideoTrackObserver {
  RteVideoTrackObserver video_track_observer;
  // ... more fields.
} RteRemoteVideoTrackObserver;


AGORA_RTE_API_C void RteRemoteVideoTrackConfigInit(RteRemoteVideoTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteRemoteVideoTrackConfigDeinit(RteRemoteVideoTrackConfig *config, RteError *err);

AGORA_RTE_API_C void RteRemoteVideoTrackInfoInit(RteRemoteVideoTrackInfo *info, RteError *err);
AGORA_RTE_API_C void RteRemoteVideoTrackInfoDeinit(RteRemoteVideoTrackInfo *info, RteError *err);


AGORA_RTE_API_C void RteRemoteVideoTrackInit(RteRemoteVideoTrack *self, RteError *err);
AGORA_RTE_API_C void RteRemoteVideoTrackDeinit(RteRemoteVideoTrack *self, RteError *err);

AGORA_RTE_API_C void RteRemoteVideoTrackRegisterTrackObserver(RteRemoteVideoTrack *self, RteRemoteVideoTrackObserver *observer, void (*destroyer)(RteRemoteVideoTrackObserver *self), RteError *err);
AGORA_RTE_API_C void RteRemoteVideoTrackUnregisterTrackObserver(RteRemoteVideoTrack *self, RteRemoteVideoTrackObserver *observer, RteError *err);


AGORA_RTE_API_C RteRemoteVideoTrackObserver *RteRemoteVideoTrackObserverCreate(RteError *err);
AGORA_RTE_API_C void RteRemoteVideoTrackObserverDestroy(RteRemoteVideoTrackObserver *self, RteError *err);



#ifdef __cplusplus
}
#endif  // __cplusplus
