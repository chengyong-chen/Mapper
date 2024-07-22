#include "stdafx.h"
#include "Theme.h"

#include <cassert>

#include "Choropleth.h"
#include "Graduated.h"
#include "Pie.h"
#include "Bar.h"
#include "Cylinder.h"
#include "../Database/Datasource.h"
#include "../Public/Global.h"
#include "../Public/ODBCDatabase.h"
#include "../Public/Databaseconnection.h"

using namespace std;
#include <list>
#include <algorithm>

#undef max
#undef min

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTheme::CTheme()
	: m_continuous(m_minValue, m_maxValue)
{
	m_minValue = std::numeric_limits<double>::min();
	m_maxValue = std::numeric_limits<double>::max();

	m_Alpha = 0;
	m_bIgnoreZeroAndNull = true;
}

CTheme::~CTheme()
{
}

BOOL CTheme::operator==(const CTheme& theme) const
{
	if(this->Gettype()!=theme.Gettype())
		return FALSE;
	else if(m_continuous!=theme.m_continuous)
		return FALSE;
	else if(m_divisions!=theme.m_divisions)
		return FALSE;
	else if(m_fields!=theme.m_fields)
		return FALSE;
	else if(m_Alpha!=theme.m_Alpha)
		return FALSE;
	else if(m_minValue!=theme.m_minValue)
		return FALSE;
	else if(m_maxValue!=theme.m_maxValue)
		return FALSE;
	else
		return TRUE;
}

BOOL CTheme::operator !=(const CTheme& theme) const
{
	return !(*this==theme);
}

CTheme* CTheme::Clone()
{
	const BYTE type = this->Gettype();
	CTheme* pClone = CTheme::Apply(type);
	if(pClone!=nullptr)
	{
		this->CopyTo(pClone);
	}
	return pClone;
}

void CTheme::CopyTo(CTheme* pTheme) const
{
	pTheme->m_minValue = m_minValue;
	pTheme->m_maxValue = m_maxValue;
	//	pTheme->m_continuous= m_continuous;//ARGB and size can't copy to each other
	//	pTheme->m_divisions	= m_divisions;
	pTheme->m_Alpha = m_Alpha;
	pTheme->m_fields = m_fields;
}

int CTheme::GetSize(double value)
{
	if(m_divisions.size()==0)
		return m_continuous.GetSize(value);
	else if(value<=m_divisions.begin()->m_minValue)
		return m_divisions.begin()->m_Value;
	if(value>=m_divisions.rbegin()->m_minValue)
		return m_divisions.rbegin()->m_Value;
	else
	{
		for(std::list<CDivision<DWORD>>::iterator it = m_divisions.begin(); it!=m_divisions.end(); ++it)
		{
			if(value>=it->m_minValue&&value<it->m_maxValue)
				return it->m_Value;
		}
	}
	return 0;
}

Gdiplus::ARGB CTheme::GetARGB(double value)
{
	if(m_divisions.size()==0)
		return m_continuous.GetARGB(value);
	else if(value<=m_divisions.begin()->m_minValue)
		return m_divisions.begin()->m_Value;
	else if(value>=m_divisions.rbegin()->m_maxValue)
		return m_divisions.rbegin()->m_Value;
	else
	{
		for(std::list<CDivision<COLORREF>>::iterator it = m_divisions.begin(); it!=m_divisions.end(); ++it)
		{
			if(value>=it->m_minValue&&value<it->m_maxValue)
				return it->m_Value;
		}
	}
	return Gdiplus::Color(255, 255, 255, 255).GetValue();
}

void CTheme::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar<<m_minValue;
		ar<<m_maxValue;
		ar<<m_Alpha;

		ar<<(int)m_fields.size();
		for(std::list<CString>::iterator it = m_fields.begin(); it!=m_fields.end(); ++it)
		{
			ar<<*it;
		}
		ar<<m_continuous;
		CTheme::Serialize(ar, dwVersion, m_divisions);
	}
	else
	{
		ar>>m_minValue;
		ar>>m_maxValue;
		ar>>m_Alpha;

		int nCount;
		ar>>nCount;
		for(int index = 0; index<nCount; index++)
		{
			CString strField;
			ar>>strField;
			m_fields.push_back(strField);
		}
		ar>>m_continuous;
		CTheme::Serialize(ar, dwVersion, m_divisions);
	}
}

bool CTheme::AddField(CDatabase& database, CATTDatasource& datasource, const CString& strField)
{
	const std::list<CString>::iterator it = std::find(m_fields.begin(), m_fields.end(), strField);
	if(it==m_fields.end())
	{
		m_fields.push_back(strField);
		RecalculateLimit(database, datasource);
		return true;
	}
	else
		return false;
}

bool CTheme::RemoveField(CDatabase& database, CATTDatasource& datasource, const CString& strField)
{
	const std::list<CString>::iterator it = std::find(m_fields.begin(), m_fields.end(), strField);
	if(it!=m_fields.end())
	{
		m_fields.erase(it);
		RecalculateLimit(database, datasource);
		return true;
	}
	else
		return false;
}

void CTheme::RemoveAllFields()
{
	m_fields.clear();

	m_minValue = std::numeric_limits<double>::min();
	m_maxValue = std::numeric_limits<double>::max();
}

void CTheme::AddFields(CDatabase& database, CATTDatasource& datasource, std::list<CString> fields)
{
	for(std::list<CString>::iterator it = m_fields.begin(); it!=m_fields.end(); ++it)
	{
		AddField(database, datasource, *it);
	}
}

void CTheme::RecalculateLimit(CDatabase& database, CATTDatasource& datasource)
{
	assert(database.IsOpen());

	m_minValue = std::numeric_limits<double>::min();
	m_maxValue = std::numeric_limits<double>::max();
	if(m_fields.size()==0)
		return;
	
	const DBMSTYPE type = CODBCDatabase::GetDBMSType(&database);
	CString strSummation;
	for(std::list<CString>::iterator it = m_fields.begin(); it!=m_fields.end(); ++it)
	{
		CString str;
		switch(type)
		{
		case DBMSTYPE::MSSQL:
			str.Format(_T("IsNull([%s],0)+"), *it);
			break;
		case DBMSTYPE::Access:
			str.Format(_T("IIF(IsNull([%s]),0,[%s])+"), *it, *it);
			break;
		case DBMSTYPE::Oracle:
			str.Format(_T("NVL(%s,0)+"), *it);
			break;
		case DBMSTYPE::MySQL:
			str.Format(_T("IFNULL(%s,0)+"), *it);
			break;
		default:
			str.Format(_T("IsNull([%s],0)+"), *it);
			break;
		}

		strSummation += str;
	}
	strSummation = strSummation.Trim(_T("+"));
	CString strSQL;
	const CHeaderProfile& headerprofile = datasource.GetHeaderProfile();
	if(headerprofile.m_strWhere!=_T(""))
		strSQL.Format(_T("Select MIN(%s),MAX(%s) From %s Where ID Is Not nullptr And %s"), strSummation, strSummation, headerprofile.m_strKeyTable, headerprofile.m_strWhere);
	else
		strSQL.Format(_T("Select MIN(%s),MAX(%s) From %s Where ID Is Not nullptr"), strSummation, strSummation, headerprofile.m_strKeyTable);

	CODBCDatabase::ParseSQL(&database, strSQL);
	CRecordset rs(&database);
	try
	{
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		if(rs.IsEOF()==FALSE)
		{
			CString strValue;
			rs.GetFieldValue((short)0, strValue);
			m_minValue = _ttof(strValue);
			rs.GetFieldValue((short)1, strValue);
			m_maxValue = _ttof(strValue);
		}
		rs.Close();
	}
	catch(CDBException*ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
}

void CTheme::RedivideSize(int steps)
{
	if(steps==0)
		m_divisions.clear();
	else
	{
		while(m_divisions.size()>steps)
			m_divisions.pop_back();
		while(m_divisions.size()<steps)
			m_divisions.push_back(CDivision<DWORD>());
		const double segmentValue = (m_maxValue-m_minValue)/steps;
		const double segmentT = (m_continuous.m_maxT-m_continuous.m_minT)/steps;
		int index = 0;
		for(std::list<CDivision<DWORD>>::iterator it = m_divisions.begin(); it!=m_divisions.end(); ++it)
		{
			it->m_minValue = m_minValue+index*segmentValue;
			it->m_maxValue = m_minValue+index*segmentValue+segmentValue;
			it->m_Value = m_continuous.m_minT+index*segmentT;
			index++;
		}
	}
}

void CTheme::RedivideARGB(int steps)
{
	if(steps==0)
		m_divisions.clear();
	else if(m_divisions.size()!=steps)
	{
		while(m_divisions.size()>steps)
			m_divisions.pop_back();
		while(m_divisions.size()<steps)
			m_divisions.push_back(CDivision<Gdiplus::ARGB>());
		const BYTE minR = GetRValue(m_continuous.m_minT);
		const BYTE minG = GetGValue(m_continuous.m_minT);
		const BYTE minB = GetBValue(m_continuous.m_minT);
		const BYTE maxR = GetRValue(m_continuous.m_maxT);
		const BYTE maxG = GetGValue(m_continuous.m_maxT);
		const BYTE maxB = GetBValue(m_continuous.m_maxT);
		const double segmentValue = (m_maxValue-m_minValue)/steps;
		int index = 0;
		for(std::list<CDivision<Gdiplus::ARGB>>::iterator it = m_divisions.begin(); it!=m_divisions.end(); ++it)
		{
			it->m_minValue = m_minValue+index*segmentValue;
			it->m_maxValue = m_minValue+index*segmentValue+segmentValue;
			const int r = minR+(maxR-minR)*index/steps;
			const int g = minG+(maxG-minG)*index/steps;
			const int b = minB+(maxB-minB)*index/steps;
			it->m_Value = Gdiplus::Color::MakeARGB(255, r, g, b);

			index++;
		}
	}
	else if(m_divisions.front().m_minValue!=m_minValue||m_divisions.front().m_maxValue!=m_maxValue)
	{
		const double segmentValue = (m_maxValue-m_minValue)/steps;
		const int index = 0;
		for(std::list<CDivision<Gdiplus::ARGB>>::iterator it = m_divisions.begin(); it!=m_divisions.end(); ++it)
		{
			it->m_minValue = m_minValue+index*segmentValue;
			it->m_maxValue = m_minValue+index*segmentValue+segmentValue;
		}
	}
}

void CTheme::Invalidate(CDocument* pDocument) const
{
	(CFrameWnd*)AfxGetMainWnd()->PostMessage(WM_CROSSTHREAD, 1, (UINT)pDocument);
}

CString CTheme::MakrScript(CDatabase& database, CATTDatasource& datasource)
{
	const CHeaderProfile& headerprofile = datasource.GetHeaderProfile();
	const DBMSTYPE type = CODBCDatabase::GetDBMSType(&database);
	CString strFields;
	for(std::list<CString>::iterator it = m_fields.begin(); it!=m_fields.end(); ++it)
	{
		CString str;
		switch(type)
		{
		case DBMSTYPE::MSSQL:
			str.Format(_T("IsNull([%s],0) As [%s],"), *it, *it);
			break;
		case DBMSTYPE::Access:
			str.Format(_T("IIF(IsNull([%s].[%s]),0,[%s].[%s]) As [%s],"), headerprofile.m_strKeyTable, *it, headerprofile.m_strKeyTable, *it, *it);
			break;
		case DBMSTYPE::Oracle:
			str.Format(_T("NVL(%s,0) As %s,"), *it, *it);
			break;
		case DBMSTYPE::MySQL:
			str.Format(_T("IFNULL(%s,0) As %s,"), *it, *it);
			break;
		default:
			str.Format(_T("IsNull(%s,0) As %s,"), *it, *it);
			break;
		}

		strFields += str;
	}
	strFields = strFields.Trim(_T(","));

	CString strSQL;
	if(headerprofile.m_strWhere!=_T(""))
		strSQL.Format(_T("Select ID,%s From %s Where %s"), strFields, headerprofile.m_strKeyTable, headerprofile.m_strWhere);
	else
		strSQL.Format(_T("Select ID,%s From %s"), strFields, headerprofile.m_strKeyTable);

	CODBCDatabase::ParseSQL(&database, strSQL);
	return strSQL;
}

// 
//T CTheme::GetT(double value)
//{
//	//if(m_divisions.size()>0)
//	//	size = GetT(value);
//	//else if(m_maxValue != m_minValue)
//	//	size = m_continuous.m_minT + (m_continuous.m_maxT-m_continuous.m_minT)*value/(m_maxValue-m_minValue);
//	//else
//	//	size = (m_continuous.m_maxT+m_continuous.m_minT)/2;
//	//if(value<=potions.begin()->m_minValue)
//	//	return potions.begin()->m_Value;
//	//if(value>=potions.rbegin()->m_minValue)
//	//	return potions.rbegin()->m_Value;
//	//for(std::list<CDivision<T>>::iterator it=potions.begin();it!= potions.end();++it)
//	//{
//	//	if(value>=it->m_minValue && value<it->m_maxValue)
//	//		return it->m_Value;
//	//}
//
//	return T(0);
//};

Gdiplus::ARGB CTheme::LightColor(Gdiplus::ARGB argb)
{
	const BYTE a = Gdiplus::Color(argb).GetA();
	BYTE r = Gdiplus::Color(argb).GetR();
	BYTE g = Gdiplus::Color(argb).GetG();
	BYTE b = Gdiplus::Color(argb).GetB();
	const float rGrad = (255-r)/255.0f;
	const float gGrad = (255-g)/255.0f;
	const float bGrad = (255-b)/255.0f;

	r = BYTE(min(r+rGrad*50, 255.f));
	b = BYTE(min(b+bGrad*50, 255.f));
	g = BYTE(min(g+gGrad*50, 255.f));

	return Gdiplus::Color::MakeARGB(a, r, g, b);
}

Gdiplus::ARGB CTheme::DarkColor(Gdiplus::ARGB argb)
{
	const BYTE a = Gdiplus::Color(argb).GetA();
	BYTE r = Gdiplus::Color(argb).GetR();
	BYTE g = Gdiplus::Color(argb).GetG();
	BYTE b = Gdiplus::Color(argb).GetB();
	const float rGrad = r/255.0f;
	const float gGrad = g/255.0f;
	const float bGrad = b/255.0f;

	r = BYTE(max(r-rGrad*50, 0.f));
	b = BYTE(max(b-bGrad*50, 0.f));
	g = BYTE(max(g-gGrad*50, 0.f));

	return Gdiplus::Color::MakeARGB(a, r, g, b);
}

CTheme* CTheme::Apply(BYTE type)
{
	switch(type)
	{
	case 0:
		break;
	case 1:
		return new CChoropleth();
		break;
	case 2:
		return new CGraduated();
		break;
	case 5:
		return new CPie();
		break;
	case 6:
		return new CBar();
		break;
	case 7:
		return new CCylinder();
		break;
	}

	return nullptr;
};

void AFXAPI operator <<(CArchive& ar, CContinous<DWORD>& continous)
{
	ar<<continous.m_minT;
	ar<<continous.m_maxT;
	ar<<continous.m_scaling;
};

void AFXAPI operator >>(CArchive& ar, CContinous<DWORD>& continous)
{
	DWORD byte;
	ar>>continous.m_minT;
	ar>>continous.m_maxT;
	ar>>byte;
	continous.m_scaling = (CContinous<DWORD>::Scaling)byte;
};
void CTheme::Serialize(CArchive& ar, const DWORD dwVersion, std::list<CDivision<DWORD>>& divisions)
{
	if(ar.IsStoring())
	{
		ar << (int)divisions.size();
		for(std::list<CDivision<DWORD>>::iterator it = divisions.begin(); it != divisions.end(); ++it)
		{
			(*it).Serialize(ar, dwVersion);
		}
	}
	else
	{
		int count;
		ar >> count;
		for(int index = 0; index < count; index++)
		{
			divisions.push_back(CDivision<DWORD>());
		}
		for(std::list<CDivision<DWORD>>::iterator it = divisions.begin(); it != divisions.end(); ++it)
		{
			(*it).Serialize(ar, dwVersion);
		}
	}
}
