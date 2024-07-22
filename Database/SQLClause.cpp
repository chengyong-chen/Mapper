#include "Stdafx.h"

#include "SQLClause.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CClauseSelect::CClauseSelect()
{
	m_strFields = _T("*");
}

BOOL CClauseSelect::operator !=(const CClauseSelect& source) const
{
	return !(*this==source);
}

BOOL CClauseSelect::operator ==(const CClauseSelect& source) const
{
	if(m_strFields!=m_strFields)
		return FALSE;
	else if(m_strTables!=m_strTables)
		return FALSE;
	else if(m_strWhere!=m_strWhere)
		return FALSE;
	else if(m_strGroupby!=m_strGroupby)
		return FALSE;
	else if(m_strOrderby!=m_strOrderby)
		return FALSE;
	else
		return TRUE;
}

void CClauseSelect::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar<<m_strFields;
		ar<<m_strTables;
		ar<<m_strWhere;
		ar<<m_strGroupby;
		ar<<m_strOrderby;
	}
	else
	{
		ar>>m_strFields;
		ar>>m_strTables;
		ar>>m_strWhere;
		ar>>m_strGroupby;
		ar>>m_strOrderby;
	}
}

CString CClauseSelect::MakeSelectStatment() const
{
	return _T("");
}
