/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "rte_base/c/handle.h"
#include "rte_base/c/common.h"
#include "rte_base/c/track/track.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


typedef struct RteAudioTrackConfig {
  RteTrackConfig track_config;
} RteAudioTrackConfig;

typedef struct RteAudioTrackInfo {
  RteTrackInfo track_info;
  // ... more fields.
} RteAudioTrackInfo;


typedef struct RteAudioTrackObserver {
  RteTrackObserver track_observer;

  // void (*on_event)(RteAudioTrackObserver *self, RteAudioTrackEvent event);
  // void (*on_frame)(RteAudioTrackObserver *self, RteAudioFrame *audio_frame);

} RteAudioTrackObserver;

AGORA_RTE_API_C void RteAudioTrackConfigInit(RteAudioTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteAudioTrackConfigDeinit(RteAudioTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteAudioTrackConfigCopy(RteAudioTrackConfig *dest, const RteAudioTrackConfig *src, RteError *err);

AGORA_RTE_API_C void RteAudioTrackInfoInit(RteAudioTrackInfo *info, RteError *err);
AGORA_RTE_API_C void RteAudioTrackInfoDeinit(RteAudioTrackInfo *info, RteError *err);
AGORA_RTE_API_C void RteAudioTrackInfoCopy(RteAudioTrackInfo *dest, const RteAudioTrackInfo *src, RteError *err);

AGORA_RTE_API_C void RteAudioTrackInit(RteAudioTrack *self, RteError *err);
AGORA_RTE_API_C void RteAudioTrackDeinit(RteAudioTrack *self, RteError *err);


AGORA_RTE_API_C bool RteAudioTrackRegisterTrackObserver(RteAudioTrack *self, RteAudioTrackObserver *observer, RteError *err);
AGORA_RTE_API_C bool RteAudioTrackUnregisterTrackObserver(RteAudioTrack *self, RteAudioTrackObserver *observer, RteError *err);


AGORA_RTE_API_C RteAudioTrackObserver *RteAudioTrackObserverCreate(RteError *err);
AGORA_RTE_API_C void RteAudioTrackObserverDestroy(RteAudioTrackObserver *self, RteError *err);


#ifdef __cplusplus
}
#endif  // __cplusplus
