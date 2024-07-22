#pragma once
class CFillLinear;

#include "GradientDlg.h"

class CFillLinearDlg : public CDialog
{
public:
	CFillLinearDlg(CWnd* pParent = nullptr, CFillLinear* pfill = nullptr);

	// Dialog Data
	//{{AFX_DATA(CFillLinearDlg)
	enum
	{
		IDD = IDD_FILLLINEAR
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	CGradientDlg m_GradientDlg;
	CFillLinear* d_pFill;
	void DrawSample();
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFillLinearDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFillLinearDlg)
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		afx_msg void OnEnChangeAngle();
	afx_msg LONG OnRedrawPreview(UINT WPARAM, LONG LPARAM);
};
