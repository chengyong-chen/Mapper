#include "stdafx.h"

#include "../Style/Line.h"
#include "../Layer/Layer.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Pbf/writer.hpp"
#include "Edge.h"
#include "Node.h"
#include "Topo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEdge::CEdge()
{
	m_dwId = -1;
	m_dwFromNode = -1;
	m_dwToNode = -1;

	m_wLayer = 0;
	m_dwPoly = 0;

	m_pPoly = nullptr;

	m_bActive = false;
	m_bModified = false;
}

CEdge::~CEdge()
{
	delete m_pPoly;
}

CEdge* CEdge::Clone()
{
	CEdge* pEdge = new CEdge();

	pEdge->m_dwId = m_dwId;
	pEdge->m_dwFromNode = m_dwFromNode;
	pEdge->m_dwToNode = m_dwToNode;

	pEdge->m_wLayer = m_wLayer;
	pEdge->m_dwPoly = m_dwPoly;
	pEdge->m_Rect = m_Rect;

	if(m_pPoly!=nullptr)
	{
		pEdge->m_pPoly = (CPoly*)m_pPoly->Clone();
	}

	return pEdge;
}
CPoly* CEdge::CreatePoly(const CTopology* pTopology, bool direction) const
{
	CPoly* poly=pTopology->GetEdgePoly(m_dwId);
	poly=(CPoly*)poly->Clone();

	CNode* pFromNode=pTopology->GetNode(m_dwFromNode);
	CNode* pToNode=pTopology->GetNode(m_dwToNode);
	const CPoint start=pFromNode->GetPoint();
	const CPoint end=pToNode->GetPoint();

	if(direction == true)
	{
		poly->AddAnchor(start, false);
		poly->AddAnchor(end, true);
	}
	else
	{
		poly->Reverse();
		poly->AddAnchor(start, true);
		poly->AddAnchor(end, false);
	}
	poly->RecalRect();
	return poly;
}

void CEdge::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar<<m_dwId;
		ar<<m_Rect;

		ar<<m_dwFromNode;
		ar<<m_dwToNode;

		ar<<m_wLayer;
		ar<<m_dwPoly;
	}
	else
	{
		ar>>m_dwId;
		ar>>m_Rect;

		ar>>m_dwFromNode;
		ar>>m_dwToNode;

		ar>>m_wLayer;
		ar>>m_dwPoly;
	}
}
void CEdge::ReleaseWeb(const CTopology& topology, pbf::writer& writer)
{
	CPoly* poly = topology.GetEdgePoly(m_dwId);

	writer.add_byte(poly->Gettype());
	poly->ReleasePoints(writer);
}
void CEdge::ExportPC(std::map<DWORD, DWORD>& mapIdtoIndexNode, std::map<DWORD, DWORD>& mapIdtoIndexPoly, CFile& file)
{
	const DWORD headIndex = mapIdtoIndexNode.find(m_dwFromNode)!=mapIdtoIndexNode.end() ? mapIdtoIndexNode[m_dwFromNode] : 0XFFFFFFFF;
	const DWORD tailIndex = mapIdtoIndexNode.find(m_dwToNode)!=mapIdtoIndexNode.end() ? mapIdtoIndexNode[m_dwToNode] : 0XFFFFFFFF;
	const DWORD polyIndex = mapIdtoIndexPoly.find(m_dwPoly)!=mapIdtoIndexPoly.end() ? mapIdtoIndexPoly[m_dwPoly] : 0XFFFFFFFF;

	file.Write(&m_Rect, sizeof(CRect));
	file.Write(&headIndex, sizeof(DWORD));
	file.Write(&tailIndex, sizeof(DWORD));
	file.Write(&polyIndex, sizeof(DWORD));
}

void CEdge::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLine& line, CRect inRect, const CTopology& topology)
{
	CRect rect = m_Rect;
	if(rect.IntersectRect(rect, inRect) == TRUE)
	{
		CPoly* poly = topology.GetEdgePoly(m_dwId);
		if(poly != nullptr)
		{
			CLine* pOld = poly->m_pLine;
			poly->m_pLine = (CLine*)&line;
			poly->Draw(g, viewinfo, 1.f);
			poly->m_pLine = pOld;
		}
	}
	else if(rect.IsRectEmpty() == false)
		return;
	else if(inRect.PtInRect(m_Rect.TopLeft()) || inRect.PtInRect(m_Rect.BottomRight()))
	{
		CPoly* poly = topology.GetEdgePoly(m_dwId);
		if(poly != nullptr)
		{
			CLine* pOld = poly->m_pLine;
			poly->m_pLine = (CLine*)&line;
			poly->Draw(g, viewinfo, 1.f);
			poly->m_pLine = pOld;
		}
	}
}

void CEdge::DrawActive(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CTopology& topology, float width)
{
	CPoly* poly = topology.GetEdgePoly(m_dwId);
	if(poly!=nullptr)
	{
		CLine line;
		line.m_nWidth = width*10;
		line.m_pColor = new CColorSpot(255, 0, 0, 255);
		CLine* pOld = poly->m_pLine;
		poly->m_pLine = &line;
		poly->Draw(g, viewinfo, 1.f);
		poly->m_pLine = pOld;
	}
}

CPoint CEdge::GetStartPoint(const CTopology& topology) const
{
	CPoly* poly = topology.GetEdgePoly(m_dwId);
	if(poly!=nullptr)
		return poly->GetAnchor(1);
	else
		return CPoint(0, 0);
}

CPoint CEdge::GetEndPoint(const CTopology& topology) const
{
	CPoly* poly = topology.GetEdgePoly(m_dwId);
	if(poly!=nullptr)
		return poly->GetAnchor(poly->m_nAnchors);
	else
		return CPoint(0, 0);
}

bool CEdge::Pick(const CViewinfo& viewinfo, const CPoint& docPoint, int nRadius, const CTopology& topology) const
{
	CRect rect = m_Rect;
	rect.InflateRect(nRadius, nRadius);
	if(rect.PtInRect(docPoint)==FALSE)
		return false;

	CPoly* pPoly = topology.GetEdgePoly(m_dwId);
	if(pPoly!=nullptr)
	{
		return pPoly->PickOn(docPoint, nRadius);
	}
	return false;
}

bool CEdge::Pick(CPoly& docPolygon, const CTopology& topology) const
{
	CRect rect = m_Rect;
	if(rect.IntersectRect(rect, docPolygon.m_Rect)==FALSE)
		return false;

	CPoly* poly = topology.GetEdgePoly(m_dwId);
	if(poly!=nullptr)
	{
		return poly->PickIn(docPolygon);
	}
	else
	{
		return false;
	}
}

void CEdge::Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const CTopology& topology) const
{
	if(pWnd==nullptr)
		return;

	CPoly* pPoly = topology.GetEdgePoly(m_dwId);
	if(pPoly!=nullptr)
	{
		pPoly->Invalidate(pWnd, viewinfo, 4);
	}
}
