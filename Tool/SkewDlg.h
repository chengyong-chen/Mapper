#pragma once

#include "resource.h"

class CSkewDlg : public CDialog
{
public:
	CSkewDlg(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CSkewDlg)
	enum
	{
		IDD = IDD_SKEW
	};

	long m_nHangle;
	long m_nVangle;
	BOOL m_bFill;
	BOOL m_bLine;
	BOOL m_bSpot;
	BOOL m_bType;
	BOOL m_bRegular;
	//}}AFX_DATA

	BOOL p_bMouse;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkewDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSkewDlg)
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
