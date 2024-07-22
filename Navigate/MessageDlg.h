#include "resource.h"

#pragma once

// MessageDlg.h : header file

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg dialog

class CMessageDlg : public CDialog
{
	public:
	CMessageDlg(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CMessageDlg)
	enum
	{
		IDD = IDD_MESSAGE
	};

	DWORD m_dwId;
	CString m_strMessage;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageDlg)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CMessageDlg)
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
