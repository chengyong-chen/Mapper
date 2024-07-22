#pragma once

#include "Resource.h"


class CRRectDlg : public CDialog
{
public:
	CRRectDlg(CWnd* pParent = nullptr, long nRadius = 10, long nMax = 10);

	// Dialog Data
	//{{AFX_DATA(CRRectDlg)
	enum
	{
		IDD = IDD_RRECT
	};

	long m_nRadius;
	//}}AFX_DATA

	long m_nMax;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRRectDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRRectDlg)
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
