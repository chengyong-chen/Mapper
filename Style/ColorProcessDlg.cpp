#include "stdafx.h"
#include "Global.h"
#include "resource.h"

#include "Color.h"
#include "ColorProcess.h"
#include "ColorProcessDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorProcessDlg dialog

CColorProcessDlg::CColorProcessDlg(CWnd* pParent /*=nullptr*/, CColorProcess* pcolor)
	: CDialog(CColorProcessDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorProcessDlg)

	d_pColor = pcolor;
	//}}AFX_DATA_INIT
}

void CColorProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorProcessDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CColorProcessDlg, CDialog)
	//{{AFX_MSG_MAP(CColorProcessDlg)
	ON_EN_CHANGE(IDC_EDITC, OnChangeEditC)
	ON_EN_CHANGE(IDC_EDITM, OnChangeEditM)
	ON_EN_CHANGE(IDC_EDITY, OnChangeEditY)
	ON_EN_CHANGE(IDC_EDITK, OnChangeEditK)
	ON_EN_CHANGE(IDC_EDITA, OnChangeEditA)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorProcessDlg message handlers

BOOL CColorProcessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	TCHAR ch[4];

	CScrollBar* pSC = (CScrollBar*)GetDlgItem(IDC_SCROLLC);
	pSC->SetScrollRange(0, 100);
	pSC->SetScrollPos(d_pColor->m_bC);
	_itot(d_pColor->m_bC, ch, 10);
	GetDlgItem(IDC_EDITC)->SetWindowText(ch);

	CScrollBar* pSM = (CScrollBar*)GetDlgItem(IDC_SCROLLM);
	pSM->SetScrollRange(0, 100);
	pSM->SetScrollPos(d_pColor->m_bM);
	_itot(d_pColor->m_bM, ch, 10);
	GetDlgItem(IDC_EDITM)->SetWindowText(ch);

	CScrollBar* pSY = (CScrollBar*)GetDlgItem(IDC_SCROLLY);
	pSY->SetScrollRange(0, 100);
	pSY->SetScrollPos(d_pColor->m_bY);
	_itot(d_pColor->m_bY, ch, 10);
	GetDlgItem(IDC_EDITY)->SetWindowText(ch);

	CScrollBar* pSK = (CScrollBar*)GetDlgItem(IDC_SCROLLK);
	pSK->SetScrollRange(0, 100);
	pSK->SetScrollPos(d_pColor->m_bK);
	_itot(d_pColor->m_bK, ch, 10);
	GetDlgItem(IDC_EDITK)->SetWindowText(ch);

	CScrollBar* pSA = (CScrollBar*)GetDlgItem(IDC_SCROLLA);
	pSA->SetScrollRange(0, 255);
	pSA->SetScrollPos(d_pColor->m_bAlpha);
	_itot(d_pColor->m_bAlpha, ch, 10);
	GetDlgItem(IDC_EDITA)->SetWindowText(ch);

	SetWindowPos(&wndTop, 2, 35, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CColorProcessDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

	pScrollBar->SetScrollPos(nPos);

	TCHAR ch[4];
	_itot(nPos, ch, 10);
	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SCROLLC:
		d_pColor->m_bC = nPos;
		GetDlgItem(IDC_EDITC)->SetWindowText(ch);
		break;
	case IDC_SCROLLM:
		d_pColor->m_bM = nPos;
		GetDlgItem(IDC_EDITM)->SetWindowText(ch);
		break;
	case IDC_SCROLLY:
		d_pColor->m_bY = nPos;
		GetDlgItem(IDC_EDITY)->SetWindowText(ch);
		break;
	case IDC_SCROLLK:
		d_pColor->m_bK = nPos;
		GetDlgItem(IDC_EDITK)->SetWindowText(ch);
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

void CColorProcessDlg::OnChangeEditC()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItem(IDC_EDITC)->GetWindowText(str);
	d_pColor->m_bC = _ttoi(str);
	d_pColor->m_bC %= 101;
	CScrollBar* pSC = (CScrollBar*)GetDlgItem(IDC_SCROLLC);
	pSC->SetScrollPos(d_pColor->m_bC);
	const CWnd* wnd = GetParent();
	wnd->SendMessage(WM_DRAWCOLOR, 0, 0);
}

void CColorProcessDlg::OnChangeEditM()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItem(IDC_EDITM)->GetWindowText(str);
	d_pColor->m_bM = _ttoi(str);
	d_pColor->m_bM %= 101;
	CScrollBar* pSM = (CScrollBar*)GetDlgItem(IDC_SCROLLM);
	pSM->SetScrollPos(d_pColor->m_bM);
	const CWnd* wnd = GetParent();
	wnd->SendMessage(WM_DRAWCOLOR, 0, 0);
}

void CColorProcessDlg::OnChangeEditY()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItem(IDC_EDITY)->GetWindowText(str);
	d_pColor->m_bY = _ttoi(str);
	d_pColor->m_bY %= 101;
	CScrollBar* pSY = (CScrollBar*)GetDlgItem(IDC_SCROLLY);
	pSY->SetScrollPos(d_pColor->m_bY);
	const CWnd* wnd = GetParent();
	wnd->SendMessage(WM_DRAWCOLOR, 0, 0);
}

void CColorProcessDlg::OnChangeEditK()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItem(IDC_EDITK)->GetWindowText(str);
	d_pColor->m_bK = _ttoi(str);
	d_pColor->m_bK %= 101;
	CScrollBar* pSK = (CScrollBar*)GetDlgItem(IDC_SCROLLK);
	pSK->SetScrollPos(d_pColor->m_bK);
	const CWnd* wnd = GetParent();
	wnd->SendMessage(WM_DRAWCOLOR, 0, 0);
}

void CColorProcessDlg::OnChangeEditA()
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
