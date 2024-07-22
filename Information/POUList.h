#pragma once


using namespace std;
#include <map>
#include <list>
#include "../Style/Library.h"

class CPOU;
class CODBCDatabase;
class CDatainfo;

class __declspec(dllexport) CPOUList
{
public:
	CPOUList();
	~CPOUList();

public:
	CLibrary m_library;
	std::map<CString, CODBCDatabase*> m_DBList;
	std::list<CPOU*> m_pous;
public:
	CODBCDatabase* GetDatabase(CString strName);
public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseWEB(CString strFile, const CDatainfo& datainfo);
};
