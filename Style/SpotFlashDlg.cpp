#include "stdafx.h"

#include "Midtable.hpp"
#include "FlashMid.h"
#include "SpotFlash.h"
#include "SpotFlashDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpotFlashDlg dialog

CSpotFlashDlg::CSpotFlashDlg(CWnd* pParent, CMidtable<CFlashMid>& tagtable, CSpotFlash* pSpotFlash)
	: TFlashMidDlg(CSpotFlashDlg::IDD, pParent, tagtable, pSpotFlash)
{
	//{{AFX_DATA_INIT(CSpotFlashDlg)

	//}}AFX_DATA_INIT
}

void CSpotFlashDlg::DoDataExchange(CDataExchange* pDX)
{
	TFlashMidDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpotFlashDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSpotFlashDlg, TFlashMidDlg)
	//{{AFX_MSG_MAP(CSpotFlashDlg)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpotFlashDlg message handlers
BOOL CSpotFlashDlg::OnInitDialog()
{
	TFlashMidDlg::OnInitDialog();

	SetWindowPos(&wndTop, 8, 45, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
