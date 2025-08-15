#pragma once

#include <string>

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