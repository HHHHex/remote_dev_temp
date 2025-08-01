Write-Host "=== ThousChannel 详细诊断脚本 ===" -ForegroundColor Green
Write-Host ""

# 检查文件存在性
$exePath = "project\x64\Debug\ThousChannel.exe"
if (Test-Path $exePath) {
    Write-Host "[OK] 可执行文件存在: $exePath" -ForegroundColor Green
    $fileInfo = Get-Item $exePath
    Write-Host "文件大小: $($fileInfo.Length) bytes"
    Write-Host "修改时间: $($fileInfo.LastWriteTime)"
} else {
    Write-Host "[ERROR] 可执行文件不存在: $exePath" -ForegroundColor Red
    exit 1
}

Write-Host ""

# 检查DLL文件
$dllPath = "project\x64\Debug"
$requiredDlls = @("agora_rtc_sdk.dll", "glfw3.dll")

foreach ($dll in $requiredDlls) {
    $fullPath = Join-Path $dllPath $dll
    if (Test-Path $fullPath) {
        Write-Host "[OK] 找到DLL: $dll" -ForegroundColor Green
        $dllInfo = Get-Item $fullPath
        Write-Host "  大小: $($dllInfo.Length) bytes"
    } else {
        Write-Host "[WARNING] 缺少DLL: $dll" -ForegroundColor Yellow
    }
}

Write-Host ""

# 检查Visual C++ 运行时
Write-Host "检查 Visual C++ 运行时库..." -ForegroundColor Cyan
try {
    $vcRuntime = Get-ItemProperty "HKLM:\SOFTWARE\Classes\Installer\Dependencies\Microsoft.VS.VC_RuntimeMinimumVSU_amd64,v14" -ErrorAction SilentlyContinue
    if ($vcRuntime) {
        Write-Host "[OK] Visual C++ 2022 x64 运行时已安装: $($vcRuntime.Version)" -ForegroundColor Green
    } else {
        Write-Host "[WARNING] Visual C++ 2022 x64 运行时可能未安装" -ForegroundColor Yellow
    }
}
catch {
    Write-Host "[ERROR] 无法检查运行时库状态" -ForegroundColor Red
}

Write-Host ""

# 尝试使用Process Monitor方式启动
Write-Host "尝试启动应用程序并捕获详细错误..." -ForegroundColor Cyan
try {
    $process = Start-Process -FilePath $exePath -WorkingDirectory (Split-Path $exePath) -PassThru -Wait
    Write-Host "应用程序退出代码: $($process.ExitCode)"
    
    if ($process.ExitCode -eq -1073741701) {
        Write-Host ""
        Write-Host "错误分析: 0xC000007B - 应用程序无法正常启动" -ForegroundColor Red
        Write-Host "可能的原因:" -ForegroundColor Yellow
        Write-Host "1. 32位/64位架构不匹配"
        Write-Host "2. 缺少必要的DLL依赖"
        Write-Host "3. DLL版本不兼容"
        Write-Host "4. 运行时库版本问题"
    }
}
catch {
    Write-Host "[ERROR] 无法启动应用程序: $($_.Exception.Message)" -ForegroundColor Red
}

Write-Host ""
Write-Host "诊断完成。" -ForegroundColor Green
Write-Host "请将以上信息提供给开发者进行进一步分析。" -ForegroundColor Cyan 