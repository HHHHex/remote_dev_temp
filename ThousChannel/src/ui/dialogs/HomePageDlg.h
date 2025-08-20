#pragma once

#include "afxdialogex.h"
#include "resource.h"
#include "../../core/TokenManager.h"
#include <string>
#include <vector>

// 加入频道参数结构
struct ChannelJoinParams {
	std::string appId;           // AppID
	std::string appCertificate;  // App证书
	std::string channelId;       // 频道ID
	std::string userId;          // 用户ID
	std::string audioPullMode;   // 音频拉流方式
	std::string token;           // RTC Token
	bool enableCamera;           // 是否开启摄像头
	bool enableMic;              // 是否开启麦克风
};

// Token结果结构
struct TokenResult {
	std::string token;
	bool success;
	std::string errorMsg;
};

// AppID和证书配对结构
struct AppIdInfo {
	std::string appId;
	std::string certificate;
	std::string displayName;  // 显示名称
};

// 自定义消息
#define WM_TOKEN_GENERATED (WM_USER + 200)

// 主页对话框类
class CHomePageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CHomePageDlg)

public:
	CHomePageDlg(CWnd* pParent = nullptr);   
	virtual ~CHomePageDlg();

	// 获取加入参数
	const ChannelJoinParams& GetJoinParams() const { return m_joinParams; }

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JOIN_CHANNEL_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

private:
	// 控件变量
	CComboBox m_comboAppId;
	// 证书输入框已移除 - CEdit m_editAppCertificate;
	CEdit m_editChannelId;
	// 用户名输入框已移除 - CEdit m_editUserName;
	CComboBox m_comboAudioPull;
	CButton m_btnJoinChannel;
	CButton m_checkEnableCamera;
	CButton m_checkEnableMic;
	CStatic m_staticTokenStatus;

	// 数据成员
	ChannelJoinParams m_joinParams;
	CTokenManager* m_tokenManager;
	bool m_isGeneratingToken;
	
	// AppID数据源
	static const std::vector<AppIdInfo> m_appIdList;

	// 初始化控件
	void InitializeControls();
	
	// 输入验证
	bool ValidateInput();

	// Token生成相关
	void GenerateToken();
	std::string GenerateRandomUserId();
	void OnTokenGenerated(const std::string& token, bool success, const std::string& errorMsg);
	void UpdateTokenStatus(const std::string& status, bool isError = false);

	// 消息处理函数
	afx_msg void OnBnClickedJoinChannel();
	afx_msg void OnEnChangeChannelId();
	// 用户名输入框已移除 - afx_msg void OnEnChangeUsername();
	afx_msg void OnCbnSelchangeAppid();
	afx_msg void OnCbnSelchangeAudioPull();
	// 证书输入框已移除 - afx_msg void OnEnChangeAppCertificate();
	afx_msg LRESULT OnTokenGenerated(WPARAM wParam, LPARAM lParam);
};

