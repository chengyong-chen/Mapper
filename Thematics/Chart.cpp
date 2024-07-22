// Chart.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "Chart.h"

#include "../DataView/Viewinfo.h"

#include "../Geometry/Geom.h"
using namespace std;
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CChart::CChart()
{
	m_minValue = 0;

	m_continuous.m_maxT = 30;
	m_colorBorder = 0XFFFFFF;
	m_colorAxis = 0X000000;
	m_widthBorder = 1;
	m_widthAxis = 1;

	m_angleIncline = 30;
	m_deepth = 5.f;

	Category = CTheme::CATEGORY::Spot;
}

CChart::~CChart()
{
}

void CChart::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CTheme::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_colorBorder;
		ar<<m_widthBorder;
		ar<<m_colorAxis;
		ar<<m_widthAxis;

		ar<<m_deepth;
		ar<<m_angleIncline;

		ar<<(int)m_fieldcolors.size();
		for(std::map<CString, Gdiplus::ARGB>::iterator it = m_fieldcolors.begin(); it!=m_fieldcolors.end(); ++it)
		{
			ar<<it->first;
			ar<<it->second;
		}
	}
	else
	{
		ar>>m_colorBorder;
		ar>>m_widthBorder;
		ar>>m_colorAxis;
		ar>>m_widthAxis;

		ar>>m_deepth;
		ar>>m_angleIncline;

		int nCount;
		ar>>nCount;
		for(int index = 0; index<nCount; index++)
		{
			CString strField;
			Gdiplus::ARGB argb;
			ar>>strField;
			ar>>argb;
			m_fieldcolors[strField] = argb;
		}
	}
}

BOOL CChart::operator==(const CTheme& theme) const
{
	if(CTheme::operator==(theme)==FALSE)
		return FALSE;

	if(theme.Gettype()!=this->Gettype())
		return FALSE;
	else if(m_colorBorder!=((CChart&)theme).m_colorBorder)
		return FALSE;
	else if(m_widthBorder!=((CChart&)theme).m_widthBorder)
		return FALSE;
	else if(m_colorAxis!=((CChart&)theme).m_colorAxis)
		return FALSE;
	else if(m_colorBorder!=((CChart&)theme).m_colorBorder)
		return FALSE;
	else if(m_widthAxis!=((CChart&)theme).m_widthAxis)
		return FALSE;
	else if(m_angleIncline!=((CChart&)theme).m_angleIncline)
		return FALSE;
	else if(m_deepth!=((CChart&)theme).m_deepth)
		return FALSE;
	else if(m_fieldcolors!=((CChart&)theme).m_fieldcolors)
		return FALSE;
	else
		return TRUE;
}

void CChart::CopyTo(CTheme* pTheme) const
{
	CTheme::CopyTo(pTheme);

	if(pTheme->Gettype()==this->Gettype())
	{
		((CChart*)pTheme)->m_colorBorder = m_colorBorder;
		((CChart*)pTheme)->m_widthBorder = m_widthBorder;
		((CChart*)pTheme)->m_colorAxis = m_colorAxis;
		((CChart*)pTheme)->m_colorBorder = m_colorBorder;
		((CChart*)pTheme)->m_widthAxis = m_widthAxis;
		((CChart*)pTheme)->m_angleIncline = m_angleIncline;

		((CChart*)pTheme)->m_deepth = m_deepth;

		((CChart*)pTheme)->m_fieldcolors = m_fieldcolors;
	}
}

bool CChart::AddField(CDatabase& database, CATTDatasource& datasource, const CString& strField)
{
	CTheme::AddField(database, datasource, strField);
	const std::map<CString, Gdiplus::ARGB>::iterator it = m_fieldcolors.find(strField);
	if(it==m_fieldcolors.end())
	{
		m_fieldcolors[strField] = DefaultColor(m_fieldcolors);
		return true;
	}
	else
		return false;
}

bool CChart::RemoveField(CDatabase& database, CATTDatasource& datasource, const CString& strField)
{
	CTheme::RemoveField(database, datasource, strField);
	const std::map<CString, Gdiplus::ARGB>::iterator it = m_fieldcolors.find(strField);
	if(it!=m_fieldcolors.end())
	{
		m_fieldcolors.erase(it);
		return true;
	}
	else
		return false;
}

void CChart::RemoveAllFields()
{
	CTheme::RemoveAllFields();

	m_fieldcolors.clear();
}

void CChart::AddFields(CDatabase& database, CATTDatasource& datasource, std::list<CString> fields)
{
	CTheme::AddFields(database, datasource, fields);

	for(std::list<CString>::iterator it = fields.begin(); it!=fields.end(); ++it)
	{
		AddField(database, datasource, *it);
	}
}

void CChart::RecalculateLimit(CDatabase& database, CATTDatasource& datasource)
{
	CTheme::RecalculateLimit(database, datasource);

	m_minValue = max(0, m_minValue);
}

int CChart::Incline(int size, int angle)
{
	angle = max(0, min(angle, 90));
	return size*cos(M_PI*angle/180);
}

void CChart::Draw(Gdiplus::Graphics& g, CDatabase& database, CATTDatasource& datasource, const CViewinfo& viewinfo, std::map<DWORD, CGeom*>& list)
{
	assert(database.IsOpen());

	if(list.size()==0)
		return;
	if(m_fields.size()==0)
		return;

	std::list<std::pair<Gdiplus::Point, std::map<CString, double>>> objects;
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
				std::map<CString, double> values;
				for(std::list<CString>::iterator it = m_fields.begin(); it!=m_fields.end(); ++it)
				{
					rs.GetFieldValue(*it, strValue);
					values[*it] = _tcstod(strValue, nullptr);
				}
				CPoint docPoint = pGeom->GetGeogroid();
				Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
				objects.push_back(std::pair<Gdiplus::Point, std::map<CString, double>>(cliPoint, values));
				//	Draw(g,viewinfo.CurrentRatio(),cliPoint,invalidRect,values);
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

	objects.sort(&CChart::ComparePair);
	for(std::list<std::pair<Gdiplus::Point, std::map<CString, double>>>::iterator it = objects.begin(); it!=objects.end(); ++it)
	{
		Draw(g, viewinfo.CurrentRatio(), it->first, it->second);
	}
}

bool CChart::ComparePair(std::pair<Gdiplus::Point, std::map<CString, double>>& pair1, std::pair<Gdiplus::Point, std::map<CString, double>>& pair2)
{
	return pair1.first.Y<pair2.first.Y;
}
