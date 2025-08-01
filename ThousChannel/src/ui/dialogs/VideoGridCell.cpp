#include "pch.h"
#include "VideoGridCell.h"
#include "Logger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Define control IDs
#define IDC_LABEL_UID         1001
#define IDC_BTN_TOGGLE_VIDEO  1002
#define IDC_BTN_TOGGLE_AUDIO  1003

IMPLEMENT_DYNAMIC(CVideoGridCell, CWnd)

BEGIN_MESSAGE_MAP(CVideoGridCell, CWnd)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BTN_TOGGLE_VIDEO, &CVideoGridCell::OnBnClickedToggleVideo)
    ON_BN_CLICKED(IDC_BTN_TOGGLE_AUDIO, &CVideoGridCell::OnBnClickedToggleAudio)
END_MESSAGE_MAP()

CVideoGridCell::CVideoGridCell()
{
    m_isConnected = FALSE;
    m_isVideoSubscribed = FALSE;
    m_isAudioSubscribed = FALSE;
    m_cellIndex = -1;
    m_pParent = nullptr;
    m_videoSubscriptionCallback = nullptr;
    m_audioSubscriptionCallback = nullptr;
}

CVideoGridCell::~CVideoGridCell()
{
}

BOOL CVideoGridCell::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= WS_CHILD | WS_VISIBLE | WS_BORDER;
    return CWnd::PreCreateWindow(cs);
}

int CVideoGridCell::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Create UID Label
    m_labelUid.Create(_T("UID: N/A"), WS_CHILD | WS_VISIBLE | SS_LEFT, CRect(0,0,0,0), this, IDC_LABEL_UID);
    
    // Create Toggle Video Button
    m_btnToggleVideo.Create(_T("V"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(0,0,0,0), this, IDC_BTN_TOGGLE_VIDEO);

    // Create Toggle Audio Button
    m_btnToggleAudio.Create(_T("A"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(0,0,0,0), this, IDC_BTN_TOGGLE_AUDIO);

    return 0;
}


void CVideoGridCell::OnPaint()
{
    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);
    
    // Fill background
    dc.FillSolidRect(rect, RGB(0, 0, 0));

    // Draw connection status dot
    CRect dotRect(10, 10, 20, 20); // Position of the dot
    CBrush dotBrush;
    if (m_isConnected)
    {
        dotBrush.CreateSolidBrush(RGB(0, 255, 0)); // Green for connected
    }
    else
    {
        dotBrush.CreateSolidBrush(RGB(255, 0, 0)); // Red for disconnected
    }
    dc.SelectObject(&dotBrush);
    dc.Ellipse(dotRect);
}

void CVideoGridCell::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    if (m_labelUid.GetSafeHwnd())
    {
        // Position UID label near the status dot
        m_labelUid.SetWindowPos(NULL, 25, 8, 100, 15, SWP_NOZORDER | SWP_NOACTIVATE);
    }

    if (m_btnToggleVideo.GetSafeHwnd())
    {
        // Position buttons at the bottom right
        m_btnToggleVideo.SetWindowPos(NULL, cx - 60, cy - 30, 25, 25, SWP_NOZORDER | SWP_NOACTIVATE);
    }
    
    if (m_btnToggleAudio.GetSafeHwnd())
    {
        m_btnToggleAudio.SetWindowPos(NULL, cx - 30, cy - 30, 25, 25, SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void CVideoGridCell::SetUserInfo(const CString& userId, const CString& userName, BOOL isConnected)
{
    m_userId = userId;
    m_userName = userName;
    m_isConnected = isConnected;

    if (GetSafeHwnd())
    {
        // Update UID label text
        if (!m_userId.IsEmpty())
        {
            m_labelUid.SetWindowText(_T("UID: ") + m_userId);
        }
        else
        {
            m_labelUid.SetWindowText(_T(""));
        }
        
        // Trigger repaint to update status dot
        Invalidate();
    }
}

HWND CVideoGridCell::GetCanvasContainer() const
{
    return GetSafeHwnd();
}

CRect CVideoGridCell::GetVideoRect()
{
    CRect rect;
    GetClientRect(&rect);
    return rect;
} 

void CVideoGridCell::SetVideoSubscriptionCallback(CWnd* pParent, VideoSubscriptionCallback callback)
{
    m_pParent = pParent;
    m_videoSubscriptionCallback = callback;
}

void CVideoGridCell::SetAudioSubscriptionCallback(CWnd* pParent, AudioSubscriptionCallback callback)
{
    m_pParent = pParent;
    m_audioSubscriptionCallback = callback;
}

void CVideoGridCell::SetVideoSubscription(BOOL isSubscribed)
{
    m_isVideoSubscribed = isSubscribed;
    if (GetSafeHwnd())
    {
        Invalidate();
    }
}

void CVideoGridCell::SetAudioSubscription(BOOL isSubscribed)
{
    m_isAudioSubscribed = isSubscribed;
    if (GetSafeHwnd())
    {
        Invalidate();
    }
} 

void CVideoGridCell::OnBnClickedToggleVideo()
{
    if (m_videoSubscriptionCallback)
    {
        m_videoSubscriptionCallback(m_pParent, m_cellIndex, !m_isVideoSubscribed);
    }
}

void CVideoGridCell::OnBnClickedToggleAudio()
{
    if (m_audioSubscriptionCallback)
    {
        m_audioSubscriptionCallback(m_pParent, m_cellIndex, !m_isAudioSubscribed);
    }
} 
