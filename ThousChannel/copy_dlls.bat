@echo off
setlocal enabledelayedexpansion

:: 获取配置参数，默认为Debug
set CONFIGURATION=%1
if "%CONFIGURATION%"=="" set CONFIGURATION=Debug

echo ========================================
echo Copying Agora SDK DLLs to output directories
echo Configuration: %CONFIGURATION%
echo Architecture: x64 (64-bit)
echo ========================================

set OUTPUT_DIR1=..\x64\%CONFIGURATION%
set OUTPUT_DIR2=project\x64\%CONFIGURATION%
set SDK_DIR=sdk\x86_64

:: 检查64位SDK目录是否存在
if not exist "%SDK_DIR%" (
    echo [ERROR] 64-bit SDK directory not found: %SDK_DIR%
    echo Please ensure you have the correct 64-bit SDK installed
    pause
    exit /b 1
)

:: 复制到根目录的输出目录
if not exist "%OUTPUT_DIR1%" (
    echo Creating output directory: %OUTPUT_DIR1%
    mkdir "%OUTPUT_DIR1%"
)

:: 复制到项目目录的输出目录
if not exist "%OUTPUT_DIR2%" (
    echo Creating output directory: %OUTPUT_DIR2%
    mkdir "%OUTPUT_DIR2%"
)

echo Copying 64-bit DLLs from %SDK_DIR% to output directories...

:: 复制所有64位DLL文件
for %%F in ("%SDK_DIR%\*.dll") do (
    copy "%%F" "%OUTPUT_DIR1%\" /Y >nul 2>&1
    if !errorlevel! equ 0 (
        echo Copied %%~nxF to %OUTPUT_DIR1%
    ) else (
        echo Failed to copy %%~nxF to %OUTPUT_DIR1%
    )
    
    copy "%%F" "%OUTPUT_DIR2%\" /Y >nul 2>&1
    if !errorlevel! equ 0 (
        echo Copied %%~nxF to %OUTPUT_DIR2%
    ) else (
        echo Failed to copy %%~nxF to %OUTPUT_DIR2%
    )
)

:: 验证关键DLL文件
echo.
echo Verifying critical DLL files...
if exist "%OUTPUT_DIR1%\agora_rtc_sdk.dll" (
    echo ✓ agora_rtc_sdk.dll found in %OUTPUT_DIR1%
) else (
    echo ✗ agora_rtc_sdk.dll missing in %OUTPUT_DIR1%
)

if exist "%OUTPUT_DIR2%\agora_rtc_sdk.dll" (
    echo ✓ agora_rtc_sdk.dll found in %OUTPUT_DIR2%
) else (
    echo ✗ agora_rtc_sdk.dll missing in %OUTPUT_DIR2%
)

echo.
echo ========================================
echo 64-bit DLL copy completed
echo ======================================== 