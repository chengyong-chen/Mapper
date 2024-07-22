// Choropleth.cpp : implementation file
//

#include "stdafx.h"
#include "Choropleth.h"

#include "../Geometry/Geom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CChoropleth::CChoropleth()
{
	m_continuous.m_minT = 0XFFFFFFFF;
	m_continuous.m_maxT = 0XFF000000;

	Category = CTheme::CATEGORY::Area;
}

CChoropleth::~CChoropleth()
{
}

void CChoropleth::Draw(Gdiplus::Graphics& g, CDatabase& database, CATTDatasource& datasource, const CViewinfo& viewinfo, Gdiplus::Pen* pen, std::map<DWORD, CGeom*>& list)
{
	//	assert(database.IsOpen());
	if(list.size()==0)
		return;
	if(m_fields.size()==0)
		return;

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
			if(pGeom!=nullptr&&pGeom->m_bClosed==true)
			{
				double value = 0;
				for(std::list<CString>::iterator it = m_fields.begin(); it!=m_fields.end(); ++it)
				{
					rs.GetFieldValue(*it, strValue);
					value += _tcstod(strValue, nullptr);
				}

				Gdiplus::ARGB argb = GetARGB(value);
				Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
			//	pGeom->AddPath(path, viewinfo, Interpolation::NoInter);
				Gdiplus::SolidBrush brush(argb);
				g.FillPath(&brush, &path);
				if(pen!=nullptr)
				{
					g.DrawPath(pen, &path);
				}
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
}

void CChoropleth::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CTheme::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

BOOL CChoropleth::operator==(const CTheme& theme) const
{
	if(CTheme::operator==(theme)==FALSE)
		return FALSE;

	if(theme.Gettype()!=this->Gettype())
		return FALSE;
	else
		return TRUE;
}

void CChoropleth::CopyTo(CTheme* pTheme) const
{
	CTheme::CopyTo(pTheme);

	if(pTheme->Gettype()==this->Gettype())
	{
	}
}

void CChoropleth::RecalculateLimit(CDatabase& database, CATTDatasource& datasource)
{
	CTheme::RecalculateLimit(database, datasource);
	const int steps = m_divisions.size();
	RedivideARGB(steps);
}
