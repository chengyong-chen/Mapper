#pragma once

class CTextStyleDlg0;

#include "HintDlg.h"

class __declspec(dllexport) CTypeDlg : public CHintDlg
{
public:
	CTypeDlg(CWnd* pParent, CHint& hint);

	~CTypeDlg() override;

public:
	CTextStyleDlg0* m_pTextStyleDlg0;

protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTypeDlg)
	BOOL OnInitDialog() override;
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTypeDlg)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnContextChanged();
};
