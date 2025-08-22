param(
    [Parameter(Mandatory=$false)]
    [string]$SDKUrl = "",
    
    [Parameter(Mandatory=$false)]
    [string]$SDKVersion = "3.0",
    
    [Parameter(Mandatory=$false)]
    [switch]$Backup = $false,
    
    [Parameter(Mandatory=$false)]
    [switch]$Force = $false,
    
    [Parameter(Mandatory=$false)]
    [switch]$CleanOnly = $false
)

# 设置错误处理
$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Green
Write-Host "Agora SDK Update Script" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green

# 配置变量
$currentDir = Get-Location
$sdkDir = "sdk"
$backupDir = "sdk_backup_$(Get-Date -Format 'yyyyMMdd_HHmmss')"
$tempDir = "temp_sdk"
$sdkZip = "new_sdk.zip"

# 函数：显示帮助信息
function Show-Help {
    Write-Host "Usage:" -ForegroundColor Yellow
    Write-Host "  .\update_sdk.ps1 [-SDKUrl <url>] [-SDKVersion <version>] [-Backup] [-Force] [-CleanOnly]" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Parameters:" -ForegroundColor Yellow
    Write-Host "  -SDKUrl      : Custom SDK download URL" -ForegroundColor White
    Write-Host "  -SDKVersion  : SDK version (default: 3.0)" -ForegroundColor White
    Write-Host "  -Backup      : Create backup of current SDK before replacement" -ForegroundColor White
    Write-Host "  -Force       : Force update without confirmation" -ForegroundColor White
    Write-Host "  -CleanOnly   : Only clean temporary files, don't download/install" -ForegroundColor White
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor Yellow
    Write-Host "  .\update_sdk.ps1" -ForegroundColor Cyan
    Write-Host "  .\update_sdk.ps1 -SDKUrl 'http://example.com/sdk.zip' -Backup" -ForegroundColor Cyan
    Write-Host "  .\update_sdk.ps1 -CleanOnly" -ForegroundColor Cyan
}

# 函数：清理临时文件
function Clean-TempFiles {
    Write-Host "Cleaning temporary files..." -ForegroundColor Yellow
    
    $tempItems = @($sdkZip, $tempDir)
    foreach ($item in $tempItems) {
        if (Test-Path $item) {
            try {
                Remove-Item $item -Recurse -Force -ErrorAction SilentlyContinue
                Write-Host "  ✓ Removed: $item" -ForegroundColor Green
            } catch {
                Write-Host "  ⚠ Warning: Could not remove $item" -ForegroundColor Yellow
            }
        }
    }
    
    # 清理可能的解压目录
    $extractedDirs = Get-ChildItem -Directory -Name "Shengwang_Native_SDK_for_Windows*" -ErrorAction SilentlyContinue
    foreach ($dir in $extractedDirs) {
        try {
            Remove-Item $dir -Recurse -Force -ErrorAction SilentlyContinue
            Write-Host "  ✓ Removed: $dir" -ForegroundColor Green
        } catch {
            Write-Host "  ⚠ Warning: Could not remove $dir" -ForegroundColor Yellow
        }
    }
}

# 函数：验证SDK安装
function Test-SDKInstallation {
    Write-Host "Verifying SDK installation..." -ForegroundColor Yellow
    
    $requiredFiles = @(
        "high_level_api\include\rte_cpp.h",
        "high_level_api\include\rte_base\rte_cpp_rte.h",
        "high_level_api\include\rte_base\rte_cpp_channel.h",
        "high_level_api\include\rte_base\rte_cpp_user.h",
        "x86_64\agora_rtc_sdk.dll",
        "x86_64\agora_rtc_sdk.dll.lib"
    )
    
    $allValid = $true
    foreach ($file in $requiredFiles) {
        $fullPath = Join-Path $sdkDir $file
        if (Test-Path $fullPath) {
            Write-Host "  ✓ $file" -ForegroundColor Green
        } else {
            Write-Host "  ✗ $file (missing)" -ForegroundColor Red
            $allValid = $false
        }
    }
    
    return $allValid
}

# 函数：显示SDK信息
function Show-SDKInfo {
    Write-Host "Current SDK Information:" -ForegroundColor Cyan
    
    if (Test-Path $sdkDir) {
        $sdkSize = (Get-ChildItem $sdkDir -Recurse | Measure-Object -Property Length -Sum).Sum
        $sdkSizeMB = [math]::Round($sdkSize / 1MB, 2)
        Write-Host "  Size: $sdkSizeMB MB" -ForegroundColor White
        
        $fileCount = (Get-ChildItem $sdkDir -Recurse -File).Count
        Write-Host "  Files: $fileCount" -ForegroundColor White
        
        $dirCount = (Get-ChildItem $sdkDir -Recurse -Directory).Count
        Write-Host "  Directories: $dirCount" -ForegroundColor White
    } else {
        Write-Host "  No SDK directory found" -ForegroundColor Red
    }
}

# 主程序开始
try {
    # 如果只是清理，执行清理后退出
    if ($CleanOnly) {
        Clean-TempFiles
        Write-Host "Clean operation completed!" -ForegroundColor Green
        exit 0
    }
    
    # 显示当前SDK信息
    Show-SDKInfo
    
    # 检查是否已有SDK目录
    if (Test-Path $sdkDir) {
        if (-not $Force) {
            Write-Host "SDK directory already exists." -ForegroundColor Yellow
            $response = Read-Host "Do you want to continue? (y/N)"
            if ($response -ne "y" -and $response -ne "Y") {
                Write-Host "Operation cancelled by user." -ForegroundColor Yellow
                exit 0
            }
        }
        
        # 创建备份
        if ($Backup) {
            Write-Host "Creating backup of current SDK..." -ForegroundColor Yellow
            try {
                Copy-Item $sdkDir $backupDir -Recurse -Force
                Write-Host "  ✓ Backup created: $backupDir" -ForegroundColor Green
            } catch {
                Write-Host "  ✗ Failed to create backup: $($_.Exception.Message)" -ForegroundColor Red
                if (-not $Force) {
                    $response = Read-Host "Continue without backup? (y/N)"
                    if ($response -ne "y" -and $response -ne "Y") {
                        exit 1
                    }
                }
            }
        }
    }
    
    # 清理临时文件
    Clean-TempFiles
    
    # 确定下载URL
    if ([string]::IsNullOrEmpty($SDKUrl)) {
        # 使用默认URL（这里需要根据实际情况调整）
        $SDKUrl = "https://download.agora.io/sdk/release/Agora_Native_SDK_for_Windows_v${SDKVersion}_FULL.zip"
        Write-Host "Using default SDK URL: $SDKUrl" -ForegroundColor Cyan
    } else {
        Write-Host "Using custom SDK URL: $SDKUrl" -ForegroundColor Cyan
    }
    
    # 下载SDK
    Write-Host "Downloading SDK..." -ForegroundColor Yellow
    try {
        $webClient = New-Object System.Net.WebClient
        $webClient.DownloadFile($SDKUrl, $sdkZip)
        Write-Host "  ✓ Download completed" -ForegroundColor Green
    } catch {
        Write-Host "  ✗ Download failed: $($_.Exception.Message)" -ForegroundColor Red
        exit 1
    }
    
    # 验证下载文件
    if (-not (Test-Path $sdkZip)) {
        Write-Host "  ✗ Downloaded file not found" -ForegroundColor Red
        exit 1
    }
    
    $fileSize = (Get-Item $sdkZip).Length
    $fileSizeMB = [math]::Round($fileSize / 1MB, 2)
    Write-Host "  File size: $fileSizeMB MB" -ForegroundColor White
    
    # 解压SDK
    Write-Host "Extracting SDK..." -ForegroundColor Yellow
    try {
        # 创建临时目录
        New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
        
        # 解压文件
        Expand-Archive -Path $sdkZip -DestinationPath $tempDir -Force
        Write-Host "  ✓ Extraction completed" -ForegroundColor Green
    } catch {
        Write-Host "  ✗ Extraction failed: $($_.Exception.Message)" -ForegroundColor Red
        Clean-TempFiles
        exit 1
    }
    
    # 查找解压后的SDK目录
    $extractedDirs = Get-ChildItem -Path $tempDir -Directory -Name "*SDK*" -ErrorAction SilentlyContinue
    if ($extractedDirs.Count -eq 0) {
        Write-Host "  ✗ No SDK directory found in extracted files" -ForegroundColor Red
        Clean-TempFiles
        exit 1
    }
    
    $sdkSourceDir = Join-Path $tempDir $extractedDirs[0]
    Write-Host "  Found SDK directory: $extractedDirs[0]" -ForegroundColor White
    
    # 删除现有SDK目录
    if (Test-Path $sdkDir) {
        Write-Host "Removing existing SDK directory..." -ForegroundColor Yellow
        Remove-Item $sdkDir -Recurse -Force
        Write-Host "  ✓ Existing SDK removed" -ForegroundColor Green
    }
    
    # 移动新SDK到目标位置
    Write-Host "Installing new SDK..." -ForegroundColor Yellow
    try {
        # 查找SDK子目录
        $sdkSubDir = Join-Path $sdkSourceDir "sdk"
        if (Test-Path $sdkSubDir) {
            Move-Item $sdkSubDir $sdkDir
        } else {
            # 如果没有sdk子目录，直接移动整个目录
            Move-Item $sdkSourceDir $sdkDir
        }
        Write-Host "  ✓ SDK installation completed" -ForegroundColor Green
    } catch {
        Write-Host "  ✗ SDK installation failed: $($_.Exception.Message)" -ForegroundColor Red
        Clean-TempFiles
        exit 1
    }
    
    # 验证安装
    if (Test-SDKInstallation) {
        Write-Host "  ✓ SDK verification successful" -ForegroundColor Green
    } else {
        Write-Host "  ✗ SDK verification failed" -ForegroundColor Red
        exit 1
    }
    
    # 清理临时文件
    Clean-TempFiles
    
    # 显示最终信息
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "SDK Update Completed Successfully!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "SDK Path: $((Get-Location).Path)\$sdkDir" -ForegroundColor Cyan
    Write-Host "Version: $SDKVersion" -ForegroundColor Cyan
    
    if ($Backup -and (Test-Path $backupDir)) {
        Write-Host "Backup: $((Get-Location).Path)\$backupDir" -ForegroundColor Cyan
    }
    
    Show-SDKInfo
    
} catch {
    Write-Host "An error occurred: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host "Stack trace: $($_.ScriptStackTrace)" -ForegroundColor Red
    Clean-TempFiles
    exit 1
}
