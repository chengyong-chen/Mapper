#pragma once
#include "resource.h"
#include "Midtable.hpp"
#include "SymbolMid.hpp"
#include "SymbolMidDlg.h"

class CSpotSymbol;
class CSymbolSpot;

class CSpotSymbolDlg : public TSymbolMidDlg<CSymbolSpot, CSpotSymbol>
{
public:
	CSpotSymbolDlg(CWnd* pParent, CMidtable<CSymbolMid<CSymbolSpot>>& tagtable, CSpotSymbol* pSpot);

	// Dialog Data
	//{{AFX_DATA(CSpotSymbolDlg)
	enum
	{
		IDD = IDD_SPOTSYMBOL
	};

	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpotSymbolDlg)
protected:
	BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpotSymbolDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
