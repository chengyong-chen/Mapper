#pragma once

#include "Resource.h"

class CLinkHttp;

class CLinkHttpDlg : public CDialog
{
public:
	CLinkHttpDlg(CWnd* pParent = nullptr, CLinkHttp* plink = nullptr);

	// Dialog Data
	//{{AFX_DATA(CLinkHttpDlg)
	enum
	{
		IDD = IDD_LINKHTTP
	};

	CString m_strHttp;
	//}}AFX_DATA

private:
	CLinkHttp* d_pLink;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkHttpDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinkHttpDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
