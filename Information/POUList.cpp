#include "Stdafx.h"

#include "POUList.h"
#include "POU.h"

#include "../DataView/Datainfo.h"
#include "../Public/ODBCDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPOUList::CPOUList()
{
}

CPOUList::~CPOUList()
{
	for(std::list<CPOU*>::iterator it = m_pous.begin(); it!=m_pous.end(); ++it)
	{
		const CPOU* pPOU = *it;
		delete pPOU;
	}
	m_pous.clear();

	for(std::map<CString, CODBCDatabase*>::iterator it = m_DBList.begin(); it!=m_DBList.end(); it++)
	{
		CODBCDatabase* pDatabase = it->second;
		if(pDatabase!=nullptr)
		{
			pDatabase->Close();
			delete pDatabase;
			pDatabase = nullptr;
		}
	}
	m_DBList.clear();
}

CODBCDatabase* CPOUList::GetDatabase(CString strName)
{
	if(m_DBList.find(strName)!=m_DBList.end())
		return m_DBList[strName];
	else
		return nullptr;
}

void CPOUList::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	m_library.Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<(int)(m_DBList.size());
		for(std::map<CString, CODBCDatabase*>::iterator it = m_DBList.begin(); it!=m_DBList.end(); ++it)
		{
			ar<<it->first;
			CODBCDatabase* pODBCDatabase = it->second;
			pODBCDatabase->Serialize(ar, dwVersion);
		}

		ar<<(int)(m_pous.size());
	}
	else
	{
		int countDB;
		ar>>countDB;
		for(int index = 0; index<countDB; index++)
		{
			CString strName;
			ar>>strName;
			CODBCDatabase* pODBCDatabase = new CODBCDatabase();
			pODBCDatabase->Serialize(ar, dwVersion);
			if(m_DBList.find(strName)==m_DBList.end())
			{
				m_DBList[strName] = pODBCDatabase;
				pODBCDatabase->Open(CDatabase::noOdbcDialog);
			}
			else
			{
				delete pODBCDatabase;
				pODBCDatabase = nullptr;
			}
		}
		int countPOU;
		ar>>countPOU;
		for(int index = 0; index<countPOU; index++)
		{
			CPOU* pPOU = new CPOU(*this);
			m_pous.push_back(pPOU);
		}
	}

	for(std::list<CPOU*>::iterator it = m_pous.begin(); it!=m_pous.end(); ++it)
	{
		CPOU* pPOU = *it;
		pPOU->Serialize(ar, dwVersion);
	}
}

void CPOUList::ReleaseWEB(CString strFile, const CDatainfo& datainfo)
{
	CFile file;
	if(file.Open(strFile, CFile::modeCreate|CFile::modeWrite)==TRUE)
	{
		m_library.ReleaseWeb(file);

		unsigned short nCount = m_pous.size();
		ReverseOrder(nCount);
		file.Write(&nCount, sizeof(unsigned short));

		for(std::list<CPOU*>::iterator it = m_pous.begin(); it!=m_pous.end(); ++it)
		{
			CPOU* pPOU = *it;
			if(pPOU!=nullptr)
				pPOU->ReleaseWeb(file, datainfo);
		}
		file.Close();
	}
}
