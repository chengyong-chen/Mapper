#pragma once

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CTransformDlg dialog

class __declspec(dllexport) CTransformDlg : public CDialog
{
public:
	CTransformDlg(CWnd* pParent);

	// Dialog Data
	//{{AFX_DATA(CTransformDlg)
	enum
	{
		IDD = IDD_TRANSFORM
	};

	double m_a11;
	double m_a12;
	double m_a22;
	double m_a31;
	double m_a32;
	double m_a21;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransformDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransformDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
