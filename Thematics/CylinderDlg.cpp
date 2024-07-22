#include "stdafx.h"
#include "Cylinder.h"
#include "CylinderDlg.h"
#include <stdlib.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CCylinderDlg, CGraduatedDlg)

CCylinderDlg::CCylinderDlg(CWnd* pParent, CCylinder& graduated, CDatabase& database, CATTDatasource& datasource)
	: CGraduatedDlg(pParent, graduated, database, datasource)
{
}

CCylinderDlg::~CCylinderDlg()
{
}

BEGIN_MESSAGE_MAP(CCylinderDlg, CGraduatedDlg)
	ON_WM_PAINT()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

BOOL CCylinderDlg::OnInitDialog()
{
	CGraduatedDlg::OnInitDialog();

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Diameter"), (_variant_t)((CCylinder&)m_graduated).m_diameter, _T(""));
	m_PropertyGrid.AddProperty(pProp);
	return TRUE;
}

void CCylinderDlg::OnPaint()
{
	CPaintDC dc(this);

	CRect crect1;
	CRect crect2;
	GetDlgItem(IDC_STATIC1)->GetClientRect(crect1);
	GetDlgItem(IDC_STATIC2)->GetClientRect(crect2);
	const Gdiplus::Rect rect1(crect1.left, crect1.top, crect1.Width(), crect1.Height());
	const Gdiplus::Rect rect2(crect2.left, crect2.top, crect2.Width(), crect2.Height());
	Gdiplus::Graphics g1(GetDlgItem(IDC_STATIC1)->m_hWnd);
	Gdiplus::Graphics g2(GetDlgItem(IDC_STATIC2)->m_hWnd);
	g1.SetPageUnit(Gdiplus::UnitPixel);
	g1.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g1.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	g1.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	g2.SetPageUnit(Gdiplus::UnitPixel);
	g2.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g2.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	g2.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

	((CCylinder&)m_graduated).Draw(g1, 1, Gdiplus::Point(rect1.X+rect1.Width/2, rect1.Y+rect1.Height-((CCylinder&)m_graduated).m_diameter/2), m_graduated.m_minValue);
	((CCylinder&)m_graduated).Draw(g2, 1, Gdiplus::Point(rect2.X+rect2.Width/2, rect2.Y+rect2.Height-((CCylinder&)m_graduated).m_diameter/2), m_graduated.m_maxValue);
}

LRESULT CCylinderDlg::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty* pProperty = (CMFCPropertyGridProperty*)lParam;
	if(pProperty==nullptr)
		return 0;
	const CString strName = pProperty->GetName();
	const COleVariant varValue = pProperty->GetValue();
	if(strName==_T("Diameter"))
	{
		CString str = varValue.bstrVal;
		((CCylinder&)m_graduated).m_diameter = varValue.intVal;
	}

	return CGraduatedDlg::OnPropertyChanged(wParam, lParam);
}
