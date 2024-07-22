#include "Stdafx.h"
#include "Databaseconnection.h"

#include "../Public/Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CDatabaseconnection::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar<<m_dbms;
		ar<<m_strServer;
		if(m_dbms==DBMSTYPE::Access)
		{
			CString strDatabase = m_strDatabase;
			AfxToRelatedPath(ar.m_strFileName, strDatabase);
			ar<<strDatabase;
		}
		else
		{
			ar<<m_strDatabase;
		}
		ar<<m_strUID;
		ar<<m_strPWD;
	}
	else
	{
		ar>>m_dbms;
		ar>>m_strServer;
		ar>>m_strDatabase;
		ar>>m_strUID;
		ar>>m_strPWD;
		if(m_dbms==DBMSTYPE::Access)
		{
			AfxToFullPath(ar.m_strFileName, m_strDatabase);
		}
	}
}

void AFXAPI operator <<(CArchive& ar, DBMSTYPE& type)
{
	ar<<(BYTE)type;
};

void AFXAPI operator >>(CArchive& ar, DBMSTYPE& type)
{
	BYTE byte;
	ar>>byte;
	type = (DBMSTYPE)byte;
};
