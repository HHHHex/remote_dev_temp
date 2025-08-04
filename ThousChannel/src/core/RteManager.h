#pragma once

#include <string>
#include <vector>
#include <map>

// Forward declaration
class IRteManagerEventHandler;

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

private:
    IRteManagerEventHandler* m_eventHandler;
};

class IRteManagerEventHandler {
public:
    virtual ~IRteManagerEventHandler() {}

    virtual void OnConnectionStateChanged(int state) = 0;
    virtual void OnUserJoined(const std::string& userId) = 0;
    virtual void OnUserLeft(const std::string& userId) = 0;
    virtual void OnLocalAudioStateChanged(int state) = 0;
    virtual void OnLocalVideoStateChanged(int state) = 0;
    virtual void OnRemoteAudioStateChanged(const std::string& userId, int state) = 0;
    virtual void OnRemoteVideoStateChanged(const std::string& userId, int state) = 0;
    virtual void OnError(int error) = 0;
    virtual void OnUserListChanged() = 0;
};