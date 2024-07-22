#pragma once

/////////////////////////////////////////////////////////////////////////////
// CHtmlView view
#include "../Atlas/WebBrowser.h"

class CHtmlView : public CWnd
{
public:
	CHtmlView(); // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CHtmlView)

	// Attributes
public:
	CWebBrowser m_WebBrowser;

	BOOL m_bCanForward;
	BOOL m_bCanBack;
	// Operations
public:
	void Navigate(LPCTSTR lpszNavigatePage);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlView)
public:
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	virtual void OnCommandStateChange(long nCommand, BOOL bEnable);
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel);
	//}}AFX_VIRTUAL

	void GoBack();
	void GoForward();

	// Implementation
public:
	~CHtmlView() override;
protected:
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	// Event reflectors (not normally overridden)
	//	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,LPCTSTR lpszHeaders, BOOL* pbCancel) override;
protected:
	// Generated message map functions
protected:
	//{{AFX_MSG(CHtmlView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void BeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
