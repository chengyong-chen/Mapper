#pragma once

#include "webbrowser.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CLegendDlg dialog

class __declspec(dllexport) CLegendDlg : public CDialog
{
public:
	CLegendDlg(CWnd* pParent, CString strHtml);

	// Dialog Data
	//{{AFX_DATA(CLegendDlg)
	enum
	{
		IDD = IDD_LEGEND
	};

	CWebBrowser m_browser;
	//}}AFX_DATA

	CString m_strHtml;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLegendDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLegendDlg)
	BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
