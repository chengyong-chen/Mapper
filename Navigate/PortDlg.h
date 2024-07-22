#include "resource.h"

#pragma once

// PortDlg.h : header file

class CPort;

/////////////////////////////////////////////////////////////////////////////
// CPortDlg dialog

class __declspec(dllexport) CPortDlg : public CDialog
{
	public:
	CPortDlg(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CPortDlg)
	enum
	{
		IDD = IDD_PORT
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	public:
	CDialog* d_pStyledlg;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPortDlg)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CPortDlg)
	afx_msg void OnDestroy();
	BOOL OnInitDialog() override;
	afx_msg void OnTypeSelchange();
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
