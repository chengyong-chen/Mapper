#include "stdafx.h"
#include "Topo.h"
#include "Topoer.h"
#include "RegionTopoer.h"

#include "Node.h"
#include "Polygon.h"
#include "Polying.h"
#include "layon.h"

#include "Edge.h"

#include "../Style/Line.h"
#include "../Layer/LayerTree.h"
#include "../Geometry/Global.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Doughnut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRegionTopoer::CRegionTopoer(CLayerTree& layertree, std::list<CLayon*>& layonlist)
	: CTopoer(layertree)
{
	for(auto it = layonlist.begin(); it != layonlist.end(); it++)
	{
		m_layonlist.push_back(*it);
	}
}

CRegionTopoer::~CRegionTopoer()
{
}
CNode* CRegionTopoer::ApplyNode() const
{
	CNode* pNode = new CNode();
	pNode->m_dwId = this->ApplyNodeId();
	pNode->m_bModified = true;
	return pNode;
}
CEdge* CRegionTopoer::ApplyEdge() const
{
	CEdge* pEdge = new CEdge();
	pEdge->m_dwId = this->ApplyEdgeId();
	pEdge->m_bModified = true;
	return pEdge;
}
void CRegionTopoer::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CTopoer::Serialize(ar, dwVersion);

	for(auto it = m_layonlist.begin(); it != m_layonlist.end(); it++)
	{
		(*it)->Serialize(ar, dwVersion);
	}
}
void CRegionTopoer::ReleaseWeb(pbf::writer& writer) const
{
	std::string cdata;
	pbf::writer child(cdata);

	std::map<DWORD, DWORD> mapEdgeIdIndex;
	{
		child.add_tag((pbf::tag)1, pbf::type::bytes);
		child.add_variant_uint32(m_mapIdEdge.size());
		unsigned int index = 0;
		for(CEdgeMap::const_iterator it = m_mapIdEdge.cbegin(); it != m_mapIdEdge.cend(); it++)
		{
			CEdge* pEdge = (CEdge*)it->second;
			pEdge->ReleaseWeb(*this, child);
			mapEdgeIdIndex[it->first] = index++;
		}
	}
	{
		child.add_tag((pbf::tag)2, pbf::type::bytes);
		child.add_variant_uint16(m_layonlist.size());
		for(auto it = m_layonlist.begin(); it != m_layonlist.end(); it++)
		{
			CLayon* layon = *it;
			child.add_variant_uint16(layon->m_wId);
			child.add_variant_uint16(layon->m_geomlist.GetCount());
			POSITION pos = layon->m_geomlist.GetHeadPosition();
			while(pos != nullptr)
			{
				CGeom* pGeom = layon->m_geomlist.GetNext(pos);
				BYTE type = pGeom->Gettype();
				switch(type)
				{
					case 51:
						{
							CPolygon* pPolygon = (CPolygon*)pGeom;
							pPolygon->ReleaseWeb(*this, child, mapEdgeIdIndex);
						}
						break;
					case 52:
						{
							CGonroup* pGonroup = (CGonroup*)pGeom;
							pGonroup->ReleaseWeb(*this, child, mapEdgeIdIndex);
						}
						break;
					case 61:
						{
							CPolying* pLinestring = (CPolying*)pGeom;
							pLinestring->ReleaseWeb(*this, child, mapEdgeIdIndex);
						}
						break;
					case 62:
						{
							CIngroup* pIngroup = (CIngroup*)pGeom;
							pIngroup->ReleaseWeb(*this, child, mapEdgeIdIndex);
						}
						break;
					default://it is an error
						break;
				}
			}
		}
	}

	writer.add_block((pbf::tag)this->Gettype(), cdata.c_str(), cdata.length());
}
void CRegionTopoer::FilterEdge(const CDatainfo& datainfo)
{
	for(CEdgeMap::iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end();)
	{
		CEdge* pEdge = (CEdge*)it->second;
		if(pEdge->m_dwFromNode == pEdge->m_dwToNode)
		{
			CNode* fNode = this->GetNode(pEdge->m_dwFromNode);
			CNode* tNode = this->GetNode(pEdge->m_dwToNode);

			fNode->RemoveRelatingEdge(pEdge->m_dwId);
			tNode->RemoveRelatingEdge(pEdge->m_dwId);

			if(fNode->m_edgelist.GetSize() == 0)
			{
				CNodeMap::iterator it1 = m_mapIdNode.find(fNode->m_dwId);
				if(it1 != m_mapIdNode.end())
				{
					this->PreRemoveNode(fNode);
					this->BeforeDisposeNode(fNode, datainfo);

					m_mapIdNode.erase(it1);
					delete fNode;
					fNode = nullptr;
				}
			}

			this->PreRemoveEdge(pEdge);
			this->BeforeDisposeEdge(pEdge, datainfo);

			m_mapIdEdge.erase(it++);
			delete pEdge;
			pEdge = nullptr;
		}
		else
			it++;
	}

	m_bModifyFlag = true;
}

//ȥ������ڵ������
void CRegionTopoer::FilterNode(const CDatainfo& datainfo)
{
	bool keep = false;
	do
	{
		keep = false;

		for(CNodeMap::iterator it = m_mapIdNode.begin(); it != m_mapIdNode.end();)
		{
			CNode* pNode = (CNode*)it->second;
			const long edgecount = pNode->m_edgelist.GetSize();
			if(edgecount == 0)
			{
				m_mapIdNode.erase(it++);

				this->PreRemoveNode(pNode);
				this->BeforeDisposeNode(pNode, datainfo);

				delete pNode;
				pNode = nullptr;
				keep = true;
			}
			else if(edgecount == 1)
			{
				const DWORD dwEdge = pNode->m_edgelist.GetAt(0);
				CEdge* pEdge = this->GetEdge(dwEdge);
				if(pEdge != nullptr)
				{
					if(pEdge->m_dwFromNode == pEdge->m_dwToNode)//it is a self closing ring
					{
						++it;
						continue;
					}
					else if(pEdge->m_dwFromNode == pNode->m_dwId)
					{
						CNode* eNode = GetNode(pEdge->m_dwToNode);
						eNode->RemoveRelatingEdge(pEdge->m_dwId);
					}
					else if(pEdge->m_dwToNode == pNode->m_dwId)
					{
						CNode* sNode = GetNode(pEdge->m_dwFromNode);
						sNode->RemoveRelatingEdge(pEdge->m_dwId);
					}
				}

				m_mapIdNode.erase(it++);

				this->PreRemoveNode(pNode);
				this->BeforeDisposeNode(pNode, datainfo);

				delete pNode;
				pNode = nullptr;

				if(pEdge != nullptr)
				{
					m_mapIdEdge.erase(pEdge->m_dwId);

					this->PreRemoveEdge(pEdge);
					this->BeforeDisposeEdge(pEdge, datainfo);

					delete pEdge;
					pEdge = nullptr;
				}

				keep = true;
			}
			else
				++it;
		}
	} while(keep == true);

	m_bModifyFlag = true;
}

void CRegionTopoer::RemoveEdge(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const DWORD& dwEdgeId)
{
	for(auto it = m_layonlist.begin(); it != m_layonlist.end(); it++)
	{
		CGonroup* pGonroup1 = nullptr;
		CGonroup* pGonroup2 = nullptr;
		CPolygon* pPolygon1 = nullptr;
		CPolygon* pPolygon2 = nullptr;
		bool direction1;
		bool direction2;

		CLayon* layon = *it;
		POSITION pos1 = layon->m_geomlist.GetTailPosition();
		POSITION pos2;
		while((pos2 = pos1) != nullptr)
		{
			CGeom* pGeom = layon->m_geomlist.GetPrev(pos1);
			switch(pGeom->Gettype()) {
				case 51:
					{
						CPolygon* pPolygon = (CPolygon*)pGeom;
						if(pPolygon->IsEdgeInvolved(*this, dwEdgeId) == false)
						{
						}
						else if(pPolygon1 == nullptr)
						{
							pGonroup1 = nullptr;
							pPolygon1 = pPolygon;
						}
						else if(pPolygon2 == nullptr)
						{
							pGonroup2 = nullptr;
							pPolygon2 = pPolygon;
						}
					}
					break;
				case 52:
					{
						CGonroup* pGonroup = (CGonroup*)pGeom;
						CPolygon* pPolygon = pGonroup->FindInvolvedEdge(*this, dwEdgeId, pPolygon1);
						if(pPolygon == nullptr)
						{
						}
						else if(pPolygon1 == nullptr)
						{
							pGonroup1 = pGonroup;
							pPolygon1 = pPolygon;
						}
						else if(pPolygon2 == nullptr)
						{
							pGonroup2 = pGonroup;
							pPolygon2 = pPolygon;
						}
					}
					break;
				case 61:
					{
						CPolying* pPolying = (CPolying*)pGeom;
						CPolying* pExtra = pPolying->RemoveEdge(*this, dwEdgeId);
						if(pExtra != nullptr)
						{
							layon->m_geomlist.InsertAfter(pos2, pExtra);
						}
						else if(pPolying->m_edgelist.GetSize() == 0)
						{
							delete pPolying;
							layon->m_geomlist.RemoveAt(pos2);
						}
					}
					break;
				case 62:
					{
						CIngroup* pIngroup = (CIngroup*)pGeom;
						pIngroup->RemoveEdge(*this, dwEdgeId);
						if(pIngroup->m_geomlist.GetCount() == 0)
						{
							delete pIngroup;
							layon->m_geomlist.RemoveAt(pos2);
						}
					}
					break;
			}
		}

		if(pPolygon1 != nullptr && pPolygon2 != nullptr)
		{
			if(pGonroup1 != nullptr && pGonroup2 != nullptr)//merge polygon2 to polygon1		
			{
				if(pGonroup1->m_geomlist.GetCount() > pGonroup2->m_geomlist.GetCount())//from bigger group to smaller group
				{
					CPolygon* temp1 = pPolygon2;
					pPolygon2 = pPolygon1;
					pPolygon1 = temp1;

					CGonroup* temp2 = pGonroup2;
					pGonroup2 = pGonroup1;
					pGonroup1 = temp2;
				}
			}
			else if(pGonroup1 != nullptr)//from group to standalone
			{
				CPolygon* temp1 = pPolygon2;
				pPolygon2 = pPolygon1;
				pPolygon1 = temp1;

				CGonroup* temp2 = pGonroup2;
				pGonroup2 = pGonroup1;
				pGonroup1 = temp2;
			}

			pPolygon1->RemoveEdge(dwEdgeId);
			pPolygon2->RemoveEdge(dwEdgeId);
			CEdge* pEdge = GetEdge(dwEdgeId);
			CNode* pNode1 = GetNode(pEdge->m_dwFromNode);
			CNode* pNode2 = GetNode(pEdge->m_dwToNode);
			while(pNode1->GetEdgeCount() == 2)
			{
				DWORD nextEdge = pNode1->GetNextEdge(dwEdgeId);
				CEdge* prev = GetEdge(nextEdge);
				pPolygon1->RemoveEdge(prev->m_dwId);
				pPolygon2->RemoveEdge(prev->m_dwId);
				DWORD other = prev->m_dwFromNode == pNode1->m_dwId ? prev->m_dwToNode : prev->m_dwFromNode;
				pNode1 = GetNode(other);
			}
			while(pNode2->GetEdgeCount() == 2)
			{
				DWORD nextEdge = pNode2->GetNextEdge(dwEdgeId);
				CEdge* next = GetEdge(nextEdge);
				pPolygon1->RemoveEdge(next->m_dwId);
				pPolygon2->RemoveEdge(next->m_dwId);
				DWORD other = next->m_dwFromNode == pNode2->m_dwId ? next->m_dwToNode : next->m_dwFromNode;
				pNode2 = GetNode(other);
			}
			int lastEdge1 = pPolygon1->m_edgelist.GetAt(pPolygon1->m_edgelist.GetSize() - 1);
			int firstEdge2 = pPolygon2->m_edgelist.GetAt(0);
			int lastEdge2 = pPolygon2->m_edgelist.GetAt(pPolygon2->m_edgelist.GetSize() - 1);
			CEdge* pLast1 = GetEdge(lastEdge1 >= 0 ? lastEdge1 : ~lastEdge1);
			CEdge* pFirst2 = GetEdge(firstEdge2 >= 0 ? firstEdge2 : ~firstEdge2);
			CEdge* pLast2 = GetEdge(lastEdge2 >= 0 ? lastEdge2 : ~lastEdge2);
			DWORD lastNode1 = lastEdge1 >= 0 ? pLast1->m_dwToNode : pLast1->m_dwFromNode;
			DWORD firstNode2 = firstEdge2 >= 0 ? pFirst2->m_dwFromNode : pFirst2->m_dwToNode;
			DWORD lastNode2 = lastEdge2 >= 0 ? pLast2->m_dwToNode : pLast2->m_dwFromNode;
			if(lastNode1 == firstNode2)
			{
				pPolygon1->m_edgelist.Append(pPolygon2->m_edgelist);
			}
			else if(lastNode1 == lastNode2)
			{
				for(int index = pPolygon2->m_edgelist.GetSize() - 1; index >= 0; index--)
				{
					pPolygon1->m_edgelist.Add(pPolygon2->m_edgelist.GetAt(index));
				}
			}
			pPolygon1->RecalRect();
			pPolygon1->Invalidate(pWnd, viewinfo, 3);
			m_bModifyFlag = true;

			DeletePolygon(pWnd, viewinfo, pPolygon2);
		}
		else if(pPolygon1 != nullptr)
		{
			DeletePolygon(pWnd, viewinfo, pPolygon1);
		}
	}

	CTopoer::RemoveEdge(pWnd, datainfo, viewinfo, dwEdgeId);
}
void CRegionTopoer::DeletePolygon(CWnd* pWnd, const CViewinfo& viewinfo, CPolygon* pPolygon)
{
	if(pPolygon == nullptr)
		return;

	pPolygon->Invalidate(pWnd, viewinfo, 3);
	m_bModifyFlag = true;
	for(auto it = m_layonlist.begin(); it != m_layonlist.end(); it++)
	{
		CLayon* layon = *it;
		POSITION pos2;
		POSITION pos1 = layon->m_geomlist.GetTailPosition();
		while((pos2 = pos1) != nullptr)
		{
			CGeom* pGeom = layon->m_geomlist.GetPrev(pos1);
			if(pGeom == pPolygon)
			{
				delete pPolygon;
				layon->m_geomlist.RemoveAt(pos2);
				break;
			}
			if(pGeom->Gettype() == 52)
			{
				CGonroup* pGonroup = (CGonroup*)pGeom;
				if(pGonroup->RemoveMember(pPolygon))
				{
					if(pGonroup->m_geomlist.GetCount() == 0)
					{
						layon->m_geomlist.RemoveAt(pos2);
						delete pGonroup;
					}
					break;
				}
			}
		}
	}
}
void CRegionTopoer::PreDelete()
{
	for(auto it = m_layonlist.begin(); it != m_layonlist.end(); it++)
	{
		CLayon* layon = *it;
		m_layertree.m_root.DetachChild(layon);
		delete layon;
	}
	m_layonlist.clear();
}
void CRegionTopoer::CreateLinestrings(const CDatainfo& datainfo, CLayon* layon)
{
	std::vector<CEdge*> activeEdges;
	for(CEdgeMap::iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); ++it)
	{
		CEdge* pEdge = (CEdge*)it->second;
		if(pEdge->m_bActive)
			activeEdges.push_back(pEdge);
	}
	if(activeEdges.size() == 0)
		return;
	
	if(layon == nullptr)
	{
		auto it = std::find_if(m_layonlist.begin(), m_layonlist.end(), [&](CLayon* layon) {
			return layon->m_strName == _T("linestrings");
			});
		if(it != m_layonlist.end())
			layon = *it;
		else
			return;
	}
	layon = layon == nullptr ? m_layonlist.front() : layon;
	while(activeEdges.size() > 0)
	{
		CEdge* pSeed = activeEdges.front();
		activeEdges.erase(activeEdges.begin());
		std::vector<CEdge*> chain;
		chain.push_back(pSeed);
		CEdge* pBackward = pSeed;
		do
		{
			auto it = std::find_if(activeEdges.begin(), activeEdges.end(), [&](CEdge* pEdge) {
				return pEdge->m_dwToNode == pBackward->m_dwFromNode;
				});
			if(it == activeEdges.end())
				break;
			else
			{
				chain.insert(chain.begin(), *it);
				pBackward = *it;
				activeEdges.erase(it);				
			}
		} while(activeEdges.empty() == false);
		CEdge* pForeward = pSeed;
		do
		{
			auto it = std::find_if(activeEdges.begin(), activeEdges.end(), [&](CEdge* pEdge) {
				return pEdge->m_dwFromNode == pForeward->m_dwToNode;
				});
			if(it == activeEdges.end())
				break;
			else
			{
				chain.push_back(*it);
				pForeward = *it;
				activeEdges.erase(it);				
			}
		} while(activeEdges.empty() == false);

		CPolying* pPolying = new CPolying(*this);
		for(std::vector<CEdge*>::iterator it = chain.begin(); it != chain.end(); ++it)
		{
			CEdge* pEdge = *it;
			pPolying->m_edgelist.Add(pEdge->m_dwId);
		}
		pPolying->RecalRect();
		layon->m_geomlist.AddTail(pPolying);
	}
}
void CRegionTopoer::CreatePolygons(const CDatainfo& datainfo, CLayon* layon)
{
	//CRegionTopoer::FilterNode(datainfo);
	//const unsigned int radius=datainfo.m_zoomPointToDoc*5;
	//for(CNodeMap::iterator it=m_mapIdNode.begin(); it != m_mapIdNode.end(); it++)
	//{
	//	CNode* pNode=(CNode*)it->second;
	//	if(pNode != nullptr)
	//	{
	//		pNode->SortEdge(*this, radius);
	//	}
	//}
	//std::list<DWORD> leftused;
	//std::list<DWORD> rightused;
	//for(CNodeMap::iterator it=m_mapIdNode.begin(); it != m_mapIdNode.end(); it++)
	//{
	//	CNode* pEndNode=(CNode*)it->second;
	//	if(pEndNode == nullptr)
	//		continue;

	//	const int nCount=pEndNode->m_edgelist.GetSize();
	//	for(int index=0; index < nCount; index++)
	//	{
	//		const DWORD dwStartedgeID=pEndNode->m_edgelist.GetAt(index);
	//		CEdge* pEdge=this->GetEdge(dwStartedgeID);
	//		if(pEdge->m_dwFromNode == pEndNode->m_dwId && pEdge->m_dwToNode == pEndNode->m_dwId)//ring
	//		{
	//			if(std::find(leftused.begin(), leftused.end(), pEdge->m_dwId) == leftused.end())
	//			{
	//				CPolygon* polygon1=new CPolygon(*this);
	//				polygon1->m_edgelist.Add(~pEdge->m_dwId);
	//				m_layon->m_geomlist.push_back(polygon1);
	//				leftused.push_back(pEdge->m_dwId);
	//			}
	//			if(std::find(rightused.begin(), rightused.end(), pEdge->m_dwId) == rightused.end())
	//			{
	//				CPolygon* polygon2=new CPolygon(*this);
	//				polygon2->m_edgelist.Add(pEdge->m_dwId);
	//				m_layon->m_geomlist.push_back(polygon2);
	//				rightused.push_back(pEdge->m_dwId);
	//			}
	//		}
	//		else
	//		{
	//			CEdge* startedge=pEdge;
	//			const DWORD dwStartnodeID=pEndNode->m_dwId == startedge->m_dwFromNode ? startedge->m_dwToNode : startedge->m_dwFromNode;
	//			CNode* startnode=this->GetNode(dwStartnodeID);

	//			CPolygon* polygon=new CPolygon(*this);
	//			CNode* node=this->GetNode(dwStartnodeID);;
	//			CEdge* edge=startedge;
	//			do
	//			{
	//				if(node->m_dwId == edge->m_dwToNode)
	//				{
	//					if(std::find(leftused.begin(), leftused.end(), pEdge->m_dwId) != leftused.end())
	//					{
	//						delete polygon;
	//						polygon=nullptr;
	//						break;
	//					}
	//					else
	//					{
	//						polygon->m_edgelist.Add(~edge->m_dwId);

	//						node=GetNode(edge->m_dwFromNode);
	//						const DWORD dwEdgeId=node->GetNextEdge(edge->m_dwId);
	//						edge=(CEdge*)GetEdge(dwEdgeId);

	//						leftused.push_back(pEdge->m_dwId);
	//					}
	//				}
	//				else if(node->m_dwId == edge->m_dwFromNode)
	//				{
	//					if(std::find(rightused.begin(), rightused.end(), pEdge->m_dwId) != rightused.end())
	//					{
	//						delete polygon;
	//						polygon=nullptr;
	//						break;
	//					}
	//					else
	//					{
	//						polygon->m_edgelist.Add(edge->m_dwId);

	//						node=GetNode(edge->m_dwToNode);
	//						const DWORD dwEdgeId=node->GetNextEdge(edge->m_dwId);
	//						edge=(CEdge*)GetEdge(dwEdgeId);

	//						rightused.push_back(pEdge->m_dwId);
	//					}
	//				}
	//				else
	//				{
	//					break;
	//				}
	//			} while(node != startnode);

	//			if(polygon != nullptr)
	//				m_layon->m_geomlist.push_back(polygon);
	//		}
	//	}
	//}

	////����ÿһ������ε�����ͷ�Χ 
	////�ж��Ƿ�Ϊ��
	//for(std::list<CPolygon*>::iterator it=m_layon->m_geomlist.begin(); it != m_layon->m_geomlist.end(); it++)
	//{
	//	CPolygon* polygon=*it;
	//	polygon->RecalRect(*this);
	//	if(polygon->CalArea(*this) < 0)
	//	{
	//		polygon->m_bIsland=true;
	//	}
	//}
}



bool CRegionTopoer::PolySplitted(CWnd* pWnd, const CViewinfo& viewinfo, WORD wLayer, DWORD dwPoly, const CPoint& point, DWORD dwBred, DWORD& dwOldEdgeId, DWORD& dwNewEdgeId)
{
	if(CTopoer::PolySplitted(pWnd, viewinfo, wLayer, dwPoly, point, dwBred, dwOldEdgeId, dwNewEdgeId) == true)
	{
		for(auto it = m_layonlist.begin(); it != m_layonlist.end(); it++)
		{
			CLayon* layon = *it;
			POSITION pos = layon->m_geomlist.GetTailPosition();
			while(pos != nullptr)
			{
				CGeom* pGeom = layon->m_geomlist.GetPrev(pos);
				switch(pGeom->Gettype())
				{
					case 51:
						{
							CPolygon* pPolygon = (CPolygon*)pGeom;
							pPolygon->EdgeBred(dwOldEdgeId, dwNewEdgeId);
						}
						break;
					case 52:
						{
							CGonroup*pGonroup=(CGonroup*)pGeom;
							pGonroup->EdgeBred(dwOldEdgeId,dwNewEdgeId);
						}
						break;
					case 61:
						{
							CPolying* pPolying = (CPolying*)pGeom;
							pPolying->EdgeBred(dwOldEdgeId, dwNewEdgeId);
						}
						break;
					case 62:
						{
							CIngroup* pIngroup = (CIngroup*)pGeom;
							pIngroup->EdgeBred(dwOldEdgeId, dwNewEdgeId);
						}
						break;
				}
			}
		}
		return true;
	}
	return false;
}
bool CRegionTopoer::PolyJoined(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, WORD wLayer, DWORD dwPoly1, BYTE mode, DWORD dwPoly2, DWORD& dwEdgeId1, DWORD& dwEdgeId2)
{	
	if(CTopoer::PolyJoined(pWnd, datainfo, viewinfo, wLayer, dwPoly1, mode, dwPoly2, dwEdgeId1, dwEdgeId2) == true)
	{
		for(auto it = m_layonlist.begin(); it != m_layonlist.end(); it++)
		{
			CLayon* layon = *it;
			POSITION pos = layon->m_geomlist.GetTailPosition();
			while(pos != nullptr)
			{
				CGeom* pGeom = layon->m_geomlist.GetPrev(pos);
				switch(pGeom->Gettype())
				{
					case 51:
						{
							CPolygon* pPolygon = (CPolygon*)pGeom;
							pPolygon->EdgeMerged(dwEdgeId1, dwEdgeId2);
						}
						break;
					case 52:
						{
							CGonroup* pGonroup = (CGonroup*)pGeom;
							pGonroup->EdgeMerged(dwEdgeId1, dwEdgeId2);
						}
						break;
					case 61:
						{
							CPolying* pPolying = (CPolying*)pGeom;
							pPolying->EdgeMerged(dwEdgeId1, dwEdgeId2);
						}
						break;
					case 62:
						{
							CIngroup* pIngroup = (CIngroup*)pGeom;
							pIngroup->EdgeMerged(dwEdgeId1, dwEdgeId2);
						}
						break;
				}
			}
		}
		return true;
	}
	else
		return false;
}