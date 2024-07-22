#include "msdgridctrl.h"
#include "rdc.h"

#pragma once

// DBGridEar.h : header file

class CPointF;
#include "../Public/PaneToolBar.h"

class __declspec(dllexport) CDBGridCtrl : public CFrameWndEx
{
protected:
	DECLARE_DYNCREATE(CDBGridCtrl);
public:
	CDBGridCtrl();

public:
	CMsDgridCtrl m_DBGrid;
	CRdc m_Rdc;

public:
	CTabToolBar m_ToolBar;

public:
	LONG m_nTableType;
	CDatabase* m_pDatabase;
	CString m_strTable;
	CString m_strFields;
	CString m_strWhere;

	CString m_strIdField;
	bool m_keyVisible;

public:
	void Clear();
	void Fill(CDatabase* pDatabase, CString strTable, CString strFields, CString strWhere, CString strIdField);

	void PolyFilter(CPointF* pPoints, int nAnchors);
	void PolyFilter(CPoint* pPoints, int nAnchors);

	static void MarkField()
	{
	};

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBGridCtrl)
public:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CWnd* pParentWnd = nullptr, LPCTSTR lpszMenuName = nullptr, DWORD dwExStyle = 0, CCreateContext* pContext = nullptr) override;
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDBGridCtrl)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnOrderAsce();
	afx_msg void OnOrderDesc();
	afx_msg void OnFitToHeader();
	afx_msg void OnFitToWindow();
	afx_msg void OnStat();
	afx_msg void OnOrder();

	afx_msg void OnExport2Excel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
