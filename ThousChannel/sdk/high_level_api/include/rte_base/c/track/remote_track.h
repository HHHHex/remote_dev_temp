#pragma once

/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#include "rte_base/c/handle.h"
#include "rte_base/c/common.h"
#include "rte_base/c/c_error.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


typedef struct RteRemoteTrackStats {
    char placeholder;
} RteRemoteTrackStats;

typedef struct RteRemoteTrackConfig {
    char placeholder;
} RteRemoteTrackConfig;

typedef struct RteRemoteTrackInfo {
    char placeholder;
} RteRemoteTrackInfo;

typedef struct RteRemoteTrackObserver {
    char placeholder;
} RteRemoteTrackObserver;

// @{
// Stats
AGORA_RTE_API_C void RteRemoteTrackStatsInit(RteRemoteTrackStats *stats, RteError *err);
AGORA_RTE_API_C void RteRemoteTrackStatsDeinit(RteRemoteTrackStats *stats, RteError *err);
//}

// @{
// Config
AGORA_RTE_API_C void RteRemoteTrackConfigInit(RteRemoteTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteRemoteTrackConfigDeinit(RteRemoteTrackConfig *config, RteError *err);
//}

// @{
// Info
AGORA_RTE_API_C void RteRemoteTrackInfoInit(RteRemoteTrackInfo *info,
                                           RteError *err);
AGORA_RTE_API_C void RteRemoteTrackInfoDeinit(RteRemoteTrackInfo *info,
                                             RteError *err);
//}


AGORA_RTE_API_C void RteRemoteTrackInit(RteRemoteTrack *self, RteError *err);
AGORA_RTE_API_C void RteRemoteTrackDeinit(RteRemoteTrack *self, RteError *err);


AGORA_RTE_API_C bool RteRemoteTrackGetInfo(RteRemoteTrack *self,
                                          RteRemoteTrackInfo *info,
                                          RteError *err);

AGORA_RTE_API_C bool RteRemoteTrackRegisterObserver(RteRemoteTrack *self, 
                                                   RteRemoteTrackObserver *observer,
                                                   RteError *err);
AGORA_RTE_API_C bool RteRemoteTrackUnregisterObserver(RteRemoteTrack *self,
                                                     RteRemoteTrackObserver *observer, 
                                                     RteError *err);



#ifdef __cplusplus
}
#endif  // __cplusplus
