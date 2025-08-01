#include "pch.h"
#include "RteManager.h"
#include "../ui/dialogs/ChannelPageDlg.h"
#include "../../sdk/high_level_api/include/rte_base/rte_cpp_rte.h"
#include "../../sdk/high_level_api/include/rte_base/rte_cpp_user.h"
#include "../../sdk/high_level_api/include/rte_base/rte_cpp_channel.h"
#include "../../sdk/high_level_api/include/rte_base/track/rte_cpp_mic_audio_track.h"
#include "../../sdk/high_level_api/include/rte_base/track/rte_cpp_camera_video_track.h"
#include "../../sdk/high_level_api/include/rte_base/stream/rte_cpp_local_realtime_stream.h"
#include "../../sdk/high_level_api/include/rte_base/stream/rte_cpp_remote_stream.h"

#include <map>
#include <memory>
#include <functional>
#include <vector>
#include <string>
#include <set>

class RteManager::Impl {
public:
    Impl() : eventHandler_(nullptr), inChannel_(false) {}
    ~Impl() { Destroy(); }

    void SetEventHandler(IRteManagerEventHandler* handler) {
        eventHandler_ = handler;
    }

    bool Initialize(const RteManagerConfig& config) {
        try {
            config_ = config;
            // 1. Create RTE object
            rte_ = std::make_unique<rte::Rte>();
            // 2. Set AppId and other configs
            auto rteConfig = std::make_unique<rte::Config>();
            rteConfig->SetAppId(config.appId.c_str());
            rte_->SetConfigs(rteConfig.get());
            // 3. Initialize media engine
            rte_->InitMediaEngine([](rte::Error* err) {
                // Handle media engine init result
            });
            // 4. Create local user and set userId/token
            localUser_ = std::make_unique<rte::LocalUser>(rte_.get());
            auto userConfig = std::make_unique<rte::LocalUserConfig>();
            userConfig->SetUserId(config.userId.c_str());
            userConfig->SetUserToken(config.userToken.c_str());
            localUser_->SetConfigs(userConfig.get());
            localUser_->Connect([](rte::Error* err) {
                // Handle login result
            });
            // 5. Register event handlers if needed
            // RegisterEventHandlers();
            return true;
        } catch (...) {
            return false;
        }
    }

    // 用户ID列表，内部维护
    std::vector<std::string> realUserIds_;
    std::vector<std::string> robotUserIds_;

    // 新增：频道事件观察者
    class RteManagerChannelObserver : public rte::ChannelObserver {
    public:
        explicit RteManagerChannelObserver(Impl* owner) : owner_(owner) {}
        void OnRemoteUsersJoined(const std::vector<rte::RemoteUser>& new_users, const std::vector<rte::RemoteUserInfo>&) override {
            for (const auto& user : new_users) {
                const std::string& userId = user.GetUserId();
                bool isRobot = false;
                try {
                    isRobot = std::stoi(userId) > 1000;
                } catch (...) {}
                if (isRobot) {
                    if (std::find(owner_->robotUserIds_.begin(), owner_->robotUserIds_.end(), userId) == owner_->robotUserIds_.end())
                        owner_->robotUserIds_.push_back(userId);
                } else {
                    if (std::find(owner_->realUserIds_.begin(), owner_->realUserIds_.end(), userId) == owner_->realUserIds_.end())
                        owner_->realUserIds_.push_back(userId);
                }
            }
            if (owner_->eventHandler_) owner_->eventHandler_->OnUserListChanged();
        }
        void OnRemoteUsersLeft(const std::vector<rte::RemoteUser>& removed_users, const std::vector<rte::RemoteUserInfo>&) override {
            for (const auto& user : removed_users) {
                const std::string& userId = user.GetUserId();
                auto rit = std::find(owner_->robotUserIds_.begin(), owner_->robotUserIds_.end(), userId);
                if (rit != owner_->robotUserIds_.end()) {
                    owner_->robotUserIds_.erase(rit);
                    continue;
                }
                auto uit = std::find(owner_->realUserIds_.begin(), owner_->realUserIds_.end(), userId);
                if (uit != owner_->realUserIds_.end()) {
                    owner_->realUserIds_.erase(uit);
                }
            }
            if (owner_->eventHandler_) owner_->eventHandler_->OnUserListChanged();
        }
    private:
        Impl* owner_;
    };

    bool JoinChannel(const std::string& channelId, const std::string& token) {
        if (inChannel_) return false;
        try {
            // 1. Create channel object
            channel_ = std::make_unique<rte::Channel>(rte_.get());
            // 2. Set channel ID and configs
            auto channelConfig = std::make_unique<rte::ChannelConfig>();
            channelConfig->SetChannelId(channelId.c_str());
            channel_->SetConfigs(channelConfig.get());
            // 3. Register channel event handlers
            if (!channelObserver_) {
                channelObserver_ = std::make_unique<RteManagerChannelObserver>(this);
            }
            channel_->RegisterObserver(channelObserver_.get(), nullptr);
            // 4. Join channel
            bool ret = channel_->Join(localUser_.get(), token.c_str(), [this, channelId](rte::Error* err) {
                if (err && err->Code() == 0) { // kRteOk == 0
                    inChannel_ = true;
                    currentChannelId_ = channelId;
                    // 5. Create and publish local stream (publish once, keep publishing)
                    if (!localStream_) {
                        localStream_ = std::make_unique<rte::LocalRealTimeStream>(rte_.get());
                        // Add audio/video tracks if they exist
                        if (audioTrack_) localStream_->AddAudioTrack(audioTrack_.get());
                        if (videoTrack_) localStream_->AddVideoTrack(videoTrack_.get());
                    }
                    channel_->PublishStream(nullptr, localStream_.get(), [](rte::Error* err) {
                        // Handle publish result
                    });
                    NotifyConnectionStateChanged(1); // Connected
                } else {
                    NotifyError(1); // NetworkError
                }
            });
            return ret;
        } catch (...) {
            return false;
        }
    }

    bool SetLocalAudioCaptureEnabled(bool enable) {
        if (!inChannel_) return false;
        try {
            // If audio track does not exist, create and add to local stream
            if (!audioTrack_) {
                audioTrack_ = std::make_unique<rte::MicAudioTrack>(rte_.get());
                auto audioConfig = std::make_unique<rte::MicAudioTrackConfig>();
                audioTrack_->SetConfigs(audioConfig.get());
                if (!localStream_) {
                    localStream_ = std::make_unique<rte::LocalRealTimeStream>(rte_.get());
                }
                localStream_->AddAudioTrack(audioTrack_.get());
            }
            if (enable) {
                // Start audio capture (stream publishing is always on)
                audioTrack_->Start([](rte::Error* err) {
                    // Handle start result
                });
                NotifyLocalAudioStateChanged(2); // Started
            } else {
                // Stop audio capture (stream publishing is always on)
                audioTrack_->Stop([](rte::Error* err) {
                    // Handle stop result
                });
                NotifyLocalAudioStateChanged(0); // Stopped
            }
            return true;
        } catch (...) {
            return false;
        }
    }

    bool SetLocalVideoCaptureEnabled(bool enable) {
        if (!inChannel_) return false;
        try {
            // If video track does not exist, create and add to local stream
            if (!videoTrack_) {
                videoTrack_ = std::make_unique<rte::CameraVideoTrack>(rte_.get());
                auto videoConfig = std::make_unique<rte::CameraVideoTrackConfig>();
                videoTrack_->SetConfigs(videoConfig.get());
                if (!localStream_) {
                    localStream_ = std::make_unique<rte::LocalRealTimeStream>(rte_.get());
                }
                localStream_->AddVideoTrack(videoTrack_.get());
            }
            if (enable) {
                // Start video capture (stream publishing is always on)
                videoTrack_->Start([](rte::Error* err) {
                    // Handle start result
                });
                NotifyLocalVideoStateChanged(2); // Started
            } else {
                // Stop video capture (stream publishing is always on)
                videoTrack_->Stop([](rte::Error* err) {
                    // Handle stop result
                });
                NotifyLocalVideoStateChanged(0); // Stopped
            }
            return true;
        } catch (...) {
            return false;
        }
    }

    bool SubscribeRemoteAudio(const std::string& userId) {
        if (!inChannel_) return false;
        try {
            // Find remote streams
            size_t remoteStreamsCount = channel_->GetRemoteStreamsCount();
            std::vector<rte::RemoteStream> remoteStreams;
            channel_->GetRemoteStreams(nullptr, remoteStreams, 0, remoteStreamsCount);
            // Subscribe to remote audio track
            for (auto& remoteStream : remoteStreams) {
                rte::RemoteStreamInfo streamInfo;
                remoteStream.GetInfo(&streamInfo);
                if (streamInfo.GetStreamId() == userId && streamInfo.HasAudio()) {
                    rte::SubscribeOptions options;
                    options.SetTrackMediaType(static_cast<rte::TrackMediaType>(0)); // kRteTrackMediaTypeAudio
                    channel_->SubscribeTrack(userId.c_str(), &options, [this, userId](rte::Track* track, rte::Error* err) {
                        if (err && err->Code() == 0) {
                            NotifyRemoteAudioStateChanged(userId, 2); // Started
                        }
                    });
                    break;
                }
            }
            // remoteUsers_[userId] = 1; // Mark as audio subscribed // Removed
            return true;
        } catch (...) {
            return false;
        }
    }

    bool UnsubscribeRemoteAudio(const std::string& userId) {
        if (!inChannel_) return false;
        try {
            // Unsubscribe from remote audio track
            rte::SubscribeOptions options;
            options.SetTrackMediaType(static_cast<rte::TrackMediaType>(0)); // kRteTrackMediaTypeAudio
            channel_->UnsubscribeTrack(userId.c_str(), &options, [this, userId](rte::Error* err) {
                if (err && err->Code() == 0) {
                    NotifyRemoteAudioStateChanged(userId, 0); // Stopped
                }
            });
            // remoteUsers_.erase(userId); // Removed
            return true;
        } catch (...) {
            return false;
        }
    }

    bool SubscribeRemoteVideo(const std::string& userId) {
        if (!inChannel_) return false;
        try {
            // Find remote streams
            size_t remoteStreamsCount = channel_->GetRemoteStreamsCount();
            std::vector<rte::RemoteStream> remoteStreams;
            channel_->GetRemoteStreams(nullptr, remoteStreams, 0, remoteStreamsCount);
            // Subscribe to remote video track
            for (auto& remoteStream : remoteStreams) {
                rte::RemoteStreamInfo streamInfo;
                remoteStream.GetInfo(&streamInfo);
                if (streamInfo.GetStreamId() == userId && streamInfo.HasVideo()) {
                    rte::SubscribeOptions options;
                    options.SetTrackMediaType(static_cast<rte::TrackMediaType>(1)); // kRteTrackMediaTypeVideo
                    channel_->SubscribeTrack(userId.c_str(), &options, [this, userId](rte::Track* track, rte::Error* err) {
                        if (err && err->Code() == 0) {
                            NotifyRemoteVideoStateChanged(userId, 2); // Started
                        }
                    });
                    break;
                }
            }
            // remoteUsers_[userId] = 1; // Mark as video subscribed // Removed
            return true;
        } catch (...) {
            return false;
        }
    }

    bool UnsubscribeRemoteVideo(const std::string& userId) {
        if (!inChannel_) return false;
        try {
            // Unsubscribe from remote video track
            rte::SubscribeOptions options;
            options.SetTrackMediaType(static_cast<rte::TrackMediaType>(1)); // kRteTrackMediaTypeVideo
            channel_->UnsubscribeTrack(userId.c_str(), &options, [this, userId](rte::Error* err) {
                if (err && err->Code() == 0) {
                    NotifyRemoteVideoStateChanged(userId, 0); // Stopped
                }
            });
            // remoteUsers_.erase(userId); // Removed
            return true;
        } catch (...) {
            return false;
        }
    }

    void LeaveChannel() {
        if (!inChannel_) return;
        try {
            // 1. Unpublish local stream
            if (localStream_) {
                channel_->UnpublishStream(localStream_.get(), [](rte::Error* err) {
                    // Handle unpublish result
                });
            }
            // 2. Stop and destroy audio/video tracks
            if (audioTrack_) {
                audioTrack_->Stop([](rte::Error* err) {});
                audioTrack_.reset();
            }
            if (videoTrack_) {
                videoTrack_->Stop([](rte::Error* err) {});
                videoTrack_.reset();
            }
            // 3. Leave channel
            bool ret = channel_->Leave([this](rte::Error* err) {
                // Handle leave result
            });
            if (ret) {
                inChannel_ = false;
                currentChannelId_.clear();
                // remoteUsers_.clear(); // Removed
                NotifyConnectionStateChanged(0); // Disconnected
            }
        } catch (...) {
            // Handle exception
        }
    }

    void Destroy() {
        try {
            // 1. Leave channel if in channel
            if (inChannel_) {
                LeaveChannel();
            }
            // 2. Destroy all RTE objects
            localStream_.reset();
            channel_.reset();
            localUser_.reset();
            rte_.reset();
            // 3. Release resources
            inChannel_ = false;
            currentChannelId_.clear();
            // remoteUsers_.clear(); // Removed
            NotifyConnectionStateChanged(0); // Disconnected
        } catch (...) {
            // Handle exception
        }
    }

    bool IsInChannel() const {
        return inChannel_;
    }
    std::string GetCurrentChannelId() const {
        return currentChannelId_;
    }
    std::vector<std::string> GetRemoteUsers() const {
        std::vector<std::string> users;
        users.insert(users.end(), realUserIds_.begin(), realUserIds_.end());
        users.insert(users.end(), robotUserIds_.begin(), robotUserIds_.end());
        return users;
    }
    std::string GetLocalUserId() const {
        return config_.userId;
    }
    void SetSubscribedUsers(const std::vector<std::string>& userIds) {
        std::set<std::string> newSet(userIds.begin(), userIds.end());
        // Subscribe to new users in the window
        for (const auto& userId : newSet) {
            if (subscribedUsers_.count(userId) == 0) {
                SubscribeRemoteAudio(userId);
                SubscribeRemoteVideo(userId);
            }
        }
        // Unsubscribe users who left the window
        for (const auto& userId : subscribedUsers_) {
            if (newSet.count(userId) == 0) {
                UnsubscribeRemoteAudio(userId);
                UnsubscribeRemoteVideo(userId);
            }
        }
        subscribedUsers_ = std::move(newSet);
    }

    void SetViewUserBindings(const std::map<void*, std::string>& viewToUserMap) {
        // Build reverse mapping: userId -> view
        std::map<std::string, void*> newUserToViewMap;
        for (const auto& kv : viewToUserMap) {
            newUserToViewMap[kv.second] = kv.first;
        }
        // 1. Unbind views that are removed or changed user
        for (const auto& kv : viewToUserMap_) {
            void* view = kv.first;
            const std::string& oldUser = kv.second;
            if (newUserToViewMap.count(view) == 0 || newUserToViewMap.at(view) != oldUser) {
                auto* videoTrack = GetUserVideoTrack(oldUser);
                if (videoTrack) {
                    videoTrack->SetCanvas(nullptr, nullptr);
                }
                userToViewMap_.erase(oldUser);
            }
        }
        // 2. Unbind users whose view changed
        for (const auto& kv : userToViewMap_) {
            const std::string& userId = kv.first;
            void* oldView = kv.second;
            if (newUserToViewMap.count(userId) == 0 || newUserToViewMap.at(userId) != oldView) {
                auto* videoTrack = GetUserVideoTrack(userId);
                if (videoTrack) {
                    videoTrack->SetCanvas(nullptr, nullptr);
                }
            }
        }
        // 3. Bind new or changed view-user pairs
        for (const auto& kv : newUserToViewMap) {
            void* view = kv.first;
            const std::string& userId = kv.second;
            if (viewToUserMap_.count(view) == 0 || viewToUserMap_[view] != userId) {
                auto* videoTrack = GetUserVideoTrack(userId);
                if (videoTrack) {
                    videoTrack->SetCanvas(GetCanvasForView(view), nullptr);
                }
            }
        }
        // 4. Update internal mappings
        viewToUserMap_ = viewToUserMap;
        userToViewMap_.clear();
        for (const auto& kv : newUserToViewMap) {
            userToViewMap_[kv.second] = kv.first;
        }
    }

private:
    // Helper functions
    rte::CameraVideoTrack* GetUserVideoTrack(const std::string& userId) {
        // This is a placeholder implementation
        // In a real implementation, you would need to track video tracks per user
        return nullptr;
    }
    
    void* GetCanvasForView(void* view) {
        // This is a placeholder implementation
        // In a real implementation, you would convert the view to a canvas
        return view;
    }
private:
    // Event notification helpers
    void NotifyConnectionStateChanged(int state) {
        if (eventHandler_) eventHandler_->OnConnectionStateChanged(state);
    }
    void NotifyUserJoined(const std::string& userId) {
        if (eventHandler_) eventHandler_->OnUserJoined(userId);
    }
    void NotifyUserLeft(const std::string& userId) {
        if (eventHandler_) eventHandler_->OnUserLeft(userId);
    }
    void NotifyLocalAudioStateChanged(int state) {
        if (eventHandler_) eventHandler_->OnLocalAudioStateChanged(state);
    }
    void NotifyLocalVideoStateChanged(int state) {
        if (eventHandler_) eventHandler_->OnLocalVideoStateChanged(state);
    }
    void NotifyRemoteAudioStateChanged(const std::string& userId, int state) {
        if (eventHandler_) eventHandler_->OnRemoteAudioStateChanged(userId, state);
    }
    void NotifyRemoteVideoStateChanged(const std::string& userId, int state) {
        if (eventHandler_) eventHandler_->OnRemoteVideoStateChanged(userId, state);
    }
    void NotifyError(int error) {
        if (eventHandler_) eventHandler_->OnError(error);
    }
    IRteManagerEventHandler* eventHandler_;
    RteManagerConfig config_;
    bool inChannel_;
    std::string currentChannelId_;
    // std::map<std::string, int> remoteUsers_; // Removed
    std::unique_ptr<rte::Rte> rte_;
    std::unique_ptr<rte::LocalUser> localUser_;
    std::unique_ptr<rte::Channel> channel_;
    std::unique_ptr<rte::MicAudioTrack> audioTrack_;
    std::unique_ptr<rte::CameraVideoTrack> videoTrack_;
    std::unique_ptr<rte::LocalRealTimeStream> localStream_;
    std::set<std::string> subscribedUsers_; // Track currently subscribed users
    std::map<void*, std::string> viewToUserMap_; // view -> userId
    std::map<std::string, void*> userToViewMap_; // userId -> view
    std::unique_ptr<RteManagerChannelObserver> channelObserver_;
};

RteManager::RteManager() : impl_(new Impl) {}
RteManager::~RteManager() = default;

void RteManager::SetEventHandler(IRteManagerEventHandler* handler) { impl_->SetEventHandler(handler); }
bool RteManager::Initialize(const RteManagerConfig& config) { return impl_->Initialize(config); }
bool RteManager::JoinChannel(const std::string& channelId, const std::string& token) { return impl_->JoinChannel(channelId, token); }
bool RteManager::SetLocalAudioCaptureEnabled(bool enable) { return impl_->SetLocalAudioCaptureEnabled(enable); }
bool RteManager::SetLocalVideoCaptureEnabled(bool enable) { return impl_->SetLocalVideoCaptureEnabled(enable); }
bool RteManager::SubscribeRemoteAudio(const std::string& userId) { return impl_->SubscribeRemoteAudio(userId); }
bool RteManager::UnsubscribeRemoteAudio(const std::string& userId) { return impl_->UnsubscribeRemoteAudio(userId); }
bool RteManager::SubscribeRemoteVideo(const std::string& userId) { return impl_->SubscribeRemoteVideo(userId); }
bool RteManager::UnsubscribeRemoteVideo(const std::string& userId) { return impl_->UnsubscribeRemoteVideo(userId); }
void RteManager::LeaveChannel() { impl_->LeaveChannel(); }
void RteManager::Destroy() { impl_->Destroy(); }
bool RteManager::IsInChannel() const { return impl_->IsInChannel(); }
std::string RteManager::GetCurrentChannelId() const { return impl_->GetCurrentChannelId(); }
std::vector<std::string> RteManager::GetRemoteUsers() const { return impl_->GetRemoteUsers(); }
std::string RteManager::GetLocalUserId() const { return impl_->GetLocalUserId(); }
void RteManager::SetSubscribedUsers(const std::vector<std::string>& userIds) {
    impl_->SetSubscribedUsers(userIds);
}

void RteManager::SetViewUserBindings(const std::map<void*, std::string>& viewToUserMap) {
    impl_->SetViewUserBindings(viewToUserMap);
}

// User management: maintain ordered lists of real and robot user IDs
const std::vector<std::string>& RteManager::GetRealUserIds() const {
    return impl_->realUserIds_;
}

const std::vector<std::string>& RteManager::GetRobotUserIds() const {
    return impl_->robotUserIds_;
} 