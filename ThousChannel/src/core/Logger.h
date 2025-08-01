#pragma once

#include <afxwin.h>
#include <afxcmn.h>
#include <fstream>
#include <sstream>

// 鏃ュ織绾у埆鏋氫妇
enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
    LOG_FATAL = 4
};

// 鏃ュ織宸ュ叿绫?
class CLogger
{
public:
    static CLogger& GetInstance();
    
    // 鍒濆鍖栨棩蹇楃郴缁?
    void Initialize(const CString& logFilePath = _T(""));
    
    // 鍐欏叆鏃ュ織
    void Log(LogLevel level, const CString& message);
    void Log(LogLevel level, LPCTSTR format, ...);
    
    // 渚挎嵎鏂规硶
    void Debug(const CString& message);
    void Info(const CString& message);
    void Warning(const CString& message);
    void Error(const CString& message);
    void Fatal(const CString& message);
    
    // 鏍煎紡鍖栨棩蹇楁秷鎭?
    CString FormatLogMessage(LogLevel level, const CString& message);
    
    // 鑾峰彇褰撳墠鏃堕棿瀛楃涓?
    CString GetCurrentTimeString();
    
    // 鑾峰彇鏃ュ織绾у埆瀛楃涓?
    CString GetLevelString(LogLevel level);

private:
    CLogger();
    ~CLogger();
    
    // 绂佺敤鎷疯礉鏋勯€犲拰璧嬪€?
    CLogger(const CLogger&) = delete;
    CLogger& operator=(const CLogger&) = delete;
    
    std::ofstream m_logFile;
    CString m_logFilePath;
    bool m_initialized;
    CRITICAL_SECTION m_cs;
    
    // 鍐欏叆鍒版枃浠?
    void WriteToFile(const CString& message);
    
    // 鍐欏叆鍒拌皟璇曡緭鍑?
    void WriteToDebug(const CString& message);
};

// 瀹忓畾涔夛紝鏂逛究浣跨敤
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

