#include "pch.h"
#include "Logger.h"
#include <ctime>
#include <iomanip>
#include <vector>
#include <cstdarg>
#include <sstream>
#include <regex>

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

void CLogger::Initialize(const std::string& logFilePath)
{
    EnterCriticalSection(&m_cs);
    
    if (m_initialized)
    {
        LeaveCriticalSection(&m_cs);
        return;
    }
    
    if (logFilePath.empty())
    {
        // Default log file path
        std::string defaultPath = "logs\\ThousChannel_" + GetCurrentTimeStringUTF8() + ".log";
        m_logFilePath = defaultPath;
    }
    else
    {
        m_logFilePath = logFilePath;
    }
    
    // Create log directory
    std::string logDir = m_logFilePath;
    size_t pos = logDir.find_last_of('\\');
    if (pos != std::string::npos)
    {
        logDir = logDir.substr(0, pos);
        CreateDirectoryA(logDir.c_str(), NULL);
    }
    
    // Open log file in binary mode for UTF-8 support
    m_logFile.open(m_logFilePath, std::ios::app | std::ios::binary);
    if (m_logFile.tellp() == 0) {
        // Write UTF-8 BOM if file is empty
        m_logFile.write("\xEF\xBB\xBF", 3);
    }
    m_initialized = true;
    
    // Write startup log message
    std::string startMsg = "=== ThousChannel Application Started at " + GetCurrentTimeStringUTF8() + " ===";
    WriteToFile(startMsg);
    WriteToDebug(startMsg);
    
    LeaveCriticalSection(&m_cs);
}



// New std::string interface implementations
void CLogger::Log(LogLevel level, const std::string& message)
{
    if (!m_initialized)
    {
        Initialize();
    }
    
    std::string formattedMessage = FormatLogMessage(level, message);
    
    EnterCriticalSection(&m_cs);
    WriteToFile(formattedMessage);
    WriteToDebug(formattedMessage);
    LeaveCriticalSection(&m_cs);
}

void CLogger::Log(LogLevel level, const char* format, ...)
{
    std::string message;
    va_list args;
    va_start(args, format);
    
    // Calculate required buffer size
    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);
    
    if (size > 0) {
        std::vector<char> buffer(static_cast<size_t>(size) + 1);
        vsnprintf(buffer.data(), buffer.size(), format, args);
        message = buffer.data();
    }
    
    va_end(args);
    
    Log(level, message);
}



// New std::string convenience methods
void CLogger::Debug(const std::string& message)
{
    Log(LOG_DEBUG, message);
}

void CLogger::Info(const std::string& message)
{
    Log(LOG_INFO, message);
}

void CLogger::Warning(const std::string& message)
{
    Log(LOG_WARNING, message);
}

void CLogger::Error(const std::string& message)
{
    Log(LOG_ERROR, message);
}

void CLogger::Fatal(const std::string& message)
{
    Log(LOG_FATAL, message);
}



std::string CLogger::FormatLogMessage(LogLevel level, const std::string& message)
{
    std::string formattedMessage;
    formattedMessage = "[" + GetCurrentTimeStringUTF8() + "] [" + GetLevelStringUTF8(level) + "] " + message;
    return formattedMessage;
}



std::string CLogger::GetCurrentTimeStringUTF8()
{
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return std::string(buffer);
}



std::string CLogger::GetLevelStringUTF8(LogLevel level)
{
    switch (level)
    {
    case LOG_DEBUG:   return "DEBUG";
    case LOG_INFO:    return "INFO ";
    case LOG_WARNING: return "WARN ";
    case LOG_ERROR:   return "ERROR";
    case LOG_FATAL:   return "FATAL";
    default:          return "UNKNOWN";
    }
}



void CLogger::WriteToFile(const std::string& message)
{
    if (m_logFile.is_open())
    {
        m_logFile << message << std::endl;
        m_logFile.flush();
    }
}



void CLogger::WriteToDebug(const std::string& message)
{
#ifdef _DEBUG
    // Convert std::string to wide string for OutputDebugStringW
    int wideLen = MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, nullptr, 0);
    if (wideLen > 0) {
        std::vector<wchar_t> wideBuffer(wideLen);
        MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, wideBuffer.data(), wideLen);
        OutputDebugStringW(wideBuffer.data());
        OutputDebugStringW(L"\n");
    }
#endif
    
    // Also output to console if available
    if (GetConsoleWindow()) {
        printf("%s\n", message.c_str());
    }
}


