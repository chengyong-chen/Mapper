#include "StdAfx.h"
#include "ADODatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CADODatabase::CADODatabase(void)
{
	m_dbms = DBMSTYPE::UnKnown;

	m_connection = nullptr;
}


CADODatabase::~CADODatabase(void)
{
	if(m_connection != nullptr)
	{
		m_connection->Close();
		m_connection = nullptr;
	}
}

void CADODatabase::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar << m_dbms;
		ar << m_strServer;
		ar << m_strDatabase;		
		ar << m_strUID;
		ar << m_strPWD;
	}
	else
	{
		ar >> m_dbms;
		ar >> m_strServer;
		ar >> m_strDatabase;
		ar >> m_strUID;
		ar >> m_strPWD;
	}
}

BOOL CADODatabase::Open()
{
	if(m_connection != nullptr)
		return TRUE;
	else if(m_strDatabase.IsEmpty() == TRUE)
		return FALSE;
	else
	{
		CString strConnection;
		switch(m_dbms)
		{
		case DBMSTYPE::Access:
			strConnection.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;User Id=%s;Password=%s;"),m_strDatabase,m_strUID,m_strPWD);
			break;
		case DBMSTYPE::SQL_SERVER:
			strConnection.Format(_T("Provider=sqloledb;Data Source=%s;Initial Catalog=%s;User Id=%s;Password=%s;"),m_strServer,m_strDatabase,m_strUID,m_strPWD);
			break;
		case DBMSTYPE::Oracle:
			strConnection.Format(_T("Provider=MSDAORA.1;Data Source=%s;User Id=%s;Password=%s;"),m_strDatabase,m_strUID,m_strPWD);
			break;
		}

		try
		{
			ADOCG::_ConnectionPtr connection;
			connection.CreateInstance(_uuidof(Connection));
			_bstr_t bstrConnection(strConnection);
			connection->CommandTimeout = 3000;
			connection->ConnectionTimeout = 3000;
			connection->Open(bstrConnection, "", "",0);
			m_connection = connection;
		}
		catch(_com_error& e)
		{	
			OutputDebugString(ex->m_strError+"\r\n");
		//	AfxMessageBox(e.ErrorMessage());
		}
		return m_connection != nullptr;
	}
}

void CADODatabase::Close()
{

}