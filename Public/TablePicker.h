#pragma once

#include "Resource.h"

class __declspec(dllexport) CPickTableDlg : public CDialog
{
public:
	CPickTableDlg(CWnd* pParent = nullptr, CDatabase* database = nullptr, LPCTSTR lpTable = nullptr);

	// Dialog Data
	//{{AFX_DATA(CPickTableDlg)
	enum
	{
		IDD = IDD_PICKTABLE
	};

	CListCtrl m_ctrlTable;
	//}}AFX_DATA

	CString m_strTable;
	CDatabase* m_database;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPickTableDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPickTableDlg)
	void OnOK() override;
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
