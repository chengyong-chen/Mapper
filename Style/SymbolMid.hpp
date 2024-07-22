#pragma once

#include "Mid.h"
#include "../Framework/Gfx.h"
#include "../Pbf/writer.hpp"
#include <boost/json.hpp>

extern __declspec(dllimport) void ReverseOrder(WORD& x);
extern __declspec(dllimport) void ReverseOrder(DWORD& x);
extern __declspec(dllimport) void ReverseOrder(int& x);
extern __declspec(dllimport) void ReverseOrder(unsigned int& x);
extern __declspec(dllimport) void ReverseOrder(long& x);
extern __declspec(dllimport) void ReverseOrder(short& x);
extern __declspec(dllimport) void ReverseOrder(float& x);
extern __declspec(dllimport) double ReverseOrder(double x);
extern __declspec(dllimport) void ReverseOrder(CPoint* pPoints, unsigned int nPoints);


extern __declspec(dllimport) DWORD AFXAPI AfxGetCurrentArVersion();

template<class TSymbol>
class __declspec(dllexport) CSymbolMid : public CMid
{
	DECLARE_DYNCREATE_T(CSymbolMid, TSymbol)
	
public:
	CTypedPtrList<CObList, TSymbol*> m_Symbollist;

public:
	CSymbolMid()
	{
	}

	~CSymbolMid() override
	{
		Clear();
	}

public:
	virtual TSymbol* GetSymbol(WORD wId) const
	{
		POSITION pos = m_Symbollist.GetHeadPosition();
		while(pos!=nullptr)
		{
			TSymbol* pSymbol = m_Symbollist.GetNext(pos);
			if(pSymbol->m_wId==wId)
				return pSymbol;
		}

		return nullptr;
	}

	virtual void Clear()
	{
		POSITION pos = m_Symbollist.GetHeadPosition();
		while(pos!=nullptr)
		{
			const TSymbol* pSymbol = m_Symbollist.GetNext(pos);
			delete pSymbol;
		}
		m_Symbollist.RemoveAll();
	}

	BOOL Load(CString strFile) override
	{
		if(_taccess(strFile, 00)==-1)
			return FALSE;

		CFile file;
		if(file.Open(strFile, CFile::modeRead|CFile::shareDenyWrite)==TRUE)
		{
			CArchive ar(&file, CArchive::load|CArchive::bNoFlushOnDelete);
			DWORD dwVersion;
			ar>> dwVersion;

			int nCount;
			ar>>nCount;
			for(int index = 0; index<nCount; index++)
			{
				TSymbol* pSymbol = new TSymbol();
				pSymbol->Serialize(ar, dwVersion);
				m_Symbollist.AddTail(pSymbol);
			}

			ar.Close();
			file.Close();

			m_strFile = strFile;
			return TRUE;
		}
		else
			return FALSE;
	}

public:
	void CopyTo(CMid* pMid) const override
	{
		CMid::CopyTo(pMid);

		if(pMid->IsKindOf(RUNTIME_CLASS(CSymbolMid))==TRUE)
		{
			POSITION pos = m_Symbollist.GetHeadPosition();
			while(pos!=nullptr)
			{
				TSymbol* pSymbol = m_Symbollist.GetNext(pos);
				pSymbol = (TSymbol*)pSymbol->Clone();
				if(pSymbol!=nullptr)
				{
					((CSymbolMid<TSymbol>*)pMid)->m_Symbollist.AddTail(pSymbol);
				}
			}
		}
	}

public:
	void ReleaseCE(CArchive& ar) const override
	{
		CMid::ReleaseCE(ar);

		if(ar.IsStoring())
		{
		}
		else
		{
		}
	}

	void ReleaseDCOM(CArchive& ar) override
	{
		CMid::ReleaseDCOM(ar);

		if(ar.IsStoring())
		{
		}
		else
		{
		}
	}

	void ReleaseWeb(CFile& file) const override
	{
		CMid::ReleaseWeb(file);

		unsigned short nCount = m_Symbollist.GetCount();
		ReverseOrder(nCount);
		file.Write(&nCount, sizeof(unsigned short));

		POSITION pos = m_Symbollist.GetHeadPosition();
		while(pos!=nullptr)
		{
			TSymbol* pSymbol = m_Symbollist.GetNext(pos);
			BYTE bType = pSymbol==nullptr ? -1 : pSymbol->Gettype();
			file.Write(&bType, sizeof(BYTE));
			pSymbol->ReleaseWeb(file);
		}
	}

	void ReleaseWeb(BinaryStream& stream) const override
	{
		CMid::ReleaseWeb(stream);
		const unsigned short nCount = m_Symbollist.GetCount();
		stream<<nCount;

		POSITION pos = m_Symbollist.GetHeadPosition();
		while(pos!=nullptr)
		{
			TSymbol* pSymbol = m_Symbollist.GetNext(pos);
			const BYTE bType = pSymbol==nullptr ? -1 : pSymbol->Gettype();
			stream<<bType;
			pSymbol->ReleaseWeb(stream);
		}
	}

	void ReleaseWeb(boost::json::object& json) const override
	{
		CMid::ReleaseWeb(json);
		const unsigned short nCount = m_Symbollist.GetCount();
		json["Count"] = nCount;

		boost::json::array child1;
		POSITION pos = m_Symbollist.GetHeadPosition();
		while(pos != nullptr)
		{
			TSymbol* pSymbol = m_Symbollist.GetNext(pos);
			boost::json::object child2;
			const BYTE bType = pSymbol == nullptr ? -1 : pSymbol->Gettype();
			child2["Type"] = bType;
			pSymbol->ReleaseWeb(child2);
			child1.push_back(child2);
		}
		json["Symbols"] = child1;
	}
	void ReleaseWeb(pbf::writer& writer) const override
	{
		CMid::ReleaseWeb(writer);

		const unsigned short nCount=m_Symbollist.GetCount();		
		writer.add_variant_uint16(nCount);
				
		POSITION pos=m_Symbollist.GetHeadPosition();
		while(pos != nullptr)
		{
			TSymbol* pSymbol=m_Symbollist.GetNext(pos);
			const BYTE bType=pSymbol == nullptr ? -1 : pSymbol->Gettype();			
			writer.add_byte(bType);
			pSymbol->ReleaseWeb(writer);
		}
	}
};

IMPLEMENT_DYNCREATE_T(CSymbolMid, TSymbol, CMid)
