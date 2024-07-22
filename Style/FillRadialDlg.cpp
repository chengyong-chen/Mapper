#include "stdafx.h"
#include "Global.h"
#include "FillRadial.h"
#include "FillRadialDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFillRadialDlg dialog

CFillRadialDlg::CFillRadialDlg(CWnd* pParent /*=nullptr*/, CFillRadial* pfill)
	: CDialog(CFillRadialDlg::IDD, pParent), m_GradientDlg(nullptr, pfill->m_stops)
{
	//{{AFX_DATA_INIT(CFillRadialDlg)

	d_pFill = pfill;
	//}}AFX_DATA_INIT
}

void CFillRadialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFillRadialDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFillRadialDlg, CDialog)
	//{{AFX_MSG_MAP(CFillRadialDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_REDRAWPREVIEW, OnRedrawPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFillRadialDlg message handlers

BOOL CFillRadialDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_GradientDlg.Create(IDD_GRADIENT, (CWnd*)this);
	m_GradientDlg.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);

	SetWindowPos(&wndTop, 8, 45, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LONG CFillRadialDlg::OnRedrawPreview(UINT WPARAM, LONG LPARAM) 
{
	return GetParent()->SendMessage(WM_REDRAWPREVIEW, 0, 0);
}
