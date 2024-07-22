#pragma once

/////////////////////////////////////////////////////////////////////////////
// CPOUPane dialog

#include "../Public/PaneToolbar.h"
class CPOUListCtrl;

class __declspec(dllexport) CPOUPane : public CDockablePane
{
	DECLARE_DYNAMIC(CPOUPane)

	public:
	CPOUPane(bool bEditable);

	public:
	bool m_bEditable;
	CComboBox m_ComboBox;
	CPaneToolBar m_ToolBar;

	public:
	CPOUListCtrl* m_pAtlPOUListCtrl;
	CPOUListCtrl* m_pMapPOUListCtrl;

	void SetMapCtrl(CPOUListCtrl* pPOUListCtrl);
	void SetAtlCtrl(CPOUListCtrl* pPOUListCtrl);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTopoPane)
	BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

	protected:

	// Generated message map functions
	//{{AFX_MSG(CPOUPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnTopicChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
