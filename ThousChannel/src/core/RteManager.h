#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <algorithm>

#include "rte_cpp.h"

#include "IRteManagerEventHandler.h"

/**
 * RTE管理器配置结构体
 * 用于初始化RTE管理器时的参数传递
 */
struct RteManagerConfig {
    std::string appId;    // Agora应用的App ID
    std::string userId;   // 本地用户的唯一标识符
};

/**
 * RTE管理器类
 * 负责管理Agora RTE SDK的核心功能，包括音视频通话、用户管理、频道管理等
 * 
 * 调用时机说明：
 * 1. 应用启动时：创建实例 -> 设置事件处理器 -> 初始化 -> 加入频道
 * 2. 通话过程中：控制音视频开关、管理远程用户视频显示
 * 3. 应用关闭时：离开频道 -> 销毁实例
 */
class RteManager {
public:
    /**
     * 构造函数
     * 调用时机：应用启动时创建RTE管理器实例
     */
    RteManager();
    
    /**
     * 析构函数
     * 调用时机：应用关闭时自动调用，会触发Destroy()方法
     */
    ~RteManager();

    /**
     * 设置事件处理器
     * 调用时机：在Initialize()之前调用，用于接收RTE事件回调
     * @param handler 事件处理器指针，用于接收用户加入/离开、音视频状态变化等事件
     */
    void SetEventHandler(IRteManagerEventHandler* handler);
    
    /**
     * 初始化RTE管理器
     * 调用时机：应用启动时，在设置事件处理器之后调用
     * @param config 包含App ID和用户ID的配置信息
     * @return 初始化是否成功
     */
    bool Initialize(const RteManagerConfig& config);
    
    /**
     * 销毁RTE管理器
     * 调用时机：应用关闭时，在离开频道之后调用
     */
    void Destroy();

    /**
     * 加入频道
     * 调用时机：初始化成功后，用户开始通话时调用
     * @param channelId 频道ID
     * @param token 用户令牌（用于身份验证）
     * @return 加入频道是否成功
     */
    bool JoinChannel(const std::string& channelId, const std::string& token);
    
    /**
     * 离开频道
     * 调用时机：通话结束时，在Destroy()之前调用
     */
    void LeaveChannel();
    
    /**
     * 更新用户令牌
     * 调用时机：令牌即将过期时，用于刷新用户身份验证
     * @param token 新的用户令牌
     */
    void RenewToken(const std::string& token);

    /**
     * 设置本地音频采集开关
     * 调用时机：用户点击麦克风开关按钮时
     * @param enabled true开启音频采集，false关闭音频采集
     */
    void SetLocalAudioCaptureEnabled(bool enabled);
    
    /**
     * 设置本地视频采集开关
     * 调用时机：用户点击摄像头开关按钮时
     * @param enabled true开启视频采集，false关闭视频采集
     */
    void SetLocalVideoCaptureEnabled(bool enabled);

    /**
     * 设置视图与用户的绑定关系
     * 调用时机：UI界面初始化时，将视频显示控件与远程用户关联
     * @param viewToUserMap 视图指针到用户ID的映射关系
     */
    void SetViewUserBindings(const std::map<void*, std::string>& viewToUserMap);
    

    
    /**
     * 设置音频订阅的用户列表
     * 调用时机：需要控制接收哪些用户的音频流时
     * @param userIds 要订阅音频的用户ID列表
     */
    void SetAudioSubscribedUsers(const std::vector<std::string>& userIds);
    
    /**
     * 设置视频订阅的用户列表
     * 调用时机：需要控制接收哪些用户的视频流时
     * @param userIds 要订阅视频的用户ID列表
     */
    void SetVideoSubscribedUsers(const std::vector<std::string>& userIds);

private:
    friend class RteManagerEventObserver;

    /**
     * 远程用户加入事件处理
     * 调用时机：由RteManagerEventObserver在检测到远程用户加入时自动调用
     * @param userId 加入的远程用户ID
     */
    void OnRemoteUserJoined(const std::string& userId);
    
    /**
     * 远程用户离开事件处理
     * 调用时机：由RteManagerEventObserver在检测到远程用户离开时自动调用
     * @param userId 离开的远程用户ID
     */
    void OnRemoteUserLeft(const std::string& userId);
    
    /**
     * 设置远程用户视频显示
     * 调用时机：远程用户加入频道时，为其分配视频显示控件
     * @param userId 远程用户ID
     * @param view 视频显示控件指针，nullptr表示移除该用户的视频显示
     * @return 0表示成功，-1表示失败
     */
    int SetupRemoteVideo(const std::string& userId, void* view);

private:
    // RTE SDK核心组件
    std::shared_ptr<rte::Rte> m_rte;                           // RTE引擎实例
    std::shared_ptr<rte::LocalUser> m_localUser;               // 本地用户对象
    std::shared_ptr<rte::Channel> m_channel;                   // 频道对象
    std::shared_ptr<rte::LocalRealTimeStream> m_localStream;   // 本地实时流
    std::shared_ptr<rte::MicAudioTrack> m_micAudioTrack;       // 麦克风音频轨道
    std::shared_ptr<rte::CameraVideoTrack> m_cameraVideoTrack; // 摄像头视频轨道
    std::shared_ptr<rte::ChannelObserver> m_channelObserver;   // 频道事件观察者

    // 事件处理
    IRteManagerEventHandler* m_eventHandler;                   // 外部事件处理器

    // 配置信息
    std::string m_appId;                                       // Agora应用ID
    std::string m_userId;                                      // 本地用户ID
    std::string m_channelId;                                   // 当前频道ID

    // 线程安全成员
    std::mutex m_mutex;                                        // 互斥锁，保护共享数据
    std::map<void*, std::string> m_viewToUserMap;              // 视图控件到用户ID的映射关系
    
    // 订阅状态管理
    std::vector<std::string> m_audioSubscribedUserIds;         // 当前已订阅音频的用户ID列表
    std::vector<std::string> m_videoSubscribedUserIds;         // 当前已订阅视频的用户ID列表
};
