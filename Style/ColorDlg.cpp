#include "stdafx.h"
#include "Global.h"

#include "Color.h"
#include "ColorDlg.h"
#include "ColorProcessDlg.h"
#include "ColorSpot.h"
#include "ColorSpotDlg.h"
#include "ColorGrayDlg.h"
#include "ColorProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CColorDlg::CColorDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CColorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorDlg)
	m_strName = _T("new");
	//}}AFX_DATA_INIT

	d_pDlg = nullptr;
	d_pColor = nullptr;
}

CColorDlg::CColorDlg(CWnd* pParent, CColor* pcolor, COLORREF defaultRGB)
	: CDialog(CColorDlg::IDD, pParent)
{
	d_pDlg = nullptr;
	d_pColor = nullptr;

	if(pcolor!=nullptr)
	{
		d_oldcolor = pcolor->GetColor();
		d_pColor = pcolor->Clone();
	}
	else
	{
		d_oldcolor = defaultRGB;
		d_pColor = new CColorSpot(GetRValue(defaultRGB), GetGValue(defaultRGB), GetBValue(defaultRGB),255);
		m_strName = _T("New");
	}
}

CColorDlg::~CColorDlg()
{
	delete d_pColor;
}

void CColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CColorDlg, CDialog)
	//{{AFX_MSG_MAP(CColorDlg)
	ON_CBN_SELCHANGE(IDC_TYPECOMBO, OnTypeSelchange)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_DRAWCOLOR, OnDrawNew)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorDlg message handlers

BOOL CColorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(d_pColor!=nullptr)
	{
		const BYTE type = d_pColor->Gettype();

		((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(type-1);

		switch(type)
		{
		case 1:
		{
			d_pDlg = new CColorProcessDlg((CWnd*)this, (CColorProcess*)d_pColor);
			d_pDlg->Create(IDD_COLORPROCESS, (CWnd*)this);
			d_pDlg->ShowWindow(SW_SHOWNORMAL);
		}
		break;
		case 2:
		{
			d_pDlg = new CColorSpotDlg((CWnd*)this, (CColorSpot*)d_pColor);
			d_pDlg->Create(IDD_COLORSPOT, (CWnd*)this);
			d_pDlg->ShowWindow(SW_SHOWNORMAL);
		}
		break;
		case 3:
		{
			d_pDlg = new CColorGrayDlg((CWnd*)this, (CColorGray*)d_pColor);
			d_pDlg->Create(IDD_COLORGRAY, (CWnd*)this);
			d_pDlg->ShowWindow(SW_SHOWNORMAL);
		}
		break;
		}
	}
	else
	{
		d_pDlg = nullptr;
	}

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control	              // EXCEPTION: OCX Property Pages should return FALSE
}

LONG CColorDlg::OnDrawNew(UINT WPARAM, LONG LPARAM)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CWnd* pWnd = GetDlgItem(IDC_NEWCOLOR);
	if(pWnd!=nullptr)
	{
		CRect rect;
		pWnd->GetClientRect(rect);
		rect.InflateRect(-1, -1);

		const CWindowDC* pDC = new CWindowDC(pWnd);
		if(d_pColor!=nullptr)
		{
			Gdiplus::Bitmap bitmap(AfxGetInstanceHandle(), (PWCHAR)MAKEINTRESOURCE(IDB_BACKGROUND));
			const Gdiplus::TextureBrush brush1(&bitmap, Gdiplus::WrapMode::WrapModeTile);
			const Gdiplus::Color color = d_pColor->GetColor();
			const Gdiplus::SolidBrush brush2(color);
			Gdiplus::Graphics g(pDC->m_hDC);
			g.FillRectangle(&brush1, rect.left, rect.top, rect.Width(), rect.Height());
			g.FillRectangle(&brush2, rect.left, rect.top, rect.Width(), rect.Height());
		}

		delete pDC;
		pDC = nullptr;
	}
	return 0L;
}

void CColorDlg::DrawOld() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CRect rect;
	CWnd* wnd = ((CWnd*)GetDlgItem(IDC_OLDCOLOR));
	wnd->GetClientRect(rect);
	rect.InflateRect(-1, -1);
	const CWindowDC* pDC = new CWindowDC(wnd);

	Gdiplus::Bitmap bitmap(AfxGetInstanceHandle(), (PWCHAR)MAKEINTRESOURCE(IDB_BACKGROUND));
	const Gdiplus::TextureBrush brush1(&bitmap, Gdiplus::WrapMode::WrapModeTile);
	const Gdiplus::SolidBrush brush2(d_oldcolor);
	Gdiplus::Graphics g(pDC->m_hDC);
	g.FillRectangle(&brush1, rect.left, rect.top, rect.Width(), rect.Height());
	g.FillRectangle(&brush2, rect.left, rect.top, rect.Width(), rect.Height());

	delete pDC;
}

void CColorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	DrawOld();
	OnDrawNew(0, 0);
}

void CColorDlg::OnTypeSelchange()
{
	const long index = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->GetCurSel();
	CColor* color = CColor::Apply(index+1);
	if(color==nullptr)
	{
	}
	else if(d_pColor==nullptr)
	{
	}
	else if(color->IsKindOf(RUNTIME_CLASS(CColorProcess))==TRUE)
	{
		const DWORD cmyk = d_pColor->ToCMYK();
		const DWORD c = cmyk&0XFF000000;
		const DWORD m = cmyk&0X00FF0000;
		const DWORD y = cmyk&0X0000FF00;
		const DWORD k = cmyk&0X000000FF;
		((CColorProcess*)color)->m_bC = c>>24;
		((CColorProcess*)color)->m_bM = m>>16;
		((CColorProcess*)color)->m_bY = y>>8;;
		((CColorProcess*)color)->m_bK = k>>0;
		((CColorProcess*)color)->m_bAlpha = d_pColor->m_bAlpha;
	}
	else if(color->IsKindOf(RUNTIME_CLASS(CColorSpot))==TRUE)
	{
		const COLORREF rgb = d_pColor->ToRGB();
		((CColorSpot*)color)->m_bR = GetRValue(rgb);
		((CColorSpot*)color)->m_bG = GetGValue(rgb);
		((CColorSpot*)color)->m_bB = GetBValue(rgb);
		((CColorSpot*)color)->m_bAlpha = d_pColor->m_bAlpha;
		//		CString str;
		//		GetDlgItem(IDC_EDITA)->GetWindowText(str);
		//		int nTransparency = _ttoi(str);
		//		d_pColor->m_bAlpha = (100-nTransparency)*255/100;
	}

	Reset(color);
}

void CColorDlg::OnOK()
{
	if(d_pDlg!=nullptr&&d_pDlg->m_hWnd!=nullptr)
	{
		d_pDlg->SendMessage(WM_COMMAND, IDOK, 0);
	}

	if(d_pColor!=nullptr)
	{
		if(d_pColor->IsKindOf(RUNTIME_CLASS(CColorSpot))==TRUE&&d_oldcolor.GetA()==d_pColor->GetColor().GetA()&&d_oldcolor.GetR()==d_pColor->GetColor().GetR()&&d_oldcolor.GetG()==d_pColor->GetColor().GetG()&&d_oldcolor.GetB()==d_pColor->GetColor().GetB())
		{
			delete d_pColor;
			d_pColor = nullptr;

			return CDialog::OnCancel();
		}
	}

	return CDialog::OnOK();
}

void CColorDlg::OnCancel()
{
	delete d_pColor;
	d_pColor = nullptr;

	CDialog::OnCancel();
}

void CColorDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	if(d_pDlg!=nullptr)
	{
		d_pDlg->PostMessage(WM_DESTROY, 0, 0);
		d_pDlg->DestroyWindow();
		delete d_pDlg;
		d_pDlg = nullptr;
	}
}

void CColorDlg::Reset(CColor* pColor)
{
	if(pColor==nullptr)
		return;

	delete d_pColor;
	d_pColor = nullptr;

	if(d_pDlg!=nullptr)
	{
		d_pDlg->PostMessage(WM_DESTROY, 0, 0);
		d_pDlg->DestroyWindow();
		delete d_pDlg;
		d_pDlg = nullptr;
	}

	if(pColor==nullptr)
	{
		return;
	}

	d_pColor = pColor;
	const BYTE type = d_pColor->Gettype();

	((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(type-1);

	switch(type)
	{
	case 1:
	{
		d_pDlg = new CColorProcessDlg((CWnd*)this, (CColorProcess*)d_pColor);
		d_pDlg->Create(IDD_COLORPROCESS, (CWnd*)this);
		d_pDlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	case 2:
	{
		d_pDlg = new CColorSpotDlg((CWnd*)this, (CColorSpot*)d_pColor);
		d_pDlg->Create(IDD_COLORSPOT, (CWnd*)this);
		d_pDlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	case 3:
	{
		d_pDlg = new CColorGrayDlg((CWnd*)this, (CColorGray*)d_pColor);
		d_pDlg->Create(IDD_COLORGRAY, (CWnd*)this);
		d_pDlg->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	}
}
