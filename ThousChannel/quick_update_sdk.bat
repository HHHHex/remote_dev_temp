@echo off
setlocal enabledelayedexpansion

:: 配置参数
set SDK_URL=%1
set BACKUP_SDK=%2

echo ========================================
echo Quick Agora SDK Update Script
echo ========================================

:: 检查参数
if "%SDK_URL%"=="" (
    echo Usage: quick_update_sdk.bat [SDK_URL] [BACKUP_OPTION]
    echo.
    echo Parameters:
    echo   SDK_URL       - SDK download URL (required)
    echo   BACKUP_OPTION - 1 to backup current SDK, 0 or empty to skip backup
    echo.
    echo Example:
    echo   quick_update_sdk.bat "http://example.com/sdk.zip" 1
    echo.
    exit /b 1
)

:: 设置变量
set SDK_DIR=sdk
set BACKUP_DIR=sdk_backup_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%
set BACKUP_DIR=%BACKUP_DIR: =0%
set TEMP_DIR=temp_sdk
set SDK_ZIP=new_sdk.zip

echo SDK URL: %SDK_URL%
echo Backup option: %BACKUP_SDK%

:: 创建备份
if "%BACKUP_SDK%"=="1" (
    if exist "%SDK_DIR%" (
        echo Creating backup of current SDK...
        xcopy "%SDK_DIR%" "%BACKUP_DIR%" /E /I /Y >nul 2>&1
        if !errorlevel! equ 0 (
            echo   ✓ Backup created: %BACKUP_DIR%
        ) else (
            echo   ✗ Failed to create backup
            set /p CONTINUE="Continue without backup? (y/N): "
            if /i not "!CONTINUE!"=="y" exit /b 1
        )
    ) else (
        echo No existing SDK to backup
    )
)

:: 清理临时文件
echo Cleaning temporary files...
if exist "%SDK_ZIP%" del "%SDK_ZIP%" /Q >nul 2>&1
if exist "%TEMP_DIR%" rmdir "%TEMP_DIR%" /S /Q >nul 2>&1
for /d %%d in (Shengwang_Native_SDK_for_Windows*) do rmdir "%%d" /S /Q >nul 2>&1

:: 下载SDK
echo Downloading SDK...
powershell -Command "& {try { (New-Object System.Net.WebClient).DownloadFile('%SDK_URL%', '%SDK_ZIP%'); Write-Host '  ✓ Download completed' -ForegroundColor Green} catch { Write-Host '  ✗ Download failed: ' + $_.Exception.Message -ForegroundColor Red; exit 1}}"
if !errorlevel! neq 0 (
    echo Download failed
    exit /b 1
)

:: 检查下载文件
if not exist "%SDK_ZIP%" (
    echo Downloaded file not found
    exit /b 1
)

:: 显示文件大小
for %%A in ("%SDK_ZIP%") do set FILE_SIZE=%%~zA
set /a FILE_SIZE_MB=%FILE_SIZE% / 1048576
echo   File size: %FILE_SIZE_MB% MB

:: 解压SDK
echo Extracting SDK...
powershell -Command "& {try { Expand-Archive -Path '%SDK_ZIP%' -DestinationPath '.' -Force; Write-Host '  ✓ Extraction completed' -ForegroundColor Green} catch { Write-Host '  ✗ Extraction failed: ' + $_.Exception.Message -ForegroundColor Red; exit 1}}"
if !errorlevel! neq 0 (
    echo Extraction failed
    exit /b 1
)

:: 查找解压后的目录
set FOUND_DIR=
for /d %%d in (Shengwang_Native_SDK_for_Windows*) do set FOUND_DIR=%%d
if "%FOUND_DIR%"=="" (
    echo No SDK directory found in extracted files
    exit /b 1
)
echo   Found SDK directory: %FOUND_DIR%

:: 删除现有SDK
if exist "%SDK_DIR%" (
    echo Removing existing SDK...
    rmdir "%SDK_DIR%" /S /Q
    echo   ✓ Existing SDK removed
)

:: 安装新SDK
echo Installing new SDK...
if exist "%FOUND_DIR%\sdk" (
    move "%FOUND_DIR%\sdk" "%SDK_DIR%" >nul 2>&1
) else (
    move "%FOUND_DIR%" "%SDK_DIR%" >nul 2>&1
)

if !errorlevel! equ 0 (
    echo   ✓ SDK installation completed
) else (
    echo   ✗ SDK installation failed
    exit /b 1
)

:: 验证安装
echo Verifying SDK installation...
set VERIFY_OK=1

if not exist "%SDK_DIR%\high_level_api\include\rte_cpp.h" (
    echo   ✗ rte_cpp.h (missing)
    set VERIFY_OK=0
) else (
    echo   ✓ rte_cpp.h
)

if not exist "%SDK_DIR%\high_level_api\include\rte_base\rte_cpp_rte.h" (
    echo   ✗ rte_cpp_rte.h (missing)
    set VERIFY_OK=0
) else (
    echo   ✓ rte_cpp_rte.h
)

if not exist "%SDK_DIR%\x86_64\agora_rtc_sdk.dll" (
    echo   ✗ agora_rtc_sdk.dll (missing)
    set VERIFY_OK=0
) else (
    echo   ✓ agora_rtc_sdk.dll
)

if !VERIFY_OK! equ 0 (
    echo SDK verification failed
    exit /b 1
)

:: 清理临时文件
echo Cleaning up...
if exist "%SDK_ZIP%" del "%SDK_ZIP%" /Q >nul 2>&1
if exist "%FOUND_DIR%" rmdir "%FOUND_DIR%" /S /Q >nul 2>&1

:: 显示结果
echo.
echo ========================================
echo SDK Update Completed Successfully!
echo ========================================
echo SDK Path: %CD%\%SDK_DIR%

if "%BACKUP_SDK%"=="1" if exist "%BACKUP_DIR%" (
    echo Backup: %CD%\%BACKUP_DIR%
)

:: 显示SDK信息
if exist "%SDK_DIR%" (
    dir "%SDK_DIR%" /S | find /c "File(s)" > temp_count.txt
    set /p FILE_COUNT=<temp_count.txt
    del temp_count.txt >nul 2>&1
    echo Files: %FILE_COUNT%
)

echo ========================================
