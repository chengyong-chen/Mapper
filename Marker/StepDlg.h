#pragma once

#include "resource.h"

class CStepDlg : public CDialog
{
public:
	CStepDlg(CWnd* pParent = nullptr, UINT pStep = 10);

	// Dialog Data
	//{{AFX_DATA(CStepDlg)
	enum
	{
		IDD = IDD_STEP
	};

	float m_fStep;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStepDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStepDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
