/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "rte_base/c/handle.h"
#include "rte_base/c/common.h"
#include "rte_base/c/c_error.h"
#include "rte_base/c/observer.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct RteStream RteStream;

typedef enum RteTrackMediaType {
  kRteTrackMediaTypeAudio,
  kRteTrackMediaTypeVideo,
  kRteTrackMediaTypeData
} RteTrackMediaType;

typedef enum RteTrackSrc {
  kRteTrackSrcUnknown,
  kRteTrackSrcMix,
  kRteTrackSrcNetwork,
  kRteTrackSrcMicrophone,
  kRteTrackSrcLoopbackRecording,
  kRteTrackSrcCamera,
  kRteTrackSrcScreen,
  kRteTrackSrcCustom,
} RteTrackSrc;

typedef enum RteTrackState {
  kRteTrackStateIdle,
  kRteTrackStateStreaming
} RteTrackState;

typedef enum RteTrackStateChangedReason {
  kRteTrackStateChangedReasonLocalStartCapturing,
} RteTrackStateChangedReason;

typedef struct RteTrackConfig {
  char placeholder;
  RteString *json_parameter;
} RteTrackConfig;

typedef struct RteTrackInfo {
  RteStream *stream;
} RteTrackInfo;

typedef struct RteTrackStats {
RteTrackSrc track_source;
RteTrackMediaType track_media_type;
} RteTrackStats;

typedef struct RteTrackObserver {
  RteBaseObserver base_observer;

  void (*on_track_state_changed)(RteTrackObserver *self, RteTrackState old_state, RteTrackState new_state, RteTrackStateChangedReason reason, RteError *err);
} RteTrackObserver;

// @{
// Config
AGORA_RTE_API_C void RteTrackConfigInit(RteTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteTrackConfigDeinit(RteTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteTrackConfigCopy(RteTrackConfig *dst, const RteTrackConfig *src, RteError *err);
//}

// @{
// Info
AGORA_RTE_API_C void RteTrackInfoInit(RteTrackInfo *info, RteError *err);
AGORA_RTE_API_C void RteTrackInfoDeinit(RteTrackInfo *info, RteError *err);
AGORA_RTE_API_C void RteTrackInfoCopy(RteTrackInfo *dst, const RteTrackInfo *src, RteError *err);
//}

// @{
// Stats
AGORA_RTE_API_C void RteTrackStatsInit(RteTrackStats *stats, RteError *err);
AGORA_RTE_API_C void RteTrackStatsDeinit(RteTrackStats *stats, RteError *err);
// }

// @{
// Track
AGORA_RTE_API_C void RteTrackInit(RteTrack *self, RteError *err);
AGORA_RTE_API_C void RteTrackDeinit(RteTrack *self, RteError *err);
AGORA_RTE_API_C void RteTrackGetStats(RteTrack *self, void (*cb)(RteTrack *track, RteTrackStats *stats, void *cb_data, RteError *err), void *cb_data);
AGORA_RTE_API_C bool RteTrackGetInfo(RteTrack *self, RteTrackInfo *info, RteError *err);
AGORA_RTE_API_C bool RteTrackRegisterObserver(RteTrack *self, RteTrackObserver *observer, RteError *err);
AGORA_RTE_API_C bool RteTrackUnregisterObserver(RteTrack *self, RteTrackObserver *observer, RteError *err);
// }

#ifdef __cplusplus
}
#endif  // __cplusplus
