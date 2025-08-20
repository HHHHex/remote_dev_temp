#pragma once

#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <future>

// Modern token retrieval callback function type
using TokenCallback = std::function<void(const std::string& token, bool success, const std::string& errorMsg)>;

// Modern token generation parameter structure
struct ModernTokenParams {
    std::string appId;              // Application ID
    std::string appCertificate;     // Application certificate
    std::string channelName;        // Channel name
    std::string userId;             // User ID
    int expire;                     // Expiration time (seconds)
    int type;                       // Token type (1: RTC Token, 2: RTM Token)
    std::string src;                // Source identifier

    ModernTokenParams() {
        expire = 900;               // Default 15 minutes
        type = 1;                   // Default RTC Token
        src = "Windows";
    }
};

// Modern token result structure
struct ModernTokenResult {
    std::string token;
    bool success;
    std::string errorMsg;
    
    ModernTokenResult() : success(false) {}
    ModernTokenResult(const std::string& t, bool s, const std::string& e) 
        : token(t), success(s), errorMsg(e) {}
};

/**
 * Modern Token Manager
 * Completely C++20 compliant token manager without any MFC dependencies
 */
class ModernTokenManager
{
public:
    ModernTokenManager();
    virtual ~ModernTokenManager();

    // Set request timeout
    void setTimeout(std::chrono::milliseconds timeout);

    // Asynchronously generate token (returns future)
    std::future<ModernTokenResult> generateTokenAsync(const ModernTokenParams& params);

    // Asynchronously generate token with callback
    void generateTokenAsync(const ModernTokenParams& params, TokenCallback callback);

    // Synchronously generate token (blocking call)
    ModernTokenResult generateTokenSync(const ModernTokenParams& params);

private:
    std::chrono::milliseconds m_timeout;
    
    // Build request JSON
    std::string buildRequestJson(const ModernTokenParams& params);

    // Parse response JSON
    bool parseResponseJson(const std::string& jsonResponse, std::string& token, std::string& errorMsg);

    // Execute HTTP POST request
    bool executeHttpRequest(const std::string& url, const std::string& postData, 
                           std::string& response, std::string& errorMsg);

    // URL encode
    std::string urlEncode(const std::string& text);

    // Get current timestamp (milliseconds)
    std::string getCurrentTimestamp();

    // Asynchronous request thread function
    static void asyncRequestThread(ModernTokenManager* manager, ModernTokenParams params, 
                                  std::promise<ModernTokenResult> promise);
};
