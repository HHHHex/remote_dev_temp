/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "rte_base/c/c_error.h"
#include "rte_base/c/common.h"
#include "rte_base/c/stream/stream.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct RteLocalStreamConfig {
  RteStreamConfig stream_config;
} RteLocalStreamConfig;

typedef struct RteLocalStreamStats {
  RteStreamStats stream_stats;
} RteLocalStreamStats;

typedef struct RteLocalStreamInfo {
  RteStreamInfo stream_info;
  bool has_subscribed;
} RteLocalStreamInfo;


typedef struct RteLocalStreamObserver {
  RteStreamObserver stream_observer;
} RteLocalStreamObserver;


AGORA_RTE_API_C void RteLocalStreamConfigInit(RteLocalStreamConfig *config,
                                             RteError *err);

AGORA_RTE_API_C void RteLocalStreamConfigDeinit(RteLocalStreamConfig *config,
                                               RteError *err);

AGORA_RTE_API_C void RteLocalStreamConfigCopy(RteLocalStreamConfig *dest, const RteLocalStreamConfig *src, RteError *err);


AGORA_RTE_API_C void RteLocalStreamStatsInit(RteLocalStreamStats *stats,
                                            RteError *err);

AGORA_RTE_API_C void RteLocalStreamStatsDeinit(RteLocalStreamStats *stats,
                                              RteError *err);


AGORA_RTE_API_C void RteLocalStreamInfoInit(RteLocalStreamInfo *info,
                                           RteError *err);

AGORA_RTE_API_C void RteLocalStreamInfoDeinit(RteLocalStreamInfo *info,
                                             RteError *err);



AGORA_RTE_API_C void RteLocalStreamInit(RteLocalStream *self, RteError *err);

AGORA_RTE_API_C void RteLocalStreamDeinit(RteLocalStream *self, RteError *err);

AGORA_RTE_API_C bool RteLocalStreamGetConfigs(RteLocalStream *self, RteLocalStreamConfig *config, RteError *err);

AGORA_RTE_API_C bool RteLocalStreamSetConfigs(RteLocalStream *self, RteLocalStreamConfig *config, RteError *err);

AGORA_RTE_API_C void RteLocalStreamGetStats(RteLocalStream *self, void (*cb)(RteLocalStream *stream, RteLocalStreamStats *stats, void *cb_data, RteError *err), void *cb_data);

AGORA_RTE_API_C bool RteLocalStreamGetInfo(RteLocalStream *self, RteLocalStreamInfo *info, RteError *err);

AGORA_RTE_API_C bool RteLocalStreamRegisterObserver(RteLocalStream *self, RteLocalStreamObserver *observer, RteError *err);

AGORA_RTE_API_C bool RteLocalStreamUnregisterObserver(RteLocalStream *self, RteLocalStreamObserver *observer, RteError *err);




#ifdef __cplusplus
}
#endif  // __cplusplus
