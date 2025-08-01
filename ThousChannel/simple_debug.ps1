Write-Host "=== ThousChannel 简单诊断 ===" -ForegroundColor Green

# 检查文件
$exePath = "project\x64\Debug\ThousChannel.exe"
if (Test-Path $exePath) {
    Write-Host "[OK] 可执行文件存在" -ForegroundColor Green
    $fileInfo = Get-Item $exePath
    Write-Host "文件大小: $($fileInfo.Length) bytes"
} else {
    Write-Host "[ERROR] 可执行文件不存在" -ForegroundColor Red
    exit 1
}

# 检查DLL
$dlls = @("agora_rtc_sdk.dll", "glfw3.dll")
foreach ($dll in $dlls) {
    $dllPath = "project\x64\Debug\$dll"
    if (Test-Path $dllPath) {
        Write-Host "[OK] 找到 $dll" -ForegroundColor Green
    } else {
        Write-Host "[WARNING] 缺少 $dll" -ForegroundColor Yellow
    }
}

# 启动程序
Write-Host "启动程序..." -ForegroundColor Cyan
$process = Start-Process -FilePath $exePath -PassThru -Wait
Write-Host "退出代码: $($process.ExitCode)" -ForegroundColor Yellow

Write-Host "诊断完成" -ForegroundColor Green 