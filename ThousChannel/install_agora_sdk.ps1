param(
    [string]$SDKVersion = "4.5.2"
)

Write-Host "========================================" -ForegroundColor Green
Write-Host "Agora SDK Installation Script" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green

$agora_sdk_url = "https://download.agora.io/sdk/release/Agora_Native_SDK_for_Windows_v${SDKVersion}_FULL.zip"
$agora_sdk_zip = "AgoraSdk.zip"
$sdk_dir = "sdk"

if (Test-Path $sdk_dir) {
    Write-Host "SDK directory already exists, skipping download..." -ForegroundColor Yellow
} else {
    Write-Host "Starting download of Agora SDK v${SDKVersion}..." -ForegroundColor Yellow

    New-Item -ItemType Directory -Path $sdk_dir -Force | Out-Null
    New-Item -ItemType Directory -Path "$sdk_dir\x64" -Force | Out-Null
    New-Item -ItemType Directory -Path "$sdk_dir\high_level_api" -Force | Out-Null
    New-Item -ItemType Directory -Path "$sdk_dir\high_level_api\include" -Force | Out-Null

    try {
        Write-Host "Downloading SDK..." -ForegroundColor Cyan
        (New-Object System.Net.WebClient).DownloadFile($agora_sdk_url, $agora_sdk_zip)

        Write-Host "Extracting SDK..." -ForegroundColor Cyan
        Unblock-File $agora_sdk_zip
        Expand-Archive -Path $agora_sdk_zip -DestinationPath . -Force

        Write-Host "Organizing SDK files..." -ForegroundColor Cyan
        if (Test-Path "Agora_Native_SDK_for_Windows_FULL\sdk\x86") {
            Copy-Item "Agora_Native_SDK_for_Windows_FULL\sdk\x86\*" $sdk_dir -Recurse -Force
        }
        if (Test-Path "Agora_Native_SDK_for_Windows_FULL\sdk\x86_64") {
            Copy-Item "Agora_Native_SDK_for_Windows_FULL\sdk\x86_64\*" "$sdk_dir\x64" -Recurse -Force
        }
        if (Test-Path "Agora_Native_SDK_for_Windows_FULL\sdk\high_level_api\include") {
            Copy-Item "Agora_Native_SDK_for_Windows_FULL\sdk\high_level_api\include\*" "$sdk_dir\high_level_api\include" -Recurse -Force
        }

        Remove-Item $agora_sdk_zip -Force -ErrorAction SilentlyContinue
        Remove-Item "Agora_Native_SDK_for_Windows_FULL" -Recurse -Force -ErrorAction SilentlyContinue

        Write-Host "SDK installation completed!" -ForegroundColor Green
    } catch {
        Write-Host "SDK download failed: $($_.Exception.Message)" -ForegroundColor Red
        exit 1
    }
}

Write-Host "Verifying SDK installation..." -ForegroundColor Yellow
if (Test-Path "$sdk_dir\high_level_api\include\IAgoraRtcEngine.h") {
    Write-Host "✓ SDK header files verification successful" -ForegroundColor Green
} else {
    Write-Host "✗ SDK header files verification failed" -ForegroundColor Red
    exit 1
}

if (Test-Path "$sdk_dir\x64\agora_rtc_sdk.dll.lib") {
    Write-Host "✓ SDK library files verification successful" -ForegroundColor Green
} else {
    Write-Host "✗ SDK library files verification failed" -ForegroundColor Red
    exit 1
}

Write-Host "========================================" -ForegroundColor Green
Write-Host "Agora SDK installation completed!" -ForegroundColor Green
Write-Host "SDK path: $((Get-Location).Path)\$sdk_dir" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Green 