#pragma once

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CRenameDlg

// @class This is the Rename datasource dialog box
class CRenameDlg : public CDialog
{
public:
	// Dialog Data
	//{{AFX_DATA(CRenameDlg)
	enum
	{
		IDD = IDD_RENAME
	};

	CString m_cNewName;

	CString m_cOldName;

	//}}AFX_DATA
public:
	// @cmember Constructor
	CRenameDlg(CWnd* pParent, const CString& cOldName);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenameDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRenameDlg)
	afx_msg void OnOk();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
