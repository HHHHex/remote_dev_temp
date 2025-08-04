# ThousChannel 自动化打包脚本
# 版本: 1.0.1 (修复路径问题)

param(
    [Parameter(Mandatory=$false)]
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",
    
    [Parameter(Mandatory=$false)]
    [ValidateSet("x64")]
    [string]$Platform = "x64",
    
    [Parameter(Mandatory=$false)]
    [string]$OutputPath = ".\packages",
    
    [Parameter(Mandatory=$false)]
    [ValidateSet("Portable", "Installer", "All")]
    [string]$PackageType = "All",
    
    [Parameter(Mandatory=$false)]
    [string]$Version = "1.0.0",
    
    [Parameter(Mandatory=$false)]
    [string]$Company = "Your Company",
    
    [Parameter(Mandatory=$false)]
    [switch]$Clean,
    
    [Parameter(Mandatory=$false)]
    [switch]$SkipBuild
)

# 设置控制台颜色函数
function Write-ColorOutput {
    param([string]$Message, [string]$Color = "White")
    Write-Host $Message -ForegroundColor $Color
}

# 检查必要工具
function Test-Prerequisites {
    Write-ColorOutput "检查打包环境..." "Yellow"
    
    # 检查MSBuild
    $msbuildPath = $null
    $vsInstallations = @(
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
    )
    
    foreach ($path in $vsInstallations) {
        if (Test-Path $path) {
            $msbuildPath = $path
            break
        }
    }
    
    if (-not $msbuildPath) {
        Write-ColorOutput "错误: 找不到MSBuild，请确保已安装Visual Studio 2022" "Red"
        exit 1
    }
    Write-ColorOutput "找到MSBuild: $msbuildPath" "Green"

    # 检查Inno Setup
    $innoSetupPath = "${env:ProgramFiles(x86)}\Inno Setup 6\iscc.exe"
    if (-not (Test-Path $innoSetupPath)) {
        Write-ColorOutput "警告: 找不到Inno Setup编译器 (iscc.exe)，将无法创建安装包。" "Yellow"
        Write-ColorOutput "请访问 https://jrsoftware.org/isinfo.php 下载并安装。" "Yellow"
        $innoSetupPath = $null
    } else {
        Write-ColorOutput "找到Inno Setup: $innoSetupPath" "Green"
    }

    return $msbuildPath, $innoSetupPath
}

# 清理输出目录
function Clear-OutputDirectory {
    param([string]$Path)
    if (Test-Path $Path) {
        Write-ColorOutput "清理输出目录: $Path" "Yellow"
        Remove-Item $Path -Recurse -Force
    }
    New-Item -ItemType Directory -Path $Path -Force | Out-Null
}

# 编译项目
function Build-Project {
    param([string]$MsbuildPath, [string]$Configuration, [string]$Platform)
    
    Write-ColorOutput "开始编译项目..." "Yellow"
    Write-ColorOutput "配置: $Configuration, 平台: $Platform" "Cyan"
    
    $buildArgs = @(
        "ThousChannel.sln",
        "/p:Configuration=$Configuration",
        "/p:Platform=$Platform",
        "/verbosity:minimal"
    )
    
    $process = Start-Process -FilePath $MsbuildPath -ArgumentList $buildArgs -Wait -PassThru -NoNewWindow
    
    if ($process.ExitCode -ne 0) {
        Write-ColorOutput "编译失败！退出代码: $($process.ExitCode)" "Red"
        exit 1
    }
    
    Write-ColorOutput "编译成功！" "Green"
}

# 收集依赖文件
function Copy-Dependencies {
    param([string]$SourceDir, [string]$TargetDir)
    
    Write-ColorOutput "复制依赖文件..." "Yellow"
    
    # 创建目录结构
    $dirs = @("", "sdk", "resources")
    foreach ($dir in $dirs) {
        $targetPath = Join-Path $TargetDir $dir
        if (-not (Test-Path $targetPath)) {
            New-Item -ItemType Directory -Path $targetPath -Force | Out-Null
        }
    }
    
    # 复制主程序
    $exePath = Join-Path $SourceDir "ThousChannel.exe"
    if (Test-Path $exePath) {
        Copy-Item $exePath $TargetDir
        Write-ColorOutput "复制主程序: ThousChannel.exe" "Green"
    } else {
        Write-ColorOutput "警告: 找不到主程序文件" "Yellow"
    }
    
    # 复制SDK文件
    $sdkSource = "sdk\x86_64"
    $sdkTarget = Join-Path $TargetDir "sdk"
    if (Test-Path $sdkSource) {
        Copy-Item "$sdkSource\*.dll" $sdkTarget -ErrorAction SilentlyContinue
        Copy-Item "$sdkSource\*.lib" $sdkTarget -ErrorAction SilentlyContinue
        Write-ColorOutput "复制SDK文件到: $sdkTarget" "Green"
    }
    
    # 复制资源文件
    $resSource = "resources"
    $resTarget = Join-Path $TargetDir "resources"
    if (Test-Path $resSource) {
        Copy-Item "$resSource\*" $resTarget -Recurse -ErrorAction SilentlyContinue
        Write-ColorOutput "复制资源文件到: $resTarget" "Green"
    }
}

# 创建便携版
function Create-PortablePackage {
    param([string]$SourceDir, [string]$OutputPath, [string]$Version)
    
    Write-ColorOutput "创建便携版..." "Yellow"
    
    $portableDir = Join-Path $OutputPath "ThousChannel-Portable-v$Version"
    Clear-OutputDirectory $portableDir
    
    # 复制文件
    Copy-Dependencies $SourceDir $portableDir
    
    # 创建启动脚本
    $startupScriptContent = @'
@echo off
chcp 65001 >nul
title ThousChannel 启动器

echo ========================================
echo           ThousChannel v{0}
echo ========================================
echo.

REM 检查主程序是否存在
if not exist "ThousChannel.exe" (
    echo [错误] 找不到主程序文件 ThousChannel.exe
    pause
    exit /b 1
)

echo [信息] 正在启动 ThousChannel...
start "" "ThousChannel.exe"
echo [信息] 程序已启动
pause
'@ -f $Version
    
    $startupScriptContent | Out-File (Join-Path $portableDir "启动ThousChannel.bat") -Encoding oem
    
    # 创建README文件
    $readmeContent = @'
# ThousChannel 便携版 v{0}

## 使用说明
1. 双击 `启动ThousChannel.bat` 启动程序
2. 程序会自动检查依赖文件并启动

## 系统要求
- Windows 10 或更高版本
- x64 架构

## 文件说明
- `ThousChannel.exe` - 主程序
- `sdk/` - SDK依赖文件
- `resources/` - 资源文件
- `启动ThousChannel.bat` - 启动脚本

## 注意事项
- 请勿删除任何文件
- 建议将整个文件夹放在固定位置使用
'@ -f $Version
    
    $readmeContent | Out-File (Join-Path $portableDir "README.txt") -Encoding UTF8BOM
    
    # 创建ZIP包
    $zipPath = Join-Path $OutputPath "ThousChannel-Portable-v$Version.zip"
    if (Test-Path $zipPath) {
        Remove-Item $zipPath -Force
    }
    
    Write-ColorOutput "创建ZIP包..." "Yellow"
    Compress-Archive -Path "$portableDir\*" -DestinationPath $zipPath -CompressionLevel Optimal
    
    Write-ColorOutput "便携版创建完成！" "Green"
    Write-ColorOutput "目录: $portableDir" "Cyan"
    Write-ColorOutput "ZIP包: $zipPath" "Cyan"
}

# 创建安装程序
function Create-InstallerPackage {
    param([string]$SourceDir, [string]$OutputPath, [string]$Version, [string]$Company, [string]$InnoSetupPath)

    if (-not $InnoSetupPath) {
        Write-ColorOutput "跳过创建安装程序，因为找不到Inno Setup编译器。" "Yellow"
        return
    }

    Write-ColorOutput "创建安装程序..." "Yellow"

    $appName = "ThousChannel"
    $installerDir = Join-Path $OutputPath "ThousChannel-Installer-v$Version"
    Clear-OutputDirectory $installerDir

    # 复制文件到临时目录
    Copy-Dependencies $SourceDir $installerDir

    # Inno Setup 脚本模板
    $issScriptContent = @'
[Setup]
AppName={0}
AppVersion={1}
AppPublisher={2}
DefaultDirName={{autopf}}\{0}
DefaultGroupName={0}
UninstallDisplayIcon={{app}}\{0}.exe
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
OutputBaseFilename=ThousChannel-Setup-v{1}
OutputDir={3}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "chinesesimplified"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"

[Tasks]
Name: "desktopicon"; Description: "{{cm:CreateDesktopIcon}}"; GroupDescription: "{{cm:AdditionalIcons}}"; Flags: unchecked

[Files]
Source: "{4}\*"; DestDir: "{{app}}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{{group}}\{0}"; Filename: "{{app}}\{0}.exe"
Name: "{{group}}\{{cm:UninstallProgram,{0}}}"; Filename: "{{uninstallexe}}"
Name: "{{autodesktop}}\{0}"; Filename: "{{app}}\{0}.exe"; Tasks: desktopicon

[Run]
Filename: "{{app}}\{0}.exe"; Description: "{{cm:LaunchProgram,{0}}}"; Flags: nowait postinstall skipifsilent
'@ -f $appName, $Version, $Company, $OutputPath, $installerDir

    $issPath = Join-Path $OutputPath "ThousChannel.iss"
    $issScriptContent | Out-File -FilePath $issPath -Encoding UTF8BOM

    Write-ColorOutput "生成Inno Setup脚本: $issPath" "Green"

    # 运行Inno Setup编译器
    $process = Start-Process -FilePath $InnoSetupPath -ArgumentList "/q `"$issPath`"" -Wait -PassThru -NoNewWindow
    if ($process.ExitCode -ne 0) {
        Write-ColorOutput "创建安装程序失败！退出代码: $($process.ExitCode)" "Red"
    } else {
        Write-ColorOutput "安装程序创建成功！" "Green"
        Write-ColorOutput "输出文件: $(Join-Path $OutputPath "ThousChannel-Setup-v$Version.exe")" "Cyan"
    }

    # 清理临时文件
    Remove-Item $issPath -Force
    Remove-Item $installerDir -Recurse -Force
}

# 主函数
function Main {
    Write-ColorOutput "========================================" "Cyan"
    Write-ColorOutput "      ThousChannel 自动化打包工具" "Cyan"
    Write-ColorOutput "========================================" "Cyan"
    Write-ColorOutput ""
    
    # 检查参数
    Write-ColorOutput "打包参数:" "Yellow"
    Write-ColorOutput "  配置: $Configuration" "White"
    Write-ColorOutput "  平台: $Platform" "White"
    Write-ColorOutput "  输出路径: $OutputPath" "White"
    Write-ColorOutput "  打包类型: $PackageType" "White"
    Write-ColorOutput "  版本: $Version" "White"
    Write-ColorOutput ""
    
    # 检查必要工具
    $msbuildPath, $innoSetupPath = Test-Prerequisites
    
    # 清理输出目录
    if ($Clean) {
        Clear-OutputDirectory $OutputPath
    }
    
    # 编译项目
    if (-not $SkipBuild) {
        Build-Project $msbuildPath $Configuration $Platform
    }
    
    # 确定源目录 - 修复版本：支持多个可能的路径
    $possiblePaths = @(
        "x64\$Configuration",
        "project\x64\$Configuration",
        "build\x64\$Configuration"
    )
    
    $sourceDir = $null
    foreach ($path in $possiblePaths) {
        if (Test-Path $path) {
            $sourceDir = $path
            Write-ColorOutput "找到编译输出目录: $sourceDir" "Green"
            break
        }
    }
    
    if (-not $sourceDir) {
        Write-ColorOutput "错误: 找不到编译输出目录" "Red"
        Write-ColorOutput "尝试过的路径:" "Red"
        foreach ($path in $possiblePaths) {
            Write-ColorOutput "  $path" "Red"
        }
        exit 1
    }
    
    # 创建包
    switch ($PackageType) {
        "Portable" {
            Create-PortablePackage $sourceDir $OutputPath $Version
        }
        "Installer" {
            Create-InstallerPackage $sourceDir $OutputPath $Version $Company $innoSetupPath
        }
        "All" {
            Create-PortablePackage $sourceDir $OutputPath $Version
            Create-InstallerPackage $sourceDir $OutputPath $Version $Company $innoSetupPath
        }
    }
    
    Write-ColorOutput ""
    Write-ColorOutput "========================================" "Cyan"
    Write-ColorOutput "      打包完成！" "Green"
    Write-ColorOutput "========================================" "Cyan"
    Write-ColorOutput "输出目录: $OutputPath" "White"
}

# 执行主函数
try {
    Main
} catch {
    Write-ColorOutput "打包过程中发生错误: $($_.Exception.Message)" "Red"
    exit 1
}