#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>

#include "rte_cpp.h"

#include "IRteManagerEventHandler.h"

/**
 * RteManager初始化配置结构体
 * 包含连接Agora RTE服务所需的基本参数
 */
struct RteManagerConfig {
    std::string appId;    ///< Agora应用ID，用于身份验证
    std::string userId;   ///< 本地用户ID，用于身份识别
};

/**
 * RteManager - Agora RTE（实时互动）功能的核心管理类
 * 
 * 该类负责处理：
 * - RTE SDK的初始化和生命周期管理
 * - 本地用户连接和频道管理
 * - 本地音视频轨道控制
 * - 远程用户事件处理
 * - 视图到用户的绑定管理
 * 
 * 线程安全：大多数方法不是线程安全的，应在UI线程中调用
 *           内部状态在必要时通过互斥锁保护
 */
class RteManager {
public:
    /**
     * 构造函数 - 创建RteManager实例
     * 调用时机：应用程序启动时，在任何其他操作之前
     */
    RteManager();
    
    /**
     * 析构函数 - 清理资源并断开RTE服务连接
     * 调用时机：应用程序关闭时，自动调用
     */
    ~RteManager();

    /**
     * 设置接收RTE事件的事件处理器
     * @param handler 事件处理器接口指针
     * 调用时机：构造之后，Initialize()之前
     */
    void SetEventHandler(IRteManagerEventHandler* handler);
    
    /**
     * 使用配置初始化RTE SDK
     * @param config 包含appId和userId的配置
     * @return 初始化成功返回true，失败返回false
     * 调用时机：SetEventHandler()之后，JoinChannel()之前
     */
    bool Initialize(const RteManagerConfig& config);
    
    /**
     * 销毁RTE SDK并释放所有资源
     * 调用时机：应用程序关闭时，析构函数之前
     */
    void Destroy();

    /**
     * 加入指定ID和令牌的频道
     * @param channelId 目标频道标识符
     * @param token 频道访问的认证令牌
     * @return 加入成功返回true，失败返回false
     * 调用时机：Initialize()之后，当用户想要进入频道时
     */
    bool JoinChannel(const std::string& channelId, const std::string& token);
    
    /**
     * 离开当前频道并断开本地用户连接
     * 调用时机：当用户想要退出当前频道时
     */
    void LeaveChannel();
    
    /**
     * 更新当前连接的认证令牌
     * @param token 新的认证令牌
     * 调用时机：当令牌过期且有新令牌可用时
     */
    void RenewToken(const std::string& token);

    /**
     * 启用或禁用本地音频采集
     * @param enabled true启用，false禁用
     * 调用时机：当用户切换麦克风开关时
     */
    void SetLocalAudioCaptureEnabled(bool enabled);
    
    /**
     * 启用或禁用本地视频采集
     * @param enabled true启用，false禁用
     * 调用时机：当用户切换摄像头开关时
     */
    void SetLocalVideoCaptureEnabled(bool enabled);

    /**
     * 更新视图到用户的绑定关系
     * @param viewToUserMap 视图指针到用户ID的映射
     * 调用时机：当UI布局改变或用户列表更新时
     */
    void SetViewUserBindings(const std::map<void*, std::string>& viewToUserMap);
    
private:
    friend class RteManagerEventObserver;

    /**
     * 处理远程用户加入事件（由RteManagerEventObserver调用）
     * @param userId 加入用户的ID
     * 调用时机：当远程用户加入频道时自动调用
     */
    void OnRemoteUserJoined(const std::string& userId);
    
    /**
     * 处理远程用户离开事件（由RteManagerEventObserver调用）
     * @param userId 离开用户的ID
     * 调用时机：当远程用户离开频道时自动调用
     */
    void OnRemoteUserLeft(const std::string& userId);

    /**
     * 为特定用户设置远程视频渲染
     * @param userId 目标用户ID
     * @param view 视频渲染的视图指针
     * @return 成功返回0，失败返回-1
     * 调用时机：当需要显示远程用户视频时
     */
    int SetupRemoteVideo(const std::string& userId, void* view);

private:
    // RTE SDK核心组件
    std::shared_ptr<rte::Rte> m_rte;                           ///< 主RTE SDK实例
    std::shared_ptr<rte::LocalUser> m_localUser;               ///< 本地用户表示
    std::shared_ptr<rte::Channel> m_channel;                   ///< 当前频道实例
    std::shared_ptr<rte::LocalRealTimeStream> m_localStream;   ///< 用于发布的本地媒体流
    
    // 本地媒体轨道
    std::shared_ptr<rte::MicAudioTrack> m_micAudioTrack;       ///< 本地麦克风音频轨道
    std::shared_ptr<rte::CameraVideoTrack> m_cameraVideoTrack; ///< 本地摄像头视频轨道
    
    // 事件处理
    std::shared_ptr<rte::ChannelObserver> m_channelObserver;   ///< 频道事件观察者
    IRteManagerEventHandler* m_eventHandler;                   ///< 外部事件处理器接口

    // 配置和状态
    std::string m_appId;                                       ///< Agora应用ID
    std::string m_userId;                                      ///< 本地用户ID
    std::string m_channelId;                                   ///< 当前频道ID

    // 线程安全状态管理
    std::mutex m_mutex;                                        ///< 用于保护共享状态的互斥锁
    std::map<void*, std::string> m_viewToUserMap;             ///< 视图指针到用户ID的映射
};