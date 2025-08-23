/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "rte_base/c/common.h"
#include "rte_base/c/stream/realtime_stream.h"
#include "rte_base/c/stream/remote_stream.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct Rte Rte;
typedef struct RteChannel RteChannel;

typedef struct RteRemoteRealTimeStreamConfig {
  RteRemoteStreamConfig remote_stream_config;
} RteRemoteRealTimeStreamConfig;

typedef struct RteRemoteRealTimeStreamInfo {
  RteRemoteStreamInfo remote_stream_info;
  // ... more fields
} RteRemoteRealTimeStreamInfo;

AGORA_RTE_API_C void RteRemoteRealTimeStreamConfigInit(RteRemoteRealTimeStreamConfig *config, RteError *err);
AGORA_RTE_API_C void RteRemoteRealTimeStreamConfigDeinit(RteRemoteRealTimeStreamConfig *config, RteError *err);
AGORA_RTE_API_C void RteRemoteRealTimeStreamConfigCopy(RteRemoteRealTimeStreamConfig *dst, const RteRemoteRealTimeStreamConfig *src, RteError *err);

AGORA_RTE_API_C void RteRemoteRealTimeStreamInfoInit(RteRemoteRealTimeStreamInfo *info, RteError *err);
AGORA_RTE_API_C void RteRemoteRealTimeStreamInfoDeinit(RteRemoteRealTimeStreamInfo *info, RteError *err);
AGORA_RTE_API_C void RteRemoteRealTimeStreamInfoCopy(RteRemoteRealTimeStreamInfo *dst, const RteRemoteRealTimeStreamInfo *src, RteError *err);
    

AGORA_RTE_API_C bool RteRemoteRealTimeStreamGetConfigs(RteRemoteRealTimeStream *self, RteRemoteRealTimeStreamConfig *config, RteError *err);
AGORA_RTE_API_C bool RteRemoteRealTimeStreamSetConfigs(RteRemoteRealTimeStream *self, RteRemoteRealTimeStreamConfig *config, RteError *err);

AGORA_RTE_API_C bool RteRemoteRealTimeStreamGetInfo(RteRemoteRealTimeStream *self, RteRemoteRealTimeStreamInfo *info, RteError *err);


#ifdef __cplusplus
}
#endif  // __cplusplus
