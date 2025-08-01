/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "handle.h"
#include "common.h"
#include "metadata.h"
#include "observer.h"
#include "utils/string.h"
#include "track/track.h"
#include "utils/buf.h"
#include "user/remote_user.h"
#include "stream/remote_stream.h"
#include "rte_base/c/utils/audio_utility_types.h"
#include "utils/audio_utility_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct RteTrack RteTrack;
typedef struct RteUser RteUser;
typedef struct RteStream RteStream;
typedef struct Rte Rte;
typedef struct RteLocalStream RteLocalStream;
typedef struct RteLocalStreamInfo RteLocalStreamInfo;
typedef struct RteRemoteStream RteRemoteStream;

typedef enum RteChannelProfile {
  kRteChannelProfileInteractive = 0,
  kRteChannelProfileLargeScale = 1,
} RteChannelProfile;

typedef enum RteChannelConnectionState {
  kRteChannelConnectionStateDisconnected,
  kRteChannelConnectionStateDisconnecting,
  kRteChannelConnectionStateConnecting,
  kRteChannelConnectionStateConnected,
  kRteChannelConnectionStateReconnecting,
  kRteChannelConnectionStateFailed
} RteChannelConnectionState;

typedef enum RteChannelConnectionStateChangedReason {
  kRteChannelConnectionStateChangedReasonConnecting,
  kRteChannelConnectionStateChangedReasonSuccess,
  kRteChannelConnectionStateChangedReasonInterrupted,
  kRteChannelConnectionStateChangedReasonBannedByServer,
  kRteChannelConnectionStateChangedReasonJoinFailed,
  kRteChannelConnectionStateChangedReasonLeaveChannel,
  kRteChannelConnectionStateChangedReasonInvalidAppId,
  kRteChannelConnectionStateChangedReasonInvalidChannelName,
  kRteChannelConnectionStateChangedReasonInvalidToken,
  kRteChannelConnectionStateChangedReasonTokenExpired,
  kRteChannelConnectionStateChangedReasonRejectedByServer,
  kRteChannelConnectionStateChangedReasonSettingProxyServer,
  kRteChannelConnectionStateChangedReasonRenewToken,
  kRteChannelConnectionStateChangedReasonClientIpChanged,
  kRteChannelConnectionStateChangedReasonKeepAliveTimeout,
  kRteChannelConnectionStateChangedReasonRejoinSuccess,
  kRteChannelConnectionStateChangedReasonLost,
  kRteChannelConnectionStateChangedReasonEchoTest,
  kRteChannelConnectionStateChangedReasonClientIpChangedByUser,
  kRteChannelConnectionStateChangedReasonSameUidLogin,
  kRteChannelConnectionStateChangedReasonTooManyBroadcasters,
  kRteChannelConnectionStateChangedReasonLicenseValidationFailure,
  kRteChannelConnectionStateChangedReasonCertificationVerifyFailure,
  kRteChannelConnectionStateChangedReasonStreamChannelNotAvailable,
  kRteChannelConnectionStateChangedReasonInconsistentAppId
} RteChannelConnectionStateChangedReason;

typedef enum RteTrackSubState {
  kRteTrackSubStateSubscribing,
  kRteTrackSubStateSubscribed,
  kRteTrackSubStateNotSubscribed
} RteTrackSubState;

typedef enum RteTrackSubStateChangedReason {
  kRteTrackSubStateChangedReasonRemotePublished,
  kRteTrackSubStateChangedReasonRemoteUnpublished,
  kRteTrackSubStateChangedReasonLocalSubscribed,
  kRteTrackSubStateChangedReasonLocalUnsubscribed
} RteTrackSubStateChangedReason;

typedef enum RteTrackPubState {
  kRteTrackPubStatePublishing,
  kRteTrackPubStatePublished,
  kRteTrackPubStateNotPublished
} RteTrackPubState;

typedef enum RteTrackPubStateChangedReason {
  kRteTrackPubStateChangedReasonLocalPublished,
  kRteTrackPubStateChangedReasonLocalUnpublished
} RteTrackPubStateChangedReason;

typedef struct RtePresenceStateItem {
  RteString *key;
  RteString *value;
} RtePresenceStateItem;

typedef struct RtePresenceState {
  RteString *name;
  RtePresenceStateItem *items;
  size_t items_cnt;
} RtePresenceState;

typedef struct RteLock {
  RteString *lock_name;
  RteString *owner;
  uint32_t ttl;
} RteLock;

typedef enum RteLockChangedEvent {
  kRteLockChangedEventSet,
  kRteLockChangedEventRemoved,
  kRteLockChangedEventAcquired,
  kRteLockChangedEventReleased,
  kRteLockChangedEventExpired
} RteLockChangedEvent;

typedef struct RteChannelConfig {
  RteString *channel_id;
  bool _channel_id_is_set;

  bool allow_auto_subscribe_all;
  bool _allow_auto_subscribe_all_is_set;

  bool enable_audio_top_n;
  bool _enable_audio_top_n_is_set;

  bool auto_subscribe_message;
  bool _auto_subscribe_message_is_set;

  bool auto_subscribe_metadata;
  bool _auto_subscribe_metadata_is_set;

  bool auto_subscribe_user_presence;
  bool _auto_subscribe_user_presence_is_set;

  bool auto_subscribe_stream_presence;
  bool _auto_subscribe_stream_presence_is_set;

  bool be_quiet;
  bool _be_quiet_is_set;

  bool auto_subscribe_audio;
  bool _auto_subscribe_audio_is_set;

  bool auto_subscribe_video;
  bool _auto_subscribe_video_is_set;

  RteString *json_parameter;
  bool _json_parameter_is_set;
} RteChannelConfig;

typedef struct RteChannelObserver RteChannelObserver;
struct RteChannelObserver {
  RteBaseObserver base_observer;

  void (*on_remote_users_joined)(RteChannelObserver *self, RteRemoteUser *new_users, RteRemoteUserInfo *new_users_info, size_t new_users_cnt);
  void (*on_remote_users_left)(RteChannelObserver *self, RteRemoteUser *left_users, RteRemoteUserInfo *left_users_info, size_t left_users_cnt);
  void (*on_channel_message_received)(RteChannelObserver *self,
                                      RteString* publisher, RteBuf *message);
  void (*on_channel_metadata_snapshot_received)(RteChannelObserver *self, RteMetadata *metadata, RteMetadataInfo *metadata_info);
  void (*on_channel_metadata_changed)(RteChannelObserver *self, RteMetadata *metadata, RteMetadataInfo *metadata_info);
  void (*on_channel_user_presence_snapshot_received)(RteChannelObserver *self, RtePresenceState *states, size_t states_cnt);
  void (*on_channel_user_presence_changed)(RteChannelObserver *self, RtePresenceState *state);

  void (*on_remote_streams_added)(RteChannelObserver *self, RteRemoteStream *new_streams, RteRemoteStreamInfo *new_streams_info, size_t new_streams_cnt);
  void (*on_remote_streams_removed)(RteChannelObserver *self, RteRemoteStream *removed_streams, RteRemoteStreamInfo *removed_streams_info, size_t removed_streams_cnt);
  void (*on_local_stream_info)(RteChannelObserver *self, RteLocalStream *stream, RteLocalStreamInfo *info);
  void (*on_remote_stream_info)(RteChannelObserver *self, RteRemoteStream *stream, RteRemoteStreamInfo *info);
  void (*on_sub_state_changed)(RteChannelObserver *self, RteRemoteStream *stream, RteTrack *track, RteTrackMediaType track_media_type, 
                                RteTrackSubState old_state, RteTrackSubState new_state, RteTrackSubStateChangedReason reason, RteError *err);
  void (*on_pub_state_changed)(RteChannelObserver *self, RteLocalStream *stream, RteTrack *track, RteTrackMediaType track_media_type, 
                                RteTrackPubState old_state, RteTrackPubState new_state, RteTrackPubStateChangedReason reason, RteError *err);
  void (*on_active_speaker)(RteChannelObserver *self, RteStream *stream);
  void (*on_audio_volume_indication)(RteChannelObserver *self, RteAudioVolumeInfo *audio_volume_infos, size_t audio_volume_infos_cnt);
  void (*on_channel_token_will_expire)(RteChannelObserver *self, RteString *channel_token);
  void (*on_stream_token_will_expire)(RteChannelObserver *self, RteStream *stream, RteString *stream_token);
  void (*on_channel_token_expired)(RteChannelObserver *self);
  void (*on_stream_token_expired)(RteChannelObserver *self, RteStream *stream);
  void (*on_channel_stream_state_snapshot_received)(RteChannelObserver *self, RtePresenceState *states, size_t states_cnt);
  void (*on_channel_stream_state_changed)(RteChannelObserver *self, RtePresenceState *state);


  void (*on_subscribe_channel_user_result)(RteChannelObserver *self, RteError *err);
  void (*on_unsubscribe_channel_user_result)(RteChannelObserver *self, RteError *err);

  void (*on_subscribe_channel_stream_result)(RteChannelObserver *self, RteError *err);
  void (*on_unsubscribe_channel_stream_result)(RteChannelObserver *self, RteError *err);
};

typedef struct RteSubscribeOptions {
  RteTrackMediaType track_media_type;
  RteString *data_track_topic;
  RteCanvas *canvas;
} RteSubscribeOptions;


// @{
// StateItem
AGORA_RTE_API_C void RtePresenceStateItemInit(RtePresenceStateItem *self, RteError *err);
AGORA_RTE_API_C void RtePresenceStateItemDeinit(RtePresenceStateItem *self, RteError *err);
AGORA_RTE_API_C void RtePresenceStateItemCopy(RtePresenceStateItem *self, const RtePresenceStateItem *other, RteError *err);
AGORA_RTE_API_C void RtePresenceStateItemSetKey(RtePresenceStateItem *self, RteString *key, RteError *err);
AGORA_RTE_API_C void RtePresenceStateItemSetValue(RtePresenceStateItem *self, RteString *value, RteError *err);
AGORA_RTE_API_C void RtePresenceStateItemGetKey(RtePresenceStateItem *self, RteString *key, RteError *err);
AGORA_RTE_API_C void RtePresenceStateItemGetValue(RtePresenceStateItem *self, RteString *value, RteError *err);
// @}

// @{
// State
AGORA_RTE_API_C void RtePresenceStateInit(RtePresenceState *self, RteError *err);
AGORA_RTE_API_C void RtePresenceStateDeinit(RtePresenceState *self, RteError *err);
AGORA_RTE_API_C void RtePresenceStateCopy(RtePresenceState *self, const RtePresenceState *other, RteError *err);
AGORA_RTE_API_C void RtePresenceStateSetName(RtePresenceState *self, RteString *name, RteError *err);
AGORA_RTE_API_C void RtePresenceStateGetName(RtePresenceState *self, RteString *name, RteError *err);
AGORA_RTE_API_C void RtePresenceStateSetItems(RtePresenceState *self, RtePresenceStateItem *items, size_t count, RteError *err);
AGORA_RTE_API_C void RtePresenceStateGetItems(RtePresenceState *self, RtePresenceStateItem **items, size_t *count, RteError *err);
AGORA_RTE_API_C void RtePresenceStateClearItems(RtePresenceState *self, RteError *err);
// @}

// @{
// Config
AGORA_RTE_API_C void RteChannelConfigInit(RteChannelConfig *config,
                                         RteError *err);
AGORA_RTE_API_C void RteChannelConfigDeinit(RteChannelConfig *config,
                                           RteError *err);
AGORA_RTE_API_C void RteChannelConfigCopy(RteChannelConfig *self,
                                         RteChannelConfig *other,
                                         RteError *err);

AGORA_RTE_API_C void RteChannelConfigSetChannelId(RteChannelConfig *self,
                                                RteString *channel_id,
                                                 RteError *err);
AGORA_RTE_API_C void RteChannelConfigGetChannelId(RteChannelConfig *self,
                                                 RteString *channel_id,
                                                 RteError *err);

AGORA_RTE_API_C void RteChannelConfigSetAllowAutoSubscribeAll(RteChannelConfig *self,
                                                           bool allow_auto_subscribe_all,
                                                           RteError *err);
AGORA_RTE_API_C void RteChannelConfigGetAllowAutoSubscribeAll(RteChannelConfig *self,
                                                           bool *allow_auto_subscribe_all,
                                                           RteError *err);


AGORA_RTE_API_C void RteChannelConfigSetEnableAudioTopN(RteChannelConfig *self,
                                                   bool enable_audio_top_n,
                                                   RteError *err);
AGORA_RTE_API_C void RteChannelConfigGetEnableAudioTopN(RteChannelConfig *self,
                                                   bool *enable_audio_top_n,
                                                   RteError *err);

AGORA_RTE_API_C void RteChannelConfigSetAutoSubscribeMessage(
    RteChannelConfig *self, bool auto_subscribe_message, RteError *err);
AGORA_RTE_API_C void RteChannelConfigGetAutoSubscribeMessage(
    RteChannelConfig *self, bool *auto_subscribe_message, RteError *err);

AGORA_RTE_API_C void RteChannelConfigSetAutoSubscribeMetadata(
    RteChannelConfig *self, bool auto_subscribe_metadata, RteError *err);
AGORA_RTE_API_C void RteChannelConfigGetAutoSubscribeMetadata(
    RteChannelConfig *self, bool *auto_subscribe_metadata, RteError *err);

AGORA_RTE_API_C void RteChannelConfigSetAutoSubscribeUserPresence(
    RteChannelConfig *self, bool auto_subscribe_user_presence, RteError *err);  
AGORA_RTE_API_C void RteChannelConfigGetAutoSubscribeUserPresence(
    RteChannelConfig *self, bool *auto_subscribe_user_presence, RteError *err);

AGORA_RTE_API_C void RteChannelConfigSetAutoSubscribeStreamPresence(
    RteChannelConfig *self, bool auto_subscribe_stream_presence, RteError *err);
AGORA_RTE_API_C void RteChannelConfigGetAutoSubscribeStreamPresence(
    RteChannelConfig *self, bool *auto_subscribe_stream_presence, RteError *err);

AGORA_RTE_API_C void RteChannelConfigSetBeQuiet(
    RteChannelConfig *self, bool be_quiet, RteError *err);
AGORA_RTE_API_C void RteChannelConfigGetBeQuiet(
    RteChannelConfig *self, bool *be_quiet, RteError *err);


AGORA_RTE_API_C void RteChannelConfigSetAutoSubscribeAudio(
    RteChannelConfig *self, bool auto_subscribe_audio, RteError *err);
AGORA_RTE_API_C void RteChannelConfigGetAutoSubscribeAudio(
    RteChannelConfig *self, bool *auto_subscribe_audio, RteError *err);

AGORA_RTE_API_C void RteChannelConfigSetAutoSubscribeVideo(
    RteChannelConfig *self, bool auto_subscribe_video, RteError *err);
AGORA_RTE_API_C void RteChannelConfigGetAutoSubscribeVideo(
    RteChannelConfig *self, bool *auto_subscribe_video, RteError *err);

AGORA_RTE_API_C void RteChannelConfigSetJsonParameter(RteChannelConfig *self,
                                                     RteString *json_parameter,
                                                     RteError *err);
AGORA_RTE_API_C void RteChannelConfigGetJsonParameter(RteChannelConfig *self,
                                                     RteString *json_parameter,
                                                     RteError *err);
// @}

// @{
// Observer
AGORA_RTE_API_C RteChannelObserver *RteChannelObserverCreate(RteError *err);
AGORA_RTE_API_C void RteChannelObserverDestroy(RteChannelObserver *self,
                                              RteError *err);
AGORA_RTE_API_C RteChannel
RteChannelObserverGetEventSrc(RteChannelObserver *self, RteError *err);
// @}

// @{
// SubscribeOptions

AGORA_RTE_API_C void RteSubscribeOptionsInit(RteSubscribeOptions *self, RteError *err);
AGORA_RTE_API_C void RteSubscribeOptionsDeinit(RteSubscribeOptions *self, RteError *err);
AGORA_RTE_API_C void RteSubscribeOptionsSetDataTrackTopic(RteSubscribeOptions *self, RteString *topic, RteError *err);
AGORA_RTE_API_C void RteSubscribeOptionsGetDataTrackTopic(RteSubscribeOptions *self, RteString *topic, RteError *err);

//}

AGORA_RTE_API_C RteChannel RteChannelCreate(Rte *self, RteChannelConfig *config,
                                           RteError *err);
AGORA_RTE_API_C void RteChannelDestroy(RteChannel *channel, RteError *err);

AGORA_RTE_API_C bool RteChannelGetConfigs(RteChannel *self,
                                         RteChannelConfig *config,
                                         RteError *err);
AGORA_RTE_API_C bool RteChannelSetConfigs(
    RteChannel *self, RteChannelConfig *config, RteError *err);

AGORA_RTE_API_C void RteChannelPublishStream(
    RteChannel *self, RteLocalUser *user, RteLocalStream *stream,
    void (*cb)(RteChannel *self, RteLocalStream *stream, void *cb_data,
               RteError *err),
    void *cb_data);
AGORA_RTE_API_C void RteChannelUnpublishStream(
    RteChannel *self, RteLocalStream *stream,
    void (*cb)(RteChannel *self, RteLocalStream *stream, void *cb_data,
               RteError *err),
    void *cb_data);

AGORA_RTE_API_C void RteChannelSubscribeTrack(
    RteChannel *self, RteString *stream_id, RteSubscribeOptions *options,
    void (*cb)(RteChannel *self, RteTrack *track, void *cb_data, RteError *err),
    void *cb_data);

AGORA_RTE_API_C void RteChannelUnsubscribeTrack(RteChannel *self, RteString *stream_id, 
    RteSubscribeOptions *options, 
    void (*cb)(RteChannel *self, RteString *stream, RteSubscribeOptions *options, void *cb_data, RteError *err), void *cb_data);

AGORA_RTE_API_C bool RteChannelRegisterObserver(
    RteChannel *self, RteChannelObserver *observer, RteError *err);
AGORA_RTE_API_C bool RteChannelUnregisterObserver(RteChannel *self, RteChannelObserver *observer, RteError *err);

AGORA_RTE_API_C RteLocalUser RteChannelGetLocalUser(RteChannel *self, RteError *err);
AGORA_RTE_API_C size_t RteChannelGetRemoteUsersCount(RteChannel *self,
                                                    RteError *err);
AGORA_RTE_API_C bool RteChannelGetRemoteUsers(RteChannel *self,
                                             RteRemoteUser *remote_users,
                                             size_t start_idx,
                                             size_t remote_users_cnt,
                                             RteError *err);

AGORA_RTE_API_C bool RteChannelJoin(RteChannel *self, RteLocalUser *user,
                                   RteString *channel_token, RteError *err);
AGORA_RTE_API_C bool RteChannelLeave(RteChannel *self, RteError *err);

AGORA_RTE_API_C void RteChannelRenewToken(
    RteChannel *self, RteString *channel_token,
    void (*cb)(RteChannel *self, void *cb_data, RteError *err), void *cb_data);

AGORA_RTE_API_C void RteChannelPublishMessage(
    RteChannel *self, RteBuf *message,
    void (*cb)(RteChannel *self, void *cb_data, RteError *err), void *cb_data);

AGORA_RTE_API_C void RteChannelGetMetadata(RteChannel *self,
                                          void (*cb)(RteChannel *self,
                                                     RteMetadata *items,
                                                     void *cb_data,
                                                     RteError *err),
                                          void *cb_data);
AGORA_RTE_API_C void RteChannelSetMetadata(RteChannel *self, RteMetadata *items, RteMetadataConfig *config, 
    void (*cb)(RteChannel *self, void *cb_data, RteError *err), void *cb_data);
AGORA_RTE_API_C void RteChannelRemoveMetadata(
    RteChannel *self, RteMetadata *items, RteMetadataConfig *config, 
    void (*cb)(RteChannel *self, void *cb_data, RteError *err), void *cb_data);

AGORA_RTE_API_C void RteChannelGetUserState(
    RteChannel *self, RteString *user_id,
    void (*cb)(RteChannel *self, RtePresenceState *state, void *cb_data, RteError *err),
    void *cb_data);
AGORA_RTE_API_C void RteChannelSetUserState(
    RteChannel *self, RtePresenceState *state,
    void (*cb)(RteChannel *self, void *cb_data, RteError *err), void *cb_data);
AGORA_RTE_API_C void RteChannelRemoveUserState(
    RteChannel *self, RteString **key, size_t count,
    void (*cb)(RteChannel *self, void *cb_data, RteError *err), void *cb_data);

AGORA_RTE_API_C void RteChannelSetLock(
    RteChannel *self, RteString *lock_name, uint32_t ttl,
    void (*cb)(RteChannel *self, void *cb_data, RteError *err), void *cb_data);
AGORA_RTE_API_C void RteChannelRemoveLock(
    RteChannel *self, RteString *lock_name,
    void (*cb)(RteChannel *self, void *cb_data, RteError *err), void *cb_data);
AGORA_RTE_API_C void RteChannelGetLocks(RteChannel *self,
                                       void (*cb)(RteChannel *self,
                                                  RteLock *locks,
                                                  size_t locks_cnt,
                                                  void *cb_data, RteError *err),
                                       void *cb_data);
AGORA_RTE_API_C void RteChannelAcquireLock(
    RteChannel *self, RteString *lock_name, bool retry,
    void (*cb)(RteChannel *self, RteString owner, void *cb_data, RteError *err),
    void *cb_data);
AGORA_RTE_API_C void RteChannelReleaseLock(
    RteChannel *self, RteString *lock_name,
    void (*cb)(RteChannel *self, void *cb_data, RteError *err), void *cb_data);

AGORA_RTE_API_C size_t RteChannelGetLocalStreamsCount(RteChannel *self,
                                                     RteError *err);
AGORA_RTE_API_C bool RteChannelGetLocalStreams(RteChannel *self,
                                              RteLocalStream *streams,
                                              size_t start_idx,
                                              size_t streams_cnt,
                                              RteError *err);

AGORA_RTE_API_C size_t RteChannelGetRemoteStreamsCount(RteChannel *self,
                                                      RteError *err);
AGORA_RTE_API_C bool RteChannelGetRemoteStreams(
    RteChannel *self, RteRemoteStream *streams,
    size_t start_idx, size_t streams_cnt, RteError *err);

#ifdef __cplusplus
}
#endif  // __cplusplus
