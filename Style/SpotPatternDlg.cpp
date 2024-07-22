#include "stdafx.h"

#include "SpotPattern.h"
#include "SpotPatternDlg.h"
#include "PatternMid.h"
#include "Midtable.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpotPatternDlg dialog

CSpotPatternDlg::CSpotPatternDlg(CWnd* pParent, CMidtable<CPatternMid>& tagtable, CSpotPattern* pSpot)
	: TPatternMidDlg(CSpotPatternDlg::IDD, pParent, tagtable, pSpot)
{
	//{{AFX_DATA_INIT(CSpotPatternDlg)

	//}}AFX_DATA_INIT
}

void CSpotPatternDlg::DoDataExchange(CDataExchange* pDX)
{
	TPatternMidDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpotPatternDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSpotPatternDlg, TPatternMidDlg)
	//{{AFX_MSG_MAP(CSpotPatternDlg)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpotPatternDlg message handlers
BOOL CSpotPatternDlg::OnInitDialog()
{
	TPatternMidDlg::OnInitDialog();

	SetWindowPos(&wndTop, 8, 45, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
