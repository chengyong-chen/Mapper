#pragma once

// PortDlg.h : header file

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg dialog

class AFX_EXT_CLASS CSetupDlg : public CDialog
{
public:
	CSetupDlg(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CSetupDlg)
	enum
	{
		IDD = IDD_SETUP
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	CDialog* d_pStyledlg;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupDlg)
	afx_msg void OnDestroy();
	BOOL OnInitDialog() override;
	afx_msg void OnTypeSelchange();
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
