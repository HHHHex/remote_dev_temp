#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>

#include "rte_cpp.h"

#include "IRteManagerEventHandler.h"

// Forward declaration
class RteManager;

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

    void SetViewUserBindings(const std::map<void*, std::string>& viewToUserMap);
    int SetupRemoteVideo(const std::string& userId, void* view);

private:
    friend class RteManagerEventObserver;

    void OnRemoteUserJoined(const std::string& userId);
    void OnRemoteUserLeft(const std::string& userId);

private:
    std::shared_ptr<rte::Rte> m_rte;
    std::shared_ptr<rte::LocalUser> m_localUser;
    std::shared_ptr<rte::Channel> m_channel;
    std::shared_ptr<rte::LocalRealTimeStream> m_localStream;
    std::shared_ptr<rte::MicAudioTrack> m_micAudioTrack;
    std::shared_ptr<rte::CameraVideoTrack> m_cameraVideoTrack;
    std::shared_ptr<rte::IChannelObserver> m_channelObserver;


    IRteManagerEventHandler* m_eventHandler;

    std::string m_appId;
    std::string m_userId;
    std::string m_channelId;

    // Thread-safe members
    std::mutex m_mutex;
    std::map<void*, std::string> m_viewToUserMap;
    std::map<std::string, std::shared_ptr<rte::Canvas>> m_remoteUserCanvases;
    std::vector<std::string> m_remoteUsers;
};