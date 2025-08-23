/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "handle.h"
#include "common.h"
#include "./stream/stream.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

AGORA_RTE_API_C Rte RteGetFromBridge(RteError *err);

AGORA_RTE_API_C void RteChannelAndStreamGetFromBridge(
    Rte *rte, const char *channel_name, int uid, RteChannel *channel,
    RteStream *stream, RteError *error);

AGORA_RTE_API_C void RteChannelGetFromBridge(
        Rte *rte, const char *channel_name, int uid, 
        void(*cb)(Rte *self, RteChannel *channel, RteLocalUser *localUser, void *cb_data, RteError *error),
        void *cb_data);

#ifdef __cplusplus
}
#endif  // __cplusplus
