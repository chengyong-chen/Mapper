#include "stdafx.h"
#include "FrameDlg.h"
#include "Resource.h"

#include "../Style/Color.h"
#include "../Style/ColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CFrameDlg, CDialog)
CFrameDlg::CFrameDlg(CWnd* pParent, BOOL enabledSpere, CColor* pSphereColor1, CColor* pSphereColor2, float fSphereWidth, BOOL enabledGraticule, CColor* pGraticuleColor, float fGraticuleWidth, int lngInterval, int latInterval)
	: CDialog(IDD_FRAME, pParent), m_bEnabledSphere(enabledSpere), m_fSphereWidth(fSphereWidth), m_bEnabledGraticule(enabledGraticule), m_fGraticuleWidth(fGraticuleWidth),  m_lngInterval(lngInterval),m_latInterval(latInterval)
{
	m_pSphereColor1=pSphereColor1 != nullptr ? pSphereColor1->Clone() : nullptr;
	m_pSphereColor2 = pSphereColor2 != nullptr ? pSphereColor2->Clone() : nullptr;
	m_pGraticuleColor=pGraticuleColor != nullptr ? pGraticuleColor->Clone() : nullptr;
}

CFrameDlg::~CFrameDlg()
{
}

void CFrameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_SPHEREENABLED, m_bEnabledSphere);
	DDX_Check(pDX, IDC_GRARTICULEENABLED, m_bEnabledGraticule);
	DDX_Text(pDX, IDC_SPHEREWIDTH, m_fSphereWidth);
	DDX_Text(pDX, IDC_GRATICULEWIDTH, m_fGraticuleWidth);
	DDX_Text(pDX, IDC_LNGINTERVAL, m_lngInterval);
	DDX_Text(pDX, IDC_LATINTERVAL, m_latInterval);
	DDV_MinMaxFloat(pDX, m_fSphereWidth, 0.01f, 100.f);
	DDV_MinMaxFloat(pDX, m_fGraticuleWidth, 0.01f, 100.f);
}


BEGIN_MESSAGE_MAP(CFrameDlg, CDialog)
	//{{AFX_MSG_MAP(CStyleDlg)
	ON_BN_DOUBLECLICKED(IDC_SPHERECOLOR1, OnSphereColor1)
	ON_BN_DOUBLECLICKED(IDC_SPHERECOLOR2, OnSphereColor2)
	ON_BN_DOUBLECLICKED(IDC_GRATICULECOLOR, OnGraticuleColor)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFrameDlg::OnSphereColor1()
{
	const HINSTANCE hInstOld=AfxGetResourceHandle();
	const HINSTANCE hInst=::LoadLibrary(_T("Style.dll"));
	AfxSetResourceHandle(hInst);

	CColorDlg dlg(nullptr, m_pSphereColor1, RGB(0, 0, 0));
	if(dlg.DoModal() == IDOK)
	{
		delete m_pSphereColor1;
		m_pSphereColor1=dlg.d_pColor;
		dlg.d_pColor=nullptr;
		GetDlgItem(IDC_SPHERECOLOR1)->Invalidate();
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}
void CFrameDlg::OnSphereColor2()
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
	AfxSetResourceHandle(hInst);

	CColorDlg dlg(nullptr, m_pSphereColor2, RGB(0, 0, 0));
	if(dlg.DoModal() == IDOK)
	{
		delete m_pSphereColor2;
		m_pSphereColor2 = dlg.d_pColor;
		dlg.d_pColor = nullptr;
		GetDlgItem(IDC_SPHERECOLOR2)->Invalidate();
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CFrameDlg::OnGraticuleColor()
{
	const HINSTANCE hInstOld=AfxGetResourceHandle();
	const HINSTANCE hInst=::LoadLibrary(_T("Style.dll"));
	AfxSetResourceHandle(hInst);

	CColorDlg dlg(nullptr, m_pGraticuleColor, RGB(0, 0, 0));
	if(dlg.DoModal() == IDOK)
	{
		delete m_pGraticuleColor;
		m_pGraticuleColor=dlg.d_pColor;
		dlg.d_pColor=nullptr;
		GetDlgItem(IDC_GRATICULECOLOR)->Invalidate();
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CFrameDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(lpDrawItemStruct->itemID == LB_ERR)
		return;

	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect=lpDrawItemStruct->rcItem;

	if(lpDrawItemStruct->CtlID == IDC_SPHERECOLOR1)
	{
		rect.InflateRect(-1, -1);
		pDC->FillSolidRect(rect, m_pSphereColor1 != nullptr ? m_pSphereColor1->GetMonitorRGB() : RGB(0, 0, 0));
	}
	if(lpDrawItemStruct->CtlID == IDC_SPHERECOLOR2)
	{
		rect.InflateRect(-1, -1);
		pDC->FillSolidRect(rect, m_pSphereColor2 != nullptr ? m_pSphereColor2->GetMonitorRGB() : RGB(0, 0, 0));
	}
	if(lpDrawItemStruct->CtlID == IDC_GRATICULECOLOR)
	{
		rect.InflateRect(-1, -1);
		pDC->FillSolidRect(rect, m_pGraticuleColor != nullptr ? m_pGraticuleColor->GetMonitorRGB() : RGB(0, 0, 0));
	}

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}