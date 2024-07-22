#include "stdafx.h"
#include "Global.h"
#include "resource.h"

#include "Color.h"
#include "ColorGray.h"
#include "ColorGrayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorGrayDlg dialog

CColorGrayDlg::CColorGrayDlg(CWnd* pParent /*=nullptr*/, CColorGray* pcolor)
	: CDialog(CColorGrayDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorGrayDlg)

	d_pColor = pcolor;
	//}}AFX_DATA_INIT
}

void CColorGrayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorGrayDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CColorGrayDlg, CDialog)
	//{{AFX_MSG_MAP(CColorGrayDlg)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDITT, OnChangeEditT)
	ON_EN_CHANGE(IDC_EDITA, OnChangeEditA)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorGrayDlg message handlers

BOOL CColorGrayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	TCHAR ch[4];

	CScrollBar* pST = (CScrollBar*)GetDlgItem(IDC_SCROLLT);
	pST->SetScrollRange(0, 255);
	pST->SetScrollPos(d_pColor->m_gray);
	_itot_s(d_pColor->m_gray, ch, 10);
	GetDlgItem(IDC_EDITT)->SetWindowText(ch);
	
	CScrollBar* pSA = (CScrollBar*)GetDlgItem(IDC_SCROLLA);
	pSA->SetScrollRange(0, 255);
	pSA->SetScrollPos(d_pColor->m_bAlpha);
	_itot_s(d_pColor->m_bAlpha, ch, 10);
	GetDlgItem(IDC_EDITA)->SetWindowText(ch);

	SetWindowPos(&wndTop, 2, 35, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CColorGrayDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	ASSERT(pScrollBar);

	if(nSBCode==8)
		return;

	const long oneLine = 1;
	const long onePage = 10;

	const long temp = pScrollBar->GetScrollPos();
	switch(nSBCode)
	{
	case SB_PAGELEFT:
		nPos = temp-onePage;
		break;
	case SB_PAGERIGHT:
		nPos = temp+onePage;
		break;
	case SB_LINERIGHT:
		nPos = temp+oneLine;
		break;
	case SB_LINELEFT:
		if(temp!=0)
			nPos = temp-oneLine;
		break;
	}

	if(nPos<0) nPos = 0;
	if(nPos>255) nPos = 255;

	pScrollBar->SetScrollPos(nPos);

	TCHAR ch[4];
	_itot(nPos, ch, 10);

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SCROLLT:
		d_pColor->m_gray = nPos;
		GetDlgItem(IDC_EDITT)->SetWindowText(ch);
		break;
	case IDC_SCROLLA:
		d_pColor->m_bAlpha = nPos;
		GetDlgItem(IDC_EDITA)->SetWindowText(ch);
		break;
	}

	const CWnd* wnd = GetParent();
	wnd->SendMessage(WM_DRAWCOLOR, 0, 0);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CColorGrayDlg::OnChangeEditT()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItem(IDC_EDITT)->GetWindowText(str);
	d_pColor->m_gray = _ttoi(str);
	d_pColor->m_gray %= 255;
	CScrollBar* pST = (CScrollBar*)GetDlgItem(IDC_SCROLLT);
	pST->SetScrollPos(d_pColor->m_gray);
	const CWnd* wnd = GetParent();
	wnd->SendMessage(WM_DRAWCOLOR, 0, 0);
}

void CColorGrayDlg::OnChangeEditA()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItem(IDC_EDITA)->GetWindowText(str);
	const int nPos = _ttoi(str);
	CScrollBar* pSA = (CScrollBar*)GetDlgItem(IDC_SCROLLA);
	pSA->SetScrollPos(nPos);
	d_pColor->m_bAlpha = nPos;

	const CWnd* wnd = GetParent();
	wnd->SendMessage(WM_DRAWCOLOR, 0, 0);
}
