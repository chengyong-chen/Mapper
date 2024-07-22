#include "stdafx.h"
#include "Topodb.h"

#include "Node.h"
#include "RoadNode.h"
#include "RoadEdge.h"

#include "TopoTableDlg.h"
#include "../Dataview/DataInfo.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Database/Database.h"
#include "../Database/ODBCRecordset.h"
#include "../Public/Global.h"

using namespace std;

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTopodb::CTopodb()
{
	m_colorEdge.SetRGB(0, 0, 255);
	m_colorNode.SetRGB(0, 0, 0);

	m_pDatabase = nullptr;
}

CTopodb::CTopodb(CDatabase* pDatabase)
{
	m_colorEdge.SetRGB(0, 0, 255);
	m_colorNode.SetRGB(0, 0, 0);

	m_pDatabase = pDatabase;
}

CTopodb::~CTopodb()
{
}

DWORD CTopodb::ApplyEdgeId() const
{
	return 0;
}

DWORD CTopodb::ApplyNodeId() const
{
	return 0;
}

CPoly* CTopodb::GetEdgePoly(const DWORD& dwEdgeId) const
{
	CEdge* pEdge = this->GetEdge(dwEdgeId);
	if(pEdge == nullptr)
		return nullptr;
	else
		return pEdge->m_pPoly;
}

void CTopodb::Serialize(CArchive& ar, const DWORD& dwVersion)
{
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
}

void CTopodb::ExportPC(CString strFile, const CDatainfo& datainfo)
{
	if(m_pDatabase == nullptr)
		return;

	CString strNode = strFile;
	CString strEdge = strFile;
	CString strPoly = strFile;
	strNode += _T(".Nod");
	strEdge += _T(".Edg");
	strPoly += _T(".Ply");
	CFile fileNode;
	CFile fileEdge;
	CFile filePoly;

	if(fileNode.Open(strNode, CFile::modeCreate | CFile::modeWrite) == TRUE && fileEdge.Open(strEdge, CFile::modeCreate | CFile::modeWrite) == TRUE && filePoly.Open(strPoly, CFile::modeCreate | CFile::modeWrite) == TRUE)
	{
		DWORD dwNodeCount = 0;
		DWORD dwEdgeCount = 0;
		DWORD dwPolyCount = 0;

		try
		{
			CString strSQL;
			strSQL.Format(_T("Select count(*) as NodeCount From %s"), m_tableNode);
			if(m_strFilter.IsEmpty() == FALSE)
			{
				strSQL += _T(" Where ") + m_strFilter;
			}

			CODBCRecordset rs(m_pDatabase);
			rs.Open(strSQL);
			if(!rs.IsEOF())
			{
				dwNodeCount = rs.Field(_T("NodeCount")).AsInt();
			}
			rs.Close();
		}
		catch(CDBException* ex)
		{
			//	AfxMessageBox(ex->m_strError);
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}

		try
		{
			CString strSQL;
			strSQL.Format(_T("Select count(*) as EdgeCount From %s"), m_tableEdge);
			if(m_strFilter.IsEmpty() == FALSE)
			{
				strSQL += _T(" Where ") + m_strFilter;
			}

			CODBCRecordset rs(m_pDatabase);
			rs.Open(strSQL);
			if(!rs.IsEOF())
			{
				dwEdgeCount = rs.Field(_T("EdgeCount")).AsInt();
			}
			rs.Close();
		}
		catch(CDBException* ex)
		{
			//	AfxMessageBox(ex->m_strError);
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}

		try
		{
			CString strSQL;
			strSQL.Format(_T("Select count(*) as PolyCount From %s"), m_tablePolyData);
			_variant_t varRowsAff;
			CODBCRecordset rs(m_pDatabase);
			rs.Open(strSQL);
			if(!rs.IsEOF())
			{
				dwPolyCount = rs.Field(_T("PolyCount")).AsInt();
			}
			rs.Close();
		}
		catch(CDBException* ex)
		{
			//	AfxMessageBox(ex->m_strError);
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}

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
			if(fileAddress.Open(_T("Poly.Add"), CFile::modeCreate | CFile::modeWrite) == FALSE || fileContent.Open(_T("Poly.Con"), CFile::modeCreate | CFile::modeWrite) == FALSE)
			{
				AfxMessageBox(_T("创建Poly的临时文件有误!"));
				return;
			}

			DWORD polyIndex = 0;
			DWORD position = 0;

			CString strTag;
			CString strSQL;
			strSQL.Format(_T("Select * From %s order by ID"), m_tablePolyData);
			CODBCRecordset rs(m_pDatabase);
			rs.Open(strSQL);
			while(!rs.IsEOF())
			{
				CPoly* poly = new CPoly();
				if(poly->GetValues(rs, datainfo, strTag) == true)
				{
					mapIdtoIndexPoly[poly->m_geoId] = polyIndex++;

					fileAddress.Write(&position, sizeof(DWORD));

					fileContent.Write(&poly->m_nAnchors, sizeof(unsigned short));
					fileContent.Write(poly->m_pPoints, sizeof(CPoint)*poly->m_nAnchors);

					position += sizeof(unsigned short) + poly->m_nAnchors*sizeof(CPoint);
				}
				else
				{
					AfxMessageBox(_T("导出Topo中，读取Poly表存在错误!"));
				}

				delete poly;
				poly = nullptr;

				rs.MoveNext();
			}
			rs.Close();

			fileAddress.Close();
			fileContent.Close();
			CFileException fe;
			if(fileAddress.Open(_T("Poly.Add"), CFile::modeRead | CFile::shareDenyWrite, &fe) != 0 && fileContent.Open(_T("Poly.Con"), CFile::modeRead | CFile::shareDenyWrite, &fe) != 0)
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
		catch(CDBException* ex)
		{
			//	AfxMessageBox(ex->m_strError);
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}

		try
		{
			DWORD nodeIndex = 0;
			DWORD position = 0;

			CString strSQL;
			strSQL.Format(_T("Select * From %s"), m_tableNode);
			if(m_strFilter.IsEmpty() == FALSE)
			{
				strSQL += _T(" Where ") + m_strFilter;
			}
			strSQL += _T("order by ID");

			CODBCRecordset rs(m_pDatabase);
			rs.Open(strSQL);
			while(!rs.IsEOF())
			{
				CRoadNode* pNode = new CRoadNode();
				if(pNode->GetValues(rs, datainfo) == true)
				{
					mapIdtoIndexNode[pNode->m_dwId] = nodeIndex++;

					pNode->ExportPC1(fileNode, position);
				}
				else
				{
					AfxMessageBox(_T("导出Topo中，读取Node表存在错误!"));
				}

				delete pNode;
				pNode = nullptr;

				rs.MoveNext();
			}
			rs.Close();
		}
		catch(CDBException* ex)
		{
			//	AfxMessageBox(ex->m_strError);
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}

		try
		{
			DWORD edgeIndex = 0;

			CString strSQL;
			strSQL.Format(_T("Select * From %s"), m_tableEdge);
			if(m_strFilter.IsEmpty() == FALSE)
			{
				strSQL += _T(" Where ") + m_strFilter;
			}
			strSQL += _T("order by ID");

			CODBCRecordset rs(m_pDatabase);
			rs.Open(strSQL);
			while(!rs.IsEOF())
			{
				CRoadEdge* pEdge = new CRoadEdge();
				if(pEdge->GetValues(rs) == true)
				{
					mapIdtoIndexEdge[pEdge->m_dwId] = edgeIndex++;

					{
						strSQL.Format(_T("Select minX,minY,maxX,maxY From %s Where ID=%d"), m_tablePolyData, pEdge->m_dwPoly);
						CODBCRecordset rsPoly(m_pDatabase);
						rsPoly.Open(strSQL);
						if(!rsPoly.IsEOF())
						{
							pEdge->m_Rect.left = datainfo.MapToDoc(rsPoly.Field(_T("minX")).AsDouble());
							pEdge->m_Rect.top = datainfo.MapToDoc(rsPoly.Field(_T("minY")).AsDouble());
							pEdge->m_Rect.right = datainfo.MapToDoc(rsPoly.Field(_T("maxX")).AsDouble());
							pEdge->m_Rect.bottom = datainfo.MapToDoc(rsPoly.Field(_T("maxY")).AsDouble());
						}
						rsPoly.Close();
					}

					if(pEdge->m_fLength <= 0.0f)
					{
						CString strTag;
						strSQL.Format(_T("Select * From %s Where ID=%d"), m_tablePolyData, pEdge->m_dwPoly);
						CODBCRecordset rsPoly(m_pDatabase);
						rsPoly.Open(strSQL);
						if(!rsPoly.IsEOF())
						{
							CPoly* poly = new CPoly();
							if(poly->GetValues(rsPoly, datainfo, strTag) == true)
							{
								double length = 0.0f;
								for(int index = 0; index < poly->GetAnchors() - 1; index++)
								{
									double fx = datainfo.DocToMap(poly->m_pPoints[index].x);
									double fy = datainfo.DocToMap(poly->m_pPoints[index].y);
									double tx = datainfo.DocToMap(poly->m_pPoints[index + 1].x);
									double ty = datainfo.DocToMap(poly->m_pPoints[index + 1].y);
									fx = fx*M_PI/180.0f;
									fy = fy*M_PI/180.0f;
									tx = tx*M_PI/180.0f;
									ty = ty*M_PI/180.0f;

									double t = 0;
									if(tx == fx && ty == fy)
									{
										t = 0.0f;
									}
									else if(tx == fx)
									{
										t = std::abs(ty - fy);
									}
									else
									{
										double g = atan(1/(tan(ty)*cos(fy)*(1/sin(tx - fx)) - (1/tan(tx - fx))*sin(fy)));
										t = std::abs(asin(cos(ty)*sin(tx - fx)/sin(g)));
									}

									length += t*180/M_PI*60*6371.118/3437.75;
								}
								pEdge->m_fLength = length;
							}

							delete poly;
							poly = nullptr;
						}
						rsPoly.Close();
					}

					pEdge->ExportPC(mapIdtoIndexNode, mapIdtoIndexPoly, fileEdge);
				}
				else
				{
					AfxMessageBox(_T("导出Topo中，读取Edge表存在错误!"));
				}

				delete pEdge;
				pEdge = nullptr;

				rs.MoveNext();
			}

			mapIdtoIndexNode.empty();
			mapIdtoIndexPoly.empty();

			rs.Close();
		}
		catch(CDBException* ex)
		{
			//	AfxMessageBox(ex->m_strError);
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}

		try
		{
			CString strSQL;
			strSQL.Format(_T("Select * From %s"), m_tableNode);
			if(m_strFilter.IsEmpty() == FALSE)
			{
				strSQL += _T(" Where ") + m_strFilter;
			}
			strSQL += _T("order by ID");

			_variant_t varRowsAff;
			CODBCRecordset rs(m_pDatabase);
			rs.Open(strSQL);
			while(!rs.IsEOF())
			{
				CRoadNode* pNode = new CRoadNode();
				if(pNode->GetValues(rs, datainfo) == true)
				{
					pNode->ExportPC2(mapIdtoIndexEdge, fileNode);
				}
				else
				{
					AfxMessageBox(_T("导出Topo中，读取Node表存在错误!"));
				}

				delete pNode;
				pNode = nullptr;

				rs.MoveNext();
			}

			mapIdtoIndexEdge.empty();

			rs.Close();
		}
		catch(CDBException* ex)
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
	}
}

bool CTopodb::IsPolyExisting(const CPoly* pPoly) const
{
	if(CTopology::IsPolyExisting(pPoly) == true)
		return true;

	try
	{
		CString strSQL;
		strSQL.Format(_T("Select ID From %s where Poly=%d"), m_tableEdge, pPoly->m_geoId);

		CODBCRecordset rs(m_pDatabase);
		rs.Open(strSQL);
		if(!rs.IsEOF())
		{
			rs.Close();
			return true;
		}
		rs.Close();
	}
	catch(CDBException* ex)
	{
		//	AfxMessageBox(ex->m_strError);
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}

	return false;
}

void CTopodb::OnRectChanged(CRectF& mapRect, const CDatainfo& datainfo)
{
	if(m_bVisible == false)
	{
		for(CEdgeMap::iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); it++)
		{
			CEdge* pEdge = (CEdge*)it->second;
			{
				BeforeDisposeEdge(pEdge, datainfo);

				delete pEdge;
				pEdge = nullptr;
			}
		}
		m_mapIdEdge.clear();

		for(CNodeMap::iterator it = m_mapIdNode.begin(); it != m_mapIdNode.end(); it++)
		{
			CNode* pNode = (CNode*)it->second;
			{
				BeforeDisposeNode(pNode, datainfo);

				delete pNode;
				pNode = nullptr;
			}
		}
		m_mapIdNode.clear();

		return;
	}

	if(m_pDatabase == nullptr)
		return;

	if(m_rectCurrent == mapRect)
		return;

	m_pActivateNode = nullptr;

	if(m_rectCurrent.Contains(mapRect) == TRUE)//放大
	{
		CNodeMap::iterator it = m_mapIdNode.begin();
		while(it != m_mapIdNode.end())
		{
			CNode* pNode = (CNode*)it->second;
			CPointF mapPoint = datainfo.DocToMap(pNode->m_Point.x, pNode->m_Point.y);

			if(mapRect.PtInRect(mapPoint.x, mapPoint.y) == FALSE)
			{
				BeforeDisposeNode(pNode, datainfo);

				delete pNode;
				pNode = nullptr;

				CNodeMap::iterator it1 = it;
				it++;
				m_mapIdNode.erase(it1);
				continue;
			}
			else
			{
				it++;
			}
		}

		CEdgeMap mapIdEdge;
		for(CNodeMap::iterator it = m_mapIdNode.begin(); it != m_mapIdNode.end(); it++)
		{
			CNode* pNode = (CNode*)it->second;
			for(int index = 0; index < pNode->m_edgelist.GetCount(); index++)
			{
				DWORD dwEdge = pNode->m_edgelist.GetAt(index);
				if(m_mapIdEdge.find(dwEdge) != m_mapIdEdge.end())
				{
					CEdge* pEdge = m_mapIdEdge[dwEdge];
					mapIdEdge[dwEdge] = pEdge;
					m_mapIdEdge.erase(dwEdge);
				}
			}
		}

		for(CEdgeMap::iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); it++)
		{
			CEdge* pEdge = (CEdge*)it->second;
			if(pEdge != nullptr)
			{
				BeforeDisposeEdge(pEdge, datainfo);

				delete pEdge;
				pEdge = nullptr;
			}
		}
		m_mapIdEdge.clear();

		for(CEdgeMap::iterator it = mapIdEdge.begin(); it != mapIdEdge.end(); it++)
		{
			CEdge* pEdge = (CEdge*)it->second;
			m_mapIdEdge[pEdge->m_dwId] = pEdge;
		}
		mapIdEdge.clear();
	}
	else if(mapRect.Contains(m_rectCurrent) == TRUE)//缩小
	{
		if(m_tableNode.IsEmpty() == FALSE)
		{
			CString strSQL;
			strSQL.Format(_T("Select * From %s Where X<=%g And X>=%g And Y<=%g And Y>=%g"), m_tableNode, mapRect.right, mapRect.left, mapRect.bottom, mapRect.top);
			if(m_strFilter.IsEmpty() == FALSE)
			{
				strSQL = strSQL + _T(" And ") + m_strFilter;
			}
			try
			{
				CODBCRecordset rsNode(m_pDatabase);
				rsNode.Open(strSQL);
				while(!rsNode.IsEOF())
				{
					_variant_t var;

					DWORD dwId = rsNode.Field(_T("ID")).AsInt();
					CNodeMap::iterator it = m_mapIdNode.find(dwId);
					if(it != m_mapIdNode.end())
					{
						rsNode.MoveNext();
						continue;
					}

					CRoadNode* pNode = new CRoadNode;
					pNode->GetValues(rsNode, datainfo);
					m_mapIdNode[dwId] = pNode;

					for(int index = 0; index < pNode->m_edgelist.GetCount(); index++)
					{
						DWORD dwEdge = pNode->m_edgelist.GetAt(index);
						if(m_mapIdEdge.find(dwEdge) == m_mapIdEdge.end())
						{
							CRoadEdge* pEdge = new CRoadEdge();
							pEdge->GetValues(m_pDatabase, m_tableEdge, datainfo, dwEdge);
							m_mapIdEdge[dwEdge] = pEdge;
						}
					}

					rsNode.MoveNext();
				}
				rsNode.Close();
			}
			catch(CDBException* ex)
			{
				//	AfxMessageBox(ex->m_strError);
				OutputDebugString(ex->m_strError);
				ex->Delete();
			}
		}
	}
	else//移动
	{
		CNodeMap::iterator it = m_mapIdNode.begin();
		while(it != m_mapIdNode.end())
		{
			CNode* pNode = (CNode*)it->second;
			CPointF mapPoint = datainfo.DocToMap(pNode->m_Point.x, pNode->m_Point.y);

			if(mapRect.PtInRect(mapPoint.x, mapPoint.y) == FALSE)
			{
				BeforeDisposeNode(pNode, datainfo);

				delete pNode;
				pNode = nullptr;

				CNodeMap::iterator it1 = it;
				it++;
				m_mapIdNode.erase(it1);
			}
			else
			{
				it++;
			}
		}

		if(m_tableNode.IsEmpty() == FALSE)
		{
			CString strSQL;
			strSQL.Format(_T("Select * From %s Where X<=%g And X>=%g And Y<=%g And Y>=%g"), m_tableNode, mapRect.right, mapRect.left, mapRect.bottom, mapRect.top);
			if(m_strFilter.IsEmpty() == FALSE)
			{
				strSQL = strSQL + _T(" And ") + m_strFilter;
			}

			try
			{
				CODBCRecordset rsNode(m_pDatabase);
				rsNode.Open(strSQL);
				while(!rsNode.IsEOF())
				{
					DWORD dwId = rsNode.Field(_T("ID")).AsInt();
					if(m_mapIdNode.find(dwId) == m_mapIdNode.end())
					{
						CRoadNode* pNode = new CRoadNode;
						pNode->GetValues(rsNode, datainfo);
						m_mapIdNode[dwId] = pNode;
					}

					rsNode.MoveNext();
				}
				rsNode.Close();

				CEdgeMap mapIdEdge;
				for(CNodeMap::iterator it = m_mapIdNode.begin(); it != m_mapIdNode.end(); it++)
				{
					CNode* pNode = (CNode*)it->second;
					for(int index = 0; index < pNode->m_edgelist.GetCount(); index++)
					{
						DWORD dwEdge = pNode->m_edgelist.GetAt(index);
						if(m_mapIdEdge.find(dwEdge) != m_mapIdEdge.end())
						{
							CEdge* pEdge = m_mapIdEdge[dwEdge];
							mapIdEdge[dwEdge] = pEdge;
							m_mapIdEdge.erase(dwEdge);
						}
						else if(mapIdEdge.find(dwEdge) == mapIdEdge.end())
						{
							CRoadEdge* pEdge = new CRoadEdge;
							pEdge->GetValues(m_pDatabase, m_tableEdge, datainfo, dwEdge);
							mapIdEdge[dwEdge] = pEdge;
						}
					}
				}

				for(CEdgeMap::iterator it = m_mapIdEdge.begin(); it != m_mapIdEdge.end(); it++)
				{
					CEdge* pEdge = (CEdge*)it->second;
					if(pEdge != nullptr)
					{
						BeforeDisposeEdge(pEdge, datainfo);

						delete pEdge;
						pEdge = nullptr;
					}
				}
				m_mapIdEdge.clear();

				for(CEdgeMap::iterator it = mapIdEdge.begin(); it != mapIdEdge.end(); it++)
				{
					CEdge* pEdge = (CEdge*)it->second;
					if(pEdge != nullptr)
					{
						m_mapIdEdge[pEdge->m_dwId] = pEdge;
					}
				}
				mapIdEdge.clear();
			}
			catch(CDBException* ex)
			{
				//	AfxMessageBox(ex->m_strError);
				OutputDebugString(ex->m_strError);
				ex->Delete();
			}
		}
	}

	m_rectCurrent = mapRect;
}

void CTopodb::NewNodeCreated(CNode* pNode)
{
	if(m_pDatabase != nullptr)
	{
		_variant_t varRowsAff;

		CString strSQL;
		strSQL.Format(_T("Insert Into %s (X,Y) Values(%g,%g)"), m_tableNode, 0, 0);
		m_pDatabase->ExecuteSQL(LPCTSTR(strSQL));

		pNode->m_dwId = GetMaxId(m_tableNode);;
		pNode->m_bModified = true;
	}
}

void CTopodb::NewEdgeCreated(CEdge* pEdge)
{
	if(m_pDatabase != nullptr)
	{
		if(pEdge->m_dwId == 0)
		{
			CString strSQL;
			strSQL.Format(_T("Insert Into %s (Poly,Node_From,Node_To) Values(-1,-1,-1)"), m_tableEdge);
			m_pDatabase->ExecuteSQL(LPCTSTR(strSQL));
			pEdge->m_dwId = GetMaxId(m_tableEdge);
		}
		else
		{
			pEdge->m_dwId = this->DuplicateRec(m_tableEdge, pEdge->m_dwId, -1);
		}

		if(pEdge->m_pPoly != nullptr)
		{
			const DWORD dwOldID = pEdge->m_pPoly->m_geoId;
			const DWORD dwNewId = this->DuplicateRec(m_tablePolyData, dwOldID, -1);

			this->DuplicateRec(m_tablePolyAttribute, dwOldID, dwNewId);

			pEdge->m_dwPoly = dwNewId;
			pEdge->m_pPoly->m_geoId = dwNewId;
		}
	}
}

void CTopodb::PreRemoveNode(CNode* pNode)
{
	if(m_pDatabase != nullptr)
	{
		CString strSQL;
		strSQL.Format(_T("Delete From %s Where ID=%d"), m_tableNode, pNode->m_dwId);
		m_pDatabase->ExecuteSQL(LPCTSTR(strSQL));
	}
	pNode->m_bModified = false;

	CTopology::PreRemoveNode(pNode);
}

void CTopodb::PreRemoveEdge(CEdge* pEdge)
{
	if(m_pDatabase != nullptr)
	{
		CString strSQL;
		strSQL.Format(_T("Delete From %s Where ID=%d"), m_tableEdge, pEdge->m_dwId);
		m_pDatabase->ExecuteSQL(LPCTSTR(strSQL));
	}
	pEdge->m_bModified = false;

	CTopology::PreRemoveEdge(pEdge);
}

void CTopodb::RemovePoly(CEdge* pEdge)
{
	if(m_pDatabase != nullptr && pEdge->m_pPoly != nullptr)
	{
		CString strSQL1;
		strSQL1.Format(_T("Delete From %s Where ID=%d"), m_tablePolyData, pEdge->m_pPoly->m_geoId);
		m_pDatabase->ExecuteSQL(LPCTSTR(strSQL1));

		CString strSQL2;
		strSQL2.Format(_T("Delete From %s Where ID=%d"), m_tablePolyAttribute, pEdge->m_pPoly->m_geoId);
		m_pDatabase->ExecuteSQL(LPCTSTR(strSQL2));

		delete pEdge->m_pPoly;
		pEdge->m_pPoly = nullptr;
	}

	CTopology::RemovePoly(pEdge);
}

void CTopodb::BeforeDisposeNode(CNode* pNode, const CDatainfo& datainfo)
{
	CTopology::BeforeDisposeNode(pNode, datainfo);
}

void CTopodb::BeforeDisposeEdge(CEdge* pEdge, const CDatainfo& datainfo)
{
	if(pEdge->m_bModified == true)
	{
		if(m_pDatabase != nullptr)
		{
			CString strSQL;
			strSQL.Format(_T("Update %s Set Poly=%d,Node_From=%d,Node_To=%d Where ID=%d"), m_tableEdge, pEdge->m_pPoly->m_geoId, pEdge->m_dwFromNode, pEdge->m_dwToNode, pEdge->m_dwId);
			m_pDatabase->ExecuteSQL(LPCTSTR(strSQL));

			if(pEdge->m_pPoly != nullptr)
			{
				pEdge->m_pPoly->BeforeDispose(m_pDatabase, m_tablePolyData, datainfo);
			}
		}

		pEdge->m_bModified = false;
	}

	CTopology::BeforeDisposeEdge(pEdge, datainfo);
}

void CTopodb::SetTables()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CTopoTableDlg dlg(nullptr, m_tableEdge, m_tableNode, m_tablePolyData, m_tablePolyAttribute, m_strFilter);
	if(dlg.DoModal() == IDOK)
	{
		m_tableEdge = dlg.m_tableEdge;
		m_tableNode = dlg.m_tableNode;
		m_tablePolyData = dlg.m_tablePolyData;
		m_tablePolyAttribute = dlg.m_tablePolyAttribute;
		m_strFilter = dlg.m_strFilter;
	}
}

void CTopodb::Initialize(const CDatainfo& datainfo)
{
	try
	{
		CString strSQL1;
		strSQL1.Format(_T("Truncate Table %s"), m_tableNode);
		CString strSQL2;
		strSQL2.Format(_T("Truncate Table %s"), m_tableEdge);

		m_pDatabase->ExecuteSQL(LPCTSTR(strSQL1));
		m_pDatabase->ExecuteSQL(LPCTSTR(strSQL2));
	}
	catch(CDBException* ex)
	{
		//	AfxMessageBox(ex->m_strError);
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}
}

DWORD CTopodb::GetMaxId(const CString& strTable) const
{
	DWORD dwMaxId = 0;
	if(m_pDatabase != nullptr/* && m_pDatabase.IsOpen() == TRUE*/)
	{
		CString strSQL;
		strSQL.Format(_T("Select Max(ID) As MaxID From %s"), strTable);

		try
		{
			_variant_t varRowsAff;

			CODBCRecordset rs(m_pDatabase);
			rs.Open(strSQL);
			if(!rs.IsEOF())
			{
				dwMaxId = rs.Field(_T("MaxID")).AsInt();
			}
			rs.Close();
		}
		catch(CDBException* ex)
		{
			//	AfxMessageBox(ex->m_strError);
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}
	}

	return dwMaxId;
}

DWORD CTopodb::DuplicateRec(const CString& strTable, DWORD dwId, DWORD dwNewId)
{
	if(m_pDatabase == nullptr)
		return 0;

	CString strSQL;
	strSQL.Format(_T("Select * From %s where ID=%d"), strTable, dwId);
	try
	{
		CODBCRecordset rs(m_pDatabase);
		rs.Open(strSQL);
		if(!rs.IsEOF())
		{
			CString strValues;
			CString strFields;
			for(short nIndex = 0; nIndex < rs.GetODBCFieldCount(); nIndex++)
			{
				CString strName = rs.GetFieldName(nIndex);
				if(strName == _T("ID"))
				{
					if(dwNewId != -1)
					{
						strFields += _T("ID,");
						CString strID;
						strID.Format(_T("%d"), dwNewId);
						strValues += strID + _T(',');
					}

					continue;
				}

				CString strValue = rs.Field(nIndex).AsString();;
				strValue.TrimLeft();
				strValue.TrimRight();
				switch(rs.Field(nIndex).m_dwType)
				{
					case ADOCG::adBigInt:
					case ADOCG::adDecimal:
					case ADOCG::adDouble:
					case ADOCG::adGUID:
					case ADOCG::adSingle:
					case ADOCG::adSmallInt:
					case ADOCG::adTinyInt:
					case ADOCG::adUnsignedBigInt:
					case ADOCG::adUnsignedInt:
					case ADOCG::adUnsignedSmallInt:
					case ADOCG::adUnsignedTinyInt:
					case ADOCG::adInteger:
					case ADOCG::adNumeric:
					case ADOCG::adVarNumeric:
					case ADOCG::adBoolean:
					case ADOCG::adCurrency:
						{
							strFields += strName + _T(',');
							if(strValue.IsEmpty() == TRUE)
							{
								strValues += _T("0,");
							}
							else
							{
								strValues += strValue + _T(',');
							}
						}
						break;
					case ADOCG::adBinary:
					case ADOCG::adVarBinary:
					case ADOCG::adLongVarBinary:
						break;
					case ADOCG::adBSTR:
					case ADOCG::adChapter:
					case ADOCG::adChar:
					case ADOCG::adDate:
					case ADOCG::adDBDate:
					case ADOCG::adDBTime:
					case ADOCG::adDBTimeStamp:
					case ADOCG::adEmpty:
					case ADOCG::adError:
					case ADOCG::adFileTime:
					case ADOCG::adIDispatch:
					case ADOCG::adIUnknown:
					case ADOCG::adLongVarChar:
					case ADOCG::adLongVarWChar:
					case ADOCG::adPropVariant:
					case ADOCG::adUserDefined:
					case ADOCG::adVarChar:
					case ADOCG::adVariant:
					case ADOCG::adVarWChar:
					case ADOCG::adWChar:
						{
							strFields += strName + _T(',');
							strValues += _T('\'') + strValue + _T("\',");
						}
						break;
					default:
						break;
				}
			}

			rs.Close();

			if(strValues.IsEmpty() == FALSE)
			{
				strValues = strValues.Left(strValues.GetLength() - 1);
			}
			if(strFields.IsEmpty() == FALSE)
			{
				strFields = strFields.Left(strFields.GetLength() - 1);
			}
			if(strValues.IsEmpty() == FALSE && strFields.IsEmpty() == FALSE)
			{
				CString strSQL;
				strSQL.Format(_T("Insert Into %s (%s) Values(%s)"), strTable, strFields, strValues);
				m_pDatabase->ExecuteSQL(LPCTSTR(strSQL));

				dwId = GetMaxId(strTable);
			}
		}
	}
	catch(CDBException* ex)
	{
		//	AfxMessageBox(ex->m_strError);
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}

	return dwId;
}


bool CTopodb::PolySplitted(CWnd* pWnd, const CViewinfo& viewinfo, WORD wLayer, DWORD dwPoly, const CPoint& point, DWORD dwBred, DWORD& dwOldEdgeId, DWORD& dwNewEdgeId)
{

	//	CPoly* pPoly1 = this->GetEdgePoly(dwOldEdgeId);
	//	CPoly* pPoly2 = this->GetEdgePoly(dwNewEdgeId);
	//	const double length1 = pPoly1->GetLength(viewinfo.m_datainfo.m_zoomPointToDoc);
	//	const double length2 = pPoly2->GetLength(viewinfo.m_datainfo.m_zoomPointToDoc);
	//
	//	CRoadEdge* pOldEdge = (CRoadEdge*)GetEdge(dwOldEdgeId);
	//	CRoadEdge* pNewEdge = (CRoadEdge*)GetEdge(dwNewEdgeId);
	//	pOldEdge->m_fLength *= length1/(length1 + length2);
	//	pNewEdge->m_fLength *= length2/(length1 + length2);

	return true;
}
bool CTopodb::PolyJoined(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, WORD wLayer, DWORD dwPoly1, BYTE mode, DWORD dwPoly2, DWORD& dwEdgeId1, DWORD& dwEdgeId2)
{
	//	if(typeid(pEdge1) == typeid(CRoadEdge) && typeid(pEdge2) == typeid(CRoadEdge))
	//	{
	//		((CRoadEdge*)pEdge1)->m_fLength = ((CRoadEdge*)pEdge1)->m_fLength + ((CRoadEdge*)pEdge2)->m_fLength;
	//	}
	return true;
}