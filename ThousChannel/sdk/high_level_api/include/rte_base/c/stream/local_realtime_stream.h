/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "rte_base/c/common.h"
#include "rte_base/c/stream/local_stream.h"
#include "rte_base/c/stream/realtime_stream.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct Rte Rte;
typedef struct RteChannel RteChannel;

typedef struct RteLocalRealTimeStreamConfig {
  RteLocalStreamConfig local_stream_config;

  RteString *stream_token;
  bool _stream_token_is_set;

  RteString *json_parameter;
  bool _json_parameter_is_set;

} RteLocalRealTimeStreamConfig;


typedef struct RteLocalRealTimeStreamInfo {
  RteLocalStreamInfo local_stream_info;
  // ... more fields
} RteLocalRealTimeStreamInfo;


AGORA_RTE_API_C void RteLocalRealTimeStreamInfoInit(RteLocalRealTimeStreamInfo *info, RteError *err);

AGORA_RTE_API_C void RteLocalRealTimeStreamInfoDeinit(RteLocalRealTimeStreamInfo *info, RteError *err);

AGORA_RTE_API_C void RteLocalRealTimeStreamInfoCopy(RteLocalRealTimeStreamInfo *dest, const RteLocalRealTimeStreamInfo *src, RteError *err);


AGORA_RTE_API_C void RteLocalRealTimeStreamConfigInit(RteLocalRealTimeStreamConfig *config, RteError *err);

AGORA_RTE_API_C void RteLocalRealTimeStreamConfigDeinit(RteLocalRealTimeStreamConfig *config, RteError *err);

AGORA_RTE_API_C void RteLocalRealTimeStreamConfigCopy(RteLocalRealTimeStreamConfig *dest, const RteLocalRealTimeStreamConfig *src, RteError *err);



AGORA_RTE_API_C void RteLocalRealTimeStreamConfigSetStreamToken(RteLocalRealTimeStreamConfig *self, RteString *stream_token, RteError *err);

AGORA_RTE_API_C void RteLocalRealTimeStreamConfigGetStreamToken(RteLocalRealTimeStreamConfig *self, RteString *stream_token, RteError *err);

AGORA_RTE_API_C void RteLocalRealTimeStreamConfigSetJsonParameter(RteLocalRealTimeStreamConfig *self, RteString *json_parameter, RteError *err);

AGORA_RTE_API_C void RteLocalRealTimeStreamConfigGetJsonParameter(RteLocalRealTimeStreamConfig *self, RteString *json_parameter, RteError *err);


AGORA_RTE_API_C RteLocalRealTimeStream RteLocalRealTimeStreamCreate(
    Rte *rte, RteLocalRealTimeStreamConfig *config, RteError *err);

AGORA_RTE_API_C void RteLocalRealTimeStreamDestroy(RteLocalRealTimeStream *self,
                                                  RteError *err);

AGORA_RTE_API_C bool RteLocalRealTimeStreamGetConfigs(RteLocalRealTimeStream *self, RteLocalRealTimeStreamConfig *config, RteError *err);

AGORA_RTE_API_C bool RteLocalRealTimeStreamSetConfigs(RteLocalRealTimeStream *self, RteLocalRealTimeStreamConfig *config, RteError *err);

AGORA_RTE_API_C bool RteLocalRealTimeStreamGetInfo(RteLocalRealTimeStream *self, RteLocalRealTimeStreamInfo *info, RteError *err);



#ifdef __cplusplus
}
#endif  // __cplusplus
