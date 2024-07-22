#pragma once

// QueryPane.h : header file

#include "KeyQueryEar.h"
#include "CooQueryEar.h"
#include "GeomQueryEar.h"

/////////////////////////////////////////////////////////////////////////////
// CQueryPane

class __declspec(dllexport) CQueryPane : public CDockablePane
{
	DECLARE_DYNAMIC(CQueryPane)

	public:
	CQueryPane();

	public:
	CMapStringToPtr m_mapForm;
	CComboBox m_ComboBox;

	// Attributes
	public:
	CKeyQueryEar m_KeyQueryEar;
	//	CSQLQueryEar m_SQLQueryEar;
	CCooQueryEar m_CooQueryEar;
	//	CTabQueryEar m_TabQueryEar;
	CGeomQueryEar m_GeomQueryEar;
	public:
	void RemoveEar(CString strTitle);

	static void ActiveEar(int nEar)
	{
	};
	// Operations
	public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQueryPane)
	BOOL DestroyWindow() override;
	BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	//}}AFX_VIRTUAL

	// Generated message map functions
	protected:
	//{{AFX_MSG(CQueryPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSelChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
