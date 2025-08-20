#pragma once

#include <afxwin.h>
#include <afxcmn.h>
#include <fstream>
#include <sstream>
#include <string>

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
    void Initialize(const CString& logFilePath = _T(""));
    
    // Writes a log message - Legacy CString interface for backward compatibility
    void Log(LogLevel level, const CString& message);
    void Log(LogLevel level, LPCTSTR format, ...);
    
    // Convenience methods - Legacy CString interface for backward compatibility
    void Debug(const CString& message);
    void Info(const CString& message);
    void Warning(const CString& message);
    void Error(const CString& message);
    void Fatal(const CString& message);
    
    // New std::string interface for better encoding stability
    void Log(LogLevel level, const std::string& message);
    void Log(LogLevel level, const char* format, ...);
    
    // New convenience methods using std::string
    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warning(const std::string& message);
    void Error(const std::string& message);
    void Fatal(const std::string& message);
    
    // Formats a log message
    CString FormatLogMessage(LogLevel level, const CString& message);
    std::string FormatLogMessage(LogLevel level, const std::string& message);
    
    // Gets the current time as a string
    CString GetCurrentTimeString();
    std::string GetCurrentTimeStringUTF8();
    
    // Gets the string representation of a log level
    CString GetLevelString(LogLevel level);
    std::string GetLevelStringUTF8(LogLevel level);

private:
    CLogger();
    ~CLogger();
    
    // Disable copy constructor and assignment operator
    CLogger(const CLogger&) = delete;
    CLogger& operator=(const CLogger&) = delete;
    
    std::ofstream m_logFile;
    CString m_logFilePath;
    bool m_initialized;
    CRITICAL_SECTION m_cs;
    
    // Writes a message to the log file
    void WriteToFile(const CString& message);
    void WriteToFile(const std::string& message);
    
    // Writes a message to the debug output
    void WriteToDebug(const CString& message);
    void WriteToDebug(const std::string& message);
    
    // Helper methods for string conversion
    std::string CStringToUTF8(const CString& str);
    std::string WideCharToUTF8(LPCWSTR wideStr);
};

// Macro definitions for easy use - automatically handle _T() conversion
#define LOG_DEBUG(msg) CLogger::GetInstance().Debug(_T(msg))
#define LOG_INFO(msg) CLogger::GetInstance().Info(_T(msg))
#define LOG_WARNING(msg) CLogger::GetInstance().Warning(_T(msg))
#define LOG_ERROR(msg) CLogger::GetInstance().Error(_T(msg))
#define LOG_FATAL(msg) CLogger::GetInstance().Fatal(_T(msg))

#define LOG_DEBUG_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_DEBUG, _T(fmt), ##__VA_ARGS__)
#define LOG_INFO_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_INFO, _T(fmt), ##__VA_ARGS__)
#define LOG_WARNING_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_WARNING, _T(fmt), ##__VA_ARGS__)
#define LOG_ERROR_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_ERROR, _T(fmt), ##__VA_ARGS__)
#define LOG_FATAL_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_FATAL, _T(fmt), ##__VA_ARGS__)

