#include "ModernLogger.h"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

std::string ModernLogger::formatMessage(LogLevel level, const std::string& message, 
                                       const std::source_location& location) {
    std::ostringstream oss;
    
    // Timestamp
    oss << "[" << getCurrentTime() << "] ";
    
    // Log level
    oss << "[" << getLevelString(level) << "] ";
    
    // Source location (file:line:function)
    oss << "[" << std::filesystem::path(location.file_name()).filename().string() 
        << ":" << location.line() << ":" << location.function_name() << "] ";
    
    // Message
    oss << message;
    
    return oss.str();
}

void ModernLogger::writeToFile(const std::string& message) {
    // Ensure log directory exists
    auto logDir = std::filesystem::path(m_logPath).parent_path();
    if (!logDir.empty()) {
        std::filesystem::create_directories(logDir);
    }
    
    // Open file if not already open
    if (!m_logFile.is_open()) {
        m_logFile.open(m_logPath, std::ios::app | std::ios::binary);
        if (m_logFile.tellp() == 0) {
            // Write UTF-8 BOM for new files
            m_logFile.write("\xEF\xBB\xBF", 3);
        }
    }
    
    if (m_logFile.is_open()) {
        m_logFile << message << std::endl;
        m_logFile.flush();
    }
}

void ModernLogger::writeToDebug(const std::string& message) {
#ifdef _DEBUG
#ifdef _WIN32
    // Windows debug output
    OutputDebugStringA((message + "\n").c_str());
#else
    // Unix debug output
    std::cerr << message << std::endl;
#endif
#endif
}

std::string ModernLogger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

std::string ModernLogger::getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info:  return "INFO ";
        case LogLevel::Warn:  return "WARN ";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Fatal: return "FATAL";
        default:              return "UNKNOWN";
    }
}
