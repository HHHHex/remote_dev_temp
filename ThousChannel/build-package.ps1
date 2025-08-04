# ThousChannel Build Package Script
# Version: 1.0.3 (Fix encoding issues)

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

# Console color output function
function Write-ColorOutput {
    param([string]$Message, [string]$Color = "White")
    Write-Host $Message -ForegroundColor $Color
}

# Check prerequisites
function Test-Prerequisites {
    Write-ColorOutput "Checking build environment..." "Yellow"
    
    # Check MSBuild
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
        Write-ColorOutput "Error: MSBuild not found. Please ensure Visual Studio 2022 is installed." "Red"
        exit 1
    }
    Write-ColorOutput "Found MSBuild: $msbuildPath" "Green"

    # Check Inno Setup
    $innoSetupPath = "${env:ProgramFiles(x86)}\Inno Setup 6\iscc.exe"
    if (-not (Test-Path $innoSetupPath)) {
        Write-ColorOutput "Warning: Inno Setup compiler (iscc.exe) not found. Installer creation will be skipped." "Yellow"
        Write-ColorOutput "Please visit https://jrsoftware.org/isinfo.php to download and install." "Yellow"
        $innoSetupPath = $null
    } else {
        Write-ColorOutput "Found Inno Setup: $innoSetupPath" "Green"
    }

    return $msbuildPath, $innoSetupPath
}

# Clear output directory
function Clear-OutputDirectory {
    param([string]$Path)
    if (Test-Path $Path) {
        Write-ColorOutput "Clearing output directory: $Path" "Yellow"
        Remove-Item $Path -Recurse -Force
    }
    New-Item -ItemType Directory -Path $Path -Force | Out-Null
}

# Build project
function Build-Project {
    param([string]$MsbuildPath, [string]$Configuration, [string]$Platform)
    
    Write-ColorOutput "Starting project build..." "Yellow"
    Write-ColorOutput "Configuration: $Configuration, Platform: $Platform" "Cyan"
    
    $buildArgs = @(
        "ThousChannel.sln",
        "/p:Configuration=$Configuration",
        "/p:Platform=$Platform",
        "/verbosity:minimal"
    )
    
    $process = Start-Process -FilePath $MsbuildPath -ArgumentList $buildArgs -Wait -PassThru -NoNewWindow
    
    if ($process.ExitCode -ne 0) {
        Write-ColorOutput "Build failed! Exit code: $($process.ExitCode)" "Red"
        exit 1
    }
    
    Write-ColorOutput "Build successful!" "Green"
}

# Copy dependencies
function Copy-Dependencies {
    param([string]$SourceDir, [string]$TargetDir)
    
    Write-ColorOutput "Copying dependencies..." "Yellow"
    
    # Create directory structure
    $dirs = @("", "sdk", "resources")
    foreach ($dir in $dirs) {
        $targetPath = Join-Path $TargetDir $dir
        if (-not (Test-Path $targetPath)) {
            New-Item -ItemType Directory -Path $targetPath -Force | Out-Null
        }
    }
    
    # Copy main executable
    $exePath = Join-Path $SourceDir "ThousChannel.exe"
    if (Test-Path $exePath) {
        Copy-Item $exePath $TargetDir
        Write-ColorOutput "Copied main executable: ThousChannel.exe" "Green"
    } else {
        Write-ColorOutput "Warning: Main executable not found" "Yellow"
    }
    
    # Copy SDK files
    $sdkSource = "sdk\x86_64"
    $sdkTarget = Join-Path $TargetDir "sdk"
    if (Test-Path $sdkSource) {
        Copy-Item "$sdkSource\*.dll" $sdkTarget -ErrorAction SilentlyContinue
        Copy-Item "$sdkSource\*.lib" $sdkTarget -ErrorAction SilentlyContinue
        Write-ColorOutput "Copied SDK files to: $sdkTarget" "Green"
    }
    
    # Copy resource files
    $resSource = "resources"
    $resTarget = Join-Path $TargetDir "resources"
    if (Test-Path $resSource) {
        Copy-Item "$resSource\*" $resTarget -Recurse -ErrorAction SilentlyContinue
        Write-ColorOutput "Copied resource files to: $resTarget" "Green"
    }
}

# Create portable package
function Create-PortablePackage {
    param([string]$SourceDir, [string]$OutputPath, [string]$Version)
    
    Write-ColorOutput "Creating portable package..." "Yellow"
    
    $portableDir = Join-Path $OutputPath "ThousChannel-Portable-v$Version"
    Clear-OutputDirectory $portableDir
    
    # Copy files
    Copy-Dependencies $SourceDir $portableDir
    
    # Create startup script
    $startupScript = @"
@echo off
chcp 65001 >nul
title ThousChannel Launcher

echo ========================================
echo           ThousChannel v$Version
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
"@
    
    $startupScript | Out-File (Join-Path $portableDir "StartThousChannel.bat") -Encoding ASCII
    
    # Create README file
    $readmeContent = @"
# ThousChannel Portable v$Version

## Usage Instructions
1. Double-click `StartThousChannel.bat` to launch the application
2. The application will automatically check dependencies and start

## System Requirements
- Windows 10 or higher
- x64 architecture

## File Description
- `ThousChannel.exe` - Main executable
- `sdk/` - SDK dependency files
- `resources/` - Resource files
- `StartThousChannel.bat` - Startup script

## Notes
- Do not delete any files
- It is recommended to place the entire folder in a fixed location
"@
    
    $readmeContent | Out-File (Join-Path $portableDir "README.txt") -Encoding ASCII
    
    # Create ZIP package
    $zipPath = Join-Path $OutputPath "ThousChannel-Portable-v$Version.zip"
    if (Test-Path $zipPath) {
        Remove-Item $zipPath -Force
    }
    
    Write-ColorOutput "Creating ZIP package..." "Yellow"
    Compress-Archive -Path "$portableDir\*" -DestinationPath $zipPath -CompressionLevel Optimal
    
    Write-ColorOutput "Portable package created successfully!" "Green"
    Write-ColorOutput "Directory: $portableDir" "Cyan"
    Write-ColorOutput "ZIP package: $zipPath" "Cyan"
}

# Create installer package
function Create-InstallerPackage {
    param([string]$SourceDir, [string]$OutputPath, [string]$Version, [string]$Company, [string]$InnoSetupPath)

    if (-not $InnoSetupPath) {
        Write-ColorOutput "Skipping installer creation - Inno Setup compiler not found." "Yellow"
        return
    }

    Write-ColorOutput "Creating installer package..." "Yellow"

    $appName = "ThousChannel"
    $installerDir = Join-Path $OutputPath "ThousChannel-Installer-v$Version"
    Clear-OutputDirectory $installerDir

    # Copy files to temporary directory
    Copy-Dependencies $SourceDir $installerDir

    # Inno Setup script template
    $issScriptContent = @"
[Setup]
AppName=$appName
AppVersion=$Version
AppPublisher=$Company
DefaultDirName={autopf}\$appName
DefaultGroupName=$appName
UninstallDisplayIcon={app}\$appName.exe
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
OutputBaseFilename=ThousChannel-Setup-v$Version
OutputDir=$OutputPath

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "chinesesimplified"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "$installerDir\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\$appName"; Filename: "{app}\$appName.exe"
Name: "{group}\{cm:UninstallProgram,$appName}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\$appName"; Filename: "{app}\$appName.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\$appName.exe"; Description: "{cm:LaunchProgram,$appName}"; Flags: nowait postinstall skipifsilent
"@
    
    $issPath = Join-Path $OutputPath "ThousChannel.iss"
    $issScriptContent | Out-File -FilePath $issPath -Encoding ASCII
    
    Write-ColorOutput "Generated Inno Setup script: $issPath" "Green"
    
    # Run Inno Setup compiler
    $process = Start-Process -FilePath $InnoSetupPath -ArgumentList "/q `"$issPath`"" -Wait -PassThru -NoNewWindow
    if ($process.ExitCode -ne 0) {
        Write-ColorOutput "Installer creation failed! Exit code: $($process.ExitCode)" "Red"
    } else {
        Write-ColorOutput "Installer created successfully!" "Green"
        Write-ColorOutput "Output file: $(Join-Path $OutputPath "ThousChannel-Setup-v$Version.exe")" "Cyan"
    }

    # Clean up temporary files
    Remove-Item $issPath -Force
    Remove-Item $installerDir -Recurse -Force
}

# Main function
function Main {
    Write-ColorOutput "========================================" "Cyan"
    Write-ColorOutput "      ThousChannel Build Package Tool" "Cyan"
    Write-ColorOutput "========================================" "Cyan"
    Write-ColorOutput ""
    
    # Check parameters
    Write-ColorOutput "Package parameters:" "Yellow"
    Write-ColorOutput "  Configuration: $Configuration" "White"
    Write-ColorOutput "  Platform: $Platform" "White"
    Write-ColorOutput "  Output path: $OutputPath" "White"
    Write-ColorOutput "  Package type: $PackageType" "White"
    Write-ColorOutput "  Version: $Version" "White"
    Write-ColorOutput ""
    
    # Check prerequisites
    $msbuildPath, $innoSetupPath = Test-Prerequisites
    
    # Clear output directory
    if ($Clean) {
        Clear-OutputDirectory $OutputPath
    }
    
    # Build project
    if (-not $SkipBuild) {
        Build-Project $msbuildPath $Configuration $Platform
    }
    
    # Determine source directory - support multiple possible paths
    $possiblePaths = @(
        "x64\$Configuration",
        "project\x64\$Configuration",
        "build\x64\$Configuration"
    )
    
    $sourceDir = $null
    foreach ($path in $possiblePaths) {
        if (Test-Path $path) {
            $sourceDir = $path
            Write-ColorOutput "Found build output directory: $sourceDir" "Green"
            break
        }
    }
    
    if (-not $sourceDir) {
        Write-ColorOutput "Error: Build output directory not found" "Red"
        Write-ColorOutput "Tried paths:" "Red"
        foreach ($path in $possiblePaths) {
            Write-ColorOutput "  $path" "Red"
        }
        exit 1
    }
    
    # Create packages
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
    Write-ColorOutput "      Package creation completed!" "Green"
    Write-ColorOutput "========================================" "Cyan"
    Write-ColorOutput "Output directory: $OutputPath" "White"
}

# Execute main function
try {
    Main
} catch {
    Write-ColorOutput "Error occurred during packaging: $($_.Exception.Message)" "Red"
    exit 1
} 