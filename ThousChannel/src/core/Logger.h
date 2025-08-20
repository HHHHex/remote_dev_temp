#pragma once

#include <afxwin.h>
#include <afxcmn.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

// Enum for log levels
enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
    LOG_FATAL = 4
};

// Logger utility class
class CLogger
{
public:
    static CLogger& GetInstance();
    
    // Initializes the logging system
    void Initialize(const std::string& logFilePath = "");
    
    // New std::string interface for better encoding stability
    void Log(LogLevel level, const std::string& message);
    void Log(LogLevel level, const char* format, ...);
    
    // C++ style formatting with variadic templates
    template<typename... Args>
    void Log(LogLevel level, const std::string& format, Args&&... args) {
        if (!m_initialized) {
            Initialize();
        }
        
        // Simple {} replacement implementation
        std::string result = format;
        std::vector<std::string> values;
        
        // Convert arguments to strings using fold expression
        (values.push_back([&]() {
            std::ostringstream oss;
            oss << std::forward<Args>(args);
            return oss.str();
        }()), ...);
        
        // Replace {} placeholders with values
        size_t placeholderPos = 0;
        for (const auto& value : values) {
            size_t pos = result.find("{}", placeholderPos);
            if (pos != std::string::npos) {
                result.replace(pos, 2, value);
                placeholderPos = pos + value.length();
            }
        }
        
        std::string formattedMessage = FormatLogMessage(level, result);
        
        EnterCriticalSection(&m_cs);
        WriteToFile(formattedMessage);
        WriteToDebug(formattedMessage);
        LeaveCriticalSection(&m_cs);
    }
    
    // New convenience methods using std::string
    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warning(const std::string& message);
    void Error(const std::string& message);
    void Fatal(const std::string& message);
    
    // Formats a log message
    std::string FormatLogMessage(LogLevel level, const std::string& message);
    
    // Gets the current time as a string
    std::string GetCurrentTimeStringUTF8();
    
    // Gets the string representation of a log level
    std::string GetLevelStringUTF8(LogLevel level);

private:
    CLogger();
    ~CLogger();
    
    // Disable copy constructor and assignment operator
    CLogger(const CLogger&) = delete;
    CLogger& operator=(const CLogger&) = delete;
    
    std::ofstream m_logFile;
    std::string m_logFilePath;
    bool m_initialized;
    CRITICAL_SECTION m_cs;
    
    // Writes a message to the log file
    void WriteToFile(const std::string& message);
    
    // Writes a message to the debug output
    void WriteToDebug(const std::string& message);
};

// Unified logging macros - all use std::string internally for better encoding stability
#define LOG_DEBUG(msg) CLogger::GetInstance().Debug(std::string(_T(msg)))
#define LOG_INFO(msg) CLogger::GetInstance().Info(std::string(_T(msg)))
#define LOG_WARNING(msg) CLogger::GetInstance().Warning(std::string(_T(msg)))
#define LOG_ERROR(msg) CLogger::GetInstance().Error(std::string(_T(msg)))
#define LOG_FATAL(msg) CLogger::GetInstance().Fatal(std::string(_T(msg)))

// C++ style formatting macros - use {} placeholders
#define LOG_DEBUG_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_DEBUG, std::string(_T(fmt)), ##__VA_ARGS__)
#define LOG_INFO_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_INFO, std::string(_T(fmt)), ##__VA_ARGS__)
#define LOG_WARNING_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_WARNING, std::string(_T(fmt)), ##__VA_ARGS__)
#define LOG_ERROR_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_ERROR, std::string(_T(fmt)), ##__VA_ARGS__)
#define LOG_FATAL_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_FATAL, std::string(_T(fmt)), ##__VA_ARGS__)

