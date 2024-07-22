#pragma once

#include <map>

#include "../Pbf/writer.hpp"
#include "../Public/BinaryStream.h"
#include "../Public/Function.h"
#include <boost/json.hpp>


using namespace std;
namespace boost
{
	namespace json {
		class object;
	}
};

class CLine;
class CFill;
class CSpot;

template<class TMid>
class __declspec(dllexport) CMidtable : public CObject
{
public:
	std::map<WORD, TMid*> m_midlist;
public:
	CMidtable()
	{
	}

	~CMidtable() override
	{
		for(typename std::map<WORD, TMid*>::iterator it = m_midlist.begin(); it != m_midlist.end(); ++it)
		{
			const TMid* pMid = it->second;
			delete pMid;
		}
		m_midlist.clear();
	}

public:
	TMid* GetMid(WORD wId) const
	{
		typename std::map<WORD, TMid*>::const_iterator it = m_midlist.find(wId);
		if(it != m_midlist.cend())
			return it->second;
		else
			return nullptr;
	}

	WORD GetMaxId() const
	{
		WORD maxId = 0;
		for(typename std::map<WORD, TMid*>::const_iterator it = m_midlist.cbegin(); it != m_midlist.cend(); ++it)
		{
			TMid* pMid = it->second;
			maxId = max(maxId, pMid->m_wId);
		}

		return maxId;
	}

	void Remove(TMid* mid)
	{
		for(typename std::map<WORD, TMid*>::const_iterator it = m_midlist.cbegin(); it != m_midlist.cend(); ++it)
		{
			TMid* pMid = it->second;
			if(pMid == mid)
			{
				m_midlist.erase(it);
				break;
			}
		}
	}

	void Delete(TMid* mid)
	{
		for(typename std::map<WORD, TMid*>::iterator it = m_midlist.begin(); it != m_midlist.end(); ++it)
		{
			TMid* pMid = it->second;
			if(pMid == mid)
			{
				delete mid;
				mid = nullptr;
				m_midlist.erase(it);
				break;
			}
		}
	}

public:
	TMid* Find(CString strFile) const
	{
		for(typename std::map<WORD, TMid*>::const_iterator it = m_midlist.cbegin(); it != m_midlist.cend(); ++it)
		{
			TMid* pMid = it->second;
			if(pMid->m_strFile == strFile)
				return pMid;
		}
		return nullptr;
	}

	TMid* Find(TMid* pMid)
	{
		if(pMid == nullptr)
			return nullptr;

		for(typename std::map<WORD, TMid*>::iterator it = m_midlist.begin(); it != m_midlist.end(); ++it)
		{
			TMid* mid = it->second;
			if(mid->Equals(pMid) == TRUE)
				return mid;
		}
		return nullptr;
	}

	bool Contains(TMid* pT)
	{
		for(typename std::map<WORD, TMid*>::iterator it = m_midlist.begin(); it != m_midlist.end(); ++it)
		{
			TMid* pMid = it->second;
			if(pMid == pT)
				return true;
		}
		return false;
	}

public:
	void Increase(TMid* mid)
	{
		for(typename std::map<WORD, TMid*>::iterator it = m_midlist.begin(); it != m_midlist.end(); ++it)
		{
			TMid* pMid = it->second;
			if(pMid == mid)
			{
				mid->m_nUsers++;
				break;
			}
		}
	}

	void Decrease(WORD wId)
	{
		for(typename std::map<WORD, TMid*>::iterator it = m_midlist.begin(); it != m_midlist.end(); ++it)
		{
			TMid* pMid = it->second;
			if(pMid->m_wId == wId)
			{
				pMid->m_nUsers--;
				if(pMid->m_nUsers == 0)
				{
					this->Delete(pMid);
				}
				break;
			}
		}
	}

public:
	void Clear()
	{
		for(typename std::map<WORD, TMid*>::iterator it = m_midlist.begin(); it != m_midlist.end(); ++it)
		{
			const TMid* pMid = it->second;
			delete pMid;
		}

		m_midlist.clear();
	}

	template<class S>
	void Copy(CObject* pStyle, CMidtable<TMid>& midtable)
	{
		if(pStyle == nullptr)
			return;

		if(pStyle->IsKindOf(RUNTIME_CLASS(S)) == FALSE)
			return;

		S* pS = (S*)pStyle;
		TMid* pMid = this->GetMid(pS->m_libId);
		if(pMid == nullptr)
			return;

		TMid* pOld = midtable.Find(pMid);
		if(pOld != nullptr)
		{
			pOld->m_nUsers++;
			return;
		}
		pMid = (TMid*)pMid->Clone();
		if(pMid != nullptr)
		{
			pMid->m_nUsers = 1;
			midtable.m_midlist[pMid->m_wId] = pMid;
		}
	}

	template<class S>
	void Paste(CMidtable<TMid>& midtable, CObject* pStyle)
	{
		if(pStyle == nullptr)
			return;

		if(pStyle->IsKindOf(RUNTIME_CLASS(S)) == FALSE)
			return;

		S* pS = (S*)pStyle;
		TMid* pMid = midtable.GetMid(pS->m_libId);
		if(pMid == nullptr)
			return;

		TMid* mid1 = (TMid*)this->Find(pMid);
		if(mid1 == nullptr)
		{
			pMid->m_nUsers = 1;
			pMid->m_wId = this->GetMaxId() + 1;
			pS->m_libId = pMid->m_wId;
			m_midlist[pMid->m_wId] = pMid;
			midtable.Remove(pMid);
		}
		else
		{
			pS->m_libId = mid1->m_wId;
			pMid->m_nUsers++;
		}
	}

	virtual void Serialize(CArchive& ar, const DWORD& dwVersion)
	{
		if(ar.IsStoring() == TRUE)
		{
			ar << (int)m_midlist.size();
			for(typename std::map<WORD, TMid*>::iterator it = m_midlist.begin(); it != m_midlist.end(); ++it)
			{
				TMid* pMid = it->second;
				pMid->Serialize(ar, dwVersion);
			}
		}
		else
		{
			int count;
			ar >> count;
			for(int index = 0; index < count; index++)
			{
				TMid* pMid = new TMid();
				pMid->Serialize(ar, dwVersion);
				m_midlist[pMid->m_wId] = pMid;
			}
		}
	}

	virtual void ReleaseCE(CArchive& ar) const
	{
		if(ar.IsStoring() == TRUE)
		{
			ar << (int)m_midlist.size();

			for(typename std::map<WORD, TMid*>::const_iterator it = m_midlist.cbegin(); it != m_midlist.cend(); ++it)
			{
				TMid* pMid = it->second;
				pMid->ReleaseCE(ar);
			}
		}
	}

	virtual void ReleaseDCOM(CArchive& ar)
	{
		if(ar.IsStoring() == TRUE)
		{
			ar << (int)m_midlist.size();

			for(typename std::map<WORD, TMid*>::iterator it = m_midlist.begin(); it != m_midlist.end(); ++it)
			{
				TMid* pMid = it->second;
				pMid->ReleaseDCOM(ar);
			}
		}
		else
		{
			unsigned short nCount;
			ar >> nCount;
			for(unsigned short index = 0; index < nCount; index++)
			{
				TMid* pMid = new TMid();
				pMid->ReleaseDCOM(ar);
				m_midlist[pMid->m_wId] = pMid;
			}
		}
	}

	virtual void ReleaseWeb(CFile& file) const
	{
		unsigned short nCount = m_midlist.size();
		ReverseOrder(nCount);
		file.Write(&nCount, sizeof(unsigned short));
		for(typename std::map<WORD, TMid*>::const_iterator it = m_midlist.cbegin(); it != m_midlist.cend(); ++it)
		{
			TMid* pMid = it->second;
			pMid->ReleaseWeb(file);
		}
	}
	virtual void ReleaseWeb(BinaryStream& stream) const
	{
		const unsigned short nCount = m_midlist.size();
		stream << nCount;

		for(typename std::map<WORD, TMid*>::const_iterator it = m_midlist.cbegin(); it != m_midlist.cend(); ++it)
		{
			TMid* pMid = it->second;
			pMid->ReleaseWeb(stream);
		}
	}
	virtual void ReleaseWeb(boost::json::object& json, const char* name) const
	{
		boost::json::object child1;
		const unsigned short nCount = m_midlist.size();
		child1["Count"] = nCount;

		boost::json::array child2;
		for(typename std::map<WORD, TMid*>::const_iterator it = m_midlist.cbegin(); it != m_midlist.cend(); ++it)
		{
			TMid* pMid = it->second;
			boost::json::object child3;
			pMid->ReleaseWeb(child3);
			child2.push_back(child3);
		}
		child1["Mids"] = child2;
		json[name] = child1;
	}
	virtual void ReleaseWeb(pbf::writer& writer) const
	{
		writer.add_variant_uint32(m_midlist.size());
		for(typename std::map<WORD, TMid*>::const_iterator it = m_midlist.cbegin(); it != m_midlist.cend(); ++it)
		{
			TMid* pMid = it->second;
			pMid->ReleaseWeb(writer);
		}
	}
};