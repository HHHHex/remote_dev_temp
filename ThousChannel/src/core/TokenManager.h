#pragma once

#include <functional>
#include <memory>
#include <winhttp.h>
#include <atlstr.h>
#include <thread>

// Token鑾峰彇鍥炶皟鍑芥暟绫诲瀷
typedef std::function<void(const CString& token, bool success, const CString& errorMsg)> TokenCallback;

// Token鐢熸垚鍙傛暟缁撴瀯
struct TokenGenerateParams {
    CString appId;              // 搴旂敤ID
    CString appCertificate;     // 搴旂敤璇佷功
    CString channelName;        // 棰戦亾鍚嶇О
    CString uid;                // 鐢ㄦ埛ID
    int expire;                 // 杩囨湡鏃堕棿锛堢锛?
    int type;                   // Token绫诲瀷锛?: RTC Token, 2: RTM Token锛?
    CString src;                // 鏉ユ簮鏍囪瘑

    TokenGenerateParams() {
        expire = 900;           // 榛樿15鍒嗛挓
        type = 1;               // 榛樿RTC Token
        src = _T("Windows");
    }
};

/**
 * Token绠＄悊鍣?
 * 璐熻矗浠嶢gora鏈嶅姟绔幏鍙朢TC Token
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

    // 璁剧疆Token鏈嶅姟鍣║RL
    void SetServerUrl(const CString& serverUrl);

    // 璁剧疆璇锋眰瓒呮椂鏃堕棿
    void SetTimeout(DWORD timeoutMs);

    // 寮傛鐢熸垚Token
    void GenerateTokenAsync(const TokenGenerateParams& params, TokenCallback callback);

    // 鍚屾鐢熸垚Token锛堥樆濉炶皟鐢級
    bool GenerateTokenSync(const TokenGenerateParams& params, CString& token, CString& errorMsg);

private:
    // 鏋勫缓璇锋眰JSON
    CString BuildRequestJson(const TokenGenerateParams& params);

    // 瑙ｆ瀽鍝嶅簲JSON
    bool ParseResponseJson(const CString& jsonResponse, CString& token, CString& errorMsg);

    // 鎵цHTTP POST璇锋眰
    bool ExecuteHttpRequest(const CString& url, const CString& postData, 
                           CString& response, CString& errorMsg);

    // 寮傛璇锋眰绾跨▼鍑芥暟
    static void AsyncRequestThread(CTokenManager* pManager, TokenGenerateParams params, TokenCallback callback);

    // URL缂栫爜
    CString UrlEncode(const CString& text);

    // 鑾峰彇褰撳墠鏃堕棿鎴筹紙姣锛?
    CString GetCurrentTimestamp();
}; 
