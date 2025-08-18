#include "pch.h"
#include "TokenManager.h"
#include "Logger.h"
#include <winhttp.h>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

#pragma comment(lib, "winhttp.lib")

// ============================================================================
// CTokenManager 实现
// ============================================================================

CTokenManager::CTokenManager()
    : m_timeoutMs(30000)  // 默认30秒超时
{
    LOG_INFO("Token Manager created");
}

CTokenManager::~CTokenManager()
{
    LOG_INFO("Token Manager destroyed");
}

void CTokenManager::SetTimeout(DWORD timeoutMs)
{
    m_timeoutMs = timeoutMs;
}

void CTokenManager::GenerateTokenAsync(const TokenGenerateParams& params, TokenCallback callback)
{
    if (!callback) {
        return;
    }

    std::thread requestThread(AsyncRequestThread, this, params, callback);
    requestThread.detach();
}

bool CTokenManager::GenerateTokenSync(const TokenGenerateParams& params, CString& token, CString& errorMsg)
{
    // 构建请求数据
    CString jsonData = BuildRequestJson(params);
    if (jsonData.IsEmpty()) {
        errorMsg = _T("构建请求数据失败");
        return false;
    }

    // 执行HTTP请求
    CString response;
    if (!ExecuteHttpRequest(_T("https://service.agora.io/toolbox-global/v1/token/generate"), 
                           jsonData, response, errorMsg)) {
        return false;
    }

    // 解析响应
    return ParseResponseJson(response, token, errorMsg);
}

CString CTokenManager::BuildRequestJson(const TokenGenerateParams& params)
{
    try {
        CString json;
        CString timestamp = GetCurrentTimestamp();
        
        json.Format(_T("{"
            "\"appId\":\"%s\","
            "\"appCertificate\":\"%s\","
            "\"channelName\":\"%s\","
            "\"uid\":\"%s\","
            "\"expire\":%d,"
            "\"type\":%d,"
            "\"src\":\"%s\","
            "\"ts\":\"%s\""
            "}"),
            params.appId,
            params.appCertificate,
            params.channelName,
            params.userId,
            params.expire,
            params.type,
            params.src,
            timestamp
        );
        
        return json;
    }
    catch (const std::exception& e) {
        CStringA whatStr(e.what());
        CString errorMsg;
        errorMsg.Format(_T("Failed to build JSON: %s"), CString(whatStr));
        OutputDebugString(errorMsg);
        return _T(");
    }
}

bool CTokenManager::ParseResponseJson(const CString& jsonResponse, CString& token, CString& errorMsg)
{
    try {
        // 简单的JSON解析，查找关键字段
        CString response = jsonResponse;
        
        // 查找code字段
        int codePos = response.Find(_T("\"code\":"));
        if (codePos != -1) {
            int codeStart = codePos + 7; // "code": 的长度
            int codeEnd = response.Find(_T(","), codeStart);
            if (codeEnd == -1) codeEnd = response.Find(_T("}"), codeStart);
            
            if (codeEnd != -1) {
                CString codeStr = response.Mid(codeStart, codeEnd - codeStart);
                codeStr.Trim();
                int code = _ttoi(codeStr);
                
                // 输出服务器返回内容用于调试
                LOG_INFO_FMT("服务器返回内容: %s", response);
                
                if (code != 0) {
                    // 查找msg字段
                    int msgPos = response.Find(_T("\"msg\":"));
                    if (msgPos != -1) {
                        int msgStart = response.Find(_T("\""), msgPos + 6) + 1; // "msg": 的长度
                        int msgEnd = response.Find(_T("\""), msgStart);
                        if (msgEnd != -1) {
                            CString message = response.Mid(msgStart, msgEnd - msgStart);
                            errorMsg.Format(_T("服务器返回错误(code=%d): %s"), code, message);
                        } else {
                            errorMsg.Format(_T("服务器返回错误(code=%d)"), code);
                        }
                    } else {
                        errorMsg.Format(_T("服务器返回错误(code=%d)"), code);
                    }
                    return false;
                }
            }
        }
        
        // 查找data字段中的token
        int dataPos = response.Find(_T("\"data\":"));
        if (dataPos != -1) {
            // 在data字段中查找token
            int tokenPos = response.Find(_T("\"token\":"), dataPos);
            if (tokenPos != -1) {
                int tokenStart = response.Find(_T("\""), tokenPos + 8) + 1; // "token": 的长度
                int tokenEnd = response.Find(_T("\""), tokenStart);
                
                if (tokenEnd != -1) {
                    token = response.Mid(tokenStart, tokenEnd - tokenStart);
                    return !token.IsEmpty();
                }
            }
        }
        
        errorMsg = _T("响应中未找到data.token字段");
        return false;
    }
    catch (const std::exception& e) {
        CStringA whatStr(e.what());
        errorMsg.Format(_T("解析响应异常: %s"), CString(whatStr));
        return false;
    }
}

bool CTokenManager::ExecuteHttpRequest(const CString& url, const CString& postData, 
                                      CString& response, CString& errorMsg)
{
    HINTERNET hSession = nullptr;
    HINTERNET hConnect = nullptr;
    HINTERNET hRequest = nullptr;
    bool success = false;

    try {
        // 解析URL
        URL_COMPONENTS urlComp = { 0 };
        urlComp.dwStructSize = sizeof(urlComp);
        urlComp.dwSchemeLength = -1;
        urlComp.dwHostNameLength = -1;
        urlComp.dwUrlPathLength = -1;
        urlComp.dwExtraInfoLength = -1;

        if (!WinHttpCrackUrl(url, url.GetLength(), 0, &urlComp)) {
            errorMsg = _T("解析URL失败");
            return false;
        }

        CString hostName(urlComp.lpszHostName, urlComp.dwHostNameLength);
        CString urlPath(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);

        // 创建会话
        hSession = WinHttpOpen(L"ThousChannel/1.0",
                              WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                              WINHTTP_NO_PROXY_NAME,
                              WINHTTP_NO_PROXY_BYPASS,
                              0);
        if (!hSession) {
            errorMsg = _T("创建HTTP会话失败");
            return false;
        }

        // 设置超时
        WinHttpSetTimeouts(hSession, m_timeoutMs, m_timeoutMs, m_timeoutMs, m_timeoutMs);

        // 连接服务器
        hConnect = WinHttpConnect(hSession, hostName, 
                                 urlComp.nScheme == INTERNET_SCHEME_HTTPS ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT,
                                 0);
        if (!hConnect) {
            errorMsg = _T("连接服务器失败");
            return false;
        }

        // 创建请求
        DWORD flags = urlComp.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
        hRequest = WinHttpOpenRequest(hConnect, L"POST", urlPath, nullptr,
                                     WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
        if (!hRequest) {
            errorMsg = _T("创建HTTP请求失败");
            return false;
        }

        // 设置请求头
        CString headers = _T("Content-Type: application/json\r\n");
        WinHttpAddRequestHeaders(hRequest, headers, headers.GetLength(), WINHTTP_ADDREQ_FLAG_ADD);

        // 发送请求
        std::string postDataUtf8 = CT2A(postData, CP_UTF8);
        if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                               (LPVOID)postDataUtf8.c_str(), (DWORD)postDataUtf8.length(),
                               (DWORD)postDataUtf8.length(), 0)) {
            errorMsg = _T("发送HTTP请求失败");
            return false;
        }

        // 接收响应
        if (!WinHttpReceiveResponse(hRequest, nullptr)) {
            errorMsg = _T("接收HTTP响应失败");
            return false;
        }

        // 检查状态码
        DWORD statusCode = 0;
        DWORD statusCodeSize = sizeof(statusCode);
        WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                           WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusCodeSize, WINHTTP_NO_HEADER_INDEX);

        if (statusCode != 200) {
            errorMsg.Format(_T("HTTP请求失败，状态码: %d"), statusCode);
            return false;
        }

        // 读取响应数据
        DWORD availableBytes = 0;
        std::string responseData;
        
        do {
            availableBytes = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &availableBytes)) {
                errorMsg = _T("查询可用数据失败");
                return false;
            }

            if (availableBytes > 0) {
                std::vector<char> buffer(availableBytes + 1);
                DWORD bytesRead = 0;
                
                if (WinHttpReadData(hRequest, buffer.data(), availableBytes, &bytesRead)) {
                    buffer[bytesRead] = '\0';
                    responseData.append(buffer.data(), bytesRead);
                } else {
                    errorMsg = _T("读取响应数据失败");
                    return false;
                }
            }
        } while (availableBytes > 0);

        response = CString(responseData.c_str());
        success = true;
    }
    catch (const std::exception& e) {
        CStringA whatStr(e.what());
        errorMsg.Format(_T("HTTP请求异常: %s"), CString(whatStr));
    }

    // 清理资源
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

    return success;
}

void CTokenManager::AsyncRequestThread(CTokenManager* pManager, TokenGenerateParams params, TokenCallback callback)
{
    if (!pManager || !callback) {
        return;
    }

    CString token, errorMsg;
    bool success = pManager->GenerateTokenSync(params, token, errorMsg);
    
    // 在主线程中调用回调（需要通过消息机制）
    callback(token, success, errorMsg);
}

CString CTokenManager::UrlEncode(const CString& text)
{
    CString result;
    std::string utf8Text = CT2A(text, CP_UTF8);
    
    for (char c : utf8Text) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            result += c;
        } else {
            CString encoded;
            encoded.Format(_T("%%%02X"), static_cast<unsigned char>(c));
            result += encoded;
        }
    }
    
    return result;
}

CString CTokenManager::GetCurrentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    CString timestampStr;
    timestampStr.Format(_T("%lld"), timestamp);
    return timestampStr;
}

