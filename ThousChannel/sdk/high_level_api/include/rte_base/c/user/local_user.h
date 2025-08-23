/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "rte_base/c/handle.h"
#include "rte_base/c/common.h"
#include "rte_base/c/user/user.h"
#include "rte_base/c/utils/buf.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef enum RteLocalUserLinkState {
  /**
   * The initial state.
   */
  kRteLocalUserLinkStateIdle = 0,
  /**
   * The SDK is connecting to the server.
   */
  kRteLocalUserLinkStateConnecting = 1,
  /**
   * The SDK has connected to the server.
   */
  kRteLocalUserLinkStateConnected = 2,
  /**
   * The SDK is disconnected from the server.
   */
  kRteLocalUserLinkStateDisconnected = 3,
  /**
   * The SDK link is suspended.
   */
  kRteLocalUserLinkStateSuspended = 4,
  /**
   * The SDK is failed to connect to the server.
   */
  kRteLocalUserLinkStateFailed = 5
} RteLocalUserLinkState;


typedef enum RteLocalUserLinkStateChangedReason {
  /**
   * Unknown reason.
   */
  kRteLocalUserLinkStateChangedReasonUnknown = 0,
  /**
   * Connect.
   */
  kRteLocalUserLinkStateChangedReasonConnect = 1,
  /**
   * Connect success.
   */
  kRteLocalUserLinkStateChangedReasonConnectSuccess = 2,
  /**
   * Connect timeout.
   */
  kRteLocalUserLinkStateChangedReasonConnectTimeout = 3,
  /**
   * Connect not authorized.
   */
  kRteLocalUserLinkStateChangedReasonConnectNotAuthorized = 4,
  /**
   * Connect rejected.
   */
  kRteLocalUserLinkStateChangedReasonConnectRejected = 5,
  /**
   * Re-connect.
   */
  kRteLocalUserLinkStateChangedReasonReconnect = 6,
  /**
   * Disconnect.
   */
  kRteLocalUserLinkStateChangedReasonDisconnect = 7,
  /**
   * Auto reconnect.
   */
  kRteLocalUserLinkStateChangedReasonAutoReconnect = 8,
  /**
   * Reconnect timeout.
   */
  kRteLocalUserLinkStateChangedReasonReconnectTimeout = 9,
  /**
   * Reconnect success.
   */
  kRteLocalUserLinkStateChangedReasonReconnectSuccess = 10,
  /**
   * Join.
   */
  kRteLocalUserLinkStateChangedReasonJoin = 11,
  /**
   * Join success.
   */
  kRteLocalUserLinkStateChangedReasonJoinSuccess = 12,
  /**
   * Join failed.
   */
  kRteLocalUserLinkStateChangedReasonJoinFailed = 13,
  /**
   * Rejoin.
   */
  kRteLocalUserLinkStateChangedReasonRejoin = 14,
  /**
   * Leave.
   */
  kRteLocalUserLinkStateChangedReasonLeave = 15,
  /**
   * Invalid token.
   */
  kRteLocalUserLinkStateChangedReasonInvalidToken = 16,
  /**
   * Token expired.
   */
  kRteLocalUserLinkStateChangedReasonTokenExpired = 17,
  /**
   * Inconsistent app ID.
   */
  kRteLocalUserLinkStateChangedReasonInconsistentAppId = 18,
  /**
   * Invalid channel name.
   */
  kRteLocalUserLinkStateChangedReasonInvalidChannelName = 19,
  /**
   * Invalid user ID.
   */
  kRteLocalUserLinkStateChangedReasonInvalidUserId = 20,
  /**
   * Not initialized.
   */
  kRteLocalUserLinkStateChangedReasonNotInitialized = 21,
  /**
   * Rtm service not connected.
   */
  kRteLocalUserLinkStateChangedReasonRtmServiceNotConnected = 22,
  /**
   * Channel instance exceed limitation.
   */
  kRteLocalUserLinkStateChangedReasonChannelInstanceExceedLimitation = 23,
  /**
   * Operation rate exceed limitation.
   */
  kRteLocalUserLinkStateChangedReasonOperationRateExceedLimitation = 24,
  /**
   * Channel in error state.
   */
  kRteLocalUserLinkStateChangedReasonChannelInErrorState = 25,
  /**
   * Presence not connected.
   */
  kRteLocalUserLinkStateChangedReasonPresenceNotConnected = 26,
  /**
   * Same UID login.
   */
  kRteLocalUserLinkStateChangedReasonSameUidLogin = 27,
  /**
   * Kicked out by server.
   */
  kRteLocalUserLinkStateChangedReasonKickedOutByServer = 28,
  /**
   * Keep alive timeout.
   */
  kRteLocalUserLinkStateChangedReasonKeepAliveTimeout = 29,
  /**
   * Connection error.
   */
  kRteLocalUserLinkStateChangedReasonConnectionError = 30,
  /**
   * Presence not ready.
   */
  kRteLocalUserLinkStateChangedReasonPresenceNotReady = 31,
  /**
   * Network change.
   */
  kRteLocalUserLinkStateChangedReasonNetworkChange = 32,
  /**
   * Service not supported.
   */
  kRteLocalUserLinkStateChangedReasonServiceNotSupported = 33,
  /**
   * Stream channel not available.
   */
  kRteLocalUserLinkStateChangedReasonStreamChannelNotAvailable = 34,
  /**
   * Storage not available.
   */
  kRteLocalUserLinkStateChangedReasonStorageNotAvailable = 35,
  /**
   * Lock not available.
   */
  kRteLocalUserLinkStateChangedReasonLockNotAvailable = 36
} RteLocalUserLinkStateChangedReason;

typedef struct RteLocalUserConfig {
  RteUserConfig user_config;
  RteString *user_id;
  bool _user_id_is_set;
  RteString *user_token;
  bool _user_token_is_set;
  RteString *json_parameter;
  bool _json_parameter_is_set;
} RteLocalUserConfig;

typedef struct RteLocalUserInfo {
  RteUserInfo user_info;
} RteLocalUserInfo;

typedef struct RteLocalUserObserver RteLocalUserObserver;
struct RteLocalUserObserver {
  RteUserObserver base_observer;

  void (*on_user_metadata_snapshot_received)(RteLocalUserObserver *self, RteMetadata *metadata, RteMetadataInfo *metadata_info);
  void (*on_user_metadata_changed)(RteLocalUserObserver *self, RteMetadata *metadata, RteMetadataInfo *metadata_info);
  void (*on_token_privilege_will_expire)(RteLocalUserObserver *self);
  void (*on_link_state_event)(RteLocalUserObserver *self, RteLocalUserLinkState old_state, RteLocalUserLinkState new_state, RteLocalUserLinkStateChangedReason reason, RteError *err);
  void (*on_user_message_received)(RteLocalUserObserver *self,
                                   RteString *publisher, RteBuf *message);
  void (*on_renew_token_result)(RteLocalUserObserver *self, RteError *err);
};

// @{
// Config
AGORA_RTE_API_C void RteLocalUserConfigInit(RteLocalUserConfig *config,
                                           RteError *err);
AGORA_RTE_API_C void RteLocalUserConfigDeinit(RteLocalUserConfig *config,
                                             RteError *err);
AGORA_RTE_API_C void RteLocalUserConfigCopy(RteLocalUserConfig *self,
                                           RteLocalUserConfig *other,
                                           RteError *err);

AGORA_RTE_API_C void RteLocalUserConfigSetUserId(RteLocalUserConfig *self,
                                                RteString *user_id,
                                                RteError *err);
AGORA_RTE_API_C void RteLocalUserConfigGetUserId(RteLocalUserConfig *self,
                                                RteString *user_id,
                                                RteError *err);

AGORA_RTE_API_C void RteLocalUserConfigSetUserToken(RteLocalUserConfig *self,
                                                   RteString *user_token,
                                                   RteError *err);
AGORA_RTE_API_C void RteLocalUserConfigGetUserToken(RteLocalUserConfig *self,
                                                   RteString *user_token,
                                                   RteError *err);

AGORA_RTE_API_C void RteLocalUserConfigSetJsonParameter(
    RteLocalUserConfig *self, RteString *json_parameter, RteError *err);
AGORA_RTE_API_C void RteLocalUserConfigGetJsonParameter(
    RteLocalUserConfig *self, RteString *json_parameter, RteError *err);
// @}

// @{
// Info
AGORA_RTE_API_C void RteLocalUserInfoInit(RteLocalUserInfo *info, RteError *err);
AGORA_RTE_API_C void RteLocalUserInfoDeinit(RteLocalUserInfo *info,
                                           RteError *err);
AGORA_RTE_API_C void RteLocalUserInfoCopy(RteLocalUserInfo *self,
                                           RteLocalUserInfo *other,
                                           RteError *err);
// @}

// @{
// Observer
AGORA_RTE_API_C RteLocalUserObserver *RteLocalUserObserverCreate(RteError *err);
AGORA_RTE_API_C void RteLocalUserObserverDestroy(RteLocalUserObserver *self,
                                                RteError *err);

AGORA_RTE_API_C RteLocalUser
RteLocalUserObserverGetEventSrc(RteLocalUserObserver *self, RteError *err);
// @}

AGORA_RTE_API_C RteLocalUser RteLocalUserCreate(Rte *self, RteLocalUserConfig *config,
                                RteError *err);
AGORA_RTE_API_C void RteLocalUserDestroy(RteLocalUser *self, RteError *err);

AGORA_RTE_API_C bool RteLocalUserGetConfigs(RteLocalUser *self,
                                           RteLocalUserConfig *config,
                                           RteError *err);
AGORA_RTE_API_C bool RteLocalUserSetConfigs(
    RteLocalUser *self, RteLocalUserConfig *config, RteError *err);

AGORA_RTE_API_C bool RteLocalUserGetInfo(RteLocalUser *self,
                                        RteLocalUserInfo *info, RteError *err);

AGORA_RTE_API_C void RteLocalUserConnect(RteLocalUser *self,
                                      void (*cb)(RteLocalUser *self, void *cb_data, RteError *err),
                                      void *cb_data);
AGORA_RTE_API_C void RteLocalUserDisconnect(RteLocalUser *self,
                                       void (*cb)(RteLocalUser *self, void *cb_data, RteError *err),
                                       void *cb_data);
AGORA_RTE_API_C bool RteLocalUserIsConnected(RteLocalUser *self);

AGORA_RTE_API_C void RteLocalUserPublishMessage(
    RteLocalUser *self, const char *user_id, RteBuf *message,
    void (*cb)(RteLocalUser *self, void *cb_data, RteError *err),
    void *cb_data);

AGORA_RTE_API_C void RteLocalUserGetMetadata(RteLocalUser *self, const char *user_id,
                                       void (*cb)(RteLocalUser *self,
                                                  RteMetadata *items,
                                                  void *cb_data, RteError *err),
                                       void *cb_data);
AGORA_RTE_API_C void RteLocalUserSubscribeMetadata(
    RteLocalUser *self, const char *user_id,
    void (*cb)(RteLocalUser *self, void *cb_data, RteError *err), void *cb_data);
AGORA_RTE_API_C void RteLocalUserUnsubscribeMetadata(
    RteLocalUser *self, const char *user_id,
    void (*cb)(RteLocalUser *self, void *cb_data, RteError *err), void *cb_data);
AGORA_RTE_API_C void RteLocalUserSetMetadata(
    RteLocalUser *self, const char *user_id, RteMetadata *items,
    RteMetadataConfig *config,
    void (*cb)(RteLocalUser *self, void *cb_data, RteError *err), void *cb_data);
AGORA_RTE_API_C void RteLocalUserRemoveMetadata(
    RteLocalUser *self, const char *user_id, RteMetadata *items,
    RteMetadataConfig *config,
    void (*cb)(RteLocalUser *self, void *cb_data, RteError *err), void *cb_data);

AGORA_RTE_API_C bool RteLocalUserRegisterObserver(
    RteLocalUser *self, RteLocalUserObserver *observer, RteError *err);
AGORA_RTE_API_C bool RteLocalUserUnregisterObserver(
    RteLocalUser *self, RteLocalUserObserver *observer, RteError *err);



#ifdef __cplusplus
}
#endif  // __cplusplus
