#pragma once

#include "Resource.h"

class CDeckDlg : public CDialog
{
public:
	CDeckDlg(CWnd* pParent = nullptr);
	CDeckDlg(CWnd* pParent, CString strFile, CString strHtml, CString strLegend, BOOL bNavigatable, BOOL bQueryble);

	// Dialog Data
	//{{AFX_DATA(CDeckDlg)
	enum
	{
		IDD = IDD_DECK
	};

	CString m_strFile;
	CString m_strHtml;
	CString m_strLegend;
	BOOL m_bQueryble;
	BOOL m_bNavigatable;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeckDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeckDlg)
	// NOTE: the ClassWizard will add member functions here	
	afx_msg void OnFind1();
	afx_msg void OnFind2();
	afx_msg void OnFind3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
