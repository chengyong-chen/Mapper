#include "stdafx.h"

#include "SpotCtrl.h"

#include "Spot.h"
#include "SpotSymbol.h"
#include "SpotSymbolDlg.h"
#include "SpotFont.h"
#include "SpotFontDlg.h"
#include "SpotPattern.h"
#include "SpotPatternDlg.h"
#include "SpotFlash.h"
#include "SpotFlashDlg.h"
#include "Library.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSpotCtrl::CSpotCtrl(CWnd* pParent, bool bComplex, CLibrary& library, CSpot* pSpot)
	: CDialog(CSpotCtrl::IDD, pParent), m_library(library), m_bComplex(bComplex)
{
	//{{AFX_DATA_INIT(CSpotCtrl)

	d_pSpot = nullptr;
	d_pSpotdlg = nullptr;
	//}}AFX_DATA_INIT

	if(pSpot!=nullptr)
		d_pSpot = pSpot->Clone();
	else
		d_pSpot = nullptr;
}

CSpotCtrl::~CSpotCtrl()
{
	delete d_pSpot;
}

void CSpotCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpotCtrl)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSpotCtrl, CDialog)
	//{{AFX_MSG_MAP(CSpotCtrl)
	ON_CBN_SELCHANGE(IDC_TYPECOMBO, OnTypeSelchange)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpotCtrl message handlers

BOOL CSpotCtrl::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBoxEx* pComboBoxEx = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO));
	if(pComboBoxEx!=nullptr)
	{
		CBitmap bmp;
		BITMAP bitmapData;
		bmp.LoadBitmap(MAKEINTRESOURCE(IDB_SPOTTYPES));
		bmp.GetBitmap(&bitmapData);
		m_imagelist.Create(18, 18, bitmapData.bmBitsPixel, bmp.GetBitmapDimension().cx/18, 0);
		m_imagelist.Add(&bmp, RGB(254, 254, 254));
		m_imagelist.SetBkColor(RGB(255, 255, 255));
		pComboBoxEx->SetImageList(&m_imagelist);

		COMBOBOXEXITEM cbi;
		cbi.mask = CBEIF_IMAGE|CBEIF_SELECTEDIMAGE|CBEIF_TEXT|CBEIF_LPARAM;// | CBEIF_INDENT;
		cbi.iItem = -1;

		CString string;
		if(string.LoadString(IDS_SPOTNONE)==TRUE)
		{
			cbi.pszText = (LPTSTR)(LPCTSTR)string;
			cbi.iImage = 0;
			cbi.iSelectedImage = 0;
			cbi.lParam = -1;
			const int index = pComboBoxEx->InsertItem(&cbi);
			pComboBoxEx->SetItemHeight(index, 20);
		}
		if(string.LoadString(IDS_SPOTFONT)==TRUE)
		{
			cbi.pszText = (LPTSTR)(LPCTSTR)string;
			cbi.iImage = 1;
			cbi.iSelectedImage = 1;
			cbi.lParam = 2;
			const int index = pComboBoxEx->InsertItem(&cbi);
			pComboBoxEx->SetItemHeight(index, 20);
		}

		if(m_bComplex==true)
		{
			if(string.LoadString(IDS_SPOTSYMBOL)==TRUE)
			{
				cbi.pszText = (LPTSTR)(LPCTSTR)string;
				cbi.iImage = 2;
				cbi.iSelectedImage = 2;
				cbi.lParam = 1;
				const int index = pComboBoxEx->InsertItem(&cbi);
				pComboBoxEx->SetItemHeight(index, 20);
			}
			if(string.LoadString(IDS_SPOTPATTERN)==TRUE)
			{
				cbi.pszText = (LPTSTR)(LPCTSTR)string;
				cbi.iImage = 3;
				cbi.iSelectedImage = 3;
				cbi.lParam = 3;
				const int index = pComboBoxEx->InsertItem(&cbi);
				pComboBoxEx->SetItemHeight(index, 20);
			}
			//if(string.LoadString(IDS_SPOTFLASH) == TRUE)
			//{
			//	cbi.pszText = (LPTSTR)(LPCTSTR)string;
			//	cbi.iImage = 4;
			//	cbi.iSelectedImage = 4;
			//	cbi.lParam = 4;
			//	int index = pComboBoxEx->InsertItem(&cbi);
			//	pComboBoxEx->SetItemHeight(index,20);
			//}
		}
	}

	CSpot* pSpot = d_pSpot;
	d_pSpot = nullptr;
	Reset(pSpot);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpotCtrl::OnTypeSelchange()
{
	const long index = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->GetCurSel();
	const DWORD data = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->GetItemData(index);
	CSpot* spot = CSpot::Apply((BYTE)data);

	Reset(spot);
}

void CSpotCtrl::OnOK()
{
	if(d_pSpotdlg!=nullptr&&d_pSpotdlg->m_hWnd!=nullptr)
	{
		d_pSpotdlg->SendMessage(WM_COMMAND, IDOK, 0);
	}

	CDialog::OnOK();
}

void CSpotCtrl::OnCancel()
{
	delete d_pSpot;
	d_pSpot = nullptr;

	CDialog::OnCancel();
}

void CSpotCtrl::Reset(CSpot* pSpot)
{
	if(d_pSpot!=nullptr)
	{
		d_pSpot->Decrease(m_library);
		delete d_pSpot;
		d_pSpot = nullptr;
	}

	if(d_pSpotdlg!=nullptr)
	{
		d_pSpotdlg->PostMessage(WM_DESTROY, 0, 0);
		d_pSpotdlg->DestroyWindow();
		delete d_pSpotdlg;
		d_pSpotdlg = nullptr;
	}

	d_pSpot = pSpot;
	const BYTE type = d_pSpot==nullptr ? -1 : d_pSpot->Gettype();
	switch(type)
	{
	case 1:
	{
		d_pSpotdlg = new CSpotSymbolDlg((CWnd*)this, m_library.m_libSpotSymbol, (CSpotSymbol*)d_pSpot);
		d_pSpotdlg->Create(IDD_SPOTSYMBOL, (CWnd*)this);
		d_pSpotdlg->ShowWindow(SW_SHOWNORMAL);

		CString string;
		if(string.LoadString(IDS_SPOTSYMBOL)==TRUE)
		{
			const int nIndex = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->FindStringExact(0, string);
			((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(nIndex);
		}
	}
	break;
	case 2:
	{
		d_pSpotdlg = new CSpotFontDlg((CWnd*)this, (CSpotFont*)d_pSpot);
		d_pSpotdlg->Create(IDD_SPOTFONT, (CWnd*)this);
		d_pSpotdlg->ShowWindow(SW_SHOWNORMAL);

		CString string;
		if(string.LoadString(IDS_SPOTFONT)==TRUE)
		{
			const int nIndex = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->FindStringExact(0, string);
			((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(nIndex);
		}
	}
	break;
	case 3:
	{
		d_pSpotdlg = new CSpotPatternDlg((CWnd*)this, m_library.m_libSpotPattern, (CSpotPattern*)d_pSpot);
		d_pSpotdlg->Create(IDD_SPOTPATTERN, (CWnd*)this);
		d_pSpotdlg->ShowWindow(SW_SHOWNORMAL);

		CString string;
		if(string.LoadString(IDS_SPOTPATTERN)==TRUE)
		{
			const int nIndex = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->FindStringExact(0, string);
			((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(nIndex);
		}
	}
	break;
	case 4:
	{
		d_pSpotdlg = new CSpotFlashDlg((CWnd*)this, m_library.m_libSpotFlash, (CSpotFlash*)d_pSpot);
		d_pSpotdlg->Create(IDD_SPOTFLASH, (CWnd*)this);
		d_pSpotdlg->ShowWindow(SW_SHOWNORMAL);

		CString string;
		if(string.LoadString(IDS_SPOTFLASH)==TRUE)
		{
			const int nIndex = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->FindStringExact(0, string);
			((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(nIndex);
		}
	}
	break;
	default:
	{
		CString string;
		if(string.LoadString(IDS_SPOTNONE)==TRUE)
		{
			const int nIndex = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->FindStringExact(0, string);
			((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(nIndex);
		}
	}
	break;
	}
}

void CSpotCtrl::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_imagelist.m_hImageList!=nullptr)
	{
		m_imagelist.DeleteImageList();
		m_imagelist.Detach();
	}

	if(d_pSpotdlg!=nullptr)
	{
		d_pSpotdlg->PostMessage(WM_DESTROY, 0, 0);
		d_pSpotdlg->DestroyWindow();
		delete d_pSpotdlg;
		d_pSpotdlg = nullptr;
	}
}
