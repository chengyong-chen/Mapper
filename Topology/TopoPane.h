#pragma once

#include "../Public/PaneToolbar.h"

/////////////////////////////////////////////////////////////////////////////
// CTopoPane dialog

class CTopoCtrl;

class __declspec(dllexport) CTopoPane : public CDockablePane
{
public:
	CTopoPane();

public:
	CPaneToolBar m_ToolBar;

public:
	CTopoCtrl* d_pTopoCtrl;
	void SetCtrl(CTopoCtrl* pTopoCtrl);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTopoPane)
	BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTopoPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
