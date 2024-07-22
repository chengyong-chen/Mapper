#pragma once

#include "Resource.h"

// NameFieldDlg.h : header file

/////////////////////////////////////////////////////////////////////////////
// CNameFieldDlg dialog

class CNameFieldDlg : public CDialog
{
public:
	CNameFieldDlg(CWnd* pParent = nullptr);
	CNameFieldDlg(CWnd* pParent, CString strTitle, CStringList& fieldlist, int defaultsel);

	// Dialog Data
	//{{AFX_DATA(CNameFieldDlg)
	enum
	{
		IDD = IDD_NAMEFIELD
	};

	CListBox m_listBox;
	//}}AFX_DATA

	CString m_strTitle;
	CStringList m_fieldlist;
	int m_defaultsel;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNameFieldDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNameFieldDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
