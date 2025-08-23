/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "rte_base/c/c_error.h"
#include "rte_base/c/common.h"
#include "rte_base/c/handle.h"
#include "rte_base/c/observer.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct RteLocalTrackInitialConfig {
    char placeholder;
} RteLocalTrackInitialConfig;

typedef struct RteLocalTrackConfig {
    char placeholder;
} RteLocalTrackConfig;

typedef struct RteLocalTrackInfo {
    char placeholder;
} RteLocalTrackInfo;

typedef struct RteLocalTrackObserver {
    RteBaseObserver base_observer;
} RteLocalTrackObserver;

typedef struct RteLocalTrackStats {
    char place_holder;
} RteLocalTrackStats;



AGORA_RTE_API_C void RteLocalTrackStatsInit(RteLocalTrackStats *stats, RteError *err);
AGORA_RTE_API_C void RteLocalTrackStatsDeinit(RteLocalTrackStats *stats, RteError *err);

AGORA_RTE_API_C void RteLocalTrackConfigInit(RteLocalTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteLocalTrackConfigDeinit(RteLocalTrackConfig *config, RteError *err);

// @{
// Info
AGORA_RTE_API_C void RteLocalTrackInfoInit(RteLocalTrackInfo *info, RteError *err);
AGORA_RTE_API_C void RteLocalTrackInfoDeinit(RteLocalTrackInfo *info, RteError *err);
//}


AGORA_RTE_API_C void RteLocalTrackInit(RteLocalTrack *self, RteError *err);
AGORA_RTE_API_C void RteLocalTrackDeinit(RteLocalTrack *self, RteError *err);
AGORA_RTE_API_C void RteLocalTrackStart(RteLocalTrack *self,
                                       void (*cb)(RteLocalTrack *self,
                                                  void *cb_data, RteError *err),
                                       void *cb_data);
AGORA_RTE_API_C void RteLocalTrackStop(RteLocalTrack *self,
                                      void (*cb)(RteLocalTrack *self,
                                                 void *cb_data, RteError *err),
                                      void *cb_data);

AGORA_RTE_API_C bool RteLocalTrackGetInfo(RteLocalTrack *self,
                                         RteLocalTrackInfo *info,
                                         RteError *err);

AGORA_RTE_API_C bool RteLocalTrackRegisterObserver(RteLocalTrack *self, RteLocalTrackObserver *observer, RteError *err);
AGORA_RTE_API_C bool RteLocalTrackUnregisterObserver(RteLocalTrack *self, RteLocalTrackObserver *observer, RteError *err);

#ifdef __cplusplus
}
#endif  // __cplusplus
