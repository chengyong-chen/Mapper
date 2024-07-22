#pragma once

#include "Resource.h"

class CViewPrinter;

class CMyPrintDlg : public CPrintDialog
{
	DECLARE_DYNAMIC(CMyPrintDlg)

public:
	CMyPrintDlg(BOOL bPrintSetupOnly, DWORD dwFlags = PD_ALLPAGES|PD_USEDEVMODECOPIES|PD_NOPAGENUMS|PD_HIDEPRINTTOFILE|PD_NOSELECTION, CWnd* pParentWnd = nullptr);

	// Dialog Data
	//{{AFX_DATA(CPrintDialog)
	enum
	{
		IDD = IDD_PRINTDLG
	};

	long m_nOverlap;
	//}}AFX_DATA

public:
	CViewPrinter* m_viewPrinter;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyPrintDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CMyPrintDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG

	afx_msg void OnPage();
	DECLARE_MESSAGE_MAP()
};