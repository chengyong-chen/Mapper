#pragma once

// LoginDlg.h : header file

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

class __declspec(dllexport) CLoginDlg : public CDialog
{
	public:
	CLoginDlg(CWnd* pParent = nullptr, CString strUserName = _T(""), CString strPassword = _T(""));

	// Dialog Data
	//{{AFX_DATA(CLoginDlg)
	enum
	{
		IDD = IDD_LOGIN
	};

	CString m_strPassword;
	CString m_strUsername;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoginDlg)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CLoginDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
