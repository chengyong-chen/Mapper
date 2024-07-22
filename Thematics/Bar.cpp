#include "StdAfx.h"
#include "Bar.h"
#include <math.h>

#undef max
#undef min

#include "../Database/Datasource.h"
#include "../Public/ODBCDatabase.h"
#include "../Public/Databaseconnection.h"

using namespace std;
#include <limits>
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBar::CBar()
{
	m_style = Style::TwoD;

	m_continuous.m_maxT = 30;
	m_angleSkew = 30;

	m_widthBar = 5;
	m_widthGap = 0;
}

CBar::~CBar()
{
}

void CBar::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CChart::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar << (BYTE)m_style;
		ar << m_widthBar;
		ar << m_widthGap;
		ar << m_angleSkew;
	}
	else
	{
		BYTE style;
		ar >> style;
		m_style = (Style)style;
		ar >> m_widthBar;
		ar >> m_widthGap;
		ar >> m_angleSkew;
	}
}

BOOL CBar::operator==(const CTheme& theme) const
{
	if(CChart::operator==(theme) == FALSE)
		return FALSE;

	if(theme.Gettype() != this->Gettype())
		return FALSE;
	else if(m_style != ((CBar&)theme).m_style)
		return FALSE;
	else if(m_widthBar != ((CBar&)theme).m_widthBar)
		return FALSE;
	else if(m_widthGap != ((CBar&)theme).m_widthGap)
		return FALSE;
	else if(m_angleSkew != ((CBar&)theme).m_angleSkew)
		return FALSE;
	else
		return TRUE;
}

void CBar::CopyTo(CTheme* pTheme) const
{
	CChart::CopyTo(pTheme);

	if(pTheme->Gettype() == this->Gettype())
	{
		((CBar*)pTheme)->m_style = m_style;
		((CBar*)pTheme)->m_widthBar = m_widthBar;
		((CBar*)pTheme)->m_widthGap = m_widthGap;
		((CBar*)pTheme)->m_angleSkew = m_angleSkew;
	}
}

void CBar::Draw(Gdiplus::Graphics& g, const double fRatio, Gdiplus::Point point, std::map<CString, double>& values)
{
	double m_maxValue = 0;
	for(std::map<CString, double>::iterator it = values.begin(); it != values.end(); it++)
	{
		m_maxValue = max(m_maxValue, it->second);
	}

	float maxHeight;
	if(m_maxValue <= 0)
		maxHeight = 0;
	else if(m_maxValue < m_minValue)
		maxHeight = 0;
	else
		maxHeight = m_continuous.m_maxT*m_maxValue/m_maxValue;

	maxHeight *= fRatio;
	if(maxHeight <= 0.f)
		return;

	Style style = m_style;
	if(m_deepth == 0.f)
		style = Style::TwoD;
	if(m_angleSkew == 0)
		style = Style::TwoD;

	switch(style)
	{
		case Style::TwoD:
			{
				const float cx = fRatio*(m_fieldcolors.size()*m_widthBar + (m_fieldcolors.size() - 1)*m_widthGap);
				const float cy = fRatio*(maxHeight);
				const Gdiplus::Rect rect(point.X - cx/2.f, point.Y - cy, cx, cy);
				const Gdiplus::Color color(255, GetRValue(m_colorBorder), GetGValue(m_colorBorder), GetBValue(m_colorBorder));
				const Gdiplus::Pen pen(color, m_widthBorder);

				point.X = rect.GetLeft();
				point.Y = rect.GetBottom();
				for(std::map<CString, double>::iterator it = values.begin(); it != values.end(); it++)
				{
					const float height = rect.Height*it->second/m_maxValue;
					Gdiplus::SolidBrush brush(m_fieldcolors[it->first]);
					Gdiplus::RectF barRect(point.X, point.Y - height, m_widthBar*fRatio, height);
					g.FillRectangle(&brush, barRect);
					if(m_widthBorder > 0.0)
						g.DrawRectangle(&pen, barRect);

					point.X += (m_widthBar + m_widthGap)*fRatio;
				}
			}
			break;
		case Style::ThreeD:
			{
				const double cx = fRatio*(m_fieldcolors.size()*m_widthBar + (m_fieldcolors.size() - 1)*m_widthBar + m_deepth*sin(M_PI*m_angleSkew/18.f));
				const double cy = fRatio*(maxHeight + m_deepth*sin(M_PI*m_angleIncline/180.f));
				/*Rect rect(point.X - cx/2.f, point.Y - cy, cx, -cy);

						Rect rect1(rect);
						Rect rect2(rect);
						rect1.Y -= m_deepth*fRatio;
						Rect::Union(rect,rect1,rect2);
						if(rect.IntersectsWith(invalidRect) == FALSE)
							break;

						DrawTop(g,rect1,m_maxValue,values);
						REAL start = m_angleStarting;
						for(std::map<CString,double>::iterator it=values.begin(); it != values.end(); it++)
						{
							if (start >= 360)
								start -= 360;
							ARGB argb = CTheme::DarkColor(m_fieldcolors[it->first]);
							SolidBrush brush(argb);
							double angle = total == 0 ? 360 : 360*it->second/total;
							if (start < 180 && start + angle <= 180)
							   Draw3DArc(g,brush,rect1,rect2, start, angle);
							else if(start < 180 && start + angle <= 360)
							{
								Draw3DArc(g,brush,rect1,rect2,start,180 - start);
							}
							else if (start < 180 && start + angle > 360)
							{
								Draw3DArc(g,brush,rect1,rect2, start, 180 - start);
								Draw3DArc(g,brush,rect1,rect2,0,start + angle - 360);
							}
							else if (start >= 180 && start + angle > 360)
							{
								double angle1 =  start + angle - 360;
								if (angle1 > 180)
									angle1 = 180;
								Draw3DArc(g,brush,rect1,rect2,0,angle1);
							}

							start += angle;
						}*/
			}
			break;
	}
}

void CBar::RecalculateLimit(CDatabase& database, CATTDatasource& datasource)
{
	assert(database.IsOpen());

	m_minValue = 0;
	m_maxValue = std::numeric_limits<double>::max();
	if(m_fields.size() == 0)
		return;

	const CHeaderProfile& headerprofile = datasource.GetHeaderProfile();
	CString strSQL;
	DBMSTYPE type = CODBCDatabase::GetDBMSType(&database);
	switch(type)
	{
		case DBMSTYPE::Oracle:
			{
				CString strSummation;
				for(std::list<CString>::iterator it = m_fields.begin(); it != m_fields.end(); ++it)
				{
					strSummation += *it + _T(",");
				}
				strSummation = strSummation.Trim(_T(","));
				strSQL.Format(_T("Select MAX(greatest(%s)) From %s Where ID Is Not nullptr"), strSummation, headerprofile.m_strKeyTable);
			}
			break;
		default:
			{
				CString strSummation;
				for(std::list<CString>::iterator it = m_fields.begin(); it != m_fields.end(); ++it)
				{
					CString str;
					if(headerprofile.m_strWhere != _T(""))
						str.Format(_T("Select min([%s]) as val1,max([%s]) as val2 FROM %s Where ID Is Not nullptr And %s Union "), *it, *it, headerprofile.m_strKeyTable);
					else
						str.Format(_T("Select min([%s]) as val1,max([%s]) as val2 FROM %s  Where ID Is Not nullptr Union "), *it, *it, headerprofile.m_strKeyTable);

					strSummation += str;
				}
				strSummation = strSummation.Trim(_T(" Union "));
				strSQL.Format(_T("Select MIN(val1),MAX(val2) From (%s) x"), strSummation);
			}
			break;
	}

	CODBCDatabase::ParseSQL(&database, strSQL);
	CRecordset rs(&database);
	try
	{
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		if(rs.IsEOF() == FALSE)
		{
			CString strValue;
			rs.GetFieldValue((short)0, strValue);
			m_minValue = _ttof(strValue);
			rs.GetFieldValue((short)1, strValue);
			m_maxValue = _ttof(strValue);
		}
		rs.Close();
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
}

Gdiplus::ARGB CBar::DefaultColor(std::map<CString, Gdiplus::ARGB>& exsiting)
{
	static Gdiplus::ARGB colors[] =
	{
		0xFF2a47b5,
		0xFFf3c676,
		0xFF803f23,
		0xFFc32d1c,
		0xFFe0c6a5,
		0xFFefeeda,
		0xFF28483b,
		0xFF477084,
		0xFFa7c0c7,
		0xFFdae9ca
	};

	for(int index = 0; index < 10; index++)
	{
		Gdiplus::ARGB color = colors[index];
		std::map<CString, Gdiplus::ARGB>::iterator it = std::find_if(exsiting.begin(), exsiting.end(), [&color](const std::pair<CString, Gdiplus::ARGB>& item)
			{
				return item.second == color;
			});

		if(it == exsiting.end())
		{
			return colors[index];
		}
	}

	return rand() | 0XFF000000;
}
