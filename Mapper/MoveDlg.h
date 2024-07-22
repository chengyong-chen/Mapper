#pragma once

#include "Resource.h"

class CMoveDlg : public CDialog
{
public:
	CMoveDlg(CWnd* pParent, CString strUnit);

	// Dialog Data
	//{{AFX_DATA(CMoveDlg)
	enum
	{
		IDD = IDD_MOVE
	};

	BOOL m_bContents;
	BOOL m_bFills;
	float m_fCx;
	float m_fCy;
	CString m_strUnit;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMoveDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMoveDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
