#pragma once

#include "afxdialogex.h"
#include "resource.h"

// 视频窗格单元格类
class CVideoGridCell : public CWnd
{
    DECLARE_DYNAMIC(CVideoGridCell)

public:
    // 视频订阅状态变化回调函数类型
    typedef void (*VideoSubscriptionCallback)(CWnd* pParent, int cellIndex, BOOL isSubscribed);
    
    // 音频订阅状态变化回调函数类型
    typedef void (*AudioSubscriptionCallback)(CWnd* pParent, int cellIndex, BOOL isSubscribed);

    CVideoGridCell();
    virtual ~CVideoGridCell();

    // 设置用户信息
    void SetUserInfo(const CString& userId, const CString& userName, BOOL isConnected);
    
    // 设置窗格索引
    void SetCellIndex(int index) { m_cellIndex = index; }
    int GetCellIndex() const { return m_cellIndex; }
    
    // 设置回调函数
    void SetVideoSubscriptionCallback(CWnd* pParent, VideoSubscriptionCallback callback);
    void SetAudioSubscriptionCallback(CWnd* pParent, AudioSubscriptionCallback callback);

    // 设置订阅状态
    void SetVideoSubscription(BOOL isSubscribed);
    void SetAudioSubscription(BOOL isSubscribed);

    // 获取Canvas容器，用于附加视频画布
    HWND GetCanvasContainer() const;
    
    // 获取视频渲染区域
    CRect GetVideoRect();

protected:
    DECLARE_MESSAGE_MAP()
    
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedToggleVideo();
    afx_msg void OnBnClickedToggleAudio();

private:
    CString m_userId;
    CString m_userName;
    BOOL m_isConnected;
    int m_cellIndex;
    BOOL m_isVideoSubscribed;
    BOOL m_isAudioSubscribed;
    CWnd* m_pParent;
    VideoSubscriptionCallback m_videoSubscriptionCallback;
    AudioSubscriptionCallback m_audioSubscriptionCallback;

    // New UI Controls
    CStatic m_labelUid;
    CButton m_btnToggleVideo;
    CButton m_btnToggleAudio;
}; 

