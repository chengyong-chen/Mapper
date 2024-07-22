#pragma once
#include "resource.h"
#include "Midtable.hpp"
#include "PatternMid.h"
#include "PatternMidDlg.h"

class CPatternMid;
class CFillPattern;

class CFillPatternDlg : public TPatternMidDlg<CFillPattern>
{
public:
	CFillPatternDlg(CWnd* pParent, CMidtable<CPatternMid>& tagtable, CFillPattern* pFill);

	// Dialog Data
	//{{AFX_DATA(CFillPatternDlg)
	enum
	{
		IDD = IDD_FILLPATTERN
	};

	short m_nAngle;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFillPatternDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFillPatternDlg)
	afx_msg void OnChangeAngle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
