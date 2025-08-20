#include "src/core/ModernLogger.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    // 配置日志系统
    auto& logger = ModernLogger::instance();
    logger.setLogLevel(LogLevel::Debug);
    logger.setLogFile("logs/test_modern_logger.log");
    
    std::cout << "Testing Modern Logger System..." << std::endl;
    
    // 测试基本日志功能
    LOG_INFO("=== Modern Logger Test Started ===");
    LOG_DEBUG("Debug message test");
    LOG_INFO("Info message test");
    LOG_WARN("Warning message test");
    LOG_ERROR("Error message test");
    LOG_FATAL("Fatal message test");
    
    // 测试格式化功能
    std::string userName = "TestUser";
    int userId = 12345;
    double score = 95.5;
    
    LOG_INFO("User {} (ID: {}) logged in with score: {:.2f}", userName, userId, score);
    
    // 测试条件日志
    bool debugMode = true;
    LOG_DEBUG_IF(debugMode, "Debug mode is enabled");
    LOG_DEBUG_IF(!debugMode, "This should not appear");
    
    // 测试复杂对象
    std::vector<int> data = {1, 2, 3, 4, 5};
    LOG_DEBUG("Processing {} items", data.size());
    
    for (size_t i = 0; i < data.size(); ++i) {
        LOG_TRACE("Processing item {}: {}", i, data[i]);
    }
    
    // 测试错误处理
    try {
        throw std::runtime_error("Test exception");
    } catch (const std::exception& e) {
        LOG_ERROR("Exception caught: {}", e.what());
    }
    
    // 测试性能日志
    auto start = std::chrono::high_resolution_clock::now();
    // 模拟一些工作
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    LOG_INFO("Test operation completed in {} ms", duration.count());
    
    // 测试基本字符串日志
    LOG_INFO("Testing basic string logging");
    LOG_WARN("Testing warning logging");
    LOG_ERROR("Testing error logging");
    
    LOG_INFO("=== Modern Logger Test Completed ===");
    
    std::cout << "Test completed. Check logs/test_modern_logger.log for output." << std::endl;
    
    return 0;
}
