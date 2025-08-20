#!/usr/bin/env python3
"""
Logger Migration Script
Migrates old CLogger calls to modern ModernLogger calls
"""

import os
import re
import sys
from pathlib import Path

def migrate_file(file_path):
    """Migrate a single file from old logger to new logger"""
    print(f"Migrating: {file_path}")
    
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    original_content = content
    
    # Replace old includes
    content = re.sub(r'#include\s+"Logger\.h"', '#include "ModernLogger.h"', content)
    
    # Replace old macro calls with new ones
    # LOG_DEBUG_FMT -> LOG_DEBUG
    content = re.sub(r'LOG_DEBUG_FMT\s*\(\s*([^,]+),\s*(.+?)\s*\)', r'LOG_DEBUG(\1, \2)', content)
    content = re.sub(r'LOG_INFO_FMT\s*\(\s*([^,]+),\s*(.+?)\s*\)', r'LOG_INFO(\1, \2)', content)
    content = re.sub(r'LOG_WARNING_FMT\s*\(\s*([^,]+),\s*(.+?)\s*\)', r'LOG_WARN(\1, \2)', content)
    content = re.sub(r'LOG_ERROR_FMT\s*\(\s*([^,]+),\s*(.+?)\s*\)', r'LOG_ERROR(\1, \2)', content)
    content = re.sub(r'LOG_FATAL_FMT\s*\(\s*([^,]+),\s*(.+?)\s*\)', r'LOG_FATAL(\1, \2)', content)
    
    # Replace simple macro calls (these are already compatible)
    # LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_FATAL stay the same
    
    # Replace C-style format strings with modern C++ format
    # %s -> {}
    content = re.sub(r'%s', '{}', content)
    # %d -> {}
    content = re.sub(r'%d', '{}', content)
    # %f -> {:.2f} (for better formatting)
    content = re.sub(r'%f', '{:.2f}', content)
    
    # Remove .c_str() calls where they're not needed
    content = re.sub(r'\.c_str\(\)', '', content)
    
    if content != original_content:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"  ✓ Updated")
        return True
    else:
        print(f"  - No changes needed")
        return False

def main():
    """Main migration function"""
    src_dir = Path("src")
    if not src_dir.exists():
        print("Error: src directory not found")
        sys.exit(1)
    
    migrated_files = 0
    
    # Find all C++ files
    for file_path in src_dir.rglob("*.cpp"):
        if migrate_file(file_path):
            migrated_files += 1
    
    for file_path in src_dir.rglob("*.h"):
        if migrate_file(file_path):
            migrated_files += 1
    
    print(f"\nMigration complete! Updated {migrated_files} files.")
    print("\nNext steps:")
    print("1. Remove old Logger.h and Logger.cpp files")
    print("2. Update your build system to include ModernLogger.cpp")
    print("3. Test your application")

if __name__ == "__main__":
    main()
