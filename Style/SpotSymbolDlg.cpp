#include "stdafx.h"

#include "SpotSymbol.h"
#include "SpotSymbolDlg.h"
#include "SymbolSpot.h"

#include "SymbolMid.hpp"
#include "Midtable.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSpotSymbolDlg::CSpotSymbolDlg(CWnd* pParent, CMidtable<CSymbolMid<CSymbolSpot>>& tagtable, CSpotSymbol* pSpot)
	: TSymbolMidDlg(CSpotSymbolDlg::IDD, pParent, tagtable, pSpot)
{
	//{{AFX_DATA_INIT(CSpotSymbolDlg)
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CSpotSymbolDlg, TSymbolMidDlg)
	//{{AFX_MSG_MAP(CSpotSymbolDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpotSymbolDlg message handlers

BOOL CSpotSymbolDlg::OnInitDialog()
{
	TSymbolMidDlg::OnInitDialog();

	SetWindowPos(&wndTop, 8, 45, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
