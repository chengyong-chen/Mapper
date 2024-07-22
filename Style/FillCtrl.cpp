#include "stdafx.h"

#include "Global.h"
#include "FillCtrl.h"

#include "Fill.h"
#include "FillAloneDlg.h"
#include "FillSymbol.h"
#include "FillSymbolDlg.h"
#include "FillLinear.h"
#include "FillLinearDlg.h"
#include "FillRadial.h"
#include "FillRadialDlg.h"
#include "FillPattern.h"
#include "FillPatternDlg.h"
#include "Library.h"

#include "../Geometry/PRect.h"
#include "../Dataview/Datainfo.h"
#include "../Dataview/ViewMonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFillCtrl::CFillCtrl(CWnd* pParent, bool bComplex, CLibrary& library)
	: CDialog(CFillCtrl::IDD, pParent), m_library(library), m_bComplex(bComplex)
{
	//{{AFX_DATA_INIT(CFillCtrl)

	d_pFill = nullptr;
	d_pFilldlg = nullptr;
	//}}AFX_DATA_INIT
}

CFillCtrl::~CFillCtrl()
{
	delete d_pFill;
}

void CFillCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFillCtrl)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFillCtrl, CDialog)
	//{{AFX_MSG_MAP(CFillCtrl)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_TYPECOMBO, OnTypeSelchange)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_REDRAWPREVIEW, OnRedrawPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CFillCtrl::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBoxEx* pComboBoxEx = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO));
	if(pComboBoxEx!=nullptr)
	{
		CBitmap bmp;
		BITMAP bitmapData;
		bmp.LoadBitmap(MAKEINTRESOURCE(IDB_FILLTYPES));
		bmp.GetBitmap(&bitmapData);
		m_imagelist.Create(18, 18, bitmapData.bmBitsPixel, bmp.GetBitmapDimension().cx/18, 0);
		m_imagelist.Add(&bmp, RGB(254, 254, 254));
		m_imagelist.SetBkColor(RGB(255, 255, 255));
		pComboBoxEx->SetImageList(&m_imagelist);

		COMBOBOXEXITEM cbi;
		cbi.mask = CBEIF_IMAGE|CBEIF_SELECTEDIMAGE|CBEIF_TEXT|CBEIF_LPARAM;// | CBEIF_INDENT;
		cbi.iItem = -1;

		CString string;
		if(string.LoadString(IDS_FILLINHERIT)==TRUE)
		{
			cbi.pszText = (LPTSTR)(LPCTSTR)string;
			cbi.iImage = 0;
			cbi.iSelectedImage = 0;
			cbi.lParam = CFill::FILLTYPE::Inherit;
			const int index = pComboBoxEx->InsertItem(&cbi);
			pComboBoxEx->SetItemHeight(index, 20);
		}
		if(string.LoadString(IDS_FILLEMPTY)==TRUE)
		{
			cbi.pszText = (LPTSTR)(LPCTSTR)string;
			cbi.iImage = 1;
			cbi.iSelectedImage = 1;
			cbi.lParam = CFill::FILLTYPE::Blank;
			const int index = pComboBoxEx->InsertItem(&cbi);
			pComboBoxEx->SetItemHeight(index, 20);
		}
		if(string.LoadString(IDS_FILLALONE)==TRUE)
		{
			cbi.pszText = (LPTSTR)(LPCTSTR)string;
			cbi.iImage = 2;
			cbi.iSelectedImage = 2;
			cbi.lParam = CFill::FILLTYPE::Alone;
			const int index = pComboBoxEx->InsertItem(&cbi);
			pComboBoxEx->SetItemHeight(index, 20);
		}
		if(string.LoadString(IDS_FILLLINEARGRADIENT)==TRUE)
		{
			cbi.pszText = (LPTSTR)(LPCTSTR)string;
			cbi.iImage = 4;
			cbi.iSelectedImage = 4;
			cbi.lParam = CFill::FILLTYPE::Linear;
			const int index = pComboBoxEx->InsertItem(&cbi);
			pComboBoxEx->SetItemHeight(index, 20);
		}
		if(string.LoadString(IDS_FILLRADIAL)==TRUE)
		{
			cbi.pszText = (LPTSTR)(LPCTSTR)string;
			cbi.iImage = 3;
			cbi.iSelectedImage = 3;
			cbi.lParam = CFill::FILLTYPE::Radial;
			const int index = pComboBoxEx->InsertItem(&cbi);
			pComboBoxEx->SetItemHeight(index, 20);
		}
		if(m_bComplex==true)
		{
			if(string.LoadString(IDS_FILLPATTERN)==TRUE)
			{
				cbi.pszText = (LPTSTR)(LPCTSTR)string;
				cbi.iImage = 5;
				cbi.iSelectedImage = 5;
				cbi.lParam = CFill::FILLTYPE::Pattern;
				const int index = pComboBoxEx->InsertItem(&cbi);
				pComboBoxEx->SetItemHeight(index, 20);
			}
			if(string.LoadString(IDS_FILLSYMBOL)==TRUE)
			{
				cbi.pszText = (LPTSTR)(LPCTSTR)string;
				cbi.iImage = 6;
				cbi.iSelectedImage = 6;
				cbi.lParam = CFill::FILLTYPE::Symbol;
				const int index = pComboBoxEx->InsertItem(&cbi);
				pComboBoxEx->SetItemHeight(index, 20);
			}
		}
	}

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFillCtrl::OnPaint()
{
	CPaintDC dc(this);

	OnRedrawPreview(0, 0);
}

void CFillCtrl::OnTypeSelchange()
{
	const long index = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->GetCurSel();
	const DWORD data = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->GetItemData(index);
	CFill* fill = CFill::Apply(data);
	Reset(fill);
}

void CFillCtrl::OnOK()
{
	if(d_pFilldlg!=nullptr&&d_pFilldlg->m_hWnd!=nullptr)
	{
		d_pFilldlg->SendMessage(WM_COMMAND, IDOK, 0);
	}

	CDialog::OnOK();
}

void CFillCtrl::OnCancel()
{
	if(d_pFill!=nullptr)
	{
		d_pFill->Decrease(m_library);
		delete d_pFill;
		d_pFill = nullptr;
	}

	CDialog::OnCancel();
}

void CFillCtrl::Reset(CFill* pfill)
{
	if(d_pFill!=nullptr)
	{
		d_pFill->Decrease(m_library);
		delete d_pFill;
		d_pFill = nullptr;
	}

	if(d_pFilldlg!=nullptr)
	{
		d_pFilldlg->PostMessage(WM_DESTROY, 0, 0);
		d_pFilldlg->DestroyWindow();
		delete d_pFilldlg;
		d_pFilldlg = nullptr;
	}

	d_pFill = pfill;
	const CFill::FILLTYPE type = d_pFill==nullptr ? CFill::FILLTYPE::Inherit : d_pFill->Gettype();
	switch(type)
	{
	case CFill::FILLTYPE::Inherit:
	case CFill::FILLTYPE::Blank:
		break;
	case CFill::FILLTYPE::Alone:
	{
		d_pFilldlg = new CFillAloneDlg((CWnd*)this, (CFillAlone*)d_pFill);
		d_pFilldlg->Create(IDD_FILLALONE, (CWnd*)this);
		d_pFilldlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	case CFill::FILLTYPE::Symbol:
	{
		d_pFilldlg = new CFillSymbolDlg((CWnd*)this, m_library.m_libFillSymbol, (CFillSymbol*)d_pFill);
		d_pFilldlg->Create(IDD_FILLSYMBOL, (CWnd*)this);
		d_pFilldlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	case CFill::FILLTYPE::Linear:
	{
		d_pFilldlg = new CFillLinearDlg((CWnd*)this, (CFillLinear*)d_pFill);
		d_pFilldlg->Create(IDD_FILLLINEAR, (CWnd*)this);
		d_pFilldlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	case CFill::FILLTYPE::Radial:
	{
		d_pFilldlg = new CFillRadialDlg((CWnd*)this, (CFillRadial*)d_pFill);
		d_pFilldlg->Create(IDD_FILLRADIAL, (CWnd*)this);
		d_pFilldlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	case CFill::FILLTYPE::Pattern:
	{
		d_pFilldlg = new CFillPatternDlg((CWnd*)this, m_library.m_libFillPattern, (CFillPattern*)d_pFill);
		d_pFilldlg->Create(IDD_FILLPATTERN, (CWnd*)this);
		d_pFilldlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	}
	for(int nIndex = 0; nIndex<((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->GetCount(); nIndex++)
	{
		const int data = ((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->GetItemData(nIndex);
		if(data==type)
		{
			((CComboBoxEx*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(nIndex);
			break;
		}
	}
	SendMessage(WM_REDRAWPREVIEW);
}

void CFillCtrl::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_imagelist.m_hImageList!=nullptr)
	{
		m_imagelist.DeleteImageList();
		m_imagelist.Detach();
	}

	if(d_pFilldlg!=nullptr)
	{
		d_pFilldlg->PostMessage(WM_DESTROY, 0, 0);
		d_pFilldlg->DestroyWindow();
		delete d_pFilldlg;
	}
}

LONG CFillCtrl::OnRedrawPreview(UINT WPARAM, LONG LPARAM)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CWnd* pWnd = GetDlgItem(IDC_SAMPLE);
	if(pWnd!=nullptr)
	{
		CRect rect;
		pWnd->GetClientRect(rect);

		CClientDC dc(pWnd);
		Gdiplus::Graphics g(dc.m_hDC);
		Gdiplus::Bitmap bitmap(AfxGetInstanceHandle(), (PWCHAR)MAKEINTRESOURCE(IDB_BACKGROUND));
		Gdiplus::TextureBrush brush1(&bitmap, Gdiplus::WrapMode::WrapModeTile);
		g.FillRectangle(&brush1, rect.left, rect.top, rect.Width(), rect.Height());
		if(d_pFill!=nullptr)
		{
			Gdiplus::Rect theRect(rect.left, rect.top, rect.Width(), rect.Height());
			Gdiplus::Brush* brush = d_pFill->GetBrush(1, CSize(72, 72));
			if(brush==nullptr)
			{
			}
			else if(brush->GetType()==Gdiplus::BrushTypePathGradient)
			{
				Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
				path.AddRectangle(theRect);

				CDatainfo datainfo;
				CViewMonitor viewinfo(datainfo);
				viewinfo.m_xFactorMapToView = +1.f;
				viewinfo.m_yFactorMapToView = -1.f;
				CPRect prect(CRect(rect.left*10000, rect.top*10000, rect.right*10000, rect.bottom*10000), nullptr, nullptr);
				Gdiplus::PathGradientBrush* local = prect.SetGradientBrush((RadialBrush*)brush, viewinfo, &path);
				g.FillRectangle(local, theRect);
				::delete local;
			}
			else if(brush->GetType()==Gdiplus::BrushTypeLinearGradient)
			{
				CDatainfo datainfo;
				CViewMonitor viewinfo(datainfo);
				viewinfo.m_xFactorMapToView = +1.f;
				viewinfo.m_yFactorMapToView = -1.f;
				CPRect prect(CRect(rect.left*10000, rect.top*10000, rect.right*10000, rect.bottom*10000), nullptr, nullptr);
				Gdiplus::LinearGradientBrush* local = prect.SetGradientBrush((LinearBrush*)brush, viewinfo);
				g.FillRectangle(local, theRect);
				::delete local;
			}
			else
			{
				g.FillRectangle(brush, theRect);
			}

			::delete brush;
			brush = nullptr;
		}
	}

	return 1L;
}
