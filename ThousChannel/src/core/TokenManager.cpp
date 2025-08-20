#include "pch.h"
#include "TokenManager.h"
#include "Logger.h"
#include <winhttp.h>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>

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

bool CTokenManager::GenerateTokenSync(const TokenGenerateParams& params, std::string& token, std::string& errorMsg)
{
    // 构建请求数据
    std::string jsonData = BuildRequestJson(params);
    if (jsonData.empty()) {
        errorMsg = "构建请求数据失败";
        return false;
    }

    // 执行HTTP请求
    std::string response;
    if (!ExecuteHttpRequest("https://service.agora.io/toolbox-global/v1/token/generate", 
                           jsonData, response, errorMsg)) {
        return false;
    }

    // 解析响应
    return ParseResponseJson(response, token, errorMsg);
}

std::string CTokenManager::BuildRequestJson(const TokenGenerateParams& params)
{
    try {
        std::string timestamp = GetCurrentTimestamp();
        
        // 使用现代C++字符串格式化
        std::ostringstream json;
        json << "{"
             << "\"appId\":\"" << params.appId << "\","
             << "\"appCertificate\":\"" << params.appCertificate << "\","
             << "\"channelName\":\"" << params.channelName << "\","
             << "\"uid\":\"" << params.userId << "\","
             << "\"expire\":" << params.expire << ","
             << "\"type\":" << params.type << ","
             << "\"src\":\"" << params.src << "\","
             << "\"ts\":\"" << timestamp << "\""
             << "}";
        
        return json.str();
    }
    catch (const std::exception& e) {
        LOG_ERROR("Failed to build JSON: {}", e.what());
        return "";
    }
}

bool CTokenManager::ParseResponseJson(const std::string& jsonResponse, std::string& token, std::string& errorMsg)
{
    try {
        // 简单的JSON解析，查找关键字段
        std::string response = jsonResponse;
        
        // 查找code字段
        size_t codePos = response.find("\"code\":");
        if (codePos != std::string::npos) {
            size_t codeStart = codePos + 7; // "code": 的长度
            size_t codeEnd = response.find(",", codeStart);
            if (codeEnd == std::string::npos) codeEnd = response.find("}", codeStart);
            
            if (codeEnd != std::string::npos) {
                std::string codeStr = response.substr(codeStart, codeEnd - codeStart);
                // 移除空白字符
                codeStr.erase(0, codeStr.find_first_not_of(" \t\r\n"));
                codeStr.erase(codeStr.find_last_not_of(" \t\r\n") + 1);
                
                int code = std::stoi(codeStr);
                
                // 输出服务器返回内容用于调试
                LOG_INFO("服务器返回内容: {}", response);
                
                if (code != 0) {
                    // 查找msg字段
                    size_t msgPos = response.find("\"msg\":");
                    if (msgPos != std::string::npos) {
                        size_t msgStart = response.find("\"", msgPos + 6) + 1; // "msg": 的长度
                        size_t msgEnd = response.find("\"", msgStart);
                        if (msgEnd != std::string::npos) {
                            std::string message = response.substr(msgStart, msgEnd - msgStart);
                            errorMsg = "服务器返回错误(code=" + std::to_string(code) + "): " + message;
                        } else {
                            errorMsg = "服务器返回错误(code=" + std::to_string(code) + ")";
                        }
                    } else {
                        errorMsg = "服务器返回错误(code=" + std::to_string(code) + ")";
                    }
                    return false;
                }
            }
        }
        
        // 查找data字段中的token
        size_t dataPos = response.find("\"data\":");
        if (dataPos != std::string::npos) {
            // 在data字段中查找token
            size_t tokenPos = response.find("\"token\":", dataPos);
            if (tokenPos != std::string::npos) {
                size_t tokenStart = response.find("\"", tokenPos + 8) + 1; // "token": 的长度
                size_t tokenEnd = response.find("\"", tokenStart);
                
                if (tokenEnd != std::string::npos) {
                    token = response.substr(tokenStart, tokenEnd - tokenStart);
                    return !token.empty();
                }
            }
        }
        
        errorMsg = "响应中未找到data.token字段";
        return false;
    }
    catch (const std::exception& e) {
        errorMsg = "解析响应异常: " + std::string(e.what());
        return false;
    }
}

bool CTokenManager::ExecuteHttpRequest(const std::string& url, const std::string& postData, 
                                      std::string& response, std::string& errorMsg)
{
    HINTERNET hSession = nullptr;
    HINTERNET hConnect = nullptr;
    HINTERNET hRequest = nullptr;
    bool success = false;

    try {
        // 解析URL - 使用宽字符串进行WinHTTP调用
        URL_COMPONENTS urlComp = { 0 };
        urlComp.dwStructSize = sizeof(urlComp);
        urlComp.dwSchemeLength = -1;
        urlComp.dwHostNameLength = -1;
        urlComp.dwUrlPathLength = -1;
        urlComp.dwExtraInfoLength = -1;

        // 转换std::string到宽字符串
        std::wstring urlWide(url.begin(), url.end());
        if (!WinHttpCrackUrl(urlWide.c_str(), urlWide.length(), 0, &urlComp)) {
            errorMsg = "解析URL失败";
            return false;
        }

        std::wstring hostNameWide(urlComp.lpszHostName, urlComp.dwHostNameLength);
        std::wstring urlPathWide(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);

        // 创建会话
        hSession = WinHttpOpen(L"ThousChannel/1.0",
                              WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                              WINHTTP_NO_PROXY_NAME,
                              WINHTTP_NO_PROXY_BYPASS,
                              0);
        if (!hSession) {
            errorMsg = "创建HTTP会话失败";
            return false;
        }

        // 设置超时
        WinHttpSetTimeouts(hSession, m_timeoutMs, m_timeoutMs, m_timeoutMs, m_timeoutMs);

        // 连接服务器
        hConnect = WinHttpConnect(hSession, hostNameWide.c_str(), 
                                 urlComp.nScheme == INTERNET_SCHEME_HTTPS ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT,
                                 0);
        if (!hConnect) {
            errorMsg = "连接服务器失败";
            return false;
        }

        // 创建请求
        DWORD flags = urlComp.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
        hRequest = WinHttpOpenRequest(hConnect, L"POST", urlPathWide.c_str(), nullptr,
                                     WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
        if (!hRequest) {
            errorMsg = "创建HTTP请求失败";
            return false;
        }

        // 设置请求头
        std::wstring headers = L"Content-Type: application/json\r\n";
        WinHttpAddRequestHeaders(hRequest, headers.c_str(), headers.length(), WINHTTP_ADDREQ_FLAG_ADD);

        // 发送请求 - 直接使用std::string
        if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                               (LPVOID)postData.c_str(), (DWORD)postData.length(),
                               (DWORD)postData.length(), 0)) {
            errorMsg = "发送HTTP请求失败";
            return false;
        }

        // 接收响应
        if (!WinHttpReceiveResponse(hRequest, nullptr)) {
            errorMsg = "接收HTTP响应失败";
            return false;
        }

        // 检查状态码
        DWORD statusCode = 0;
        DWORD statusCodeSize = sizeof(statusCode);
        WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                           WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusCodeSize, WINHTTP_NO_HEADER_INDEX);

        if (statusCode != 200) {
            errorMsg = "HTTP请求失败，状态码: " + std::to_string(statusCode);
            return false;
        }

        // 读取响应数据
        DWORD availableBytes = 0;
        std::string responseData;
        
        do {
            availableBytes = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &availableBytes)) {
                errorMsg = "查询可用数据失败";
                return false;
            }

            if (availableBytes > 0) {
                std::vector<char> buffer(availableBytes + 1);
                DWORD bytesRead = 0;
                
                if (WinHttpReadData(hRequest, buffer.data(), availableBytes, &bytesRead)) {
                    buffer[bytesRead] = '\0';
                    responseData.append(buffer.data(), bytesRead);
                } else {
                    errorMsg = "读取响应数据失败";
                    return false;
                }
            }
        } while (availableBytes > 0);

        response = responseData;
        success = true;
    }
    catch (const std::exception& e) {
        errorMsg = "HTTP请求异常: " + std::string(e.what());
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

    std::string token, errorMsg;
    bool success = pManager->GenerateTokenSync(params, token, errorMsg);
    
    // 在主线程中调用回调（需要通过消息机制）
    callback(token, success, errorMsg);
}

std::string CTokenManager::UrlEncode(const std::string& text)
{
    std::ostringstream result;
    
    for (char c : text) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            result << c;
        } else {
            result << '%' << std::hex << std::setw(2) << std::setfill('0') 
                   << static_cast<unsigned char>(c);
        }
    }
    
    return result.str();
}

std::string CTokenManager::GetCurrentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    return std::to_string(timestamp);
}

