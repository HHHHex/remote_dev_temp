#include "pch.h"
#include "ThousChannel.h"
#include "ChannelPageDlg.h"
#include "Logger.h"
#include "RteManager.h"
#include "afxdialogex.h"
#include <algorithm>
#include <string>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CChannelPageDlg, CDialogEx)

BEGIN_MESSAGE_MAP(CChannelPageDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_EXIT_CHANNEL, &CChannelPageDlg::OnBnClickedExitChannel)
    ON_CBN_SELCHANGE(IDC_COMBO_GRID_MODE, &CChannelPageDlg::OnCbnSelchangeGridMode)
    ON_BN_CLICKED(IDC_BTN_PREV_PAGE, &CChannelPageDlg::OnBnClickedPrevPage)
    ON_BN_CLICKED(IDC_BTN_NEXT_PAGE, &CChannelPageDlg::OnBnClickedNextPage)
    ON_WM_SIZE()
    ON_MESSAGE(WM_USER_RTE_JOIN_CHANNEL_SUCCESS, &CChannelPageDlg::OnRteJoinChannelSuccess)
    ON_MESSAGE(WM_USER_RTE_USER_JOINED, &CChannelPageDlg::OnRteUserJoined)
    ON_MESSAGE(WM_USER_RTE_USER_LEFT, &CChannelPageDlg::OnRteUserLeft)
    ON_MESSAGE(WM_USER_RTE_REMOTE_VIDEO_STATE_CHANGED, &CChannelPageDlg::OnRteRemoteVideoStateChanged)
    ON_MESSAGE(WM_USER_RTE_LOCAL_VIDEO_STATE_CHANGED, &CChannelPageDlg::OnRteLocalVideoStateChanged)
    ON_MESSAGE(WM_USER_RTE_ERROR, &CChannelPageDlg::OnRteError)
    ON_MESSAGE(WM_USER_RTE_USER_LIST_CHANGED, &CChannelPageDlg::OnRteUserListChanged)
    ON_MESSAGE(WM_USER_RTE_REMOTE_AUDIO_STATE_CHANGED, &CChannelPageDlg::OnRteRemoteAudioStateChanged)
    ON_MESSAGE(WM_USER_RTE_LOCAL_AUDIO_STATE_CHANGED, &CChannelPageDlg::OnRteLocalAudioStateChanged)
END_MESSAGE_MAP()

//===========================================================================
// CChannelPageDlg Constructor & Destructor
//===========================================================================

CChannelPageDlg::CChannelPageDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CHANNEL_PAGE_DLG, pParent)
{
    m_pageState.currentGridMode = 2;
    m_pageState.currentPage = 1;
    m_pageState.usersPerPage = 4;
    m_rteManager = nullptr;
    m_isChannelJoined = false;
}

CChannelPageDlg::CChannelPageDlg(const ChannelJoinParams& joinParams, CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CHANNEL_PAGE_DLG, pParent), m_joinParams(joinParams)
{
    m_pageState.channelId = joinParams.channelId;
    m_pageState.currentUserId = joinParams.userId;
    m_pageState.audioMode = joinParams.audioPullMode;
    m_pageState.isLocalVideoEnabled = joinParams.enableCamera;
    m_pageState.isLocalAudioEnabled = joinParams.enableMic;
    m_pageState.currentGridMode = 2;
    m_pageState.currentPage = 1;
    m_pageState.usersPerPage = 4;
    m_rteManager = nullptr;
    m_isChannelJoined = false;

    // Create placeholder users for grid display
    int maxUsers = 32; // Assuming a max of 32 users for grid display
    for (int i = 1; i < maxUsers; i++) {
        ChannelUser* placeholderUser = new ChannelUser();
        placeholderUser->userId = "-1";
        placeholderUser->isLocal = false;
        placeholderUser->isConnected = false;
        placeholderUser->isRobot = false;
        m_pageState.userList.Add(placeholderUser);
    }
}

CChannelPageDlg::~CChannelPageDlg()
{
    LOG_INFO("Channel page dialog destroyed");
    
    LeaveRteChannel();
    ReleaseRteEngine();
    DestroyVideoWindows();
    
    // Clean up user list
    for (int i = 0; i < m_pageState.userList.GetSize(); i++) {
        delete m_pageState.userList[i];
    }
    m_pageState.userList.RemoveAll();
}


//===========================================================================
// MFC Overrides
//===========================================================================

void CChannelPageDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_CHANNEL_ID_PAGE, m_staticChannelId);
    DDX_Control(pDX, IDC_BTN_EXIT_CHANNEL, m_btnExitChannel);
    DDX_Control(pDX, IDC_STATIC_VIDEO_CONTAINER, m_staticVideoContainer);
    DDX_Control(pDX, IDC_COMBO_GRID_MODE, m_comboGridMode);
    DDX_Control(pDX, IDC_BTN_PREV_PAGE, m_btnPrevPage);
    DDX_Control(pDX, IDC_BTN_NEXT_PAGE, m_btnNextPage);
    DDX_Control(pDX, IDC_STATIC_CURRENT_PAGE, m_staticCurrentPage);
}

BOOL CChannelPageDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    	LOG_INFO_FMT("Channel page dialog initialized for channel: {}", m_pageState.channelId);

    InitializeFonts();
    InitializeControls();
    
    // SetupVideoContainer(); // This function was removed during refactoring.
    CreateVideoWindows();
    UpdateGridLayout();

    if (!InitializeRteEngine()) {
        LOG_ERROR("Failed to initialize RTE engine");
        AfxMessageBox(_T("Failed to initialize RTE engine."));
        EndDialog(IDCANCEL);
        return FALSE;
    }
    
    // Create local user data
    ChannelUser* localUser = new ChannelUser();
    localUser->isLocal = true;
    localUser->isConnected = true;
    localUser->userId = m_pageState.currentUserId;
    m_pageState.userList.InsertAt(0, localUser);
    
    if (!JoinRteChannel()) {
        LOG_ERROR("Failed to join RTE channel");
        return FALSE;
    }
    
    // Enable local audio/video after successful channel join
    if (m_rteManager) {
        m_rteManager->SetLocalAudioCaptureEnabled(m_joinParams.enableMic);
        m_rteManager->SetLocalVideoCaptureEnabled(m_joinParams.enableCamera);
    }

    return TRUE;
}

void CChannelPageDlg::OnOK()
{
    // Override to prevent closing with Enter key
}

void CChannelPageDlg::OnCancel()
{
    // Handle closing via Esc key or title bar close button
    EndDialog(IDCANCEL);
}


//===========================================================================
// UI Event Handlers
//===========================================================================

void CChannelPageDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);
    UpdateGridLayout();
}

void CChannelPageDlg::OnBnClickedExitChannel()
{
    if (AfxMessageBox(_T("Are you sure you want to exit the channel?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        EndDialog(IDCANCEL);
    }
}

void CChannelPageDlg::OnCbnSelchangeGridMode()
{
    int sel = static_cast<int>(m_comboGridMode.GetCurSel());
    int newGridMode = 2 + sel;
    if (sel == 4) newGridMode = 7;
    
    SetGridMode(newGridMode);
}

void CChannelPageDlg::OnBnClickedPrevPage()
{
    if (m_pageState.currentPage > 1)
    {
        m_pageState.currentPage--;
        UpdateVideoLayout();
        UpdatePageDisplay();
        UpdateSubscribedUsers();
        UpdateViewUserBindings();
    }
}

void CChannelPageDlg::OnBnClickedNextPage()
{
    if (m_pageState.currentPage < GetMaxPages())
    {
        m_pageState.currentPage++;
        UpdateVideoLayout();
        UpdatePageDisplay();
        UpdateSubscribedUsers();
        UpdateViewUserBindings();
    }
}


//===========================================================================
// RTE Callback Handlers
//===========================================================================

// IRteManagerEventHandler implementation
void CChannelPageDlg::OnConnectionStateChanged(int state)
{
    // Post message to UI thread
    PostMessage(WM_USER_RTE_JOIN_CHANNEL_SUCCESS, 0, 0);
}

void CChannelPageDlg::OnUserJoined(const std::string& userId)
{
    // Convert std::string to CString and post it
    CString* uidStr = new CString(userId.c_str());
    PostMessage(WM_USER_RTE_USER_JOINED, (WPARAM)uidStr, 0);
}

void CChannelPageDlg::OnUserLeft(const std::string& userId)
{
    // Convert std::string to CString and post it
    CString* uidStr = new CString(userId.c_str());
    PostMessage(WM_USER_RTE_USER_LEFT, (WPARAM)uidStr, 0);
}

void CChannelPageDlg::OnLocalAudioStateChanged(int state)
{
    // Post message to UI thread
    PostMessage(WM_USER_RTE_LOCAL_AUDIO_STATE_CHANGED, state, 0);
}

void CChannelPageDlg::OnLocalVideoStateChanged(int state, int reason)
{
    // Post message to UI thread
    PostMessage(WM_USER_RTE_LOCAL_VIDEO_STATE_CHANGED, MAKEWPARAM(state, reason), 0);
}

void CChannelPageDlg::OnRemoteAudioStateChanged(const std::string& userId, int state)
{
    // Convert std::string to CString and post it
    CString* uidStr = new CString(userId.c_str());
    PostMessage(WM_USER_RTE_REMOTE_AUDIO_STATE_CHANGED, (WPARAM)uidStr, state);
}

void CChannelPageDlg::OnRemoteVideoStateChanged(const std::string& userId, int state)
{
    // Convert std::string to CString and post it
    CString* uidStr = new CString(userId.c_str());
    PostMessage(WM_USER_RTE_REMOTE_VIDEO_STATE_CHANGED, (WPARAM)uidStr, state);
}

void CChannelPageDlg::OnError(int error)
{
    // Post message to UI thread
    PostMessage(WM_USER_RTE_ERROR, error, 0);
}

void CChannelPageDlg::OnUserListChanged()
{
    // Post message to UI thread
    PostMessage(WM_USER_RTE_USER_LIST_CHANGED, 0, 0);
}

LRESULT CChannelPageDlg::OnRteJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
    // Use the real user ID passed from the previous page
    std::string realUserId = m_pageState.currentUserId;
    CString localUserId(realUserId.c_str());

    LOG_INFO_FMT("Join channel success - Using user ID: {}", realUserId);

    if (m_pageState.userList.GetSize() > 0) {
        ChannelUser* localUser = m_pageState.userList[0];
        if (localUser->isLocal) {
            // Update the local user with the real user ID from RTE
            localUser->userId = realUserId;
            LOG_INFO_FMT("Updated local user ID to: {}", realUserId);

            UpdateVideoLayout();

            CString strChannelInfo;
            strChannelInfo.Format(_T("Channel: %s (My UID: %s)"), 
                CString(m_pageState.channelId.c_str()), localUserId);
            m_staticChannelId.SetWindowText(strChannelInfo);
            
            LOG_INFO_FMT("Channel info updated: {}", std::string(CT2A(strChannelInfo)));
        }
    }

    return 0;
}

LRESULT CChannelPageDlg::OnRteUserJoined(WPARAM wParam, LPARAM lParam)
{
    CString* uidPtr = (CString*)wParam;
    CString uid = *uidPtr;
    delete uidPtr;

    LOG_INFO_FMT("User joined: {}", std::string(CT2A(uid)));

    // 1. 用户类型判断
    bool isRobot = (_ttoi(uid) >= 1000);
    std::string userId = std::string(CT2A(uid));

    // 2. 用户存在性检查
    int userIndex = FindUserIndex(uid);
    ChannelUser* targetUser = nullptr;

    if (isRobot) {
        // 机器人用户处理
        if (userIndex != -1) {
            // 已存在 -> 不做任何事
            LOG_INFO_FMT("Robot user {} already exists, skipping", std::string(CT2A(uid)));
            return 0;
        }
        
        // 创建新的机器人用户
        targetUser = new ChannelUser();
        targetUser->userId = userId;
        targetUser->isConnected = true;
        targetUser->isLocal = false;
        targetUser->isRobot = true;
        targetUser->isVideoSubscribed = true;
        targetUser->isAudioSubscribed = true;
        
        m_pageState.userList.Add(targetUser);
        LOG_INFO_FMT("Created new robot user: {}", userId);
        
    } else {
        // 真人用户处理
        if (userIndex != -1) {
            // 更新现有占位符用户
            targetUser = m_pageState.userList[userIndex];
            if (targetUser && !targetUser->isConnected) {
                targetUser->isConnected = true;
                targetUser->userId = userId;
                targetUser->isVideoSubscribed = true;
                targetUser->isAudioSubscribed = true;
                LOG_INFO_FMT("Updated existing placeholder user: {}", userId);
            }
        } else {
            // 占位符不存在，创建新用户（异常情况）
            targetUser = new ChannelUser();
            targetUser->userId = userId;
            targetUser->isConnected = true;
            targetUser->isLocal = false;
            targetUser->isRobot = false;
            targetUser->isVideoSubscribed = true;
            targetUser->isAudioSubscribed = true;
            
            m_pageState.userList.Add(targetUser);
            LOG_WARN_FMT("Created new real user (should be placeholder): {}", userId);
        }
    }

    // 3. 订阅状态更新
    if (m_rteManager && targetUser && !targetUser->isLocal) {
        LOG_INFO_FMT("Subscribing to remote video/audio for user: {}", userId);
        // TODO: 实现真正的订阅逻辑
    }

    // 4. 更新UI状态
    UpdateVideoLayout();
    UpdatePageDisplay();
    UpdateSubscribedUsers();
    UpdateViewUserBindings();

    return 0;
}

LRESULT CChannelPageDlg::OnRteUserLeft(WPARAM wParam, LPARAM lParam)
{
    CString* uidPtr = (CString*)wParam;
    CString uid = *uidPtr;
    delete uidPtr;

    LOG_INFO_FMT("User left: {}", std::string(CT2A(uid)));

    // 1. 用户存在性检查
    int userIndex = FindUserIndex(uid);
    if (userIndex == -1) {
        LOG_WARN_FMT("User {} not found in list, ignoring leave event", std::string(CT2A(uid)));
        return 0;
    }

    // 2. 获取用户信息
    ChannelUser* targetUser = m_pageState.userList[userIndex];
    if (!targetUser) {
        LOG_ERROR_FMT("User object at index {} is null", userIndex);
        return 0;
    }

    // 3. 用户类型判断和处理
    bool isRobot = targetUser->isRobot;
    std::string userId = targetUser->GetUserId();

    if (isRobot) {
        // 机器人用户 -> 从列表中移除
        LOG_INFO_FMT("Removing robot user: {}", userId);
        delete targetUser;
        m_pageState.userList.RemoveAt(userIndex);
        
    } else {
        // 真人用户 -> 设置为离线状态
        LOG_INFO_FMT("Setting real user offline: {}", userId);
        targetUser->isConnected = false;
        targetUser->isVideoSubscribed = false;
        targetUser->isAudioSubscribed = false;
        targetUser->userId = userId + " (Offline)";
    }

    // 4. 订阅状态更新
    if (m_rteManager && !targetUser->isLocal) {
        LOG_INFO_FMT("Unsubscribing from remote video/audio for user: {}", userId);
        // TODO: 实现真正的取消订阅逻辑
    }

    // 5. 更新UI状态
    UpdateVideoLayout();
    UpdatePageDisplay();
    UpdateSubscribedUsers();
    UpdateViewUserBindings();

    return 0;
}

LRESULT CChannelPageDlg::OnRteRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
{
    CString* uidPtr = (CString*)wParam;
    CString uid = *uidPtr;
    delete uidPtr;

    int state = LOWORD(lParam);
    int reason = HIWORD(lParam);
    LOG_INFO_FMT("Remote video state changed for user {}, state={}, reason={}", std::string(CT2A(uid)), state, reason);

    // You might want to update the UI for this user
    // For example, show an icon if their video is disabled

    return 0;
}

LRESULT CChannelPageDlg::OnRteLocalVideoStateChanged(WPARAM wParam, LPARAM lParam)
{
    int state = LOWORD(wParam);
    int reason = HIWORD(wParam);
    LOG_INFO_FMT("Local video state changed, state={}, reason={}", state, reason);

    // You can update the local user's UI based on the state
    // e.g., show a "camera off" icon
    return 0;
}

LRESULT CChannelPageDlg::OnRteRemoteAudioStateChanged(WPARAM wParam, LPARAM lParam)
{
    CString* uidPtr = (CString*)wParam;
    CString uid = *uidPtr;
    delete uidPtr;

    int state = (int)lParam;
    LOG_INFO_FMT("Remote audio state changed for user {}, state={}", std::string(CT2A(uid)), state);

    return 0;
}

LRESULT CChannelPageDlg::OnRteLocalAudioStateChanged(WPARAM wParam, LPARAM lParam)
{
    int state = (int)wParam;
    LOG_INFO_FMT("Local audio state changed, state={}", state);

    return 0;
}

LRESULT CChannelPageDlg::OnRteError(WPARAM wParam, LPARAM lParam)
{
    // Placeholder implementation
    int error_code = (int)wParam;
    LOG_ERROR_FMT("RTE Engine Error: {}", error_code);
    
    // Display an error message to the user
    CString errorMsg;
    errorMsg.Format(_T("An RTE error occurred: %d. Please check the logs."), error_code);
    AfxMessageBox(errorMsg, MB_ICONERROR);

    return 0;
}

LRESULT CChannelPageDlg::OnRteUserListChanged(WPARAM wParam, LPARAM lParam)
{
    // Placeholder implementation
    // This message is sent when the user list changes, e.g., when a user joins or leaves.
    // You might need to re-sort or update the UI based on the new user list.
    UpdateVideoLayout();
    UpdatePageDisplay();
    UpdateSubscribedUsers();
    UpdateViewUserBindings();
    return 0;
}


//===========================================================================
// Initialization
//===========================================================================

void CChannelPageDlg::InitializeFonts()
{
    m_titleFont.CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Microsoft YaHei"));

    m_normalFont.CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Microsoft YaHei"));
}

void CChannelPageDlg::InitializeControls()
{
    m_comboGridMode.AddString(_T("2x2"));
    m_comboGridMode.AddString(_T("3x3"));
    m_comboGridMode.AddString(_T("4x4"));
    m_comboGridMode.AddString(_T("5x5"));
    m_comboGridMode.AddString(_T("7x7"));
    m_comboGridMode.SetCurSel(0);

    m_btnExitChannel.SetWindowText(_T("Exit Channel"));
    
    CString strChannelInfo;
    strChannelInfo.Format(_T("Channel: %s"), m_pageState.channelId);
    m_staticChannelId.SetWindowText(strChannelInfo);
    
    UpdatePageDisplay();
}


//===========================================================================
// RTE Engine Management
//===========================================================================

BOOL CChannelPageDlg::InitializeRteEngine()
{
    m_rteManager = new RteManager();
    if (!m_rteManager) {
        return FALSE;
    }

    // Set event handler
    m_rteManager->SetEventHandler(this);

    // Initialize RTE with config
    RteManagerConfig config;
    // Convert std::string to std::string (no conversion needed for appId)
    config.appId = m_joinParams.appId;
    config.userId = m_pageState.currentUserId;
    
    // Debug: Log the converted values using CString to avoid encoding issues
    LOG_INFO_FMT("Converted appId: {}", CString(config.appId.c_str()));
    LOG_INFO_FMT("Converted userId: {}", CString(config.userId.c_str()));
    // userToken is not a member of RteManagerConfig
    // Token should be passed separately to JoinChannel method

    if (!m_rteManager->Initialize(config)) {
        LOG_ERROR("Failed to initialize RteManager");
        return FALSE;
    }

    return TRUE;
}

void CChannelPageDlg::ReleaseRteEngine()
{
    if (m_rteManager) {
        m_rteManager->Destroy();
        delete m_rteManager;
        m_rteManager = nullptr;
    }
}

BOOL CChannelPageDlg::JoinRteChannel()
{
    if (!m_rteManager || m_joinParams.token.empty()) {
        return FALSE;
    }

    // Convert std::string to std::string (no conversion needed)
    std::string channelId = m_joinParams.channelId;
    std::string token = m_joinParams.token;
    
    // Debug: Log the values directly using std::string
    LOG_INFO_FMT("Converted channelId: {}", channelId);
    LOG_INFO_FMT("Converted token: {}", token.substr(0, 20));
    
    bool result = m_rteManager->JoinChannel(channelId, token);
    m_isChannelJoined = result;
    return result;
}

void CChannelPageDlg::LeaveRteChannel()
{
    if (m_rteManager && m_isChannelJoined) {
        m_rteManager->LeaveChannel();
        m_isChannelJoined = false;
    }
}


//===========================================================================
// Video Window & Layout Management
//===========================================================================

void CChannelPageDlg::CreateVideoWindows()
{
    DestroyVideoWindows();

    int gridSize = GetGridSize(m_pageState.currentGridMode);
    int maxWindows = gridSize * gridSize;
    
    for (int i = 0; i < maxWindows; i++)
    {
        CVideoGridCell* pVideoWnd = new CVideoGridCell();
        BOOL bResult = pVideoWnd->Create(NULL, _T(""), 
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN,
            CRect(0,0,0,0), this, static_cast<UINT>(IDC_VIDEO_WINDOW_BASE + i));

        if (bResult)
        {
            pVideoWnd->SetCellIndex(i);
            pVideoWnd->SetVideoSubscriptionCallback(this, &CChannelPageDlg::OnVideoCellVideoSubscriptionChangedCallback);
            pVideoWnd->SetAudioSubscriptionCallback(this, &CChannelPageDlg::OnVideoCellAudioSubscriptionChangedCallback);
            m_videoWindows.Add(pVideoWnd);
        }
        else
        {
            delete pVideoWnd;
        }
    }
}

void CChannelPageDlg::DestroyVideoWindows()
{
    for (int i = 0; i < m_videoWindows.GetSize(); i++)
    {
        if (m_videoWindows[i] && ::IsWindow(m_videoWindows[i]->GetSafeHwnd()))
        {
            m_videoWindows[i]->DestroyWindow();
        }
        delete m_videoWindows[i];
    }
    m_videoWindows.RemoveAll();
}

void CChannelPageDlg::UpdateGridLayout()
{
    if (m_staticVideoContainer.GetSafeHwnd() && m_videoWindows.GetSize() > 0)
    {
        CRect containerRect;
        m_staticVideoContainer.GetWindowRect(&containerRect);
        ScreenToClient(&containerRect);
        
        CArray<CRect> windowRects;
        CalculateGridLayout(m_pageState.currentGridMode, containerRect, windowRects);
        
        for (int i = 0; i < m_videoWindows.GetSize(); i++)
        {
            if (i < windowRects.GetSize())
            {
                m_videoWindows[i]->SetWindowPos(NULL, 
                    windowRects[i].left, windowRects[i].top,
                    windowRects[i].Width(), windowRects[i].Height(),
                    SWP_NOZORDER | SWP_NOACTIVATE);
                m_videoWindows[i]->ShowWindow(SW_SHOW);
            }
            else
            {
                m_videoWindows[i]->ShowWindow(SW_HIDE);
            }
        }
    }
}

void CChannelPageDlg::UpdateVideoLayout()
{
    int startUserIndex = (m_pageState.currentPage - 1) * m_pageState.usersPerPage;
    int userCount = (int)m_pageState.userList.GetSize();
    int windowCount = (int)m_videoWindows.GetSize();

    for (int i = 0; i < windowCount; i++)
    {
        CVideoGridCell* pVideoWnd = m_videoWindows[i];
        if (!pVideoWnd) continue;

        int userIndex = startUserIndex + i;
        if (userIndex < userCount)
        {
            ChannelUser* userInfo = m_pageState.userList[userIndex];
            if (pVideoWnd && userInfo) {
                pVideoWnd->SetUserInfo(CString(userInfo->userId.c_str()), CString(userInfo->userId.c_str()), userInfo->isConnected);
            }
            pVideoWnd->SetVideoSubscription(userInfo->isVideoSubscribed);
            pVideoWnd->SetAudioSubscription(userInfo->isAudioSubscribed);
            pVideoWnd->ShowWindow(SW_SHOW);
        }
        else
        {
            pVideoWnd->SetUserInfo(_T(""), _T(""), false);
            pVideoWnd->ShowWindow(SW_HIDE);
        }
    }
}

void CChannelPageDlg::SetGridMode(int gridMode)
{
    if (gridMode != m_pageState.currentGridMode)
    {
        m_pageState.currentGridMode = gridMode;
        m_pageState.currentPage = 1;
        
        CreateVideoWindows();
        UpdateGridLayout();
        UpdateVideoLayout();
        UpdatePageDisplay();
        UpdateSubscribedUsers();
        UpdateViewUserBindings();
    }
}

int CChannelPageDlg::GetGridSize(int gridMode)
{
    return gridMode;
}

void CChannelPageDlg::CalculateGridLayout(int gridMode, CRect containerRect, CArray<CRect>& windowRects)
{
    windowRects.RemoveAll();
    
    int gridSize = GetGridSize(gridMode);
    if (gridSize <= 0) return;

    int windowWidth = containerRect.Width() / gridSize;
    int windowHeight = containerRect.Height() / gridSize;

    for (int row = 0; row < gridSize; row++)
    {
        for (int col = 0; col < gridSize; col++)
        {
            CRect windowRect;
            windowRect.left = containerRect.left + col * windowWidth;
            windowRect.top = containerRect.top + row * windowHeight;
            windowRect.right = windowRect.left + windowWidth - 2; // -2 for border
            windowRect.bottom = windowRect.top + windowHeight - 2; // -2 for border
            windowRects.Add(windowRect);
        }
    }

    m_pageState.usersPerPage = gridSize * gridSize;
}


//===========================================================================
// User & Page Management
//===========================================================================

int CChannelPageDlg::FindUserIndex(LPCTSTR uid)
{
    for (int i = 0; i < m_pageState.userList.GetSize(); i++)
    {
        if (m_pageState.userList[i]->GetUserId() == std::string(CT2A(uid)))
        {
            return i;
        }
    }
    return -1;
}



void CChannelPageDlg::UpdatePageDisplay()
{
    int maxPages = GetMaxPages();
    CString strPageInfo;
    strPageInfo.Format(_T("%d / %d"), m_pageState.currentPage, maxPages);
    m_staticCurrentPage.SetWindowText(strPageInfo);

    m_btnPrevPage.EnableWindow(m_pageState.currentPage > 1);
    m_btnNextPage.EnableWindow(m_pageState.currentPage < maxPages);
}

int CChannelPageDlg::GetMaxPages()
{
    if (m_pageState.usersPerPage <= 0) return 1;
    if (m_pageState.userList.GetSize() == 0) return 1;
    
    size_t totalUsers = m_pageState.userList.GetSize();
    size_t usersPerPage = static_cast<size_t>(m_pageState.usersPerPage);
    return static_cast<int>((totalUsers + usersPerPage - 1) / usersPerPage);
}


//===========================================================================
// Subscription Callbacks
//===========================================================================

void CChannelPageDlg::OnVideoCellVideoSubscriptionChangedCallback(CWnd* pParent, int cellIndex, BOOL isVideoSubscribed)
{
    CChannelPageDlg* pDlg = dynamic_cast<CChannelPageDlg*>(pParent);
    if (pDlg) {
        pDlg->OnVideoCellVideoSubscriptionChanged(cellIndex, isVideoSubscribed);
    }
}

void CChannelPageDlg::OnVideoCellAudioSubscriptionChangedCallback(CWnd* pParent, int cellIndex, BOOL isAudioSubscribed)
{
    CChannelPageDlg* pDlg = dynamic_cast<CChannelPageDlg*>(pParent);
    if (pDlg) {
        pDlg->OnVideoCellAudioSubscriptionChanged(cellIndex, isAudioSubscribed);
    }
}

void CChannelPageDlg::OnVideoCellVideoSubscriptionChanged(int cellIndex, BOOL isVideoSubscribed)
{
    int userIndex = (m_pageState.currentPage - 1) * m_pageState.usersPerPage + cellIndex;
    if (userIndex >= 0 && userIndex < m_pageState.userList.GetSize()) {
        ChannelUser* user = m_pageState.userList[userIndex];
        if (user && !user->isLocal) {
            user->isVideoSubscribed = isVideoSubscribed;
            if (m_rteManager) {
                // SubscribeRemoteVideo and UnsubscribeRemoteVideo methods are not implemented in RteManager.
                // The video subscription logic needs to be updated based on the new RTE SDK API.
                // For now, we'll just log it.
                LOG_INFO_FMT("Video subscription for user {} set to {}", user->GetUserId(), isVideoSubscribed);
            }

            // 更新UI显示状态
            if (cellIndex < m_videoWindows.GetSize()) {
                m_videoWindows[cellIndex]->SetVideoSubscription(isVideoSubscribed);
            }

            // 同步订阅用户列表
            UpdateSubscribedUsers();
        }
    }
}

void CChannelPageDlg::OnVideoCellAudioSubscriptionChanged(int cellIndex, BOOL isAudioSubscribed)
{
    int userIndex = (m_pageState.currentPage - 1) * m_pageState.usersPerPage + cellIndex;
    if (userIndex >= 0 && userIndex < m_pageState.userList.GetSize()) {
        ChannelUser* user = m_pageState.userList[userIndex];
        if (user && !user->isLocal) {
            user->isAudioSubscribed = isAudioSubscribed;
            if (m_rteManager) {
                // SubscribeRemoteAudio and UnsubscribeRemoteAudio were removed or renamed.
                // The logic for audio subscription needs to be updated based on the new RteManager API.
                // For now, we'll just log it.
                LOG_INFO_FMT("Audio subscription for user {} set to {}", user->GetUserId(), isAudioSubscribed);
            }
            
            // 更新UI显示状态
            if (cellIndex < m_videoWindows.GetSize()) {
                m_videoWindows[cellIndex]->SetAudioSubscription(isAudioSubscribed);
            }
        }
    }
}

//===========================================================================
// RTE Integration Helpers
//===========================================================================

void CChannelPageDlg::UpdateSubscribedUsers()
{
    if (!m_rteManager) return;

    // 获取当前页面显示的用户列表
    std::vector<std::string> audioSubscribedUserIds;
    std::vector<std::string> videoSubscribedUserIds;
    int startUserIndex = (m_pageState.currentPage - 1) * m_pageState.usersPerPage;
    int endUserIndex = startUserIndex + m_pageState.usersPerPage;

    LOG_INFO_FMT("Updating subscribed users for page {} (users {} to {})", 
             m_pageState.currentPage, startUserIndex, endUserIndex - 1);

    for (int i = startUserIndex; i < endUserIndex && i < m_pageState.userList.GetSize(); i++) {
        ChannelUser* user = m_pageState.userList[i];
        if (user && !user->isLocal && user->isConnected) {
            // 检查音频订阅状态
            if (user->isAudioSubscribed) {
                audioSubscribedUserIds.push_back(user->GetUserId());
                LOG_INFO_FMT("Adding user {} to audio subscription list", user->GetUserId());
            } else {
                LOG_INFO_FMT("User {} audio subscription is disabled", user->GetUserId());
            }
            
            // 检查视频订阅状态
            if (user->isVideoSubscribed) {
                videoSubscribedUserIds.push_back(user->GetUserId());
                LOG_INFO_FMT("Adding user {} to video subscription list", user->GetUserId());
            } else {
                LOG_INFO_FMT("User {} video subscription is disabled", user->GetUserId());
            }
        }
    }

    // 将音频订阅用户列表传给RTE管理器
    if (!audioSubscribedUserIds.empty()) {
        LOG_INFO_FMT("Subscribing to {} users for audio: {}", audioSubscribedUserIds.size(), 
                 [&]() -> std::string {
                     std::string result;
                     for (const auto& id : audioSubscribedUserIds) {
                         if (!result.empty()) result += ", ";
                         result += id;
                     }
                     return result;
                 }());
        
        m_rteManager->SetAudioSubscribedUsers(audioSubscribedUserIds);
    } else {
        LOG_INFO("No users to subscribe to for audio");
        m_rteManager->SetAudioSubscribedUsers(std::vector<std::string>());
    }

    // 将视频订阅用户列表传给RTE管理器
    if (!videoSubscribedUserIds.empty()) {
        LOG_INFO_FMT("Subscribing to {} users for video: {}", videoSubscribedUserIds.size(), 
                 [&]() -> std::string {
                     std::string result;
                     for (const auto& id : videoSubscribedUserIds) {
                         if (!result.empty()) result += ", ";
                         result += id;
                     }
                     return result;
                 }());
        
        m_rteManager->SetVideoSubscribedUsers(videoSubscribedUserIds);
    } else {
        LOG_INFO("No users to subscribe to for video");
        m_rteManager->SetVideoSubscribedUsers(std::vector<std::string>());
    }
}

void CChannelPageDlg::UpdateViewUserBindings()
{
    if (!m_rteManager) return;

    std::map<void*, std::string> viewToUserMap;
    int startUserIndex = (m_pageState.currentPage - 1) * m_pageState.usersPerPage;

    LOG_INFO_FMT("Updating view-user bindings for page {} (users {} to {})", 
             m_pageState.currentPage, startUserIndex, startUserIndex + m_videoWindows.GetSize() - 1);

    for (int i = 0; i < m_videoWindows.GetSize(); i++) {
        int userIndex = startUserIndex + i;
        HWND videoWindow = m_videoWindows[i]->GetSafeHwnd();
        
        if (userIndex < m_pageState.userList.GetSize()) {
            ChannelUser* user = m_pageState.userList[userIndex];
            if (user && user->isConnected) {
                // 绑定已连接的用户到视频窗口
                viewToUserMap[videoWindow] = user->GetUserId();
                LOG_INFO_FMT("Binding user {} to video window {} (index {})", 
                         user->GetUserId(), (void*)videoWindow, i);
            } else if (user) {
                LOG_INFO_FMT("User {} is not connected, skipping binding for window {}", 
                         user->GetUserId(), (void*)videoWindow);
            } else {
                LOG_INFO_FMT("No user at index {}, skipping binding for window {}", 
                         userIndex, (void*)videoWindow);
            }
        } else {
            LOG_INFO_FMT("No user at index {} (beyond list size), skipping binding for window {}", 
                     userIndex, (void*)videoWindow);
        }
    }

    LOG_INFO_FMT("Setting {} view-user bindings", viewToUserMap.size());
    m_rteManager->SetViewUserBindings(viewToUserMap);
}

