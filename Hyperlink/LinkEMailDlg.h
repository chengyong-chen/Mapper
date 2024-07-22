#pragma once

#include "Resource.h"

class CLinkEMail;

class CLinkEMailDlg : public CDialog
{
public:
	CLinkEMailDlg(CWnd* pParent = nullptr, CLinkEMail* plink = nullptr);

	// Dialog Data
	//{{AFX_DATA(CLinkEMailDlg)
	enum
	{
		IDD = IDD_LINKEMAIL
	};

	CString m_strEMail;
	//}}AFX_DATA

private:
	CLinkEMail* d_pLink;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkEMailDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinkEMailDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
