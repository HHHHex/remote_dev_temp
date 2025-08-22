#include "pch.h"
#include "RteManager.h"
#include "Logger.h"
#include <atomic>
#include <future>
#include <chrono>
#include <algorithm>

// AsyncResult class for handling asynchronous operations
template<typename T>
class AsyncResult {
public:
    AsyncResult() : valid_(false) {}
    
    void SetResult(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ = value;
        valid_ = true;
        cv_.notify_all();
    }
    
    struct Result {
        bool valid;
        T value;
    };
    
    Result Wait(int timeoutMs) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs), [this] { return valid_; })) {
            return {true, value_};
        }
        return {false, T{}};
    }
    
private:
    T value_;
    bool valid_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// RTE Event Observer for channel events
class RteManagerEventObserver : public rte::ChannelObserver {
private:
    RteManager* m_rteManager;

public:
    RteManagerEventObserver(RteManager* rteManager) : m_rteManager(rteManager) {}

    // Override the correct virtual functions from ChannelObserver
    void OnRemoteUsersJoined(const std::vector<rte::RemoteUser>& new_users, const std::vector<rte::RemoteUserInfo>& new_users_info) override {
        LOG_INFO("OnRemoteUsersJoined");
        for (size_t i = 0; i < new_users.size(); ++i) {
            std::string userId = new_users_info[i].UserId();
            LOG_INFO_FMT("OnUserJoined: userId={}", userId);
            
            m_rteManager->OnRemoteUserJoined(userId);
            
            if (m_rteManager->m_eventHandler) {
                m_rteManager->m_eventHandler->OnUserJoined(userId);
                m_rteManager->m_eventHandler->OnUserListChanged();
            }
        }
    }

    void OnRemoteUsersLeft(const std::vector<rte::RemoteUser>& removed_users, const std::vector<rte::RemoteUserInfo>& removed_users_info) override {
        LOG_INFO("OnRemoteUsersLeft");
        for (size_t i = 0; i < removed_users.size(); ++i) {
            std::string userId = removed_users_info[i].UserId();
            LOG_INFO_FMT("OnUserLeft: userId={}", userId);
            
            m_rteManager->OnRemoteUserLeft(userId);
            
            if (m_rteManager->m_eventHandler) {
                m_rteManager->m_eventHandler->OnUserLeft(userId);
                m_rteManager->m_eventHandler->OnUserListChanged();
            }
        }
    }

    void OnRemoteStreamsAdded(const std::vector<rte::RemoteStream>& new_streams, const std::vector<rte::RemoteStreamInfo>& new_streams_info) override {
        LOG_INFO("OnRemoteStreamsAdded");
        for (size_t i = 0; i < new_streams.size(); ++i) {
            // TODO: Fix HasAudio() const issue - temporarily commented out
            // if (m_rteManager->m_eventHandler && new_streams_info[i].HasAudio()) {
            //     // Note: We need to get the user ID from the stream info or context
            //     // For now, we'll use a placeholder approach
            //     m_rteManager->m_eventHandler->OnRemoteAudioStateChanged("", 2); // REMOTE_AUDIO_STATE_DECODING
            // }
        }
    }

    void OnRemoteStreamsRemoved(const std::vector<rte::RemoteStream>& removed_streams, const std::vector<rte::RemoteStreamInfo>& removed_streams_info) override {
        LOG_INFO("OnRemoteStreamsRemoved");
        for (size_t i = 0; i < removed_streams.size(); ++i) {
            // TODO: Fix HasAudio() const issue - temporarily commented out
            // if (m_rteManager->m_eventHandler && removed_streams_info[i].HasAudio()) {
            //     // Note: We need to get the user ID from the stream info or context
            //     // For now, we'll use a placeholder approach
            //     m_rteManager->m_eventHandler->OnRemoteAudioStateChanged("", 0); // REMOTE_AUDIO_STATE_STOPPED
            // }
        }
    }

    // Connection state handling - these might need to be handled differently
    // as ChannelObserver doesn't have direct OnConnected/OnDisconnected methods
    // We might need to use other callbacks or handle this through different mechanisms
};

RteManager::RteManager() : m_eventHandler(nullptr) {
    LOG_INFO("RteManager created.");
    
    // Test new std::string interface
    LOG_INFO("Testing new std::string interface");
    LOG_INFO("Testing direct std::string");
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
    LOG_INFO_FMT("Initialize: appId={}, userId={}", config.appId, config.userId);
    m_appId = config.appId;
    m_userId = config.userId;

    rte::Error err;
    
    // Create RTE instance
    m_rte = std::make_shared<rte::Rte>();
    
    // Configure RTE
    rte::Config rteConfig;
    rteConfig.SetAppId(m_appId.c_str(), &err);
    if (err.Code() != kRteOk) {
        LOG_ERROR_FMT("Initialize failed: SetAppId error={}", err.Code());
        return false;
    }

    m_rte->SetConfigs(&rteConfig, &err);
    if (err.Code() != kRteOk) {
        LOG_ERROR_FMT("Initialize failed: SetConfigs error={}", err.Code());
        return false;
    }

    // Initialize media engine
    bool initSuccess = false;
    m_rte->InitMediaEngine([&initSuccess, this](rte::Error* err) {
        if (err && err->Code() == kRteOk) {
            initSuccess = true;
            LOG_INFO("Media engine initialized successfully");
        } else {
            LOG_ERROR_FMT("Media engine initialization failed: error={}", err ? err->Code() : -1);
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
    localUserConfig.SetUserId(m_userId.c_str());
    LOG_INFO_FMT("localUserConfig.SetUserId: {}", m_userId);
    m_localUser->SetConfigs(&localUserConfig, &err);
    if (err.Code() != kRteOk) {
        LOG_ERROR_FMT("Initialize failed: LocalUser SetConfigs error={}", err.Code());
        return false;
    }

    // Create media tracks
    m_micAudioTrack = std::make_shared<rte::MicAudioTrack>(m_rte.get());
    {
        rte::MicAudioTrackConfig micConfig;
        // Use minimal configuration to avoid audio device issues
        micConfig.SetRecordingVolume(50);  // Reduced volume
        // Don't set JSON parameter to let RTE use system default
        m_micAudioTrack->SetConfigs(&micConfig, &err);
        if (err.Code() != kRteOk) {
            LOG_ERROR_FMT("Initialize failed: MicAudioTrack SetConfigs error={}", err.Code());
            return false;
        }
    } // micConfig goes out of scope here and is properly destroyed

    m_cameraVideoTrack = std::make_shared<rte::CameraVideoTrack>(m_rte.get());
    {
        rte::CameraVideoTrackConfig cameraConfig;
        m_cameraVideoTrack->SetConfigs(&cameraConfig, &err);
        if (err.Code() != kRteOk) {
            LOG_ERROR_FMT("Initialize failed: CameraVideoTrack SetConfigs error={}", err.Code());
            return false;
        }
    } // cameraConfig goes out of scope here and is properly destroyed

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
        m_micAudioTrack->Stop([](rte::Error* err) {
            if (err && err->Code() != kRteOk) {
                LOG_ERROR_FMT("MicAudioTrack Stop failed: error={}", err->Code());
            } else {
                LOG_INFO("MicAudioTrack stopped successfully");
            }
        });
        m_micAudioTrack.reset();
    }
    
    if (m_cameraVideoTrack) {
        m_cameraVideoTrack->Stop([](rte::Error* err) {
            if (err && err->Code() != kRteOk) {
                LOG_ERROR_FMT("CameraVideoTrack Stop failed: error={}", err->Code());
            } else {
                LOG_INFO("CameraVideoTrack stopped successfully");
            }
        });
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
    LOG_INFO_FMT("JoinChannel: channelId={}", channelId);
    m_channelId = channelId;
    
    if (!m_rte || !m_localUser) {
        LOG_ERROR("JoinChannel failed: RTE or LocalUser not initialized");
        return false;
    }
    
    rte::Error err;
    
    // Update user token if provided
    if (!token.empty()) {
        {
            rte::LocalUserConfig localUserConfig;
            m_localUser->GetConfigs(&localUserConfig, &err);
            localUserConfig.SetUserToken(token.c_str());
            m_localUser->SetConfigs(&localUserConfig, &err);
                    if (err.Code() != kRteOk) {
            LOG_ERROR_FMT("JoinChannel failed: SetUserToken error={}", err.Code());
            return false;
        }
        } // localUserConfig goes out of scope here
    }
    
    // Connect local user
    std::atomic<bool> connectSuccess(false);
    m_localUser->Connect([&connectSuccess](rte::Error* err) {
        if (err && err->Code() == kRteOk) {
            connectSuccess = true;
            LOG_INFO("Local user connected successfully");
        } else {
            LOG_ERROR_FMT("Local user connection failed: error={}", err ? err->Code() : -1);
        }
    });
    
    // Wait for connection with longer timeout
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    
    if (!connectSuccess.load()) {
        LOG_ERROR("JoinChannel failed: Local user connection timeout");
        return false;
    }
    
    // Create and configure channel
    m_channel = std::make_shared<rte::Channel>(m_rte.get());
    m_channelObserver = std::make_shared<RteManagerEventObserver>(this);
    
    {
        rte::ChannelConfig channelConfig;
        channelConfig.SetChannelId(channelId.c_str());
        
        m_channel->SetConfigs(&channelConfig, &err);
        if (err.Code() != kRteOk) {
            LOG_ERROR_FMT("JoinChannel failed: Channel SetConfigs error={}", err.Code());
            return false;
        }
    } // channelConfig goes out of scope here
    
    m_channel->RegisterObserver(m_channelObserver.get(), &err);
    if (err.Code() != kRteOk) {
        LOG_ERROR("JoinChannel failed: RegisterObserver error={}", err.Code());
        return false;
    }
    
    // Join channel
    bool joinSuccess = m_channel->Join(m_localUser.get(), token.c_str(), &err);
    if (!joinSuccess || err.Code() != kRteOk) {
        LOG_ERROR_FMT("JoinChannel failed: Join error={}", err.Code());
        return false;
    }
    
    LOG_INFO("Channel joined successfully");
    
    // Start audio and video tracks before adding to stream - following UT pattern
    std::atomic<bool> audioTrackStarted(false);
    std::atomic<bool> videoTrackStarted(false);
    
    // Start mic audio track first
    {
        std::shared_ptr<AsyncResult<bool>> mic_audio_track_start_result = std::make_shared<AsyncResult<bool>>();
        m_micAudioTrack->Start([&audioTrackStarted, mic_audio_track_start_result](rte::Error* err) {
            if (err && err->Code() == kRteOk) {
                audioTrackStarted.store(true);
                mic_audio_track_start_result->SetResult(true);
                LOG_INFO("MicAudioTrack started successfully");
            } else {
                mic_audio_track_start_result->SetResult(false);
                LOG_ERROR_FMT("MicAudioTrack start failed: error={}", err ? err->Code() : -1);
            }
        });

        auto result = mic_audio_track_start_result->Wait(5000); // 5 second timeout
        if (!result.valid || !result.value) {
            LOG_WARN("MicAudioTrack failed to start, continuing anyway");
        }
    }
    
    // Start camera video track
    {
        std::shared_ptr<AsyncResult<bool>> camera_video_track_start_result = std::make_shared<AsyncResult<bool>>();
        m_cameraVideoTrack->Start([&videoTrackStarted, camera_video_track_start_result](rte::Error* err) {
            if (err && err->Code() == kRteOk) {
                videoTrackStarted.store(true);
                camera_video_track_start_result->SetResult(true);
                LOG_INFO("CameraVideoTrack started successfully");
            } else {
                camera_video_track_start_result->SetResult(false);
                LOG_ERROR_FMT("CameraVideoTrack start failed: error={}", err ? err->Code() : -1);
            }
        });

        auto result = camera_video_track_start_result->Wait(5000); // 5 second timeout
        if (!result.valid || !result.value) {
            LOG_WARN("CameraVideoTrack failed to start, continuing anyway");
        }
    }
    
    // Create and publish local stream
    m_localStream = std::make_shared<rte::LocalRealTimeStream>(m_rte.get());
    
    // Add tracks to stream - only add audio if it started successfully
    if (audioTrackStarted.load()) {
        m_localStream->AddAudioTrack(m_micAudioTrack.get(), &err);
        if (err.Code() != kRteOk) {
            LOG_ERROR_FMT("JoinChannel warning: AddAudioTrack error={}", err.Code());
        } else {
            LOG_INFO("Audio track added to stream successfully");
        }
    } else {
        LOG_WARN("Skipping audio track addition due to initialization failure");
    }
    
    // Always try to add video track
    m_localStream->AddVideoTrack(m_cameraVideoTrack.get(), &err);
            if (err.Code() != kRteOk) {
            LOG_ERROR_FMT("JoinChannel warning: AddVideoTrack error={}", err.Code());
        } else {
            LOG_INFO("Video track added to stream successfully");
        }
    
    // Publish stream to channel
    m_channel->PublishStream(m_localUser.get(), m_localStream.get(), [this](rte::Error* err) {
        if (err && err->Code() == kRteOk) {
            LOG_INFO("Local stream published successfully");
        } else {
            LOG_ERROR_FMT("Publish stream failed: error={}", err ? err->Code() : -1);
        }
    });
    
    LOG_INFO("JoinChannel successful");
    return true;
}

void RteManager::LeaveChannel() {
    LOG_INFO_FMT("LeaveChannel: channelId={}", m_channelId);
    
    if (m_channel) {
        rte::Error err;
        
        // Unpublish local stream
        if (m_localStream) {
            m_channel->UnpublishStream(m_localStream.get(), [this](rte::Error* err) {
                if (err && err->Code() == kRteOk) {
                    LOG_INFO("Local stream unpublished successfully");
                        } else {
            LOG_ERROR_FMT("Unpublish stream failed: error={}", err ? err->Code() : -1);
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
        m_localUser->Disconnect([](rte::Error* err) {
                    if (err && err->Code() != kRteOk) {
            LOG_ERROR_FMT("LocalUser Disconnect failed: error={}", err->Code());
        } else {
            LOG_INFO("LocalUser disconnected successfully");
        }
        });
    }
    
    // Clear remote user data
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_viewToUserMap.clear();
        m_videoSubscribedUsers.clear();
        m_audioSubscribedUsers.clear();
    }
}

void RteManager::RenewToken(const std::string& token) {
    LOG_INFO("RenewToken called.");
    if (m_localUser) {
        rte::Error err;
        rte::LocalUserConfig localUserConfig;
        m_localUser->GetConfigs(&localUserConfig, &err);
        localUserConfig.SetUserToken(token.c_str());
        m_localUser->SetConfigs(&localUserConfig, &err);
        if (err.Code() != kRteOk) {
            LOG_ERROR_FMT("RenewToken failed: error={}", err.Code());
        }
    }
}

void RteManager::SetLocalAudioCaptureEnabled(bool enabled) {
    LOG_INFO_FMT("SetLocalAudioCaptureEnabled: enabled={}", enabled);
    if (m_micAudioTrack) {
        if (enabled) {
            // Add delay to ensure RTE is fully initialized
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            
            // Start audio track with simple retry mechanism
            m_micAudioTrack->Start([](rte::Error* err) {
                            if (err && err->Code() != kRteOk) {
                LOG_ERROR_FMT("MicAudioTrack Start failed: error={}", err->Code());
            } else {
                LOG_INFO("MicAudioTrack started successfully");
            }
            });
        } else {
            m_micAudioTrack->Stop([](rte::Error* err) {
                            if (err && err->Code() != kRteOk) {
                LOG_ERROR_FMT("MicAudioTrack Stop failed: error={}", err->Code());
            } else {
                LOG_INFO("MicAudioTrack stopped successfully");
            }
            });
        }
        if (m_eventHandler) {
            int state = enabled ? 1 : 0; // Corresponds to LOCAL_AUDIO_STREAM_STATE_RECORDING and LOCAL_AUDIO_STREAM_STATE_STOPPED
            m_eventHandler->OnLocalAudioStateChanged(state);
        }
    }
}

void RteManager::SetLocalVideoCaptureEnabled(bool enabled) {
    LOG_INFO_FMT("SetLocalVideoCaptureEnabled: enabled={}", enabled);
    if (m_cameraVideoTrack) {
        if (enabled) {
            m_cameraVideoTrack->Start([](rte::Error* err) {
                            if (err && err->Code() != kRteOk) {
                LOG_ERROR_FMT("CameraVideoTrack Start failed: error={}", err->Code());
            } else {
                LOG_INFO("CameraVideoTrack started successfully");
            }
            });
        } else {
            m_cameraVideoTrack->Stop([](rte::Error* err) {
                            if (err && err->Code() != kRteOk) {
                LOG_ERROR_FMT("CameraVideoTrack Stop failed: error={}", err->Code());
            } else {
                LOG_INFO("CameraVideoTrack stopped successfully");
            }
            });
        }
    }
}

void RteManager::SetViewUserBindings(const std::map<void*, std::string>& viewToUserMap) {
    LOG_INFO("SetViewUserBindings called.");
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_rte || !m_channel) {
        LOG_ERROR("SetViewUserBindings failed: RTE or Channel not initialized");
        return;
    }
    
    // Check for views that are no longer bound and need cleanup
    for (const auto& pair : m_viewToUserMap) {
        void* view = pair.first;
        const std::string& userId = pair.second;
        if (viewToUserMap.find(view) == viewToUserMap.end() || viewToUserMap.at(view) != userId) {
            LOG_INFO_FMT("Unbinding view for user: {}", userId);
            
            // Unbind user from view using RTE SDK
            rte::Error err;
            rte::View rteView = reinterpret_cast<rte::View>(view);
            
            // Get remote users from channel
            auto remoteUsers = m_channel->GetRemoteUsers(&err);
            if (err.Code() == kRteOk) {
                for (const auto& remoteUser : remoteUsers) {
                    if (remoteUser.UserId() == userId) {
                        // Unbind user from view
                        remoteUser.UnbindFromView(&rteView, &err);
                        if (err.Code() == kRteOk) {
                            LOG_INFO_FMT("Successfully unbound user {} from view", userId);
                        } else {
                            LOG_ERROR_FMT("Failed to unbind user {} from view: error={}", userId, err.Code());
                        }
                        break;
                    }
                }
            } else {
                LOG_ERROR_FMT("Failed to get remote users for unbinding: error={}", err.Code());
            }
        }
    }
    
    // Check for new view bindings that need setup
    for (const auto& pair : viewToUserMap) {
        void* view = pair.first;
        const std::string& userId = pair.second;
        if (m_viewToUserMap.find(view) == m_viewToUserMap.end() || m_viewToUserMap.at(view) != userId) {
            LOG_INFO_FMT("Binding view for user: {}", userId);
            
            // Bind user to view using RTE SDK
            rte::Error err;
            rte::View rteView = reinterpret_cast<rte::View>(view);
            
            // Get remote users from channel
            auto remoteUsers = m_channel->GetRemoteUsers(&err);
            if (err.Code() == kRteOk) {
                for (const auto& remoteUser : remoteUsers) {
                    if (remoteUser.UserId() == userId) {
                        // Bind user to view
                        remoteUser.BindToView(&rteView, &err);
                        if (err.Code() == kRteOk) {
                            LOG_INFO_FMT("Successfully bound user {} to view", userId);
                        } else {
                            LOG_ERROR_FMT("Failed to bind user {} to view: error={}", userId, err.Code());
                        }
                        break;
                    }
                }
            } else {
                LOG_ERROR_FMT("Failed to get remote users for binding: error={}", err.Code());
            }
        }
    }
    
    m_viewToUserMap = viewToUserMap;
}

void RteManager::OnRemoteUserJoined(const std::string& userId) {
    LOG_INFO_FMT("Remote user joined: {}", userId);
}

void RteManager::OnRemoteUserLeft(const std::string& userId) {
    LOG_INFO_FMT("Remote user left: {}", userId);
}

void RteManager::UpdateVideoSubscriptions(const std::vector<std::string>& videoSubscribedUsers) {
    LOG_INFO("UpdateVideoSubscriptions called");
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_channel) {
        LOG_ERROR("UpdateVideoSubscriptions failed: Channel not initialized");
        return;
    }
    
    rte::Error err;
    
    // Get current remote streams
    size_t remote_streams_count = m_channel->GetRemoteStreamsCount(&err);
    if (err.Code() != kRteOk) {
        LOG_ERROR_FMT("Failed to get remote streams count: error={}", err.Code());
        return;
    }
    
    std::vector<rte::RemoteStream> remote_streams;
    m_channel->GetRemoteStreams(remote_streams, 0, remote_streams_count, &err);
    if (err.Code() != kRteOk) {
        LOG_ERROR_FMT("Failed to get remote streams: error={}", err.Code());
        return;
    }
    
    // Find users to unsubscribe (in current list but not in new list)
    for (const auto& userId : m_videoSubscribedUsers) {
        if (std::find(videoSubscribedUsers.begin(), videoSubscribedUsers.end(), userId) == videoSubscribedUsers.end()) {
            LOG_INFO_FMT("Unsubscribing video for user: {}", userId);
            
            // Find the stream for this user and unsubscribe video
            for (auto& remote_stream : remote_streams) {
                rte::RemoteStreamInfo remote_stream_info;
                remote_stream.GetInfo(&remote_stream_info, &err);
                if (err.Code() != kRteOk) continue;
                
                std::string stream_id = remote_stream_info.GetStreamId();
                if (stream_id == userId) {  // Assuming stream_id matches userId
                    rte::SubscribeOptions subscribeOption;
                    subscribeOption.SetTrackMediaType(rte::TrackMediaType::kRteTrackMediaTypeVideo);
                    
                    m_channel->UnsubscribeTrack(stream_id, &subscribeOption, [userId](rte::Error* err) {
                        if (err && err->Code() == kRteOk) {
                            LOG_INFO_FMT("Successfully unsubscribed video for user: {}", userId);
                        } else {
                            LOG_ERROR_FMT("Failed to unsubscribe video for user {}: error={}", userId, err ? err->Code() : -1);
                        }
                    });
                    break;
                }
            }
        }
    }
    
    // Find users to subscribe (in new list but not in current list)
    for (const auto& userId : videoSubscribedUsers) {
        if (std::find(m_videoSubscribedUsers.begin(), m_videoSubscribedUsers.end(), userId) == m_videoSubscribedUsers.end()) {
            LOG_INFO_FMT("Subscribing video for user: {}", userId);
            
            // Find the stream for this user and subscribe video
            for (auto& remote_stream : remote_streams) {
                rte::RemoteStreamInfo remote_stream_info;
                remote_stream.GetInfo(&remote_stream_info, &err);
                if (err.Code() != kRteOk) continue;
                
                std::string stream_id = remote_stream_info.GetStreamId();
                if (stream_id == userId) {  // Assuming stream_id matches userId
                    rte::SubscribeOptions subscribeOption;
                    subscribeOption.SetTrackMediaType(rte::TrackMediaType::kRteTrackMediaTypeVideo);
                    
                    m_channel->SubscribeTrack(stream_id, &subscribeOption, [userId](rte::Track* track, rte::Error* err) {
                        if (err && err->Code() == kRteOk) {
                            LOG_INFO_FMT("Successfully subscribed video for user: {}", userId);
                            
                            // Set canvas for video rendering if we have a view for this user
                            // This would need to be implemented based on the view binding logic
                        } else {
                            LOG_ERROR_FMT("Failed to subscribe video for user {}: error={}", userId, err ? err->Code() : -1);
                        }
                    });
                    break;
                }
            }
        }
    }
    
    // Update the subscription list
    m_videoSubscribedUsers = videoSubscribedUsers;
    LOG_INFO_FMT("Video subscription list updated, total subscribed users: {}", m_videoSubscribedUsers.size());
}

void RteManager::UpdateAudioSubscriptions(const std::vector<std::string>& audioSubscribedUsers) {
    LOG_INFO("UpdateAudioSubscriptions called");
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_channel) {
        LOG_ERROR("UpdateAudioSubscriptions failed: Channel not initialized");
        return;
    }
    
    rte::Error err;
    
    // Get current remote streams
    size_t remote_streams_count = m_channel->GetRemoteStreamsCount(&err);
    if (err.Code() != kRteOk) {
        LOG_ERROR_FMT("Failed to get remote streams count: error={}", err.Code());
        return;
    }
    
    std::vector<rte::RemoteStream> remote_streams;
    m_channel->GetRemoteStreams(remote_streams, 0, remote_streams_count, &err);
    if (err.Code() != kRteOk) {
        LOG_ERROR_FMT("Failed to get remote streams: error={}", err.Code());
        return;
    }
    
    // Find users to unsubscribe (in current list but not in new list)
    for (const auto& userId : m_audioSubscribedUsers) {
        if (std::find(audioSubscribedUsers.begin(), audioSubscribedUsers.end(), userId) == audioSubscribedUsers.end()) {
            LOG_INFO_FMT("Unsubscribing audio for user: {}", userId);
            
            // Find the stream for this user and unsubscribe audio
            for (auto& remote_stream : remote_streams) {
                rte::RemoteStreamInfo remote_stream_info;
                remote_stream.GetInfo(&remote_stream_info, &err);
                if (err.Code() != kRteOk) continue;
                
                std::string stream_id = remote_stream_info.GetStreamId();
                if (stream_id == userId) {  // Assuming stream_id matches userId
                    rte::SubscribeOptions subscribeOption;
                    subscribeOption.SetTrackMediaType(rte::TrackMediaType::kRteTrackMediaTypeAudio);
                    
                    m_channel->UnsubscribeTrack(stream_id, &subscribeOption, [userId](rte::Error* err) {
                        if (err && err->Code() == kRteOk) {
                            LOG_INFO_FMT("Successfully unsubscribed audio for user: {}", userId);
                        } else {
                            LOG_ERROR_FMT("Failed to unsubscribe audio for user {}: error={}", userId, err ? err->Code() : -1);
                        }
                    });
                    break;
                }
            }
        }
    }
    
    // Find users to subscribe (in new list but not in current list)
    for (const auto& userId : audioSubscribedUsers) {
        if (std::find(m_audioSubscribedUsers.begin(), m_audioSubscribedUsers.end(), userId) == m_audioSubscribedUsers.end()) {
            LOG_INFO_FMT("Subscribing audio for user: {}", userId);
            
            // Find the stream for this user and subscribe audio
            for (auto& remote_stream : remote_streams) {
                rte::RemoteStreamInfo remote_stream_info;
                remote_stream.GetInfo(&remote_stream_info, &err);
                if (err.Code() != kRteOk) continue;
                
                std::string stream_id = remote_stream_info.GetStreamId();
                if (stream_id == userId) {  // Assuming stream_id matches userId
                    rte::SubscribeOptions subscribeOption;
                    subscribeOption.SetTrackMediaType(rte::TrackMediaType::kRteTrackMediaTypeAudio);
                    
                    m_channel->SubscribeTrack(stream_id, &subscribeOption, [userId](rte::Track* track, rte::Error* err) {
                        if (err && err->Code() == kRteOk) {
                            LOG_INFO_FMT("Successfully subscribed audio for user: {}", userId);
                        } else {
                            LOG_ERROR_FMT("Failed to subscribe audio for user {}: error={}", userId, err ? err->Code() : -1);
                        }
                    });
                    break;
                }
            }
        }
    }
    
    // Update the subscription list
    m_audioSubscribedUsers = audioSubscribedUsers;
    LOG_INFO_FMT("Audio subscription list updated, total subscribed users: {}", m_audioSubscribedUsers.size());
}