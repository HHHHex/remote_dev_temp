#include "pch.h"
#include "Logger.h"
#include <ctime>
#include <iomanip>
#include <vector>

CLogger::CLogger()
    : m_initialized(false)
{
    InitializeCriticalSection(&m_cs);
}

CLogger::~CLogger()
{
    if (m_logFile.is_open())
    {
        m_logFile.close();
    }
    DeleteCriticalSection(&m_cs);
}

CLogger& CLogger::GetInstance()
{
    static CLogger instance;
    return instance;
}

void CLogger::Initialize(const CString& logFilePath)
{
    EnterCriticalSection(&m_cs);
    
    if (m_initialized)
    {
        LeaveCriticalSection(&m_cs);
        return;
    }
    
    if (logFilePath.IsEmpty())
    {
        // Default log file path
        CString defaultPath;
        defaultPath.Format(_T("logs\\ThousChannel_%s.log"), GetCurrentTimeString());
        m_logFilePath = defaultPath;
    }
    else
    {
        m_logFilePath = logFilePath;
    }
    
    // Create log directory
    CString logDir = m_logFilePath;
    int pos = logDir.ReverseFind(_T('\\'));
    if (pos > 0)
    {
        logDir = logDir.Left(pos);
        CreateDirectory(logDir, NULL);
    }
    
    // Open log file
    m_logFile.open(m_logFilePath, std::ios::app);
    m_initialized = true;
    
    // Write startup log message
    CString startMsg;
    startMsg.Format(_T("=== ThousChannel Application Started at %s ==="), GetCurrentTimeString());
    WriteToFile(startMsg);
    WriteToDebug(startMsg);
    
    LeaveCriticalSection(&m_cs);
}

void CLogger::Log(LogLevel level, const CString& message)
{
    if (!m_initialized)
    {
        Initialize();
    }
    
    CString formattedMessage = FormatLogMessage(level, message);
    
    EnterCriticalSection(&m_cs);
    WriteToFile(formattedMessage);
    WriteToDebug(formattedMessage);
    LeaveCriticalSection(&m_cs);
}

void CLogger::Log(LogLevel level, LPCTSTR format, ...)
{
    CString message;
    va_list args;
    va_start(args, format);
    message.FormatV(format, args);
    va_end(args);
    
    Log(level, message);
}

void CLogger::Debug(const CString& message)
{
    Log(LOG_DEBUG, message);
}

void CLogger::Info(const CString& message)
{
    Log(LOG_INFO, message);
}

void CLogger::Warning(const CString& message)
{
    Log(LOG_WARNING, message);
}

void CLogger::Error(const CString& message)
{
    Log(LOG_ERROR, message);
}

void CLogger::Fatal(const CString& message)
{
    Log(LOG_FATAL, message);
}

CString CLogger::FormatLogMessage(LogLevel level, const CString& message)
{
    CString formattedMessage;
    formattedMessage.Format(_T("[%s] [%s] %s"), 
        GetCurrentTimeString(), 
        GetLevelString(level), 
        message);
    return formattedMessage;
}

CString CLogger::GetCurrentTimeString()
{
    CTime currentTime = CTime::GetCurrentTime();
    CString timeStr = currentTime.Format(_T("%Y-%m-%d %H:%M:%S"));
    return timeStr;
}

CString CLogger::GetLevelString(LogLevel level)
{
    switch (level)
    {
    case LOG_DEBUG:   return _T("DEBUG");
    case LOG_INFO:    return _T("INFO ");
    case LOG_WARNING: return _T("WARN ");
    case LOG_ERROR:   return _T("ERROR");
    case LOG_FATAL:   return _T("FATAL");
    default:          return _T("UNKNOWN");
    }
}

void CLogger::WriteToFile(const CString& message)
{
    if (m_logFile.is_open())
    {
        // Convert to UTF-8 and write to file
        int utf8Len = WideCharToMultiByte(CP_UTF8, 0, message, -1, nullptr, 0, nullptr, nullptr);
        if (utf8Len > 0) {
            std::vector<char> utf8Buffer(utf8Len);
            WideCharToMultiByte(CP_UTF8, 0, message, -1, utf8Buffer.data(), utf8Len, nullptr, nullptr);
            m_logFile << utf8Buffer.data() << std::endl;
            m_logFile.flush();
        }
    }
}

void CLogger::WriteToDebug(const CString& message)
{
#ifdef _DEBUG
    OutputDebugString(message + _T("\n"));
#endif
}
