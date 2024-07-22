#pragma once

#include "resource.h"

class CReflectDlg : public CDialog
{
public:
	CReflectDlg(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CReflectDlg)
	enum
	{
		IDD = IDD_REFLECT
	};

	BOOL m_bFill;
	BOOL m_bLine;
	BOOL m_bSpot;
	BOOL m_bType;
	BOOL m_bRegular;
	//}}AFX_DATA
	double p_fAngle;
	BOOL p_bContens;
	BOOL p_bFills;
	BOOL p_bMouse;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReflectDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReflectDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
