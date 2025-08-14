#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "IAgoraRtcEngine.h"

// Forward declaration
class IRteManagerEventHandler;
class RteManagerEventHandler;

struct RteManagerConfig {
    std::string appId;
    std::string userId;
    std::string userToken;
};

class RteManager {
public:
    RteManager();
    ~RteManager();

    void SetEventHandler(IRteManagerEventHandler* handler);
    bool Initialize(const RteManagerConfig& config);
    void Destroy();

    bool JoinChannel(const std::string& channelId, const std::string& token);
    void LeaveChannel();

    void SetLocalAudioCaptureEnabled(bool enabled);
    void SetLocalVideoCaptureEnabled(bool enabled);

    void SubscribeRemoteVideo(const std::string& userId);
    void UnsubscribeRemoteVideo(const std::string& userId);

    void SubscribeRemoteAudio(const std::string& userId);
    void UnsubscribeRemoteAudio(const std::string& userId);

    void SetSubscribedUsers(const std::vector<std::string>& userIds);
    void SetViewUserBindings(const std::map<void*, std::string>& viewToUserMap);

    friend class RteManagerEventHandler;

private:
    agora::rtc::IRtcEngine* m_rtcEngine;
    std::shared_ptr<RteManagerEventHandler> m_sdkEventHandler;
    IRteManagerEventHandler* m_eventHandler;

    std::string m_appId;
    std::string m_userId;
    std::string m_channelId;
    
    std::vector<std::string> m_subscribedUsers;
    std::map<void*, std::string> m_viewToUserMap;
    std::map<agora::rtc::uid_t, std::string> m_uid_to_user_id;
    std::map<std::string, agora::rtc::uid_t> m_user_id_to_uid;
};

class IRteManagerEventHandler {
public:
    virtual ~IRteManagerEventHandler() {}

    virtual void OnConnectionStateChanged(int state) = 0;
    virtual void OnUserJoined(const std::string& userId) = 0;
    virtual void OnUserLeft(const std::string& userId) = 0;
    virtual void OnLocalAudioStateChanged(int state) = 0;
    virtual void OnLocalVideoStateChanged(int state, int reason) = 0;
    virtual void OnRemoteAudioStateChanged(const std::string& userId, int state) = 0;
    virtual void OnRemoteVideoStateChanged(const std::string& userId, int state) = 0;
    virtual void OnError(int error) = 0;
    virtual void OnUserListChanged() = 0;
};