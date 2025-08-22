#!/usr/bin/env python3
"""
Agora SDK Update Script
跨平台SDK更新脚本，支持Windows、macOS和Linux
"""

import os
import sys
import shutil
import zipfile
import argparse
import urllib.request
import urllib.error
from datetime import datetime
from pathlib import Path

class Colors:
    """终端颜色代码"""
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    CYAN = '\033[96m'
    WHITE = '\033[97m'
    RESET = '\033[0m'

def print_colored(text, color):
    """打印彩色文本"""
    print(f"{color}{text}{Colors.RESET}")

def print_header():
    """打印脚本头部"""
    print_colored("=" * 40, Colors.GREEN)
    print_colored("Agora SDK Update Script", Colors.GREEN)
    print_colored("=" * 40, Colors.GREEN)

def print_help():
    """显示帮助信息"""
    print("Usage: python update_sdk.py [options]")
    print()
    print("Options:")
    print("  -u, --url URL           SDK下载URL")
    print("  -v, --version VERSION   SDK版本 (默认: 3.0)")
    print("  -b, --backup            创建备份")
    print("  -f, --force             强制更新，不询问确认")
    print("  -c, --clean-only        只清理临时文件")
    print("  -h, --help              显示此帮助信息")
    print()
    print("Examples:")
    print("  python update_sdk.py")
    print("  python update_sdk.py -u 'http://example.com/sdk.zip' -b")
    print("  python update_sdk.py -c")

def clean_temp_files():
    """清理临时文件"""
    print_colored("Cleaning temporary files...", Colors.YELLOW)
    
    temp_items = ['new_sdk.zip', 'temp_sdk']
    for item in temp_items:
        if os.path.exists(item):
            try:
                if os.path.isfile(item):
                    os.remove(item)
                else:
                    shutil.rmtree(item)
                print_colored(f"  ✓ Removed: {item}", Colors.GREEN)
            except Exception as e:
                print_colored(f"  ⚠ Warning: Could not remove {item}: {e}", Colors.YELLOW)
    
    # 清理可能的解压目录
    for item in os.listdir('.'):
        if item.startswith('Shengwang_Native_SDK_for_Windows'):
            try:
                shutil.rmtree(item)
                print_colored(f"  ✓ Removed: {item}", Colors.GREEN)
            except Exception as e:
                print_colored(f"  ⚠ Warning: Could not remove {item}: {e}", Colors.YELLOW)

def get_file_size_mb(file_path):
    """获取文件大小（MB）"""
    size_bytes = os.path.getsize(file_path)
    return round(size_bytes / (1024 * 1024), 2)

def download_file(url, filename):
    """下载文件"""
    print_colored(f"Downloading SDK from: {url}", Colors.YELLOW)
    
    try:
        def progress_hook(block_num, block_size, total_size):
            if total_size > 0:
                percent = min(100, (block_num * block_size * 100) // total_size)
                sys.stdout.write(f"\r  Progress: {percent}%")
                sys.stdout.flush()
        
        urllib.request.urlretrieve(url, filename, progress_hook)
        print()  # 换行
        print_colored("  ✓ Download completed", Colors.GREEN)
        
        # 显示文件大小
        size_mb = get_file_size_mb(filename)
        print_colored(f"  File size: {size_mb} MB", Colors.WHITE)
        
    except urllib.error.URLError as e:
        print_colored(f"  ✗ Download failed: {e}", Colors.RED)
        return False
    except Exception as e:
        print_colored(f"  ✗ Download failed: {e}", Colors.RED)
        return False
    
    return True

def extract_zip(zip_path, extract_to):
    """解压ZIP文件"""
    print_colored("Extracting SDK...", Colors.YELLOW)
    
    try:
        # 创建临时目录
        os.makedirs(extract_to, exist_ok=True)
        
        with zipfile.ZipFile(zip_path, 'r') as zip_ref:
            zip_ref.extractall(extract_to)
        
        print_colored("  ✓ Extraction completed", Colors.GREEN)
        return True
        
    except zipfile.BadZipFile:
        print_colored("  ✗ Invalid ZIP file", Colors.RED)
        return False
    except Exception as e:
        print_colored(f"  ✗ Extraction failed: {e}", Colors.RED)
        return False

def find_sdk_directory(temp_dir):
    """查找SDK目录"""
    if not os.path.exists(temp_dir):
        return None
    
    # 查找包含SDK的目录
    for item in os.listdir(temp_dir):
        item_path = os.path.join(temp_dir, item)
        if os.path.isdir(item_path) and 'SDK' in item.upper():
            return item
    
    return None

def verify_sdk_installation(sdk_dir):
    """验证SDK安装"""
    print_colored("Verifying SDK installation...", Colors.YELLOW)
    
    required_files = [
        "high_level_api/include/rte_cpp.h",
        "high_level_api/include/rte_base/rte_cpp_rte.h",
        "high_level_api/include/rte_base/rte_cpp_channel.h",
        "high_level_api/include/rte_base/rte_cpp_user.h",
        "x86_64/agora_rtc_sdk.dll",
        "x86_64/agora_rtc_sdk.dll.lib"
    ]
    
    all_valid = True
    for file_path in required_files:
        full_path = os.path.join(sdk_dir, file_path)
        if os.path.exists(full_path):
            print_colored(f"  ✓ {file_path}", Colors.GREEN)
        else:
            print_colored(f"  ✗ {file_path} (missing)", Colors.RED)
            all_valid = False
    
    return all_valid

def show_sdk_info(sdk_dir):
    """显示SDK信息"""
    print_colored("Current SDK Information:", Colors.CYAN)
    
    if os.path.exists(sdk_dir):
        total_size = 0
        file_count = 0
        dir_count = 0
        
        for root, dirs, files in os.walk(sdk_dir):
            dir_count += len(dirs)
            for file in files:
                file_count += 1
                file_path = os.path.join(root, file)
                total_size += os.path.getsize(file_path)
        
        size_mb = round(total_size / (1024 * 1024), 2)
        print_colored(f"  Size: {size_mb} MB", Colors.WHITE)
        print_colored(f"  Files: {file_count}", Colors.WHITE)
        print_colored(f"  Directories: {dir_count}", Colors.WHITE)
    else:
        print_colored("  No SDK directory found", Colors.RED)

def main():
    """主函数"""
    parser = argparse.ArgumentParser(description='Agora SDK Update Script')
    parser.add_argument('-u', '--url', help='SDK download URL')
    parser.add_argument('-v', '--version', default='3.0', help='SDK version (default: 3.0)')
    parser.add_argument('-b', '--backup', action='store_true', help='Create backup before update')
    parser.add_argument('-f', '--force', action='store_true', help='Force update without confirmation')
    parser.add_argument('-c', '--clean-only', action='store_true', help='Only clean temporary files')
    
    args = parser.parse_args()
    
    print_header()
    
    # 配置变量
    sdk_dir = "sdk"
    backup_dir = f"sdk_backup_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
    temp_dir = "temp_sdk"
    sdk_zip = "new_sdk.zip"
    
    # 如果只是清理，执行清理后退出
    if args.clean_only:
        clean_temp_files()
        print_colored("Clean operation completed!", Colors.GREEN)
        return 0
    
    # 显示当前SDK信息
    show_sdk_info(sdk_dir)
    
    # 检查是否已有SDK目录
    if os.path.exists(sdk_dir):
        if not args.force:
            response = input("SDK directory already exists. Do you want to continue? (y/N): ")
            if response.lower() not in ['y', 'yes']:
                print_colored("Operation cancelled by user.", Colors.YELLOW)
                return 0
        
        # 创建备份
        if args.backup:
            print_colored("Creating backup of current SDK...", Colors.YELLOW)
            try:
                shutil.copytree(sdk_dir, backup_dir)
                print_colored(f"  ✓ Backup created: {backup_dir}", Colors.GREEN)
            except Exception as e:
                print_colored(f"  ✗ Failed to create backup: {e}", Colors.RED)
                if not args.force:
                    response = input("Continue without backup? (y/N): ")
                    if response.lower() not in ['y', 'yes']:
                        return 1
    
    # 清理临时文件
    clean_temp_files()
    
    # 确定下载URL
    if not args.url:
        # 使用默认URL
        args.url = f"https://download.agora.io/sdk/release/Agora_Native_SDK_for_Windows_v{args.version}_FULL.zip"
        print_colored(f"Using default SDK URL: {args.url}", Colors.CYAN)
    else:
        print_colored(f"Using custom SDK URL: {args.url}", Colors.CYAN)
    
    # 下载SDK
    if not download_file(args.url, sdk_zip):
        return 1
    
    # 验证下载文件
    if not os.path.exists(sdk_zip):
        print_colored("  ✗ Downloaded file not found", Colors.RED)
        return 1
    
    # 解压SDK
    if not extract_zip(sdk_zip, temp_dir):
        clean_temp_files()
        return 1
    
    # 查找解压后的SDK目录
    sdk_source_dir = find_sdk_directory(temp_dir)
    if not sdk_source_dir:
        print_colored("  ✗ No SDK directory found in extracted files", Colors.RED)
        clean_temp_files()
        return 1
    
    sdk_source_path = os.path.join(temp_dir, sdk_source_dir)
    print_colored(f"  Found SDK directory: {sdk_source_dir}", Colors.WHITE)
    
    # 删除现有SDK目录
    if os.path.exists(sdk_dir):
        print_colored("Removing existing SDK directory...", Colors.YELLOW)
        shutil.rmtree(sdk_dir)
        print_colored("  ✓ Existing SDK removed", Colors.GREEN)
    
    # 移动新SDK到目标位置
    print_colored("Installing new SDK...", Colors.YELLOW)
    try:
        # 查找SDK子目录
        sdk_sub_dir = os.path.join(sdk_source_path, "sdk")
        if os.path.exists(sdk_sub_dir):
            shutil.move(sdk_sub_dir, sdk_dir)
        else:
            # 如果没有sdk子目录，直接移动整个目录
            shutil.move(sdk_source_path, sdk_dir)
        print_colored("  ✓ SDK installation completed", Colors.GREEN)
    except Exception as e:
        print_colored(f"  ✗ SDK installation failed: {e}", Colors.RED)
        clean_temp_files()
        return 1
    
    # 验证安装
    if verify_sdk_installation(sdk_dir):
        print_colored("  ✓ SDK verification successful", Colors.GREEN)
    else:
        print_colored("  ✗ SDK verification failed", Colors.RED)
        return 1
    
    # 清理临时文件
    clean_temp_files()
    
    # 显示最终信息
    print()
    print_colored("=" * 40, Colors.GREEN)
    print_colored("SDK Update Completed Successfully!", Colors.GREEN)
    print_colored("=" * 40, Colors.GREEN)
    print_colored(f"SDK Path: {os.path.abspath(sdk_dir)}", Colors.CYAN)
    print_colored(f"Version: {args.version}", Colors.CYAN)
    
    if args.backup and os.path.exists(backup_dir):
        print_colored(f"Backup: {os.path.abspath(backup_dir)}", Colors.CYAN)
    
    show_sdk_info(sdk_dir)
    
    return 0

if __name__ == "__main__":
    try:
        exit_code = main()
        sys.exit(exit_code)
    except KeyboardInterrupt:
        print_colored("\nOperation cancelled by user.", Colors.YELLOW)
        sys.exit(1)
    except Exception as e:
        print_colored(f"An error occurred: {e}", Colors.RED)
        sys.exit(1)
