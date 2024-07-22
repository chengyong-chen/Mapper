#include "stdafx.h"
#include "Graduated.h"

#include "../Geometry/Geom.h"
#include "../Dataview/viewinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGraduated::CGraduated()
{
	m_widthBorder = 0;
	m_continuous.m_minT = 10;
	m_continuous.m_maxT = 30;
	m_symbol = 1;
}

CGraduated::~CGraduated()
{
}

void CGraduated::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CTheme::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_symbol;
		ar<<m_colorBorder;
		ar<<m_widthBorder;
		ar<<m_colorFill;
	}
	else
	{
		ar>>m_symbol;
		ar>>m_colorBorder;
		ar>>m_widthBorder;
		ar>>m_colorFill;
	}
}

BOOL CGraduated::operator==(const CTheme& theme) const
{
	if(CTheme::operator==(theme)==FALSE)
		return FALSE;

	if(theme.Gettype()!=this->Gettype())
		return FALSE;
	else if(m_symbol!=((CGraduated&)theme).m_symbol)
		return FALSE;
	else if(m_colorBorder!=((CGraduated&)theme).m_colorBorder)
		return FALSE;
	else if(m_widthBorder!=((CGraduated&)theme).m_widthBorder)
		return FALSE;
	else if(m_colorFill!=((CGraduated&)theme).m_colorFill)
		return FALSE;
	else if(m_divisions!=((CGraduated&)theme).m_divisions)
		return FALSE;
	else
		return TRUE;
}

void CGraduated::CopyTo(CTheme* pTheme) const
{
	CTheme::CopyTo(pTheme);

	if(pTheme->Gettype()==this->Gettype())
	{
		((CGraduated*)pTheme)->m_symbol = m_symbol;
		((CGraduated*)pTheme)->m_colorBorder = m_colorBorder;
		((CGraduated*)pTheme)->m_widthBorder = m_widthBorder;
		((CGraduated*)pTheme)->m_colorFill = m_colorFill;
	}
}

void CGraduated::RecalculateLimit(CDatabase& database, CATTDatasource& datasource)
{
	CTheme::RecalculateLimit(database, datasource);

	const int steps = m_divisions.size();
	if(steps>1)
	{
		const double segmentValue = (m_maxValue-m_minValue)/steps;
		const int index = 0;
		for(std::list<CDivision<DWORD>>::iterator it = m_divisions.begin(); it!=m_divisions.end(); ++it)
		{
			it->m_minValue = m_minValue+index*segmentValue;
			it->m_maxValue = m_minValue+index*segmentValue+segmentValue;
		}
	}
}

// CGraduated member functions
void CGraduated::Draw(Gdiplus::Graphics& g, CDatabase& database, CATTDatasource& datasource, const CViewinfo& viewinfo, std::map<DWORD, CGeom*>& list)
{
	if(list.size()==0)
		return;
	if(m_fields.size()==0)
		return;

	std::list<std::pair<Gdiplus::Point, double>> objects;
	CString strSQL = MakrScript(database, datasource);
	try
	{
		CRecordset rs(&database);
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		while(rs.IsEOF()==FALSE)
		{
			CString strValue;
			rs.GetFieldValue(_T("ID"), strValue);
			DWORD dwATT = _tcstoul(strValue, nullptr, 10);

			CGeom* pGeom = list[dwATT];
			if(pGeom!=nullptr)
			{
				double value = 0;
				for(std::list<CString>::iterator it = m_fields.begin(); it!=m_fields.end(); ++it)
				{
					rs.GetFieldValue(*it, strValue);
					value += _tcstod(strValue, nullptr);
				}
				CPoint docPoint = pGeom->GetGeogroid();
				Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
				objects.push_back(std::pair<Gdiplus::Point, double>(cliPoint, value));
			}

			rs.MoveNext();
		}
		rs.Close();
	}
	catch(CDBException*ex)
	{
		OutputDebugString(ex->m_strError);
		//	ex->ReportError();
		ex->Delete();
	}

	objects.sort(&CGraduated::ComparePair);
	for(std::list<std::pair<Gdiplus::Point, double>>::iterator it = objects.begin(); it!=objects.end(); ++it)
	{
		Draw(g, viewinfo.CurrentRatio(), it->first, it->second);
	}
}

void CGraduated::Draw(Gdiplus::Graphics& g, const double fRatio, Gdiplus::Point point, double& value)
{
	if(value<=0)
		return;

	int size = GetSize(value);
	if(size==0)
		return;

	size *= fRatio;
	Gdiplus::Rect rect(point.X, point.Y, 0, 0);
	rect.Inflate(size/2, size/2);
	const _bstr_t btrFont(_T("Map Symbols"));
	const Gdiplus::FontFamily* fontFamily = ::new Gdiplus::FontFamily(btrFont);
	if(fontFamily->IsAvailable()==true)
	{
		const Gdiplus::Font font(fontFamily, size, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

		CString strChar;
		strChar.Format(_T("%c"), 0X20+m_symbol);
		const _bstr_t bstr(strChar);
		const Gdiplus::ARGB argb = Gdiplus::Color::MakeARGB(255, GetRValue(m_colorFill), GetGValue(m_colorFill), GetBValue(m_colorFill));
		const Gdiplus::SolidBrush brush(argb);
		Gdiplus::StringFormat stringFormat;
		stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		g.DrawString(bstr, -1, &font, Gdiplus::PointF(point.X, point.Y), &stringFormat, &brush);
	}
	::SysFreeString(btrFont);
	::delete fontFamily;
}

bool CGraduated::ComparePair(std::pair<Gdiplus::Point, double>& pair1, std::pair<Gdiplus::Point, double>& pair2)
{
	return pair1.first.Y<pair2.first.Y;
}
