#include "stdafx.h"
#include "RoadTopo.h"
#include "Probe.h"
#include "FilePoly.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Global.h"
#include "../Utility/Rect.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CRoadTopo, CObject, 0)

CRoadTopo::CRoadTopo()
{
	m_pNodes = nullptr;
	m_pEdges = nullptr;

	m_pNodeDate = nullptr;
	m_pPolyFile = nullptr;

	s_pNewlyPoly = nullptr;
	s_pCutedPoly1 = nullptr;
	s_pCutedPoly2 = nullptr;
	s_dwOlderEdge = -1;

	e_pNewlyPoly = nullptr;
	e_pCutedPoly1 = nullptr;
	e_pCutedPoly2 = nullptr;
	e_dwOlderEdge = -1;

	m_SizeNode = 0;
	m_dwEnterEdge = -1;
}

CRoadTopo::~CRoadTopo()
{
	delete[] m_pNodes;
	delete[] m_pEdges;
	delete m_pNodeDate;

	if(m_pPolyFile!=nullptr)
	{
		m_pPolyFile->Close();
		delete m_pPolyFile;
		m_pPolyFile = nullptr;
	}
}

CPoly* CRoadTopo::GetEdgePoly(const DWORD& dwEdge) const
{
	if(m_pPolyFile!=nullptr)
	{
		const DWORD indexPoly = m_pEdges[dwEdge].m_indexPoly;
		CPoly* poly = m_pPolyFile->GetPoly(indexPoly);
		poly->m_Rect = m_pEdges[dwEdge].m_Rect;
		return poly;
	}
	else
	{
		return nullptr;
	}
}

bool CRoadTopo::Load(CString strFile)
{
	CString strNode = strFile;
	strNode += _T(".Nod");
	CFile fileNode;
	if(fileNode.Open(strNode, CFile::modeRead|CFile::shareDenyWrite)==TRUE)
	{
		fileNode.Read(&m_nNodeCount, sizeof(DWORD));
		m_pNodes = new Node[m_nNodeCount+4];
		fileNode.Read(m_pNodes, m_nNodeCount*sizeof(Node));

		m_SizeNode = fileNode.GetLength()-m_nNodeCount*sizeof(Node);
		m_pNodeDate = new BYTE[m_SizeNode+200];
		fileNode.Read(m_pNodeDate, m_SizeNode);
		memset(m_pNodeDate+m_SizeNode, 0XFF, 200);

		m_pNodes[m_nNodeCount-1+1].m_nDimension = 1;
		m_pNodes[m_nNodeCount-1+2].m_nDimension = 3;
		m_pNodes[m_nNodeCount-1+3].m_nDimension = 1;
		m_pNodes[m_nNodeCount-1+4].m_nDimension = 3;

		m_pNodes[m_nNodeCount-1+1].m_dwPos = m_SizeNode;
		m_pNodes[m_nNodeCount-1+2].m_dwPos = m_pNodes[m_nNodeCount-1+1].m_dwPos+sizeof(DWORD)*1+sizeof(BYTE)*1;
		m_pNodes[m_nNodeCount-1+3].m_dwPos = m_pNodes[m_nNodeCount-1+2].m_dwPos+sizeof(DWORD)*3+sizeof(BYTE)*2;
		m_pNodes[m_nNodeCount-1+4].m_dwPos = m_pNodes[m_nNodeCount-1+3].m_dwPos+sizeof(DWORD)*1+sizeof(BYTE)*1;

		fileNode.Close();
	}
	else
	{
		return false;
	}

	CString strEdge = strFile;
	strEdge += _T(".Edg");
	CFile fileEdg;
	if(fileEdg.Open(strEdge, CFile::modeRead|CFile::shareDenyWrite)==TRUE)
	{
		fileEdg.Read(&m_nEdgeCount, sizeof(DWORD));
		m_pEdges = new Edge[m_nEdgeCount+6];
		fileEdg.Read(m_pEdges, sizeof(Edge)*m_nEdgeCount);

		for(int index = 1; index<=6; index++)
		{
			m_pEdges[m_nEdgeCount-1+index].m_Mode = 0X00;
			m_pEdges[m_nEdgeCount-1+index].m_designedSpeed = 20;
			m_pEdges[m_nEdgeCount-1+index].m_restrictSpeed = 20;
			m_pEdges[m_nEdgeCount-1+index].m_cashToll = 0;

			m_pEdges[m_nEdgeCount-1+index].m_indexPoly = 0;
			m_pEdges[m_nEdgeCount-1+index].m_fLength = 0.0f;
		}
		m_pEdges[m_nEdgeCount-1+1].m_indexFromNode = m_nNodeCount-1+1;
		m_pEdges[m_nEdgeCount-1+1].m_indexToNode = m_nNodeCount-1+2;

		m_pEdges[m_nEdgeCount-1+2].m_indexToNode = m_nNodeCount-1+2;
		m_pEdges[m_nEdgeCount-1+3].m_indexFromNode = m_nNodeCount-1+2;

		m_pEdges[m_nEdgeCount-1+4].m_indexFromNode = m_nNodeCount-1+3;
		m_pEdges[m_nEdgeCount-1+4].m_indexToNode = m_nNodeCount-1+4;

		m_pEdges[m_nEdgeCount-1+5].m_indexToNode = m_nNodeCount-1+4;
		m_pEdges[m_nEdgeCount-1+6].m_indexFromNode = m_nNodeCount-1+4;

		fileEdg.Close();
	}
	else
	{
		delete[] m_pNodes;
		m_pNodes = nullptr;
		return false;
	}

	CString strPoly = strFile;
	strPoly += _T(".ply");
	m_pPolyFile = new CFilePoly();
	if(m_pPolyFile->Open(strPoly)==true)
	{
	}
	else
	{
		delete m_pPolyFile;
		m_pPolyFile = nullptr;
	}
	return true;
}

DWORD CRoadTopo::CreatStartTousy(const CPointF& geoPoint)
{
	if(m_pNodeDate!=nullptr)
	{
		const DWORD dwInsertEdge1 = m_nEdgeCount-1+1;
		const DWORD dwInsertEdge2 = m_nEdgeCount-1+2;
		const DWORD dwInsertEdge3 = m_nEdgeCount-1+3;
		const DWORD dwInsertEdge4 = m_nEdgeCount-1+4;
		const DWORD dwInsertEdge5 = m_nEdgeCount-1+5;
		const DWORD dwInsertEdge6 = m_nEdgeCount-1+6;

		BYTE* p = m_pNodeDate+m_SizeNode;
		memcpy(p, &dwInsertEdge1, sizeof(DWORD));
		p = p+sizeof(DWORD);
		p = p+sizeof(BYTE)*1;
		memcpy(p, &dwInsertEdge1, sizeof(DWORD));
		p = p+sizeof(DWORD);
		memcpy(p, &dwInsertEdge2, sizeof(DWORD));
		p = p+sizeof(DWORD);
		memcpy(p, &dwInsertEdge3, sizeof(DWORD));
		p = p+sizeof(DWORD);
		p = p+sizeof(BYTE)*2;

		memcpy(p, &dwInsertEdge4, sizeof(DWORD));
		p = p+sizeof(DWORD);
		p = p+sizeof(BYTE)*1;
		memcpy(p, &dwInsertEdge4, sizeof(DWORD));
		p = p+sizeof(DWORD);
		memcpy(p, &dwInsertEdge5, sizeof(DWORD));
		p = p+sizeof(DWORD);
		memcpy(p, &dwInsertEdge6, sizeof(DWORD));
		p = p+sizeof(DWORD);
	}

	CPointF mapPoint;
	mapPoint.x = geoPoint.x*10000000;
	mapPoint.y = geoPoint.y*10000000;
	CPoint point;
	point.x = mapPoint.x;
	point.y = mapPoint.y;

	CPoint trend;
	const DWORD indexEdge = this->GetProximalEdge(point, trend);
	if(indexEdge==-1)
	{
		m_dwEnterEdge = -1;
		return this->GetProximalNode(point);
	}
	else
	{
		m_dwEnterEdge = indexEdge;
		CPoly* startPoly = this->GetEdgePoly(indexEdge);
		if(startPoly==nullptr)
			return -1;

		if(trend.x==startPoly->GetAnchor(1).x&&trend.y==startPoly->GetAnchor(1).y)
		{
			delete startPoly;
			return m_pEdges[indexEdge].m_indexFromNode;
		}
		else if(trend.x==startPoly->GetAnchor(startPoly->m_nAnchors).x&&trend.y==startPoly->GetAnchor(startPoly->m_nAnchors).y)
		{
			delete startPoly;
			return m_pEdges[indexEdge].m_indexToNode;
		}
		else
		{
			s_dwOlderEdge = indexEdge;//Ҫ��s_pCutedNode��forbiden����ȷ�е��趨

			s_pNewlyPoly = new CPoly();
			s_pNewlyPoly->AddAnchor(point);
			s_pNewlyPoly->AddAnchor(CPoint(trend.x, trend.y));

			m_pNodes[m_nNodeCount-1+1].m_Point = point;
			m_pNodes[m_nNodeCount-1+2].m_Point = CPoint(trend.x, trend.y);

			m_pEdges[m_nEdgeCount-1+2].m_indexFromNode = m_pEdges[indexEdge].m_indexFromNode;
			m_pEdges[m_nEdgeCount-1+3].m_indexToNode = m_pEdges[indexEdge].m_indexToNode;

			this->ReplaceNodeEdge(m_pEdges[indexEdge].m_indexFromNode, indexEdge, m_nEdgeCount-1+2);;
			this->ReplaceNodeEdge(m_pEdges[indexEdge].m_indexToNode, indexEdge, m_nEdgeCount-1+3);;

			unsigned int nAnchor = 0;
			double t = 0.0f;
			if(startPoly->GetOrthocentre(trend, nAnchor, t)==true)
			{
				s_pCutedPoly1 = startPoly->GetByFHTToTHT(1, 0.0f, nAnchor, t);
				s_pCutedPoly2 = startPoly->GetByFHTToTHT(nAnchor, t, startPoly->m_nAnchors, 0.0f);
				const double len0 = startPoly->GetLength(1000/2.f);
				const double len1 = s_pCutedPoly1->GetLength(1000/2.f);
				const double len2 = s_pCutedPoly2->GetLength(1000/2.f);
				m_pEdges[m_nEdgeCount-1+2].m_fLength = m_pEdges[indexEdge].m_fLength*len1/len0;
				m_pEdges[m_nEdgeCount-1+3].m_fLength = m_pEdges[indexEdge].m_fLength*len2/len0;
			}
			else
			{
				m_pEdges[m_nEdgeCount-1+2].m_fLength = m_pEdges[indexEdge].m_fLength;
				m_pEdges[m_nEdgeCount-1+3].m_fLength = m_pEdges[indexEdge].m_fLength;
			}

			if(this->CanNodeThruEdge(m_pEdges[indexEdge].m_indexFromNode, m_nEdgeCount-1+2)==false)
			{
				this->SetNodeForbid(m_nNodeCount-1+2, 1, 2, false);
				this->SetNodeForbid(m_nNodeCount-1+2, 1, 3, false);
				this->SetNodeForbid(m_nNodeCount-1+2, 3, 2, false);
			}
			if(this->CanNodeThruEdge(m_pEdges[indexEdge].m_indexToNode, m_nEdgeCount-1+3)==false)
			{
				this->SetNodeForbid(m_nNodeCount-1+2, 2, 1, false);
				this->SetNodeForbid(m_nNodeCount-1+2, 2, 3, false);
				this->SetNodeForbid(m_nNodeCount-1+2, 3, 1, false);
			}

			delete startPoly;
			return m_nNodeCount-1+1;
		}
	}
}

DWORD CRoadTopo::CreatStopTousy(const CPointF& geoPoint)
{
	CPointF mapPoint;
	mapPoint.x = geoPoint.x*10000000;
	mapPoint.y = geoPoint.y*10000000;
	CPoint point;
	point.x = mapPoint.x;
	point.y = mapPoint.y;

	CPoint trend;
	const DWORD indexEdge = this->GetProximalEdge(point, trend);
	if(indexEdge==-1)
	{
		return this->GetProximalNode(point);
	}
	else
	{
		CPoly* stopPoly = this->GetEdgePoly(indexEdge);
		if(stopPoly==nullptr)
			return -1;

		if(trend.x==stopPoly->GetAnchor(1).x&&trend.y==stopPoly->GetAnchor(1).y)
		{
			delete stopPoly;
			return m_pEdges[indexEdge].m_indexFromNode;
		}
		else if(trend.x==stopPoly->GetAnchor(stopPoly->m_nAnchors).x&&trend.y==stopPoly->GetAnchor(stopPoly->m_nAnchors).y)
		{
			delete stopPoly;
			return m_pEdges[indexEdge].m_indexToNode;
		}
		else
		{
			e_dwOlderEdge = indexEdge;

			e_pNewlyPoly = new CPoly();
			e_pNewlyPoly->AddAnchor(point);
			e_pNewlyPoly->AddAnchor(CPoint(trend.x, trend.y));

			m_pNodes[m_nNodeCount-1+3].m_Point = point;
			m_pNodes[m_nNodeCount-1+4].m_Point = CPoint(trend.x, trend.y);

			if(indexEdge==m_dwEnterEdge)
			{
				delete stopPoly;

				CPoint ortho1;
				CPoint ortho2;
				const double dis1 = s_pCutedPoly1->Shortcut(point, ortho1);
				const double dis2 = s_pCutedPoly2->Shortcut(point, ortho2);
				if(dis1<dis2)
				{
					m_pEdges[m_nEdgeCount-1+2].m_indexFromNode = m_nNodeCount-1+4;
					m_pEdges[m_nEdgeCount-1+5].m_indexFromNode = m_pEdges[indexEdge].m_indexFromNode;// dis1>dis2ʱ �߲�������m_nEdgeCount-1+5

					this->ReplaceNodeEdge(m_pEdges[indexEdge].m_indexFromNode, m_nEdgeCount-1+2, m_nEdgeCount-1+5);
					this->ReplaceNodeEdge(m_nNodeCount-1+4, m_nEdgeCount-1+6, m_nEdgeCount-1+2);

					unsigned int nAnchor = 0;
					double t = 0.0f;
					if(s_pCutedPoly1->GetOrthocentre(trend, nAnchor, t)==true)
					{
						e_pCutedPoly1 = s_pCutedPoly1->GetByFHTToTHT(1, 0.0f, nAnchor, t);
						e_pCutedPoly2 = s_pCutedPoly1->GetByFHTToTHT(nAnchor, t, s_pCutedPoly1->m_nAnchors, 0.0f);
						const double len0 = s_pCutedPoly1->GetLength(1000/2.f);
						const double len1 = e_pCutedPoly1->GetLength(1000/2.f);
						const double len2 = e_pCutedPoly2->GetLength(1000/2.f);
						m_pEdges[m_nEdgeCount-1+5].m_fLength = m_pEdges[m_nEdgeCount-1+2].m_fLength*len1/len0;
						m_pEdges[m_nEdgeCount-1+2].m_fLength = m_pEdges[m_nEdgeCount-1+2].m_fLength*len2/len0;

						delete s_pCutedPoly1;
						s_pCutedPoly1 = e_pCutedPoly2;
						e_pCutedPoly2 = nullptr;
					}
					else
					{
						m_pEdges[m_nEdgeCount-1+5].m_fLength = m_pEdges[m_nEdgeCount-1+2].m_fLength;
					}
				}
				else
				{
					m_pEdges[m_nEdgeCount-1+3].m_indexToNode = m_nNodeCount-1+4;
					m_pEdges[m_nEdgeCount-1+6].m_indexToNode = m_pEdges[indexEdge].m_indexToNode; // dis1<dis2ʱ �߲�������m_nEdgeCount-1+6

					this->ReplaceNodeEdge(m_pEdges[indexEdge].m_indexToNode, m_nEdgeCount-1+3, m_nEdgeCount-1+6);
					this->ReplaceNodeEdge(m_nNodeCount-1+4, m_nEdgeCount-1+5, m_nEdgeCount-1+3);

					unsigned int nAnchor = 0;
					double t = 0.0f;
					if(s_pCutedPoly2->GetOrthocentre(trend, nAnchor, t)==true)
					{
						e_pCutedPoly1 = s_pCutedPoly2->GetByFHTToTHT(1, 0.0f, nAnchor, t);
						e_pCutedPoly2 = s_pCutedPoly2->GetByFHTToTHT(nAnchor, t, s_pCutedPoly2->m_nAnchors, 0.0f);
						const double len0 = s_pCutedPoly2->GetLength(1000/2.f);
						const double len1 = e_pCutedPoly1->GetLength(1000/2.f);
						const double len2 = e_pCutedPoly2->GetLength(1000/2.f);
						m_pEdges[m_nEdgeCount-1+3].m_fLength = m_pEdges[m_nEdgeCount-1+3].m_fLength*len1/len0;
						m_pEdges[m_nEdgeCount-1+6].m_fLength = m_pEdges[m_nEdgeCount-1+3].m_fLength*len2/len0;

						delete s_pCutedPoly2;
						s_pCutedPoly2 = e_pCutedPoly1;
						e_pCutedPoly1 = nullptr;
					}
					else
					{
						m_pEdges[m_nEdgeCount-1+6].m_fLength = m_pEdges[m_nEdgeCount-1+3].m_fLength;
					}
				}
			}
			else
			{
				m_pEdges[m_nEdgeCount-1+5].m_indexFromNode = m_pEdges[indexEdge].m_indexFromNode;// dis1>dis2ʱ �߲�������m_nEdgeCount-1+5
				m_pEdges[m_nEdgeCount-1+6].m_indexToNode = m_pEdges[indexEdge].m_indexToNode; // dis1<dis2ʱ �߲�������m_nEdgeCount-1+6

				this->ReplaceNodeEdge(m_pEdges[indexEdge].m_indexFromNode, indexEdge, m_nEdgeCount-1+5);
				this->ReplaceNodeEdge(m_pEdges[indexEdge].m_indexToNode, indexEdge, m_nEdgeCount-1+6);

				if(this->CanNodeThruEdge(m_pEdges[indexEdge].m_indexFromNode, m_nEdgeCount-1+5)==false)
				{
					this->SetNodeForbid(m_nNodeCount-1+4, 1, 2, false);
					this->SetNodeForbid(m_nNodeCount-1+4, 1, 3, false);
					this->SetNodeForbid(m_nNodeCount-1+4, 3, 2, false);
				}
				if(this->CanNodeThruEdge(m_pEdges[indexEdge].m_indexToNode, m_nEdgeCount-1+6)==false)
				{
					this->SetNodeForbid(m_nNodeCount-1+4, 2, 1, false);
					this->SetNodeForbid(m_nNodeCount-1+4, 2, 3, false);
					this->SetNodeForbid(m_nNodeCount-1+4, 3, 1, false);
				}

				unsigned int nAnchor = 0;
				double t = 0.0f;
				if(stopPoly->GetOrthocentre(trend, nAnchor, t)==true)
				{
					e_pCutedPoly1 = stopPoly->GetByFHTToTHT(1, 0.0f, nAnchor, t);
					e_pCutedPoly2 = stopPoly->GetByFHTToTHT(nAnchor, t, stopPoly->m_nAnchors, 0.0f);
					const double len0 = stopPoly->GetLength(1000/2.f);
					const double len1 = e_pCutedPoly1->GetLength(1000/2.f);
					const double len2 = e_pCutedPoly2->GetLength(1000/2.f);
					m_pEdges[m_nEdgeCount-1+5].m_fLength = m_pEdges[indexEdge].m_fLength*len1/len0;
					m_pEdges[m_nEdgeCount-1+6].m_fLength = m_pEdges[indexEdge].m_fLength*len2/len0;
				}
				else
				{
					m_pEdges[m_nEdgeCount-1+5].m_fLength = m_pEdges[indexEdge].m_fLength;
					m_pEdges[m_nEdgeCount-1+6].m_fLength = m_pEdges[indexEdge].m_fLength;
				}

				delete stopPoly;
			}

			return m_nNodeCount-1+3;
		}
	}
}

DWORD CRoadTopo::GetProximalEdge(const CPoint& point, CPoint& trend) const
{
	double minimum = 999999999999999.0f;
	for(DWORD index = 0; index<m_nNodeCount; index++)
	{
		const double dx = m_pNodes[index].m_Point.x-point.x;
		const double dy = m_pNodes[index].m_Point.y-point.y;
		const double dis = dx*dx+dy*dy;
		if(dis<minimum)
		{
			minimum = dis;
		}
	}

	minimum = sqrt(minimum);
	CRect inRect = CRect(point.x, point.y, point.x, point.y);
	inRect.InflateRect(minimum+1, minimum+1);

	DWORD dwMiniEdge = -1;
	for(DWORD indexEdge = 0; indexEdge<m_nEdgeCount; indexEdge++)
	{
		CRect rect = m_pEdges[indexEdge].m_Rect;
		if(Util::Rect::Intersect(rect, inRect)==false)
			continue;

		CPoly* poly = this->GetEdgePoly(indexEdge);
		CPoint ortho;
		const double dis = poly->Shortcut(point, ortho);
		if(dis<minimum)
		{
			trend = ortho;
			minimum = dis;
			dwMiniEdge = indexEdge;
		}
		delete poly;
	}

	return dwMiniEdge;
}

DWORD CRoadTopo::GetProximalNode(const CPoint& point) const
{
	DWORD dwMiniNode = -1;
	double minimum = 999999999999999.0f;
	for(DWORD index = 0; index<m_nNodeCount; index++)
	{
		const double dx = m_pNodes[index].m_Point.x-point.x;
		const double dy = m_pNodes[index].m_Point.y-point.y;
		const double dis = sqrt(dx*dx+dy*dy);
		if(dis<minimum)
		{
			minimum = dis;
			dwMiniNode = index;
		}
	}

	return dwMiniNode;
}

void CRoadTopo::Release()
{
	delete s_pNewlyPoly;
	s_pNewlyPoly = nullptr;
	delete s_pCutedPoly1;
	s_pCutedPoly1 = nullptr;
	delete s_pCutedPoly2;
	s_pCutedPoly2 = nullptr;

	delete e_pNewlyPoly;
	e_pNewlyPoly = nullptr;
	delete e_pCutedPoly1;
	e_pCutedPoly1 = nullptr;
	delete e_pCutedPoly2;
	e_pCutedPoly2 = nullptr;

	if(s_dwOlderEdge!=-1)
	{
		this->ReplaceNodeEdge(m_pEdges[s_dwOlderEdge].m_indexFromNode, m_nEdgeCount-1+2, s_dwOlderEdge);
		this->ReplaceNodeEdge(m_pEdges[s_dwOlderEdge].m_indexFromNode, m_nEdgeCount-1+5, s_dwOlderEdge);
		this->ReplaceNodeEdge(m_pEdges[s_dwOlderEdge].m_indexToNode, m_nEdgeCount-1+3, s_dwOlderEdge);
		this->ReplaceNodeEdge(m_pEdges[s_dwOlderEdge].m_indexToNode, m_nEdgeCount-1+6, s_dwOlderEdge);

		s_dwOlderEdge = -1;
	}

	if(e_dwOlderEdge!=-1)
	{
		this->ReplaceNodeEdge(m_pEdges[e_dwOlderEdge].m_indexFromNode, m_nEdgeCount-1+2, e_dwOlderEdge);
		this->ReplaceNodeEdge(m_pEdges[e_dwOlderEdge].m_indexFromNode, m_nEdgeCount-1+5, e_dwOlderEdge);
		this->ReplaceNodeEdge(m_pEdges[e_dwOlderEdge].m_indexToNode, m_nEdgeCount-1+3, e_dwOlderEdge);
		this->ReplaceNodeEdge(m_pEdges[e_dwOlderEdge].m_indexToNode, m_nEdgeCount-1+6, e_dwOlderEdge);

		e_dwOlderEdge = -1;
	}
}

Probe* CRoadTopo::Dijkstra(DWORD dwHeadNode, DWORD dwTailNode)
{
	if(dwHeadNode==-1)
		return nullptr;
	else if(dwTailNode==-1)
		return nullptr;
	else if(dwHeadNode==dwTailNode)
		return nullptr;

	Probe* pProbe = new Probe[m_nNodeCount+4];
	for(DWORD index = 0; index<m_nNodeCount+4; index++)
	{
		pProbe[index].dwNodeIndex = index;
		pProbe[index].m_bUsed = false;
		pProbe[index].m_dShortestLength = -1.0f;
		pProbe[index].m_dwPreShortestEdge = -1;
		//	pProbe[index].m_edifyToHead       = -1.0f;
		//	pProbe[index].m_edifyToTail       = -1.0f;
	}

	pProbe[dwHeadNode].m_bUsed = true;
	pProbe[dwHeadNode].m_dShortestLength = 0.0f;
	//	pProbe[dwHeadNode].m_edifyToHead       = 0.0f;
	const int nPriority = AfxGetApp()->GetProfileInt(_T("Navigate"), _T("Priority"), 0);
	const CString strOilPrice = AfxGetApp()->GetProfileString(_T("Navigate"), _T("OilPrice"), _T("3.2"));
	const CString strOilLitre = AfxGetApp()->GetProfileString(_T("Navigate"), _T("OilLitre"), _T("10"));
	const CString strExpressPrice = AfxGetApp()->GetProfileString(_T("Navigate"), _T("ExpressPrice"), _T("0.5"));

	float fOilLitre = _tcstod(strOilLitre, nullptr);
	float fOilPrice = _tcstod(strOilPrice, nullptr);
	float fExpressPrice = _tcstod(strExpressPrice, nullptr);

	DWORD total = 0;
	CList<DWORD, DWORD&> grayNodes;//��Żҵ㼯
	DWORD minNode = dwHeadNode;
	do
	{
		const BYTE fromIndex = this->GetNodeEdgeIndex(minNode, pProbe[minNode].m_dwPreShortestEdge);
		const BYTE nEdgeCount = m_pNodes[minNode].m_nDimension;
		for(BYTE index = 0; index<nEdgeCount; index++)
		{
			if(index==fromIndex)
				continue;
			const DWORD dwEdge = this->GetNodeEdge(minNode, index);
			DWORD dwNextNode = m_pEdges[dwEdge].m_indexFromNode==minNode ? m_pEdges[dwEdge].m_indexToNode : m_pEdges[dwEdge].m_indexFromNode;
			if(pProbe[dwNextNode].m_bUsed==true)//�Ѿ��Ǻ��
				continue;

			if(pProbe[minNode].m_dwPreShortestEdge!=-1)
			{
				if(this->GetNodeForbid(minNode, fromIndex, index)==false)//�ڸĽڵ㴦�޷�ͨ��ù�����ʱ(��ֹ����)�����������
					continue;
			}
			else if(this->CanNodeThruEdge(minNode, dwEdge)==false)//�Ӹýڵ㵽����dwEdge���ǵ����ߣ����������		
				continue;

			float length = m_pEdges[dwEdge].m_fLength;
			switch(nPriority)
			{
			case 0: //�ٶ�����
				length = m_pEdges[dwEdge].m_fLength/(m_pEdges[dwEdge].m_designedSpeed);
				break;
			case 1: //��������
				length = m_pEdges[dwEdge].m_fLength;
				break;
			case 2: //��������
				//				length = (m_pEdges[dwEdge].m_fLength/100)*fOilLitre*fOilPrice + (m_pEdges[dwEdge].IsCharge() == true ? m_pEdges[dwEdge].m_fLength*fExpressPrice : 0);
				break;
			default:
				break;
			}

			if(pProbe[dwNextNode].m_dShortestLength==-1.0f)//�õ㻹��δ��������
			{
				pProbe[dwNextNode].m_dShortestLength = pProbe[minNode].m_dShortestLength+length;
				pProbe[dwNextNode].m_dwPreShortestEdge = dwEdge;
				grayNodes.AddTail(dwNextNode);
			}
			else if(pProbe[dwNextNode].m_dShortestLength>(pProbe[minNode].m_dShortestLength+length))//�Ӹõ㵽������������֮
			{
				pProbe[dwNextNode].m_dShortestLength = pProbe[minNode].m_dShortestLength+length;
				pProbe[dwNextNode].m_dwPreShortestEdge = dwEdge;
			}
			else
			{
			}
		}

		minNode = -1;
		const DWORD tick1 = GetTickCount();
		float miniLength = 999999999.0f;
		POSITION miniPos = nullptr;
		POSITION pos1 = grayNodes.GetHeadPosition();
		POSITION pos2;
		while((pos2 = pos1)!=nullptr)
		{
			const DWORD dwNode = grayNodes.GetNext(pos1);

			if(minNode==-1)
			{
				miniLength = pProbe[dwNode].m_dShortestLength;
				minNode = dwNode;
				miniPos = pos2;
			}
			else if(pProbe[dwNode].m_dShortestLength<miniLength)
			{
				miniLength = pProbe[dwNode].m_dShortestLength;
				minNode = dwNode;
				miniPos = pos2;
			}
		}

		if(minNode==-1)
		{
			delete[] pProbe;
			return nullptr;
		}
		else
		{
			pProbe[minNode].m_bUsed = true;
			grayNodes.RemoveAt(miniPos);
		}
		const DWORD tick2 = GetTickCount();
		total += (tick2-tick1);
	} while(minNode!=dwTailNode);

	grayNodes.RemoveAll();

	CString str;
	str.Format(_T("������Сֵ �ܹ����ѵ�ʱ�䣺 %d ��\n"), total/1000);
	OutputDebugString(str);

	return pProbe;
}

void CRoadTopo::ReplaceNodeEdge(DWORD dwNode, DWORD dwId1, DWORD dwId2) const
{
	DWORD* pDWORD = (DWORD*)(m_pNodeDate+m_pNodes[dwNode].m_dwPos);
	for(BYTE index = 0; index<m_pNodes[dwNode].m_nDimension; index++)
	{
		const DWORD dwId = *(pDWORD+index);
		if(dwId==dwId1)
		{
			*(pDWORD+index) = dwId2;
			break;
		}
	}
}

bool CRoadTopo::GetNodeForbid(DWORD dwNode, BYTE bRow, BYTE nCol) const
{
	const BYTE* pArray = m_pNodeDate+m_pNodes[dwNode].m_dwPos+m_pNodes[dwNode].m_nDimension*sizeof(DWORD);
	const int nPos = bRow*m_pNodes[dwNode].m_nDimension+nCol;
	const int nByte = nPos/8;
	const int nBit = 7-nPos%8;
	BYTE byte = pArray[nByte];
	byte = (byte>>nBit);
	if((byte&0X01)==0X01)
		return true;
	else
		return false;
}

void CRoadTopo::SetNodeForbid(DWORD dwNode, BYTE bRow, BYTE nCol, bool bValue) const
{
	BYTE* pArray = m_pNodeDate+m_pNodes[dwNode].m_dwPos+m_pNodes[dwNode].m_nDimension*sizeof(DWORD);
	const int nPos = bRow*m_pNodes[dwNode].m_nDimension+nCol;
	const int nByte = nPos/8;
	const int nBit = 7-nPos%8;
	BYTE byte = 0X01;
	byte = (byte<<nBit);
	if(bValue==true)
	{
		pArray[nByte] |= byte;
	}
	else
	{
		pArray[nByte] &= ~byte;
	}
}

bool CRoadTopo::CanNodeThruEdge(DWORD dwNode, const DWORD& dwEdge)
{
	if(m_pNodes[dwNode].m_nDimension==1)
		return true;
	const BYTE index = this->GetNodeEdgeIndex(dwNode, dwEdge);
	for(BYTE i = 0; i<m_pNodes[dwNode].m_nDimension; i++)
	{
		if(this->GetNodeForbid(dwNode, i, index)==true)
			return true;
	}

	return false;
}

bool CRoadTopo::CanNodeEdgeThru(DWORD dwNode, const DWORD& dwEdge)
{
	const BYTE index = this->GetNodeEdgeIndex(dwNode, dwEdge);
	for(BYTE i = 0; i<m_pNodes[dwNode].m_nDimension; i++)
	{
		if(i==index)
			continue;

		if(this->GetNodeForbid(dwNode, index, i)==true)
			return true;
	}

	return false;
}

DWORD CRoadTopo::GetNodeEdge(DWORD dwNode, BYTE index) const
{
	if(index>=m_pNodes[dwNode].m_nDimension)
		return -1;
	else
	{
		BYTE* pByte = m_pNodeDate+m_pNodes[dwNode].m_dwPos;
		DWORD* pDWORD = (DWORD*)pByte;
		return *(pDWORD+index);
	}
}

BYTE CRoadTopo::GetNodeEdgeIndex(DWORD dwNode, const DWORD& dwEdge) const
{
	DWORD* pDWORD = (DWORD*)(m_pNodeDate+m_pNodes[dwNode].m_dwPos);
	for(BYTE index = 0; index<m_pNodes[dwNode].m_nDimension; index++)
	{
		const DWORD dwId = *(pDWORD+index);
		if(dwId==dwEdge)
			return index;
	}

	return -1;
}

/*
//				pProbe[dwNextNode].m_edifyToHead = pProbe[minNode].m_edifyToHead + edify;
//				pProbe[dwNextNode].m_edifyToHead = pProbe[minNode].m_edifyToHead + edify;

//	double dx = m_pNodes[minNode].m_Point.x - m_pNodes[dwNextNode].m_Point.x;
		//	double dy = m_pNodes[minNode].m_Point.y - m_pNodes[dwNextNode].m_Point.y;
		//	double edify = sqrt(dx*dx+dy*dy)/(m_pEdges[dwEdge].m_designedSpeed);

//��������㷨���ٶȣ���������70������ڵ�ĵ�ͼ��˵���������飬�ٶ�û�����̫�࣬������ʱ���ã��Ƚ�����ͼ�������ޱȴ�ʱ�ٲ���
			if(pProbe[index].m_edifyToTail == -1.0f)
			{
				double dx = m_pNodes[dwNode].m_Point.x - m_pNodes[dwTailNode].m_Point.x;
				double dy = m_pNodes[dwNode].m_Point.y - m_pNodes[dwTailNode].m_Point.y;
				pProbe[dwNode].m_edifyToTail  = sqrt(dx*dx+dy*dy)/80;//Ӧ����ʵ��ֵ�����㣬���׼ȷһЩ	edify /= 80;//80�����ֵԽС���ж�Խ��������ֵ�������ĸ��Ǻ�㣬���ٶ�Խ��
			}

			if(minNode == -1)
			{
				miniEdify = pProbe[dwNode].m_edifyToHead + pProbe[dwNode].m_edifyToTail;
				minNode   = dwNode;
				pos       = index;
			}
			else if(pProbe[dwNode].m_edifyToHead+pProbe[dwNode].m_edifyToTail<miniEdify)
			{
				miniEdify = pProbe[dwNode].m_edifyToHead + pProbe[dwNode].m_edifyToTail;
				minNode   = dwNode;
				pos       = index;
			}

			else
		{
			nodearray.RemoveAt(pos);
			pProbe[minNode].m_bUsed = true;
		}

*/
