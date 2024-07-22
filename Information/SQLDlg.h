#pragma once

// SQLDLG.h : header file

#include "WhereTextPage.h"
#include "WherePickPage.h"

/////////////////////////////////////////////////////////////////////////////
// CSQLDlg dialog

class __declspec(dllexport) CSQLDlg : public CDialog
{
public:
	CSQLDlg(CWnd* pParent = nullptr, CDatabase* pDatabase = nullptr, CString strTable = _T(""));

	// Dialog Data
	//{{AFX_DATA(CSQLDlg)
	enum
	{
		IDD = IDD_SQL
	};

	static CString m_strColumns;
	static CString m_strTable;
	//}}AFX_DATA

public:
	CPropertySheet m_WhereSheet;
	CWhereTextPage m_textWherePage;
	CWherePickPage m_pickWherePage;

public:
	static CDatabase* m_pDatabase;
	CString m_strWhere;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSQLDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSQLDlg)
	void OnOK() override;
	BOOL OnInitDialog() override;
	afx_msg void OnSelchangeTable();
	afx_msg void OnSelchangeFieldcombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
