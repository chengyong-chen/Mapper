#include "stdafx.h"
#include "Hint.h"
#include "HintDlg.h"
#include "Color.h"
#include "ColorDlg.h"
#include "TextStyleDlg1.h"
#include "TextStyleDlg2.h"

#include "../Dataview/Datainfo.h"
#include "../Dataview/ViewMonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHintDlg dialog

CHintDlg::CHintDlg(CWnd* pParent, CHint& hint)
	: CDialog(CHintDlg::IDD, pParent), m_hint(hint)
{
	//{{AFX_DATA_INIT(CHintDlg)
	m_pTextStyleDlgx = nullptr;
	//}}AFX_DATA_INIT
}

CHintDlg::~CHintDlg()
{
}

void CHintDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHintDlg)
	DDX_Control(pDX, IDC_FONTCOMBO, m_fontcombo);
	DDX_Text(pDX, IDC_FONTSIZE, m_hint.m_fSize);
	DDV_MinMaxFloat(pDX, m_hint.m_fSize, 1.0f, 1000.f);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHintDlg, CDialog)
	//{{AFX_MSG_MAP(CHintDlg)
	ON_WM_DESTROY()
	ON_WM_DRAWITEM()
	ON_BN_DOUBLECLICKED(IDC_COLOR, OnDCColor)
	ON_CBN_SELCHANGE(IDC_FILTERS, OnChangeStyle)
	ON_CBN_SELCHANGE(IDC_FONTCOMBO, OnChangeFont)
	ON_EN_CHANGE(IDC_FONTSIZE, OnContextChanged)
	ON_EN_CHANGE(IDC_HORZSCALE, OnContextChanged)
	ON_NOTIFY(NM_CLICK, IDC_BORDCOLOR, OnNotified)
	ON_NOTIFY(BN_CLICKED, IDC_GROUND, OnNotified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHintDlg message handlers

BOOL CHintDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CBitmap bmp;
	BITMAP bitmapData;
	bmp.LoadBitmap(MAKEINTRESOURCE(IDB_ANNOTYPES));
	bmp.GetBitmap(&bitmapData);
	m_imagelist.Create(18, 18, bitmapData.bmBitsPixel, bmp.GetBitmapDimension().cx/18, 0);
	m_imagelist.Add(&bmp, RGB(254, 254, 254));
	m_imagelist.SetBkColor(RGB(255, 255, 255));

	m_fontcombo.LoadFontList(DEFAULT_CHARSET);
	m_fontcombo.MatchFont(&m_hint.m_fontdesc);

	CComboBox* pComboBoxStyles = ((CComboBox*)GetDlgItem(IDC_FONTSTYLES));
	if (pComboBoxStyles != nullptr)
	{
		const int nIndex1 = pComboBoxStyles->AddString(_T("Regular"));
		pComboBoxStyles->SetItemData(nIndex1, Gdiplus::FontStyle::FontStyleRegular);
		const int nIndex2 = pComboBoxStyles->AddString(_T("Italic"));
		pComboBoxStyles->SetItemData(nIndex2, Gdiplus::FontStyle::FontStyleItalic);
		const int nIndex3 = pComboBoxStyles->AddString(_T("Bold"));
		pComboBoxStyles->SetItemData(nIndex3, Gdiplus::FontStyle::FontStyleBold);
		const int nIndex4 = pComboBoxStyles->AddString(_T("Underline"));
		pComboBoxStyles->SetItemData(nIndex4, Gdiplus::FontStyle::FontStyleUnderline);
		const int nIndex5 = pComboBoxStyles->AddString(_T("Strikeout"));
		pComboBoxStyles->SetItemData(nIndex5, Gdiplus::FontStyle::FontStyleStrikeout);
		const int nIndex6 = pComboBoxStyles->AddString(_T("Bold Italic"));
		pComboBoxStyles->SetItemData(nIndex6, Gdiplus::FontStyle::FontStyleBoldItalic);
	}
	for (int index = 0; index < pComboBoxStyles->GetCount(); index++)
	{
		if (pComboBoxStyles->GetItemData(index) == m_hint.m_fontdesc.m_style)
		{
			pComboBoxStyles->SetCurSel(index);
			break;
		}
	}

	CComboBoxEx* pComboBoxEx = ((CComboBoxEx*)GetDlgItem(IDC_FILTERS));
	if (pComboBoxEx != nullptr)
	{
		pComboBoxEx->SetImageList(&m_imagelist);

		COMBOBOXEXITEM cbi;
		cbi.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_TEXT | CBEIF_LPARAM;// | CBEIF_INDENT;
		cbi.iItem = -1;

		cbi.pszText = (LPTSTR)_T("None");
		cbi.iImage = 0;
		cbi.iSelectedImage = 0;
		cbi.lParam = CHint::Filter::None;
		const int index0 = pComboBoxEx->InsertItem(&cbi);
		pComboBoxEx->SetItemHeight(index0, 20);

		cbi.pszText = (LPTSTR)_T("Frame");
		cbi.iImage = 1;
		cbi.iSelectedImage = 1;
		cbi.lParam = CHint::Filter::None;
		const int index1 = pComboBoxEx->InsertItem(&cbi);
		pComboBoxEx->SetItemHeight(index1, 20);

		cbi.pszText = (LPTSTR)_T("Outline");
		cbi.iImage = 2;
		cbi.iSelectedImage = 2;
		cbi.lParam = CHint::Filter::Outline;
		const int index2 = pComboBoxEx->InsertItem(&cbi);
		pComboBoxEx->SetItemHeight(index2, 20);

		//cbi.pszText = _T("Shadow");
		//cbi.iImage = 3;
		//cbi.iSelectedImage = 3;
		//cbi.lParam = CHint::Filter::Shadow;
		//int index3 = pComboBoxEx->InsertItem(&cbi);
		//pComboBoxEx->SetItemHeight(index3,20);

		pComboBoxEx->SetCurSel(m_hint.m_filter);
	}

	OnChangeStyle();
	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CHintDlg::OnDestroy()
{
	if (m_imagelist.m_hImageList != nullptr)
	{
		m_imagelist.DeleteImageList();
		m_imagelist.Detach();
	}

	if (m_pTextStyleDlgx != nullptr)
	{
		m_pTextStyleDlgx->PostMessage(WM_DESTROY, 0, 0);
		m_pTextStyleDlgx->DestroyWindow();
		delete m_pTextStyleDlgx;
		m_pTextStyleDlgx = nullptr;
	}

	CDialog::OnDestroy();
}

void CHintDlg::OnChangeFont()
{
	CFontDesc* pFontDesc = m_fontcombo.GetCurSelDesc();
	if (pFontDesc != nullptr)
	{
		m_hint.m_fontdesc = *pFontDesc;
		const CStringA strReal = CFontDesc::GetRealByFace(pFontDesc->GetFaceName());
		m_hint.m_fontdesc.SetRealName(strReal);
		OnContextChanged();
	}
}

void CHintDlg::OnDCColor()
{
	CColorDlg dlg(this, m_hint.m_pColor, RGB(255, 255, 255));
	if (dlg.DoModal() == IDOK)
	{
		delete m_hint.m_pColor;
		m_hint.m_pColor = dlg.d_pColor;
		dlg.d_pColor = nullptr;

		GetDlgItem(IDC_COLOR)->Invalidate();
		OnContextChanged();
	}
}

void CHintDlg::OnOK()
{
	CComboBox* pComboBoxStyles = ((CComboBox*)GetDlgItem(IDC_FONTSTYLES));
	if (pComboBoxStyles != nullptr)
	{
		const int nIndex = pComboBoxStyles->GetCurSel();
		m_hint.m_fontdesc.m_style = (Gdiplus::FontStyle)pComboBoxStyles->GetItemData(nIndex);
	}

	CDialog::OnOK();
}

void CHintDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	if (lpDrawItemStruct->itemID == LB_ERR)
		return;

	switch (lpDrawItemStruct->CtlID)
	{
	case IDC_COLOR:
	{
		CColor* color = m_hint.m_pColor;
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		CRect rect = lpDrawItemStruct->rcItem;
		rect.InflateRect(-1, -1);
		if (color != nullptr)
			pDC->FillSolidRect(rect, color->GetMonitorRGB());
		else
			pDC->FillSolidRect(rect, RGB(128, 128, 128));
	}
	break;
	case IDC_PREVIEW:
	{
		CRect box = lpDrawItemStruct->rcItem;
		box.DeflateRect(1, 1);
		CRgn rgn;
		rgn.CreateRectRgnIndirect(&box);
		::SelectClipRgn(lpDrawItemStruct->hDC, (HRGN)rgn.m_hObject);

		Gdiplus::Graphics g(lpDrawItemStruct->hDC);
		g.Clear(Gdiplus::Color::AntiqueWhite);
		CDatainfo datainfo;
		CViewMonitor viewinfo(datainfo);
		viewinfo.m_ptViewPos = CPoint(0, 0);
		viewinfo.m_sizeView = box.Size();
		viewinfo.m_xFactorMapToView = 1.f;
		viewinfo.m_yFactorMapToView = 1.f;
		Gdiplus::PointF center = Gdiplus::PointF(box.CenterPoint().x, box.CenterPoint().y);
		m_hint.DrawString(g, viewinfo, _T("Diwatu"), center, 0, HALIGN::HA_CENTER, VALIGN::VA_CENTER);

		rgn.DeleteObject();
	}
	break;
	}

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CHintDlg::OnChangeStyle()
{
	int index = ((CComboBoxEx*)GetDlgItem(IDC_FILTERS))->GetCurSel();
	if (index == -1)
		return;

	if (m_pTextStyleDlgx != nullptr)
	{
		m_pTextStyleDlgx->PostMessage(WM_DESTROY, 0, 0);
		m_pTextStyleDlgx->DestroyWindow();
		delete m_pTextStyleDlgx;
		m_pTextStyleDlgx = nullptr;
	}

	switch (index)
	{
	case 0X01:
	{
		CTextStyleDlg1* pDlg = new CTextStyleDlg1((CWnd*)this, m_hint.m_frame);
		pDlg->Create(IDD_TEXTSTYLE1, (CWnd*)this);
		m_pTextStyleDlgx = pDlg;
	}
	break;
	case 0X02:
	{
		CTextStyleDlg2* pDlg = new CTextStyleDlg2((CWnd*)this, m_hint.m_outline);
		pDlg->Create(IDD_TEXTSTYLE2, (CWnd*)this);
		m_pTextStyleDlgx = pDlg;
	}
	break;
	}

	if (m_pTextStyleDlgx != nullptr && m_pTextStyleDlgx->m_hWnd != nullptr)
	{
		CRect rect;
		((CWnd*)GetDlgItem(IDC_GROUPBOX))->GetWindowRect(&rect);
		this->ScreenToClient(rect);
		m_pTextStyleDlgx->SetWindowPos(&wndTop, rect.left + 10, rect.top + 15, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOSIZE);
	}

	m_hint.m_filter = (CHint::Filter)index;

	OnContextChanged();
}

void CHintDlg::OnContextChanged()
{
	if (m_pTextStyleDlgx != nullptr)
	{
		m_pTextStyleDlgx->UpdateData();
	}
	UpdateData();
	GetDlgItem(IDC_PREVIEW)->Invalidate();
}

void CHintDlg::OnNotified(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnContextChanged();
}
