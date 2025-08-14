#pragma once

#include <afxcoll.h> // For CMap
#include "afxdialogex.h"
#include "resource.h"
#include "HomePageDlg.h"
#include "VideoGridCell.h"
#include "../../core/RteManager.h"  // 替换RTC为RTE
#include "IAgoraRtcEngine.h"

using namespace agora;
using namespace agora::rtc;

// Custom Windows Messages for RTE events
#define WM_USER_RTE_JOIN_CHANNEL_SUCCESS        (WM_USER + 201)
#define WM_USER_RTE_USER_JOINED                 (WM_USER + 202)
#define WM_USER_RTE_USER_LEFT                   (WM_USER + 203)
#define WM_USER_RTE_REMOTE_VIDEO_STATE_CHANGED  (WM_USER + 204)
#define WM_USER_RTE_LOCAL_VIDEO_STATE_CHANGED   (WM_USER + 205)
#define WM_USER_RTE_ERROR                       (WM_USER + 206)
#define WM_USER_RTE_USER_LIST_CHANGED           (WM_USER + 207)

// ChannelUser类定义，简化为纯数据结构
class ChannelUser
{
public:
    ChannelUser() : uid(_T("")), isLocal(FALSE), isRobot(FALSE), isConnected(FALSE),
        isVideoSubscribed(TRUE), isAudioSubscribed(TRUE),
        isCurrentlyVisible(FALSE), lastVisiblePage(0) {}

    CString GetUID() const { return uid; }

public:
    CString userName;
    CString uid;
    BOOL isLocal;
    BOOL isRobot;
    BOOL isConnected;
    BOOL isVideoSubscribed;
    BOOL isAudioSubscribed;
    BOOL isCurrentlyVisible;
    int lastVisiblePage;
};

// 页面状态管理
struct ChannelPageState {
    CString channelId;              // 当前频道ID
    CString currentUserId;          // 当前用户ID
    int currentGridMode;            // 当前宫格模式(2,3,4,5,7)
    int currentPage;                // 当前页数
    int usersPerPage;              // 每页用户数
    CArray<ChannelUser*> userList;          // 用户列表（使用指针避免拷贝）
    CString audioMode;              // 音频模式
    BOOL isLocalVideoEnabled;       // 本地视频状态
    BOOL isLocalAudioEnabled;       // 本地音频模式状态
};

// 频道页面对话框类
class CChannelPageDlg : public CDialogEx, public IRteManagerEventHandler
{
    DECLARE_DYNAMIC(CChannelPageDlg)

public:
    CChannelPageDlg(CWnd* pParent = nullptr);
    CChannelPageDlg(const ChannelJoinParams& joinParams, CWnd* pParent = nullptr);
    virtual ~CChannelPageDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CHANNEL_PAGE_DLG };
#endif

protected:
    // MFC Overrides
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();

    // Message Map Functions
    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedExitChannel();
    afx_msg void OnCbnSelchangeGridMode();
    afx_msg void OnBnClickedPrevPage();
    afx_msg void OnBnClickedNextPage();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    
    // RTE Event Handlers
    afx_msg LRESULT OnRteJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnRteUserJoined(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnRteUserLeft(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnRteRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnRteLocalVideoStateChanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnRteError(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnRteUserListChanged(WPARAM wParam, LPARAM lParam);

private:
    // UI Controls
    CStatic m_staticChannelId;
    CButton m_btnExitChannel;
    CStatic m_staticVideoContainer;
    CComboBox m_comboGridMode;
    CButton m_btnPrevPage;
    CButton m_btnNextPage;
    CStatic m_staticCurrentPage;
    CArray<CVideoGridCell*> m_videoWindows;
    CFont m_titleFont;
    CFont m_normalFont;

    // RTE & Data Members
    ChannelJoinParams m_joinParams;
    ChannelPageState m_pageState;
    RteManager* m_rteManager;
    BOOL m_isChannelJoined;
    CMap<CString, LPCTSTR, HWND, HWND> m_userCanvasMap;

    // Initialization
    void InitializeControls();
    void InitializeFonts();

    // RTE Engine Management
    BOOL InitializeRteEngine();
    void ReleaseRteEngine();
    BOOL JoinRteChannel();
    void LeaveRteChannel();

    // Video Window & Layout Management
    void CreateVideoWindows();
    void DestroyVideoWindows();
    void UpdateGridLayout();
    void UpdateVideoLayout();
    void SetGridMode(int gridMode);
    int GetGridSize(int gridMode);
    void CalculateGridLayout(int gridMode, CRect containerRect, CArray<CRect>& windowRects);

    // Video Canvas Management (for RTE rendering)
    void DetachAllUsersFromDisplay();
    void AttachVisibleUsersToDisplay();
    HWND GetOrCreateUserCanvas(LPCTSTR uid);
    void DestroyUserCanvas(LPCTSTR uid);

    // User & Page Management
    int FindUserIndex(LPCTSTR uid);
    void SortUserList();
    void UpdatePageDisplay();
    int GetMaxPages();
    
    // Static Callbacks for CVideoGridCell
    static void OnVideoCellVideoSubscriptionChangedCallback(CWnd* pParent, int cellIndex, BOOL isVideoSubscribed);
    static void OnVideoCellAudioSubscriptionChangedCallback(CWnd* pParent, int cellIndex, BOOL isAudioSubscribed);
    
    // Callback Implementations
    void OnVideoCellVideoSubscriptionChanged(int cellIndex, BOOL isVideoSubscribed);
    void OnVideoCellAudioSubscriptionChanged(int cellIndex, BOOL isAudioSubscribed);

    // IRteManagerEventHandler implementation
    void OnConnectionStateChanged(int state) override;
    void OnUserJoined(const std::string& userId) override;
    void OnUserLeft(const std::string& userId) override;
    void OnLocalAudioStateChanged(int state) override;
        void OnLocalVideoStateChanged(agora::rtc::LOCAL_VIDEO_STREAM_STATE state, agora::rtc::LOCAL_VIDEO_STREAM_REASON reason) override;
    void OnRemoteAudioStateChanged(const std::string& userId, int state) override;
    void OnRemoteVideoStateChanged(const std::string& userId, int state) override;
    void OnError(int error) override;
    void OnUserListChanged() override;

    // RTE Integration Helpers
    void UpdateSubscribedUsers();
    void UpdateViewUserBindings();
};

