#pragma once

#include "Resource.h"

class CLinkHtml;

class CLinkHtmlDlg : public CDialog
{
public:
	CLinkHtmlDlg(CWnd* pParent = nullptr, CLinkHtml* plink = nullptr);

	// Dialog Data
	//{{AFX_DATA(CLinkHtmlDlg)
	enum
	{
		IDD = IDD_LINKHTML
	};

	CString m_strHtml;
	//}}AFX_DATA

private:
	CLinkHtml* d_pLink;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkHtmlDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinkHtmlDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	afx_msg void OnSearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
