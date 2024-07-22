#include "stdafx.h"
#include "Polygon.h"

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
#include "../Style/ColorSpot.h"
#include "../Pbf/writer.hpp"

#include <queue>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPolygon::CPolygon(CRegionTopoer& topology)
	:CPolying(topology)
{
	m_bType = 51;
	m_pFill = new CFillAlone(new CColorSpot(200+rand()%55, 200+rand()%55, 200+rand()%55, 255));
}

CGeom* CPolygon::Apply(BYTE index, CRegionTopoer& regiontopoer)
{
	CGeom* pPolygon = nullptr;
	switch(index)
	{
		case 0:
			break;
		case 1:
			pPolygon = new CPolygon(regiontopoer);
			break;
		case 2:
			pPolygon = new CGonroup(regiontopoer);
			break;
	}

	return pPolygon;
}
unsigned long long CPolygon::Distance2(const CPoint& point) const
{
	unsigned long long dismin = MAXUINT64;
	for(int index1 = 0; index1<m_edgelist.GetSize(); index1++)
	{
		const int dwId = m_edgelist.GetAt(index1);
		const CPoly* pPoly = m_topology.GetEdgePoly(dwId>=0 ? dwId : ~dwId);
		dismin = min(dismin, pPoly->Distance2(point));
	}
	return dismin;
}
bool CPolygon::PickIn(const CPoint& point) const
{
	int odd = 0;
	unsigned long long dismin = MAXUINT64;
	for(int index1 = 0; index1<m_edgelist.GetSize(); index1++)
	{
		const int dwId = m_edgelist.GetAt(index1);
		const CPoly* pPoly = m_topology.GetEdgePoly(dwId>=0 ? dwId : ~dwId);
		odd += CPoly::PointInPolygon(point, pPoly->m_pPoints, pPoly->m_nAnchors) ? 1 : 0;
	}
	return odd%2==1;
}
void CPolygon::RecalCentroid()
{
	CPath::RecalCentroid();
}
void CPolygon::ReleaseWeb(const CTopology& topology, pbf::writer& writer, std::map<DWORD, DWORD>& mapEdgeIdIndex)
{
	CPolying::ReleaseWeb(topology, writer, mapEdgeIdIndex);

	const CPoint geogroid = this->GetGeogroid();
	writer.add_variant_sint32(geogroid.x);
	writer.add_variant_sint32(geogroid.y);

	const CPoint labeloid = this->GetLabeloid();
	writer.add_variant_sint32(labeloid.x);
	writer.add_variant_sint32(labeloid.y);
}

void CPolygon::RemoveEdge(const DWORD& dwEdge)
{
	for(int index1 = 0; index1<m_edgelist.GetSize(); index1++)
	{
		const int nEdge = m_edgelist.GetAt(index1);
		if((nEdge>=0 ? nEdge : ~nEdge)==dwEdge)
		{
			m_edgelist.RemoveAt(index1);
			for(int index2 = 0; index2<index1; index2++)
			{
				m_edgelist.Add(m_edgelist.GetAt(0));
				m_edgelist.RemoveAt(0);
			}
			this->RecalRect();
			break;
		}
	}
}
//CPoly* CPolygon::CreateGeom(const CTopology& topology) const
//{
//	bool bPoly = TRUE;
//	for(int index = 0; index < m_edgelist.GetSize(); index++)
//	{
//		const int dwId = m_edgelist.GetAt(index);
//		const CPoly* pPoly = topology.GetEdgePoly(dwId >= 0 ? dwId : ~dwId);
//
//		if(pPoly->m_bType == 2)
//			bPoly = false;
//	}
//
//	CPoly* polygon;
//	if(bPoly == true)
//		polygon = new CPoly;
//	else
//		polygon = new CBezier;
//
//	for(int index = 0; index < m_edgelist.GetSize(); index++)
//	{
//		const int dwId = m_edgelist.GetAt(index);
//		CEdge* pEdge = topology.GetEdge(dwId >= 0 ? dwId : ~dwId);
//		CPoly* pPoly = topology.GetEdgePoly(dwId >= 0 ? dwId : ~dwId);
//		pPoly = (CPoly*)pPoly->Clone();
//
//		const CNode* pFromNode = topology.GetNode(pEdge->m_dwFromNode);
//		const CNode* pToNode = topology.GetNode(pEdge->m_dwToNode);
//		const CPoint start = pFromNode->GetPoint();
//		const CPoint end = pToNode->GetPoint();
//
//		if(dwId >= 0)
//		{
//			pPoly->AddAnchor(start, false);
//			pPoly->AddAnchor(end, true);
//		}
//		else
//		{
//			pPoly->Reverse();
//			pPoly->AddAnchor(start, true);
//			pPoly->AddAnchor(end, false);
//		}
//		pPoly->RecalRect();
//
//		polygon->Add(pPoly);
//		delete pPoly;
//		pPoly = nullptr;
//	}
//
//	polygon->m_bClosed = TRUE;
//	delete polygon->m_pFill;
//
//	CFillAlone* pFillAlone = new CFillAlone();
//	pFillAlone->m_pColor = new CColorSpot(m_color.GetR(), m_color.GetG(), m_color.GetB(), 255);
//	polygon->m_pFill = pFillAlone;
//	return polygon;
//}
void CPolygon::AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const
{
	CPolying::AddPath(path, matrix);

	path.CloseFigure();
}

void CPolygon::AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const
{
	path.StartFigure();
	CPolying::AddPath(path, viewinfo, tolerance);
	path.CloseAllFigures();
}
double CPolygon::GetArea() const
{
	double fArea = 0;
	const int nCount = m_edgelist.GetSize();
	for(int index = 0; index<nCount; index++)
	{
		const int dwId = m_edgelist.GetAt(index);
		CEdge* edge = m_topology.GetEdge(dwId>=0 ? dwId : ~dwId);
		const CPoly* pPoly = m_topology.GetEdgePoly(dwId>=0 ? dwId : ~dwId);

		CPoint fpoint;
		CPoint tpoint;
		if(dwId>=0)
		{
			fpoint = pPoly->GetAnchor(pPoly->m_nAnchors);
			tpoint = pPoly->GetAnchor(1);
		}
		else
		{
			fpoint = pPoly->GetAnchor(1);
			tpoint = pPoly->GetAnchor(pPoly->m_nAnchors);
		}

		double s = (double)(tpoint.x-fpoint.x)*(double)(tpoint.y+fpoint.y)/2.f;
		for(unsigned int nAnchor = 1; nAnchor<pPoly->m_nAnchors; nAnchor++)
		{
			fpoint = pPoly->GetAnchor(nAnchor);
			tpoint = pPoly->GetAnchor(nAnchor+1);
			s += (double)(tpoint.x-fpoint.x)*(double)(tpoint.y+fpoint.y)/2.f;
		}

		if(dwId>=0)
		{
			fpoint = pPoly->GetAnchor(pPoly->m_nAnchors);
			tpoint = pPoly->GetAnchor(1);
		}
		else
		{
			fpoint = pPoly->GetAnchor(1);
			tpoint = pPoly->GetAnchor(pPoly->m_nAnchors);
			s *= -1;
		}
		s += (double)(tpoint.x-fpoint.x)*(double)(tpoint.y+fpoint.y)/2.f;
		fArea += s;
	}
	return -fArea;
}

CGonroup::CGonroup(CRegionTopoer& regiontopoer)
	:CCollection<CPolygon>(), m_topology(regiontopoer)
{
	m_bType = 52;
	m_pFill = new CFillAlone(new CColorSpot(200+rand()%55, 200+rand()%55, 200+rand()%55, 255));
	m_bClosed = true;
}

CGonroup::~CGonroup()
{
}

void CGonroup::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CGeom::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		int count = m_geomlist.GetCount();
		ar<<count;
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CPolygon* pPolygon = m_geomlist.GetNext(pos);
			pPolygon->Serialize(ar, dwVersion);
		}
	}
	else
	{
		int count;
		ar>>count;
		for(int index = 0; index<count; index++)
		{
			CPolygon* pPolygon = new CPolygon(m_topology);
			pPolygon->Serialize(ar, dwVersion);
			m_geomlist.AddTail(pPolygon);
		}
	}
}
Gdiplus::PointF CGonroup::GetTagAnchor(const CViewinfo& viewinfo, HALIGN& hAlign, VALIGN& vAlign) const
{
	CGeom* pGeom = m_geomlist.GetHead();
	return pGeom->GetTagAnchor(viewinfo, hAlign, vAlign);
}
void CGonroup::DrawCentroid(CDC* pDC, const CViewinfo& viewinfo, const bool& labeloid, const bool& geogroid) const
{
	CGeom* pGeom = m_geomlist.GetHead();
	pGeom->DrawCentroid(pDC, viewinfo, labeloid, geogroid);
}
CPolygon* CGonroup::FindInvolvedEdge(const CTopology& topology, DWORD dwEdgeId, CPolygon* exclude)
{
	POSITION pos1, pos2;
	pos1 = m_geomlist.GetTailPosition();
	while((pos2 = pos1)!=nullptr)
	{
		CPolygon* pPolygon = m_geomlist.GetPrev(pos1);
		if(pPolygon==exclude)
			continue;
		if(pPolygon->IsEdgeInvolved(topology, dwEdgeId)==true)
		{
			return pPolygon;
		}
	}
	return nullptr;
}
void CGonroup::ReleaseWeb(const CTopology& topology, pbf::writer& writer, std::map<DWORD, DWORD>& mapEdgeIdIndex)
{
	CGeom::ReleaseWeb(writer);

	unsigned short count = m_geomlist.GetCount();
	writer.add_variant_uint16(count);
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPolygon* pPolygon = m_geomlist.GetNext(pos);
		pPolygon->ReleaseWeb(topology, writer, mapEdgeIdIndex);
	}

	const CPoint geogroid = this->GetGeogroid();
	writer.add_variant_sint32(geogroid.x);
	writer.add_variant_sint32(geogroid.y);

	const CPoint labeloid = this->GetLabeloid();
	writer.add_variant_sint32(labeloid.x);
	writer.add_variant_sint32(labeloid.y);
}

void CGonroup::EdgeBred(DWORD dwEdgeId, DWORD dwBredId)
{
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos!=nullptr)
	{
		CPolygon* pPolygon = m_geomlist.GetPrev(pos);
		pPolygon->EdgeBred(dwEdgeId, dwBredId);
	}
}
void CGonroup::EdgeMerged(DWORD dwEdgeId1, DWORD dwEdgeId2)
{
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos!=nullptr)
	{
		CPolygon* pPolygon = m_geomlist.GetPrev(pos);
		pPolygon->EdgeMerged(dwEdgeId1, dwEdgeId2);
	}
}