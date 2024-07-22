#include "stdafx.h"
#include "Geocentric.h"
#include "ParameterCtrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParameterCtrl dialog
CParameterCtrl::CParameterCtrl(CWnd* pParent, CGeocentric*& pGeocentric)
	: CDialog(CParameterCtrl::IDD, pParent), m_pGeocentric(pGeocentric), m_bModifyToPopup(false)
{
	//{{AFX_DATA_INIT(CParameterCtrl)
	//}}AFX_DATA_INIT
	if(pGeocentric != nullptr)
	{
		m_lng0 = pGeocentric->m_lng0;
		m_lat0 = pGeocentric->m_lat0;
		m_lat1 = pGeocentric->m_lat1;
		m_lat2 = pGeocentric->m_lat2;
		m_lats = pGeocentric->m_lats;
		m_lngc = pGeocentric->m_lngc;
		m_lng1 = pGeocentric->m_lng1;
		m_lng2 = pGeocentric->m_lng2;
		m_k0 = pGeocentric->m_k0;
		m_south = pGeocentric->m_south;
	}
}

void CParameterCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParameterCtrl)
	DDX_Text(pDX, IDC_LNG0, m_lng0);
	DDX_Text(pDX, IDC_LNGC, m_lngc);
	DDX_Text(pDX, IDC_LNG1, m_lng1);
	DDX_Text(pDX, IDC_LNG2, m_lng2);
	DDX_Text(pDX, IDC_LAT0, m_lat0);
	DDX_Text(pDX, IDC_LATS, m_lats);
	DDX_Text(pDX, IDC_LAT1, m_lat1);
	DDX_Text(pDX, IDC_LAT2, m_lat2);
	DDX_Text(pDX, IDC_K0, m_k0);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CParameterCtrl, CDialog)
	//{{AFX_MSG_MAP(CParameterCtrl)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParameterCtrl message handlers

BOOL CParameterCtrl::OnInitDialog()
{
	CDialog::OnInitDialog();

	CGeocentric::ProjType* pProjType = CGeocentric::GetProjType(m_pGeocentric->m_keyProj4);
	if(pProjType != nullptr)
	{
		int y = 15;
		if(pProjType->b_lng0 == false)
		{
			GetDlgItem(IDC_STATICLON0)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LNG0)->ShowWindow(SW_HIDE);
		}
		else
		{
			GetDlgItem(IDC_STATICLON0)->SetWindowPos(&wndTop, 3, y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			GetDlgItem(IDC_LNG0)->SetWindowPos(&wndTop, 170, y - 3, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			y += 25;
		}
		if(pProjType->b_lat0 == false)
		{
			GetDlgItem(IDC_STATICLAT0)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LAT0)->ShowWindow(SW_HIDE);
		}
		else
		{
			GetDlgItem(IDC_STATICLAT0)->SetWindowPos(&wndTop, 3, y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			GetDlgItem(IDC_LAT0)->SetWindowPos(&wndTop, 170, y - 3, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			y += 25;
		}
		if(pProjType->b_lats == false)
		{
			GetDlgItem(IDC_STATICLATS)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LATS)->ShowWindow(SW_HIDE);
		}
		else
		{
			GetDlgItem(IDC_STATICLATS)->SetWindowPos(&wndTop, 3, y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			GetDlgItem(IDC_LATS)->SetWindowPos(&wndTop, 170, y - 3, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			y += 25;
		}
		if(pProjType->b_south == false)
		{
			GetDlgItem(IDC_STATICPOLECOMBO)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_POLECOMBO)->ShowWindow(SW_HIDE);
		}
		else
		{
			((CComboBox*)GetDlgItem(IDC_POLECOMBO))->AddString(_T("North Pole"));
			((CComboBox*)GetDlgItem(IDC_POLECOMBO))->SetItemData(0, 0);
			((CComboBox*)GetDlgItem(IDC_POLECOMBO))->AddString(_T("South Pole"));
			((CComboBox*)GetDlgItem(IDC_POLECOMBO))->SetItemData(1, 1);
			((CComboBox*)GetDlgItem(IDC_POLECOMBO))->SetCurSel(m_south);
			GetDlgItem(IDC_STATICPOLECOMBO)->SetWindowPos(&wndTop, 3, y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			GetDlgItem(IDC_POLECOMBO)->SetWindowPos(&wndTop, 170, y - 3, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			y += 25;
		}
		if(pProjType->b_lat1 == false)
		{
			GetDlgItem(IDC_STATICLAT1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LAT1)->ShowWindow(SW_HIDE);
		}
		else
		{
			GetDlgItem(IDC_STATICLAT1)->SetWindowPos(&wndTop, 3, y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			GetDlgItem(IDC_LAT1)->SetWindowPos(&wndTop, 170, y - 3, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			y += 25;
		}
		if(pProjType->b_lat2 == false)
		{
			GetDlgItem(IDC_STATICLAT2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LAT2)->ShowWindow(SW_HIDE);
		}
		else
		{
			GetDlgItem(IDC_STATICLAT2)->SetWindowPos(&wndTop, 3, y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			GetDlgItem(IDC_LAT2)->SetWindowPos(&wndTop, 170, y - 3, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			y += 25;
		}
		if(pProjType->b_lngc == false)
		{
			GetDlgItem(IDC_STATICLONC)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LNGC)->ShowWindow(SW_HIDE);
		}
		else
		{
			GetDlgItem(IDC_STATICLONC)->SetWindowPos(&wndTop, 3, y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			GetDlgItem(IDC_LNGC)->SetWindowPos(&wndTop, 170, y - 3, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			y += 25;
		}
		if(pProjType->b_lng1 == false)
		{
			GetDlgItem(IDC_STATICLON1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LNG1)->ShowWindow(SW_HIDE);
		}
		else
		{
			GetDlgItem(IDC_STATICLAT1)->SetWindowText(_T("Lat of the 1st point:"));
			GetDlgItem(IDC_STATICLON1)->SetWindowPos(&wndTop, 3, y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			GetDlgItem(IDC_LNG1)->SetWindowPos(&wndTop, 170, y - 3, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			y += 25;
		}
		if(pProjType->b_lng2 == false)
		{
			GetDlgItem(IDC_STATICLON2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LNG2)->ShowWindow(SW_HIDE);
		}
		else
		{
			GetDlgItem(IDC_STATICLAT2)->SetWindowText(_T("Lat of the 2nd point:"));
			GetDlgItem(IDC_STATICLON2)->SetWindowPos(&wndTop, 3, y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			GetDlgItem(IDC_LNG2)->SetWindowPos(&wndTop, 170, y - 3, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			y += 25;
		}
		if(pProjType->b_k0 == false)
		{
			GetDlgItem(IDC_STATICK0)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_K0)->ShowWindow(SW_HIDE);
		}
		else
		{
			GetDlgItem(IDC_STATICK0)->SetWindowPos(&wndTop, 3, y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			GetDlgItem(IDC_K0)->SetWindowPos(&wndTop, 170, y - 3, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			y += 25;
		}
	}
	if((m_bModifyToPopup == true) && m_hWnd)
	{
		CRect rectFrame, rectDlg;
		CWnd* pMainWnd = AfxGetMainWnd();
		if(pMainWnd != nullptr)
		{
			pMainWnd->GetClientRect(rectFrame);
			pMainWnd->ClientToScreen(rectFrame);
			GetWindowRect(rectDlg);
			const int nXPos = rectFrame.left + (rectFrame.Width()/2) - (rectDlg.Width()/2);
			const int nYPos = rectFrame.top + (rectFrame.Height()/2) - (rectDlg.Height()/2);

			::SetWindowPos(m_hWnd, HWND_TOP, nXPos, nYPos, rectDlg.Width(), rectDlg.Height() + 20, SWP_NOCOPYBITS);
		}
	}

	return TRUE;
}

void CParameterCtrl::PreSubclassWindow()
{
	if(m_bModifyToPopup == true)
	{
		if(m_hWnd != nullptr)
		{
			LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);

			lStyle &= ~WS_CHILD;
			lStyle &= ~DS_CONTROL;
			//		lStyle &= ~WS_DISABLED;		//remove the DISABLED style

			lStyle |= WS_POPUP; //Add the POPUP style
			lStyle |= WS_VISIBLE; //Add the VISIBLE style
			lStyle |= WS_SYSMENU; //Add the SYSMENU to have a close button
			lStyle |= WS_CAPTION;
			SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
			CDialog::CenterWindow();
		}
	}

	CDialog::PreSubclassWindow();
}

void CParameterCtrl::OnOK()
{
	CDialog::OnOK();

	m_south = ((CComboBox*)GetDlgItem(IDC_POLECOMBO))->GetCurSel();	
	if(m_pGeocentric != nullptr)
	{
		m_pGeocentric->m_lng0 = m_lng0;
		m_pGeocentric->m_lat0 = m_lat0;
		m_pGeocentric->m_lat1 = m_lat1;
		m_pGeocentric->m_lat2 = m_lat2;
		m_pGeocentric->m_lats = m_lats;
		m_pGeocentric->m_lngc = m_lngc;
		m_pGeocentric->m_lng1 = m_lng1;
		m_pGeocentric->m_lng2 = m_lng2;
		m_pGeocentric->m_k0 = m_k0;
		m_pGeocentric->m_south = m_south;		
	}
}
