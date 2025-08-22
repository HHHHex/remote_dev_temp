# Agora SDK 更新脚本使用说明

本项目提供了多个脚本来帮助您自动更新 Agora SDK。

## 脚本概览

### 1. `update_sdk.py` - 跨平台 Python 更新脚本 (推荐)
功能最全面的脚本，支持Windows、macOS和Linux，支持多种参数和选项。

### 2. `update_sdk.ps1` - PowerShell 更新脚本
Windows专用的PowerShell脚本，功能全面。

### 3. `quick_update_sdk.bat` - 快速批处理更新脚本
Windows专用的简单批处理脚本，适合快速更新。

### 4. `install_agora_sdk.ps1` - 原始安装脚本
原有的SDK安装脚本，主要用于初始安装。

## 使用方法

### Python 脚本 (推荐，跨平台)

```bash
# 基本用法 - 使用默认URL
python3 update_sdk.py

# 使用自定义URL
python3 update_sdk.py -u "http://10.80.1.174:8090/agora_sdk/develop/zhouzhengyan/2025-08-21/windows/full_with_rtm/Shengwang_Native_SDK_for_Windows_rel.v3.0_jira_NMS_26113_arsenal_20250821.29917_29917_FULL_WITH_RTM_20250821_1725_848536.zip"

# 创建备份并强制更新
python3 update_sdk.py -u "your_sdk_url" -b -f

# 只清理临时文件
python3 update_sdk.py -c

# 显示帮助信息
python3 update_sdk.py -h
```

**参数说明：**
- `-u, --url`: 自定义SDK下载URL
- `-v, --version`: SDK版本号 (默认: 3.0)
- `-b, --backup`: 在更新前创建备份
- `-f, --force`: 强制更新，不询问确认
- `-c, --clean-only`: 只清理临时文件，不下载/安装
- `-h, --help`: 显示帮助信息

### PowerShell 脚本 (Windows)

```powershell
# 基本用法 - 使用默认URL
.\update_sdk.ps1

# 使用自定义URL
.\update_sdk.ps1 -SDKUrl "http://10.80.1.174:8090/agora_sdk/develop/zhouzhengyan/2025-08-21/windows/full_with_rtm/Shengwang_Native_SDK_for_Windows_rel.v3.0_jira_NMS_26113_arsenal_20250821.29917_29917_FULL_WITH_RTM_20250821_1725_848536.zip"

# 创建备份并强制更新
.\update_sdk.ps1 -SDKUrl "your_sdk_url" -Backup -Force

# 只清理临时文件
.\update_sdk.ps1 -CleanOnly
```

**参数说明：**
- `-SDKUrl`: 自定义SDK下载URL
- `-SDKVersion`: SDK版本号 (默认: 3.0)
- `-Backup`: 在更新前创建备份
- `-Force`: 强制更新，不询问确认
- `-CleanOnly`: 只清理临时文件，不下载/安装

### 批处理脚本

```batch
# 基本用法
quick_update_sdk.bat "http://your_sdk_url.zip"

# 创建备份
quick_update_sdk.bat "http://your_sdk_url.zip" 1

# 不创建备份
quick_update_sdk.bat "http://your_sdk_url.zip" 0
```

**参数说明：**
- 第一个参数: SDK下载URL (必需)
- 第二个参数: 是否创建备份 (1=是, 0=否)

## 实际使用示例

### 更新到最新的内部SDK

```bash
# 使用Python脚本 (推荐)
python3 update_sdk.py -u "http://10.80.1.174:8090/agora_sdk/develop/zhouzhengyan/2025-08-21/windows/full_with_rtm/Shengwang_Native_SDK_for_Windows_rel.v3.0_jira_NMS_26113_arsenal_20250821.29917_29917_FULL_WITH_RTM_20250821_1725_848536.zip" -b

# 使用PowerShell脚本 (Windows)
.\update_sdk.ps1 -SDKUrl "http://10.80.1.174:8090/agora_sdk/develop/zhouzhengyan/2025-08-21/windows/full_with_rtm/Shengwang_Native_SDK_for_Windows_rel.v3.0_jira_NMS_26113_arsenal_20250821.29917_29917_FULL_WITH_RTM_20250821_1725_848536.zip" -Backup

# 使用批处理脚本 (Windows)
quick_update_sdk.bat "http://10.80.1.174:8090/agora_sdk/develop/zhouzhengyan/2025-08-21/windows/full_with_rtm/Shengwang_Native_SDK_for_Windows_rel.v3.0_jira_NMS_26113_arsenal_20250821.29917_29917_FULL_WITH_RTM_20250821_1725_848536.zip" 1
```

### 清理临时文件

```bash
# 使用Python脚本
python3 update_sdk.py -c

# 使用PowerShell脚本 (Windows)
.\update_sdk.ps1 -CleanOnly
```

## 脚本功能特性

### 安全特性
- ✅ 自动备份现有SDK (可选)
- ✅ 验证下载文件完整性
- ✅ 验证安装结果
- ✅ 错误处理和回滚
- ✅ 临时文件自动清理

### 验证功能
脚本会自动验证以下关键文件：
- `high_level_api\include\rte_cpp.h`
- `high_level_api\include\rte_base\rte_cpp_rte.h`
- `high_level_api\include\rte_base\rte_cpp_channel.h`
- `high_level_api\include\rte_base\rte_cpp_user.h`
- `x86_64\agora_rtc_sdk.dll`
- `x86_64\agora_rtc_sdk.dll.lib`

### 备份管理
- 备份目录格式: `sdk_backup_YYYYMMDD_HHMMSS`
- 自动清理临时文件
- 支持强制覆盖备份

## 注意事项

1. **权限要求**: 脚本需要管理员权限来操作文件系统
2. **网络连接**: 确保能够访问SDK下载URL
3. **磁盘空间**: 确保有足够的磁盘空间存储SDK和备份
4. **防病毒软件**: 某些防病毒软件可能会阻止下载或解压操作

## 故障排除

### 常见问题

1. **下载失败**
   - 检查网络连接
   - 验证URL是否正确
   - 检查防火墙设置

2. **解压失败**
   - 检查磁盘空间
   - 验证下载文件完整性
   - 检查文件权限

3. **验证失败**
   - 检查SDK包是否完整
   - 重新下载SDK
   - 联系SDK提供方

### 手动恢复

如果脚本执行失败，可以手动恢复：

```powershell
# 恢复备份
if (Test-Path "sdk_backup_YYYYMMDD_HHMMSS") {
    Remove-Item "sdk" -Recurse -Force -ErrorAction SilentlyContinue
    Move-Item "sdk_backup_YYYYMMDD_HHMMSS" "sdk"
}
```

## 版本历史

- **v1.0**: 初始版本，支持基本SDK更新功能
- **v1.1**: 添加备份功能和验证机制
- **v1.2**: 添加批处理脚本和错误处理改进

## 联系支持

如果遇到问题，请检查：
1. 脚本输出日志
2. 系统错误信息
3. 网络连接状态
4. 磁盘空间使用情况
