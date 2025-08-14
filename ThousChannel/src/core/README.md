## `RteManager` 类方法

### 生命周期管理

- **`RteManager()`**
  - **功能**：构造函数，创建 `RteManager` 实例。

- **`~RteManager()`**
  - **功能**：析构函数，负责清理资源。

- **`Initialize(const RteManagerConfig& config)`**
  - **功能**：初始化RTE引擎。这是调用任何其他方法之前必须执行的第一步。
  - **参数**：
    - `config`: 一个 `RteManagerConfig` 结构体，包含 `appId`、`userId` 和 `userToken`。

- **`Destroy()`**
  - **功能**：释放由 `Initialize` 创建的所有资源。在应用程序退出时调用。

- **`SetEventHandler(IRteManagerEventHandler* handler)`**
  - **功能**：注册一个事件处理器，用于接收来自RTE引擎的回调。
  - **参数**：
    - `handler`: 一个实现了 `IRteManagerEventHandler` 接口的对象的指针。

### 频道操作

- **`JoinChannel(const std::string& channelId, const std::string& token)`**
  - **功能**：加入一个指定的音视频频道。
  - **参数**：
    - `channelId`: 要加入的频道ID。
    - `token`: 用于身份验证的频道Token。

- **`LeaveChannel()`**
  - **功能**：离开当前所在的频道。

### 本地媒体控制

- **`SetLocalAudioCaptureEnabled(bool enabled)`**
  - **功能**：开启或关闭本地音频采集和发送。
  - **参数**：
    - `enabled`: `true` 为开启，`false` 为关闭。

- **`SetLocalVideoCaptureEnabled(bool enabled)`**
  - **功能**：开启或关闭本地视频采集和发送。
  - **参数**：
    - `enabled`: `true` 为开启，`false` 为关闭。

### 远端媒体订阅

- **`SubscribeRemoteVideo(const std::string& userId)`**
  - **功能**：订阅指定远端用户的视频流。
  - **参数**：
    - `userId`: 要订阅的远端用户的ID。

- **`UnsubscribeRemoteVideo(const std::string& userId)`**
  - **功能**：取消订阅指定远端用户的视频流。
  - **参数**：
    - `userId`: 要取消订阅的远端用户的ID。

- **`SubscribeRemoteAudio(const std::string& userId)`**
  - **功能**：订阅指定远端用户的音频流。
  - **参数**：
    - `userId`: 要订阅的远端用户的ID。

- **`UnsubscribeRemoteAudio(const std::string& userId)`**
  - **功能**：取消订阅指定远端用户的音频流。
  - **参数**：
    - `userId`: 要取消订阅的远端用户的ID。

### 高级功能

- **`SetSubscribedUsers(const std::vector<std::string>& userIds)`**
  - **功能**：批量设置需要订阅的远端用户（视频）。
  - **参数**：
    - `userIds`: 包含所有希望订阅的用户ID的向量。
  - 执行过程：
    - `userIds` 和内部存储的订阅列表对照，进行订阅和取消订阅

- **`SetViewUserBindings(const std::map<void*, std::string>& viewToUserMap)`**
  - **功能**：将视频渲染窗口（视图）与指定的用户ID进行绑定。
  - **参数**：
    - `viewToUserMap`: 一个 `map`，键为窗口句柄（`void*`），值为用户ID。
  - 执行过程：
    - 比对内部映射关系
    - 解绑新数据中不存在的`userId`、解绑旧数据中与新数据不对应的映射
    - 绑定并记录新的映射关系

---

## `IRteManagerEventHandler` 接口

这是一个回调接口，您需要实现它来处理来自 `RteManager` 的异步事件。

- **`OnConnectionStateChanged(int state)`**: 当网络连接状态发生改变时触发。
- **`OnUserJoined(const std::string& userId)`**: 当有新的远端用户加入频道时触发。
  - UI操作：更新本地视窗

- **`OnUserLeft(const std::string& userId)`**: 当有远端用户离开频道时触发。
  - UI操作：更新本地视窗
- **`OnLocalAudioStateChanged(int state)`**: 当本地音频的发布状态改变时触发。
  - UI操作：更新本地 A+- 显示
- **`OnLocalVideoStateChanged(int state)`**: 当本地视频的发布状态改变时触发。
  - UI操作：更新本地 V+- 显示
- **`OnRemoteAudioStateChanged(const std::string& userId, int state)`**: 当远端用户的音频状态（如静音/取消静音）改变时触发。
  - 可能没有该状态
- **`OnRemoteVideoStateChanged(const std::string& userId, int state)`**: 当远端用户的视频状态（如开启/关闭摄像头）改变时触发。
  - 可能没有该状态
- **`OnError(int error)`**: 当SDK内部发生错误时触发。
  - UI操作：显示错误提示
  - 日志记录：记录错误信息
- **`OnUserListChanged()`**: 当频道内的用户列表发生变化时触发。
  - UI操作：更新用户列表和用户数量显示
