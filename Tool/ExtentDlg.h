#pragma once

#include "Resource.h"

class CExtentDlg : public CDialog
{
public:
	CExtentDlg(CWnd* pParent = nullptr, long nRadius = 10);

	// Dialog Data
	//{{AFX_DATA(CExtentDlg)
	enum
	{
		IDD = IDD_EXTENT
	};

	long m_nExtent;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtentDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExtentDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
