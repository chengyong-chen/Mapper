#pragma once

class CNavinfoDlg;

class __declspec(dllexport) CNavinfoPane : public CDockablePane
{
	public:
	CNavinfoPane();

	public:
	CNavinfoDlg* m_pNavigateDlg;

	public:
	// Operations

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNavinfoPane)	
	public:
	BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

	// Implementation
	// Generated message map functions
	protected:
	//{{AFX_MSG(CNavinfoPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
