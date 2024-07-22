#include "stdafx.h"
#include "Topo.h"
#include "Topodb.h"
#include "RoadTopodb.h"

#include "Node.h"
#include "RoadNode.h"
#include "Edge.h"
#include "RoadEdge.h"
#include "../Dataview/DataInfo.h"
#include "../Database/Database.h"
#include "../Database/ODBCRecordset.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"

using namespace std;

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRoadTopodb::CRoadTopodb(CDatabase* pDatabase)
	: CTopodb(pDatabase)
{
}

CRoadTopodb::~CRoadTopodb()
{
}

void CRoadTopodb::Initialize(const CDatainfo& datainfo)
{
	CTopodb::Initialize(datainfo);

	try
	{
		DWORD idNode = 1;
		DWORD idEdge = 1;
		_variant_t varRowsAff;

		CString strTag;
		CString strSQL;
		strSQL.Format(_T("Select * From %s"), m_tableNode);
		CODBCRecordset rsNode(m_pDatabase);
		rsNode.Open(strSQL);

		if(m_strFilter.IsEmpty() == FALSE)
			strSQL.Format(_T("Select A.* From %s A,%s B Where %s order by A.ID"), m_tablePolyData, m_tablePolyAttribute, m_strFilter);
		else
			strSQL.Format(_T("Select * From %s order by ID"), m_tablePolyData);

		CODBCRecordset rsPoly(m_pDatabase);
		rsPoly.Open(strSQL);
		while(!rsPoly.IsEOF())
		{
			CPoly* poly = new CPoly();
			if(poly->GetValues(rsPoly, datainfo, strTag) == true)
			{
				const CPoint& point1 = poly->GetAnchor(1);
				const CPoint& point2 = poly->GetAnchor(poly->m_nAnchors);

				rsNode.AddNew();
				double x1 = datainfo.DocToMap(point1.x);
				double y1 = datainfo.DocToMap(point1.y);
				rsNode.Field(_T("X")) = x1;
				rsNode.Field(_T("Y")) = y1;
				rsNode.Field(_T("Edge_Count")) = 1;
				SAFEARRAYBOUND rgsabound1[1];
				rgsabound1[0].lLbound = 0;
				rgsabound1[0].cElements = sizeof(DWORD);
				SAFEARRAY* psa1 = SafeArrayCreate(VT_UI1, 1, rgsabound1);
				if(psa1 != nullptr)
				{
					LPVOID lpArrayData = nullptr;
					::SafeArrayAccessData(psa1, &lpArrayData);
					::memcpy((char*)lpArrayData, &idEdge, sizeof(DWORD));
					::SafeArrayUnaccessData(psa1);

					VARIANT varEdges;
					varEdges.vt = VT_ARRAY | VT_UI1;
					varEdges.parray = psa1;
					rsNode.Field(_T("Edge_List")) = psa1;

					::SafeArrayDestroy(psa1);
				}
				rsNode.Update();

				rsNode.AddNew();
				double x2 = datainfo.DocToMap(point2.x);
				double y2 = datainfo.DocToMap(point2.y);
				rsNode.Field(_T("X")) = x2;
				rsNode.Field(_T("Y")) = y2;
				rsNode.Field(_T("Edge_Count")) = 1;
				SAFEARRAYBOUND rgsabound2[1];
				rgsabound2[0].lLbound = 0;
				rgsabound2[0].cElements = sizeof(DWORD);
				SAFEARRAY* psa2 = SafeArrayCreate(VT_UI1, 1, rgsabound2);
				if(psa2 != nullptr)
				{
					LPVOID lpArrayData = nullptr;
					::SafeArrayAccessData(psa2, &lpArrayData);
					::memcpy((char*)lpArrayData, &idEdge, sizeof(DWORD));
					::SafeArrayUnaccessData(psa2);

					VARIANT varEdges;
					varEdges.vt = VT_ARRAY | VT_UI1;
					varEdges.parray = psa2;
					rsNode.Field(_T("Edge_List")) = psa2;

					::SafeArrayDestroy(psa2);
				}
				rsNode.Update();

				//	CString strSQL1;
				//	strSQL1.Format(_T("Insert Into %s (X,Y) Values(%g,%g)"),m_tableNode,point1.x/10000000.0f,point1.y/10000000.0f);
				//	m_pDatabase->ExecuteSQL(LPCTSTR(strSQL1), &varRowsAff, ADOCG::adCmdText);

				//	CString strSQL2;
				//	strSQL2.Format(_T("Insert Into %s (X,Y) Values(%g,%g)"),m_tableNode,point2.x/10000000.0f,point2.y/10000000.0f);
				//	m_pDatabase->ExecuteSQL(LPCTSTR(strSQL2), &varRowsAff, ADOCG::adCmdText);

				CString strSQL;
				strSQL.Format(_T("Insert Into %s (Poly,Node_From,Node_To) Values(%d,%d,%d)"), m_tableEdge, poly->m_geoId, idNode, idNode + 1);
				m_pDatabase->ExecuteSQL(LPCTSTR(strSQL));

				idNode++;
				idNode++;
				idEdge++;
				delete poly;
				poly = nullptr;
			}
			rsPoly.MoveNext();
		}
		rsNode.Close();
		rsPoly.Close();
	}
	catch(CDBException* ex)
	{
		//	AfxMessageBox(ex->m_strError);
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}
}

CNode* CRoadTopodb::ApplyNode() const
{
	CNode* pNode = new CRoadNode();
	pNode->m_dwId = this->ApplyNodeId();
	pNode->m_bModified = true;
	return pNode;
}
CEdge* CRoadTopodb::ApplyEdge() const
{
	CEdge* pEdge = new CRoadEdge();
	pEdge->m_dwId = this->ApplyEdgeId();
	pEdge->m_bModified = true;
	return pEdge;
}

void CRoadTopodb::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CTopodb::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

void CRoadTopodb::ExportPC(CString strFile, const CDatainfo& datainfo)
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

			_variant_t varRowsAff;
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

					fileContent.Write(&poly->m_nAnchors, sizeof(unsigned int));
					fileContent.Write(poly->m_pPoints, sizeof(CPoint)*poly->m_nAnchors);

					position += sizeof(unsigned int) + poly->m_nAnchors*sizeof(CPoint);
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

			_variant_t varRowsAff;
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

void CRoadTopodb::BeforeDisposeNode(CNode* pNode, const CDatainfo& datainfo)
{
	if(pNode->m_bModified==true)
	{
		if(m_pDatabase!=nullptr)
		{
			try
			{
				CString strSQL;
				strSQL.Format(_T("Select * From %s Where ID=%d"), m_tableNode, pNode->m_dwId);

			CODBCRecordset rs(m_pDatabase);
			rs.Open(strSQL);
			if(!rs.IsEOF())
			{
				double x = datainfo.DocToMap(pNode->m_Point.x);
				double y = datainfo.DocToMap(pNode->m_Point.y);
				rs.Field(_T("X")) = x;
				rs.Field(_T("Y")) = y;

				int nEdgeCount = pNode->m_edgelist.GetCount();
				rs.Field(_T("Edge_Count")) = nEdgeCount;

				SAFEARRAYBOUND rgsabound[1];
				rgsabound[0].lLbound = 0;
				rgsabound[0].cElements = sizeof(DWORD)*nEdgeCount;

				SAFEARRAY* psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
				if(psa != nullptr)
				{
					LPVOID lpArrayData = nullptr;
					::SafeArrayAccessData(psa, &lpArrayData);
					for(int index = 0; index < nEdgeCount; index++)
					{
						DWORD dwId = pNode->m_edgelist.GetAt(index);
						::memcpy((char*)lpArrayData, &dwId, sizeof(DWORD));
						lpArrayData = (char*)lpArrayData + sizeof(DWORD);
					}
					::SafeArrayUnaccessData(psa);

					VARIANT varEdges;
					varEdges.vt = VT_ARRAY | VT_UI1;
					varEdges.parray = psa;
					rs.Field(_T("Edge_List")) = psa;

					::SafeArrayDestroy(psa);
				}

				int Bytes = ((CRoadNode*)pNode)->m_Forbid.GetByteLength();
				rgsabound[0].cElements = Bytes;
				psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
				if(psa != nullptr)
				{
					LPVOID lpArrayData = nullptr;
					::SafeArrayAccessData(psa, &lpArrayData);
					::memcpy((char*)lpArrayData, ((CRoadNode*)pNode)->m_Forbid.m_pArray, Bytes);
					::SafeArrayUnaccessData(psa);

					VARIANT varMatrix;
					varMatrix.vt = VT_ARRAY | VT_UI1;
					varMatrix.parray = psa;
					rs.Field(_T("Edge_Matrix")) = psa;

					::SafeArrayDestroy(psa);
				}

				rs.Update();
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

		pNode->m_bModified = false;
	}

	CTopodb::BeforeDisposeNode(pNode, datainfo);
}
