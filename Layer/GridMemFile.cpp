#include "stdafx.h"
#include <io.h>
#include <stdio.h>

#include "GridMemFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGridMemFile::CGridMemFile()
{
	m_pData = nullptr;
}

CGridMemFile::~CGridMemFile()
{
	if(m_pData!=nullptr)
	{
		free(m_pData);
		m_pData = nullptr;
	}
	m_LayerIdMap.RemoveAll();
}

bool CGridMemFile::Open(CString strFile)
{
	if(_taccess(strFile, 00)==-1)
		return false;

	CFile file;
	CFileException fe;
	if(file.Open(strFile, CFile::modeRead|CFile::shareDenyWrite, &fe)!=0)
	{
		if(file.GetLength()>sizeof(CGridMemFile::HEAD))
		{
			file.Read(&m_Head, sizeof(CGridMemFile::HEAD));

			const int length = file.GetLength();
			m_pData = (BYTE*)malloc(length-sizeof(CGridMemFile::HEAD));
			file.Read(m_pData, length-sizeof(CGridMemFile::HEAD));

			CGridMemFile::LAYER* pLayer = (CGridMemFile::LAYER*)m_pData;
			for(WORD wLayerIndex = 0; wLayerIndex<m_Head.wLayerCount; wLayerIndex++)
			{
				m_LayerIdMap.SetAt(pLayer->wId, wLayerIndex);
				pLayer++;
			}

			file.Close();
			return true;
		}
		else
		{
			file.Close();
			return false;
		}
	}
	else
	{
		return false;
	}
}

void CGridMemFile::Close()
{
	delete[] m_pData;
	m_pData = nullptr;

	m_LayerIdMap.RemoveAll();
}

int CGridMemFile::GetLayerIndex(WORD wLayerID) const
{
	WORD wLayerIndex = 0;
	if(m_LayerIdMap.Lookup(wLayerID, wLayerIndex)==TRUE)
		return wLayerIndex;
	else
		return -1;
}

CGridMemFile::GRID* CGridMemFile::GetGrid(const WORD& wLayerIndex, const short& yGrid, const short& xGrid) const
{
	if(m_pData==nullptr)
		return nullptr;

	if(xGrid < 0||xGrid > m_Head.xCount-1)
		return nullptr;
	if(yGrid < 0||yGrid > m_Head.yCount-1)
		return nullptr;

	CGridMemFile::LAYER* pLayer = (LAYER*)m_pData;
	pLayer += wLayerIndex;

	CGridMemFile::GRID* pGrid = (CGridMemFile::GRID*)(m_pData+pLayer->dwGridOffset-sizeof(CGridMemFile::HEAD));

	pGrid += (yGrid*m_Head.xCount+xGrid);
	return pGrid;
}

DWORD* CGridMemFile::GetGridGeomList(CGridMemFile::GRID* pLayerGrid) const
{
	if(m_pData==nullptr)
		return nullptr;
	if(pLayerGrid==nullptr)
		return nullptr;

	return (DWORD*)(m_pData+pLayerGrid->dwOffset-sizeof(CGridMemFile::HEAD));
}
