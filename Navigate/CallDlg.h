#pragma once
#include "SelectDlg.h"
class CPort;

/////////////////////////////////////////////////////////////////////////////
// CCallDlg dialog

class CCallDlg : public CDialog
{
public:
	CCallDlg(CWnd* pParent = nullptr, CPort* pPort = nullptr);

	// Dialog Data
	//{{AFX_DATA(CCallDlg)
	enum
	{
		IDD = IDD_CALL
	};

	//}}AFX_DATA

public:
	CPort* m_pPort;
	CSelectDlg m_SelectDlg;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCallDlg)
protected:
	BOOL OnInitDialog() override;
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCallDlg)
	afx_msg void OnOk();
	afx_msg void OnClickOnceRadio();
	afx_msg void OnClickDurativeRadio();
	afx_msg void OnClickNointerval();
	afx_msg void OnClickLimitless();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
