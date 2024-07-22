#pragma once

// InfoDlg.h : header file

/////////////////////////////////////////////////////////////////////////////
// CAttributeCtrl dialog

class __declspec(dllexport) CAttributeCtrl : public CListCtrl
{
public:
	CAttributeCtrl();
public:
	CDatabase* m_database;
	CString m_strTable;

	LONG m_nTableType;

	BOOL OnTableChanged(CDatabase* pDatabase, CString strTable);
	BOOL ShowInfo(CString strIdField, const DWORD& dwId);

public:
	BOOL Create(CWnd* pParent);

protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlEx)
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CAttributeCtrl() override;

protected:
	// Generated message map functions
	//{{AFX_MSG(CAttributeCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
