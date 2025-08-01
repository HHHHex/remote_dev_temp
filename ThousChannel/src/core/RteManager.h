#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>

// 事件回调接口声明
class IRteManagerEventHandler {
public:
    virtual void OnConnectionStateChanged(int state) {}
    virtual void OnUserJoined(const std::string& userId) {}
    virtual void OnUserLeft(const std::string& userId) {}
    virtual void OnLocalAudioStateChanged(int state) {}
    virtual void OnLocalVideoStateChanged(int state) {}
    virtual void OnRemoteAudioStateChanged(const std::string& userId, int state) {}
    virtual void OnRemoteVideoStateChanged(const std::string& userId, int state) {}
    virtual void OnError(int error) {}
    // Called when the user list changes (user join/leave)
    virtual void OnUserListChanged() {}
    virtual ~IRteManagerEventHandler() = default;
};

struct RteManagerConfig {
    std::string appId;
    std::string userId;
    std::string userToken;
};

class RteManager {
public:
    RteManager();
    ~RteManager();

    // 设置事件回调
    void SetEventHandler(IRteManagerEventHandler* handler);

    // 1. 初始化
    bool Initialize(const RteManagerConfig& config);
    // 2. 加入频道
    bool JoinChannel(const std::string& channelId, const std::string& token);
    // 3. 控制本地音视频采集开关（发流始终保持，采集内容有/无由此接口控制）
    bool SetLocalAudioCaptureEnabled(bool enable);
    bool SetLocalVideoCaptureEnabled(bool enable);
    // 4. 订阅/取消订阅远端音视频
    bool SubscribeRemoteAudio(const std::string& userId);
    bool UnsubscribeRemoteAudio(const std::string& userId);
    bool SubscribeRemoteVideo(const std::string& userId);
    bool UnsubscribeRemoteVideo(const std::string& userId);
    // 5. 退出频道及销毁
    void LeaveChannel();
    void Destroy();

    // 辅助接口
    bool IsInChannel() const;
    std::string GetCurrentChannelId() const;
    std::vector<std::string> GetRemoteUsers() const;
    std::string GetLocalUserId() const;
    // 动态管理当前窗口内需要订阅的用户（滑动窗口/分页/虚拟化场景）
    void SetSubscribedUsers(const std::vector<std::string>& userIds);
    // Efficiently manage the mapping between views and user streams (add, switch user, switch view, remove, no change)
    void SetViewUserBindings(const std::map<void*, std::string>& viewToUserMap);

    // User management APIs
    const std::vector<std::string>& GetRealUserIds() const;
    const std::vector<std::string>& GetRobotUserIds() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
}; 