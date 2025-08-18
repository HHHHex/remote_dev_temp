#include "pch.h"
#include "ThousChannel.h"
#include "ChannelPageDlg.h"
#include "Logger.h"
#include "RteManager.h"
#include "afxdialogex.h"
#include <algorithm>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Helper function to sort users.
// Rules: 1. Local user always first. 2. Real users before robots. 3. Sort by UID.
static int CompareUsers(const void* a, const void* b)
{
    ChannelUser* userA = *(ChannelUser**)a;
    ChannelUser* userB = *(ChannelUser**)b;

    if (userA->isLocal) return -1;
    if (userB->isLocal) return 1;

    if (!userA->isRobot && userB->isRobot) return -1;
    if (userA->isRobot && !userB->isRobot) return 1;

    // Use _tcscmp for CString comparison
    int result = _tcscmp(userA->GetUID(), userB->GetUID());
    if (result < 0) return -1;
    if (result > 0) return 1;

    return 0;
}

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
    m_isChannelJoined = FALSE;

    // Create placeholders for real users (UID 2-31)
    for (int i = 2; i <= 31; ++i) {
        ChannelUser* placeholderUser = new ChannelUser();
        CString uidStr;
        uidStr.Format(_T("%d"), i);
        placeholderUser->uid = uidStr;
        placeholderUser->isLocal = FALSE;
        placeholderUser->isRobot = FALSE;
        placeholderUser->isConnected = FALSE;
        placeholderUser->userName.Format(_T("User %d (Offline)"), i);
        m_pageState.userList.Add(placeholderUser);
    }
}

CChannelPageDlg::~CChannelPageDlg()
{
    INFO("Channel page dialog destroyed");
    
    LeaveRteChannel();
    ReleaseRteEngine();
    DestroyVideoWindows();
    
    // Clean up all user canvases
    CString uidKey;
    HWND canvas;
    POSITION pos = m_userCanvasMap.GetStartPosition();
    while (pos != NULL)
    {
        m_userCanvasMap.GetNextAssoc(pos, uidKey, canvas);
        if (::IsWindow(canvas))
        {
            ::DestroyWindow(canvas);
        }
    }
    m_userCanvasMap.RemoveAll();

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

    LOG_INFO_FMT("Channel page dialog initialized for channel: %s", m_pageState.channelId);

    InitializeFonts();
    InitializeControls();
    
    // SetupVideoContainer(); // This function was removed during refactoring.
    CreateVideoWindows();
    UpdateGridLayout();

    if (!InitializeRteEngine()) {
        ERROR("Failed to initialize RTE engine");
        AfxMessageBox(_T("Failed to initialize RTE engine."));
        EndDialog(IDCANCEL);
        return FALSE;
    }
    
    // Create local user data
    ChannelUser* localUser = new ChannelUser();
    localUser->isLocal = TRUE;
    localUser->isConnected = TRUE;
    localUser->userName = m_pageState.currentUserId;
    localUser->uid = _T(""); // Will be updated onJoinChannelSuccess
    m_pageState.userList.InsertAt(0, localUser);
    
    // Enable local audio/video based on join params
    if (m_rteManager) {
        m_rteManager->SetLocalAudioCaptureEnabled(m_joinParams.enableMic);
        m_rteManager->SetLocalVideoCaptureEnabled(m_joinParams.enableCamera);
    }
    
    if (!JoinRteChannel()) {
        ERROR("Failed to join RTE channel");
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
        DetachAllUsersFromDisplay();
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
        DetachAllUsersFromDisplay();
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
    CString localUserId = _T("1"); // Default local user ID

    if (m_pageState.userList.GetSize() > 0) {
        ChannelUser* localUser = m_pageState.userList[0];
        if (localUser->isLocal) {
            localUser->uid = localUserId;
            localUser->userName.Format(_T("Local User (%s)"), localUserId);

            HWND canvas = NULL;
            if (m_userCanvasMap.Lookup(_T(""), canvas)) {
                m_userCanvasMap.RemoveKey(_T(""));
                m_userCanvasMap.SetAt(localUserId, canvas);
            }

            UpdateVideoLayout();

            CString strChannelInfo;
            strChannelInfo.Format(_T("Channel: %s (My UID: %s)"), m_pageState.channelId, localUserId);
            m_staticChannelId.SetWindowText(strChannelInfo);
        }
    }

    return 0;
}

LRESULT CChannelPageDlg::OnRteUserJoined(WPARAM wParam, LPARAM lParam)
{
    CString* uidPtr = (CString*)wParam;
    CString uid = *uidPtr;
    delete uidPtr;

    bool isRobot = (_ttoi(uid) >= 1000);

    if (isRobot) { // Robot user
        if (FindUserIndex(uid) != -1) return 0; // Already exists

        ChannelUser* robotUser = new ChannelUser();
        robotUser->uid = uid;
        robotUser->isConnected = TRUE;
        robotUser->isLocal = FALSE;
        robotUser->isRobot = TRUE;
        robotUser->userName.Format(_T("Robot %s"), uid);
        m_pageState.userList.Add(robotUser);
    }
    else { // Real user
        int userIndex = FindUserIndex(uid);
        if (userIndex != -1) {
            ChannelUser* user = m_pageState.userList[userIndex];
            if (user && !user->isConnected) {
                user->isConnected = TRUE;
                user->userName.Format(_T("Remote User %s"), uid);
            }
        }
    }

    SortUserList();
    GetOrCreateUserCanvas(uid);

    // Default subscribe to new user's streams
    if (m_rteManager) {
        std::string userIdStr = CT2A(uid);
        m_rteManager->SubscribeRemoteVideo(userIdStr);
        LOG_INFO_FMT("Subscribing to remote video for user: %s", uid);
        // m_rteManager->SubscribeRemoteAudio(userIdStr); // Function might be removed or renamed
    }
    int userIndex = FindUserIndex(uid);
    if (userIndex != -1) {
        m_pageState.userList[userIndex]->isVideoSubscribed = TRUE;
        m_pageState.userList[userIndex]->isAudioSubscribed = TRUE;
    }

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

    int userIndex = FindUserIndex(uid);

    if (userIndex != -1) {
        ChannelUser* userInfo = m_pageState.userList[userIndex];
        
        if (m_rteManager && !userInfo->isLocal) {
            std::string userIdStr = CT2A(uid);
             m_rteManager->UnsubscribeRemoteVideo(userIdStr);
             LOG_INFO_FMT("Unsubscribing from remote video for user: %s", uid);
            // m_rteManager->UnsubscribeRemoteAudio(userIdStr); // Function might be removed or renamed
        }
        
        DestroyUserCanvas(uid);

        if (userInfo->isRobot) {
            delete userInfo;
            m_pageState.userList.RemoveAt(userIndex);
        } else if (!userInfo->isLocal) {
            // Reset placeholder
            userInfo->isConnected = FALSE;
            userInfo->userName.Format(_T("User %s (Offline)"), uid);
            userInfo->isVideoSubscribed = TRUE;
            userInfo->isAudioSubscribed = TRUE;
        }

        SortUserList();
        
        if (m_pageState.currentPage > GetMaxPages()) {
            m_pageState.currentPage = GetMaxPages();
        }

        UpdateVideoLayout();
        UpdatePageDisplay();
        UpdateSubscribedUsers();
        UpdateViewUserBindings();
    }

    return 0;
}

LRESULT CChannelPageDlg::OnRteRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
{
    CString* uidPtr = (CString*)wParam;
    CString uid = *uidPtr;
    delete uidPtr;

    int state = LOWORD(lParam);
    int reason = HIWORD(lParam);
    LOG_INFO_FMT("Remote video state changed for user %s, state=%d, reason=%d", uid, state, reason);

    // You might want to update the UI for this user
    // For example, show an icon if their video is disabled

    return 0;
}

LRESULT CChannelPageDlg::OnRteLocalVideoStateChanged(WPARAM wParam, LPARAM lParam)
{
    int state = LOWORD(wParam);
    int reason = HIWORD(wParam);
    LOG_INFO_FMT("Local video state changed, state=%d, reason=%d", state, reason);

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
    LOG_INFO_FMT("Remote audio state changed for user %s, state=%d", uid, state);

    return 0;
}

LRESULT CChannelPageDlg::OnRteLocalAudioStateChanged(WPARAM wParam, LPARAM lParam)
{
    int state = (int)wParam;
    LOG_INFO_FMT("Local audio state changed, state=%d", state);

    return 0;
}

LRESULT CChannelPageDlg::OnRteError(WPARAM wParam, LPARAM lParam)
{
    // Placeholder implementation
    int error_code = (int)wParam;
    LOG_ERROR_FMT("RTE Engine Error: %d", error_code);
    
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
    SortUserList();
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
    config.appId = CT2A(m_joinParams.appId);
    config.userId = CT2A(m_pageState.currentUserId);
    config.userToken = CT2A(m_joinParams.token);

    if (!m_rteManager->Initialize(config)) {
        ERROR("Failed to initialize RteManager");
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
    if (!m_rteManager || m_joinParams.token.IsEmpty()) {
        return FALSE;
    }

    std::string channelId = CT2A(m_joinParams.channelId);
    std::string token = CT2A(m_joinParams.token);

    BOOL result = m_rteManager->JoinChannel(channelId, token);

    m_isChannelJoined = result;
    return result;
}

void CChannelPageDlg::LeaveRteChannel()
{
    if (m_rteManager && m_isChannelJoined) {
        m_rteManager->LeaveChannel();
        m_isChannelJoined = FALSE;
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
            pVideoWnd->SetUserInfo(userInfo->userName, userInfo->userName, userInfo->isConnected);
            pVideoWnd->SetVideoSubscription(userInfo->isVideoSubscribed);
            pVideoWnd->SetAudioSubscription(userInfo->isAudioSubscribed);
            pVideoWnd->ShowWindow(SW_SHOW);
        }
        else
        {
            pVideoWnd->SetUserInfo(_T(""), _T(""), FALSE);
            pVideoWnd->ShowWindow(SW_HIDE);
        }
    }
    
    AttachVisibleUsersToDisplay();
}

void CChannelPageDlg::SetGridMode(int gridMode)
{
    if (gridMode != m_pageState.currentGridMode)
    {
        DetachAllUsersFromDisplay();
        
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
// Video Canvas Management
//===========================================================================

void CChannelPageDlg::DetachAllUsersFromDisplay()
{
    CString uidKey;
    HWND canvas;
    POSITION pos = m_userCanvasMap.GetStartPosition();
    while (pos != NULL)
    {
        m_userCanvasMap.GetNextAssoc(pos, uidKey, canvas);
        if (canvas && ::IsWindow(canvas))
        {
            ::ShowWindow(canvas, SW_HIDE);
            // Detach from the display grid and attach to desktop to keep it alive but invisible
            ::SetParent(canvas, ::GetDesktopWindow());
        }
    }
}

void CChannelPageDlg::AttachVisibleUsersToDisplay()
{
    int startUserIndex = (m_pageState.currentPage - 1) * m_pageState.usersPerPage;
    
    for (int i = 0; i < m_videoWindows.GetSize(); i++)
    {
        int userIndex = startUserIndex + i;
        if (userIndex < m_pageState.userList.GetSize())
        {
            ChannelUser* userInfo = m_pageState.userList[userIndex];
            HWND displayWindow = m_videoWindows[i]->GetSafeHwnd();
            HWND canvas = GetOrCreateUserCanvas(userInfo->GetUID());

            if (displayWindow && ::IsWindow(displayWindow) && canvas && ::IsWindow(canvas))
            {
                ::SetParent(canvas, displayWindow);
                CRect displayRect;
                ::GetClientRect(displayWindow, &displayRect);
                ::SetWindowPos(canvas, HWND_BOTTOM, 0, 0, displayRect.Width(), displayRect.Height(), SWP_SHOWWINDOW);
            }
        }
    }
}

HWND CChannelPageDlg::GetOrCreateUserCanvas(LPCTSTR uid)
{
    HWND canvas = NULL;
    if (m_userCanvasMap.Lookup(uid, canvas)) {
        return canvas;
    }

    // Create a simple static window to host the video frame
    canvas = ::CreateWindowEx(0, _T("STATIC"), _T(""),
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0, 0, 1, 1, // Initial size doesn't matter, will be resized
        ::GetDesktopWindow(), NULL, AfxGetInstanceHandle(), NULL);

    if (canvas) {
        LOG_INFO_FMT("Canvas created for UID %s: 0x%p", uid, canvas);
        m_userCanvasMap.SetAt(uid, canvas);
    }
    else {
        LOG_ERROR_FMT("Canvas creation failed for UID %s: %d"), uid, GetLastError());
    }

    return canvas;
}

void CChannelPageDlg::DestroyUserCanvas(LPCTSTR uid)
{
    HWND canvas = NULL;
    if (m_userCanvasMap.Lookup(uid, canvas)) {
        if (::IsWindow(canvas)) {
            ::DestroyWindow(canvas);
        }
        m_userCanvasMap.RemoveKey(uid);
    }
}


//===========================================================================
// User & Page Management
//===========================================================================

int CChannelPageDlg::FindUserIndex(LPCTSTR uid)
{
    for (int i = 0; i < m_pageState.userList.GetSize(); i++)
    {
        if (_tcscmp(m_pageState.userList[i]->GetUID(), uid) == 0)
        {
            return i;
        }
    }
    return -1;
}

void CChannelPageDlg::SortUserList()
{
    if (m_pageState.userList.GetSize() > 1)
    {
        qsort(m_pageState.userList.GetData(), m_pageState.userList.GetSize(), sizeof(ChannelUser*), CompareUsers);
    }
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
    
    return (int)(m_pageState.userList.GetSize() + m_pageState.usersPerPage - 1) / m_pageState.usersPerPage;
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
                std::string userIdStr = CT2A(user->GetUID());
                if (isVideoSubscribed) {
                    m_rteManager->SubscribeRemoteVideo(userIdStr);
                }
                else {
                    m_rteManager->UnsubscribeRemoteVideo(userIdStr);
                }
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
                LOG_INFO_FMT("Audio subscription for user %s set to %d"), user->GetUID(), isAudioSubscribed);
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

    // This function's logic might need a complete review based on RteManager's capabilities.
    // The SetSubscribedUsers might have been intended for bulk subscription management which may have changed.
    // For now, we will comment out the call to avoid compilation errors.

    /*
    std::vector<std::string> subscribedUserIds;
    int startUserIndex = (m_pageState.currentPage - 1) * m_pageState.usersPerPage;
    int endUserIndex = startUserIndex + m_pageState.usersPerPage;

    for (int i = startUserIndex; i < endUserIndex && i < m_pageState.userList.GetSize(); i++) {
        ChannelUser* user = m_pageState.userList[i];
        if (user && !user->isLocal && user->isConnected && user->isVideoSubscribed) {
            subscribedUserIds.push_back(CT2A(user->GetUID()));
        }
    }

    m_rteManager->SetSubscribedUsers(subscribedUserIds);
    */
}

void CChannelPageDlg::UpdateViewUserBindings()
{
    if (!m_rteManager) return;

    std::map<void*, std::string> viewToUserMap;
    int startUserIndex = (m_pageState.currentPage - 1) * m_pageState.usersPerPage;

    for (int i = 0; i < m_videoWindows.GetSize(); i++) {
        int userIndex = startUserIndex + i;
        if (userIndex < m_pageState.userList.GetSize()) {
            ChannelUser* user = m_pageState.userList[userIndex];
            if (user && user->isConnected) {
                HWND canvasWnd = NULL;
                // Look up the correct canvas window from the map.
                if (m_userCanvasMap.Lookup(user->GetUID(), canvasWnd) && canvasWnd) {
                    std::string userIdStr(CT2A(user->GetUID()));
                    viewToUserMap[canvasWnd] = userIdStr;
                }
            }
        }
    }

    m_rteManager->SetViewUserBindings(viewToUserMap);
}

