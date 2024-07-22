#include "stdafx.h"
#include "Polying.h"

#include "Node.h"
#include "Edge.h"
#include "Topo.h"
#include "RegionTopoer.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Bezier.h"
#include "../Geometry/Global.h"
#include "../Geometry/Doughnut.h"

#include "../Style/FillAlone.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPolying::CPolying(CRegionTopoer& topology)
	:m_topology(topology)
{
	m_bType = 61;
}

CPolying::~CPolying()
{
	m_edgelist.RemoveAll();
}
void CPolying::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CPath::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		int count = m_edgelist.GetCount();
		ar << count;
		for(int index = 0; index < count; index++)
		{
			const DWORD dwId = m_edgelist.GetAt(index);
			ar << dwId;
		}
	}
	else
	{
		int count;
		ar >> count;
		for(int index = 0; index < count; index++)
		{
			DWORD dwId;
			ar >> dwId;
			m_edgelist.Add(dwId);
		}
	}
}
bool CPolying::IsEdgeInvolved(const CTopology& topology, const DWORD& dwEdgeId)
{
	for(int index = 0; index < m_edgelist.GetSize(); index++)
	{
		const int dwId = m_edgelist.GetAt(index);
		if((dwId >= 0 ? dwId : ~dwId) == dwEdgeId)
			return true;
	}
	return false;
}

void CPolying::ReleaseWeb(const CTopology& topology, pbf::writer& writer, std::map<DWORD, DWORD>& mapEdgeIdIndex)
{
	CPath::ReleaseWeb(writer);

	unsigned short count = m_edgelist.GetCount();
	writer.add_variant_uint16(count);
	for(int index = 0; index < count; index++)
	{
		const int dwId = m_edgelist.GetAt(index);
		const DWORD dwIndex = mapEdgeIdIndex[dwId > 0 ? dwId : ~dwId];
		writer.add_variant_sint32(dwId > 0 ? dwIndex : ~dwIndex);
	}
}

CPolying* CPolying::RemoveEdge(CRegionTopoer& topology, const DWORD& dwEdge)
{
	const int nEdge1 = m_edgelist.GetAt(0);
	const int nEdge2 = m_edgelist.GetAt(m_edgelist.GetSize() - 1);
	if((nEdge1 >= 0 ? nEdge1 : ~nEdge1) == dwEdge)
	{
		m_edgelist.RemoveAt(0);
		this->RecalRect();
	}
	else if((nEdge2 >= 0 ? nEdge2 : ~nEdge2) == dwEdge)
	{
		m_edgelist.RemoveAt(m_edgelist.GetSize() - 1);
		this->RecalRect();
	}
	else
	{
		for(int index = 1; index < m_edgelist.GetSize() - 1; index++)
		{
			const int nEdge = m_edgelist.GetAt(index);
			if((nEdge >= 0 ? nEdge : ~nEdge) == dwEdge)
			{
				CPolying* pExtra = new CPolying(topology);
				m_edgelist.RemoveAt(index);
				while(index < m_edgelist.GetSize())
				{
					pExtra->m_edgelist.Add(m_edgelist.GetAt(index));
					m_edgelist.RemoveAt(index);
				}
				pExtra->RecalRect();
				this->RecalRect();
				return pExtra;
			}
		}
	}
	return nullptr;
}

void CPolying::AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const
{
	const int nCount = m_edgelist.GetSize();
	for(int index = 0; index < nCount; index++)
	{
		const int dwId = m_edgelist.GetAt(index);
		CPoly* poly = m_topology.GetEdgePoly(dwId >= 0 ? dwId : ~dwId);
		poly->AddPath(path, matrix, dwId < 0);
	}
}

void CPolying::AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const
{
	const int nCount = m_edgelist.GetSize();
	for(int index = 0; index < nCount; index++)
	{
		const int dwId = m_edgelist.GetAt(index);
		CPoly* poly = m_topology.GetEdgePoly(dwId >= 0 ? dwId : ~dwId);
		poly->AddPath(path, viewinfo, tolerance, dwId < 0);
	}
}
void CPolying::RecalRect()
{
	m_Rect.SetRectEmpty();
	const int nCount = m_edgelist.GetSize();
	for(int index = 0; index < nCount; index++)
	{
		const int dwId = m_edgelist.GetAt(index);
		CPoly* pPoly = m_topology.GetEdgePoly(dwId >= 0 ? dwId : ~dwId);
		m_Rect = UnionRect(m_Rect, pPoly->GetRect());
	}

	m_Rect.NormalizeRect();
}
void CPolying::EdgeBred(DWORD dwEdgeId, DWORD dwBredId)
{
	const int nCount = m_edgelist.GetSize();
	for(int index = 0; index < nCount; index++)
	{
		const int nId = m_edgelist.GetAt(index);
		const DWORD dwId = nId >= 0 ? nId : ~nId;
		if(dwId == dwEdgeId)
		{
			if(nId >= 0)
			{
				m_edgelist.InsertAt(index + 1, dwBredId);
			}
			else
			{
				m_edgelist.InsertAt(index + 1, ~dwEdgeId);
			}
			break;
		}
	}
}
void CPolying::EdgeMerged(DWORD dwEdgeId1, DWORD dwEdgeId2)
{
	const int nCount = m_edgelist.GetSize();
	int nPrev = m_edgelist.GetAt(0);
	DWORD dwPrev = nPrev >= 0 ? nPrev : ~nPrev;
	if(nCount == 1 && dwPrev == dwEdgeId2)
	{
		m_edgelist.SetAt(0, dwEdgeId1);
	}
	for(int index = 1; index < nCount; index++)
	{
		const int nId = m_edgelist.GetAt(index);
		const DWORD dwId = nId >= 0 ? nId : ~nId;
		if(dwPrev == dwEdgeId1 && dwId == dwEdgeId2)
		{
			m_edgelist.RemoveAt(index);	
			break;
		}
		else if(dwPrev == dwEdgeId2 && dwId == dwEdgeId1)
		{
			m_edgelist.RemoveAt(index-1);
			break;
		}	
		nPrev = nId;
		dwPrev = dwId;
	}
}
CIngroup::CIngroup(CRegionTopoer& regiontopoer)
	:CCollection<CPolying>(), m_topology(regiontopoer)
{
	m_bType = 62;
}

CIngroup::~CIngroup()
{
}

void CIngroup::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CGeom::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		int count = m_geomlist.GetCount();
		ar << count;
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CPolying* pPolygon = m_geomlist.GetNext(pos);
			pPolygon->Serialize(ar, dwVersion);
		}
	}
	else
	{
		int count;
		ar >> count;
		for(int index = 0; index < count; index++)
		{
			CPolying* pPolygon = new CPolying(m_topology);
			pPolygon->Serialize(ar, dwVersion);
			m_geomlist.AddTail(pPolygon);
		}
	}
}
void CIngroup::RemoveEdge(CRegionTopoer& topology, const DWORD& dwEdge)
{
	POSITION pos1 = m_geomlist.GetTailPosition();
	POSITION pos2;
	while((pos2 = pos1) != nullptr)
	{
		CPolying* pPolying = m_geomlist.GetPrev(pos1);
		CPolying* pExtra = pPolying->RemoveEdge(topology, dwEdge);
		if(pExtra != nullptr)
		{
			m_geomlist.InsertAfter(pos2, pExtra);
			this->RecalRect();
		}
		else if(pPolying->m_edgelist.GetSize() == 0)
		{
			delete pPolying;
			m_geomlist.RemoveAt(pos2);
			this->RecalRect();
		}
	}
}

void CIngroup::ReleaseWeb(const CTopology& topology, pbf::writer& writer, std::map<DWORD, DWORD>& mapEdgeIdIndex)
{
	CGeom::ReleaseWeb(writer);

	unsigned short count = m_geomlist.GetCount();
	writer.add_variant_uint16(count);
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CPolying* pPolygon = m_geomlist.GetNext(pos);
		pPolygon->ReleaseWeb(topology, writer, mapEdgeIdIndex);
	}
}

void CIngroup::EdgeBred(DWORD dwEdgeId, DWORD dwBredId)
{
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CPolying* pPolying = m_geomlist.GetPrev(pos);
		pPolying->EdgeBred(dwEdgeId, dwBredId);
	}
}
void CIngroup::EdgeMerged(DWORD dwEdgeId1, DWORD dwEdgeId2)
{
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CPolying* pPolying = m_geomlist.GetPrev(pos);
		pPolying->EdgeMerged(dwEdgeId1, dwEdgeId2);
	}
}