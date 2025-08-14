#include "RteManager.h"
#include "IAgoraRtcEngine.h"
#include "AgoraMediaBase.h"
#include <iterator>
#include <algorithm>

class RteManagerEventHandler : public agora::rtc::IRtcEngineEventHandler {
private:
    RteManager* m_rteManager;

public:
    RteManagerEventHandler(RteManager* rteManager) : m_rteManager(rteManager) {}

    void onConnectionStateChanged(agora::rtc::CONNECTION_STATE_TYPE state, agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) override {
        if (m_rteManager->m_eventHandler) {
            m_rteManager->m_eventHandler->OnConnectionStateChanged((int)state);
        }
    }

    void onUserJoined(agora::rtc::uid_t uid, int elapsed) override {
        agora::rtc::UserInfo userInfo;
        if (m_rteManager->m_rtcEngine->getUserInfoByUid(uid, &userInfo) == 0) {
            std::string userId(userInfo.userAccount);
            m_rteManager->m_uid_to_user_id[uid] = userId;
            m_rteManager->m_user_id_to_uid[userId] = uid;

            if (m_rteManager->m_eventHandler) {
                m_rteManager->m_eventHandler->OnUserJoined(userId);
                m_rteManager->m_eventHandler->OnUserListChanged();
            }
        }
    }

    void onUserOffline(agora::rtc::uid_t uid, agora::rtc::USER_OFFLINE_REASON_TYPE reason) override {
        if (m_rteManager->m_uid_to_user_id.count(uid)) {
            std::string userId = m_rteManager->m_uid_to_user_id[uid];
            m_rteManager->m_uid_to_user_id.erase(uid);
            m_rteManager->m_user_id_to_uid.erase(userId);

            if (m_rteManager->m_eventHandler) {
                m_rteManager->m_eventHandler->OnUserLeft(userId);
                m_rteManager->m_eventHandler->OnUserListChanged();
            }
        }
    }

    void onLocalAudioStateChanged(agora::rtc::LOCAL_AUDIO_STREAM_STATE state, agora::rtc::LOCAL_AUDIO_STREAM_REASON reason) override {
        if (m_rteManager->m_eventHandler) {
            m_rteManager->m_eventHandler->OnLocalAudioStateChanged((int)state);
        }
    }

    void RteManagerEventHandler::onLocalVideoStateChanged(agora::rtc::VIDEO_SOURCE_TYPE source, agora::rtc::LOCAL_VIDEO_STREAM_STATE state, agora::rtc::LOCAL_VIDEO_STREAM_REASON reason)
{
    if (m_manager->m_eventHandler) {
        m_manager->m_eventHandler->OnLocalVideoStateChanged(state, reason);
    }
}

    void onRemoteAudioStateChanged(agora::rtc::uid_t uid, agora::rtc::REMOTE_AUDIO_STATE state, agora::rtc::REMOTE_AUDIO_STATE_REASON reason, int elapsed) override {
        if (m_rteManager->m_eventHandler && m_rteManager->m_uid_to_user_id.count(uid)) {
            std::string userId = m_rteManager->m_uid_to_user_id[uid];
            m_rteManager->m_eventHandler->OnRemoteAudioStateChanged(userId, (int)state);
        }
    }

    void onRemoteVideoStateChanged(agora::rtc::uid_t uid, agora::rtc::REMOTE_VIDEO_STATE state, agora::rtc::REMOTE_VIDEO_STATE_REASON reason, int elapsed) override {
        if (m_rteManager->m_eventHandler && m_rteManager->m_uid_to_user_id.count(uid)) {
            std::string userId = m_rteManager->m_uid_to_user_id[uid];
            m_rteManager->m_eventHandler->OnRemoteVideoStateChanged(userId, (int)state);
        }
    }

    void onError(int err, const char* msg) override {
        if (m_rteManager->m_eventHandler) {
            m_rteManager->m_eventHandler->OnError(err);
        }
    }
};

RteManager::RteManager() : m_rtcEngine(nullptr), m_eventHandler(nullptr) {
    m_sdkEventHandler = std::make_shared<RteManagerEventHandler>(this);
}

RteManager::~RteManager() {
    Destroy();
}

void RteManager::SetEventHandler(IRteManagerEventHandler* handler) {
    m_eventHandler = handler;
}

bool RteManager::Initialize(const RteManagerConfig& config) {
    m_appId = config.appId;
    m_userId = config.userId;

    agora::rtc::RtcEngineContext context;
    context.appId = m_appId.c_str();
    context.eventHandler = m_sdkEventHandler.get();
    
    m_rtcEngine = static_cast<agora::rtc::IRtcEngine*>(createAgoraRtcEngine());
    if (!m_rtcEngine) {
        return false;
    }

    if (m_rtcEngine->initialize(context) != 0) {
        return false;
    }

    m_rtcEngine->enableVideo();
    m_rtcEngine->enableAudio();
    m_rtcEngine->setChannelProfile(agora::CHANNEL_PROFILE_TYPE::CHANNEL_PROFILE_LIVE_BROADCASTING);
    m_rtcEngine->setClientRole(agora::rtc::CLIENT_ROLE_BROADCASTER);

    return true;
}

void RteManager::Destroy() {
    if (m_rtcEngine) {
        m_rtcEngine->release();
        m_rtcEngine = nullptr;
    }
}

bool RteManager::JoinChannel(const std::string& channelId, const std::string& token) {
    m_channelId = channelId;
    if (m_rtcEngine) {
        return m_rtcEngine->joinChannelWithUserAccount(token.c_str(), channelId.c_str(), m_userId.c_str()) == 0;
    }
    return false;
}

void RteManager::LeaveChannel() {
    if (m_rtcEngine) {
        m_rtcEngine->leaveChannel();
    }
}

void RteManager::SetLocalAudioCaptureEnabled(bool enabled) {
    if (m_rtcEngine) {
        m_rtcEngine->enableLocalAudio(enabled);
    }
}

void RteManager::SetLocalVideoCaptureEnabled(bool enabled) {
    if (m_rtcEngine) {
        m_rtcEngine->enableLocalVideo(enabled);
    }
}

void RteManager::SubscribeRemoteVideo(const std::string& userId) {
    if (m_rtcEngine && m_user_id_to_uid.count(userId)) {
        agora::rtc::uid_t uid = m_user_id_to_uid[userId];
        m_rtcEngine->muteRemoteVideoStream(uid, false);
    }
}

void RteManager::UnsubscribeRemoteVideo(const std::string& userId) {
    if (m_rtcEngine && m_user_id_to_uid.count(userId)) {
        agora::rtc::uid_t uid = m_user_id_to_uid[userId];
        m_rtcEngine->muteRemoteVideoStream(uid, true);
    }
}

void RteManager::SubscribeRemoteAudio(const std::string& userId) {
    if (m_rtcEngine && m_user_id_to_uid.count(userId)) {
        agora::rtc::uid_t uid = m_user_id_to_uid[userId];
        m_rtcEngine->muteRemoteAudioStream(uid, false);
    }
}

void RteManager::UnsubscribeRemoteAudio(const std::string& userId) {
    if (m_rtcEngine && m_user_id_to_uid.count(userId)) {
        agora::rtc::uid_t uid = m_user_id_to_uid[userId];
        m_rtcEngine->muteRemoteAudioStream(uid, true);
    }
}

void RteManager::SetSubscribedUsers(const std::vector<std::string>& userIds) {
    std::vector<std::string> to_subscribe;
    std::vector<std::string> to_unsubscribe;

    std::vector<std::string> sorted_subscribedUsers = m_subscribedUsers;
    std::sort(sorted_subscribedUsers.begin(), sorted_subscribedUsers.end());
    std::vector<std::string> sorted_userIds = userIds;
    std::sort(sorted_userIds.begin(), sorted_userIds.end());

    std::set_difference(sorted_userIds.begin(), sorted_userIds.end(),
                        sorted_subscribedUsers.begin(), sorted_subscribedUsers.end(),
                        std::back_inserter(to_subscribe));

    std::set_difference(sorted_subscribedUsers.begin(), sorted_subscribedUsers.end(),
                        sorted_userIds.begin(), sorted_userIds.end(),
                        std::back_inserter(to_unsubscribe));

    for (std::vector<std::string>::const_iterator it = to_subscribe.begin(); it != to_subscribe.end(); ++it) {
        SubscribeRemoteVideo(*it);
        SubscribeRemoteAudio(*it);
    }

    for (std::vector<std::string>::const_iterator it = to_unsubscribe.begin(); it != to_unsubscribe.end(); ++it) {
        UnsubscribeRemoteVideo(*it);
        UnsubscribeRemoteAudio(*it);
    }

    m_subscribedUsers = userIds;
}

void RteManager::SetViewUserBindings(const std::map<void*, std::string>& viewToUserMap) {
    // Unbind old views
    for (std::map<void*, std::string>::const_iterator it = m_viewToUserMap.begin(); it != m_viewToUserMap.end(); ++it) {
        void* view = it->first;
        const std::string& userId = it->second;
        if (viewToUserMap.find(view) == viewToUserMap.end() || viewToUserMap.at(view) != userId) {
             if (m_user_id_to_uid.count(userId)) {
                agora::rtc::uid_t uid = m_user_id_to_uid[userId];
                agora::rtc::VideoCanvas canvas;
                canvas.uid = uid;
                canvas.view = nullptr;
                m_rtcEngine->setupRemoteVideo(canvas);
            }
        }
    }

    // Bind new views
    for (std::map<void*, std::string>::const_iterator it = viewToUserMap.begin(); it != viewToUserMap.end(); ++it) {
        void* view = it->first;
        const std::string& userId = it->second;
        if (m_user_id_to_uid.count(userId)) {
            agora::rtc::uid_t uid = m_user_id_to_uid[userId];
            agora::rtc::VideoCanvas canvas;
            canvas.uid = uid;
            canvas.view = (agora::media::base::view_t)view;
            m_rtcEngine->setupRemoteVideo(canvas);
        }
    }
    m_viewToUserMap = viewToUserMap;
}