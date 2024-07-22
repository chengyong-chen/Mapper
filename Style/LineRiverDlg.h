#pragma once

#include "resource.h"

class CLineRiver;

class CLineRiverDlg : public CDialog
{
public:
	CLineRiverDlg(CWnd* pParent = nullptr, CLineRiver* pline = nullptr);

	// Dialog Data
	//{{AFX_DATA(CLineRiverDlg)
	enum
	{
		IDD = IDD_LINERIVER
	};

	long d_miterlimit;
	float d_fFWidth;
	float d_fTWidth;
	//}}AFX_DATA

private:
	CLineRiver* d_pLine;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLineRiverDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLineRiverDlg)
	afx_msg void OnDCDash();
	afx_msg void OnDCColor();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
