/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include "rte_base/c/c_error.h"
#include "rte_base/c/handle.h"
#include "rte_base/c/info.h"
#include "rte_base/c/metadata.h"
#include "rte_base/c/observer.h"
#include "rte_base/c/common.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus



typedef struct RteUserConfig {
    char placeholder;
} RteUserConfig;

typedef struct RteUserInfo {
  RteBaseInfo base_info;
  RteString *user_id;
} RteUserInfo;

typedef struct RteUserObserver {
  RteBaseObserver base_observer;
} RteUserObserver;


// @{
// Config
AGORA_RTE_API_C void RteUserConfigInit(RteUserConfig *config, RteError *err);
AGORA_RTE_API_C void RteUserConfigDeinit(RteUserConfig *config, RteError *err);
AGORA_RTE_API_C void RteUserConfigCopy(RteUserConfig *self,
                                         RteUserConfig *other,
                                         RteError *err);
// @}

// @{
// Info
AGORA_RTE_API_C void RteUserInfoInit(RteUserInfo *info, RteError *err);
AGORA_RTE_API_C void RteUserInfoDeinit(RteUserInfo *info, RteError *err);
AGORA_RTE_API_C void RteUserInfoCopy(RteUserInfo *self,
                                         RteUserInfo *other,
                                         RteError *err);
// @}

// @{
// Observer
AGORA_RTE_API_C RteUserObserver *RteUserObserverCreate(RteError *err);
AGORA_RTE_API_C void RteUserObserverDestroy(RteUserObserver *self,
                                                RteError *err);
AGORA_RTE_API_C RteUser
RteUserObserverGetEventSrc(RteUserObserver *self, RteError *err);
// @}

AGORA_RTE_API_C void RteUserConfigSetJsonParameter(RteUserConfig *self,
                                                  RteString *json_parameter,
                                                  RteError *err);
AGORA_RTE_API_C void RteUserConfigGetJsonParameter(RteUserConfig *self,
                                                  RteString *json_parameter,
                                                  RteError *err);

AGORA_RTE_API_C void RteUserInit(RteUser *self, RteUserConfig *config,
                                RteError *err);
AGORA_RTE_API_C void RteUserDeinit(RteUser *self, RteError *err);

AGORA_RTE_API_C bool RteUserGetConfigs(RteUser *self, RteUserConfig *config,
                                      RteError *err);
AGORA_RTE_API_C bool RteUserSetConfigs(RteUser *self, RteUserConfig *config,
                                      RteError *err);
AGORA_RTE_API_C bool RteUserGetInfo(RteUser *self, RteUserInfo *info, RteError *err);
AGORA_RTE_API_C bool RteUserRegisterObserver(
    RteUser *self, RteUserObserver *observer,
    RteError *err);
AGORA_RTE_API_C bool RteUserUnregisterObserver(RteUser *self,
                                              RteUserObserver *observer,
                                              RteError *err);

#ifdef __cplusplus
}
#endif  // __cplusplus
