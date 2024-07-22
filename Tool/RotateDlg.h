#pragma once

#include "resource.h"

class CRotateDlg : public CDialog
{
public:
	CRotateDlg(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CRotateDlg)
	enum	{ IDD=IDD_ROTATE };

	BOOL m_bFill;
	BOOL m_bLine;
	BOOL m_bSpot;
	BOOL m_bType;
	BOOL m_bRegular;
	//}}AFX_DATA
	double p_fAngle;
	BOOL p_bMouse;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRotateDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRotateDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
