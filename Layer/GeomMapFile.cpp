#include "stdafx.h"
#include <io.h>
#include <stdio.h>

#include "GeomMapFile.h"

#include "../Geometry/Geom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGeomMapFile::CGeomMapFile()
{
	m_hFile = nullptr;
	m_hFileMap = nullptr;
	m_hFileMapView = nullptr;

	m_pFlash = nullptr;
	m_pGet = nullptr;
}

CGeomMapFile::~CGeomMapFile()
{
	if(m_hFileMapView!=nullptr)
	{
		UnmapViewOfFile(m_hFileMapView);
		m_hFileMapView = nullptr;
	}
	if(m_hFileMap!=nullptr)
	{
		CloseHandle(m_hFileMap);
		m_hFileMap = nullptr;
	}
	if(m_hFile!=nullptr)
	{
		CloseHandle(m_hFile);
		m_hFile = nullptr;
	}

	delete m_pFlash;
	m_pFlash = nullptr;
	delete m_pGet;
	m_pGet = nullptr;
}

bool CGeomMapFile::Open(CString strFile)
{
	if(_taccess(strFile, 00)==-1)
		return false;

	m_hFile = CreateFile(strFile, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);
	if(m_hFile==INVALID_HANDLE_VALUE)
	{
		m_hFile = nullptr;
		return false;
	}

	m_hFileMap = CreateFileMapping(m_hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
	if(m_hFileMap==INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = nullptr;
		m_hFileMap = nullptr;
		return false;
	}
	if(GetLastError()==ERROR_ALREADY_EXISTS)
	{
		CloseHandle(m_hFileMap);
		CloseHandle(m_hFile);
		m_hFile = nullptr;
		m_hFileMap = nullptr;
		return false;
	}

	// 映射至内存
	m_hFileMapView = MapViewOfFile(m_hFileMap, FILE_MAP_READ, 0, 0, 0);
	if(m_hFileMapView==nullptr)
	{
		const DWORD dwError = GetLastError();
		if(dwError==8)
		{
			DWORD maxBlock = 0;
			PBYTE p = nullptr;
			while(true)
			{
				MEMORY_BASIC_INFORMATION mbi;
				if(VirtualQuery(p, &mbi, sizeof(mbi))==0)
					break;
				p = (PBYTE)mbi.BaseAddress+mbi.RegionSize;
				if(mbi.AllocationBase!=nullptr)
					continue;
				if(mbi.RegionSize>maxBlock)
					maxBlock = (DWORD)mbi.RegionSize;
			}

			const int fileSize = ::GetFileSize(m_hFile, nullptr);
			CString str;
			str.Format(_T("当前进程中未使用的最大虚拟地址空间大小 %d k, 而你要加载的文件大小为: %d k, 文件太大无法加载！"), maxBlock/1000, fileSize/1000);
			AfxMessageBox(str);
		}

		CloseHandle(m_hFileMap);
		CloseHandle(m_hFile);
		m_hFile = nullptr;
		m_hFileMap = nullptr;
		m_hFileMapView = nullptr;

		return false;
	}

	return true;
}

void CGeomMapFile::Close()
{
	if(m_hFileMapView!=nullptr)
	{
		UnmapViewOfFile(m_hFileMapView);
		m_hFileMapView = nullptr;
	}
	if(m_hFileMap!=nullptr)
	{
		CloseHandle(m_hFileMap);
		m_hFileMap = nullptr;
	}
	if(m_hFile!=nullptr)
	{
		CloseHandle(m_hFile);
		m_hFile = nullptr;
	}

	delete m_pFlash;
	m_pFlash = nullptr;
	delete m_pGet;
	m_pGet = nullptr;
}

CGeomMapFile::GEOMHEAD* CGeomMapFile::GetGeomHeadByIndex(const WORD& wLayerIndex, const DWORD& dwGeomIndex) const
{
	if(m_hFileMapView==nullptr)
		return nullptr;

	LAYER* pLayer = (LAYER*)((BYTE*)m_hFileMapView);
	pLayer += wLayerIndex;

	//	{
	DWORD dwCode = (pLayer->wId<<16)|(pLayer->wId+42342);
	dwCode = dwCode^0X76864594;
	const DWORD dwGeomHeadOffset = pLayer->dwGeomHeadOffset^dwCode;
	//	}

	GEOMHEAD* pGeomHead = (GEOMHEAD*)((BYTE*)m_hFileMapView+dwGeomHeadOffset);
	pGeomHead += dwGeomIndex;

	return pGeomHead;
}

CGeomMapFile::GEOMHEAD* CGeomMapFile::GetGeomHeadByID(const WORD& wLayerIndex, const DWORD& dwGeomID) const
{
	if(m_hFileMapView==nullptr)
		return nullptr;

	LAYER* pLayer = (LAYER*)((BYTE*)m_hFileMapView);
	pLayer += wLayerIndex;

	DWORD dwCode = (pLayer->wId<<16)|(pLayer->wId+42342);
	dwCode = dwCode^0X76864594;
	const DWORD dwGeomCount = pLayer->dwGeomCount^dwCode;
	const DWORD dwGeomHeadOffset = pLayer->dwGeomHeadOffset^dwCode;

	GEOMHEAD* pGeomHead = (GEOMHEAD*)((BYTE*)m_hFileMapView+dwGeomHeadOffset);
	for(DWORD dwGeomIndex = 0; dwGeomIndex<dwGeomCount; dwGeomIndex++)
	{
		if(pGeomHead->dwId==dwGeomID)
		{
			return pGeomHead;
		}

		pGeomHead++;
	}

	return nullptr;
}

CGeomMapFile::GEOMHEAD* CGeomMapFile::GetGeomHeadByID(const DWORD& dwGeomID, WORD& layerCount) const
{
	if(m_hFileMapView==nullptr)
		return nullptr;

	LAYER* pLayer = (LAYER*)((BYTE*)m_hFileMapView);
	for(int index = 0; index<layerCount; index++)
	{
		DWORD dwCode = (pLayer->wId<<16)|(pLayer->wId+42342);
		dwCode = dwCode^0X76864594;
		const DWORD dwGeomCount = pLayer->dwGeomCount^dwCode;
		const DWORD dwGeomHeadOffset = pLayer->dwGeomHeadOffset^dwCode;

		GEOMHEAD* pGeomHead = (GEOMHEAD*)((BYTE*)m_hFileMapView+dwGeomHeadOffset);
		for(DWORD dwGeomIndex = 0; dwGeomIndex<dwGeomCount; dwGeomIndex++)
		{
			if(pGeomHead->dwId==dwGeomID)
			{
				return pGeomHead;
			}

			pGeomHead++;
		}
		pLayer++;
	}

	return nullptr;
}

CGeom* CGeomMapFile::GetGeom(CGeomMapFile::GEOMHEAD* pGeomHead) const
{
	if(m_hFile==nullptr||m_hFileMap==nullptr||m_hFileMapView==nullptr)
		return nullptr;
	if(pGeomHead==nullptr)
		return nullptr;
	const DWORD dwCode = (pGeomHead->dwId+1232*pGeomHead->bType)^0X66546675;
	const DWORD dwGeomOffset = (pGeomHead->dwGeomOffset^dwCode);

	BYTE* bytes = (BYTE*)m_hFileMapView+dwGeomOffset;
	CGeom* geom = CGeom::Apply(pGeomHead->bType);
	if(geom!=nullptr)
	{
		geom->m_Rect = pGeomHead->rect;
		geom->m_geoId = pGeomHead->dwId;
		geom->PackPC(nullptr, bytes);
	}

	return geom;
}

void CGeomMapFile::Export(const WORD& wLayerIndex, CObList& geomlist)
{
	if(m_hFileMapView==nullptr)
		return;

	LAYER* pLayer = (LAYER*)((BYTE*)m_hFileMapView);
	pLayer += wLayerIndex;

	DWORD dwCode = (pLayer->wId<<16)|(pLayer->wId+42342);
	dwCode = dwCode^0X76864594;
	const DWORD dwGeomCount = pLayer->dwGeomCount^dwCode;
	for(DWORD index = 0; index<dwGeomCount; index++)
	{
		GEOMHEAD* pGeomHead = GetGeomHeadByIndex(wLayerIndex, index);
		CGeom* pGeom = this->GetGeom(pGeomHead);
		if(pGeom!=nullptr)
		{
			geomlist.AddTail(pGeom);
		}
	}
	/*		else
	{
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it!=m_layertree.postend();++it)
	{
		CLayer* layer = *it;
	int wLayerIndex = m_pGridMapFile->GetLayerIndex(layer->m_wId);
	if(wLayerIndex != -1)
	{
	m_pGeomMapFile->Export(wLayerIndex,layer->m_geomlist);
	}
	}
	}*/
}
