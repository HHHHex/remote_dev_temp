/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "rte_base/c/handle.h"
#include "rte_base/c/common.h"
#include "rte_base/c/track/local_audio_track.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef enum RteVoiceBeautifierPreset {
  kRteVoiceBeautifierPresetOff,
  kRteVoiceBeautifierPresetChatBeautifierMagnetic,
  kRteVoiceBeautifierPresetChatBeautifierFresh,
  kRteVoiceBeautifierPresetChatBeautifierVitality,
  kRteVoiceBeautifierPresetSingingBeautifier,
  kRteVoiceBeautifierPresetTimbreTransformationVigorous,
  kRteVoiceBeautifierPresetTimbreTransformationDeep,
  kRteVoiceBeautifierPresetTimbreTransformationMellow,
  kRteVoiceBeautifierPresetTimbreTransformationFalsetto,
  kRteVoiceBeautifierPresetTimbreTransformationFull,
  kRteVoiceBeautifierPresetTimbreTransformationClear,
  kRteVoiceBeautifierPresetTimbreTransformationResounding,
  kRteVoiceBeautifierPresetTimbreTransformationRinging,
  kRteVoiceBeautifierPresetUltraHighQualityVoice
} RteVoiceBeautifierPreset;

typedef enum RteAudioEffectPreset {
  kRteAudioEffectPresetOff,
  kRteAudioEffectPresetKtv,
  kRteAudioEffectPresetVocalConcert,
  kRteAudioEffectPresetStudio,
  kRteAudioEffectPresetPhonograph,
  kRteAudioEffectPresetVirtualStereo,
  kRteAudioEffectPresetSpecial,
  kRteAudioEffectPresetEthereal,
  kRteAudioEffectPresetAcoustics3DVoice,
  kRteAudioEffectPresetVirtualSurroundSound,
  kRteAudioEffectPresetVoiceChangerEffectUncle,
  kRteAudioEffectPresetVoiceChangerEffectOldMan,
  kRteAudioEffectPresetVoiceChangerEffectBoy,
  kRteAudioEffectPresetVoiceChangerEffectSister,
  kRteAudioEffectPresetVoiceChangerEffectGirl,
  kRteAudioEffectPresetVoiceChangerEffectPigKing,
  kRteAudioEffectPresetVoiceChangerEffectHulk,
  kRteAudioEffectPresetStyleTransformationRnb,
  kRteAudioEffectPresetStyleTransformationPopular,
  kRteAudioEffectPresetPitchCorrection
} RteAudioEffectPreset;

typedef enum RteVoiceConversionPreset {
  kRteVoiceConversionPresetOff,
  kRteVoiceConversionPresetNeutral,
  kRteVoiceConversionPresetSweet,
  kRteVoiceConversionPresetSolid,
  kRteVoiceConversionPresetBass,
  kRteVoiceConversionPresetCartoon,
  kRteVoiceConversionPresetChildlike,
  kRteVoiceConversionPresetPhoneOperator,
  kRteVoiceConversionPresetMonster,
  kRteVoiceConversionPresetTransformers,
  kRteVoiceConversionPresetGroot,
  kRteVoiceConversionPresetDarthVader,
  kRteVoiceConversionPresetIronLady,
  kRteVoiceConversionPresetShinChan,
  kRteVoiceConversionPresetGirlishMan,
  kRteVoiceConversionPresetChipMunk
} RteVoiceConversionPreset;

typedef struct RteMicAudioTrackConfig {
  RteLocalAudioTrackConfig local_audio_track_config;
  uint32_t recording_volume;
  bool _recording_volume_is_set;
  RteString* json_parameter;
} RteMicAudioTrackConfig;

typedef struct RteMicAudioTrackInfo {
  RteLocalAudioTrackInfo local_audio_track_info;
} RteMicAudioTrackInfo;

// @{ 
// Config
AGORA_RTE_API_C void RteMicAudioTrackConfigInit(RteMicAudioTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteMicAudioTrackConfigDeinit(RteMicAudioTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteMicAudioTrackConfigCopy(RteMicAudioTrackConfig *dest, const RteMicAudioTrackConfig *src, RteError *err);
AGORA_RTE_API_C void RteMicAudioTrackConfigSetRecordingVolume(RteMicAudioTrackConfig *self, uint32_t volume, RteError *err);
AGORA_RTE_API_C void RteMicAudioTrackConfigGetRecordingVolume(RteMicAudioTrackConfig *self, uint32_t *volume, RteError *err);
AGORA_RTE_API_C void RteMicAudioTrackConfigSetJsonParameter(RteMicAudioTrackConfig *self, RteString *json_parameter, RteError *err);
AGORA_RTE_API_C void RteMicAudioTrackConfigGetJsonParameter(RteMicAudioTrackConfig *self, RteString *json_parameter, RteError *err);
//}

// @{
// Info
AGORA_RTE_API_C void RteMicAudioTrackInfoInit(RteMicAudioTrackInfo *self,
                                             RteError *err);
AGORA_RTE_API_C void RteMicAudioTrackInfoDeinit(RteMicAudioTrackInfo *self,
                                               RteError *err);
AGORA_RTE_API_C void RteMicAudioTrackInfoCopy(RteMicAudioTrackInfo *dest, const RteMicAudioTrackInfo *src, RteError *err);
//}

AGORA_RTE_API_C RteMicAudioTrack RteMicAudioTrackCreate(
    Rte *self, RteMicAudioTrackConfig *config, RteError *err);
AGORA_RTE_API_C void RteMicAudioTrackDestroy(RteMicAudioTrack *self,
                                            RteError *err);

AGORA_RTE_API_C bool RteMicAudioTrackGetConfigs(RteMicAudioTrack *self,
                                               RteMicAudioTrackConfig *config,
                                               RteError *err);
AGORA_RTE_API_C bool RteMicAudioTrackSetConfigs(RteMicAudioTrack *self,
                                               RteMicAudioTrackConfig *config,
                                               RteError *err);

AGORA_RTE_API_C void RteMicAudioTrackSetVoiceBeautifierPreset(
    RteMicAudioTrack *self, RteVoiceBeautifierPreset preset,
    void (*cb)(RteMicAudioTrack* track, void* cb_data, RteError *err), void *cb_data);
AGORA_RTE_API_C void RteMicAudioTrackSetAudioEffectPreset(
    RteMicAudioTrack *self, RteAudioEffectPreset preset,
    void (*cb)(RteMicAudioTrack* track, void* cb_data, RteError *err), void *cb_data);
AGORA_RTE_API_C void RteMicAudioTrackSetVoiceConversionPreset(
    RteMicAudioTrack *self, RteVoiceConversionPreset preset,
    void (*cb)(RteMicAudioTrack* track, void* cb_data, RteError *err), void *cb_data);

AGORA_RTE_API_C bool RteMicAudioTrackGetInfo(RteMicAudioTrack *self,
                                            RteMicAudioTrackInfo *info,
                                            RteError *err);

#ifdef __cplusplus
}
#endif  // __cplusplus
