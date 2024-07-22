#include "stdafx.h"

#include "Compound.h"

#include  <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CCompound, CCluster, 0)

CCompound::CCompound()
	: CCluster()
{
	m_bGroup = true;
	m_bClosed = true;
	m_bType = 20;
}
CCompound::~CCompound()
{
}

void CCompound::RecalCentroid()
{
	CCluster::RecalCentroid();

	CPath* pPath = CCluster::GetBiggest();
	if(pPath != nullptr)
	{
		m_centroid = pPath->m_centroid;
	}
}