#pragma once

#include "resource.h"

class CScaleDlg : public CDialog
{
public:
	CScaleDlg(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CScaleDlg)
	enum
	{
		IDD = IDD_SCALE
	};

	BOOL m_bFill;
	BOOL m_bLine;
	BOOL m_bSpot;
	BOOL m_bType;
	BOOL m_bRegular;
	//}}AFX_DATA
	double p_fVscale;
	double p_fHscale;
	BOOL p_bMouse;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScaleDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScaleDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
