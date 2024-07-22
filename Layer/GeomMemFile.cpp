#include "stdafx.h"
#include <io.h>
#include <stdio.h>

#include "GeomMemFile.h"

#include "../Geometry/Geom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGeomMemFile::CGeomMemFile()
{
	m_pData = nullptr;
}

CGeomMemFile::~CGeomMemFile()
{
	if(m_pData!=nullptr)
	{
		free(m_pData);
		m_pData = nullptr;
	}
}

bool CGeomMemFile::Open(CString strFile)
{
	if(_taccess(strFile, 00)==-1)
		return false;

	CFile file;
	CFileException fe;
	if(file.Open(strFile, CFile::modeRead|CFile::shareDenyWrite, &fe)!=0)
	{
		const int length = file.GetLength();
		m_pData = (BYTE*)malloc(length);
		file.Read(m_pData, length);

		file.Close();
		return true;
	}
	else
		return false;
}

void CGeomMemFile::Close()
{
	delete[] m_pData;
	m_pData = nullptr;
}

CGeomMemFile::GEOMHEAD* CGeomMemFile::GetGeomHeadByIndex(const WORD& wLayerIndex, const DWORD& dwGeomIndex) const
{
	if(m_pData==nullptr)
		return nullptr;

	LAYER* pLayer = (LAYER*)(m_pData);
	pLayer += wLayerIndex;

	//	{
	DWORD dwCode = (pLayer->wId<<16)|(pLayer->wId+42342);
	dwCode = dwCode^0X76864594;

	const DWORD dwGeomHeadOffset = pLayer->dwGeomHeadOffset^dwCode;
	//	}

	GEOMHEAD* pGeomHead = (GEOMHEAD*)(m_pData+dwGeomHeadOffset);
	pGeomHead += dwGeomIndex;

	return pGeomHead;
}

CGeomMemFile::GEOMHEAD* CGeomMemFile::GetGeomHeadByID(const WORD& wLayerIndex, const DWORD& dwGeomID) const
{
	if(m_pData==nullptr)
		return nullptr;

	LAYER* pLayer = (LAYER*)(m_pData);
	pLayer += wLayerIndex;

	DWORD dwCode = (pLayer->wId<<16)|(pLayer->wId+42342);
	dwCode = dwCode^0X76864594;

	const DWORD dwGeomCount = pLayer->dwGeomCount^dwCode;
	const DWORD dwGeomHeadOffset = pLayer->dwGeomHeadOffset^dwCode;

	GEOMHEAD* pGeomHead = (GEOMHEAD*)(m_pData+dwGeomHeadOffset);
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

CGeom* CGeomMemFile::GetGeom(CGeomMemFile::GEOMHEAD* pGeomHead) const
{
	if(m_pData==nullptr)
		return nullptr;
	if(pGeomHead==nullptr)
		return nullptr;
	const DWORD dwCode = (pGeomHead->dwId+1232*pGeomHead->bType)^0X66546675;
	const DWORD dwGeomOffset = (pGeomHead->dwGeomOffset^dwCode);

	BYTE* bytes = m_pData+dwGeomOffset;
	CGeom* geom = CGeom::Apply(pGeomHead->bType);
	if(geom!=nullptr)
	{
		geom->m_Rect = pGeomHead->rect;
		geom->m_geoId = pGeomHead->dwId;
		geom->PackPC(nullptr, bytes);
	}

	return geom;
}
