#pragma once

#include <functional>
#include <memory>
#include <winhttp.h>
#include <atlstr.h>
#include <thread>

// Token retrieval callback function type
typedef std::function<void(const CString& token, bool success, const CString& errorMsg)> TokenCallback;

// Token generation parameter structure
struct TokenGenerateParams {
    CString appId;              // Application ID
    CString appCertificate;     // Application certificate
    CString channelName;        // Channel name
    CString userId;             // User ID
    int expire;                 // Expiration time (seconds)
    int type;                   // Token type (1: RTC Token, 2: RTM Token)
    CString src;                // Source identifier

    TokenGenerateParams() {
        expire = 900;           // Default 15 minutes
        type = 1;               // Default RTC Token
        src = _T("Windows");
    }
};

/**
 * Token Manager
 * Responsible for obtaining RTC Token from Agora server
 */
class CTokenManager
{
private:
    static const CString DEFAULT_TOKEN_SERVER_URL;
    CString m_serverUrl;
    DWORD m_timeoutMs;

public:
    CTokenManager();
    virtual ~CTokenManager();

    // Set Token server URL
    void SetServerUrl(const CString& serverUrl);

    // Set request timeout
    void SetTimeout(DWORD timeoutMs);

    // Asynchronously generate Token
    void GenerateTokenAsync(const TokenGenerateParams& params, TokenCallback callback);

    // Synchronously generate Token (blocking call)
    bool GenerateTokenSync(const TokenGenerateParams& params, CString& token, CString& errorMsg);

private:
    // Build request JSON
    CString BuildRequestJson(const TokenGenerateParams& params);

    // Parse response JSON
    bool ParseResponseJson(const CString& jsonResponse, CString& token, CString& errorMsg);

    // Execute HTTP POST request
    bool ExecuteHttpRequest(const CString& url, const CString& postData, 
                           CString& response, CString& errorMsg);

    // Asynchronous request thread function
    static void AsyncRequestThread(CTokenManager* pManager, TokenGenerateParams params, TokenCallback callback);

    // URL encode
    CString UrlEncode(const CString& text);

    // Get current timestamp (milliseconds)
    CString GetCurrentTimestamp();
};
