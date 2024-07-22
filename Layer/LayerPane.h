#pragma once

#include "../Public/PaneToolBar.h"

class CBackgroundBar;
class CLayerTreeCtrl;

/////////////////////////////////////////////////////////////////////////////
// CLayerPane dialog

class CLayerTreeCtrl;

class AFX_EXT_CLASS CLayerPane : public CDockablePane
{
public:
	CLayerPane(bool bEditable);

public:
	bool m_bEditable;
public:
	CPaneToolBar m_ToolBar;

public:
	CLayerTreeCtrl* m_pLayerTreeCtrl;
	CBackgroundBar* m_pBackgroundBar;

public:
	void SetCtrl(CLayerTreeCtrl* pLayerTreeCtrl);
	void EnableBackgroundBar(BOOL bEnable);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTopoPane)
	BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLayerPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
