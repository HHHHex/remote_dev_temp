@echo off
chcp 65001 >nul
title ThousChannel Launcher

:: 设置64位DLL搜索路径
set PATH=%PATH%;%~dp0sdk

echo ========================================
echo           ThousChannel v1.0.0
echo ========================================
echo.

REM Check if main executable exists
if not exist "ThousChannel.exe" (
    echo [ERROR] Main executable ThousChannel.exe not found
    pause
    exit /b 1
)

echo [INFO] Starting ThousChannel...
start "" "ThousChannel.exe"
echo [INFO] Application started
pause
