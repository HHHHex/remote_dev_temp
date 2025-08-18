#include "pch.h"
#include "ThousChannel.h"
#include "HomePageDlg.h"
#include "ChannelPageDlg.h"
#include "Logger.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// AppID数据源定义
const AppIdInfo CHomePageDlg::m_appIdList[] = {
	{ _T("38fdab08081e4dd7975aa430b35690ab"), _T("c2fab1d528db43cdb52482490bf2e1b2"), _T("38fdab***90ab") }
};

const int CHomePageDlg::m_appIdCount = 1;

// CHomePageDlg 对话框

IMPLEMENT_DYNAMIC(CHomePageDlg, CDialogEx)

CHomePageDlg::CHomePageDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_JOIN_CHANNEL_DLG, pParent)
	, m_tokenManager(nullptr)
	, m_isGeneratingToken(FALSE)
{
	// 初始化参数
	m_joinParams.appId = _T("");
	m_joinParams.appCertificate = _T("");
	m_joinParams.channelId = _T("");
	m_joinParams.audioPullMode = _T("");
	m_joinParams.token = _T("");
	m_joinParams.enableCamera = TRUE;  // 默认开启摄像头
	m_joinParams.enableMic = TRUE;     // 默认开启麦克风
}

CHomePageDlg::~CHomePageDlg()
{
	if (m_tokenManager) {
		delete m_tokenManager;
		m_tokenManager = nullptr;
	}
}

void CHomePageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
	// 使用更简单和安全的方式绑定控件
	try {
		DDX_Control(pDX, IDC_COMBO_APPID, m_comboAppId);
		// 证书输入框已移除 - DDX_Control(pDX, IDC_EDIT_APP_CERTIFICATE, m_editAppCertificate);
		DDX_Control(pDX, IDC_EDIT_CHANNEL_ID, m_editChannelId);
		// 用户名输入框已移除 - DDX_Control(pDX, IDC_EDIT_USERNAME, m_editUserName);
		DDX_Control(pDX, IDC_COMBO_AUDIO_PULL, m_comboAudioPull);
		DDX_Control(pDX, IDC_BTN_JOIN_CHANNEL, m_btnJoinChannel);
		DDX_Control(pDX, IDC_CHECK_ENABLE_CAMERA, m_checkEnableCamera);
		DDX_Control(pDX, IDC_CHECK_ENABLE_MIC, m_checkEnableMic);
		// 暂时跳过静态控件绑定
		// DDX_Control(pDX, IDC_STATIC_TOKEN_STATUS, m_staticTokenStatus);
	}
	catch (...) {
		// 如果DDX绑定失败，记录错误但继续执行
		TRACE(_T("Warning: Some controls failed to bind in DoDataExchange\n"));
	}
}

BEGIN_MESSAGE_MAP(CHomePageDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_JOIN_CHANNEL, &CHomePageDlg::OnBnClickedJoinChannel)
	ON_EN_CHANGE(IDC_EDIT_CHANNEL_ID, &CHomePageDlg::OnEnChangeChannelId)
	// 用户名输入框已移除 - ON_EN_CHANGE(IDC_EDIT_USERNAME, &CHomePageDlg::OnEnChangeUsername)
	ON_CBN_SELCHANGE(IDC_COMBO_APPID, &CHomePageDlg::OnCbnSelchangeAppid)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_PULL, &CHomePageDlg::OnCbnSelchangeAudioPull)
	// 证书输入框已移除 - ON_EN_CHANGE(IDC_EDIT_APP_CERTIFICATE, &CHomePageDlg::OnEnChangeAppCertificate)
	ON_MESSAGE(WM_TOKEN_GENERATED, &CHomePageDlg::OnTokenGenerated)
END_MESSAGE_MAP()

// CHomePageDlg 消息处理程序

BOOL CHomePageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	LOG_INFO("Home page dialog initialized");

	try {
		// 设置对话框标题
		CString strTitle;
		strTitle.LoadString(IDS_DLG_TITLE);
		SetWindowText(strTitle);

		// 创建Token管理器
		m_tokenManager = new CTokenManager();

		// 初始化控件
		InitializeControls();

		LOG_INFO("Home page dialog controls initialized");
	}
	catch (...) {
		LOG_ERROR("Failed to initialize dialog");
		// 继续执行，让对话框至少能显示
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHomePageDlg::InitializeControls()
{
	try {
		CString strText;
		
		// 初始化AppID下拉框 - 直接添加AppID项目，不需要提示项
		for (int i = 0; i < m_appIdCount; i++) {
			m_comboAppId.AddString(m_appIdList[i].displayName);
		}
		
		// 默认选择第一个AppID（索引0）
		if (m_appIdCount > 0) {
			m_comboAppId.SetCurSel(0);
		}

		// 初始化音频拉流方式下拉框
		strText.LoadString(IDS_SELECT_AUDIO_PULL);
		m_comboAudioPull.AddString(strText);
		strText.LoadString(IDS_AUDIO_PULL_UID);
		m_comboAudioPull.AddString(strText);
		strText.LoadString(IDS_AUDIO_PULL_TOPN);
		m_comboAudioPull.AddString(strText);
		m_comboAudioPull.SetCurSel(1);  // 选择第一个实际的音频拉流方式（索引1）

		// 设置复选框默认状态
		m_checkEnableCamera.SetCheck(BST_CHECKED);
		m_checkEnableMic.SetCheck(BST_CHECKED);

		// 设置按钮文本
		strText.LoadString(IDS_JOIN_CHANNEL);
		m_btnJoinChannel.SetWindowText(strText);

		// 初始化Token状态显示
		UpdateTokenStatus(_T("请填写完整信息后生成Token"), FALSE);
	}
	catch (...) {
		// 如果初始化失败，至少设置基本状态
		m_comboAppId.AddString(_T("选择AppID"));
		m_comboAudioPull.AddString(_T("选择音频拉流方式"));
		m_btnJoinChannel.SetWindowText(_T("加入频道"));
	}
}

BOOL CHomePageDlg::ValidateInput()
{
	CString strMessage;
	
	// 检查AppID选择
	int appIdSel = m_comboAppId.GetCurSel();
	if (appIdSel < 0)
	{
		strMessage.LoadString(IDS_MSG_SELECT_APPID);
		WARNING("User tried to join without selecting AppID");
		AfxMessageBox(strMessage);
		m_comboAppId.SetFocus();
		return FALSE;
	}

	// App证书不需要用户输入，直接从配置中获取

	// 检查频道ID输入
	CString channelId;
	m_editChannelId.GetWindowText(channelId);
	channelId.Trim();
	if (channelId.IsEmpty())
	{
		strMessage.LoadString(IDS_MSG_ENTER_CHANNEL);
		WARNING("User tried to join without entering channel ID");
		AfxMessageBox(strMessage);
		m_editChannelId.SetFocus();
		return FALSE;
	}

	// 用户名检查已移除 - 不再需要用户名输入

	// 检查音频拉流方式选择
	int audioPullSel = m_comboAudioPull.GetCurSel();
	if (audioPullSel <= 0)
	{
		strMessage.LoadString(IDS_MSG_SELECT_AUDIO);
		WARNING("User tried to join without selecting audio pull mode");
		AfxMessageBox(strMessage);
		m_comboAudioPull.SetFocus();
		return FALSE;
	}

	LOG_INFO("Input validation passed");
	return TRUE;
}

void CHomePageDlg::GenerateToken()
{
	if (m_isGeneratingToken) {
		WARNING("Token generation already in progress");
		return;
	}

	// 收集用户输入
	CString temp;
	
	// 获取AppID和证书
	int appIdSel = m_comboAppId.GetCurSel();
	if (appIdSel >= 0 && appIdSel < m_appIdCount) {
		m_joinParams.appId = m_appIdList[appIdSel].appId;  // 使用真实的AppID
		m_joinParams.appCertificate = m_appIdList[appIdSel].certificate;  // 使用对应的证书
	} else {
		m_joinParams.appId = _T("");
		m_joinParams.appCertificate = _T("");
	}

	// 获取频道ID
	m_editChannelId.GetWindowText(temp);
	m_joinParams.channelId = temp.Trim();

	// 生成随机用户ID
	m_joinParams.userId = GenerateRandomUserId();

	// 获取音频拉流方式
	int audioPullSel = m_comboAudioPull.GetCurSel();
	m_comboAudioPull.GetLBText(audioPullSel, temp);
	m_joinParams.audioPullMode = temp;

	// 获取复选框状态
	m_joinParams.enableCamera = (m_checkEnableCamera.GetCheck() == BST_CHECKED);
	m_joinParams.enableMic = (m_checkEnableMic.GetCheck() == BST_CHECKED);

	// 记录日志
	LOG_INFO_FMT("Join params collected. Channel: %s, UserID: %s, Camera: %d, Mic: %d"), 
		m_joinParams.channelId, m_joinParams.userId, m_joinParams.enableCamera, m_joinParams.enableMic);

	// 构建Token生成参数
	TokenGenerateParams tokenParams;
	tokenParams.appId = m_joinParams.appId;
	tokenParams.appCertificate = m_joinParams.appCertificate;
	tokenParams.channelName = m_joinParams.channelId;
	tokenParams.userId = m_joinParams.userId;
	tokenParams.expire = 24 * 60 * 60 ;  // 24小时过期
	tokenParams.type = 1;      // RTC Token
	tokenParams.src = _T("Windows");

	LOG_INFO_FMT("Generating token for AppID=%s, Channel=%s, UserID=%s"), 
		tokenParams.appId, tokenParams.channelName, tokenParams.userId);

	// 更新状态
	m_isGeneratingToken = TRUE;
	UpdateTokenStatus(_T("正在生成Token..."), FALSE);
	m_btnJoinChannel.EnableWindow(FALSE);

	// 异步生成Token
	m_tokenManager->GenerateTokenAsync(tokenParams, [this](const CString& token, bool success, const CString& errorMsg) {
		// 通过消息机制在主线程中处理回调
		TokenResult* pResult = new TokenResult();
		pResult->token = token;
		pResult->success = success;
		pResult->errorMsg = errorMsg;
		
		::PostMessage(this->GetSafeHwnd(), WM_TOKEN_GENERATED, 0, (LPARAM)pResult);
	});
}

void CHomePageDlg::OnTokenGenerated(const CString& token, bool success, const CString& errorMsg)
{
	m_isGeneratingToken = FALSE;
	m_btnJoinChannel.EnableWindow(TRUE);

	if (success && !token.IsEmpty()) {
		m_joinParams.token = token;
		UpdateTokenStatus(_T("Token生成成功，可以加入频道"), FALSE);
		
		        LOG_INFO_FMT("Token generated successfully: %s", token.Left(20) + _T("..."));
		
		// 自动跳转到频道页面
		LOG_INFO("Creating channel page dialog");
		CChannelPageDlg channelPageDlg(m_joinParams, this);
		
		// 隐藏当前对话框
		ShowWindow(SW_HIDE);
		
		// 显示频道页面
		INT_PTR result = channelPageDlg.DoModal();
		
		// 根据频道页面的返回结果处理
		if (result == IDOK) {
			// 用户从频道页面正常退出，关闭整个应用
			LOG_INFO("User exited from channel page, closing application");
			EndDialog(IDCANCEL); // 通知主程序退出
		} else {
			// 用户从频道页面返回，重新显示加入频道对话框
			LOG_INFO("User returned from channel page, showing join dialog again");
			ShowWindow(SW_SHOW);
			// 清空token，要求重新生成
			m_joinParams.token = _T("");
			UpdateTokenStatus(_T("请重新生成Token"), FALSE);
		}
	} else {
		UpdateTokenStatus(_T("Token生成失败: ") + errorMsg, TRUE);
		LOG_ERROR_FMT("Token generation failed: %s", errorMsg);
		AfxMessageBox(_T("Token生成失败: ") + errorMsg);
	}
}

void CHomePageDlg::UpdateTokenStatus(const CString& status, BOOL isError)
{
	try {
		// 尝试通过控件ID直接更新文本，避免使用成员变量
		CWnd* pStatusWnd = GetDlgItem(IDC_STATIC_TOKEN_STATUS);
		if (pStatusWnd && pStatusWnd->GetSafeHwnd()) {
			CString displayText = _T("Token状态: ") + status;
			pStatusWnd->SetWindowText(displayText);
		}
		
		// 记录状态到日志
		if (isError) {
			LOG_ERROR_FMT("Token status error: %s", status);
		} else {
			LOG_INFO_FMT("Token status: %s", status);
		}
	}
	catch (...) {
		LOG_ERROR("Failed to update token status display");
		// 即使更新失败也要记录日志
		if (isError) {
			LOG_ERROR_FMT("Token status error: %s", status);
		} else {
			LOG_INFO_FMT("Token status: %s", status);
		}
	}
}

void CHomePageDlg::OnBnClickedJoinChannel()
{
	LOG_INFO("Join channel button clicked");

	// 验证输入
	if (!ValidateInput())
		return;

	// 生成Token并加入频道
	GenerateToken();
}

LRESULT CHomePageDlg::OnTokenGenerated(WPARAM wParam, LPARAM lParam)
{
	TokenResult* pResult = (TokenResult*)lParam;
	if (pResult) {
		OnTokenGenerated(pResult->token, pResult->success, pResult->errorMsg);
		delete pResult;
	}
	
	return 0;
}

void CHomePageDlg::OnEnChangeChannelId()
{
	// 频道ID输入变化时的处理
	DEBUG("Channel ID input changed");
	// 清空token状态
	if (!m_joinParams.token.IsEmpty()) {
		m_joinParams.token = _T("");
		UpdateTokenStatus(_T("频道信息已更改，需要重新生成Token"), FALSE);
	}
}

void CHomePageDlg::OnCbnSelchangeAppid()
{
	// AppID选择变化时的处理
	DEBUG("AppID selection changed");
	
	// 获取当前选择的索引
	int sel = m_comboAppId.GetCurSel();
	if (sel >= 0 && sel < m_appIdCount) {
		LOG_INFO_FMT("AppID selected: %s", m_appIdList[sel].displayName);
	}
	
	// 清空token状态
	if (!m_joinParams.token.IsEmpty()) {
		m_joinParams.token = _T("");
		UpdateTokenStatus(_T("AppID已更改，需要重新生成Token"), FALSE);
	}
}

void CHomePageDlg::OnCbnSelchangeAudioPull()
{
	// 音频拉流方式选择变化时的处理
	DEBUG("Audio pull mode selection changed");
}

// 证书输入框已移除，不再需要此函数
// void CHomePageDlg::OnEnChangeAppCertificate() - 已删除

void CHomePageDlg::OnOK()
{
	LOG_INFO("Home page dialog OK button pressed");
	OnBnClickedJoinChannel();
}

CString CHomePageDlg::GenerateRandomUserId()
{
	// 获取当前时间戳（毫秒级）
	SYSTEMTIME st;
	GetSystemTime(&st);
	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);
	ULARGE_INTEGER uli;
	uli.LowPart = ft.dwLowDateTime;
	uli.HighPart = ft.dwHighDateTime;
	
	// 获取毫秒时间戳（从1601年开始的100纳秒间隔数）
	ULONGLONG timestamp = uli.QuadPart;
	
	// 生成一个3位的随机数 (100-999)
	srand((unsigned int)time(NULL));
	int random = 100 + (rand() % 900);
	
	// 使用3位随机数和时间戳的后4位组合
	// 格式：YYYXXXX，其中YYY是3位随机数, XXXX是时间戳后4位
	CString userId;
	userId.Format(_T("%03d%04d"), random, (int)(timestamp % 10000));
	
	return userId;
}

