#pragma once

#include "../Public/Databaseconnection.h"

class __declspec(dllexport) CADODatabase
{
public:
	CADODatabase(void);
	~CADODatabase(void);

public:
	DBMSTYPE m_dbms;
	CString m_strServer;
	CString m_strDatabase;	
	CString m_strUID;	
	CString m_strPWD;

public:
	ADOCG::_ConnectionPtr m_connection;
public:
	void Close();
public:
	BOOL Open();

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
};

