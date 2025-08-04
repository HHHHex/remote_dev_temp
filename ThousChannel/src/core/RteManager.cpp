#include "RteManager.h"

// Mock implementation of RteManager

RteManager::RteManager() : m_eventHandler(nullptr) {}

RteManager::~RteManager() {}

void RteManager::SetEventHandler(IRteManagerEventHandler* handler) {
    m_eventHandler = handler;
}

bool RteManager::Initialize(const RteManagerConfig& config) {
    // Mock success
    return true;
}

void RteManager::Destroy() {
    // Mock cleanup
}

bool RteManager::JoinChannel(const std::string& channelId, const std::string& token) {
    // Mock success
    if (m_eventHandler) {
        m_eventHandler->OnConnectionStateChanged(0); // Simulate success
    }
    return true;
}

void RteManager::LeaveChannel() {
    // Mock leaving channel
}

void RteManager::SetLocalAudioCaptureEnabled(bool enabled) {
    // Mock action
}

void RteManager::SetLocalVideoCaptureEnabled(bool enabled) {
    // Mock action
}

void RteManager::SubscribeRemoteVideo(const char* userId) {
    // Mock action
}

void RteManager::UnsubscribeRemoteVideo(const char* userId) {
    // Mock action
}

void RteManager::SubscribeRemoteAudio(const std::string& userId) {
    // Mock action
}

void RteManager::UnsubscribeRemoteAudio(const std::string& userId) {
    // Mock action
}

void RteManager::SetSubscribedUsers(const std::vector<std::string>& userIds) {
    // Mock action
}

void RteManager::SetViewUserBindings(const std::map<void*, std::string>& viewToUserMap) {
    // Mock action
}