#include "stdafx.h"

#include "Topo.h"
#include "Topoer.h"
#include "Node.h"
#include "Edge.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Global.h"
#include "../Layer/Layer.h"
#include "../Layer/LayerTree.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTopoer::CTopoer(CLayerTree& layertree)
	: CTopology(), m_layertree(layertree)
{
}

CTopoer::~CTopoer()
{
}

DWORD CTopoer::ApplyEdgeId() const
{
	return m_dwMaxEdgeId++;
}

DWORD CTopoer::ApplyNodeId() const
{	
	return m_dwMaxNodeId++;
}
CEdge* CTopoer::AddAEdge(const CLayer* layer, const CPoly* poly)
{
	CEdge* pEdge=new CEdge();
	pEdge->m_bModified=true;
	pEdge->m_wLayer=layer->m_wId;
	pEdge->m_dwPoly=poly->m_geoId;
	pEdge->m_dwId=CTopoer::ApplyEdgeId();
	pEdge->m_Rect=poly->m_Rect;
	m_mapIdEdge[pEdge->m_dwId]=pEdge;

	const CPoint& point1 = poly->GetAnchor(1);
	const CPoint& point2=poly->GetAnchor(poly->m_nAnchors);
	CNode* pNode1=GetNode(point1);
	if(pNode1 == nullptr)
	{
		pNode1=CTopology::ApplyNode(point1);
		m_mapIdNode[pNode1->m_dwId]=pNode1;
	}
	pEdge->m_dwFromNode=pNode1->m_dwId;
	pNode1->m_edgelist.Add(pEdge->m_dwId);

	CNode* pNode2=GetNode(point2);
	if(pNode2 == nullptr)
	{
		pNode2=CTopology::ApplyNode(point2);
		m_mapIdNode[pNode2->m_dwId]=pNode2;
	}
	pEdge->m_dwToNode=pNode2->m_dwId;
	if(pNode2 != pNode1)
	{
		pNode2->m_edgelist.Add(pEdge->m_dwId);
	}
	m_bModifyFlag = true;
	return pEdge;
}

CLayer* CTopoer::GetEdgeLayer(const DWORD& dwEdgeId) const
{
	CEdge* pEdge = this->GetEdge(dwEdgeId);
	if(pEdge==nullptr)
		return nullptr;

	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_wId==pEdge->m_wLayer)
			return layer;
	}
	return nullptr;
}

CPoly* CTopoer::GetEdgePoly(const DWORD& dwEdgeId) const
{
	CEdge* pEdge = this->GetEdge(dwEdgeId);
	if(pEdge==nullptr)
		return nullptr;

	CLayer* pLayer = GetEdgeLayer(dwEdgeId);
	if(pLayer==nullptr)
		return nullptr;

	POSITION pos = pLayer->m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = pLayer->m_geomlist.GetNext(pos);
		if(pGeom->m_geoId==pEdge->m_dwPoly)
		{
			return (CPoly*)pGeom;
		}
	}

	return nullptr;
}
void CTopoer::RemovePoly(CEdge* pEdge)
{
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_wId==pEdge->m_wLayer)
		{
			POSITION pos2 = layer->m_geomlist.GetHeadPosition();
			while(pos2!=nullptr)
			{
				CGeom* geom = layer->m_geomlist.GetAt(pos2);
				if(geom==nullptr)
				{
					layer->m_geomlist.GetNext(pos2);
					continue;
				}

				if(geom->m_geoId==pEdge->m_dwPoly)
				{
					layer->m_geomlist.RemoveAt(pos2);

					delete geom;
					geom = nullptr;
					break;
				}
				else
				{
					layer->m_geomlist.GetNext(pos2);
				}
			}
			break;
		}
	}

	CTopology::RemovePoly(pEdge);
}
bool CTopoer::PolySplitted(CWnd* pWnd, const CViewinfo& viewinfo, WORD wLayer, DWORD dwPoly, const CPoint& point, DWORD dwBred, DWORD& dwOldEdgeId, DWORD& dwNewEdgeId)
{
	CEdge* pOldEdge = GetEdge(wLayer, dwPoly);
	if(pOldEdge == nullptr)
		return false;

	CNode* pNode1 = this->GetNode(pOldEdge->m_dwFromNode);
	CNode* pNode2 = this->GetNode(pOldEdge->m_dwToNode);

	CEdge* pNewEdge = pOldEdge->Clone();
	pNewEdge->m_dwId = this->ApplyEdgeId();
	pNewEdge->m_dwPoly = dwBred;
	this->NewEdgeCreated(pNewEdge);
	m_mapIdEdge[pNewEdge->m_dwId] = pNewEdge;
	pNode2->ReplaceRelatingEdge(pOldEdge->m_dwId, pNewEdge->m_dwId);

	CNode* pNewNode = this->ApplyNode();
	pNewNode->m_Point = point;
	pNewNode->m_dwId = this->ApplyNodeId();
	this->NewNodeCreated(pNewNode);
	m_mapIdNode[pNewNode->m_dwId] = pNewNode;

	pNewEdge->m_dwFromNode = pNewNode->m_dwId;
	pOldEdge->m_dwToNode = pNewNode->m_dwId;

	pNewNode->InsertRelatingEdge(pOldEdge->m_dwId);
	pNewNode->InsertRelatingEdge(pNewEdge->m_dwId);
	pNewEdge->Invalidate(pWnd, viewinfo, *this);
	m_bModifyFlag = true;

	dwOldEdgeId = pOldEdge->m_dwId;
	dwNewEdgeId = pNewEdge->m_dwId;
	return true;
}

bool CTopoer::PolyJoined(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, WORD wLayer, DWORD dwPoly1, BYTE mode, DWORD dwPoly2, DWORD& dwEdgeId1, DWORD& dwEdgeId2)
{
	CEdge* pEdge1 = GetEdge(wLayer, dwPoly1);
	CEdge* pEdge2 = GetEdge(wLayer, dwPoly2);
	if(pEdge1 == nullptr || pEdge2 == nullptr)
		return false;

	CNode* pNode1 = nullptr;
	CNode* pJunctureNode1 = nullptr;
	CNode* pJunctureNode2 = nullptr;
	CNode* pNode2 = nullptr;
	switch(mode)
	{
		case 1:
			pNode1 = this->GetNode(pEdge2->m_dwToNode);
			pJunctureNode1 = this->GetNode(pEdge2->m_dwFromNode);
			pJunctureNode2 = this->GetNode(pEdge1->m_dwFromNode);
			pNode2 = this->GetNode(pEdge1->m_dwToNode);
			break;
		case 2:
			pNode1 = this->GetNode(pEdge2->m_dwFromNode);
			pJunctureNode1 = this->GetNode(pEdge2->m_dwToNode);
			pJunctureNode2 = this->GetNode(pEdge1->m_dwFromNode);
			pNode2 = this->GetNode(pEdge2->m_dwToNode);
			break;
		case 3:
			pNode1 = this->GetNode(pEdge1->m_dwFromNode);
			pJunctureNode1 = this->GetNode(pEdge1->m_dwToNode);
			pJunctureNode2 = this->GetNode(pEdge2->m_dwFromNode);
			pNode2 = this->GetNode(pEdge2->m_dwToNode);
			break;
		case 4:
			pNode1 = this->GetNode(pEdge1->m_dwFromNode);
			pJunctureNode1 = this->GetNode(pEdge1->m_dwToNode);
			pJunctureNode2 = this->GetNode(pEdge2->m_dwToNode);
			pNode2 = this->GetNode(pEdge2->m_dwFromNode);
			break;
		default:
			return false;
	}	

	pJunctureNode1->RemoveRelatingEdge(pEdge1->m_dwId);
	pJunctureNode1->RemoveRelatingEdge(pEdge2->m_dwId);
	pJunctureNode2->RemoveRelatingEdge(pEdge1->m_dwId);
	pJunctureNode2->RemoveRelatingEdge(pEdge2->m_dwId);
	if(pJunctureNode1->m_edgelist.GetCount() == 0)
	{
		this->RemoveNode(pWnd, datainfo, viewinfo, pJunctureNode1->m_dwId);//ɾ���ӵ㡢				
	}
	if(pJunctureNode2->m_edgelist.GetCount() == 0)
	{
		this->RemoveNode(pWnd, datainfo, viewinfo, pJunctureNode2->m_dwId);//ɾ���ӵ㡢				
	}

	pNode1->ReplaceRelatingEdge(pEdge2->m_dwId, pEdge1->m_dwId);
	pNode2->ReplaceRelatingEdge(pEdge2->m_dwId, pEdge1->m_dwId);
	pEdge1->m_dwFromNode = pNode1->m_dwId;
	pEdge1->m_dwToNode = pNode2->m_dwId;
	this->PreRemoveEdge(pEdge2);
	this->BeforeDisposeEdge(pEdge2, datainfo);
	auto it = m_mapIdEdge.find(pEdge2->m_dwId);
	m_mapIdEdge.erase(it);
	
	dwEdgeId1 = pEdge1->m_dwId;
	dwEdgeId2 = pEdge2->m_dwId;

	delete pEdge2;
	pEdge2 = nullptr;

	return true;
}