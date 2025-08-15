#include "pch.h"
#include "RteManager.h"
#include "Logger.h"
#include <iterator>
#include <algorithm>
#include <set>

// RTE Event Observer for channel events
class RteManagerEventObserver : public rte::ChannelObserver {
private:
    RteManager* m_rteManager;

public:
    RteManagerEventObserver(RteManager* rteManager) : m_rteManager(rteManager) {}

    // Override the correct virtual functions from ChannelObserver
    void OnRemoteUsersJoined(const std::vector<rte::RemoteUser>& new_users, const std::vector<rte::RemoteUserInfo>& new_users_info) override {
        LOG_INFO("OnRemoteUsersJoined");
        for (const auto& user : new_users) {
            std::string userId = user.GetUserId();
            LOG_INFO_FMT("OnUserJoined: userId=%hs", userId.c_str());
            
            m_rteManager->OnRemoteUserJoined(userId);
            
            if (m_rteManager->m_eventHandler) {
                m_rteManager->m_eventHandler->OnUserJoined(userId);
                m_rteManager->m_eventHandler->OnUserListChanged();
            }
        }
    }

    void OnRemoteUsersLeft(const std::vector<rte::RemoteUser>& removed_users, const std::vector<rte::RemoteUserInfo>& removed_users_info) override {
        LOG_INFO("OnRemoteUsersLeft");
        for (const auto& user : removed_users) {
            std::string userId = user.GetUserId();
            LOG_INFO_FMT("OnUserLeft: userId=%hs", userId.c_str());
            
            m_rteManager->OnRemoteUserLeft(userId);
            
            if (m_rteManager->m_eventHandler) {
                m_rteManager->m_eventHandler->OnUserLeft(userId);
                m_rteManager->m_eventHandler->OnUserListChanged();
            }
        }
    }

    void OnRemoteStreamsAdded(const std::vector<rte::RemoteStream>& new_streams, const std::vector<rte::RemoteStreamInfo>& new_streams_info) override {
        LOG_INFO("OnRemoteStreamsAdded");
        for (const auto& stream : new_streams) {
            rte::RemoteStreamInfo streamInfo;
            stream.GetInfo(&streamInfo, nullptr);
            if (m_rteManager->m_eventHandler && streamInfo.HasAudio()) {
                // Note: We need to get the user ID from the stream info or context
                // For now, we'll use a placeholder approach
                m_rteManager->m_eventHandler->OnRemoteAudioStateChanged("", 2); // REMOTE_AUDIO_STATE_DECODING
            }
        }
    }

    void OnRemoteStreamsRemoved(const std::vector<rte::RemoteStream>& removed_streams, const std::vector<rte::RemoteStreamInfo>& removed_streams_info) override {
        LOG_INFO("OnRemoteStreamsRemoved");
        for (const auto& stream : removed_streams) {
            rte::RemoteStreamInfo streamInfo;
            stream.GetInfo(&streamInfo, nullptr);
            if (m_rteManager->m_eventHandler && streamInfo.HasAudio()) {
                // Note: We need to get the user ID from the stream info or context
                // For now, we'll use a placeholder approach
                m_rteManager->m_eventHandler->OnRemoteAudioStateChanged("", 0); // REMOTE_AUDIO_STATE_STOPPED
            }
        }
    }

    // Connection state handling - these might need to be handled differently
    // as ChannelObserver doesn't have direct OnConnected/OnDisconnected methods
    // We might need to use other callbacks or handle this through different mechanisms
};

RteManager::RteManager() : m_eventHandler(nullptr) {
    LOG_INFO("RteManager created.");
}

RteManager::~RteManager() {
    LOG_INFO("RteManager destroyed.");
    Destroy();
}

void RteManager::SetEventHandler(IRteManagerEventHandler* handler) {
    LOG_INFO("SetEventHandler called.");
    m_eventHandler = handler;
}

bool RteManager::Initialize(const RteManagerConfig& config) {
    LOG_INFO_FMT("Initialize: appId=%hs, userId=%hs", config.appId.c_str(), config.userId.c_str());
    m_appId = config.appId;
    m_userId = config.userId;

    rte::Error err;
    
    // Create RTE instance
    m_rte = std::make_shared<rte::Rte>();
    
    // Configure RTE
    rte::Config rteConfig;
    rteConfig.SetAppId(m_appId.c_str(), &err);
    if (err.Code() != rte::kRteOk) {
        LOG_ERROR_FMT("Initialize failed: SetAppId error=%d", err.Code());
        return false;
    }

    m_rte->SetConfigs(&rteConfig, &err);
    if (err.Code() != rte::kRteOk) {
        LOG_ERROR_FMT("Initialize failed: SetConfigs error=%d", err.Code());
        return false;
    }

    // Initialize media engine
    bool initSuccess = false;
    m_rte->InitMediaEngine([&initSuccess, this](rte::Error* err) {
        if (err && err->Code() == rte::kRteOk) {
            initSuccess = true;
            LOG_INFO("Media engine initialized successfully");
        } else {
            LOG_ERROR_FMT("Media engine initialization failed: error=%d", err ? err->Code() : -1);
        }
    }, nullptr);

    // Wait for initialization (simple blocking approach)
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    if (!initSuccess) {
        LOG_ERROR("Initialize failed: Media engine initialization timeout or error");
        return false;
    }

    // Create local user
    m_localUser = std::make_shared<rte::LocalUser>(m_rte.get());
    
    rte::LocalUserConfig localUserConfig;
    localUserConfig.SetUserId(m_userId);
    
    m_localUser->SetConfigs(&localUserConfig, &err);
    if (err.Code() != rte::kRteOk) {
        LOG_ERROR_FMT("Initialize failed: LocalUser SetConfigs error=%d", err.Code());
        return false;
    }

    // Create media tracks
    m_micAudioTrack = std::make_shared<rte::MicAudioTrack>(m_rte.get());
    rte::MicAudioTrackConfig micConfig;
    m_micAudioTrack->SetConfigs(&micConfig, &err);
    if (err.Code() != rte::kRteOk) {
        LOG_ERROR_FMT("Initialize failed: MicAudioTrack SetConfigs error=%d", err.Code());
        return false;
    }

    m_cameraVideoTrack = std::make_shared<rte::CameraVideoTrack>(m_rte.get());
    rte::CameraVideoTrackConfig cameraConfig;
    m_cameraVideoTrack->SetConfigs(&cameraConfig, &err);
    if (err.Code() != rte::kRteOk) {
        LOG_ERROR_FMT("Initialize failed: CameraVideoTrack SetConfigs error=%d", err.Code());
        return false;
    }

    LOG_INFO("Initialize successful.");
    return true;
}

void RteManager::Destroy() {
    LOG_INFO("Destroy called.");
    
    // Leave channel if connected
    if (m_channel) {
        LeaveChannel();
    }
    
    // Stop and release media tracks
    if (m_micAudioTrack) {
        m_micAudioTrack->Stop(nullptr);
        m_micAudioTrack.reset();
    }
    
    if (m_cameraVideoTrack) {
        m_cameraVideoTrack->Stop(nullptr);
        m_cameraVideoTrack.reset();
    }
    
    // Release local stream and user
    if (m_localStream) {
        m_localStream.reset();
    }
    
    if (m_localUser) {
        m_localUser.reset();
    }
    
    // Release RTE
    if (m_rte) {
        m_rte->Destroy();
        m_rte.reset();
        LOG_INFO("RTE destroyed.");
    }
}

bool RteManager::JoinChannel(const std::string& channelId, const std::string& token) {
    LOG_INFO_FMT("JoinChannel: channelId=%hs", channelId.c_str());
    m_channelId = channelId;
    
    if (!m_rte || !m_localUser) {
        LOG_ERROR("JoinChannel failed: RTE or LocalUser not initialized");
        return false;
    }
    
    rte::Error err;
    
    // Update user token if provided
    if (!token.empty()) {
        rte::LocalUserConfig localUserConfig;
        m_localUser->GetConfigs(&localUserConfig, &err);
        localUserConfig.SetUserToken(token);
        m_localUser->SetConfigs(&localUserConfig, &err);
        if (err.Code() != rte::kRteOk) {
            LOG_ERROR_FMT("JoinChannel failed: SetUserToken error=%d", err.Code());
            return false;
        }
    }
    
    // Connect local user
    bool connectSuccess = false;
    m_localUser->Connect([&connectSuccess, this](rte::Error* err) {
        if (err && err->Code() == rte::kRteOk) {
            connectSuccess = true;
            LOG_INFO("Local user connected successfully");
        } else {
            LOG_ERROR_FMT("Local user connection failed: error=%d", err ? err->Code() : -1);
        }
    });
    
    // Wait for connection
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    
    if (!connectSuccess) {
        LOG_ERROR("JoinChannel failed: Local user connection timeout");
        return false;
    }
    
    // Create and configure channel
    m_channel = std::make_shared<rte::Channel>(m_rte.get());
    m_channelObserver = std::make_shared<RteManagerEventObserver>(this);
    
    rte::ChannelConfig channelConfig;
    channelConfig.SetChannelId(channelId);
    
    m_channel->SetConfigs(&channelConfig, &err);
    if (err.Code() != rte::kRteOk) {
        LOG_ERROR_FMT("JoinChannel failed: Channel SetConfigs error=%d", err.Code());
        return false;
    }
    
    m_channel->RegisterObserver(m_channelObserver.get(), &err);
    if (err.Code() != rte::kRteOk) {
        LOG_ERROR_FMT("JoinChannel failed: RegisterObserver error=%d", err.Code());
        return false;
    }
    
    // Join channel
    bool joinSuccess = m_channel->Join(m_localUser.get(), token, &err);
    if (!joinSuccess || err.Code() != rte::kRteOk) {
        LOG_ERROR_FMT("JoinChannel failed: Join error=%d", err.Code());
        return false;
    }
    
    LOG_INFO("Channel joined successfully");
    
    // Create and publish local stream
    m_localStream = std::make_shared<rte::LocalRealTimeStream>(m_rte.get());
    
    // Add tracks to stream
    m_localStream->AddAudioTrack(m_micAudioTrack.get(), &err);
    if (err.Code() != rte::kRteOk) {
        LOG_ERROR_FMT("JoinChannel warning: AddAudioTrack error=%d", err.Code());
    }
    
    m_localStream->AddVideoTrack(m_cameraVideoTrack.get(), &err);
    if (err.Code() != rte::kRteOk) {
        LOG_ERROR_FMT("JoinChannel warning: AddVideoTrack error=%d", err.Code());
    }
    
    // Publish stream to channel
    m_channel->PublishStream(m_localUser.get(), m_localStream.get(), [this](rte::Error* err) {
        if (err && err->Code() == rte::kRteOk) {
            LOG_INFO("Local stream published successfully");
        } else {
            LOG_ERROR_FMT("Publish stream failed: error=%d", err ? err->Code() : -1);
        }
    });
    
    LOG_INFO("JoinChannel successful");
    return true;
}

void RteManager::LeaveChannel() {
    LOG_INFO_FMT("LeaveChannel: channelId=%hs", m_channelId.c_str());
    
    if (m_channel) {
        rte::Error err;
        
        // Unpublish local stream
        if (m_localStream) {
            m_channel->UnpublishStream(m_localStream.get(), [this](rte::Error* err) {
                if (err && err->Code() == rte::kRteOk) {
                    LOG_INFO("Local stream unpublished successfully");
                } else {
                    LOG_ERROR_FMT("Unpublish stream failed: error=%d", err ? err->Code() : -1);
                }
            });
            m_localStream.reset();
        }
        
        // Leave channel
        m_channel->Leave(&err);
        
        // Unregister observer
        if (m_channelObserver) {
            m_channel->UnregisterObserver(m_channelObserver.get(), &err);
            m_channelObserver.reset();
        }
        
        m_channel.reset();
        LOG_INFO("Channel left successfully");
    }
    
    // Disconnect local user
    if (m_localUser) {
        rte::Error err;
        m_localUser->Disconnect(&err);
    }
    
    // Clear remote user data
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_remoteUsers.clear();
        m_remoteUserCanvases.clear();
        m_viewToUserMap.clear();
    }
}

void RteManager::RenewToken(const std::string& token) {
    LOG_INFO("RenewToken called.");
    if (m_localUser) {
        rte::Error err;
        rte::LocalUserConfig localUserConfig;
        m_localUser->GetConfigs(&localUserConfig, &err);
        localUserConfig.SetUserToken(token);
        m_localUser->SetConfigs(&localUserConfig, &err);
        if (err.Code() != rte::kRteOk) {
            LOG_ERROR_FMT("RenewToken failed: error=%d", err.Code());
        }
    }
}

void RteManager::SetLocalAudioCaptureEnabled(bool enabled) {
    LOG_INFO_FMT("SetLocalAudioCaptureEnabled: enabled=%d", enabled);
    if (m_micAudioTrack) {
        if (enabled) {
            m_micAudioTrack->Start(nullptr);
        } else {
            m_micAudioTrack->Stop(nullptr);
        }
        if (m_eventHandler) {
            int state = enabled ? 1 : 0; // Corresponds to LOCAL_AUDIO_STREAM_STATE_RECORDING and LOCAL_AUDIO_STREAM_STATE_STOPPED
            m_eventHandler->OnLocalAudioStateChanged(state);
        }
    }
}

void RteManager::SetLocalVideoCaptureEnabled(bool enabled) {
    LOG_INFO_FMT("SetLocalVideoCaptureEnabled: enabled=%d", enabled);
    if (m_cameraVideoTrack) {
        if (enabled) {
            m_cameraVideoTrack->Start(nullptr);
        } else {
            m_cameraVideoTrack->Stop(nullptr);
        }
    }
}

void RteManager::SetViewUserBindings(const std::map<void*, std::string>& viewToUserMap) {
    LOG_INFO("SetViewUserBindings called.");
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Clear existing canvases for views that are no longer bound
    for (const auto& pair : m_viewToUserMap) {
        void* view = pair.first;
        const std::string& userId = pair.second;
        if (viewToUserMap.find(view) == viewToUserMap.end() || viewToUserMap.at(view) != userId) {
            LOG_INFO_FMT("Unbinding view for user: %hs", userId.c_str());
            if (m_remoteUserCanvases.count(userId)) {
                m_remoteUserCanvases.erase(userId);
            }
        }
    }
    
    // Create new canvases for new view bindings
    for (const auto& pair : viewToUserMap) {
        void* view = pair.first;
        const std::string& userId = pair.second;
        if (m_viewToUserMap.find(view) == m_viewToUserMap.end() || m_viewToUserMap.at(view) != userId) {
            LOG_INFO_FMT("Binding view for user: %hs", userId.c_str());
            if (m_rte) {
                auto canvas = std::make_shared<rte::Canvas>(m_rte.get());
                rte::Error err;
                rte::View rteView = reinterpret_cast<rte::View>(view);
                rte::ViewConfig viewConfig;
                canvas->AddView(&rteView, &viewConfig, &err);
                if (err.Code() == rte::kRteOk) {
                    m_remoteUserCanvases[userId] = canvas;
                } else {
                    LOG_ERROR_FMT("Failed to create canvas for user %hs: error=%d", userId.c_str(), err.Code());
                }
            }
        }
    }
    
    m_viewToUserMap = viewToUserMap;
}

int RteManager::SetupRemoteVideo(const std::string& userId, void* view) {
    LOG_INFO_FMT("SetupRemoteVideo for user: %hs", userId.c_str());
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_rte) {
        LOG_ERROR("SetupRemoteVideo failed: RTE not initialized");
        return -1;
    }
    
    if (!view) {
        // Remove canvas for user
        if (m_remoteUserCanvases.count(userId)) {
            m_remoteUserCanvases.erase(userId);
            LOG_INFO_FMT("Removed canvas for user: %hs", userId.c_str());
        }
        return 0;
    }
    
    // Create canvas for user
    auto canvas = std::make_shared<rte::Canvas>(m_rte.get());
    rte::Error err;
    rte::View rteView = reinterpret_cast<rte::View>(view);
    rte::ViewConfig viewConfig;
    canvas->AddView(&rteView, &viewConfig, &err);
    
    if (err.Code() == rte::kRteOk) {
        rte::CanvasConfig canvasConfig;
        canvasConfig.SetRenderMode(rte::VideoRenderMode::kRteVideoRenderModeFit);
        canvas->SetConfigs(&canvasConfig, &err);
        
        if (err.Code() == rte::kRteOk) {
            m_remoteUserCanvases[userId] = canvas;
            LOG_INFO_FMT("Created canvas for user: %hs", userId.c_str());
            return 0;
        }
    }
    
    LOG_ERROR_FMT("SetupRemoteVideo failed for user: %hs, error=%d", userId.c_str(), err.Code());
    return -1;
}

void RteManager::OnRemoteUserJoined(const std::string& userId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_remoteUsers.push_back(userId);
    LOG_INFO_FMT("Remote user joined: %hs", userId.c_str());
}

void RteManager::OnRemoteUserLeft(const std::string& userId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find(m_remoteUsers.begin(), m_remoteUsers.end(), userId);
    if (it != m_remoteUsers.end()) {
        m_remoteUsers.erase(it);
    }
    
    // Clean up canvas for this user
    if (m_remoteUserCanvases.count(userId)) {
        m_remoteUserCanvases.erase(userId);
    }
    
    LOG_INFO_FMT("Remote user left: %hs", userId.c_str());
}