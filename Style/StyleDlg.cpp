#include "stdafx.h"
#include "StyleDlg.h"

#include "Line.h"
#include "LineCtrl.h"
#include "Fill.h"
#include "FillCtrl.h"
#include "Library.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStyleDlg dialog

CStyleDlg::CStyleDlg(CWnd* pParent, bool bComplex, CLibrary& library, CLine* pline, CFill* pfill)
	: CDialog(CStyleDlg::IDD, pParent), m_LineCtrl(nullptr, bComplex, library), m_FillCtrl(nullptr, bComplex, library)
{
	d_pFill = nullptr;
	d_pLine = nullptr;

	if(pfill!=nullptr)
	{
		d_pFill = pfill->Clone();
	}

	if(pline!=nullptr)
	{
		d_pLine = pline->Clone();
	}
}

void CStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStyleDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CStyleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_FillCtrl.Create(IDD_FILLCTRL, (CWnd*)this);
	m_FillCtrl.ShowWindow(SW_SHOWNORMAL);
	m_FillCtrl.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	m_FillCtrl.Reset(d_pFill);

	m_LineCtrl.Create(IDD_LINECTRL, (CWnd*)this);
	m_LineCtrl.ShowWindow(SW_SHOWNORMAL);
	m_LineCtrl.SetWindowPos(&wndTop, 190, 0, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	m_LineCtrl.Reset(d_pLine);

	const CRect rect1 = CRect(0, 0, 190, 302);
	const CRect rect2 = CRect(140, 0, 190, 45);

	CRgn rgn1;
	CRgn rgn2;
	rgn1.CreateRectRgnIndirect(&rect1);
	rgn2.CreateRectRgnIndirect(&rect2);
	rgn1.CombineRgn(&rgn1, &rgn2, RGN_XOR);
	m_LineCtrl.SetWindowRgn(rgn1, TRUE);
	rgn1.DeleteObject();
	rgn2.DeleteObject();

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE	
}

BEGIN_MESSAGE_MAP(CStyleDlg, CDialog)
	//{{AFX_MSG_MAP(CStyleDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CStyleDlg::OnOK()
{
	m_FillCtrl.SendMessage(WM_COMMAND, IDOK, 0);
	d_pFill = m_FillCtrl.d_pFill;
	m_FillCtrl.d_pFill = nullptr;

	m_LineCtrl.SendMessage(WM_COMMAND, IDOK, 0);
	d_pLine = m_LineCtrl.d_pLine;
	m_LineCtrl.d_pLine = nullptr;

	CDialog::OnOK();
}

void CStyleDlg::OnDestroy()
{
	CDialog::OnDestroy();

	m_FillCtrl.PostMessage(WM_DESTROY, 0, 0);
	m_FillCtrl.DestroyWindow();
	m_LineCtrl.PostMessage(WM_DESTROY, 0, 0);
	m_LineCtrl.DestroyWindow();
}
