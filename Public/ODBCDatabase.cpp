#include "stdafx.h"
#include <odbcinst.h>

#include "Global.h"
#include "Function.h"
#include "ODBCDatabase.h"

#include "../Viewer/Global.h"

using namespace std;
#include <sstream>
#include <iterator>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

IMPLEMENT_SERIAL(CODBCDatabase, CDatabase, 0)

CODBCDatabase::CODBCDatabase()
{
	m_dbms=DBMSTYPE::UnKnown;
}

CODBCDatabase::~CODBCDatabase()
{
	if(IsOpen() == TRUE)
	{
		Close();
	}
}

BOOL CODBCDatabase::operator==(const CODBCDatabase& database) const
{
	CRuntimeClass* pRuntimeClass1=this->GetRuntimeClass();
	CRuntimeClass* pRuntimeClass2=database.GetRuntimeClass();
	if(pRuntimeClass1 != pRuntimeClass2)
		return FALSE;
	else if(m_dbms != database.m_dbms)
		return FALSE;
	else if(m_strServer != database.m_strServer)
		return FALSE;
	else if(m_strDatabase != database.m_strDatabase)
		return FALSE;
	else
		return TRUE;
}

BOOL CODBCDatabase::operator !=(const CODBCDatabase& database) const
{
	return !(*this == database);
}

CODBCDatabase& CODBCDatabase::operator =(const CODBCDatabase& source)
{
	m_dbms=source.m_dbms;
	m_strServer=source.m_strServer;
	m_strDatabase=source.m_strDatabase;
	m_strUID=source.m_strUID;
	m_strPWD=source.m_strPWD;
	return *this;
}

CODBCDatabase* CODBCDatabase::Clone() const
{
	CRuntimeClass* pRuntimeClass=this->GetRuntimeClass();
	CODBCDatabase* pClone=(CODBCDatabase*)pRuntimeClass->CreateObject();
	*pClone=*this;
	return pClone;
}
void CODBCDatabase::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar << m_dbms;
		ar << m_strServer;
		if(m_dbms == DBMSTYPE::Access)
		{
			CString strDatabase=m_strDatabase;
			const CString path1=m_strDatabase.Left(m_strDatabase.ReverseFind('\\'));
			const CString path2=ar.m_strFileName.Left(ar.m_strFileName.ReverseFind('\\'));
			if(path1.IsEmpty())
			{
			}
			else if(path2.IsEmpty())
			{
			}
			else if(path1 != path2)
			{
				strDatabase=ar.m_strFileName.Mid(ar.m_strFileName.ReverseFind('\\') + 1);
				strDatabase=strDatabase.Left(strDatabase.ReverseFind('.')) + _T(".mdb");
				::CopyFile(m_strDatabase, path2 + "\\" + strDatabase, TRUE);
				m_strDatabase=path2 + "\\" + strDatabase;

				CODBCDatabase::Open(CDatabase::noOdbcDialog);
			}
			else
				AfxToRelatedPath(ar.m_strFileName, strDatabase);

			ar << strDatabase;
		}
		else
		{
			ar << m_strDatabase;
		}
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
		if(m_dbms == DBMSTYPE::Access)
		{
			AfxToFullPath(ar.m_strFileName, m_strDatabase);
		}
	}
}

void CODBCDatabase::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		const CString strDatabase=m_strDatabase;
		/*	if(m_bFileSource == TRUE)
			{
				AfxToRelatedPath(ar.m_strFileName,strDatabase);
			}*/

		SerializeStrCE(ar, m_strServer);
		SerializeStrCE(ar, strDatabase);
		SerializeStrCE(ar, m_strUID);
		SerializeStrCE(ar, m_strPWD);
	}
	else
	{
		SerializeStrCE(ar, m_strServer);
		SerializeStrCE(ar, m_strDatabase);
		SerializeStrCE(ar, m_strUID);
		SerializeStrCE(ar, m_strPWD);

		/*	if(m_bFileSource == TRUE)
			{
				AfxToFullPath(ar.m_strFileName,m_strDatabase);
			}*/
	}
}

void CODBCDatabase::ReleaseDCOM(CArchive& ar)
{
	if(ar.IsStoring())
	{
		const CString strDatabase=m_strDatabase;
		/*if(m_bFileSource == TRUE)
		{
			AfxToRelatedPath(ar.m_strFileName,strDatabase);
		}*/

		ar << m_dbms;
		ar << m_strServer;
		ar << strDatabase;
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

		/*if(m_bFileSource == TRUE)
		{
			AfxToFullPath(ar.m_strFileName,m_strDatabase);
		}*/
	}
}

BOOL CODBCDatabase::Open(DWORD dwOptions)
{
	Close();

	if(m_strDatabase.IsEmpty() == TRUE)
		return FALSE;
	else if(IsDriverExsiting(m_dbms) == FALSE)
		return false;
	else if(m_dbms == DBMSTYPE::Access)
	{
	}
	else if(m_strServer.IsEmpty() == TRUE)
		return false;

	try
	{
		SetLoginTimeout(60);
		const CString strConnectionstring=this->GetODBCConnectionString();
		return CDatabase::OpenEx(strConnectionstring, dwOptions);
	}
	catch(CDBException* ex)
	{
		CString msg = ex->m_strError;
		msg.Replace(_T("'(unknown)'"), m_strDatabase);
		OutputDebugString(msg);
		ex->ReportError();
		ex->Delete();
	}
	catch(CMemoryException* ex)
	{
		ex->Delete();
	}

	return FALSE;
}

DBMSTYPE CODBCDatabase::GetDBMSType(const CDatabase* pDatabase)
{
	if(pDatabase == nullptr)
		return DBMSTYPE::UnKnown;
	if(pDatabase->IsOpen() == FALSE)
		return DBMSTYPE::UnKnown;

	SWORD cbData;
	char buffer[250];
	::SQLGetInfoA(pDatabase->m_hdbc, SQL_DBMS_NAME, (PTR)buffer, 200, &cbData);
	CString dbms=CString(buffer);
	dbms.MakeUpper();

	if(dbms.Find(_T("ORACLE")) >= 0)
		return DBMSTYPE::Oracle;
	else if(dbms.Find(_T("MICROSOFT SQL SERVER")) >= 0)
		return DBMSTYPE::MSSQL;
	else if(dbms.Find(_T("ACCESS")) >= 0)
		return DBMSTYPE::Access;
	else if(dbms.Find(_T("MYSQL")) >= 0)
		return DBMSTYPE::MySQL;
	else if(dbms.Find(_T("DBASE")) >= 0)
		return DBMSTYPE::Dbase;
	else
		return DBMSTYPE::UnKnown;
}

void CODBCDatabase::ParseSQL(const CDatabase* pDatabase, CString& strSQL)
{
	const DBMSTYPE type=CODBCDatabase::GetDBMSType(pDatabase);
	switch(type)
	{
		case DBMSTYPE::Oracle:
		{
			strSQL.Replace(_T("["), nullptr);
			strSQL.Replace(_T("]"), nullptr);
			strSQL.Replace(_T("ISNULL"), _T("NVL"));
		}
		break;
		default:
			break;
	}
}

CString CODBCDatabase::GetTag(CString strTable, const DWORD& dwId)
{
	CString strTag;
	if(strTable.IsEmpty())
		return strTag;

	if(CDatabase::IsOpen() == FALSE)
		return strTag;

	CString strSQL;
	strSQL.Format(_T("SELECT * FROM [%s] WHERE ID=%d"), strTable, dwId);
	CODBCDatabase::ParseSQL(this, strSQL);

	CRecordset rs(this);
	try
	{
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		if(rs.IsEOF() == FALSE)
		{
			for(short nIndex=0; nIndex < rs.GetODBCFieldCount(); nIndex++)
			{
				CODBCFieldInfo fieldinfo;
				rs.GetODBCFieldInfo(nIndex, fieldinfo);
				if(fieldinfo.m_strName == _T("ID"))
					continue;
				else if(fieldinfo.m_strName.CompareNoCase(_T("X")) == 0)
					continue;
				else if(fieldinfo.m_strName.CompareNoCase(_T("Y")) == 0)
					continue;

				CString strValue;
				rs.GetFieldValue(nIndex, strValue);
				strValue.Trim();

				strTag+=fieldinfo.m_strName + _T(": ") + strValue + _T("\n");
			}
		}
		rs.Close();
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
	return strTag.Trim(_T("\n"));
}

void CODBCDatabase::ShowInfo(CView* pView, CString strTable, const DWORD& dwId)
{
	if(strTable.IsEmpty())
		return;

	if(IsOpen() == FALSE)
		return;

	CRecordset rs(this);
	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM [%s] WHERE ID=%d"), strTable, dwId);
		CODBCDatabase::ParseSQL(this, strSQL);

		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		CString strPath;
		CString strRegistryKey=AfxGetApp()->m_pszRegistryKey;
		if(strRegistryKey.IsEmpty() == FALSE)
		{
			strPath=AfxGetProfileString(HKEY_LOCAL_MACHINE, strRegistryKey, _T("Map"), _T("Path"), nullptr);
		}
		else
		{
			CDocument* pDocument=pView->GetDocument();
			strPath=pDocument->GetPathName();
			if(strPath.ReverseFind(_T('.')) != -1)
			{
				strPath=strPath.Left(strPath.ReverseFind(_T('.')));
			}
		}

		int fieldCount=rs.GetODBCFieldCount();
		while(!rs.IsEOF())
		{
			for(short nIndex=0; nIndex < fieldCount; nIndex++)
			{
				CString strValue;
				rs.GetFieldValue(nIndex, strValue);

				CODBCFieldInfo fieldinfo;
				rs.GetODBCFieldInfo(nIndex, fieldinfo);
				if(fieldinfo.m_strName.CompareNoCase(_T("Voice")) == 0)
				{
					AfxToFullPath(strPath, strValue);
					PlayMCI(strValue);
				}
				else if(fieldinfo.m_strName.CompareNoCase(_T("Html")) == 0)
				{
					AfxToFullPath(strPath, strValue);
					AfxGetMainWnd()->SendMessage(WM_BROWSEHTML, (UINT)&strValue, 1);
				}
			}
			rs.MoveNext();
		}

		rs.Close();
	}
	catch(CDBException* ex)
	{
		ex->ReportError();
		ex->Delete();
	}
}

CString CODBCDatabase::GetODBCConnectionString()
{
	CString strConnectstring;
	switch(m_dbms)
	{
		case DBMSTYPE::MSSQL:
			strConnectstring.Format(_T("Driver={SQL Server};Server=%s;Database=%s;Uid=%s;Pwd=%s;Trusted_Connection=no;Regional=No;"), m_strServer, m_strDatabase, m_strUID, m_strPWD);
			break;
		case DBMSTYPE::Oracle:
			strConnectstring.Format(_T("Driver={Oracle ODBC Driver};Connectstring=%s.%s;User Id=%s;Password=%s;"), m_strServer, m_strDatabase, m_strUID, m_strPWD);
			break;
		case DBMSTYPE::Access:
			strConnectstring.Format(_T("Driver={Microsoft Access Driver (*.mdb)};DBQ=%s;Uid=%s;Pwd=%s;"), m_strDatabase, m_strUID, m_strPWD);
			break;
		case DBMSTYPE::Dbase:
			strConnectstring.Format(_T("Driver=Microsoft.Jet.OLEDB.4.0;Data Source=%s;User Id=%s;Password=%s;"), m_strServer, m_strUID, m_strPWD);
			break;
	}

	return strConnectstring;
}

CString CODBCDatabase::GetADOConnectionString()
{
	if(CDatabase::IsOpen() == FALSE)
		return _T("");

	CString strConnectstring;
	switch(m_dbms)
	{
		case DBMSTYPE::MSSQL:
			strConnectstring.Format(_T("Provider=sqloledb;Data Source=%s;Initial Catalog=%s;User Id=%s;Password=%s;"), m_strServer, m_strDatabase, m_strUID, m_strPWD);
			break;
		case DBMSTYPE::Oracle:
			strConnectstring.Format(_T("Provider=MSDAORA.1;Data Source=%s;User Id=%s;Password=%s;"), m_strDatabase, m_strUID, m_strPWD);
			break;
		case DBMSTYPE::Access:
			strConnectstring.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;"), m_strDatabase);
			break;
		case DBMSTYPE::Dbase:
			strConnectstring.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s.dbf;User Id=%s;Password=%s;"), m_strDatabase, m_strUID, m_strPWD);
			break;
	}

	return strConnectstring;
}

BOOL CODBCDatabase::IsDriverExsiting(const DBMSTYPE dbms)
{
	return TRUE;
}

BOOL CODBCDatabase::IsDSNExsiting(CString dbms)
{
	const char datasrc[1024]={0};
	const char descrip[1024]={0};
	short rdsrc=0, rdesc=0;
	SQLHENV hEnv;
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HENV, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);

	SQLRETURN ret=SQLDataSources(hEnv, SQL_FETCH_FIRST_SYSTEM, (SQLTCHAR*)datasrc, 1024, &rdsrc, (SQLTCHAR*)descrip, 1024, &rdesc);
	while(ret != SQL_NO_DATA)
	{
		//      CharToOem(datasrc, datasrc);
		//      CharToOem(descrip, descrip);

		ret=SQLDataSources(hEnv, SQL_FETCH_NEXT, (SQLTCHAR*)datasrc, 1024, &rdsrc, (SQLTCHAR*)descrip, 1024, &rdesc);
	}

	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

	return TRUE;
}

BOOL CODBCDatabase::CreateUserDSN(CString dbms)
{
	dbms=dbms.MakeUpper();
	if(dbms.Find(_T("Microsoft SQL Server")) >= 0)
	{
		return ::SQLConfigDataSource(nullptr, ODBC_ADD_DSN, _T("SQL Server"), _T("DSN=SQL Server"));
	}
	/*else if(dbms.Find(_T("ORACLE")) >= 0)
	{

	}*/
	else if(dbms.Find(_T("ACCESS")) >= 0)
	{
		return ::SQLConfigDataSource(nullptr, ODBC_ADD_DSN, _T("Microsoft Access Driver (*.mdb)"), _T("DSN=Microsoft Access Database"));
	}
	/*else if(dbms.Find(_T("DBASE")) >= 0)
	{

	}*/
	else
		return FALSE;
}

CString CODBCDatabase::GetServerFromDSN(CString strDSN, CString strDefault)
{
	HKEY hKey=nullptr;
	if(::RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\ODBC\\ODBC.INI\\") + strDSN, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwSize=255;
		TCHAR szString[255];
		const LONG lRet=::RegQueryValueEx(hKey, _T("SERVER"), nullptr, nullptr, (LPBYTE)szString, &dwSize);
		if(lRet == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return CString(szString);
		}

		RegCloseKey(hKey);
	}
	return strDefault;
}

SWORD CODBCDatabase::GetFieldSQLType(CDatabase* pDatabase, CString strTable, CString strField)
{
	SWORD sqltype=SQL_UNKNOWN_TYPE;
	if(pDatabase == nullptr)
		return sqltype;
	if(pDatabase->IsOpen() == FALSE)
		return sqltype;

	if(strTable.IsEmpty() == TRUE)
		return sqltype;
	if(strField.IsEmpty() == TRUE)
		return sqltype;

	CRecordset rs(pDatabase);
	try
	{
		CString strSQL;
		strSQL.Format(_T("Select [%s] FROM [%s] Where 1=0"), strField, strTable);
		CODBCDatabase::ParseSQL(pDatabase, strSQL);
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);

		if(rs.IsOpen())
		{
			CODBCFieldInfo fieldinfo;
			rs.GetODBCFieldInfo(strField, fieldinfo);
			sqltype=fieldinfo.m_nSQLType;
		}
		rs.Close();
	}
	catch(CDBException* ex)
	{
		ex->ReportError();
		ex->Delete();
	}
	return sqltype;
}

bool CODBCDatabase::IsFiledNumeric(CDatabase* pDatabase, CString strTable, CString strField)
{
	const SWORD sqltype=CODBCDatabase::GetFieldSQLType(pDatabase, strTable, strField);
	switch(sqltype)
	{
		case SQL_GUID:
		case SQL_BIT:
		case SQL_FLOAT:
		case SQL_REAL:
		case SQL_DOUBLE:
		case SQL_DECIMAL:
		case SQL_NUMERIC:
		case SQL_TINYINT:
		case SQL_SMALLINT:
		case SQL_INTEGER:
		case SQL_BIGINT:
			return true;
		default:
			return false;
	}
}

CString CODBCDatabase::SmartClause(SWORD sqltype, CString field, CString value)
{
	CString stat;
	switch(sqltype)
	{
		case SQL_UNKNOWN_TYPE:
			break;
		case SQL_BINARY:
		case SQL_VARBINARY:
		case SQL_LONGVARBINARY:
			break;
		case SQL_CHAR:
		case SQL_VARCHAR:
		case SQL_LONGVARCHAR:
		case SQL_WCHAR:
		case SQL_WVARCHAR:
		case SQL_WLONGVARCHAR:
		{
			value.Replace(_T("*"), _T("%"));
			if(value == _T("%"))
				break;
			else if(value.Find(_T("%")) == -1)
				stat.Format(_T("%s like '%s%c'"), field, value, _T('%'));
			else
				stat.Format(_T("%s like '%s'"), field, value);
		}
		break;
		case SQL_NUMERIC:
		case SQL_DECIMAL:
		case SQL_INTEGER:
		case SQL_BIGINT:
		case SQL_SMALLINT:
		case SQL_FLOAT:
		case SQL_REAL:
		case SQL_DOUBLE:
		{
			if(value.Find(_T(" ")) != -1)
			{
				const std::string str(CW2A(value.GetString()));
				std::istringstream iss(str);
				std::vector<string> tokens;
				std::copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string>>(tokens));
				if(tokens.size() == 1 && IsNumeric(tokens[0].c_str()))
					stat.Format(_T("%s>=%s"), field, tokens[0]);
				else if(tokens.size() >= 2 && IsNumeric(tokens[0].c_str()) && IsNumeric(tokens[1].c_str()))
					stat.Format(_T("%s>=%s And %s<=%s"), field, tokens[0], field, tokens[1]);
			}
			else if(IsNumeric((LPCSTR)CStringA(value)))
				stat.Format(_T("%s=%s"), field, value);
		}
		break;
		case SQL_TIME:
		case SQL_DATETIME:
		case SQL_TYPE_DATE:
		case SQL_TYPE_TIME:
		case SQL_TYPE_TIMESTAMP:
			break;
	}

	return stat;
}

bool CODBCDatabase::ReadValue(const CString& strSQL, bool& value)
{
	try
	{
		CRecordset rs(this);
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		if(rs.IsEOF() == FALSE)
		{
			//	value = (bool)rs[0];
		}
		rs.Close();
		return value;
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		//	AfxMessageBox(e.ErrorMessage());
		ex->Delete();
	}
	return true;
}
