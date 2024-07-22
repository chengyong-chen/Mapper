#ifndef _LINKDLG_h_
#define _LINKDLG_h_

#include "Resource.h"

class CLink;

class __declspec(dllexport) CLinkDlg : public CDialog
{
public:
	CLinkDlg(CWnd* pParent = nullptr, CLink* plink = nullptr);

	~CLinkDlg() override;

	// Dialog Data
	//{{AFX_DATA(CLinkDlg)
	enum
	{
		IDD = IDD_LINK
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	CLink* d_pLink;
	CDialog* d_pLinkdlg;

	void Reset(CLink* pLink);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinkDlg)
	void OnOK() override;
	afx_msg void OnTypeSelchange();
	afx_msg void OnDestroy();
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
