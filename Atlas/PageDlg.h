#pragma once

#include "Resource.h"

class CPageDlg : public CDialog
{
public:
	CPageDlg(CWnd* pParent = nullptr);
	CPageDlg(CWnd* pParent, CString strTarget, WORD wType, CPoint Anchor);

	// Dialog Data
	//{{AFX_DATA(CPageDlg)
	enum
	{
		IDD = IDD_PAGE
	};

	CString m_strTarget;
	LONG m_X;
	LONG m_Y;
	//}}AFX_DATA

	WORD m_wType;
	WORD m_wMapId;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	void OnOK() override;
	BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPageDlg)
	// NOTE: the ClassWizard will add member functions here	
	afx_msg void OnFindFile();
	afx_msg void OnBnClickedFolder();
	afx_msg void OnBnClickedMap();
	afx_msg void OnBnClickedHtml();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
