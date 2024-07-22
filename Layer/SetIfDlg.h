#pragma once

#include "Resource.h"

class CSetIfDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetIfDlg)

public:
	CSetIfDlg(CWnd* pParent, CString strIf);

	~CSetIfDlg() override;

	// Dialog Data
	//{{AFX_DATA(CSetIfDlg)
	enum { IDD = IDD_SETIF };
	//}}AFX_DATA
public:
	CString m_strIf;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetIfDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetIfDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
