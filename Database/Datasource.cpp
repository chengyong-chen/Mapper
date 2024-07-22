#include "Stdafx.h"

#include "Datasource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CHeaderProfile, CObject)

CHeaderProfile::CHeaderProfile()
{
	m_strFields = _T("*");
}

CHeaderProfile::CHeaderProfile(CHeaderProfile& source)
{
	*this = source;
}

CHeaderProfile& CHeaderProfile::operator =(const CHeaderProfile& source)
{
	m_strKeyTable = source.m_strKeyTable;
	m_strIdField = source.m_strIdField;
	m_strFields = source.m_strFields;
	m_strTables = source.m_strTables;
	m_strWhere = source.m_strWhere;

	return *this;
}

BOOL CHeaderProfile::operator !=(const CHeaderProfile& source) const
{
	return !(*this==source);
}

BOOL CHeaderProfile::operator ==(const CHeaderProfile& other) const
{
	if(m_strKeyTable!=other.m_strKeyTable)
		return FALSE;
	else if(m_strIdField!=other.m_strIdField)
		return FALSE;
	else if(m_strFields!=other.m_strFields)
		return FALSE;
	else if(m_strTables!=other.m_strTables)
		return FALSE;
	else if(m_strWhere!=other.m_strWhere)
		return FALSE;
	else
		return TRUE;
}

void CHeaderProfile::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar<<m_strKeyTable;
		ar<<m_strIdField;
		ar<<m_strAnnoField;
		ar<<m_strFields;
		ar<<m_strTables;
		ar<<m_strWhere;
	}
	else
	{
		ar>>m_strKeyTable;
		ar>>m_strIdField;
		ar>>m_strAnnoField;
		ar>>m_strFields;
		ar>>m_strTables;
		ar>>m_strWhere;
	}
}

CString CHeaderProfile::MakeSelectStatment() const
{
	CString strWhere = m_strWhere;
	strWhere = strWhere.Trim();
	CString strTable = m_strTables;
	if(m_strTables.IsEmpty()==FALSE)
		strTable = m_strTables;
	else
		strTable = m_strKeyTable;
	if(strTable.IsEmpty())
		return _T("");

	CString strFields;
	if(m_strFields.IsEmpty()==FALSE)
		strFields = m_strFields;
	else
		strFields = _T("*");

	CString strSQL = _T("Select ")+strFields+_T(" From [")+strTable+_T("]");
	if(strWhere.IsEmpty()==FALSE)
	{
		strSQL += _T(" Where ")+strWhere;
	}
	return strSQL;
}

CString CHeaderProfile::MakeSelectStatment(CString strField) const
{
	CString strWhere = m_strWhere;
	strWhere = strWhere.Trim();
	CString strTable = m_strTables;
	if(m_strTables.IsEmpty()==FALSE)
		strTable = m_strTables;
	else
		strTable = m_strKeyTable;
	if(strTable.IsEmpty())
		return _T("");


	CString strSQL = _T("Select ")+(strField!=m_strIdField ? m_strIdField+_T(",")+strField : strField)+_T(" From [")+strTable+_T("]");
	if(strWhere.IsEmpty()==FALSE)
	{
		strSQL += _T(" Where ")+strWhere;
	}
	return strSQL;
}
CString CHeaderProfile::MakeSelectStatment(CString strField, CString strWhere) const
{
	if(m_strWhere.IsEmpty()) 
	{
	}
	else
	{
		strWhere = m_strWhere + " And "+strWhere;
	}
	strWhere = strWhere.Trim();
	CString strTable = m_strTables;
	if(m_strTables.IsEmpty()==FALSE)
		strTable = m_strTables;
	else
		strTable = m_strKeyTable;
	if(strTable.IsEmpty())
		return _T("");


	CString strSQL = _T("Select ")+(strField!=m_strIdField ? m_strIdField+_T(",")+strField : strField)+_T(" From [")+strTable+_T("]");
	if(strWhere.IsEmpty()==FALSE)
	{
		strSQL += _T(" Where ")+strWhere;
	}
	return strSQL;
}
IMPLEMENT_DYNAMIC(CPOUHeaderProfile, CHeaderProfile)

CPOUHeaderProfile::CPOUHeaderProfile()
{
	m_coordinatortype = COORDINATORTYPE::Geo;
	m_strIdField = _T("PID");
	m_strXField = _T("Longitude");
	m_strYField = _T("Latitude");
}

CPOUHeaderProfile::CPOUHeaderProfile(CPOUHeaderProfile& source)
{
	*this = source;
}

CHeaderProfile& CPOUHeaderProfile::operator =(const CHeaderProfile& source)
{
	CHeaderProfile::operator=(source);
	if(source.IsKindOf(RUNTIME_CLASS(CPOUHeaderProfile))==TRUE)
	{
		m_coordinatortype = ((CPOUHeaderProfile&)source).m_coordinatortype;
		m_strXField = ((CPOUHeaderProfile&)source).m_strXField;
		m_strYField = ((CPOUHeaderProfile&)source).m_strYField;
	}
	return *this;
}

BOOL CPOUHeaderProfile::operator ==(const CHeaderProfile& other) const
{
	if(CHeaderProfile::operator==(other)==FALSE)
		return FALSE;
	else if(other.IsKindOf(RUNTIME_CLASS(CPOUHeaderProfile))==FALSE)
		return FALSE;
	else if(m_strAnnoField!=((CPOUHeaderProfile&)other).m_strAnnoField)
		return FALSE;
	else if(m_coordinatortype!=((CPOUHeaderProfile&)other).m_coordinatortype)
		return FALSE;
	else if(m_strXField!=((CPOUHeaderProfile&)other).m_strXField)
		return FALSE;
	else if(m_strYField!=((CPOUHeaderProfile&)other).m_strYField)
		return FALSE;
	else
		return TRUE;
}

void CPOUHeaderProfile::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CHeaderProfile::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_strXField;
		ar<<m_strYField;
		ar<<m_coordinatortype;
	}
	else
	{
		ar>>m_strXField;
		ar>>m_strYField;
		ar>>m_coordinatortype;
	}
}

CString CPOUHeaderProfile::MakeSelectStatment() const
{
	CString strWhere = m_strWhere;
	strWhere = strWhere.Trim();
	CString strTable = m_strTables;
	if(m_strTables.IsEmpty()==FALSE)
		strTable = m_strTables;
	else
		strTable = m_strKeyTable;
	CString strFields;
	if(m_strFields.IsEmpty()==FALSE)
		strFields = m_strFields;
	else
		strFields = _T("*");

	CString strSQL = _T("Select ")+strFields+_T(" From ")+strTable;
	if(strWhere.IsEmpty()==FALSE)
	{
		strSQL += _T(" Where ")+strWhere;
	}
	return strSQL;
}

IMPLEMENT_DYNAMIC(CGEOHeaderProfile, CHeaderProfile)

CGEOHeaderProfile::CGEOHeaderProfile()
{
	m_strAnnoField = _T("Name");
}

CHeaderProfile& CGEOHeaderProfile::operator =(const CHeaderProfile& source)
{
	CHeaderProfile::operator=(source);

	if(source.IsKindOf(RUNTIME_CLASS(CGEOHeaderProfile))==TRUE)
	{
		m_tableA = ((CGEOHeaderProfile&)source).m_tableA;
		m_tableB = ((CGEOHeaderProfile&)source).m_tableB;
		m_tableC = ((CGEOHeaderProfile&)source).m_tableC;
		m_whereA = ((CGEOHeaderProfile&)source).m_whereA;
		m_whereB = ((CGEOHeaderProfile&)source).m_whereB;
	}
	return *this;
}

BOOL CGEOHeaderProfile::operator ==(const CHeaderProfile& other) const
{
	if(CHeaderProfile::operator==(other)==FALSE)
		return FALSE;
	else if(other.IsKindOf(RUNTIME_CLASS(CGEOHeaderProfile))==FALSE)
		return FALSE;
	if(m_tableA!=((CGEOHeaderProfile&)other).m_tableA)
		return FALSE;
	else if(m_tableB!=((CGEOHeaderProfile&)other).m_tableB)
		return FALSE;
	else if(m_tableC!=((CGEOHeaderProfile&)other).m_tableC)
		return FALSE;
	else if(m_strAnnoField!=((CGEOHeaderProfile&)other).m_strAnnoField)
		return FALSE;
	else if(m_whereA!=((CGEOHeaderProfile&)other).m_whereA)
		return FALSE;
	else if(m_whereB!=((CGEOHeaderProfile&)other).m_whereB)
		return FALSE;
	else
		return TRUE;
}

void CGEOHeaderProfile::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar<<m_tableA;
		ar<<m_tableB;
		ar<<m_tableC;
		ar<<m_whereA;
		ar<<m_whereB;
	}
	else
	{
		ar>>m_tableA;
		ar>>m_tableB;
		ar>>m_tableC;
		ar>>m_whereA;
		ar>>m_whereB;
	}
}

CString CGEOHeaderProfile::MakeSelectStatment() const
{
	if(m_tableA==_T(""))
		return _T("");

	CString strTable;
	CString strWhere;
	if(m_whereA!=_T("")&&m_whereB!=_T(""))
	{
		strTable.Format(_T("%s A left join %s B On A.ID=B.ID"), m_tableA, m_tableB);
		strWhere.Format(_T("%s And %s"), m_whereA, m_whereB);
	}
	else if(m_whereA!=_T(""))
	{
		strTable.Format(_T("%s A"), m_tableA, m_tableB);
		strWhere = m_whereA;
	}
	else if(m_whereB!=_T(""))
	{
		strTable.Format(_T("%s A left join %s B On A.ID=B.ID"), m_tableA, m_tableB);
		strWhere = m_whereB;
	}
	else
	{
		strTable.Format(_T("%s A"), m_tableA);
	}

	CString strField;
	if(m_strAnnoField!=_T(""))
	{
		strTable.Format(_T("%s A left join %s B On A.ID=B.ID"), m_tableA, m_tableB);
		strField.Format(_T("A.*,B.%s"), m_strAnnoField);
	}
	else
	{
		strField = _T("A.*");
	}

	CString strSQL;
	if(strWhere.IsEmpty())
		strSQL.Format(_T("Select %s From %s"), strField, strTable);
	else
		strSQL.Format(_T("Select %s From %s Where %s And "), strField, strTable, strWhere);
	return strSQL;
}

IMPLEMENT_DYNAMIC(CATTHeaderProfile, CHeaderProfile)

CATTHeaderProfile::CATTHeaderProfile()
{
	m_strIdField = _T("AID");
}

CHeaderProfile& CATTHeaderProfile::operator =(const CHeaderProfile& source)
{
	CHeaderProfile::operator=(source);
	if(source.IsKindOf(RUNTIME_CLASS(CATTHeaderProfile))==TRUE)
	{
		m_strCodeField = ((CATTHeaderProfile&)source).m_strCodeField;
	}
	return *this;
}

BOOL CATTHeaderProfile::operator ==(const CHeaderProfile& other) const
{
	if(CHeaderProfile::operator==(other)==FALSE)
		return FALSE;
	else if(other.IsKindOf(RUNTIME_CLASS(CATTHeaderProfile))==FALSE)
		return FALSE;
	else if(m_strCodeField!=((CATTHeaderProfile&)other).m_strCodeField)
		return FALSE;
	else
		return TRUE;
}

void CATTHeaderProfile::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CHeaderProfile::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_strCodeField;
	}
	else
	{
		ar >> m_strCodeField;
	}
}

IMPLEMENT_DYNAMIC(CDatasource, CObject)

CDatasource::CDatasource()
{
}

CDatasource::CDatasource(CString database)
	: m_strDatabase(database)
{
}

CDatasource* CDatasource::Clone() const
{
	CRuntimeClass* pRuntimeClass = this->GetRuntimeClass();
	CDatasource* pClone = (CDatasource*)pRuntimeClass->CreateObject();
	*pClone = *this;
	return pClone;
}

CDatasource& CDatasource::operator =(const CDatasource& source)
{
	m_strDatabase = source.m_strDatabase;
	return *this;
}

BOOL CDatasource::operator !=(const CDatasource& source) const
{
	return !(*this==source);
}

BOOL CDatasource::operator ==(const CDatasource& other) const
{
	if(m_strDatabase!=other.m_strDatabase)
		return FALSE;
	else
		return TRUE;
}

void CDatasource::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar<<m_strDatabase;
	}
	else
	{
		ar>>m_strDatabase;
	}
}

IMPLEMENT_SERIAL(CATTDatasource, CDatasource, 0)

CATTDatasource::CATTDatasource()
{
}

CATTDatasource::CATTDatasource(CString database)
	: CDatasource(database)
{
}

CDatasource& CATTDatasource::operator =(const CDatasource& source)
{
	CDatasource::operator =(source);

	if(source.IsKindOf(RUNTIME_CLASS(CATTDatasource))==TRUE)
	{
		m_headerprofile = ((CATTDatasource&)source).m_headerprofile;
	}

	return *this;
}

BOOL CATTDatasource::operator ==(const CDatasource& other) const
{
	if(CDatasource::operator==(other)==FALSE)
		return FALSE;
	else if(other.IsKindOf(RUNTIME_CLASS(CATTDatasource))==FALSE)
		return FALSE;
	else if(m_headerprofile!=((CATTDatasource&)other).m_headerprofile)
		return FALSE;
	else
		return TRUE;
}

void CATTDatasource::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CDatasource::Serialize(ar, dwVersion);

	m_headerprofile.Serialize(ar, dwVersion);
}

IMPLEMENT_SERIAL(CGEODatasource, CDatasource, 0)

CGEODatasource::CGEODatasource()
{
}

CGEODatasource::CGEODatasource(CString database)
	: CDatasource(database)
{
}

CDatasource& CGEODatasource::operator=(const CDatasource& source)
{
	CDatasource::operator =(source);

	if(source.IsKindOf(RUNTIME_CLASS(CGEODatasource))==TRUE)
	{
		m_headerprofile = ((CGEODatasource&)source).m_headerprofile;
	}

	return *this;
}

BOOL CGEODatasource::operator ==(const CDatasource& other) const
{
	if(CDatasource::operator==(other)==FALSE)
		return FALSE;
	else if(other.IsKindOf(RUNTIME_CLASS(CGEODatasource))==FALSE)
		return FALSE;
	else if(m_headerprofile!=((CGEODatasource&)other).m_headerprofile)
		return FALSE;
	else
		return TRUE;
}

void CGEODatasource::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CDatasource::Serialize(ar, dwVersion);

	m_headerprofile.Serialize(ar, dwVersion);
}

IMPLEMENT_SERIAL(CPOUDatasource, CDatasource, 0)

CPOUDatasource::CPOUDatasource()
{
}

CPOUDatasource::CPOUDatasource(CString database)
	: CDatasource(database)
{
}

CDatasource& CPOUDatasource::operator=(const CDatasource& source)
{
	CDatasource::operator =(source);

	if(source.IsKindOf(RUNTIME_CLASS(CPOUDatasource))==TRUE)
	{
		m_headerprofile = ((CPOUDatasource&)source).m_headerprofile;
	}

	return *this;
}

BOOL CPOUDatasource::operator==(const CDatasource& other) const
{
	if(CDatasource::operator==(other)==FALSE)
		return FALSE;
	else if(other.IsKindOf(RUNTIME_CLASS(CPOUDatasource))==FALSE)
		return FALSE;
	if(m_headerprofile!=((CPOUDatasource&)other).m_headerprofile)
		return FALSE;
	else
		return TRUE;
}

void CPOUDatasource::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CDatasource::Serialize(ar, dwVersion);

	m_headerprofile.Serialize(ar, dwVersion);
}

void AFXAPI operator <<(CArchive& ar, CPOUHeaderProfile::COORDINATORTYPE& coortype)
{
	ar<<(BYTE)coortype;
};

void AFXAPI operator >>(CArchive& ar, CPOUHeaderProfile::COORDINATORTYPE& coortype)
{
	BYTE type;
	ar>>type;
	coortype = (CPOUHeaderProfile::COORDINATORTYPE)type;
};
