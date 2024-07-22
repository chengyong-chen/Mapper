#pragma once

#include "resource.h"

class AFX_EXT_CLASS CTextEditDlg : public CDialogEx
{
public:
	CTextEditDlg(CWnd* pParent = nullptr, CString pstring = _T(""));

	// Dialog Data
	//{{AFX_DATA(CTextEditDlg)
	enum
	{
		IDD = IDD_TEXTEDIT
	};

	CString m_String;
	//}}AFX_DATA

	CMFCToolTipCtrl m_ToolTip;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextEditDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextEditDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
