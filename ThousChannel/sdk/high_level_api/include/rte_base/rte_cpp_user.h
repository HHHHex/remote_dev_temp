/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2025 Agora IO. All rights reserved.
 *
 */
#pragma once

#include <string>
#include <functional>
#include "rte_base/c/user/user.h"
#include "rte_base/c/user/local_user.h"
#include "rte_base/c/user/remote_user.h"
#include "rte_base/c/handle.h"

#include "rte_cpp_rte.h"
#include "rte_cpp_error.h"
#include "rte_cpp_callback_utils.h"
#include "rte_cpp_string.h"
#include "rte_cpp_buffer.h"
#include "rte_cpp_metadata.h"

namespace rte {

using LocalUserLinkState = RteLocalUserLinkState;
using LocalUserLinkStateChangedReason = RteLocalUserLinkStateChangedReason;

/**
 * @brief 
 * 
 */
class UserConfig {
  public:
    UserConfig(){ 
      c_config = new RteUserConfig();
      RteUserConfigInit(c_config, nullptr);
    }
    virtual ~UserConfig(){ 
      if (c_config != nullptr) {
        RteUserConfigDeinit(c_config, nullptr);
        delete c_config; c_config = nullptr; 
      } 
    }

    RteUserConfig* get_underlying_impl(){
      return c_config;
    }

  protected:
    explicit UserConfig(RteUserConfig* config){
      c_config = config;
    }

  protected:
    RteUserConfig* c_config = nullptr;
};

/**
 * @brief 
 * @technical preview
 */
class UserInfo {
  public:
    UserInfo(){ 
      c_info = new RteUserInfo();
      RteUserInfoInit(c_info, nullptr);
    }
    virtual ~UserInfo(){ 
      if (c_info != nullptr) { 
        RteUserInfoDeinit(c_info, nullptr);
        delete c_info; c_info = nullptr; 
      } 
    }

    
    /**
     * @brief Get the user ID.
     * @technical preview
     * @return std::string The user ID.
     */
    std::string UserId() const {
      if(c_info != nullptr){
        String str(c_info->user_id);
        return std::string(str.CStr());
      }
      return std::string();
    }

    RteUserInfo* get_underlying_impl(){
      return c_info;
    }

  protected:
    explicit UserInfo(RteUserInfo* info){
      c_info = info;
    }

  protected:
    RteUserInfo* c_info = nullptr;
};




class UserObserver {
  public:
    UserObserver(): c_observer(RteUserObserverCreate(nullptr)){
      c_observer->base_observer.me_in_target_lang = this;
    }
    virtual ~UserObserver(){ if (c_observer != nullptr) { RteUserObserverDestroy(c_observer, nullptr); c_observer = nullptr; } }

    RteUserObserver* get_underlying_impl(){
      return c_observer;
    } 

  protected:
    explicit UserObserver(RteUserObserver* observer){c_observer = observer;}

  protected:
    RteUserObserver* c_observer = nullptr;
};

class User {
  public:
    User(UserConfig* config = nullptr){
      RteUserInit(&c_user, config != nullptr ? config->get_underlying_impl() : nullptr, nullptr);
    }
    virtual ~User(){
      if(is_handle_owner){
        RteUserDeinit(&c_user, nullptr);
      }
    }

    User(User&& other) : c_user(other.c_user), is_handle_owner(other.is_handle_owner){
      other.c_user = {};
      other.is_handle_owner = false;
    }

    // @{
    User(User &other) = default;
    User& operator=(User&& other) = default;
    User& operator=(const User &other) = default;
    // @}

    bool GetConfigs(UserConfig* config, Error* err = nullptr){
      return RteUserGetConfigs(&c_user, config->get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    void SetConfigs(UserConfig* config, Error* err = nullptr){
      RteUserSetConfigs(&c_user, config->get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    bool GetInfo(UserInfo* info, Error* err = nullptr){
      return RteUserGetInfo(&c_user, info->get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    bool RegisterObserver(UserObserver* observer, Error* err = nullptr){
      return RteUserRegisterObserver(&c_user, observer->get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    bool UnregisterObserver(UserObserver* observer, Error* err = nullptr){
      return RteUserUnregisterObserver(&c_user, observer->get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    friend class Channel;

  protected:
    explicit User(const ::RteLocalUser& local_user){
      c_user.handle = local_user.handle;
    }

    explicit User(const ::RteRemoteUser& remote_user){
      c_user.handle = remote_user.handle;
    }

  protected:
    RteUser c_user = {0};
    bool is_handle_owner = false;
};


/**
 * @brief 
 * @technical preview
 */
class LocalUserConfig : public UserConfig {
  public:
    LocalUserConfig(): UserConfig(reinterpret_cast<RteUserConfig*>(new RteLocalUserConfig())){
      RteLocalUserConfigInit(reinterpret_cast<RteLocalUserConfig*>(c_config), nullptr);
    }

    ~LocalUserConfig(){
      if (c_config != nullptr) {
        RteLocalUserConfigDeinit(reinterpret_cast<RteLocalUserConfig*>(c_config), nullptr);
        delete c_config; 
        c_config = nullptr; 
      }
    }

    /**
     * @brief Set the User Id object
     * @technical preview
     * @param user_id 
     * @param err 
     */
    void SetUserId(const std::string& user_id, Error* err = nullptr){
      String str(user_id.c_str());
      RteLocalUserConfigSetUserId(reinterpret_cast<RteLocalUserConfig*>(c_config), str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the User Id object
     * @technical preview
     * @param err 
     * @return std::string 
     */
    std::string GetUserId(Error* err = nullptr){
      String str;
      RteLocalUserConfigGetUserId(reinterpret_cast<RteLocalUserConfig*>(c_config), str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
      return std::string(str.CStr());
    }

    /**
     * @brief Set the User Token object
     * @technical preview
     * @param user_token 
     * @param err 
     */
    void SetUserToken(const std::string& user_token, Error* err = nullptr){
      String str(user_token.c_str());
      RteLocalUserConfigSetUserToken(reinterpret_cast<RteLocalUserConfig*>(c_config), str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the User Token object
     * @technical preview
     * @param err 
     * @return std::string 
     */
    std::string GetUserToken(Error* err = nullptr){
      String str;
      RteLocalUserConfigGetUserToken(reinterpret_cast<RteLocalUserConfig*>(c_config), str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
      return std::string(str.CStr());
    }

    /**
     * @brief Set the Json Parameter object
     * @technical preview
     * @param json_parameter 
     * @param err 
     */
    void SetJsonParameter(const std::string& json_parameter, Error* err = nullptr){
      String str(json_parameter.c_str());
      RteLocalUserConfigSetJsonParameter(reinterpret_cast<RteLocalUserConfig*>(c_config), str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the Json Parameter object
     * @technical preview
     * @param err 
     * @return std::string 
     */
    std::string GetJsonParameter(Error* err = nullptr){
      String str;
      RteLocalUserConfigGetJsonParameter(reinterpret_cast<RteLocalUserConfig*>(c_config), str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
      return std::string(str.CStr());
    }

    RteLocalUserConfig* get_underlying_impl(){
      return reinterpret_cast<RteLocalUserConfig*>(c_config);
    }
};

/**
 * @brief 
 * @technical preview
 */
class LocalUserInfo : public UserInfo {
  public:
    LocalUserInfo() : UserInfo(reinterpret_cast<RteUserInfo*>(new RteLocalUserInfo())){
      RteLocalUserInfoInit(reinterpret_cast<RteLocalUserInfo*>(c_info), nullptr);
    }

    ~LocalUserInfo(){
      if (c_info != nullptr) { 
        RteLocalUserInfoDeinit(reinterpret_cast<RteLocalUserInfo*>(c_info), nullptr);
        delete c_info; 
        c_info = nullptr; 
      }
    }

    RteLocalUserInfo* get_underlying_impl(){
      return reinterpret_cast<RteLocalUserInfo*>(c_info);
    }
};

static void OnUserMetadataSnapshotReceived(RteLocalUserObserver *self, RteMetadata *metadata, RteMetadataInfo *metadata_info);
static void OnUserMetadataChanged(RteLocalUserObserver *self, RteMetadata *metadata, RteMetadataInfo *metadata_info);
static void OnLocalUserTokenPrivilegeWillExpire(RteLocalUserObserver *self);
static void OnLinkStateEvent(RteLocalUserObserver *self, RteLocalUserLinkState old_state, 
      RteLocalUserLinkState new_state, RteLocalUserLinkStateChangedReason reason, RteError *err);
static void OnLocalUserMessageReceived(RteLocalUserObserver *self,
                                   RteString *publisher, RteBuf *message);
static void OnRenewTokenResult(RteLocalUserObserver *self, RteError *err);

/**
 * @brief 
 * @technical preview
 */
class LocalUserObserver : public UserObserver {
  public:
    LocalUserObserver() : UserObserver(reinterpret_cast<RteUserObserver*>(RteLocalUserObserverCreate(nullptr))){
      c_observer->base_observer.me_in_target_lang = this;

      RteLocalUserObserver* local_observer = reinterpret_cast<RteLocalUserObserver*>(c_observer);
      local_observer->on_user_metadata_snapshot_received = rte::OnUserMetadataSnapshotReceived;
      local_observer->on_user_metadata_changed = rte::OnUserMetadataChanged;
      local_observer->on_token_privilege_will_expire = rte::OnLocalUserTokenPrivilegeWillExpire;
      local_observer->on_link_state_event = rte::OnLinkStateEvent;
      local_observer->on_user_message_received = rte::OnLocalUserMessageReceived;
      local_observer->on_renew_token_result = rte::OnRenewTokenResult;

    }
    ~LocalUserObserver(){if (c_observer != nullptr) { RteLocalUserObserverDestroy(reinterpret_cast<RteLocalUserObserver*>(c_observer), nullptr); c_observer = nullptr; } }

    /**
     * @brief 
     * @technical preview
     * @param metadata 
     */
    virtual void OnUserMetadataSnapshotReceived(const Metadata& metadata, const MetadataInfo& metadata_info){}

    /**
     * @brief 
     * @technical preview
     * @param metadata 
     */
    virtual void OnUserMetadataChanged(const Metadata& metadata, const MetadataInfo& metadata_info){}

    /**
     * @brief 
     * @technical preview
     */
    virtual void OnTokenPrivilegeWillExpire(){}

    /**
     * @brief 
     * @technical preview
     * @param old_state 
     * @param new_state 
     * @param reason 
     * @param err 
     */
    virtual void OnLinkStateEvent(LocalUserLinkState old_state, 
      LocalUserLinkState new_state, LocalUserLinkStateChangedReason reason, const Error& err){}

    /**
     * @brief 
     * @technical preview
     * @param publisher 
     * @param message 
     */
    virtual void OnUserMessageReceived(const std::string& publisher, const Buffer& message){}

    /**
     * @brief 
     * @technical preview
     * @param err 
     */
    virtual void OnRenewTokenResult(const Error& err){}
};

void OnUserMetadataSnapshotReceived(RteLocalUserObserver *self, RteMetadata *metadata, RteMetadataInfo *metadata_info){
  if(self != nullptr && self->base_observer.base_observer.me_in_target_lang != nullptr){
    LocalUserObserver* observer = static_cast<LocalUserObserver*>(self->base_observer.base_observer.me_in_target_lang);
    Metadata metadata_obj(metadata);
    MetadataInfo metadata_info_obj(metadata_info);
    observer->OnUserMetadataSnapshotReceived(metadata_obj, metadata_info_obj);
  }
}

void OnUserMetadataChanged(RteLocalUserObserver *self, RteMetadata *metadata, RteMetadataInfo *metadata_info){
  if(self != nullptr && self->base_observer.base_observer.me_in_target_lang != nullptr){
    LocalUserObserver* observer = static_cast<LocalUserObserver*>(self->base_observer.base_observer.me_in_target_lang);
    Metadata metadata_obj(metadata);
    MetadataInfo metadata_info_obj(metadata_info);
    observer->OnUserMetadataChanged(metadata_obj, metadata_info_obj);
  }
}

void OnLocalUserTokenPrivilegeWillExpire(RteLocalUserObserver *self){
  if(self != nullptr && self->base_observer.base_observer.me_in_target_lang != nullptr){
    LocalUserObserver* observer = static_cast<LocalUserObserver*>(self->base_observer.base_observer.me_in_target_lang);
    observer->OnTokenPrivilegeWillExpire();
  }
}

void OnLinkStateEvent(RteLocalUserObserver *self, RteLocalUserLinkState old_state, 
      RteLocalUserLinkState new_state, RteLocalUserLinkStateChangedReason reason, RteError *err){
  if(self != nullptr && self->base_observer.base_observer.me_in_target_lang != nullptr){
    LocalUserObserver* observer = static_cast<LocalUserObserver*>(self->base_observer.base_observer.me_in_target_lang);
    Error cpp_error(err);
    observer->OnLinkStateEvent(old_state, new_state, reason, cpp_error);
  }
}

void OnLocalUserMessageReceived(RteLocalUserObserver *self,
                                   RteString *publisher, RteBuf *message){
  if(self != nullptr && self->base_observer.base_observer.me_in_target_lang != nullptr){
    LocalUserObserver* observer = static_cast<LocalUserObserver*>(self->base_observer.base_observer.me_in_target_lang);
    String str(publisher);
    Buffer buf(message);
    observer->OnUserMessageReceived(std::string(str.CStr()), buf);
  }
}

void OnRenewTokenResult(RteLocalUserObserver *self, RteError *err){
  if(self != nullptr && self->base_observer.base_observer.me_in_target_lang != nullptr){
    LocalUserObserver* observer = static_cast<LocalUserObserver*>(self->base_observer.base_observer.me_in_target_lang);
    Error cpp_error(err);
    observer->OnRenewTokenResult(cpp_error);
  }
}
/**
 * @brief 
 * @technical preview
 * 
 */
class LocalUser : public User {
  public:
    LocalUser(Rte* rte, LocalUserConfig *config = nullptr) : User(RteLocalUserCreate(rte != nullptr ? &(rte->c_rte) : nullptr, 
    config != nullptr ? reinterpret_cast<RteLocalUserConfig*>(config->get_underlying_impl()) : nullptr, nullptr)){
      is_handle_owner = true;
    }

    ~LocalUser(){
      if(is_handle_owner){
        RteLocalUserDestroy(reinterpret_cast<RteLocalUser*>(&c_user), nullptr);
      }
    }

    LocalUser(LocalUser&& other) : User(std::move(other)){}

    explicit LocalUser(const ::RteLocalUser& local_user): User(local_user) {}

    // @{
    LocalUser(LocalUser &other) = delete;
    LocalUser& operator=(const LocalUser &other) = delete;
    LocalUser& operator=(LocalUser&& other) = delete;
    // @}

    /**
     * @brief Get the configs object
     * @technical preview
     * @param config The config object
     * @param err The error object
     */
    void GetConfigs(LocalUserConfig* config, Error* err = nullptr) {
      RteLocalUserGetConfigs(reinterpret_cast<RteLocalUser*>(&c_user), 
                            config != nullptr ? reinterpret_cast<RteLocalUserConfig*>(config->get_underlying_impl()) : nullptr, 
                            err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Set the Configs object
     * @technical preview
     * @param config 
     * @param err 
     */
    void SetConfigs(LocalUserConfig* config, Error* err = nullptr) {
      RteLocalUserSetConfigs(reinterpret_cast<RteLocalUser*>(&c_user), 
                            config != nullptr ? reinterpret_cast<RteLocalUserConfig*>(config->get_underlying_impl()) : nullptr, 
                            err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the Info object
     * @technical preview
     * @param info 
     * @param err 
     */
    void GetInfo(LocalUserInfo* info, Error* err = nullptr) {
      RteLocalUserGetInfo(reinterpret_cast<RteLocalUser*>(&c_user), 
                         info != nullptr ? reinterpret_cast<RteLocalUserInfo*>(info->get_underlying_impl()) : nullptr, 
                         err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief 
     * @technical preview
     * @param callback 
     */
    void Connect(std::function<void(Error*)> callback) {
      CallbackContextWithArgs<LocalUser>* callbackCtx = new CallbackContextWithArgs<LocalUser>(this, callback);
      RteLocalUserConnect(reinterpret_cast<RteLocalUser*>(&c_user), &CallbackFuncWithArgs<::RteLocalUser, LocalUser>, callbackCtx);
    }

    /**
     * @brief 
     * @technical preview
     * @param callback 
     */
    void Disconnect(std::function<void(Error*)> callback) {
      CallbackContextWithArgs<LocalUser>* callbackCtx = new CallbackContextWithArgs<LocalUser>(this, callback);
      RteLocalUserDisconnect(reinterpret_cast<RteLocalUser*>(&c_user), &CallbackFuncWithArgs<::RteLocalUser, LocalUser>, callbackCtx);
    }

    /**
     * @brief 
     * @technical preview
     * @return true 
     * @return false 
     */
    bool IsConnected() {
      return RteLocalUserIsConnected(reinterpret_cast<RteLocalUser*>(&c_user));
    }

    /**
     * @brief 
     * @technical preview
     * @param user_id 
     * @param message 
     * @param callback 
     */
    void PublishMessage(const std::string& user_id, Buffer* message, std::function<void(Error*)> callback) {
      CallbackContextWithArgs<LocalUser>* callbackCtx = new CallbackContextWithArgs<LocalUser>(this, callback);
      RteLocalUserPublishMessage(reinterpret_cast<RteLocalUser*>(&c_user), 
                                user_id.c_str(), 
                                message != nullptr ? message->get_underlying_impl() : nullptr, 
                                &CallbackFuncWithArgs<::RteLocalUser, LocalUser>, 
                                callbackCtx);
    }

    /**
     * @brief Get the Metadata object
     * @technical preview
     * @param user_id 
     * @param cb 
     */
    void GetMetadata(const std::string& user_id, std::function<void(Metadata* items, Error* err)> cb){
      std::function<void(RteMetadata* items, Error* err)> callback = [cb](RteMetadata* items, Error* err){
        if(cb != nullptr){
          Metadata metadata(items);
          cb(&metadata, err);
        }
      };
      CallbackContextWithArgs<LocalUser, RteMetadata*>* callbackCtx = new CallbackContextWithArgs<LocalUser, RteMetadata*>(this, callback);
      RteLocalUserGetMetadata(reinterpret_cast<RteLocalUser*>(&c_user), user_id.c_str(), &CallbackFuncWithArgs<::RteLocalUser, LocalUser, RteMetadata*>, callbackCtx);
    }

    /**
     * @brief 
     * @technical preview
     * @param user_id 
     * @param cb 
     */
    void SubscribeMetadata(const std::string& user_id, std::function<void(Error* err)> cb){
      CallbackContextWithArgs<LocalUser>* callbackCtx = new CallbackContextWithArgs<LocalUser>(this, cb);
      RteLocalUserSubscribeMetadata(reinterpret_cast<RteLocalUser*>(&c_user), user_id.c_str(), &CallbackFuncWithArgs<::RteLocalUser, LocalUser>, callbackCtx);
    }

    /**
     * @brief 
     * @technical preview
     * @param user_id 
     * @param cb 
     */
    void UnsubscribeMetadata(const std::string& user_id, std::function<void(Error* err)> cb){
      CallbackContextWithArgs<LocalUser>* callbackCtx = new CallbackContextWithArgs<LocalUser>(this, cb);
      RteLocalUserUnsubscribeMetadata(reinterpret_cast<RteLocalUser*>(&c_user), user_id.c_str(), &CallbackFuncWithArgs<::RteLocalUser, LocalUser>, callbackCtx);
    }

    /**
     * @brief Set the Metadata object
     * @technical preview
     * @param user_id 
     * @param items 
     * @param config 
     * @param cb 
     */
    void SetMetadata(const std::string& user_id, Metadata* items, MetadataConfig* config, std::function<void(Error* err)> cb){
      CallbackContextWithArgs<LocalUser>* callbackCtx = new CallbackContextWithArgs<LocalUser>(this, cb);
      RteLocalUserSetMetadata(reinterpret_cast<RteLocalUser*>(&c_user), user_id.c_str(), 
                         items != nullptr ? items->get_underlying_impl() : nullptr, 
                         config != nullptr ? config->get_underlying_impl() : nullptr, 
                         &CallbackFuncWithArgs<::RteLocalUser, LocalUser>, callbackCtx);
    }
  
    /**
     * @brief 
     * @technical preview
     * @param user_id 
     * @param items 
     * @param cb 
     */
    void RemoveMetadata(const std::string& user_id, Metadata* items, MetadataConfig* config, std::function<void(Error* err)> cb){
      CallbackContextWithArgs<LocalUser>* callbackCtx = new CallbackContextWithArgs<LocalUser>(this, cb);
      RteLocalUserRemoveMetadata(reinterpret_cast<RteLocalUser*>(&c_user), user_id.c_str(), 
                            items != nullptr ? items->get_underlying_impl() : nullptr, 
                            config != nullptr ? config->get_underlying_impl() : nullptr, 
                            &CallbackFuncWithArgs<::RteLocalUser, LocalUser>, 
                            callbackCtx);
    }

    /**
     * @brief 
     * @technical preview
     * @param observer 
     * @param err 
     * @return true 
     * @return false 
     */
    bool RegisterObserver(LocalUserObserver* observer, Error* err = nullptr) {
      return RteLocalUserRegisterObserver(reinterpret_cast<RteLocalUser*>(&c_user), 
                                         reinterpret_cast<RteLocalUserObserver*>(observer->get_underlying_impl()), 
                                         err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief 
     * @technical preview
     * @param observer 
     * @param err 
     * @return true 
     * @return false 
     */
    bool UnregisterObserver(LocalUserObserver* observer, Error* err = nullptr) {
      return RteLocalUserUnregisterObserver(reinterpret_cast<RteLocalUser*>(&c_user), 
                                           reinterpret_cast<RteLocalUserObserver*>(observer->get_underlying_impl()), 
                                           err != nullptr ? err->get_underlying_impl() : nullptr);
    }
    
    friend class Channel;
};


class RemoteUserConfig : public UserConfig {
  public:
    RemoteUserConfig(): UserConfig(reinterpret_cast<RteUserConfig*>(new RteRemoteUserConfig())){
      RteRemoteUserConfigInit(reinterpret_cast<RteRemoteUserConfig*>(c_config), nullptr);
    }
    ~RemoteUserConfig(){
      if (c_config != nullptr) {
        RteRemoteUserConfigDeinit(reinterpret_cast<RteRemoteUserConfig*>(c_config), nullptr);
        delete c_config;
        c_config = nullptr;
      }
    }

    void SetJsonParameter(const std::string& json_parameter, Error* err = nullptr){
      String str(json_parameter.c_str());
      RteRemoteUserConfigSetJsonParameter(reinterpret_cast<RteRemoteUserConfig*>(c_config), str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    std::string GetJsonParameter(Error* err = nullptr){ 
      String str;
      RteRemoteUserConfigGetJsonParameter(reinterpret_cast<RteRemoteUserConfig*>(c_config), str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
      return std::string(str.CStr());
    }

    RteRemoteUserConfig* get_underlying_impl(){
      return reinterpret_cast<RteRemoteUserConfig*>(c_config);
    }
};

class RemoteUserInfo : public UserInfo {
  public:
    RemoteUserInfo() : UserInfo(reinterpret_cast<RteUserInfo*>(new RteRemoteUserInfo())){
      RteRemoteUserInfoInit(reinterpret_cast<RteRemoteUserInfo*>(c_info), nullptr);
    }
    ~RemoteUserInfo(){
      if (c_info != nullptr) {
        RteRemoteUserInfoDeinit(reinterpret_cast<RteRemoteUserInfo*>(c_info), nullptr);
        delete c_info;
        c_info = nullptr;
      }
    }

    RteRemoteUserInfo* get_underlying_impl(){
      return reinterpret_cast<RteRemoteUserInfo*>(c_info);
    }
};


class RemoteUserObserver: public UserObserver {
  public:
    RemoteUserObserver() : UserObserver(reinterpret_cast<RteUserObserver*>(RteRemoteUserObserverCreate(nullptr))){
      c_observer->base_observer.me_in_target_lang = this;
    }
    ~RemoteUserObserver(){if (c_observer != nullptr) { RteRemoteUserObserverDestroy(reinterpret_cast<RteRemoteUserObserver*>(c_observer), nullptr); c_observer = nullptr; } }
};

/**
 * @brief 
 * @technical preview
 */
class RemoteUser : public User {
  public:
    RemoteUser(){}
    ~RemoteUser(){}

    RemoteUser(const ::RteRemoteUser& remote_user): User(remote_user){}

    RemoteUser(RemoteUser&& other) : User(std::move(other)){}

    // @{
    RemoteUser(RemoteUser &other) = default;
    RemoteUser& operator=(const RemoteUser &other) = default;
    RemoteUser& operator=(RemoteUser&& other) = default;
    // @}

    void GetConfigs(RemoteUserConfig* config, Error* err = nullptr) {
      RteRemoteUserGetConfigs(reinterpret_cast<RteRemoteUser*>(&c_user), 
                             config != nullptr ? reinterpret_cast<RteRemoteUserConfig*>(config->get_underlying_impl()) : nullptr, 
                             err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    void SetConfigs(RemoteUserConfig* config, std::function<void(Error*)> callback) {
      CallbackContextWithArgs<RemoteUser>* callbackCtx = new CallbackContextWithArgs<RemoteUser>(this, callback);
      RteRemoteUserSetConfigs(reinterpret_cast<RteRemoteUser*>(&c_user), 
                             config != nullptr ? reinterpret_cast<RteRemoteUserConfig*>(config->get_underlying_impl()) : nullptr, 
                             &CallbackFuncWithArgs<::RteRemoteUser, RemoteUser>, 
                             callbackCtx);
    }

    /**
     * @brief Get the Info object
     * @technical preview
     * @param info 
     * @param err 
     */
    void GetInfo(RemoteUserInfo* info, Error* err = nullptr) {
      RteRemoteUserGetInfo(reinterpret_cast<RteRemoteUser*>(&c_user), 
                          info != nullptr ? reinterpret_cast<RteRemoteUserInfo*>(info->get_underlying_impl()) : nullptr, 
                          err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    bool RegisterObserver(RemoteUserObserver* observer, Error* err = nullptr) {
      return RteRemoteUserRegisterObserver(reinterpret_cast<RteRemoteUser*>(&c_user), 
                                          reinterpret_cast<RteRemoteUserObserver*>(observer->get_underlying_impl()), 
                                          err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    bool UnregisterObserver(RemoteUserObserver* observer, Error* err = nullptr) {
      return RteRemoteUserUnregisterObserver(reinterpret_cast<RteRemoteUser*>(&c_user), 
                                           reinterpret_cast<RteRemoteUserObserver*>(observer->get_underlying_impl()), 
                                           err != nullptr ? err->get_underlying_impl() : nullptr);
    }
};

}  // namespace rte