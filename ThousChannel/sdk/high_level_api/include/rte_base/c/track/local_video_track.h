/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "rte_base/c/track/video_track.h"
#include "rte_base/c/utils/frame.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef enum RteLocalVideoTrackEvent {
} RteLocalVideoTrackEvent;

typedef struct RteLocalVideoTrackInitialConfig {
  // RteVideoTrackInitialConfig video_track_initial_config;
} RteLocalVideoTrackInitialConfig;

typedef struct RteLocalVideoTrackConfig {
  RteVideoTrackConfig video_track_config;
} RteLocalVideoTrackConfig;


typedef struct RteLocalVideoTrackInfo {
  RteVideoTrackInfo video_track_info;

  // ... more fields.
} RteLocalVideoTrackInfo;


typedef struct RteLocalVideoTrackObserver {
  RteVideoTrackObserver video_track_observer;
  
  void (*on_event)(RteLocalVideoTrackObserver *self, RteLocalVideoTrackEvent event);
  
  void (*on_frame)(RteLocalVideoTrackObserver *self, RteVideoFrame *video_frame);

} RteLocalVideoTrackObserver;


AGORA_RTE_API_C void RteLocalVideoTrackConfigInit(RteLocalVideoTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteLocalVideoTrackConfigDeinit(RteLocalVideoTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteLocalVideoTrackConfigCopy(RteLocalVideoTrackConfig *dst, const RteLocalVideoTrackConfig *src, RteError *err);

AGORA_RTE_API_C void RteLocalVideoTrackConfigSetJsonParameter(RteLocalVideoTrackConfig *self, RteString *json_parameter, RteError *err);
AGORA_RTE_API_C void RteLocalVideoTrackConfigGetJsonParameter(RteLocalVideoTrackConfig *self, RteString *json_parameter, RteError *err);


AGORA_RTE_API_C void RteLocalVideoTrackInfoInit(RteLocalVideoTrackInfo *info, RteError *err);
AGORA_RTE_API_C void RteLocalVideoTrackInfoDeinit(RteLocalVideoTrackInfo *info, RteError *err);
AGORA_RTE_API_C void RteLocalVideoTrackInfoCopy(RteLocalVideoTrackInfo *dst, const RteLocalVideoTrackInfo *src, RteError *err);


AGORA_RTE_API_C void RteLocalVideoTrackInit(RteLocalVideoTrack *self, RteError *err);
AGORA_RTE_API_C void RteLocalVideoTrackDeinit(RteLocalVideoTrack *self, RteError *err);

AGORA_RTE_API_C void RteLocalVideoTrackStart(RteLocalVideoTrack *self,
                                       void (*cb)(RteLocalVideoTrack *self,
                                                  void *cb_data, RteError *err),
                                       void *cb_data);
                                       
AGORA_RTE_API_C void RteLocalVideoTrackStop(RteLocalVideoTrack *self,
                                      void (*cb)(RteLocalVideoTrack *self,
                                                 void *cb_data, RteError *err),
                                      void *cb_data);

AGORA_RTE_API_C bool RteLocalVideoTrackRegisterTrackObserver(RteLocalVideoTrack *self, RteLocalVideoTrackObserver *observer, RteError *err);
AGORA_RTE_API_C bool RteLocalVideoTrackUnregisterTrackObserver(RteLocalVideoTrack *self, RteLocalVideoTrackObserver *observer, RteError *err);


AGORA_RTE_API_C RteLocalVideoTrackObserver *RteLocalVideoTrackObserverCreate(RteError *err);
AGORA_RTE_API_C void RteLocalVideoTrackObserverDestroy(RteLocalVideoTrackObserver *self, RteError *err);




#ifdef __cplusplus
}
#endif  // __cplusplus
