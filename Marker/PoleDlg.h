#pragma once

#include "resource.h"

class CPoleDlg : public CDialog
{
public:
	CPoleDlg(CWnd* pParent, long x, long y, BOOL bDirection);

	// Dialog Data
	//{{AFX_DATA(CPoleDlg)
	enum
	{
		IDD = IDD_POLE
	};

	BOOL m_bDirection;
	float m_fX;
	float m_fY;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPoleDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPoleDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
