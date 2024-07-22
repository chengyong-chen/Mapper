#pragma once

// RectDlg.h : header file

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CRectDlg dialog

class __declspec(dllexport) CRectDlg : public CDialog
{
public:
	CRectDlg(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CRectDlg)
	enum
	{
		IDD = IDD_RECT
	};

	// NOTE: the ClassWizard will add data members here
	long m_nLeft;
	long m_nTop;
	long m_nRight;
	long m_nBottom;
	//}}AFX_DATA

	BOOL m_bPrint;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRectDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRectDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
