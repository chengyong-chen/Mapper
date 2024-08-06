#include "stdafx.h"
#include "Global.h"
#include "resource.h"

#include "Color.h"
#include "ColorSpot.h"
#include "ColorSpotDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorSpotDlg dialog

CColorSpotDlg::CColorSpotDlg(CWnd* pParent /*=nullptr*/, CColorSpot* pcolor)
	: CDialog(CColorSpotDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorSpotDlg)
	d_pColor = pcolor;
	//}}AFX_DATA_INIT

	m_bPassive = false;
}

void CColorSpotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorSpotDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CColorSpotDlg, CDialog)
	//{{AFX_MSG_MAP(CColorSpotDlg)
	ON_EN_CHANGE(IDC_EDITB, OnChangeEditB)
	ON_EN_CHANGE(IDC_EDITG, OnChangeEditG)
	ON_EN_CHANGE(IDC_EDITR, OnChangeEditR)
	ON_EN_CHANGE(IDC_EDITA, OnChangeEditA)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorSpotDlg message handlers

BOOL CColorSpotDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndTop, 2, 35, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);

	CScrollBar* pSR = (CScrollBar*)GetDlgItem(IDC_SCROLLR);
	pSR->SetScrollRange(0, 255);

	CScrollBar* pSG = (CScrollBar*)GetDlgItem(IDC_SCROLLG);
	pSG->SetScrollRange(0, 255);

	CScrollBar* pSB = (CScrollBar*)GetDlgItem(IDC_SCROLLB);
	pSB->SetScrollRange(0, 255);

	CScrollBar* pSA = (CScrollBar*)GetDlgItem(IDC_SCROLLA);
	pSA->SetScrollRange(0, 255);

	OnSetRGB(d_pColor->ToRGB());

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CColorSpotDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
	case IDC_SCROLLR:
		d_pColor->m_bR = nPos;
		GetDlgItem(IDC_EDITR)->SetWindowText(ch);
		break;
	case IDC_SCROLLG:
		d_pColor->m_bG = nPos;
		GetDlgItem(IDC_EDITG)->SetWindowText(ch);
		break;
	case IDC_SCROLLB:
		d_pColor->m_bB = nPos;
		GetDlgItem(IDC_EDITB)->SetWindowText(ch);
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

void CColorSpotDlg::OnChangeEditR()
{
	if(m_bPassive!=true)
	{
		CString str;
		GetDlgItem(IDC_EDITR)->GetWindowText(str);
		d_pColor->m_bR = _ttoi(str);
		d_pColor->m_bR %= 256;
		CScrollBar* pSR = (CScrollBar*)GetDlgItem(IDC_SCROLLR);
		pSR->SetScrollPos(d_pColor->m_bR);

		const CWnd* wnd = GetParent();
		wnd->SendMessage(WM_DRAWCOLOR, 0, 0);
	}
}

void CColorSpotDlg::OnChangeEditG()
{
	if(m_bPassive!=true)
	{
		CString str;
		GetDlgItem(IDC_EDITG)->GetWindowText(str);
		d_pColor->m_bG = _ttoi(str);
		d_pColor->m_bG %= 256;
		CScrollBar* pSG = (CScrollBar*)GetDlgItem(IDC_SCROLLG);
		pSG->SetScrollPos(d_pColor->m_bG);

		const CWnd* wnd = GetParent();
		wnd->SendMessage(WM_DRAWCOLOR, 0, 0);
	}
}

void CColorSpotDlg::OnChangeEditB()
{
	if(m_bPassive!=true)
	{
		CString str;
		GetDlgItem(IDC_EDITB)->GetWindowText(str);
		d_pColor->m_bB = _ttoi(str);
		d_pColor->m_bB %= 256;
		CScrollBar* pSB = (CScrollBar*)GetDlgItem(IDC_SCROLLB);
		pSB->SetScrollPos(d_pColor->m_bB);

		const CWnd* wnd = GetParent();
		wnd->SendMessage(WM_DRAWCOLOR, 0, 0);
	}
}

void CColorSpotDlg::OnChangeEditA()
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

void CColorSpotDlg::OnSetRGB(COLORREF rgb)
{
	m_bPassive = true;
	TCHAR ch[4];

	CScrollBar* pSR = (CScrollBar*)GetDlgItem(IDC_SCROLLR);
	d_pColor->m_bR = GetRValue(rgb);
	d_pColor->m_bR %= 256;
	pSR->SetScrollPos(d_pColor->m_bR);
	_itot(d_pColor->m_bR, ch, 10);
	GetDlgItem(IDC_EDITR)->SetWindowText(ch);

	CScrollBar* pSG = (CScrollBar*)GetDlgItem(IDC_SCROLLG);
	d_pColor->m_bG = GetGValue(rgb);
	d_pColor->m_bG %= 256;
	pSG->SetScrollPos(d_pColor->m_bG);
	_itot(d_pColor->m_bG, ch, 10);
	GetDlgItem(IDC_EDITG)->SetWindowText(ch);

	CScrollBar* pSB = (CScrollBar*)GetDlgItem(IDC_SCROLLB);
	d_pColor->m_bB = GetBValue(rgb);
	d_pColor->m_bB %= 256;
	pSB->SetScrollPos(d_pColor->m_bB);
	_itot(d_pColor->m_bB, ch, 10);
	GetDlgItem(IDC_EDITB)->SetWindowText(ch);

	CScrollBar* pSA = (CScrollBar*)GetDlgItem(IDC_SCROLLA);
	pSA->SetScrollPos(d_pColor->m_bAlpha);
	_itot(d_pColor->m_bAlpha, ch, 10);
	GetDlgItem(IDC_EDITA)->SetWindowText(ch);

	const CWnd* wnd = GetParent();
	wnd->SendMessage(WM_DRAWCOLOR, 0, 0);

	m_bPassive = false;
}

void testfunc(COLORREF ref, void* ClientData)
{
	CColorSpotDlg* local = (CColorSpotDlg*)ClientData;

	local->OnSetRGB(ref);
}
