#pragma once

#include <afxcoll.h> // For CMap
#include "afxdialogex.h"
#include "resource.h"
#include "HomePageDlg.h"
#include "VideoGridCell.h"
#include "../../core/IRteManagerEventHandler.h"
#include <string>

// Forward declarations
class RteManager;

// Custom Windows Messages for RTE events
#define WM_USER_RTE_JOIN_CHANNEL_SUCCESS        (WM_USER + 201)
#define WM_USER_RTE_USER_JOINED                 (WM_USER + 202)
#define WM_USER_RTE_USER_LEFT                   (WM_USER + 203)
#define WM_USER_RTE_REMOTE_VIDEO_STATE_CHANGED  (WM_USER + 204)
#define WM_USER_RTE_LOCAL_VIDEO_STATE_CHANGED   (WM_USER + 205)
#define WM_USER_RTE_ERROR                       (WM_USER + 206)
#define WM_USER_RTE_USER_LIST_CHANGED           (WM_USER + 207)
#define WM_USER_RTE_REMOTE_AUDIO_STATE_CHANGED  (WM_USER + 208)
#define WM_USER_RTE_LOCAL_AUDIO_STATE_CHANGED   (WM_USER + 209)

// ChannelUser class definition
class ChannelUser 
{
public:
    // Constructor
    ChannelUser() 
        : uid(""), isLocal(false), isRobot(false), isConnected(false),
          isVideoSubscribed(true), isAudioSubscribed(true),
          isCurrentlyVisible(false), lastVisiblePage(0) 
    {
    }

    // Getter method
    std::string GetUID() const { return uid; }

    // Public members
    std::string userName;
    std::string uid;
    bool isLocal;
    bool isRobot;
    bool isConnected;
    bool isVideoSubscribed;
    bool isAudioSubscribed;
    bool isCurrentlyVisible;
    int lastVisiblePage;
};

// Page state management
struct ChannelPageState {
    std::string channelId;              // Current channel ID
    std::string currentUserId;          // Current user ID
    int currentGridMode;                // Current grid mode (2,3,4,5,7)
    int currentPage;                    // Current page number
    int usersPerPage;                   // Users per page
    CArray<ChannelUser*> userList;      // User list (using pointers to avoid copying)
    std::string audioMode;              // Audio mode
    bool isLocalVideoEnabled;           // Local video status
    bool isLocalAudioEnabled;           // Local audio mode status
};

// Channel page dialog class
class CChannelPageDlg : public CDialogEx, public IRteManagerEventHandler
{
    DECLARE_DYNAMIC(CChannelPageDlg)

public:
    CChannelPageDlg(CWnd* pParent = nullptr);
    CChannelPageDlg(const ChannelJoinParams& joinParams, CWnd* pParent = nullptr);
    virtual ~CChannelPageDlg();

    // Dialog data
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
    afx_msg LRESULT OnRteRemoteAudioStateChanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnRteLocalAudioStateChanged(WPARAM wParam, LPARAM lParam);

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
    void OnLocalVideoStateChanged(int state, int reason) override;
    void OnRemoteAudioStateChanged(const std::string& userId, int state) override;
    void OnRemoteVideoStateChanged(const std::string& userId, int state) override;
    void OnError(int error) override;
    void OnUserListChanged() override;

    // RTE Integration Helpers
    void UpdateSubscribedUsers();
    void UpdateViewUserBindings();
};

