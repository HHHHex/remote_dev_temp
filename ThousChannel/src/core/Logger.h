#pragma once

#include <afxwin.h>
#include <afxcmn.h>
#include <fstream>
#include <sstream>

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
    
    // Writes a log message
    void Log(LogLevel level, const CString& message);
    void Log(LogLevel level, LPCTSTR format, ...);
    
    // Convenience methods
    void Debug(const CString& message);
    void Info(const CString& message);
    void Warning(const CString& message);
    void Error(const CString& message);
    void Fatal(const CString& message);
    
    // Formats a log message
    CString FormatLogMessage(LogLevel level, const CString& message);
    
    // Gets the current time as a string
    CString GetCurrentTimeString();
    
    // Gets the string representation of a log level
    CString GetLevelString(LogLevel level);

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
    
    // Writes a message to the debug output
    void WriteToDebug(const CString& message);
};

// Macro definitions for easy use
#define LOG_DEBUG(msg) CLogger::GetInstance().Debug(msg)
#define LOG_INFO(msg) CLogger::GetInstance().Info(msg)
#define LOG_WARNING(msg) CLogger::GetInstance().Warning(msg)
#define LOG_ERROR(msg) CLogger::GetInstance().Error(msg)
#define LOG_FATAL(msg) CLogger::GetInstance().Fatal(msg)

#define LOG_DEBUG_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_INFO_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_INFO, fmt, ##__VA_ARGS__)
#define LOG_WARNING_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_WARNING, fmt, ##__VA_ARGS__)
#define LOG_ERROR_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_ERROR, fmt, ##__VA_ARGS__)
#define LOG_FATAL_FMT(fmt, ...) CLogger::GetInstance().Log(LOG_FATAL, fmt, ##__VA_ARGS__)

