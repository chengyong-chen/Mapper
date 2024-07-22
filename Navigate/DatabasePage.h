#pragma once

// SocketDlg.h : header file

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDatabasePage dialog

class __declspec(dllexport) CDatabasePage : public CPropertyPage
{
public:
	CDatabasePage();

	~CDatabasePage() override;

	// Dialog Data
	//{{AFX_DATA(CDatabasePage)
	enum
	{
		IDD = IDD_DATABASE
	};

	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatabasePage)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDatabasePage)
	BOOL OnInitDialog() override;
	afx_msg void OnSetup();
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};