/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once
#include <string>
#include <functional>
#include <vector>
#include "rte_base/c/handle.h"
#include "rte_base/c/channel.h"
#include "rte_base/c/utils/buf.h"
#include "rte_base/c/utils/string.h"
#include "rte_cpp_string.h"
#include "rte_cpp_error.h"
#include "rte_cpp_rte.h"
#include "rte_cpp_callback_utils.h"
#include "rte_cpp_user.h"
#include "rte_cpp_buffer.h"
#include "rte_cpp_metadata.h"
#include "stream/rte_cpp_stream.h"
#include "stream/rte_cpp_local_stream.h"
#include "stream/rte_cpp_remote_stream.h"
#include "track/rte_cpp_track.h"

namespace rte {

using ChannelProfile = RteChannelProfile;
using TrackMediaType = RteTrackMediaType;
using TrackSubState = RteTrackSubState;
using TrackPubState = RteTrackPubState;
using TrackSubStateChangedReason = RteTrackSubStateChangedReason;
using TrackPubStateChangedReason = RteTrackPubStateChangedReason;
using ChannelSubscribeType = RteChannelSubscribeType;

/**
 * @brief Audio volume information
 * @technical preview
 */
class AudioVolumeInfo {
  public:
    explicit AudioVolumeInfo(RteAudioVolumeInfo* info) {
      if(info != nullptr) {
        c_info = *info;
      }
    }

    /**
     * @brief Get the user ID
     * @technical preview
     * @return std::string User ID
     */
    std::string GetUserId() const {
      return "";
    }

    /**
     * @brief Get the volume level
     * @technical preview
     * @return int Volume level
     */
    int GetVolume() const {
      return c_info.volume;
    }

  private:
    RteAudioVolumeInfo c_info;
};

/**
 * @brief 
 * @technical preview
 */
class PresenceStateItem {
  public:
    explicit PresenceStateItem(const std::string& key, const std::string& value){
      c_presence_state_item_ = new ::RtePresenceStateItem();
      RtePresenceStateItemInit(c_presence_state_item_, nullptr);

      {String str(key.c_str());
      RtePresenceStateItemSetKey(c_presence_state_item_, str.get_underlying_impl(), nullptr);}

      {String str(value.c_str());
      RtePresenceStateItemSetValue(c_presence_state_item_, str.get_underlying_impl(), nullptr);}
      is_owner_ = true;
    }

    explicit PresenceStateItem(::RtePresenceStateItem* presence_state_item){

      c_presence_state_item_ = presence_state_item;
      is_owner_ = false;
      
    }

    explicit PresenceStateItem(const PresenceStateItem& other){
      c_presence_state_item_ = new ::RtePresenceStateItem();
      RtePresenceStateItemInit(c_presence_state_item_, nullptr);
      RtePresenceStateItemCopy(c_presence_state_item_, other.c_presence_state_item_, nullptr);
      is_owner_ = true;
    }

    PresenceStateItem(PresenceStateItem&& other){
      c_presence_state_item_ = other.c_presence_state_item_;
      is_owner_ = other.is_owner_;

      other.c_presence_state_item_ = nullptr;
      other.is_owner_ = false;
    }

    PresenceStateItem& operator=(const PresenceStateItem& other){
      RtePresenceStateItemCopy(c_presence_state_item_, other.c_presence_state_item_, nullptr);
      return *this;
    }

    ~PresenceStateItem(){
      if(is_owner_ && c_presence_state_item_ != nullptr){
        RtePresenceStateItemDeinit(c_presence_state_item_, nullptr);
        delete c_presence_state_item_;
        c_presence_state_item_ = nullptr;
      }
    }

    /**
     * @brief Get the Key object
     * @technical preview
     * @param err 
     * @return std::string 
     */
    std::string GetKey(Error *err = nullptr){
      String str;
      RtePresenceStateItemGetKey(c_presence_state_item_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl(): nullptr);
      return std::string(str.CStr());
    }

    /**
     * @brief Get the Value object
     * @technical preview
     * @param err 
     * @return std::string 
     */
    std::string GetValue(Error *err = nullptr){
      String str; 
      RtePresenceStateItemGetValue(c_presence_state_item_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl(): nullptr);
      return std::string(str.CStr());
    }

    RtePresenceStateItem* get_underlying_impl() {
      return c_presence_state_item_;
    }

  private:
    friend class PresenceState;
    ::RtePresenceStateItem* c_presence_state_item_;
    bool is_owner_ = false;
};

class PresenceState {
  public:
    PresenceState(){
      c_presence_state_ = new ::RtePresenceState();
      RtePresenceStateInit(c_presence_state_, nullptr);
      is_owner_ = true;
    }

    ~PresenceState(){
      if(is_owner_ && c_presence_state_ != nullptr){
        RtePresenceStateDeinit(c_presence_state_, nullptr);
        delete c_presence_state_;
        c_presence_state_ = nullptr;
      }
    }

    PresenceState(const PresenceState& other){
      c_presence_state_ = new ::RtePresenceState();
      RtePresenceStateInit(c_presence_state_, nullptr);
      RtePresenceStateCopy(c_presence_state_, other.c_presence_state_, nullptr);
      is_owner_ = true;
    }

    explicit PresenceState(::RtePresenceState* presence_state){
      c_presence_state_ = presence_state;
      is_owner_ = false;
    }

    PresenceState(PresenceState&& other){
      c_presence_state_ = other.c_presence_state_;
      is_owner_ = other.is_owner_;

      other.c_presence_state_ = nullptr;
      other.is_owner_ = false;
    }

    PresenceState& operator=(const PresenceState& other){
      RtePresenceStateCopy(c_presence_state_, other.c_presence_state_, nullptr);
      return *this;
    }

    /**
     * @brief Get the Name object
     * @technical preview
     * @param err 
     * @return std::string 
     */
    std::string GetName(Error *err = nullptr){
      String str;
      RtePresenceStateGetName(c_presence_state_, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl(): nullptr);
      return std::string(str.CStr());
    }
    
    /**
     * @brief Get the Items object
     * @technical preview
     * @param err 
     * @return std::vector<PresenceStateItem> 
     */
    std::vector<PresenceStateItem> GetItems(Error *err = nullptr){
      std::vector<PresenceStateItem> items;
      RtePresenceStateItem* items_ptr = nullptr;
      size_t items_cnt = 0;
      RtePresenceStateGetItems(c_presence_state_, &items_ptr, &items_cnt, err != nullptr ? err->get_underlying_impl(): nullptr);
      for(size_t i = 0; i < items_cnt; i++){
        items.push_back(PresenceStateItem(&items_ptr[i]));
      }
      return items;
    }

    /**
     * @brief Set the Items object
     * @technical preview
     * @param items 
     * @param err 
     */
    void SetItems(const std::vector<PresenceStateItem>& items, Error *err = nullptr){

      std::shared_ptr<RtePresenceStateItem> items_ptr(new RtePresenceStateItem[items.size()], [](RtePresenceStateItem* ptr){
        RtePresenceStateItemDeinit(ptr, nullptr);
        delete[] ptr;
      });

      for(size_t i = 0; i < items.size(); i++){ 
        RtePresenceStateItemInit(&items_ptr.get()[i], err != nullptr ? err->get_underlying_impl(): nullptr);
        RtePresenceStateItemCopy(&items_ptr.get()[i], items[i].c_presence_state_item_, err != nullptr ? err->get_underlying_impl(): nullptr);
      }

      RtePresenceStateSetItems(c_presence_state_, items_ptr.get(), items.size(), err != nullptr ? err->get_underlying_impl(): nullptr);
      items_ptr.reset();
     }

    RtePresenceState* get_underlying_impl(){
      return c_presence_state_;
    }

  private:
    ::RtePresenceState* c_presence_state_;
    bool is_owner_ = false;
};

/**
 * @brief 
 * @technical preview
 */
class ChannelConfig {
  public:
    ChannelConfig(){RteChannelConfigInit(&c_config, nullptr);}
    ~ChannelConfig(){RteChannelConfigDeinit(&c_config, nullptr);}

    
    /**
     * @brief Set the Channel Id object
     * @technical preview
     * @param channel_id 
     * @param err 
     */
    void SetChannelId(const std::string& channel_id, Error *err = nullptr){
      String str(channel_id.c_str());
      RteChannelConfigSetChannelId(&c_config, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl(): nullptr);
    }

    /**
     * @brief Get the Channel Id object
     * @technical preview
     * @param err 
     * @return std::string 
     */
    std::string GetChannelId(Error *err = nullptr){
      String str;
      RteChannelConfigGetChannelId(&c_config, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl(): nullptr);
      return std::string(str.CStr());
    }

    /**
     * @brief Set the Allow Auto Subscribe All object
     * @technical preview
     * @param allow_auto_subscribe_all 
     * @param err 
     */
    void SetAllowAutoSubscribeAll(bool allow_auto_subscribe_all, Error *err = nullptr){
      RteChannelConfigSetAllowAutoSubscribeAll(&c_config, allow_auto_subscribe_all, err != nullptr ? err->get_underlying_impl(): nullptr);
    }

    /**
     * @brief Get the Allow Auto Subscribe All object
     * @technical preview
     * @param err 
     * @return bool 
     */
    bool GetAllowAutoSubscribeAll(Error *err = nullptr){
      bool allow_auto_subscribe_all;
      RteChannelConfigGetAllowAutoSubscribeAll(&c_config, &allow_auto_subscribe_all, err != nullptr ? err->get_underlying_impl(): nullptr);
      return allow_auto_subscribe_all;
    }

    /**
     * @brief Set the Auto Subscribe Message object
     * @technical preview
     * @param auto_subscribe_message 
     * @param err 
     */
    void SetAutoSubscribeMessage(bool auto_subscribe_message, Error *err = nullptr){
      RteChannelConfigSetAutoSubscribeMessage(&c_config, auto_subscribe_message, err != nullptr ? err->get_underlying_impl(): nullptr);
    }

    /**
     * @brief Get the Auto Subscribe Message object
     * @technical preview
     * @param err 
     * @return bool 
     */
    bool GetAutoSubscribeMessage(Error *err = nullptr){
      bool auto_subscribe_message;
      RteChannelConfigGetAutoSubscribeMessage(&c_config, &auto_subscribe_message, err != nullptr ? err->get_underlying_impl(): nullptr);
      return auto_subscribe_message;
    }

    /**
     * @brief Set the Auto Subscribe Metadata object
     * @technical preview
     * @param auto_subscribe_metadata 
     * @param err 
     */
    void SetAutoSubscribeMetadata(bool auto_subscribe_metadata, Error *err = nullptr){
      RteChannelConfigSetAutoSubscribeMetadata(&c_config, auto_subscribe_metadata, err != nullptr ? err->get_underlying_impl(): nullptr);
    }

    /**
     * @brief Get the Auto Subscribe Metadata object
     * @technical preview
     * @param err 
     * @return bool 
     */
    bool GetAutoSubscribeMetadata(Error *err = nullptr){
      bool auto_subscribe_metadata;
      RteChannelConfigGetAutoSubscribeMetadata(&c_config, &auto_subscribe_metadata, err != nullptr ? err->get_underlying_impl(): nullptr);
      return auto_subscribe_metadata;
    }

    /**
     * @brief Set the Auto Subscribe User Presence object
     * @technical preview
     * @param auto_subscribe_user_presence 
     * @param err 
     */
    void SetAutoSubscribeUserPresence(bool auto_subscribe_user_presence, Error *err = nullptr){
      RteChannelConfigSetAutoSubscribeUserPresence(&c_config, auto_subscribe_user_presence, err != nullptr ? err->get_underlying_impl(): nullptr);
    }

    /**
     * @brief Get the Auto Subscribe User Presence object
     * @technical preview
     * @param err 
     * @return bool 
     */
    bool GetAutoSubscribeUserPresence(Error *err = nullptr){
      bool autoSubscribeUserPresence; 
      RteChannelConfigGetAutoSubscribeUserPresence(&c_config, &autoSubscribeUserPresence, err != nullptr ? err->get_underlying_impl(): nullptr);
      return autoSubscribeUserPresence;
    }

    /**
     * @brief Set the Auto Subscribe Stream Presence object
     * @technical preview
     * @param auto_subscribe_stream_presence 
     * @param err 
     */
    void SetAutoSubscribeStreamPresence(bool auto_subscribe_stream_presence, Error *err = nullptr){
      RteChannelConfigSetAutoSubscribeStreamPresence(&c_config, auto_subscribe_stream_presence, err != nullptr ? err->get_underlying_impl(): nullptr);
    }

    /**
     * @brief Get the Auto Subscribe Stream Presence object
     * @technical preview
     * @param err 
     * @return bool 
     */
    bool GetAutoSubscribeStreamPresence(Error *err = nullptr){
      bool auto_subscribe_stream_presence;
      RteChannelConfigGetAutoSubscribeStreamPresence(&c_config, &auto_subscribe_stream_presence, err != nullptr ? err->get_underlying_impl(): nullptr);
      return auto_subscribe_stream_presence;
    }

    /**
     * @brief Set the Be Quiet object
     * @technical preview
     * @param be_quiet 
     * @param err 
     */
    void SetBeQuiet(bool be_quiet, Error *err = nullptr){
      RteChannelConfigSetBeQuiet(&c_config, be_quiet, err != nullptr ? err->get_underlying_impl(): nullptr);
    }

    /**
     * @brief Get the Be Quiet object
     * @technical preview
     * @param err 
     * @return bool 
     */
    bool GetBeQuiet(Error *err = nullptr){
      bool be_quiet;
      RteChannelConfigGetBeQuiet(&c_config, &be_quiet, err != nullptr ? err->get_underlying_impl(): nullptr);
      return be_quiet;
    }
    
    /**
     * @brief Set the Auto Subscribe Audio object
     * @technical preview
     * @param auto_subscribe_audio 
     * @param err 
     */
    void SetAutoSubscribeAudio(bool auto_subscribe_audio, Error *err = nullptr){
      RteChannelConfigSetAutoSubscribeAudio(&c_config, auto_subscribe_audio, err != nullptr ? err->get_underlying_impl(): nullptr);
    }

    /**
     * @brief Get the Auto Subscribe Audio object
     * @technical preview
     * @param err 
     * @return bool 
     */
    bool GetAutoSubscribeAudio(Error *err = nullptr){
      bool auto_subscribe_audio;
      RteChannelConfigGetAutoSubscribeAudio(&c_config, &auto_subscribe_audio, err != nullptr ? err->get_underlying_impl(): nullptr);
      return auto_subscribe_audio;
    }

    /**
     * @brief Set the Auto Subscribe Video object
     * @technical preview
     * @param auto_subscribe_video 
     * @param err 
     */
    void SetAutoSubscribeVideo(bool auto_subscribe_video, Error *err = nullptr){
      RteChannelConfigSetAutoSubscribeVideo(&c_config, auto_subscribe_video, err != nullptr ? err->get_underlying_impl(): nullptr);
    }

    /**
     * @brief Get the Auto Subscribe Video object
     * @technical preview
     * @param err 
     * @return bool 
     */
    bool GetAutoSubscribeVideo(Error *err = nullptr){
      bool auto_subscribe_video;  
      RteChannelConfigGetAutoSubscribeVideo(&c_config, &auto_subscribe_video, err != nullptr ? err->get_underlying_impl(): nullptr);
      return auto_subscribe_video;
    }

    void SetJsonParameter(const std::string& json_parameter, Error *err = nullptr){
      String str(json_parameter.c_str());
      RteChannelConfigSetJsonParameter(&c_config, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl(): nullptr);
    }

    std::string GetJsonParameter(Error *err = nullptr){
      String str;
      RteChannelConfigGetJsonParameter(&c_config, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl(): nullptr);
      return std::string(str.CStr());
    } 


    RteChannelConfig* get_underlying_impl(){
      return &c_config;
    }
    
  private:
    ::RteChannelConfig c_config;
};

/**
 * @brief Options for subscribing to a track
 * @technical preview
 */
class SubscribeOptions {
  public:
    SubscribeOptions() {
      RteSubscribeOptionsInit(&c_options, nullptr);
    }
    ~SubscribeOptions() {
      RteSubscribeOptionsDeinit(&c_options, nullptr);
    }

    /**
     * @brief Set the track media type
     * @technical preview
     * @param track_media_type Track media type
     * @param err Error object to store error information
     */
    void SetTrackMediaType(TrackMediaType track_media_type, Error* err = nullptr) {
      c_options.track_media_type = track_media_type;
    }

    /**
     * @brief Get the track media type
     * @technical preview
     * @param err Error object to store error information
     * @return TrackMediaType Track media type
     */
    TrackMediaType GetTrackMediaType(Error* err = nullptr) {
      return c_options.track_media_type;
    }

    /**
     * @brief Set the data track topic
     * @technical preview
     * @param topic Data track topic
     * @param err Error object to store error information
     */
    void SetDataTrackTopic(const std::string& topic, Error* err = nullptr) {
      String str(topic.c_str());
      RteSubscribeOptionsSetDataTrackTopic(&c_options, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the data track topic
     * @technical preview
     * @param err Error object to store error information
     * @return std::string Data track topic
     */
    std::string GetDataTrackTopic(Error* err = nullptr) {
      String str;
      RteSubscribeOptionsGetDataTrackTopic(&c_options, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
      return std::string(str.CStr());
    }

    /**
     * @brief Set the canvas
     * @technical preview
     * @param canvas Pointer to the Canvas object
     * @param err Pointer to the Error object for storing error information
     */
    void SetCanvas(Canvas* canvas, Error* err = nullptr) {
      c_options.canvas = canvas != nullptr ? canvas->get_underlying_impl() : nullptr;
    }

    /**
     * @brief Get the canvas
     * @technical preview
     * @param err Pointer to the Error object for storing error information
     * @return Canvas* Pointer to the Canvas object
     */
    Canvas GetCanvas(Error* err = nullptr) {
      if (c_options.canvas == nullptr) {
        return Canvas();
      }
      return Canvas(c_options.canvas);
    }
    
    RteSubscribeOptions* get_underlying_impl() {
      return &c_options;
    }

  private:
    RteSubscribeOptions c_options;
};

static void OnRemoteUsersJoined(RteChannelObserver *self, RteRemoteUser *new_users, RteRemoteUserInfo *new_users_info, size_t new_users_cnt);
static void OnRemoteUsersLeft(RteChannelObserver *self, RteRemoteUser *removed_users, RteRemoteUserInfo *removed_users_info, size_t removed_users_cnt);
static void OnChannelMessageReceived(RteChannelObserver *self,
                              RteString* publisher, RteBuf *message);
static void OnChannelMetadataSnapshotReceived(RteChannelObserver *self, RteMetadata *metadata, RteMetadataInfo *metadata_info);
static void OnChannelMetadataChanged(RteChannelObserver *self, RteMetadata *metadata, RteMetadataInfo *metadata_info);
static void OnChannelUserPresenceSnapshotReceived(RteChannelObserver *self, RtePresenceState *states, size_t states_cnt);
static void OnChannelUserPresenceChanged(RteChannelObserver *self, RtePresenceState *state);
static void OnRemoteStreamsAdded(RteChannelObserver *self, RteRemoteStream *new_streams, RteRemoteStreamInfo *new_streams_info, size_t new_streams_cnt);
static void OnRemoteStreamsRemoved(RteChannelObserver *self, RteRemoteStream *removed_streams, RteRemoteStreamInfo *removed_streams_info, size_t removed_streams_cnt);
static void OnLocalStreamInfo(RteChannelObserver *self, RteLocalStream *stream, RteLocalStreamInfo *info);
static void OnRemoteStreamInfo(RteChannelObserver *self, RteRemoteStream *stream, RteRemoteStreamInfo *info);
static void OnSubStateChanged(RteChannelObserver *self, RteRemoteStream *stream, RteTrack *track, RteTrackMediaType track_media_type,
                            RteTrackSubState old_state, RteTrackSubState new_state, RteTrackSubStateChangedReason reason, RteError *err);
static void OnPubStateChanged(RteChannelObserver *self, RteLocalStream *stream, RteTrack *track, RteTrackMediaType track_media_type,
                            RteTrackPubState old_state, RteTrackPubState new_state, RteTrackPubStateChangedReason reason, RteError *err);
static void OnActiveSpeaker(RteChannelObserver *self, RteStream *stream);
static void OnAudioVolumeIndication(RteChannelObserver *self, RteAudioVolumeInfo *audio_volume_infos, size_t audio_volume_infos_cnt);
static void OnChannelTokenWillExpire(RteChannelObserver *self, RteString *channel_token);
static void OnStreamTokenWillExpire(RteChannelObserver *self, RteStream *stream, RteString *stream_token);
static void OnChannelTokenExpired(RteChannelObserver *self);
static void OnStreamTokenExpired(RteChannelObserver *self, RteStream *stream);
static void OnChannelStreamStateSnapshotReceived(RteChannelObserver *self, RtePresenceState *states, size_t states_cnt);
static void OnChannelStreamStateChanged(RteChannelObserver *self, RtePresenceState *state);

static void OnSubscribeResult(RteChannelObserver *self, ChannelSubscribeType subscribe_type, RteError *err);
static void OnUnsubscribeResult(RteChannelObserver *self, ChannelSubscribeType subscribe_type, RteError *err);

/*
 * @brief ChannelObserver
 * @technical preview
*/
class ChannelObserver {
  public:
    ChannelObserver() : c_observer(::RteChannelObserverCreate(nullptr)){

      c_observer->base_observer.me_in_target_lang = this;

      c_observer->on_remote_users_joined = rte::OnRemoteUsersJoined;
      c_observer->on_remote_users_left = rte::OnRemoteUsersLeft;
      c_observer->on_channel_message_received = rte::OnChannelMessageReceived;
      c_observer->on_channel_metadata_snapshot_received = rte::OnChannelMetadataSnapshotReceived;
      c_observer->on_channel_metadata_changed = rte::OnChannelMetadataChanged;
      c_observer->on_channel_user_presence_snapshot_received = rte::OnChannelUserPresenceSnapshotReceived;
      c_observer->on_channel_user_presence_changed = rte::OnChannelUserPresenceChanged;
      
      c_observer->on_remote_streams_added = rte::OnRemoteStreamsAdded;
      c_observer->on_remote_streams_removed = rte::OnRemoteStreamsRemoved;
      c_observer->on_local_stream_info = rte::OnLocalStreamInfo;
      c_observer->on_remote_stream_info = rte::OnRemoteStreamInfo;
      c_observer->on_sub_state_changed = rte::OnSubStateChanged;
      c_observer->on_pub_state_changed = rte::OnPubStateChanged;
      c_observer->on_active_speaker = rte::OnActiveSpeaker;
      c_observer->on_audio_volume_indication = rte::OnAudioVolumeIndication;
      c_observer->on_channel_token_will_expire = rte::OnChannelTokenWillExpire;
      c_observer->on_stream_token_will_expire = rte::OnStreamTokenWillExpire;
      c_observer->on_channel_token_expired = rte::OnChannelTokenExpired;
      c_observer->on_stream_token_expired = rte::OnStreamTokenExpired;
      c_observer->on_channel_stream_state_snapshot_received = rte::OnChannelStreamStateSnapshotReceived;
      c_observer->on_channel_stream_state_changed = rte::OnChannelStreamStateChanged;

      c_observer->on_subscribe_result = rte::OnSubscribeResult;
      c_observer->on_unsubscribe_result = rte::OnUnsubscribeResult;
    }
    ~ChannelObserver(){ RteChannelObserverDestroy(c_observer, nullptr);}

    /**
     * @brief 
     * @technical preview
     * @param new_users 
     */
    virtual void OnRemoteUsersJoined(const std::vector<RemoteUser>& new_users, const std::vector<RemoteUserInfo>& new_users_info){
    }

    /**
     * @brief 
     * @technical preview
     * @param removed_users 
     */
    virtual void OnRemoteUsersLeft(const std::vector<RemoteUser>& removed_users, const std::vector<RemoteUserInfo>& removed_users_info){
    }

    /**
     * @brief 
     * @technical preview
     * @param publisher 
     * @param message 
     */
    virtual void OnChannelMessageReceived(const std::string& publisher, const Buffer& message){
    } 

    /**
     * @brief 
     * @technical preview
     * @param metadata 
     */
    virtual void OnChannelMetadataSnapshotReceived(const Metadata& metadata, const MetadataInfo& metadata_info){
    }

    /**
     * @brief 
     * @technical preview
     * @param metadata 
     */
    virtual void OnChannelMetadataChanged(const Metadata& metadata, const MetadataInfo& metadata_info){
    }

    /**
     * @brief 
     * @technical preview
     * @param states 
     */
    virtual void OnChannelUserPresenceSnapshotReceived(const std::vector<PresenceState>& states){
    }

    /**
     * @brief 
     * @technical preview
     * @param state 
     */
    virtual void OnChannelUserPresenceChanged(const PresenceState& state){
    }

    /**
     * @brief Called when a remote stream is added
     * @technical preview
     * @param new_streams The newly added remote streams
     */
    virtual void OnRemoteStreamsAdded(const std::vector<RemoteStream>& new_streams, const std::vector<RemoteStreamInfo>& new_streams_info) {
    }

    /**
     * @brief Called when a remote stream is removed
     * @technical preview
     */
    virtual void OnRemoteStreamsRemoved(const std::vector<RemoteStream>& removed_streams, const std::vector<RemoteStreamInfo>& removed_streams_info) {
    }

    /**
     * @brief Called when local stream information is updated
     * @technical preview
     * @param stream The local stream
     * @param info The stream information
     */
    virtual void OnLocalStreamInfo(const LocalStream& stream, const LocalStreamInfo& info) {
    }

    /**
     * @brief Called when remote stream information is updated
     * @technical preview
     * @param stream The remote stream
     * @param info The stream information
     */
    virtual void OnRemoteStreamInfo(const RemoteStream& stream, const RemoteStreamInfo& info) {
    }

    /**
     * @brief Called when track subscription state changes
     * @technical preview
     * @param stream The remote stream
     * @param track The track
     * @param track_media_type The track media type
     * @param old_state The previous subscription state
     * @param new_state The new subscription state
     * @param reason The reason for the state change
     * @param err Error information if any
     */
    virtual void OnSubStateChanged(const RemoteStream& stream, const Track& track, TrackMediaType track_media_type,
                                 TrackSubState old_state, TrackSubState new_state, TrackSubStateChangedReason reason, Error* err) {
    }

    /**
     * @brief Called when track publication state changes
     * @technical preview
     * @param stream The local stream
     * @param track The track
     * @param track_media_type The track media type
     * @param old_state The previous publication state
     * @param new_state The new publication state
     * @param reason The reason for the state change
     * @param err Error information if any
     */
    virtual void OnPubStateChanged(const LocalStream& stream, const Track& track, TrackMediaType track_media_type,
                                 TrackPubState old_state, TrackPubState new_state, TrackPubStateChangedReason reason, Error* err) {
    }

    /**
     * @brief Called when active speaker changes
     * @technical preview
     * @param stream The active speaker
     */
    virtual void OnActiveSpeaker(const Stream& stream) {
    }

    /**
     * @brief Called when audio volume indication is received
     * @technical preview
     * @param audio_volume_infos Array of audio volume information
     * @param audio_volume_infos_cnt Number of audio volume information entries
     */
    virtual void OnAudioVolumeIndication(const std::vector<AudioVolumeInfo>& audio_volume_infos) {
    }

    /**
     * @brief Called when channel token is about to expire
     * @technical preview
     * @param channel_token The channel token that will expire
     */
    virtual void OnChannelTokenWillExpire(const std::string& channel_token) {
    }

    /**
     * @brief Called when stream token is about to expire
     * @technical preview
     * @param stream The stream
     * @param stream_token The stream token that will expire
     */
    virtual void OnStreamTokenWillExpire(const rte::Stream& stream, const std::string& stream_token) {
    }

    /**
     * @brief Called when channel token has expired
     * @technical preview
     */
    virtual void OnChannelTokenExpired() {
    }

    /**
     * @brief Called when stream token has expired
     * @technical preview
     * @param stream The stream
     */
    virtual void OnStreamTokenExpired(const Stream& stream) {
    }

    /**
     * @brief Called when channel stream state snapshot is received
     * @technical preview
     * @param states Array of presence states
     */
    virtual void OnChannelStreamStateSnapshotReceived(const std::vector<PresenceState>& states) {
    }

    /**
     * @brief Called when channel stream state changes
     * @technical preview
     * @param state The new presence state
     */
    virtual void OnChannelStreamStateChanged(const PresenceState& state) {
    }


    /**
     * @brief Called when subscribe result is received
     * @technical preview
     * @param subscribe_type The type of subscribe
     * @param err Error information if any
     */
    virtual void OnSubscribeResult(ChannelSubscribeType subscribe_type, Error* err) {
    }

    /**
     * @brief Called when unsubscribe result is received
     * @technical preview
     * @param subscribe_type The type of subscribe
     * @param err Error information if any
     */
    virtual void OnUnsubscribeResult(ChannelSubscribeType subscribe_type, Error* err) {
    }

    friend class Channel;
  private:
    ::RteChannelObserver* c_observer;
};

void OnRemoteUsersJoined(RteChannelObserver *self, RteRemoteUser *new_users, RteRemoteUserInfo *new_users_info, size_t new_users_cnt){
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr){
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    std::vector<rte::RemoteUser> new_users_vector;
    for(size_t i = 0; i < new_users_cnt; i++){
      rte::RemoteUser remoteUser(new_users[i]);
      new_users_vector.push_back(std::move(remoteUser));
    }

    std::vector<RemoteUserInfo> new_users_info_vector;
    for(size_t i = 0; i < new_users_cnt; i++) {
      // new_users_info_vector.push_back(RemoteUserInfo(&new_users_info[i]));
    }

    observer->OnRemoteUsersJoined(new_users_vector, new_users_info_vector);
  }
}

void OnRemoteUsersLeft(RteChannelObserver *self, RteRemoteUser *removed_users, RteRemoteUserInfo *removed_users_info, size_t removed_users_cnt){
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr){
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    std::vector<rte::RemoteUser> removed_users_vector;
    for(size_t i = 0; i < removed_users_cnt; i++){
      rte::RemoteUser remoteUser(removed_users[i]);
      removed_users_vector.push_back(std::move(remoteUser));
    }

    std::vector<RemoteUserInfo> removed_users_info_vector;
    for(size_t i = 0; i < removed_users_cnt; i++) {
      // removed_users_info_vector.push_back(RemoteUserInfo(&removed_users_info[i]));
    }

    observer->OnRemoteUsersLeft(removed_users_vector, removed_users_info_vector);
  }
}

void OnChannelMessageReceived(RteChannelObserver *self, ::RteString* publisher, RteBuf *message){
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr){
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    String str(publisher);
    Buffer buf(message);
    std::string publisher_str(str.CStr());
    observer->OnChannelMessageReceived(publisher_str, buf);
  }
}

void OnChannelMetadataSnapshotReceived(RteChannelObserver *self, RteMetadata *rte_metadata, RteMetadataInfo *metadata_info){
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr){
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang); 
    Metadata metadata(rte_metadata);
    MetadataInfo metadata_info_obj(metadata_info);
    observer->OnChannelMetadataSnapshotReceived(metadata, metadata_info_obj);
  }
}

void OnChannelMetadataChanged(RteChannelObserver *self, RteMetadata *rte_metadata, RteMetadataInfo *metadata_info){
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr){
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    Metadata metadata(rte_metadata);
    MetadataInfo metadata_info_obj(metadata_info);
    observer->OnChannelMetadataChanged(metadata, metadata_info_obj);
  }
}

void OnChannelUserPresenceSnapshotReceived(RteChannelObserver *self, RtePresenceState *states, size_t states_cnt){
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr){
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    std::vector<PresenceState> states_vector;
    for(size_t i = 0; i < states_cnt; i++){
      states_vector.push_back(PresenceState(&states[i]));
    }
    observer->OnChannelUserPresenceSnapshotReceived(states_vector);
  }
}

void OnChannelUserPresenceChanged(RteChannelObserver *self, RtePresenceState *rte_state){
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr){
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    PresenceState state(rte_state);
    observer->OnChannelUserPresenceChanged(state);
  }
}

void OnRemoteStreamsAdded(RteChannelObserver *self, RteRemoteStream *new_streams, RteRemoteStreamInfo *new_streams_info, size_t new_streams_cnt) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    std::vector<RemoteStream> new_streams_vector;
    for(size_t i = 0; i < new_streams_cnt; i++) {
      new_streams_vector.push_back(RemoteStream(new_streams[i].handle));
    }

    std::vector<RemoteStreamInfo> new_streams_info_vector;
    for(size_t i = 0; i < new_streams_cnt; i++) {
      new_streams_info_vector.push_back(RemoteStreamInfo(&new_streams_info[i]));
    }

    observer->OnRemoteStreamsAdded(new_streams_vector, new_streams_info_vector);
  }
}

void OnRemoteStreamsRemoved(RteChannelObserver *self, RteRemoteStream *removed_streams, RteRemoteStreamInfo *removed_streams_info, size_t removed_streams_cnt) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    std::vector<RemoteStream> removed_streams_vector;
    for(size_t i = 0; i < removed_streams_cnt; i++) {
      // removed_streams_vector.push_back(RemoteStream(&removed_streams[i]));
    }

    std::vector<RemoteStreamInfo> removed_streams_info_vector;
    for(size_t i = 0; i < removed_streams_cnt; i++) {
      // removed_streams_info_vector.push_back(RemoteStreamInfo(&removed_streams_info[i]));
    }

    observer->OnRemoteStreamsRemoved(removed_streams_vector, removed_streams_info_vector);
  }
}

void OnLocalStreamInfo(RteChannelObserver *self, RteLocalStream *stream, RteLocalStreamInfo *info) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    // ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    // LocalStream local_stream(stream);
    // LocalStreamInfo stream_info(info);
    // observer->OnLocalStreamInfo(local_stream, stream_info);
  }
}

void OnRemoteStreamInfo(RteChannelObserver *self, RteRemoteStream *stream, RteRemoteStreamInfo *info) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    // ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    // RemoteStream remote_stream(stream);
    // RemoteStreamInfo stream_info(info);
    // observer->OnRemoteStreamInfo(remote_stream, stream_info);
  }
}

void OnSubStateChanged(RteChannelObserver *self, RteRemoteStream *stream, RteTrack *track, RteTrackMediaType track_media_type,
                      RteTrackSubState old_state, RteTrackSubState new_state, RteTrackSubStateChangedReason reason, RteError *err) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    // ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    // RemoteStream remote_stream(stream);
    // Track track_obj(track);
    // Error error(err);
    // observer->OnSubStateChanged(remote_stream, track_obj, track_media_type, old_state, new_state, reason, &error);
  }
}

void OnPubStateChanged(RteChannelObserver *self, RteLocalStream *stream, RteTrack *track, RteTrackMediaType track_media_type,
                      RteTrackPubState old_state, RteTrackPubState new_state, RteTrackPubStateChangedReason reason, RteError *err) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    // ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    // LocalStream local_stream(stream);
    // Track track_obj(track);
    // Error error(err);
    // observer->OnPubStateChanged(local_stream, track_obj, track_media_type, old_state, new_state, reason, &error);
  }
}

void OnActiveSpeaker(RteChannelObserver *self, RteStream *stream) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    // ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    // Stream stream_obj(stream);
    // observer->OnActiveSpeaker(stream_obj);
  }
}

void OnAudioVolumeIndication(RteChannelObserver *self, RteAudioVolumeInfo *audio_volume_infos, size_t audio_volume_infos_cnt) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    std::vector<AudioVolumeInfo> volume_infos;
    for(size_t i = 0; i < audio_volume_infos_cnt; i++) {
      volume_infos.push_back(AudioVolumeInfo(&audio_volume_infos[i]));
    }
    observer->OnAudioVolumeIndication(volume_infos);
  }
}

void OnChannelTokenWillExpire(RteChannelObserver *self, RteString *channel_token) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    String str(channel_token);
    observer->OnChannelTokenWillExpire(std::string(str.CStr()));
  }
}

void OnStreamTokenWillExpire(RteChannelObserver *self, RteStream *stream, RteString *stream_token) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    // ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    // Stream stream_obj(stream);
    // String str(stream_token);
    // observer->OnStreamTokenWillExpire(stream_obj, std::string(str.CStr()));
  }
}

void OnChannelTokenExpired(RteChannelObserver *self) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    observer->OnChannelTokenExpired();
  }
}

void OnStreamTokenExpired(RteChannelObserver *self, RteStream *stream) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    // ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    // Stream stream_obj(stream);
    // observer->OnStreamTokenExpired(stream_obj);
  }
}

void OnChannelStreamStateSnapshotReceived(RteChannelObserver *self, RtePresenceState *states, size_t states_cnt) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    std::vector<PresenceState> states_vector;
    for(size_t i = 0; i < states_cnt; i++) {
      states_vector.push_back(PresenceState(&states[i]));
    }
    observer->OnChannelStreamStateSnapshotReceived(states_vector);
  }
}

void OnChannelStreamStateChanged(RteChannelObserver *self, RtePresenceState *state) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    PresenceState state_obj(state);
    observer->OnChannelStreamStateChanged(state_obj);
  }
}

void OnSubscribeResult(RteChannelObserver *self, ChannelSubscribeType subscribe_type, RteError *err) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    Error error(err);
    observer->OnSubscribeResult(subscribe_type, &error);
  }
}

void OnUnsubscribeResult(RteChannelObserver *self, ChannelSubscribeType subscribe_type, RteError *err) {
  if(self != nullptr && self->base_observer.me_in_target_lang != nullptr) {
    ChannelObserver* observer = static_cast<ChannelObserver*>(self->base_observer.me_in_target_lang);
    Error error(err);
    observer->OnUnsubscribeResult(subscribe_type, &error);
  }
}

/**
 * @brief 
 * @technical preview
 */
class Channel {
  public:
    Channel(Rte* rte, ChannelConfig* config = nullptr) : c_channel(::RteChannelCreate(&rte->c_rte, config != nullptr ? config->get_underlying_impl() : nullptr, nullptr)){}
    ~Channel(){RteChannelDestroy(&c_channel, nullptr);}

    /**
     * @brief Get the current channel configuration.
     * @note Technical preview.
     * @param config Pointer to a ChannelConfig object to receive the current configuration.
     * @param err Pointer to an Error object to receive error information.
     * @return bool Returns true if the operation succeeds, false otherwise.
     */
    bool GetConfigs(ChannelConfig* config, Error *err){
      return RteChannelGetConfigs(&c_channel, config != nullptr ? config->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Set the channel configuration.
     * @note Technical preview.
     * @param config Pointer to a ChannelConfig object containing the new configuration.
     * @param err Pointer to an Error object to receive error information.
     * @return bool Returns true if the operation succeeds, false otherwise.
     */
    bool SetConfigs(ChannelConfig* config, Error *err){
      return RteChannelSetConfigs(&c_channel, config != nullptr ? config->get_underlying_impl() : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Join the channel with a local user.
     * @note Technical preview.
     * @param local_user Pointer to the LocalUser object representing the user joining the channel.
     * @param token The token string used for authentication.
     * @param err Pointer to an Error object to receive error information.
     * @return bool Returns true if the operation succeeds, false otherwise.
     */
    bool Join(LocalUser* local_user, const std::string& token, Error* err = nullptr){
      String str(token.c_str());
      return RteChannelJoin(&c_channel, local_user != nullptr ? reinterpret_cast<RteLocalUser*>(&local_user->c_user) : nullptr, str.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Leave the channel.
     * @note Technical preview.
     * @param err Pointer to an Error object to receive error information.
     * @return bool Returns true if the operation succeeds, false otherwise.
     */
    bool Leave(Error* err = nullptr){
      return RteChannelLeave(&c_channel, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the local user object associated with this channel.
     * @note Technical preview.
     * @param err Pointer to an Error object to receive error information.
     * @return LocalUser Returns a LocalUser object representing the local user.
     */
    LocalUser GetLocalUser(Error* err){
      return LocalUser(RteChannelGetLocalUser(&c_channel, err != nullptr ? err->get_underlying_impl() : nullptr));
    } 

    /**
     * @brief Get the number of remote users currently in the channel.
     * @note Technical preview.
     * @param err Pointer to an Error object to receive error information.
     * @return size_t Returns the number of remote users in the channel.
     */
    size_t GetRemoteUsersCount(Error* err){
      return RteChannelGetRemoteUsersCount(&c_channel, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get a list of remote users in the channel.
     * @note Technical preview.
     * @param remote_users Reference to a vector where the retrieved remote user objects will be stored.
     * @param start_idx The starting index for retrieval.
     * @param remote_users_cnt The number of remote users to retrieve.
     * @param err Pointer to an Error object for storing error information, default is nullptr.
     * @return bool True if retrieval is successful, false otherwise.
     */
    bool GetRemoteUsers(std::vector<RemoteUser>& remote_users,size_t start_idx, size_t remote_users_cnt, Error* err = nullptr){ 
      bool result = false;


      // std::shared_ptr<RteRemoteUser> remote_users_ptr( new RteRemoteUser[remote_users_count], [](RteRemoteUser *p){ delete[] p; p = nullptr; } );
      // result = RteChannelGetRemoteUsers(&c_channel, remote_users_ptr.get(), 0, remote_users_count, err != nullptr ? err->get_underlying_impl() : nullptr);

      // for(size_t i = 0; i < remote_users_count; i++){
      //   remote_users.push_back(RemoteUser(remote_users_ptr.get()[i]));
      // }
      
      return result;
    }

    /**
     * @brief Renew the channel token.
     * @note Technical preview.
     * @param token The new token string to be used for the channel.
     * @param cb Callback function to be invoked when the token renewal operation is completed. The callback receives an Error pointer.
     */
    void RenewToken(const std::string& token, std::function<void(Error* err)> cb){
      String str(token.c_str());
      CallbackContextWithArgs<Channel>* callbackCtx = new CallbackContextWithArgs<Channel>(this, cb);
      RteChannelRenewToken(&c_channel, str.get_underlying_impl() , &CallbackFuncWithArgs<::RteChannel, Channel>, callbackCtx);
    }

    /**
     * @brief Publish a message to the channel.
     * @note Technical preview.
     * @param buffer Pointer to the Buffer object containing the message to be published.
     * @param cb Callback function to be invoked when the message publishing operation is completed. The callback receives an Error pointer.
     */
    void PublishMessage(Buffer* buffer, std::function<void(Error* err)> cb){
      CallbackContextWithArgs<Channel>* callbackCtx = new CallbackContextWithArgs<Channel>(this, cb);
      RteChannelPublishMessage(&c_channel, buffer != nullptr ? buffer->get_underlying_impl() : nullptr, &CallbackFuncWithArgs<::RteChannel, Channel>, callbackCtx);
    }

    /**
     * @brief Retrieve the metadata of the channel.
     * @note Technical preview.
     * @param cb Callback function to be invoked when the metadata retrieval operation is completed. The callback receives a Metadata pointer and an Error pointer.
     */
    void GetMetadata(std::function<void( Metadata* metadata, Error* err)> cb){
      std::function<void(RteMetadata* metadata, Error* err)> cb_ = [cb](RteMetadata* metadata, Error* err){
        cb(new Metadata(metadata), err);
      };
      CallbackContextWithArgs<Channel, RteMetadata*>* callbackCtx = new CallbackContextWithArgs<Channel, RteMetadata*>(this, cb_);
      RteChannelGetMetadata(&c_channel, &CallbackFuncWithArgs<::RteChannel, Channel, RteMetadata*>, callbackCtx);
    }

    /**
     * @brief Set the metadata for the channel.
     * @note Technical preview.
     * @param metadata Pointer to the Metadata object to be set.
     * @param config Pointer to the MetadataConfig object for configuration.
     * @param cb Callback function to be invoked when the set metadata operation is completed. The callback receives an Error pointer.
     */
    void SetMetadata(Metadata *metadata, MetadataConfig *config, std::function<void(Error* err)> cb ){
      CallbackContextWithArgs<Channel>* callbackCtx = new CallbackContextWithArgs<Channel>(this, cb);
      RteChannelSetMetadata(&c_channel, metadata != nullptr ? metadata->get_underlying_impl() : nullptr, config != nullptr ? config->get_underlying_impl() : nullptr, &CallbackFuncWithArgs<::RteChannel, Channel>, callbackCtx);
    }

    /**
     * @brief Remove metadata from the channel.
     * @note Technical preview.
     * @param metadata Pointer to the Metadata object to be removed.
     * @param config Pointer to the MetadataConfig object for configuration.
     * @param cb Callback function to be invoked when the remove metadata operation is completed. The callback receives an Error pointer.
     */
    void RemoveMetadata(Metadata *metadata, MetadataConfig *config, std::function<void(Error* err)> cb){
      CallbackContextWithArgs<Channel>* callbackCtx = new CallbackContextWithArgs<Channel>(this, cb);
      RteChannelRemoveMetadata(&c_channel, metadata != nullptr ? metadata->get_underlying_impl() : nullptr,config != nullptr ? config->get_underlying_impl() : nullptr, &CallbackFuncWithArgs<::RteChannel, Channel>, callbackCtx);
    }

    /**
     * @brief Get the user presence state in the channel.
     * @note Technical preview.
     * @param user_id The user ID whose presence state is to be retrieved.
     * @param cb Callback function to be invoked when the operation is completed. The callback receives a PresenceState pointer and an Error pointer.
     */
    void GetUserState(const std::string& user_id, std::function<void(PresenceState* state, Error* err)> cb){
      String str(user_id.c_str());
      std::function<void(RtePresenceState* state, Error* err)> cb_ = [cb](RtePresenceState* state, Error* err){
        PresenceState state_cpp(state);
        cb(&state_cpp, err);
      };
      CallbackContextWithArgs<Channel, RtePresenceState*>* callbackCtx = new CallbackContextWithArgs<Channel, RtePresenceState*>(this, cb_);
      RteChannelGetUserState(&c_channel, str.get_underlying_impl(), &CallbackFuncWithArgs<::RteChannel, Channel, RtePresenceState*>, callbackCtx);
    }

    /**
     * @brief Set the user presence state in the channel.
     * @note Technical preview.
     * @param state Pointer to the PresenceState object to be set.
     * @param cb Callback function to be invoked when the operation is completed. The callback receives an Error pointer.
     */
    void SetUserState(PresenceState* state, std::function<void(Error* err)> cb){
      CallbackContextWithArgs<Channel>* callbackCtx = new CallbackContextWithArgs<Channel>(this, cb);
      RteChannelSetUserState(&c_channel, state != nullptr ? state->get_underlying_impl() : nullptr, &CallbackFuncWithArgs<::RteChannel, Channel>, callbackCtx);
    }
    
    /**
     * @brief Remove user presence state(s) from the channel.
     * @note Technical preview.
     * @param keys Vector of keys representing the user state(s) to be removed.
     * @param cb Callback function to be invoked when the operation is completed. The callback receives an Error pointer.
     */
    void RemoveUserState(const std::vector< std::string>& keys, std::function<void(Error* err)> cb){
      std::shared_ptr<RteString*> rte_string_array = nullptr;
      if(keys.size() > 0){

        std::shared_ptr<RteString*> string_array(new RteString*[keys.size()], [size = keys.size()](RteString** p){
          for(size_t i = 0; i < size; i++){
            RteStringDeinit(p[i], nullptr);
            RteStringDestroy(p[i], nullptr);
          }
          delete[] p;
        });

        for(size_t i = 0; i < keys.size(); i++){
          string_array.get()[i] = RteStringCreate(nullptr);
          RteStringInitWithCStr(string_array.get()[i], keys[i].c_str(), nullptr);
        }
        
        rte_string_array = string_array;
      }

      CallbackContextWithArgs<Channel>* callbackCtx = new CallbackContextWithArgs<Channel>(this, cb);
      RteChannelRemoveUserState(&c_channel, rte_string_array.get(), keys.size(), &CallbackFuncWithArgs<::RteChannel, Channel>, callbackCtx);
    }

    /**
     * @brief Register a channel observer.
     * @note Technical preview.
     * @param observer Pointer to the ChannelObserver object to be registered.
     * @param err Pointer to the Error object to store error information.
     * @return bool Returns true if the observer is registered successfully, false otherwise.
     */
    bool RegisterObserver(ChannelObserver* observer, Error *err){
      return RteChannelRegisterObserver(&c_channel, observer != nullptr ? observer->c_observer : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Unregister a channel observer.
     * @note Technical preview.
     * @param observer Pointer to the ChannelObserver object to be unregistered.
     * @param err Pointer to the Error object to store error information.
     * @return bool Returns true if the observer is unregistered successfully, false otherwise.
     */
    bool UnregisterObserver(ChannelObserver* observer, Error *err){
      return RteChannelUnregisterObserver(&c_channel, observer != nullptr ? observer->c_observer : nullptr, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the count of local streams in the channel.
     * @note Technical preview.
     * @param err Pointer to the Error object to store error information.
     * @return size_t Returns the number of local streams in the channel.
     */
    size_t GetLocalStreamsCount(Error* err = nullptr) {
      return RteChannelGetLocalStreamsCount(&c_channel, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Retrieves the local streams in the channel.
     * @note Technical preview.
     * @param err Pointer to the Error object for storing error information.
     * @return std::vector<LocalStream> A vector containing LocalStream objects representing the local streams in the channel.
     */
    bool GetLocalStreams(std::vector<LocalStream> local_streams, size_t start_idx, size_t streams_cnt, Error* err = nullptr) {
      bool result = false;

      if(streams_cnt > 0) {
        std::shared_ptr<RteLocalStream> local_streams_ptr(new RteLocalStream[streams_cnt], [](RteLocalStream *p){ delete[] p; p = nullptr; });
        RteChannelGetLocalStreams(&c_channel, local_streams_ptr.get(), start_idx, streams_cnt, err != nullptr ? err->get_underlying_impl() : nullptr);

        for(size_t i = 0; i < streams_cnt; i++) {
          local_streams.push_back(LocalStream(local_streams_ptr.get()[i]));
        }
      }
      return result;
    }

    /**
     * @brief Publishes a local stream to the channel.
     * @note Technical preview.
     * @param local_user Pointer to the LocalUser who is publishing the stream.
     * @param local_stream Pointer to the LocalStream to be published.
     * @param cb Callback function to be invoked when the operation completes. The callback receives an Error pointer.
     */
    void PublishStream(LocalUser* local_user, LocalStream* local_stream, std::function<void(Error* err)> cb) {
      CallbackContextWithArgs<Channel, RteLocalStream*>* callbackCtx = new CallbackContextWithArgs<Channel, RteLocalStream*>(this, cb, true);
      RteChannelPublishStream(&c_channel, 
        local_user != nullptr ? reinterpret_cast<RteLocalUser*>(&local_user->c_user) : nullptr,
        local_stream != nullptr ? local_stream->get_underlying_impl() : nullptr,
        &CallbackFuncWithArgs<::RteChannel, Channel, RteLocalStream*>, 
        callbackCtx);
    }

    /**
     * @brief Unpublishes a local stream from the channel.
     * @note Technical preview.
     * @param local_stream Pointer to the LocalStream to be unpublished.
     * @param cb Callback function to be invoked when the operation completes. The callback receives an Error pointer.
     */
    void UnpublishStream(LocalStream* local_stream, std::function<void(Error* err)> cb) {
      CallbackContextWithArgs<Channel, RteLocalStream*>* callbackCtx = new CallbackContextWithArgs<Channel, RteLocalStream*>(this, cb, true);
      RteChannelUnpublishStream(&c_channel,
        local_stream != nullptr ? local_stream->get_underlying_impl() : nullptr,
        &CallbackFuncWithArgs<::RteChannel, Channel, RteLocalStream*>,
        callbackCtx);
    }

    /**
     * @brief Gets the count of remote streams in the channel.
     * @note Technical preview.
     * @param err Pointer to the Error object for storing error information.
     * @return size_t The number of remote streams in the channel.
     */
    size_t GetRemoteStreamsCount(Error* err = nullptr) {
      return RteChannelGetRemoteStreamsCount(&c_channel, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Retrieves the list of remote streams for a specified remote user.
     * @note Technical preview.
     * @param user Pointer to the RemoteUser whose streams are to be retrieved.
     * @param remote_streams Reference to a vector where the retrieved RemoteStream objects will be stored.
     * @param start_idx The starting index for retrieval.
     * @param streams_cnt The number of remote streams to retrieve.
     * @param err Pointer to an Error object for storing error information, default is nullptr.
     * @return bool True if retrieval is successful, false otherwise.
     */
    bool GetRemoteStreams(std::vector<RemoteStream>& remote_streams, size_t start_idx, size_t streams_cnt, Error* err = nullptr) {
      bool result = false;

      if(streams_cnt > 0) {
        std::shared_ptr<RteRemoteStream> remote_streams_ptr(new RteRemoteStream[streams_cnt], [](RteRemoteStream *p){ delete[] p; p = nullptr; });

        result = RteChannelGetRemoteStreams(&c_channel,

          remote_streams_ptr.get(), start_idx, streams_cnt,
          err != nullptr ? err->get_underlying_impl() : nullptr);

        for(size_t i = 0; i < streams_cnt; i++) {
          remote_streams.push_back(RemoteStream(remote_streams_ptr.get()[i]));
        }
      }

      return result;
    }

    /**
     * @brief Subscribes to a track in a remote stream.
     * @note Technical preview.
     * @param remote_stream Pointer to the RemoteStream containing the track.
     * @param options Pointer to the SubscribeOptions for the subscription.
     * @param cb Callback function to be invoked when the operation completes. The callback receives a Track pointer and an Error pointer.
     */
    void SubscribeTrack(const std::string& stream_id, SubscribeOptions* options, std::function<void(Track* track, Error* err)> cb) {
      std::function<void(RteTrack* track, Error* err)> cb_ = [cb](RteTrack* track, Error* err) {
        cb(new Track(track), err);
      };
      CallbackContextWithArgs<Channel, RteTrack*>* callbackCtx = new CallbackContextWithArgs<Channel, RteTrack*>(this, cb_);

      String str(stream_id.c_str());
      RteChannelSubscribeTrack(&c_channel,
        str.get_underlying_impl(),
        options != nullptr ? options->get_underlying_impl() : nullptr,
        &CallbackFuncWithArgs<::RteChannel, Channel, RteTrack*>,
        callbackCtx);
    }

    /**
     * @brief Unsubscribes from a track in a remote stream.
     * @note Technical preview.
     * @param remote_stream Pointer to the RemoteStream containing the track.
     * @param options Pointer to the SubscribeOptions for the unsubscription.
     * @param cb Callback function to be invoked when the operation completes. The callback receives an Error pointer.
     */
    void UnsubscribeTrack(const std::string& stream_id, SubscribeOptions* options, std::function<void(Error* err)> cb) {
      CallbackContextWithArgs<Channel, RteString*, RteSubscribeOptions*>* callbackCtx = 
        new CallbackContextWithArgs<Channel, RteString*, RteSubscribeOptions*>(this, cb, true);

      String str(stream_id.c_str());
      RteChannelUnsubscribeTrack(&c_channel,
        str.get_underlying_impl(),
        options != nullptr ? options->get_underlying_impl() : nullptr,
        &CallbackFuncWithArgs<::RteChannel, Channel, RteString*, RteSubscribeOptions*>,
        callbackCtx);
    }

  private:
    RteChannel c_channel;
};

}
