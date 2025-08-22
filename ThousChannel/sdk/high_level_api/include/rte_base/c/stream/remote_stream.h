/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include <stddef.h>

#include "rte_base/c/c_error.h"
#include "rte_base/c/common.h"
#include "rte_base/c/stream/stream.h"
#include "rte_base/c/track/track.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct RteRemoteStreamConfig {
  RteStreamConfig stream_config;
} RteRemoteStreamConfig;

typedef struct RteRemoteStreamStats {
  RteStreamStats stream_stats;
} RteRemoteStreamStats;

typedef struct RteRemoteStreamInfo {
  RteStreamInfo stream_info;

  RteString *stream_id;

  bool has_audio;
  bool has_video;
  // bool has_data;
  // RteTrackSrc audio_track_src;
  // RteTrackSrc video_track_src;
  // RteTrackSrc audio_track_src_original;
  // RteTrackSrc video_track_src_original;
  // RteString *data_track_topics;
  // size_t data_track_topic_cnt;
} RteRemoteStreamInfo;


AGORA_RTE_API_C void RteRemoteStreamConfigInit(RteRemoteStreamConfig *config, RteError *err);
AGORA_RTE_API_C void RteRemoteStreamConfigDeinit(RteRemoteStreamConfig *config, RteError *err);
AGORA_RTE_API_C void RteRemoteStreamConfigCopy(RteRemoteStreamConfig *dst, const RteRemoteStreamConfig *src, RteError *err);

AGORA_RTE_API_C void RteRemoteStreamStatsInit(RteRemoteStreamStats *stats,
                                             RteError *err);
AGORA_RTE_API_C void RteRemoteStreamStatsDeinit(RteRemoteStreamStats *stats,
                                               RteError *err);
AGORA_RTE_API_C void RteRemoteStreamStatsCopy(RteRemoteStreamStats *dst, const RteRemoteStreamStats *src, RteError *err);

AGORA_RTE_API_C void RteRemoteStreamInfoInit(RteRemoteStreamInfo *info, RteError *err);
AGORA_RTE_API_C void RteRemoteStreamInfoDeinit(RteRemoteStreamInfo *info, RteError *err);
AGORA_RTE_API_C void RteRemoteStreamInfoCopy(RteRemoteStreamInfo *dst, const RteRemoteStreamInfo *src, RteError *err);
AGORA_RTE_API_C void RteRemoteStreamInfoSetStreamId(RteRemoteStreamInfo *info, RteString *stream_id, RteError *err);
AGORA_RTE_API_C void RteRemoteStreamInfoGetStreamId(RteRemoteStreamInfo *info, RteString *stream_id, RteError *err);

AGORA_RTE_API_C void RteRemoteStreamInit(RteRemoteStream *self, RteError *err);
AGORA_RTE_API_C void RteRemoteStreamDeinit(RteRemoteStream *self, RteError *err);
AGORA_RTE_API_C bool RteRemoteStreamGetConfigs(RteRemoteStream *self, RteRemoteStreamConfig *config, RteError *err);
AGORA_RTE_API_C bool RteRemoteStreamSetConfigs(RteRemoteStream *self, RteRemoteStreamConfig *config, RteError *err);
AGORA_RTE_API_C bool RteRemoteStreamGetInfo(RteRemoteStream *self, RteRemoteStreamInfo *info, RteError *err);
AGORA_RTE_API_C void RteRemoteStreamGetStats(RteRemoteStream *self, void (*cb)(RteRemoteStream *stream, RteRemoteStreamStats *stats, void *cb_data, RteError *err), void *cb_data);




#ifdef __cplusplus
}
#endif  // __cplusplus
