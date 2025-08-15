#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include "IAgoraRtcEngine.h"

// Forward declaration
class RteManager;

// Event handler interface for RteManager
class IRteManagerEventHandler {
public:
    virtual ~IRteManagerEventHandler() = default;
    virtual void OnConnectionStateChanged(int state) {}
    virtual void OnUserJoined(const std::string& userId) {}
    virtual void OnUserLeft(const std::string& userId) {}
    virtual void OnUserListChanged() {}
    virtual void OnLocalAudioStateChanged(int state) {}
    virtual void OnLocalVideoStateChanged(int state, int reason) {}
    virtual void OnRemoteAudioStateChanged(const std::string& userId, int state) {}
    virtual void OnRemoteVideoStateChanged(const std::string& userId, int state) {}
    virtual void OnTokenPrivilegeWillExpire() {}
    virtual void OnError(int err) {}
};

// Configuration for RteManager
struct RteManagerConfig {
    std::string appId;
    std::string userId;
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
    void RenewToken(const std::string& token);

    void SetLocalAudioCaptureEnabled(bool enabled);
    void SetLocalVideoCaptureEnabled(bool enabled);

    void SetSubscribedUsers(const std::vector<std::string>& userIds);
    void SetViewUserBindings(const std::map<void*, std::string>& viewToUserMap);
    int SetupRemoteVideo(const std::string& userId, HWND view);

private:
    // Friend class for event handling
    friend class RteManagerEventHandler;

    void SubscribeRemoteVideo(const std::string& userId);
    void UnsubscribeRemoteVideo(const std::string& userId);
    void SubscribeRemoteAudio(const std::string& userId);
    void UnsubscribeRemoteAudio(const std::string& userId);

private:
    agora::rtc::IRtcEngine* m_rtcEngine;
    std::shared_ptr<agora::rtc::IRtcEngineEventHandler> m_sdkEventHandler;
    IRteManagerEventHandler* m_eventHandler;

    std::string m_appId;
    std::string m_userId;
    std::string m_channelId;

    // Thread-safe members
    std::mutex m_mutex;
    std::map<agora::rtc::uid_t, std::string> m_uid_to_user_id;
    std::map<std::string, agora::rtc::uid_t> m_user_id_to_uid;
    std::vector<std::string> m_subscribedUsers;
    std::map<void*, std::string> m_viewToUserMap;
};

class IRteManagerEventHandler {
public:
    virtual ~IRteManagerEventHandler() {}

    virtual void OnConnectionStateChanged(int state) = 0;
    virtual void OnUserJoined(const std::string& userId) = 0;
    virtual void OnUserLeft(const std::string& userId) = 0;
    virtual void OnLocalAudioStateChanged(int state) = 0;
    virtual void OnLocalVideoStateChanged(agora::rtc::LOCAL_VIDEO_STREAM_STATE state, agora::rtc::LOCAL_VIDEO_STREAM_REASON reason) = 0;
    virtual void OnRemoteAudioStateChanged(const std::string& userId, int state) = 0;
    virtual void OnRemoteVideoStateChanged(const std::string& userId, int state) = 0;
    virtual void OnError(int error) = 0;
    virtual void OnUserListChanged() = 0;
};