#pragma once
#include <list>

#include "../Style/Library.h"
#include "../Layer/LayerTree.h"
#include "../Dataview/Datainfo.h"
#include "../Public/ODBCDatabase.h"

class CProjection;
class CLink;
class CGridMemFile;
class CGeomMemFile;
class __declspec(dllexport) CMapItem
{
public:
	class CLayerVestige : public CObject
	{
	public:
		WORD wLayerId;
		CMap<DWORD, DWORD&, DWORD, DWORD&>* lasttimeId;
		CDWordArray existentId;
		CMap<DWORD, DWORD&, DWORD, DWORD&>* currentId;
	public:
		CLayerVestige()
		{
			lasttimeId = new CMap<DWORD, DWORD&, DWORD, DWORD&>;
			currentId = new CMap<DWORD, DWORD&, DWORD, DWORD&>;
		}

		~CLayerVestige() override
		{
			if(lasttimeId != nullptr)
			{
				lasttimeId->RemoveAll();
				delete lasttimeId;
				lasttimeId = nullptr;
			}

			existentId.RemoveAll();

			if(currentId != nullptr)
			{
				currentId->RemoveAll();
				delete currentId;
				currentId = nullptr;
			}
		}
	};

public:
	CMapItem(void);
	virtual ~CMapItem(void);

public:
	CLibrary m_library;
	CLayerTree m_layertree;
	CDatainfo m_Datainfo;
	CProjection* m_pProjection;

public:
	DWORD m_dwMaxGeomId;
	WORD m_wMapId;

public:
	CLibrary m_POUlib;

	std::list<CLink*> m_linklist;
	CODBCDatabase m_database;

public:
	VARIANT m_varHead;

public:
	CGridMemFile* m_pGridMemFile;
	CGeomMemFile* m_pGeomMemFile;

public:
	BOOL Open(CString strFile);
	STDMETHODIMP GetData(unsigned int nZoom, CSize docOffset, CRect inRect, VARIANT* varData, CObList& vestigelist) const;
};