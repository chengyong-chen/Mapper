#include "StdAfx.h"
#include "Pie.h"

#include "..//Geometry//Global.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPie::CPie()
{
	m_style = Style::TwoD;

	m_continuous.m_minT = 10;
	m_angleStarting = 0;
}

CPie::~CPie()
{
}

void CPie::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CChart::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<(BYTE)m_style;
		ar<<m_angleStarting;
	}
	else
	{
		BYTE byte;
		ar>>byte;
		m_style = (Style)byte;
		ar>>m_angleStarting;
	}
}

BOOL CPie::operator==(const CTheme& theme) const
{
	if(CChart::operator==(theme)==FALSE)
		return FALSE;

	if(theme.Gettype()!=this->Gettype())
		return FALSE;
	else if(m_style!=((CPie&)theme).m_style)
		return FALSE;
	else if(m_angleStarting!=((CPie&)theme).m_angleStarting)
		return FALSE;
	else if(m_divisions!=((CPie&)theme).m_divisions)
		return FALSE;
	else
		return TRUE;
}

void CPie::CopyTo(CTheme* pTheme) const
{
	CChart::CopyTo(pTheme);

	if(pTheme->Gettype()==this->Gettype())
	{
		((CPie*)pTheme)->m_style = m_style;
		((CPie*)pTheme)->m_angleStarting = m_angleStarting;
	}
}

void CPie::RecalculateLimit(CDatabase& database, CATTDatasource& datasource)
{
	CChart::RecalculateLimit(database, datasource);

	const int steps = m_divisions.size();
	if(steps>0)
	{
		RedivideSize(steps);
	}
}

void CPie::Draw(Gdiplus::Graphics& g, const double fRatio, Gdiplus::Point point, std::map<CString, double>& values)
{
	double total = 0;
	for(std::map<CString, double>::iterator it = values.begin(); it!=values.end(); it++)
	{
		total += it->second;
	}

	const int size = GetSize(total);
	if(size==0)
		return;

	Style style = m_style;
	if(m_deepth==0)
		style = Style::TwoD;
	if(m_angleIncline==0)
		style = Style::TwoD;

	const int cx = size*fRatio;
	const int cy = CChart::Incline(size*fRatio, m_angleIncline);
	Gdiplus::Rect rect(point.X-cx/2.f, point.Y-cy/2.f, cx, cy);

	switch(style)
	{
	case Style::TwoD:
	{
		DrawTop(g, rect, total, values);
		if(m_widthAxis>0)
		{
			const Gdiplus::Color color(255, GetRValue(m_colorAxis), GetGValue(m_colorAxis), GetBValue(m_colorAxis));
			const Gdiplus::Pen pen(color, m_widthAxis);
			g.DrawEllipse(&pen, rect);
		}
	}
	break;
	case Style::ThreeD:
	{
		Gdiplus::Rect rect1(rect);
		Gdiplus::Rect rect2(rect);
		rect1.Y -= m_deepth*fRatio;
		rect = UnionRect(rect1, rect2);

		DrawTop(g, rect1, total, values);
		Gdiplus::REAL start = m_angleStarting;
		for(std::map<CString, double>::iterator it = values.begin(); it!=values.end(); it++)
		{
			if(start>=360)
				start -= 360;
			const Gdiplus::ARGB argb = CTheme::DarkColor(m_fieldcolors[it->first]);
			Gdiplus::SolidBrush brush(argb);
			const double angle = total==0 ? 360 : 360*it->second/total;
			if(start<180&&start+angle<=180)
				Draw3DArc(g, brush, rect1, rect2, start, angle);
			else if(start<180&&start+angle<=360)
			{
				Draw3DArc(g, brush, rect1, rect2, start, 180-start);
			}
			else if(start<180&&start+angle > 360)
			{
				Draw3DArc(g, brush, rect1, rect2, start, 180-start);
				Draw3DArc(g, brush, rect1, rect2, 0, start+angle-360);
			}
			else if(start>=180&&start+angle>360)
			{
				double angle1 = start+angle-360;
				if(angle1>180)
					angle1 = 180;
				Draw3DArc(g, brush, rect1, rect2, 0, angle1);
			}

			start += angle;
		}
	}
	break;
	}
}

void CPie::DrawTop(Gdiplus::Graphics& g, Gdiplus::Rect& rect, double& total, std::map<CString, double>& values)
{
	Gdiplus::REAL start = m_angleStarting;
	for(std::map<CString, double>::iterator it = values.begin(); it!=values.end(); it++)
	{
		const Gdiplus::REAL angle = total==0 ? 360 : 360*it->second/total;
		Gdiplus::SolidBrush brush(m_fieldcolors[it->first]);
		g.FillPie(&brush, rect, start, angle);
		start += angle;
	}
	if(values.size()>1&&m_widthBorder>0)
	{
		const Gdiplus::ARGB argb = Gdiplus::Color::MakeARGB(255, GetRValue(m_colorBorder), GetGValue(m_colorBorder), GetBValue(m_colorBorder));
		Gdiplus::Pen pen(argb, m_widthBorder);
		start = m_angleStarting;
		for(std::map<CString, double>::iterator it = values.begin(); it!=values.end(); ++it)
		{
			const Gdiplus::REAL angle = total==0 ? 360 : 360*it->second/total;
			//  INT x = point.X + (rect.Width/2.f)*cos(M_PI*(start + angle)/ 180);
			//  INT y = point.Y + (rect.Height/2.f)*sin(M_PI*(start + angle)/ 180);
			//	g.DrawLine(&pen,point.X,point.Y,x,y);if it is not a regular circle, this is not correct
			start += angle;
		}
	}
}

void CPie::Draw3DArc(Gdiplus::Graphics& g, Gdiplus::Brush& brush, Gdiplus::Rect& rect1, Gdiplus::Rect& rect2, double start, double angle)
{
	Gdiplus::GraphicsPath path;
	path.AddArc(rect2, start, angle);

	if(start+angle==180)
	{
		const Gdiplus::Point point1(rect2.X, rect2.Y+rect2.Height/2);
		const Gdiplus::Point point2(rect1.X, rect1.Y+rect1.Height/2);
		path.AddLine(point1, point2);
	}

	path.AddArc(rect1, start+angle, -angle);

	if(start==0)
	{
		const Gdiplus::Point point1(rect1.GetRight(), rect1.Y+rect1.Height/2);
		const Gdiplus::Point point2(rect2.GetRight(), rect2.Y+rect2.Height/2);
		path.AddLine(point1, point2);
	}
	g.FillPath(&brush, &path);
	//	g.DrawPath(&penOutLine, &path);
}

Gdiplus::ARGB CPie::DefaultColor(std::map<CString, Gdiplus::ARGB>& exsiting)
{
	static Gdiplus::ARGB colors[] =
	{
		0XFF024e00,
		0XFF94aa24,
		0XFFe9bf31,
		0XFFf07f29,
		0XFFf33f22,
		0XFFbe472f,
		0XFF87513c,
		0XFF804ea2,
		0XFF794bff,
		0XFF8ea5fa,
		0XFFa2feef,
		0XFF89f0a6,
		0XFF68dd47,
		0XFF62ae23,
		0XFF5d8101,
	};

	for(int index = 0; index<15; index++)
	{
		Gdiplus::ARGB color = colors[index];
		std::map<CString, Gdiplus::ARGB>::iterator it = std::find_if(exsiting.begin(), exsiting.end(), [&color](const std::pair<CString, Gdiplus::ARGB>& item)
			{
				return item.second==color;
			});

		if(it==exsiting.end())
		{
			return colors[index];
		}
	}

	return rand()|0XFF000000;
}
