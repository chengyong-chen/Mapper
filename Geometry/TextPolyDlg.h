#pragma once

// TextPolyDlg.h : header file

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CTextPolyDlg dialog

class CTextPolyDlg : public CDialog
{
public:
	CTextPolyDlg(CWnd* pParent = nullptr, DWORD dwStyle = 0);

	// Dialog Data
	//{{AFX_DATA(CTextPolyDlg)
	enum
	{
		IDD = IDD_TEXTPOLY
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	DWORD m_dwStyle;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextPolyDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextPolyDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
