#pragma once
#include "resource.h"

class CFillAlone;

class CFillAloneDlg : public CDialog
{
public:
	CFillAloneDlg(CWnd* pParent = nullptr, CFillAlone* pfill = nullptr);

	// Dialog Data
	//{{AFX_DATA(CFillAloneDlg)
	enum
	{
		IDD = IDD_FILLALONE
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	CFillAlone* d_pFill;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFillAloneDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFillAloneDlg)
	afx_msg void OnPaint();
	BOOL OnInitDialog() override;
	afx_msg void OnDCColor();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
