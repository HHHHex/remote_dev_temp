// ThousChannel.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "ThousChannel.h"
#include "MainFrm.h"
#include "Logger.h"

#include "ChildFrm.h"
#include "ThousChannelDoc.h"
#include "ThousChannelView.h"
#include "HomePageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CThousChannelApp

BEGIN_MESSAGE_MAP(CThousChannelApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CThousChannelApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CThousChannelApp 构造

CThousChannelApp::CThousChannelApp() noexcept
{
	m_bHiColorIcons = TRUE;


	m_nAppLook = 0;
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，"重新启动管理器"支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("ThousChannel.AppID.NoVersion"));

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CThousChannelApp 对象

CThousChannelApp theApp;


// CThousChannelApp 初始化

BOOL CThousChannelApp::InitInstance()
{
	// 初始化现代日志系统
	auto& logger = Logger::instance();
	logger.setLogLevel(LogLevel::Debug);  // 开发时使用Debug级别
	logger.setLogFile("logs/ThousChannel.log");

	LOG_INFO("ThousChannel application starting...");

	// 初始化日志系统
	LOG_INFO("Application initialization started");

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	LOG_INFO("Common controls initialized");

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		LOG_ERROR("OLE initialization failed");
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	LOG_INFO("OLE library initialized successfully");

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)

	LOG_INFO("Registry and profile settings loaded");

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	LOG_INFO("UI managers initialized");

	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_ThousChannelTYPE,
		RUNTIME_CLASS(CThousChannelDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CThousChannelView));
	if (!pDocTemplate)
	{
		LOG_ERROR("Failed to create document template");
		return FALSE;
	}
	AddDocTemplate(pDocTemplate);

	LOG_INFO("Document template created and added");

	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		LOG_ERROR("Failed to create main frame window");
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	LOG_INFO("Main frame window created successfully");

	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	LOG_INFO_FMT("Command line parsed: {}", std::string(CW2A(cmdInfo.m_strFileName, CP_UTF8)));

	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
	{
		LOG_ERROR("Failed to process shell command");
		return FALSE;
	}

	LOG_INFO("Shell command processed successfully");

	// 显示主页对话框
	LOG_INFO("Showing home page dialog");
	CHomePageDlg homeDlg;
	if (homeDlg.DoModal() != IDOK)
	{
		// 用户取消了加入频道，退出应用程序
		LOG_INFO("User cancelled home page dialog, exiting application");
		return FALSE;
	}

	// 获取用户输入的参数
	ChannelJoinParams joinParams = homeDlg.GetJoinParams();
	LOG_INFO_FMT("User joined channel: AppID={}, ChannelID={}", 
		joinParams.appId, joinParams.channelId);
	
	// TODO: 这里可以将参数传递给频道页面
	// 现在先显示主窗口作为占位
	
	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	LOG_INFO("Application initialization completed successfully");
	return TRUE;
}

int CThousChannelApp::ExitInstance()
{
	LOG_INFO("Application exit started");
	
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	LOG_INFO("Application exit completed");
	return CWinAppEx::ExitInstance();
}

// CThousChannelApp 消息处理程序


// 用于应用程序"关于"菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CThousChannelApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CThousChannelApp 自定义加载/保存方法

void CThousChannelApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CThousChannelApp::LoadCustomState()
{
}

void CThousChannelApp::SaveCustomState()
{
}

// CThousChannelApp 消息处理程序




