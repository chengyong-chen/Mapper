#pragma once
#include "resource.h"
#include "Midtable.hpp"
#include "PatternMid.h"
#include "PatternMidDlg.h"

class CPatternMid;
class CSpotPattern;

class CSpotPatternDlg : public TPatternMidDlg<CSpotPattern>
{
public:
	CSpotPatternDlg(CWnd* pParent, CMidtable<CPatternMid>& tagtable, CSpotPattern* pSpot);

	// Dialog Data
	//{{AFX_DATA(CSpotPatternDlg)
	enum
	{
		IDD = IDD_SPOTPATTERN
	};

	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpotPatternDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpotPatternDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
