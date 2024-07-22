#include "stdafx.h"

#include "FilePoly.h"

#include "../Geometry/Poly.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFilePoly::CFilePoly()
{
}

CFilePoly::~CFilePoly()
{
}

CPoly* CFilePoly::GetPoly(DWORD dwIndex) const
{
	if(m_hFile==nullptr||m_hFileMap==nullptr||m_hFileMapView==nullptr)
		return nullptr;
	const DWORD nPolyCount = *((DWORD*)m_hFileMapView);
	DWORD* pOffset = (DWORD*)((BYTE*)m_hFileMapView+sizeof(DWORD));
	pOffset += dwIndex;

	BYTE* bytes = (BYTE*)m_hFileMapView+sizeof(DWORD)+nPolyCount*sizeof(DWORD)+*pOffset;
	CPoly* poly = new CPoly();
	if(poly!=nullptr)
	{
		poly->m_nAnchors = (unsigned short)*(unsigned short*)bytes;
		bytes += sizeof(unsigned short);

		poly->m_pPoints = new CPoint[poly->m_nAnchors];
		memcpy(poly->m_pPoints, bytes, poly->m_nAnchors*sizeof(CPoint));
	}

	return poly;
}
