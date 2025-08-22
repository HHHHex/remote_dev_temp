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

typedef enum RteVideoTrackEvent {
  kRteVideoTrackEventFirstFrameRendered
} RteVideoTrackEvent;

typedef enum RteVideoPipelinePosition {
  kRteVideoPipelinePositionLocalPostCapturer,
  kRteVideoPipelinePositionLocalPostFilters,
  kRteVideoPipelinePositionLocalPreEncoder,
  kRteVideoPipelinePositionRemotePreRenderer
} RteVideoPipelinePosition;

typedef struct RteVideoTrackConfig {
  RteTrackConfig track_config;
} RteVideoTrackConfig;

typedef struct RteVideoTrackInfo {
  RteTrackInfo track_info;
  // ... more fields.
} RteVideoTrackInfo;


typedef struct RteVideoTrackObserver {
  RteBaseObserver base_observer;

  // void (*on_event)(RteVideoTrackObserver *self, RteVideoTrackEvent event);
  // void (*on_frame)(RteVideoTrackObserver *self, RteVideoFrame *video_frame);
  // ... more fields.
} RteVideoTrackObserver;


AGORA_RTE_API_C void RteVideoTrackConfigInit(RteVideoTrackConfig *self, RteError *err);
AGORA_RTE_API_C void RteVideoTrackConfigDeinit(RteVideoTrackConfig *self, RteError *err);
AGORA_RTE_API_C void RteVideoTrackConfigCopy(RteVideoTrackConfig *dst, const RteVideoTrackConfig *src, RteError *err);

AGORA_RTE_API_C void RteVideoTrackInfoInit(RteVideoTrackInfo *self, RteError *err);
AGORA_RTE_API_C void RteVideoTrackInfoDeinit(RteVideoTrackInfo *self, RteError *err);
AGORA_RTE_API_C void RteVideoTrackInfoCopy(RteVideoTrackInfo *dst, const RteVideoTrackInfo *src, RteError *err);

AGORA_RTE_API_C void RteVideoTrackInit(RteVideoTrack *self, RteError *err);
AGORA_RTE_API_C void RteVideoTrackDeinit(RteVideoTrack *self, RteError *err);

AGORA_RTE_API_C void RteVideoTrackSetCanvas(RteVideoTrack *self, RteCanvas *canvas,
                                   RteVideoPipelinePosition position,
                                   void (*cb)(RteVideoTrack *self,
                                              RteCanvas *canvas, void *cb_data,
                                              RteError *err),
                                   void *cb_data);

AGORA_RTE_API_C bool RteVideoTrackRegisterTrackObserver(RteVideoTrack *self, RteVideoTrackObserver *observer, RteError *err);
AGORA_RTE_API_C bool RteVideoTrackUnregisterTrackObserver(RteVideoTrack *self, RteVideoTrackObserver *observer, RteError *err);                           

AGORA_RTE_API_C RteVideoTrackObserver *RteVideoTrackObserverCreate(RteError *err);
AGORA_RTE_API_C void RteVideoTrackObserverDestroy(RteVideoTrackObserver *self, RteError *err);


#ifdef __cplusplus
}
#endif  // __cplusplus
