#pragma once

enum DBMSTYPE : BYTE
{
	UnKnown = 0XFF,
	MSSQL = 0,
	Oracle = 1,
	Access = 2,
	MySQL = 3,
	Dbase = 4
};

void AFXAPI operator <<(CArchive& ar, DBMSTYPE& type);
void AFXAPI operator >>(CArchive& ar, DBMSTYPE& type);

class CDatabaseconnection
{
public:
	CString m_strName;

	DBMSTYPE m_dbms;
	CString m_strServer;
	CString m_strDatabase;
	CString m_strUID;
	CString m_strPWD;

public:
	virtual CString GetODBCConnectionString();
	virtual CString GetADOConnectionString();

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
};
