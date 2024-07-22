#pragma once
#include "resource.h"

#include "Midtable.hpp"
#include "FlashMid.h"
#include "FlashMidDlg.h"
class CFlashMid;
class CSymbol;
class CSpotFlash;

class CSpotFlashDlg : public TFlashMidDlg<CSpotFlash>
{
public:
	CSpotFlashDlg(CWnd* pParent, CMidtable<CFlashMid>& tagtable, CSpotFlash* d_pSpot);

	// Dialog Data
	//{{AFX_DATA(CSpotPatternDlg)
	enum
	{
		IDD = IDD_SPOTFLASH
	};

	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpotFlashDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override;
	BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpotFlashDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
