# 声网SDK云端集成说明

## 概述

本项目采用声网API-Example的集成方式，使用vcpkg进行云端依赖管理，确保SDK版本的一致性和可更新性。

## 集成方式

### 1. 云端依赖管理 (vcpkg)

我们使用vcpkg作为包管理器，通过云端仓库自动下载和管理声网SDK：

```json
// vcpkg.json
{
  "name": "thouschannel",
  "version": "1.0.0",
  "dependencies": [
    {
      "name": "agora-rtc-sdk",
      "version>=": "4.2.6"
    }
  ]
}
```

### 2. CMake构建系统

使用CMake进行跨平台构建，自动处理依赖关系：

```cmake
# CMakeLists.txt
find_package(agora-rtc-sdk CONFIG REQUIRED)
target_link_libraries(ThousChannel PRIVATE agora::agora-rtc-sdk)
```

## 快速开始

### 1. 自动构建 (推荐)

使用提供的构建脚本：

```bash
# Windows (PowerShell)
.\build.ps1

# Windows (Batch)
.\build.bat
```

构建脚本会自动：
- 安装vcpkg (如果未安装)
- 下载并安装声网SDK
- 配置CMake项目
- 构建应用程序

### 2. 手动构建

如果需要手动控制构建过程：

```bash
# 1. 安装vcpkg
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat

# 2. 安装声网SDK
vcpkg install agora-rtc-sdk:x64-windows

# 3. 配置项目
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake

# 4. 构建项目
cmake --build . --config Release
```

## 优势

### 1. 云端更新
- SDK版本通过vcpkg云端仓库管理
- 支持自动版本更新
- 无需手动下载和配置SDK文件

### 2. 版本一致性
- 通过vcpkg.json锁定SDK版本
- 团队成员使用相同版本的SDK
- 避免版本不匹配问题

### 3. 跨平台支持
- CMake支持Windows、Linux、macOS
- 统一的构建配置
- 便于CI/CD集成

### 4. 依赖管理
- 自动处理SDK依赖关系
- 支持多个SDK版本并存
- 简化项目配置

## SDK版本管理

### 更新SDK版本

1. 修改vcpkg.json中的版本号：
```json
{
  "dependencies": [
    {
      "name": "agora-rtc-sdk",
      "version>=": "4.3.0"  // 更新到新版本
    }
  ]
}
```

2. 重新安装SDK：
```bash
vcpkg remove agora-rtc-sdk:x64-windows
vcpkg install agora-rtc-sdk:x64-windows
```

3. 重新构建项目：
```bash
.\build.ps1
```

### 锁定特定版本

在vcpkg.json中使用overrides锁定特定版本：

```json
{
  "overrides": [
    {
      "name": "agora-rtc-sdk",
      "version": "4.2.6"
    }
  ]
}
```

## 项目结构

```
ThousChannel/
├── CMakeLists.txt          # CMake配置文件
├── vcpkg.json             # vcpkg依赖配置
├── build.ps1              # PowerShell构建脚本
├── build.bat              # Batch构建脚本
├── src/
│   ├── core/
│   │   ├── RtcEngine.h    # RTC引擎管理类
│   │   └── RtcEngine.cpp  # RTC引擎实现
│   └── ui/
│       └── dialogs/
│           ├── JoinChannelDlg.h
│           └── ChannelPageDlg.h
└── resources/
    └── ThousChannel.rc
```

## 故障排除

### 1. vcpkg安装失败
```bash
# 检查网络连接
ping github.com

# 使用代理 (如果需要)
set HTTP_PROXY=http://proxy:port
set HTTPS_PROXY=http://proxy:port
```

### 2. SDK下载失败
```bash
# 清理vcpkg缓存
vcpkg remove agora-rtc-sdk:x64-windows
vcpkg cache remove

# 重新安装
vcpkg install agora-rtc-sdk:x64-windows
```

### 3. CMake配置失败
```bash
# 检查CMake版本
cmake --version

# 清理构建目录
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
```

## 与API-Example的对比

| 特性 | 传统方式 | 云端集成方式 |
|------|----------|--------------|
| SDK获取 | 手动下载 | 自动下载 |
| 版本管理 | 手动管理 | 自动管理 |
| 更新方式 | 手动替换 | 云端更新 |
| 依赖处理 | 手动配置 | 自动处理 |
| 跨平台 | 困难 | 简单 |
| CI/CD | 复杂 | 简单 |

## 总结

采用云端集成方式的主要优势：

1. **自动化**：减少手动配置工作
2. **一致性**：确保版本一致性
3. **可维护性**：简化依赖管理
4. **可扩展性**：支持多平台和多SDK
5. **现代化**：使用现代C++构建工具链

这种方式特别适合团队开发和持续集成环境，能够显著提高开发效率和项目质量。 