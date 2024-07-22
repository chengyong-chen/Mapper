#include "stdafx.h"
#include <io.h>
#include <stdio.h>

#include "GridMapFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGridMapFile::CGridMapFile()
{
	m_hFile = nullptr;
	m_hFileMap = nullptr;
	m_hFileMapView = nullptr;
}

CGridMapFile::~CGridMapFile()
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
	m_LayerIdMap.RemoveAll();
}

bool CGridMapFile::Open(CString strFile)
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

	// ”≥…‰÷¡ƒ⁄¥Ê
	m_hFileMapView = MapViewOfFile(m_hFileMap, FILE_MAP_READ, 0, 0, 0);
	if(m_hFileMapView==nullptr)
	{
		CloseHandle(m_hFileMap);
		CloseHandle(m_hFile);
		m_hFile = nullptr;
		m_hFileMap = nullptr;
		m_hFileMapView = nullptr;
		return false;
	}
	CopyMemory(&m_Head, (BYTE*)m_hFileMapView, sizeof(CGridMapFile::HEAD));

	CGridMapFile::LAYER* pLayer = (CGridMapFile::LAYER*)((BYTE*)m_hFileMapView+sizeof(CGridMapFile::HEAD));
	for(WORD wLayerIndex = 0; wLayerIndex<m_Head.wLayerCount; wLayerIndex++)
	{
		m_LayerIdMap.SetAt(pLayer->wId, wLayerIndex);
		pLayer++;
	}

	return true;
}

void CGridMapFile::Close()
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
	m_LayerIdMap.RemoveAll();
}

int CGridMapFile::GetLayerIndex(WORD wLayerID) const
{
	WORD wLayerIndex = 0;
	if(m_LayerIdMap.Lookup(wLayerID, wLayerIndex)==TRUE)
		return wLayerIndex;
	else
		return -1;
}

WORD CGridMapFile::GetLayerCount() const
{
	return m_Head.wLayerCount;
}

CGridMapFile::GRID* CGridMapFile::GetGrid(const WORD& wLayerIndex, const short& yGrid, const short& xGrid) const
{
	if(m_hFileMapView==nullptr)
		return nullptr;

	if(xGrid < 0||xGrid > m_Head.xCount-1)
		return nullptr;
	if(yGrid < 0||yGrid > m_Head.yCount-1)
		return nullptr;

	CGridMapFile::LAYER* pLayer = (LAYER*)((BYTE*)m_hFileMapView+sizeof(CGridMapFile::HEAD));
	pLayer += wLayerIndex;

	CGridMapFile::GRID* pGrid = (CGridMapFile::GRID*)((BYTE*)m_hFileMapView+pLayer->dwGridOffset);

	pGrid += (yGrid*m_Head.xCount+xGrid);
	return pGrid;
}

DWORD* CGridMapFile::GetGridGeomList(CGridMapFile::GRID* pLayerGrid) const
{
	if(m_hFileMapView==nullptr)
		return nullptr;
	if(pLayerGrid==nullptr)
		return nullptr;

	return (DWORD*)((BYTE*)m_hFileMapView+pLayerGrid->dwOffset);
}
