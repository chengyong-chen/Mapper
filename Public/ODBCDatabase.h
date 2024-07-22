#pragma once

#include <afxdb.h>
#include "DatabaseConnection.h"

class __declspec(dllexport) CODBCDatabase : public CDatabase
{
protected:
	DECLARE_SERIAL(CODBCDatabase);
	CODBCDatabase();

	~CODBCDatabase() override;

public:
	BOOL Open(DWORD dwOptions = CDatabase::noOdbcDialog);

public:
	void ShowInfo(CView* pView, CString strTable, const DWORD& dwId);
	CString GetTag(CString strTable, const DWORD& dwId);

public:
	DBMSTYPE m_dbms;
	CString m_strServer;
	CString m_strDatabase;
	CString m_strUID;
	CString m_strPWD;
public:
	bool ReadValue(const CString& strSQL, bool& value);

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void ReleaseDCOM(CArchive& ar);

	virtual CString GetODBCConnectionString();
	virtual CString GetADOConnectionString();
	friend class CDatabase;
public:
	CODBCDatabase* Clone() const;
	virtual CODBCDatabase& operator =(const CODBCDatabase& source);
	virtual BOOL operator ==(const CODBCDatabase& database) const;
	virtual BOOL operator !=(const CODBCDatabase& database) const;

public:
	static BOOL IsDriverExsiting(const DBMSTYPE dbms);
	static BOOL IsDSNExsiting(CString strDSN);
	static BOOL CreateUserDSN(CString strDSN);
	static CString GetServerFromDSN(CString strDSN, CString strDefault);
	static DBMSTYPE GetDBMSType(const CDatabase* pDatabase);
	static void ParseSQL(const CDatabase* pDatabase, CString& strSQL);
	static SWORD GetFieldSQLType(CDatabase* pDatabase, CString strTable, CString strField);
	static bool IsFiledNumeric(CDatabase* pDatabase, CString strTable, CString strFiled);
	static CString SmartClause(SWORD sqltype, CString strFiled, CString strValue);
};

typedef CTypedPtrList<CObList, CODBCDatabase*> CODBCDatabaseList;
