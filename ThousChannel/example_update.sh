#!/bin/bash

# Agora SDK 更新示例脚本
# 展示如何使用不同的SDK更新脚本

echo "========================================"
echo "Agora SDK Update Examples"
echo "========================================"

# 示例1: 使用Python脚本更新到最新内部SDK
echo "Example 1: Update to latest internal SDK using Python script"
echo "Command: python3 update_sdk.py -u 'http://10.80.1.174:8090/agora_sdk/develop/zhouzhengyan/2025-08-21/windows/full_with_rtm/Shengwang_Native_SDK_for_Windows_rel.v3.0_jira_NMS_26113_arsenal_20250821.29917_29917_FULL_WITH_RTM_20250821_1725_848536.zip' -b"
echo ""

# 示例2: 使用Python脚本清理临时文件
echo "Example 2: Clean temporary files using Python script"
echo "Command: python3 update_sdk.py -c"
echo ""

# 示例3: 使用Python脚本显示帮助
echo "Example 3: Show help for Python script"
echo "Command: python3 update_sdk.py -h"
echo ""

# 示例4: 使用批处理脚本 (Windows)
echo "Example 4: Update using batch script (Windows only)"
echo "Command: quick_update_sdk.bat 'http://your_sdk_url.zip' 1"
echo ""

# 示例5: 使用PowerShell脚本 (Windows)
echo "Example 5: Update using PowerShell script (Windows only)"
echo "Command: .\update_sdk.ps1 -SDKUrl 'http://your_sdk_url.zip' -Backup -Force"
echo ""

echo "========================================"
echo "Available Scripts:"
echo "========================================"
echo "1. update_sdk.py          - Cross-platform Python script (Recommended)"
echo "2. update_sdk.ps1         - Windows PowerShell script"
echo "3. quick_update_sdk.bat   - Windows batch script"
echo "4. install_agora_sdk.ps1  - Original installation script"
echo ""

echo "For detailed usage instructions, see: SDK_UPDATE_README.md"
