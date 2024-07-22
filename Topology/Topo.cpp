#include "stdafx.h"
#include "Topo.h"

#include "Node.h"
#include "Edge.h"
#include "StyleDlg.h"
#include "RoadEdge.h"

#include "Topoer.h"
#include "RegionTopoer.h"
#include "RoadTopoer.h"
#include "RoadTopodb.h"
#include "Layon.h"
#include "../DataView/viewinfo.h"
#include "../Style/Line.h"
#include "../Geometry/Global.h"
#include "../Geometry/Poly.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//IMPLEMENT_SERIAL(CTopology, CObject, 0)

CTopology::CTopology()
	: m_colorNode(255, 0, 0, 255), m_colorEdge(0, 255, 255, 255)
{
	m_strName = _T("fgdsg");
	m_bVisible = TRUE;

	m_dwMaxEdgeId = 0;
	m_dwMaxNodeId = 0;

	m_pActivateNode = nullptr;

	m_bModifyFlag = false;
}

CTopology::~CTopology()
{
	for(CNodeMap::iterator it = m_mapIdNode.begin(); it != m_mapIdNode.end(); it++)
	{
		const CNode* pNode = (CNode*)it->second;
		delete pNode;
	}
	m_mapIdNode.clear();

	for(CEdgeMap::iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); it++)
	{
		const CEdge* pEdge = (CEdge*)it->second;
		delete pEdge;
	}
	m_mapIdEdge.clear();

	m_dwMaxEdgeId = 0;
	m_dwMaxNodeId = 0;

	m_pActivateNode = 0;
}

bool CTopology::IsPolyExisting(const CPoly* pPoly) const
{
	for(CEdgeMap::const_iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); it++)
	{
		CEdge* pEdge = (CEdge*)it->second;
		if(pEdge->m_dwPoly == pPoly->m_geoId)
			return true;
	}

	return false;
}
CNode* CTopology::ApplyNode(const CPoint& point) const
{
	CNode* pNode = ApplyNode();
	pNode->m_Point = point;
	return pNode;
}

CNode* CTopology::GetNode(const DWORD& dwNodeId) const
{
	if(m_mapIdNode.find(dwNodeId) != m_mapIdNode.end())
		return m_mapIdNode.at(dwNodeId);
	else
		return nullptr;
}

CEdge* CTopology::GetEdge(const DWORD& dwEdgeId) const
{
	if(m_mapIdEdge.find(dwEdgeId) != m_mapIdEdge.end())
		return m_mapIdEdge.at(dwEdgeId);
	else
		return nullptr;
}
CNode* CTopology::GetNode(const CPoint& point) const
{
	for(CNodeMap::const_iterator it = m_mapIdNode.begin(); it != m_mapIdNode.end(); it++)
	{
		CNode* pNode = (CNode*)it->second;
		if(pNode->m_Point.x == point.x && pNode->m_Point.y == point.y)
			return pNode;
	}
	return nullptr;
}
DWORD CTopology::GetEdgeIndex(const DWORD& dwEdgeId) const
{
	DWORD index = 0;
	for(CEdgeMap::const_iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); it++)
	{
		CEdge* pEdge = (CEdge*)it->second;
		if(pEdge->m_dwId == dwEdgeId)
			break;

		index++;
	}

	return index;
}

DWORD CTopology::GetNodeIndex(const DWORD& dwNodeId) const
{
	DWORD index = 0;
	for(CNodeMap::const_iterator it = m_mapIdNode.begin(); it != m_mapIdNode.end(); it++)
	{
		CNode* pNode = (CNode*)it->second;
		if(pNode->m_dwId == dwNodeId)
			break;

		index++;
	}

	return index;
}

CEdge* CTopology::GetEdge(const DWORD& dwNode1, const DWORD& dwNode2) const
{
	for(CEdgeMap::const_iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); it++)
	{
		CEdge* pEdge = (CEdge*)it->second;
		if(pEdge->m_dwFromNode == dwNode1 && pEdge->m_dwToNode == dwNode2)
			return pEdge;
		else if(pEdge->m_dwFromNode == dwNode2 && pEdge->m_dwToNode == dwNode1)
			return pEdge;
	}

	return nullptr;
}
CEdge* CTopology::GetEdge(const WORD& wLayer, const DWORD& dwPoly) const
{
	for(CEdgeMap::const_iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); it++)
	{
		CEdge* pEdge = (CEdge*)it->second;
		if(pEdge->m_wLayer == wLayer && pEdge->m_dwPoly == dwPoly)
			return pEdge;		
	}
	return nullptr;
}
void CTopology::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& inRect) const
{
	if(m_bVisible == false)
		return;

	CLine line((CColor*)&m_colorEdge, 10);
	for(CEdgeMap::const_iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); it++)
	{
		CEdge* pEdge = (CEdge*)it->second;
		pEdge->Draw(g, viewinfo, line, inRect, *this);
	}
	line.m_pColor = nullptr;
	Gdiplus::Brush* brush = ::new Gdiplus::SolidBrush(m_colorNode.GetColor());
	CSize radius(3, 3);
	for(CNodeMap::const_iterator it = m_mapIdNode.begin(); it != m_mapIdNode.end(); it++)
	{
		CNode* pNode = (CNode*)it->second;
		pNode->Draw(g, viewinfo, brush, radius, inRect, *this);
	}

	::delete brush;
	brush = nullptr;

	if(m_pActivateNode != nullptr)
	{
		const CSize radius(4, 4);
		m_pActivateNode->DrawActive(g, viewinfo, *this, radius);
	}

	for(CEdgeMap::const_iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); it++)
	{
		CEdge* pEdge = (CEdge*)it->second;
		if(pEdge->m_bActive == true)
		{
			const Gdiplus::SizeF size = CViewinfo::PointsToPixels(3, CSize(72, 72));
			pEdge->DrawActive(g, viewinfo, *this, size.Width);
		}
	}
}

void CTopology::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(ar.IsStoring())
	{
		ar << m_Rect;
		ar << m_strName;
		ar << m_bVisible;
	}
	else
	{
		ar >> m_Rect;
		ar >> m_strName;
		ar >> m_bVisible;
	}
	if(ar.IsStoring())
	{
		ar << m_mapIdNode.size();
		for(CNodeMap::iterator it = m_mapIdNode.begin(); it != m_mapIdNode.end(); it++)
		{
			CNode* pNode = (CNode*)it->second;
			pNode->Serialize(ar, dwVersion);
		}
		ar << m_mapIdEdge.size();
		for(CEdgeMap::iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); it++)
		{
			CEdge* pEdge = (CEdge*)it->second;
			pEdge->Serialize(ar, dwVersion);
		}
	}
	else
	{
		int size1;
		ar >> size1;
		for(int i = 0; i < size1; i++)
		{
			CNode* pNode = ApplyNode();
			pNode->Serialize(ar, dwVersion);
			m_mapIdNode[pNode->m_dwId] = pNode;
			if(pNode->m_dwId > m_dwMaxNodeId)
			{
				m_dwMaxNodeId = pNode->m_dwId;
			}
		}
		int size2;
		ar >> size2;
		for(int i = 0; i < size2; i++)
		{
			CEdge* pEdge = ApplyEdge();
			pEdge->Serialize(ar, dwVersion);
			m_mapIdEdge[pEdge->m_dwId] = pEdge;
			if(pEdge->m_dwId > m_dwMaxEdgeId)
			{
				m_dwMaxEdgeId = pEdge->m_dwId;
			}
		}
	}
	m_bModifyFlag = false;
}
void CTopology::ActivateNode(CWnd* pWnd, const CViewinfo& viewinfo, CNode* pNode) const
{
	if(pWnd == nullptr)
		return;
	if(pNode == nullptr)
		return;
	if(pNode == m_pActivateNode)
		return;

	if(m_pActivateNode != nullptr)
	{
		m_pActivateNode->Invalidate(pWnd, viewinfo, *this);
	}
	const CClientDC dc(pWnd);

	Gdiplus::Graphics g(dc.m_hDC);
	g.SetPageUnit(Gdiplus::UnitPixel);
	g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	//g.SetTransform(&matrixDoctoView);
	const CSize radius(4, 4);
	pNode->DrawActive(g, viewinfo, *this, radius);

	g.ReleaseHDC(dc.m_hDC);

	m_pActivateNode = pNode;
}

void CTopology::ActivateEdge(CWnd* pWnd, const CViewinfo& viewinfo, CEdge* pEdge) const
{
	if(pWnd == nullptr)
		return;
	if(pEdge->m_bActive == true)
		return;
	const CClientDC dc(pWnd);
	Gdiplus::Graphics g(dc.m_hDC);
	g.SetPageUnit(Gdiplus::UnitPixel);
	g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	//	g.SetTransform(&matrixDoctoView);

	pEdge->DrawActive(g, viewinfo, *this, 4);

	g.ReleaseHDC(dc.m_hDC);

	pEdge->m_bActive = true;
}

void CTopology::InactivateAll(CWnd* pWnd, const CViewinfo& viewinfo) const
{
	for(CEdgeMap::const_iterator it = m_mapIdEdge.cbegin(); it != m_mapIdEdge.cend(); it++)
	{
		CEdge* pEdge = (CEdge*)it->second;
		if(pEdge->m_bActive == true)
		{
			pEdge->Invalidate(pWnd, viewinfo, *this);
			pEdge->m_bActive = false;
		}
	}

	if(m_pActivateNode != nullptr)
	{
		m_pActivateNode->Invalidate(pWnd, viewinfo, *this);
		m_pActivateNode = nullptr;
	}
}

bool CTopology::InsertEdge(CWnd* pWnd, const CViewinfo& viewinfo, const WORD& wLayer, const CPoly* pPoly, const unsigned int& tolerance)
{
	if(pPoly->m_bClosed == true)
		return false;

	if(IsPolyExisting(pPoly) == true)
		return false;
	const CPoint& point1 = pPoly->GetAnchor(1);
	const CPoint& point2 = pPoly->GetAnchor(pPoly->m_nAnchors);

	double minFDist = tolerance;
	double minTDist = tolerance;
	CNode* pMinFNode = nullptr;
	CNode* pMinTNode = nullptr;
	for(CNodeMap::iterator it = m_mapIdNode.begin(); it != m_mapIdNode.end(); it++)
	{
		CNode* pNode = (CNode*)it->second;
		Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(pNode->m_Point);
		const double FDist = sqrt((double)(pNode->m_Point.x - point1.x)*(pNode->m_Point.x - point1.x) + (double)(pNode->m_Point.y - point1.y)*(pNode->m_Point.y - point1.y));
		const double TDist = sqrt((double)(pNode->m_Point.x - point2.x)*(pNode->m_Point.x - point2.x) + (double)(pNode->m_Point.y - point2.y)*(pNode->m_Point.y - point2.y));
		if(FDist < minFDist)
		{
			minFDist = FDist;
			pMinFNode = pNode;
		}
		if(TDist < minTDist)
		{
			minTDist = TDist;
			pMinTNode = pNode;
		}
	}

	CEdge* pEdge = this->ApplyEdge();
	this->NewEdgeCreated(pEdge);
	if(m_mapIdEdge.find(pEdge->m_dwId) == m_mapIdEdge.end())
	{
		m_mapIdEdge[pEdge->m_dwId] = pEdge;
	}

	pEdge->m_dwPoly = pPoly->m_geoId;
	pEdge->m_Rect = pPoly->m_Rect;
	if(wLayer == 0XFFFF)
	{
		pEdge->m_wLayer = -1;
		pEdge->m_pPoly = (CPoly*)pPoly->Clone();
	}
	else
	{
		pEdge->m_wLayer = wLayer;
		pEdge->m_pPoly = nullptr;
	}

	if(pMinFNode != nullptr)
	{
		pEdge->m_dwFromNode = pMinFNode->m_dwId;

		pMinFNode->InsertRelatingEdge(pEdge->m_dwId);
	}
	else
	{
		CNode* pNode = this->ApplyNode();
		this->NewNodeCreated(pNode);
		if(m_mapIdNode.find(pNode->m_dwId) == m_mapIdNode.end())
		{
			m_mapIdNode[pNode->m_dwId] = pNode;
		}

		pNode->m_Point = point1;
		pEdge->m_dwFromNode = pNode->m_dwId;

		pNode->InsertRelatingEdge(pEdge->m_dwId);

		pNode->Invalidate(pWnd, viewinfo, *this);

		pMinFNode = pNode;
	}
	const double TDist = sqrt((double)(pMinFNode->m_Point.x - point2.x)*(pMinFNode->m_Point.x - point2.x) + (double)(pMinFNode->m_Point.y - point2.y)*(pMinFNode->m_Point.y - point2.y));
	if(TDist < minTDist)//this edge is a ring
	{
		minTDist = TDist;
		pMinTNode = pMinFNode;
	}
	if(pMinTNode != nullptr)
	{
		pEdge->m_dwToNode = pMinTNode->m_dwId;

		pMinTNode->InsertRelatingEdge(pEdge->m_dwId);
	}
	else
	{
		CNode* pNode = this->ApplyNode();
		this->NewNodeCreated(pNode);
		if(m_mapIdNode.find(pNode->m_dwId) == m_mapIdNode.end())
		{
			m_mapIdNode[pNode->m_dwId] = pNode;
		}

		pNode->m_Point = point2;
		pEdge->m_dwToNode = pNode->m_dwId;

		pNode->InsertRelatingEdge(pEdge->m_dwId);

		pNode->Invalidate(pWnd, viewinfo, *this);
	}

	pPoly->Invalidate(pWnd, viewinfo, 1);
	m_bModifyFlag = true;
	return true;
}

void CTopology::Invalidate(CDocument* pDocument) const
{
	POSITION pos = pDocument->GetFirstViewPosition();
	while(pos != nullptr)
	{
		CView* pView = pDocument->GetNextView(pos);
		pView->Invalidate();
	}
}

CNode* CTopology::PickNode(const CViewinfo& viewinfo, const CPoint& docPoint, int nRadius) const
{
	if(m_bVisible == false)
		return nullptr;

	for(CNodeMap::const_iterator it = m_mapIdNode.cbegin(); it != m_mapIdNode.cend(); it++)
	{
		CNode* pNode = (CNode*)it->second;
		if(pNode->Pick(viewinfo, docPoint, nRadius) == true)
		{
			return pNode;
		}
	}

	return nullptr;
}

CEdge* CTopology::PickEdge(const CViewinfo& viewinfo, const CPoint& docPoint, int nRadius) const
{
	if(m_bVisible == false)
		return nullptr;

	for(CEdgeMap::const_iterator it = m_mapIdEdge.cbegin(); it != m_mapIdEdge.cend(); it++)
	{
		CEdge* pEdge = (CEdge*)it->second;
		if(pEdge->Pick(viewinfo, docPoint, nRadius, *this) == true)
		{
			return pEdge;
		}
	}

	return nullptr;
}

void CTopology::PickEdges(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& docPoint, int nRadius) const
{
	if(m_bVisible == false)
		return;
	const CClientDC dc(pWnd);
	Gdiplus::Graphics g(dc.m_hDC);
	g.SetPageUnit(Gdiplus::UnitPixel);
	g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

	for(CEdgeMap::const_iterator it = m_mapIdEdge.cbegin(); it != m_mapIdEdge.cend(); it++)
	{
		CEdge* pEdge = (CEdge*)it->second;
		if(pEdge->Pick(viewinfo, docPoint, nRadius, *this) == true)
		{
			if(pEdge->m_bActive == false)
			{
				pEdge->DrawActive(g, viewinfo, *this, 3);
				pEdge->m_bActive = true;
			}
		}
		else if(pEdge->m_bActive == true)
		{
			pEdge->Invalidate(pWnd, viewinfo, *this);

			pEdge->m_bActive = false;
		}
	}

	g.ReleaseHDC(dc.m_hDC);
}

void CTopology::PickEdges(CWnd* pWnd, const CViewinfo& viewinfo, CPoly& docPolygon) const
{
	if(m_bVisible == false)
		return;
	const CClientDC dc(pWnd);

	Gdiplus::Graphics g(dc.m_hDC);
	g.SetPageUnit(Gdiplus::UnitPixel);
	g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

	CRect gonRect = docPolygon.m_Rect;
	for(CEdgeMap::const_iterator it = m_mapIdEdge.cbegin(); it != m_mapIdEdge.cend(); it++)
	{
		CEdge* pEdge = (CEdge*)it->second;
		CRect rect = pEdge->m_Rect;
		if(rect.IntersectRect(rect, gonRect) == TRUE)
		{
			CPoly* poly = this->GetEdgePoly(pEdge->m_dwId);
			if(poly->PickIn(docPolygon) == true)
			{
				if(pEdge->m_bActive == false)
				{
					pEdge->DrawActive(g, viewinfo, *this, 3);
					pEdge->m_bActive = true;
				}
			}
			else if(pEdge->m_bActive == true)
			{
				pEdge->Invalidate(pWnd, viewinfo, *this);

				pEdge->m_bActive = false;
			}
		}
		else if(pEdge->m_bActive == true)
		{
			pEdge->Invalidate(pWnd, viewinfo, *this);

			pEdge->m_bActive = false;
		}
	}

	g.ReleaseHDC(dc.m_hDC);
}

void CTopology::RemoveNode(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const DWORD& dwId)
{
	const CNodeMap::iterator it = m_mapIdNode.find(dwId);
	if(it != m_mapIdNode.end())
	{
		CNode* pNode = (CNode*)it->second;
		if(pNode == nullptr)
			return;

		if(pNode->m_edgelist.GetCount() > 0)
		{
			AfxMessageBox(_T("�ڵ㲻�ܱ�ɾ������Ϊ���й����ıߴ���!"));
			return;
		}
		else
		{
			if(pWnd != nullptr)
			{
				const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(pNode->m_Point);
				CRect rect(CPoint(cliPoint.X, cliPoint.Y), CSize(0, 0));
				rect.InflateRect(5, 5);
				pWnd->InvalidateRect(rect);
			}

			this->PreRemoveNode(pNode);
			this->BeforeDisposeNode(pNode, datainfo);

			m_mapIdNode.erase(it);

			delete pNode;
			pNode = nullptr;

			m_bModifyFlag = true;
		}
	}
}

void CTopology::RemoveEdge(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const DWORD& dwId)
{
	const CEdgeMap::iterator it = m_mapIdEdge.find(dwId);
	if(it != m_mapIdEdge.end())
	{
		CEdge* pEdge = (CEdge*)it->second;
		if(pEdge == nullptr)
			return;

		CNode* pFNode = this->GetNode(pEdge->m_dwFromNode);
		if(pFNode != nullptr)
		{
			pFNode->RemoveRelatingEdge(dwId);
			if(pFNode->m_edgelist.GetCount() == 0)
			{
				this->RemoveNode(pWnd, datainfo, viewinfo, pEdge->m_dwFromNode);//ɾ���ӵ㡢
			}
		}

		CNode* pTNode = this->GetNode(pEdge->m_dwToNode);
		if(pTNode != nullptr)
		{
			pTNode->RemoveRelatingEdge(dwId);
			if(pTNode->m_edgelist.GetCount() == 0)
			{
				this->RemoveNode(pWnd, datainfo, viewinfo, pEdge->m_dwToNode);//ɾ���ӵ㡢
			}
		}

		if(pWnd != nullptr)
		{
			CPoly* pPoly = this->GetEdgePoly(pEdge->m_dwId);
			if(pPoly != nullptr)
			{
				if(pEdge->m_bActive == true)
				{
					pPoly->Invalidate(pWnd, viewinfo, 3);
				}
				else
				{
					pPoly->Invalidate(pWnd, viewinfo, 1);
				}
			}
		}
		this->PreRemoveEdge(pEdge);
		this->BeforeDisposeEdge(pEdge, datainfo);

		m_mapIdEdge.erase(it);

		delete pEdge;
		pEdge = nullptr;
		m_bModifyFlag = true;
	}
}

void CTopology::PreRemovePoly(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const DWORD& dwPolyId)
{
	DWORD dwEdge=-1;
	for(CEdgeMap::iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); it++)
	{
		CEdge* pEdge = (CEdge*)it->second;
		if(pEdge->m_dwPoly == dwPolyId) 
		{
			dwEdge = pEdge->m_dwId;
		}
	}
	if(dwEdge != -1)
	{
		this->RemoveEdge(pWnd, datainfo, viewinfo, dwEdge);
	}
}
void CTopology::DeleteIfOrphan(CNode* pNode)
{
	if(pNode == nullptr)
		return;
	if(pNode->m_edgelist.GetSize() == 0)
	{
		auto it = m_mapIdNode.find(pNode->m_dwId);
		if(it != m_mapIdNode.end())
		{
			m_mapIdNode.erase(it);
		}
		delete pNode;
		m_bModifyFlag = true;
	}
}
void CTopology::RemovePoly(CEdge* pEdge)
{
}


bool CTopology::CrossEdges(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& docPoint, int nRadius, Undoredo::CActionStack& actionstack)
{
	if(m_bVisible == false)
		return false;

	CEdge* pEdge1 = nullptr;
	CEdge* pEdge2 = nullptr;
	for(CEdgeMap::iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); it++)
	{
		CEdge* pEdge = (CEdge*)it->second;
		if(pEdge->Pick(viewinfo, docPoint, nRadius, *this) == true)
		{
			if(pEdge1 == nullptr)
			{
				pEdge1 = pEdge;
			}
			else if(pEdge2 == nullptr)
			{
				pEdge2 = pEdge;
			}
			else
			{
				AfxMessageBox(_T("��ѡȡ�ı߶����������޷���ӽ����!"));
				return false;
			}
		}
	}

	if(pEdge1 != nullptr && pEdge2 != nullptr)
	{
		CLayer* pLayer1 = this->GetEdgeLayer(pEdge1->m_dwId);
		CLayer* pLayer2 = this->GetEdgeLayer(pEdge2->m_dwId);
		if(pLayer1 == nullptr || pLayer2 == nullptr)
			return false;

		CPoly* poly1 = this->GetEdgePoly(pEdge1->m_dwId);
		CPoly* poly2 = this->GetEdgePoly(pEdge2->m_dwId);
		if(poly1 == nullptr || poly2 == nullptr)
			return false;

		CArray<CPoint, CPoint> pointarray;
		CPoly::GetIntersectinon(poly1, poly2, pointarray);
		if(pointarray.GetCount() == 1)
		{
			const CPoint point = pointarray.ElementAt(0);
			std::list<CGeom*> polys1 = poly1->Split(point, 0);
			std::list<CGeom*> polys2 = poly2->Split(point, 0);
			if(polys1.size() == 2 && polys2.size() == 2)
			{
				pLayer1->Replace(pWnd, viewinfo, poly1, polys1, actionstack);
				pLayer2->Replace(pWnd, viewinfo, poly2, polys2, actionstack);

				CPoly* pPoly12 = (CPoly*)*polys1.begin();
				polys2.pop_front();
				CPoly* pPoly11 = (CPoly*)*polys1.begin();
				CPoly* pPoly22 = (CPoly*)*polys2.begin();
				polys2.pop_front();
				CPoly* pPoly21 = (CPoly*)*polys2.begin();

				CNode* pNode = this->ApplyNode();
				this->NewNodeCreated(pNode);

				pNode->m_Point = point;

				CEdge* pEdge3 = pEdge1->Clone();
				CEdge* pEdge4 = pEdge2->Clone();
				delete pEdge3->m_pPoly;
				delete pEdge4->m_pPoly;

				pEdge3->m_pPoly = pPoly12;
				pEdge4->m_pPoly = pPoly22;
				pEdge3->m_Rect = pPoly12->m_Rect;
				pEdge4->m_Rect = pPoly22->m_Rect;

				this->NewEdgeCreated(pEdge3);
				this->NewEdgeCreated(pEdge4);

				pNode->InsertRelatingEdge(pEdge1->m_dwId);
				pNode->InsertRelatingEdge(pEdge2->m_dwId);
				pNode->InsertRelatingEdge(pEdge3->m_dwId);
				pNode->InsertRelatingEdge(pEdge4->m_dwId);

				const CNode* pEdge1FromNode = this->GetNode(pEdge1->m_dwFromNode);
				const CNode* pEdge1ToNode = this->GetNode(pEdge1->m_dwToNode);
				if(::LineLength2(poly1->GetAnchor(1), pEdge1FromNode->m_Point) < ::LineLength2(poly1->GetAnchor(1), pEdge1ToNode->m_Point))
				{
					pEdge1->m_dwToNode = pNode->m_dwId;
					pEdge3->m_dwFromNode = pNode->m_dwId;
				}
				else
				{
					pEdge1->m_dwFromNode = pNode->m_dwId;
					pEdge3->m_dwToNode = pNode->m_dwId;
				}

				const CNode* pEdge2FromNode = this->GetNode(pEdge2->m_dwFromNode);
				const CNode* pEdge2ToNode = this->GetNode(pEdge2->m_dwToNode);
				if(::LineLength2(poly2->GetAnchor(1), pEdge2FromNode->m_Point) < ::LineLength2(poly2->GetAnchor(1), pEdge2ToNode->m_Point))
				{
					pEdge2->m_dwToNode = pNode->m_dwId;
					pEdge4->m_dwFromNode = pNode->m_dwId;
				}
				else
				{
					pEdge2->m_dwFromNode = pNode->m_dwId;
					pEdge4->m_dwToNode = pNode->m_dwId;
				}

				if(typeid(pEdge1) == typeid(CRoadEdge))
				{
					const double length1 = pPoly11->GetLength(viewinfo.m_datainfo.m_zoomPointToDoc);
					const double length3 = pPoly12->GetLength(viewinfo.m_datainfo.m_zoomPointToDoc);

					((CRoadEdge*)pEdge1)->m_fLength *= length1/(length1 + length3);
					((CRoadEdge*)pEdge3)->m_fLength *= length3/(length1 + length3);
				}
				if(typeid(pEdge2) == typeid(CRoadEdge))
				{
					const double length2 = pPoly12->GetLength(viewinfo.m_datainfo.m_zoomPointToDoc);
					const double length4 = pPoly22->GetLength(viewinfo.m_datainfo.m_zoomPointToDoc);

					((CRoadEdge*)pEdge2)->m_fLength *= length2/(length2 + length4);
					((CRoadEdge*)pEdge4)->m_fLength *= length4/(length2 + length4);
				}

				m_mapIdNode[pNode->m_dwId] = pNode;
				m_mapIdEdge[pEdge3->m_dwId] = pEdge3;
				m_mapIdEdge[pEdge4->m_dwId] = pEdge4;

				pEdge1->m_bModified = true;
				pEdge2->m_bModified = true;
				pEdge3->m_bModified = true;
				pEdge4->m_bModified = true;

				pPoly11->m_bModified = true;
				pPoly12->m_bModified = true;
				pPoly21->m_bModified = true;
				pPoly22->m_bModified = true;

				this->InactivateAll(pWnd, viewinfo);
				this->ActivateNode(pWnd, viewinfo, pNode);
				m_bModifyFlag = true;
				return true;
			}
			else
			{
				AfxMessageBox(_T("�и��ʧ��!"));
				for(std::list<CGeom*>::iterator it = polys1.begin(); it != polys1.end(); it++)
				{
					const CGeom* geom = *it;
					delete geom;
				}
				for(std::list<CGeom*>::iterator it = polys2.begin(); it != polys2.end(); it++)
				{
					const CGeom* geom = *it;
					delete geom;
				}
			}
		}
		else
		{
			AfxMessageBox(_T("��ѡ��ķ�Χ�����������Ľ��㲻Ψһ���޷����й�ͨ!"));
		}
	}

	return false;
}

void CTopology::OnClear(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo)
{
	CEdgeMap::iterator it = m_mapIdEdge.begin();
	while(it != m_mapIdEdge.end())
	{
		CEdge* pEdge = (CEdge*)it->second;
		it++;

		if(pEdge->m_bActive == true)
		{
			this->RemoveEdge(pWnd, datainfo, viewinfo, pEdge->m_dwId);
		}
	}
}

void CTopology::CheckValidity()
{
	for(CNodeMap::iterator it = m_mapIdNode.begin(); it != m_mapIdNode.end(); it++)
	{
		CNode* pNode = (CNode*)it->second;
		for(int index = 0; index < pNode->m_edgelist.GetCount(); index++)
		{
			const DWORD dwEdge = pNode->m_edgelist.GetAt(index);
			CEdge* pEdge = this->GetEdge(dwEdge);
			if(pEdge == nullptr)
			{
				AfxMessageBox(_T("�ڵ�����ı��ڱ߼������Ҳ�����"));
			}
			else if(pEdge->m_dwFromNode != pNode->m_dwId && pEdge->m_dwToNode != pNode->m_dwId)
			{
				AfxMessageBox(_T("�ڵ�����ıߵĶ˵㲻�Ǹýڵ㣡"));
			}
		}
	}

	for(CEdgeMap::iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); it++)
	{
		CEdge* pEdge = (CEdge*)it->second;
		CNode* pFromNode = this->GetNode(pEdge->m_dwFromNode);
		if(pFromNode == nullptr)
		{
			AfxMessageBox(_T("�ߵĽڵ��ڽڵ㼯�в����ڣ�"));
		}
		else
		{
			bool has = false;
			for(int index = 0; index < pFromNode->m_edgelist.GetCount(); index++)
			{
				const DWORD dwEdge = pFromNode->m_edgelist.GetAt(index);
				if(pEdge->m_dwId == dwEdge)
				{
					has = true;
					break;
				}
			}
			if(has == false)
			{
				AfxMessageBox(_T("�ߵĽڵ�����ıߺͱ߲�ͬ��"));
			}
		}
		CNode* pToNode = this->GetNode(pEdge->m_dwToNode);
		if(pToNode == nullptr)
		{
			AfxMessageBox(_T("�ߵĽڵ��ڽڵ㼯�в����ڣ�"));
		}
		else
		{
			bool has = false;
			for(int index = 0; index < pToNode->m_edgelist.GetCount(); index++)
			{
				const DWORD dwEdge = pToNode->m_edgelist.GetAt(index);
				if(pEdge->m_dwId == dwEdge)
				{
					has = true;
					break;
				}
			}
			if(has == false)
			{
				AfxMessageBox(_T("�ߵĽڵ�����ıߺͱ߲�ͬ��"));
			}
		}
	}
}

BOOL CTopology::SetStyle()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CStyleDlg pDlg(nullptr, m_colorNode.ToRGB(), m_colorEdge.ToRGB());
	if(pDlg.DoModal() == IDOK)
	{
		m_colorNode.SetRGB(pDlg.m_colorNode);
		m_colorEdge.SetRGB(pDlg.m_colorEdge);
		m_bModifyFlag = true;
	}

	return TRUE;
}

void CTopology::BeforeDisposeNode(CNode* pNode, const CDatainfo& datainfo)
{
	if(m_pActivateNode == pNode)
	{
		m_pActivateNode = nullptr;
	}
}

void CTopology::BeforeDisposeEdge(CEdge* pEdge, const CDatainfo& datainfo)
{
}

bool CTopology::EdgeJoinable(const WORD& wLayer, const DWORD& dwPoly1, const DWORD& dwPoly2, const BYTE& mode) const
{
	CEdge* pEdge1 = GetEdge(wLayer, dwPoly1);
	CEdge* pEdge2 = GetEdge(wLayer, dwPoly2);
	if(pEdge1 == nullptr && pEdge2 == nullptr)
		return true;

	CNode* pNode1 = nullptr;
	CNode* pNode2 = nullptr;
	switch(mode)
	{
		case 1:
			pNode1 = pEdge1 == nullptr ? nullptr : GetNode(pEdge1->m_dwFromNode);
			pNode2 = pEdge2 == nullptr ? nullptr : GetNode(pEdge2->m_dwFromNode);
			break;
		case 2:
			pNode1 = pEdge1 == nullptr ? nullptr : GetNode(pEdge1->m_dwFromNode);
			pNode2 = pEdge2 == nullptr ? nullptr : GetNode(pEdge2->m_dwToNode);
			break;
		case 3:
			pNode1 = pEdge1 == nullptr ? nullptr : GetNode(pEdge1->m_dwToNode);
			pNode2 = pEdge2 == nullptr ? nullptr : GetNode(pEdge2->m_dwFromNode);
			break;
		case 4:
			pNode1 = pEdge1 == nullptr ? nullptr : GetNode(pEdge1->m_dwToNode);
			pNode2 = pEdge2 == nullptr ? nullptr : GetNode(pEdge2->m_dwToNode);
			break;
		default:
			return false;
	}
	if(pNode1 == pNode2)
		return pNode1->m_edgelist.GetSize() == 2;
	else if(pNode1 != nullptr)
		return pNode1->m_edgelist.GetSize() == 1;
	else if(pNode2 != nullptr)
		return pNode2->m_edgelist.GetSize() == 1;
	else
		return false;
}