#pragma once
#include "resource.h"

class CLine;

class CLineAloneDlg : public CDialog
{
public:
	//	CLineAloneDlg(CWnd* pParent = nullptr,COLORREF p_color=RGB(0,0,0),const CDash& p_dash=(CDash&)nullptr, long p_width=10,long p_capindex=2,  long p_joinindex=0, long p_mitrilimit=30);  
	CLineAloneDlg(CWnd* pParent, CLine* pline);

	// Dialog Data
	//{{AFX_DATA(CLineAloneDlg)
	enum
	{
		IDD = IDD_LINEALONE
	};

	long d_miterlimit;
	float d_fWidth;
	//}}AFX_DATA

public:
	CLine* d_pLine;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLineAloneDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLineAloneDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	afx_msg void OnDCDash();
	afx_msg void OnDCColor();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
