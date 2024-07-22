// DivisionalChart.cpp : implementation file
//

#include "stdafx.h"
#include "BarDlg.h"
#include "Bar.h"
#include "Resource.h"

using namespace std;
#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CBarDlg dialog

IMPLEMENT_DYNAMIC(CBarDlg, CChartDlg)

CBarDlg::CBarDlg(CWnd* pParent, CBar& bar, CDatabase& database, CATTDatasource& datasource)
	: CChartDlg(CBarDlg::IDD, pParent, bar, database, datasource), m_bar(bar)
{
}

CBarDlg::~CBarDlg()
{
}

void CBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CChartDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBarDlg, CChartDlg)
	ON_WM_PAINT()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

BOOL CBarDlg::OnInitDialog()
{
	CChartDlg::OnInitDialog();

	CMFCPropertyGridProperty* pGroupStyle = CChartDlg::GetPropertyByName(&m_PropertyGrid, _T("Style"));
	if(pGroupStyle!=nullptr)
	{
		pGroupStyle->SetValue(m_bar.m_style==CBar::Style::TwoD ? _T("2D") : _T("3D"));
	}

	CMFCPropertyGridProperty* pProp2 = new CMFCPropertyGridProperty(_T("Maximal Height"), (_variant_t)m_bar.m_continuous.m_maxT, _T(""));
	pProp2->EnableSpinControl(TRUE, 1, 1000);
	m_PropertyGrid.AddProperty(pProp2);

	CMFCPropertyGridProperty* pGroupBar = new CMFCPropertyGridProperty(_T("Bar"));
	{
		CMFCPropertyGridProperty* pProp3 = new CMFCPropertyGridProperty(_T("Single Width"), (_variant_t)m_bar.m_widthBar, _T(""));
		//	pProp3->EnableSpinControl(TRUE, 0, 90);
		pGroupBar->AddSubItem(pProp3);

		CMFCPropertyGridProperty* pProp5 = new CMFCPropertyGridProperty(_T("Gap"), (_variant_t)m_bar.m_widthGap, _T(""));
		//		pProp5->EnableSpinControl(TRUE, 0, 90);
		pGroupBar->AddSubItem(pProp5);
	}
	m_PropertyGrid.AddProperty(pGroupBar);

	CMFCPropertyGridProperty* pGroupAngle = CChartDlg::GetPropertyByName(&m_PropertyGrid, _T("Angle"));
	if(pGroupAngle!=nullptr)
	{
		CMFCPropertyGridProperty* pProp1 = new CMFCPropertyGridProperty(_T("Skew"), (_variant_t)m_bar.m_angleSkew, _T(""));
		pProp1->EnableSpinControl(TRUE, 0, 90);
		pGroupAngle->AddSubItem(pProp1);
	}
	m_PropertyGrid.ExpandAll();
	m_PropertyGrid.SetFocus();

	OnPropertyChanged(0, (DWORD)pGroupStyle);
	return TRUE;
}

void CBarDlg::OnPaint()
{
	CPaintDC dc(this);
	const int size = m_bar.m_continuous.m_maxT;
	CRect crect;
	GetDlgItem(IDC_STATIC2)->GetClientRect(crect);
	Gdiplus::Rect rect(crect.CenterPoint().x, crect.CenterPoint().y, 0, 0);
	rect.Inflate(size/2, size/2);
	Gdiplus::Graphics g(GetDlgItem(IDC_STATIC2)->m_hWnd);
	g.SetPageUnit(Gdiplus::UnitPixel);
	g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	std::map<CString, double> elements;
	for(std::map<CString, Gdiplus::ARGB>::iterator it = m_bar.m_fieldcolors.begin(); it!=m_bar.m_fieldcolors.end(); ++it)
	{
		elements[it->first] = m_bar.m_maxValue;
	}
	m_bar.Draw(g, 1, Gdiplus::Point(rect.X+rect.Width/2, rect.Y+rect.Height), elements);
}

LRESULT CBarDlg::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty* pProperty = (CMFCPropertyGridProperty*)lParam;
	if(pProperty==nullptr)
		return 0;
	const CString strName = pProperty->GetName();
	const COleVariant varValue = pProperty->GetValue();
	if(strName==_T("Style"))
	{
		CMFCPropertyGridProperty* pDeepthProp = CChartDlg::GetPropertyByName(&m_PropertyGrid, _T("Deepth"));
		CMFCPropertyGridProperty* pAngleGroup = CChartDlg::GetPropertyByName(&m_PropertyGrid, _T("Angle"));
		CMFCPropertyGridProperty* pInclineProp = CChartDlg::GetPropertyByName(&m_PropertyGrid, _T("Incline"));

		if(varValue.bstrVal==_T("3D"))
		{
			m_bar.m_style = CBar::Style::ThreeD;

			if(pDeepthProp!=nullptr)
			{
			}
			if(pInclineProp==nullptr)
			{
			}
		}
		else if(varValue.bstrVal==_T("2D"))
		{
			m_bar.m_style = CBar::Style::TwoD;

			if(pDeepthProp!=nullptr)
			{
			}
			if(pInclineProp!=nullptr)
			{
			}
		}
	}
	else if(strName==_T("Maximal Height"))
	{
		m_bar.m_continuous.m_maxT = varValue.intVal;
	}
	else if(strName==_T("Skew"))
	{
		m_bar.m_angleSkew = varValue.intVal;
	}
	else if(strName==_T("Gap"))
	{
		m_bar.m_widthGap = varValue.fltVal;
	}
	else if(strName==_T("Single Width"))
	{
		m_bar.m_widthBar = varValue.fltVal;
	}

	return CChartDlg::OnPropertyChanged(wParam, lParam);
}
