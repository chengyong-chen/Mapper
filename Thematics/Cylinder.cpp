// Cylinder.cpp : implementation file
//

#include "stdafx.h"
#include "Cylinder.h"
#include "Chart.h"

#include "..//Geometry//Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCylinder::CCylinder()
{
	m_diameter = 15;
}

CCylinder::~CCylinder()
{
}

void CCylinder::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CGraduated::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_diameter;
	}
	else
	{
		ar>>m_diameter;
	}
}

BOOL CCylinder::operator==(const CTheme& theme) const
{
	if(CGraduated::operator==(theme)==FALSE)
		return FALSE;

	if(theme.Gettype()!=this->Gettype())
		return FALSE;
	else if(m_diameter!=((CCylinder&)theme).m_diameter)
		return FALSE;
	else
		return TRUE;
}

void CCylinder::CopyTo(CTheme* pTheme) const
{
	CGraduated::CopyTo(pTheme);

	if(pTheme->Gettype()==this->Gettype())
	{
		((CCylinder*)pTheme)->m_diameter = m_diameter;
	}
}

void CCylinder::Draw(Gdiplus::Graphics& g, const double fRatio, Gdiplus::Point point, double& value)
{
	if(value<=0)
		return;

	const int size = GetSize(value);
	if(size==0)
		return;

	const int cx = m_diameter*fRatio;
	const int cy = CChart::Incline(m_diameter*fRatio, 70);
	const Gdiplus::Rect rect1(point.X-cx/2.f, point.Y-cy/2.f, cx, cy);
	Gdiplus::Rect rect2 = rect1;
	Gdiplus::Rect rect;
	rect2.Offset(0, -size*fRatio);
	rect = UnionRect(rect1, rect2);

	const Gdiplus::ARGB argbFill = Gdiplus::Color::MakeARGB(255, GetRValue(m_colorFill), GetGValue(m_colorFill), GetBValue(m_colorFill));
	const Gdiplus::SolidBrush brush1(argbFill);
	const Gdiplus::ARGB argbDark = CTheme::LightColor(argbFill);
	const Gdiplus::SolidBrush brush2(argbDark);

	Gdiplus::GraphicsPath path;
	path.AddArc(rect2, 0, 180);
	const Gdiplus::Point point11(rect2.X, rect2.Y+rect2.Height/2);
	const Gdiplus::Point point12(rect1.X, rect1.Y+rect1.Height/2);
	path.AddLine(point11, point12);
	path.AddArc(rect1, 180, -180);
	const Gdiplus::Point point21(rect1.GetRight(), rect1.Y+rect1.Height/2);
	const Gdiplus::Point point22(rect2.GetRight(), rect2.Y+rect2.Height/2);
	path.AddLine(point21, point22);
	path.CloseFigure();
	g.FillPath(&brush1, &path);
	g.FillEllipse(&brush2, rect2);

	if(m_widthBorder>0.f)
	{
		const Gdiplus::ARGB argbBorder = Gdiplus::Color::MakeARGB(255, GetRValue(m_colorBorder), GetGValue(m_colorBorder), GetBValue(m_colorBorder));
		const Gdiplus::Pen pen(argbBorder, m_widthBorder);
		g.DrawPath(&pen, &path);
		g.DrawEllipse(&pen, rect2);
	}
}
