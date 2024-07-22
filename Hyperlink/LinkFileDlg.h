#pragma once

#include "Resource.h"

class CLinkFile;

class CLinkFileDlg : public CDialog
{
public:
	CLinkFileDlg(CWnd* pParent = nullptr, CLinkFile* plink = nullptr);

	// Dialog Data
	//{{AFX_DATA(CLinkFileDlg)
	enum
	{
		IDD = IDD_LINKFILE
	};

	CString m_strFile;
	//}}AFX_DATA

private:
	CLinkFile* d_pLink;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkFileDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinkFileDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	afx_msg void OnSearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
