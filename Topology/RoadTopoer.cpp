#include "stdafx.h"
#include "Topo.h"
#include "Topoer.h"
#include "RoadTopoer.h"

#include "Node.h"
#include "RoadNode.h"
#include "Edge.h"
#include "RoadEdge.h"
#include "RoadNodeForm.h"
#include "RoadEdgeForm.h"
#include "../Dataview/DataInfo.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Mapper/Global.h"
#include "../Public/Global.h"
#include "../Public/PropertyPane.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRoadTopoer::CRoadTopoer(CLayerTree& layertree)
	: CTopoer(layertree)
{
}

CRoadTopoer::~CRoadTopoer()
{
}

CNode* CRoadTopoer::ApplyNode() const
{
	CNode* pNode = new CRoadNode();
	pNode->m_dwId = this->ApplyNodeId();
	pNode->m_bModified = true;
	return pNode;
}

CEdge* CRoadTopoer::ApplyEdge() const
{
	CEdge* pEdge = new CRoadEdge();
	pEdge->m_dwId = this->ApplyEdgeId();
	pEdge->m_bModified = true;
	return pEdge;
}

void CRoadTopoer::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CTopology::Serialize(ar, dwVersion);
}

void CRoadTopoer::ActivateNode(CWnd* pWnd, const CViewinfo& viewinfo, CNode* pNode) const
{
	CTopology::ActivateNode(pWnd, viewinfo, pNode);

	CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
	if(pPropertyPane!=nullptr)
	{
		pPropertyPane->ActivateTab(_T('N'));
		CNodeForm* pNodeDlg = (CNodeForm*)pPropertyPane->GetTabWnd('N');
		if(pNodeDlg!=nullptr)
		{
			pNodeDlg->SetNode(this, pNode);
		}
	}
}

void CRoadTopoer::ActivateEdge(CWnd* pWnd, const CViewinfo& viewinfo, CEdge* pEdge) const
{
	CTopoer::ActivateEdge(pWnd, viewinfo, pEdge);

	CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
	if(pPropertyPane!=nullptr)
	{
		pPropertyPane->ActivateTab(_T('E'));
		CEdgeForm* pEdgeDlg = (CEdgeForm*)pPropertyPane->GetTabWnd('E');
		if(pEdgeDlg!=nullptr)
		{
			pEdgeDlg->SetEdge(this, pEdge);
		}
	}
}

void CRoadTopoer::InactivateAll(CWnd* pWnd, const CViewinfo& viewinfo) const
{
	CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
	if(pPropertyPane!=nullptr)
	{
		CEdgeForm* pEdgeDlg = (CEdgeForm*)pPropertyPane->GetTabWnd('E');
		if(pEdgeDlg!=nullptr)
		{
			pEdgeDlg->SetEdge(nullptr, nullptr);
		}
		CNodeForm* pNodeDlg = (CNodeForm*)pPropertyPane->GetTabWnd('N');
		if(pNodeDlg!=nullptr)
		{
			pNodeDlg->SetNode(nullptr, nullptr);
		}
	}

	CTopology::InactivateAll(pWnd, viewinfo);
}

void CRoadTopoer::BeforeDisposeNode(CNode* pNode, const CDatainfo& datainfo)
{
	CTopology::BeforeDisposeNode(pNode, datainfo);

	CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
	if(pPropertyPane!=nullptr)
	{
		CNodeForm* pNodeDlg = (CNodeForm*)pPropertyPane->GetTabWnd('N');
		if(pNodeDlg!=nullptr&&(CTopology*)pNodeDlg->m_pTopo==this&&pNodeDlg->m_pNode==pNode)
		{
			pNodeDlg->m_pNode = nullptr;
			pNodeDlg->SetNode(nullptr, nullptr);
		}
	}
}

void CRoadTopoer::BeforeDisposeEdge(CEdge* pEdge, const CDatainfo& datainfo)
{
	CTopology::BeforeDisposeEdge(pEdge, datainfo);

	CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
	if(pPropertyPane!=nullptr)
	{
		CEdgeForm* pEdgeDlg = (CEdgeForm*)pPropertyPane->GetTabWnd('E');
		if(pEdgeDlg!=nullptr&&(CTopology*)pEdgeDlg->m_pTopo==this&&pEdgeDlg->m_pEdge==pEdge)
		{
			pEdgeDlg->m_pEdge = nullptr;
			pEdgeDlg->SetEdge(nullptr, nullptr);
		}

		CNodeForm* pNodeDlg = (CNodeForm*)pPropertyPane->GetTabWnd('N');
		if(pNodeDlg!=nullptr&&(CTopology*)pNodeDlg->m_pTopo==this)
		{
			CNode* pFNode = this->GetNode(pEdge->m_dwFromNode);
			if(pNodeDlg->m_pNode==pFNode)
			{
				pNodeDlg->Invalidate();
			}
			CNode* pTNode = this->GetNode(pEdge->m_dwToNode);
			if(pNodeDlg->m_pNode==pTNode)
			{
				pNodeDlg->Invalidate();
			}
		}
	}
}

void CRoadTopoer::ExportPC(CString strFile, const CDatainfo& datainfo)
{
	CString strNode = strFile;
	CString strEdge = strFile;
	CString strPoly = strFile;
	strNode += _T(".Nod");
	strEdge += _T(".Edg");
	strPoly += _T(".Ply");
	CFile fileNode;
	CFile fileEdge;
	CFile filePoly;

	if(fileNode.Open(strNode, CFile::modeCreate|CFile::modeWrite)==TRUE&&fileEdge.Open(strEdge, CFile::modeCreate|CFile::modeWrite)==TRUE&&filePoly.Open(strPoly, CFile::modeCreate|CFile::modeWrite)==TRUE)
	{
		DWORD dwNodeCount = m_mapIdNode.size();
		DWORD dwEdgeCount = m_mapIdEdge.size();
		DWORD dwPolyCount = m_mapIdEdge.size();

		fileNode.Write(&dwNodeCount, sizeof(DWORD));
		fileEdge.Write(&dwEdgeCount, sizeof(DWORD));
		filePoly.Write(&dwPolyCount, sizeof(DWORD));

		std::map<DWORD, DWORD> mapIdtoIndexNode;
		std::map<DWORD, DWORD> mapIdtoIndexEdge;
		std::map<DWORD, DWORD> mapIdtoIndexPoly;

		try
		{
			CFile fileAddress;
			CFile fileContent;
			if(fileAddress.Open(_T("Poly.Add"), CFile::modeCreate|CFile::modeWrite)==FALSE||fileContent.Open(_T("Poly.Con"), CFile::modeCreate|CFile::modeWrite)==FALSE)
			{
				AfxMessageBox(_T("创建Poly的临时文件有误!"));
				return;
			}

			DWORD polyIndex = 0;
			DWORD position = 0;

			for(CEdgeMap::iterator it = m_mapIdEdge.begin(); it!=m_mapIdEdge.end(); it++)
			{
				CRoadEdge* pEdge = (CRoadEdge*)it->second;
				CPoly* pPoly = this->GetEdgePoly(pEdge->m_dwId);
				if(pPoly!=nullptr)
				{
					mapIdtoIndexPoly[pPoly->m_geoId] = polyIndex++;

					fileAddress.Write(&position, sizeof(DWORD));
					fileContent.Write(&pPoly->m_nAnchors, sizeof(unsigned int));

					CPoly* poly = (CPoly*)pPoly->Clone();
					for(unsigned int nAnchor = 1; nAnchor<=poly->m_nAnchors; nAnchor++)
					{
						CPoint point = poly->GetAnchor(nAnchor);
						CPoint docPoint = point;
						CPointF geoPoint = datainfo.DocToGeo(docPoint);

						point.x = geoPoint.x*10000000;
						point.y = geoPoint.y*10000000;

						poly->m_pPoints[nAnchor-1] = point;
					}
					fileContent.Write(poly->m_pPoints, sizeof(CPoint)*poly->m_nAnchors);
					position += sizeof(unsigned int)+poly->m_nAnchors*sizeof(CPoint);

					poly->RecalRect();
					pEdge->m_Rect = poly->m_Rect;

					if(pEdge->m_fLength<=0.0f)
					{
					}

					delete poly;
				}
				else
				{
					AfxMessageBox(_T("导出Topo中，读取Poly表存在错误!"));
				}
			}

			fileAddress.Close();
			fileContent.Close();
			CFileException fe;
			if(fileAddress.Open(_T("Poly.Add"), CFile::modeRead|CFile::shareDenyWrite, &fe)!=0&&fileContent.Open(_T("Poly.Con"), CFile::modeRead|CFile::shareDenyWrite, &fe)!=0)
			{
				DWORD lengthAddress = fileAddress.GetLength();
				DWORD lengthContent = fileContent.GetLength();

				BYTE* pAddressData = (BYTE*)malloc(lengthAddress);
				BYTE* pContentData = (BYTE*)malloc(lengthContent);

				fileAddress.Read(pAddressData, lengthAddress);
				fileContent.Read(pContentData, lengthContent);

				filePoly.Write(pAddressData, lengthAddress);
				filePoly.Write(pContentData, lengthContent);
			}
			fileAddress.Close();
			fileContent.Close();

			::DeleteFile(_T("Poly.Add"));
			::DeleteFile(_T("Poly.Con"));
		}
		catch(CDBException*ex)
		{
			//	AfxMessageBox(ex->m_strError);
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}

		try
		{
			DWORD nodeIndex = 0;
			DWORD position = 0;

			for(CNodeMap::iterator it = m_mapIdNode.begin(); it!=m_mapIdNode.end(); it++)
			{
				CNode* pNode = (CNode*)it->second;
				if(pNode!=nullptr)
				{
					mapIdtoIndexNode[pNode->m_dwId] = nodeIndex++;

					CPoint point = pNode->m_Point;
					CPoint docPoint = point;
					CPointF geoPoint = datainfo.DocToGeo(docPoint);;
					pNode->m_Point.x = geoPoint.x*10000000;
					pNode->m_Point.y = geoPoint.y*10000000;

					pNode->ExportPC1(fileNode, position);

					pNode->m_Point = point;
				}
				else
				{
					AfxMessageBox(_T("导出Topo中，读取Node表存在错误!"));
				}
			}
		}
		catch(CDBException*ex)
		{
			//	AfxMessageBox(ex->m_strError);
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}

		try
		{
			DWORD edgeIndex = 0;
			for(CEdgeMap::iterator it = m_mapIdEdge.begin(); it!=m_mapIdEdge.end(); it++)
			{
				CEdge* pEdge = (CEdge*)it->second;
				if(pEdge!=nullptr)
				{
					mapIdtoIndexEdge[pEdge->m_dwId] = edgeIndex++;
					pEdge->ExportPC(mapIdtoIndexNode, mapIdtoIndexPoly, fileEdge);
				}
				else
				{
					AfxMessageBox(_T("导出Topo中，读取Edge表存在错误!"));
				}
			}

			mapIdtoIndexNode.empty();
			mapIdtoIndexPoly.empty();
		}
		catch(CDBException*ex)
		{
			//	AfxMessageBox(ex->m_strError);
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}

		try
		{
			for(CNodeMap::iterator it = m_mapIdNode.begin(); it!=m_mapIdNode.end(); it++)
			{
				CNode* pNode = (CNode*)it->second;
				if(pNode!=nullptr)
				{
					pNode->ExportPC2(mapIdtoIndexEdge, fileNode);
				}
				else
				{
					AfxMessageBox(_T("导出Topo中，读取Node表存在错误!"));
				}
			}

			mapIdtoIndexEdge.empty();
		}
		catch(CDBException*ex)
		{
			//	AfxMessageBox(ex->m_strError);
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}

		mapIdtoIndexNode.empty();
		mapIdtoIndexEdge.empty();
		mapIdtoIndexPoly.empty();

		fileNode.Close();
		fileEdge.Close();
		filePoly.Close();

		for(CEdgeMap::iterator it = m_mapIdEdge.begin(); it!=m_mapIdEdge.end(); it++)
		{
			CRoadEdge* pEdge = (CRoadEdge*)it->second;
			CPoly* pPoly = this->GetEdgePoly(pEdge->m_dwId);
			if(pPoly!=nullptr)
			{
				pEdge->m_Rect = pPoly->m_Rect;
			}
		}
	}
}
