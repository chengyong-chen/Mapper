#pragma once

#include <afxframewndex.h>

#include "Adodc.h"
#include "Truedbgridctrl.h"

#include "../Database/Datasource.h"
#include "../Public/PaneToolBar.h"

class __declspec(dllexport) CTDBGridCtrl : public CFrameWndEx
{
	DECLARE_DYNCREATE(CTDBGridCtrl);
public:
	CTDBGridCtrl();

public:
	CButton* m_pCheckBox;
	CTrueDBGridCtrl m_DBGrid;
	CAdodc m_Adodc;
	CTabToolBar m_ToolBar;

public:
	static bool m_bEditable;
public:
	CDatabase* m_pDatabase;
	const CHeaderProfile* m_pHeaderProfile;
public:
	bool SetDatabase(CDatabase* pDatabase);
	void Refresh();
	virtual void ClearGrid();
	virtual void Fill(CDatabase* pDatabase, CHeaderProfile* pHeaderProfile, BOOL bReadOnly);
public:
	bool Delete(const DWORD& dwId);
	bool AddNew(CString strFields, CString strValues);

public:
	virtual void Update();
	virtual DWORD ActiveId();
	virtual bool ActivateRec(CString strIdField, DWORD dwId, BOOL bRemind);

	virtual void MarkField()
	{
	};

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDBGridCtrl)
public:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CWnd* pParentWnd = nullptr, LPCTSTR lpszMenuName = nullptr, DWORD dwExStyle = 0, CCreateContext* pContext = nullptr) override;
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTDBGridCtrl)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsert();
	afx_msg void OnDelete();
	afx_msg void OnOrder();
	afx_msg void OnOrderAsce();
	afx_msg void OnOrderDesc();
	afx_msg void OnFitToHeader();
	afx_msg void OnFitToWindow();
	afx_msg void OnStat();
	afx_msg void OnShortCut();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void RowColChange(VARIANT* LastRow, short LastCol);
	//	void FilterChange();
	void Mouseup(short nButton, short nShiftState, long x, long y);
	void FilterButtonClick(short ColIndex);
};
