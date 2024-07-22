#include "resource.h"

#pragma once

// SearchDlg.h : header file

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg dialog

class __declspec(dllexport) CSearchDlg : public CDialog
{
	public:
	CSearchDlg(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CSearchDlg)
	enum
	{
		IDD = IDD_SEARCH
	};

	CString m_strKeyField;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchDlg)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CSearchDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
