#pragma once
class CFillGradientRadial;

#include "GradientDlg.h"

class CFillRadialDlg : public CDialog
{
public:
	CFillRadialDlg(CWnd* pParent = nullptr, CFillRadial* pfill = nullptr);

	// Dialog Data
	//{{AFX_DATA(CFillRadialDlg)
	enum
	{
		IDD = IDD_FILLRADIAL
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
public:
	CGradientDlg m_GradientDlg;
	CFillRadial* d_pFill;
	void DrawSample();
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFillRadialDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFillRadialDlg)
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg LONG OnRedrawPreview(UINT WPARAM, LONG LPARAM);
};
