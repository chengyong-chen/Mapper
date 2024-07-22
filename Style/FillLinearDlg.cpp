#include "stdafx.h"
#include "Global.h"
#include "FillLinearDlg.h"
#include "FillLinear.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFillLinearDlg dialog

CFillLinearDlg::CFillLinearDlg(CWnd* pParent /*=nullptr*/, CFillLinear* pfill)
	: CDialog(CFillLinearDlg::IDD, pParent), m_GradientDlg(nullptr, pfill->m_stops)
{
	//{{AFX_DATA_INIT(CFillLinearDlg)

	d_pFill = pfill;
	//}}AFX_DATA_INIT
}

void CFillLinearDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFillLinearDlg)
	DDX_Text(pDX, IDC_ANGLE, d_pFill->m_nAngle);
	DDV_MinMaxInt(pDX, d_pFill->m_nAngle, 0, 359);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFillLinearDlg, CDialog)
	//{{AFX_MSG_MAP(CFillLinearDlg)	
	ON_EN_CHANGE(IDC_ANGLE, OnEnChangeAngle)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_REDRAWPREVIEW, OnRedrawPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFillLinearDlg message handlers

BOOL CFillLinearDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_GradientDlg.Create(IDD_GRADIENT, (CWnd*)this);
	m_GradientDlg.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);

	SetWindowPos(&wndTop, 8, 45, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFillLinearDlg::OnEnChangeAngle()
{
	CDialog::UpdateData();

	OnRedrawPreview(0, 0);
}

LONG CFillLinearDlg::OnRedrawPreview(UINT WPARAM, LONG LPARAM)
{
	return GetParent()->SendMessage(WM_REDRAWPREVIEW, 0, 0);
}
