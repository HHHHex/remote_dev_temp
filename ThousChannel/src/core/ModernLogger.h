#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <source_location>
#include <format>

// Modern C++ logging levels
enum class LogLevel : uint8_t {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
    Fatal = 5
};

// Thread-safe singleton logger
class ModernLogger {
public:
    static ModernLogger& instance() {
        static ModernLogger logger;
        return logger;
    }

    // Modern C++20 style logging with source location
    template<typename... Args>
    void log(LogLevel level, std::string_view format, Args&&... args,
             const std::source_location& location = std::source_location::current()) {
        
        if (level < m_minLevel) return;
        
        // Use std::format for type-safe formatting (C++20)
        std::string message;
        if constexpr (sizeof...(Args) == 0) {
            message = std::string(format);
        } else {
            message = std::vformat(format, std::make_format_args(std::forward<Args>(args)...));
        }
        
        // Format with source location
        auto formatted = formatMessage(level, message, location);
        
        // Thread-safe write
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            writeToFile(formatted);
            writeToDebug(formatted);
        }
    }

    // Convenience methods
    template<typename... Args>
    void trace(std::string_view format, Args&&... args) {
        log(LogLevel::Trace, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void debug(std::string_view format, Args&&... args) {
        log(LogLevel::Debug, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(std::string_view format, Args&&... args) {
        log(LogLevel::Info, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warn(std::string_view format, Args&&... args) {
        log(LogLevel::Warn, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(std::string_view format, Args&&... args) {
        log(LogLevel::Error, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void fatal(std::string_view format, Args&&... args) {
        log(LogLevel::Fatal, format, std::forward<Args>(args)...);
    }

    // Configuration
    void setLogLevel(LogLevel level) { m_minLevel = level; }
    void setLogFile(const std::string& path) { m_logPath = path; }

private:
    ModernLogger() = default;
    ~ModernLogger() = default;
    ModernLogger(const ModernLogger&) = delete;
    ModernLogger& operator=(const ModernLogger&) = delete;

    std::string formatMessage(LogLevel level, const std::string& message, 
                             const std::source_location& location);
    void writeToFile(const std::string& message);
    void writeToDebug(const std::string& message);
    std::string getCurrentTime();
    std::string getLevelString(LogLevel level);

    std::mutex m_mutex;
    std::ofstream m_logFile;
    std::string m_logPath = "logs/modern_log.txt";
    LogLevel m_minLevel = LogLevel::Debug;
};

// Clean macro definitions - NO string conversion overhead!
#define LOG_TRACE(...) ModernLogger::instance().trace(__VA_ARGS__)
#define LOG_DEBUG(...) ModernLogger::instance().debug(__VA_ARGS__)
#define LOG_INFO(...)  ModernLogger::instance().info(__VA_ARGS__)
#define LOG_WARN(...)  ModernLogger::instance().warn(__VA_ARGS__)
#define LOG_ERROR(...) ModernLogger::instance().error(__VA_ARGS__)
#define LOG_FATAL(...) ModernLogger::instance().fatal(__VA_ARGS__)

// Conditional logging macros
#define LOG_DEBUG_IF(condition, ...) \
    do { if (condition) LOG_DEBUG(__VA_ARGS__); } while(0)

#define LOG_ERROR_IF(condition, ...) \
    do { if (condition) LOG_ERROR(__VA_ARGS__); } while(0)
