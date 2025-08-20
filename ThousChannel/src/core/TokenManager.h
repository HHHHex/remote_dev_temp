#pragma once

#include <functional>
#include <memory>
#include <winhttp.h>
#include <string>
#include <thread>

// Token retrieval callback function type
typedef std::function<void(const std::string& token, bool success, const std::string& errorMsg)> TokenCallback;

// Token generation parameter structure
struct TokenGenerateParams {
    std::string appId;              // Application ID
    std::string appCertificate;     // Application certificate
    std::string channelName;        // Channel name
    std::string userId;             // User ID
    int expire;                     // Expiration time (seconds)
    int type;                       // Token type (1: RTC Token, 2: RTM Token)
    std::string src;                // Source identifier

    TokenGenerateParams() {
        expire = 900;               // Default 15 minutes
        type = 1;                   // Default RTC Token
        src = "Windows";
    }
};

/**
 * Token Manager
 * Responsible for obtaining RTC Token from Agora server
 */
class CTokenManager
{
private:
    static const std::string DEFAULT_TOKEN_SERVER_URL;
    std::string m_serverUrl;
    DWORD m_timeoutMs;

public:
    CTokenManager();
    virtual ~CTokenManager();

    // Set Token server URL
    void SetServerUrl(const std::string& serverUrl);

    // Set request timeout
    void SetTimeout(DWORD timeoutMs);

    // Asynchronously generate Token
    void GenerateTokenAsync(const TokenGenerateParams& params, TokenCallback callback);

    // Synchronously generate Token (blocking call)
    bool GenerateTokenSync(const TokenGenerateParams& params, std::string& token, std::string& errorMsg);

private:
    // Build request JSON
    std::string BuildRequestJson(const TokenGenerateParams& params);

    // Parse response JSON
    bool ParseResponseJson(const std::string& jsonResponse, std::string& token, std::string& errorMsg);

    // Execute HTTP POST request
    bool ExecuteHttpRequest(const std::string& url, const std::string& postData, 
                           std::string& response, std::string& errorMsg);

    // Asynchronous request thread function
    static void AsyncRequestThread(CTokenManager* pManager, TokenGenerateParams params, TokenCallback callback);

    // URL encode
    std::string UrlEncode(const std::string& text);

    // Get current timestamp (milliseconds)
    std::string GetCurrentTimestamp();
};
