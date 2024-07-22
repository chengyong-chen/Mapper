#pragma once

#include "Resource.h"

class CPickLayDlg : public CDialog
{
	DECLARE_DYNAMIC(CPickLayDlg)

public:
	CPickLayDlg(CWnd* pParent = nullptr);

	~CPickLayDlg() override;

	// Dialog Data
	//{{AFX_DATA(CPickLayDlg)
	enum
	{
		IDD = IDD_PICKLAY
	};

	//}}AFX_DATA

	int m_nType;

	CString m_strTitle;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPickLayDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPickLayDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
