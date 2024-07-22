#include "stdafx.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"

#include "../Style/Line.h"
#include "../Style/ColorSpot.h"
#include "../DataView/DataInfo.h"
#include "../Database/ODBCRecordset.h"

#include "Edge.h"
#include "RoadEdge.h"

#include "Topo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CRoadEdge::CRoadEdge()
	: CEdge()
{
	m_fLength = 0;

	m_modeRestrictSpeed = 0;
	m_designedSpeed = 80;
	m_restrictSpeed = 0;

	m_modeToll = 0;
	m_cashToll = 0;

	//	m_typeSeclusion = 1;
	//	m_typePortion = 1;
	//	m_typeSlope = 1;

	//	m_typeShape = 1;
	//	m_typeHeight = 1;
	//	m_laneCount = 2;

	//	m_modeSpeedRestric = 0;
}

CRoadEdge::~CRoadEdge()
{
}

CEdge* CRoadEdge::Clone()
{
	CRoadEdge* pEdge = new CRoadEdge();

	pEdge->m_dwId = m_dwId;
	pEdge->m_dwFromNode = m_dwFromNode;
	pEdge->m_dwToNode = m_dwToNode;

	pEdge->m_wLayer = m_wLayer;
	pEdge->m_dwPoly = m_dwPoly;
	pEdge->m_Rect = m_Rect;

	pEdge->m_fLength = m_fLength;

	pEdge->m_modeRestrictSpeed = m_modeRestrictSpeed;
	pEdge->m_designedSpeed = m_designedSpeed;
	pEdge->m_restrictSpeed = m_restrictSpeed;

	pEdge->m_modeToll = m_modeToll;
	pEdge->m_cashToll = m_cashToll;

	//	pEdge->m_typeSeclusion = m_typeSeclusion;
	//	pEdge->m_typePortion = m_typePortion;
	//	pEdge->m_typeSlope = m_typeSlope;

	//	pEdge->m_typeShape = m_typeShape;
	//	pEdge->m_typeHeight = m_typeHeight;
	//	pEdge->m_laneCount = m_laneCount;
	//	
	pEdge->m_modeToll = m_modeToll;

	if(m_pPoly!=nullptr)
	{
		pEdge->m_pPoly = (CPoly*)m_pPoly->Clone();
	}

	return pEdge;
}

void CRoadEdge::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CEdge::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_fLength;

		ar<<m_designedSpeed;
		ar<<m_restrictSpeed;
	}
	else
	{
		ar>>m_fLength;

		ar>>m_designedSpeed;
		ar>>m_restrictSpeed;
	}
}

void CRoadEdge::ExportPC(std::map<DWORD, DWORD>& mapIdtoIndexNode, std::map<DWORD, DWORD>& mapIdtoIndexPoly, CFile& file)
{
	CEdge::ExportPC(mapIdtoIndexNode, mapIdtoIndexPoly, file);

	file.Write(&m_fLength, sizeof(float));

	BYTE mode = 0X00;
	mode |= (m_modeRestrictSpeed<<0); //0000 0011 �洢����������� 0 �����ƣ�1 ������� 2 �������
	mode |= (m_modeToll<<2); //0000 1100 �洢�շ��������� 0 ���շѣ�1 �������շ� 2 �������շ�

	file.Write(&m_designedSpeed, sizeof(BYTE));
	file.Write(&m_restrictSpeed, sizeof(BYTE));
	file.Write(&m_cashToll, sizeof(BYTE));
	file.Write(&mode, sizeof(BYTE));
}

bool CRoadEdge::GetValues(CDatabase* pDatabase, const CString& tableEdge, const CDatainfo& datainfo, const DWORD& dwId)
{
	try
	{
		CString strSQL;
		strSQL.Format(_T("Select * From %s Where ID=%d"), tableEdge, dwId);

		CODBCRecordset rs(pDatabase);
		rs.Open(strSQL);
		if(!rs.IsEOF())
		{
			m_dwId = rs.Field(_T("ID")).AsInt();
			m_dwPoly = rs.Field(_T("Poly")).AsInt();
			m_dwFromNode = rs.Field(_T("Node_From")).AsInt();
			m_dwToNode = rs.Field(_T("Node_To")).AsInt();

			/*		var = rs.Field(_T("Length")]->Value;
					if(var.vt != VT_NULL)
						m_fLength = (float)var;

					var = rs.Field(_T("Speed_Designed")]->Value;
					if(var.vt != VT_NULL)
						m_designedSpeed = (DWORD)var;

					var = rs.Field(_T("Speed_Restrict")]->Value;
					if(var.vt != VT_NULL)
						m_restrictSpeed = (DWORD)var;

					var = rs.Field(_T("Speed_Restrict_Mode")]->Value;
					if(var.vt != VT_NULL)
						m_modeRestrictSpeed = (DWORD)var;

					var = rs.Field(_T("Toll_Mode")]->Value;
					m_modeToll = (DWORD)var;

					var = rs.Field(_T("Toll_Cash")]->Value;
					if(var.vt != VT_NULL)
						m_cashToll = (DWORD)var;
		*/
		//			var = rs.Field(_T("Portion")]->Value;
		//			m_typePortion = (DWORD)var;
		}

		rs.Close();
	}
	catch(CDBException*ex)
	{
		//	AfxMessageBox(ex->m_strError);
		OutputDebugString(ex->m_strError);
		ex->Delete();
		return false;
	}

	try
	{
		CString strTag;
		CString strSQL;
		strSQL.Format(_T("Select * From Road_Data Where ID=%d"), m_dwPoly);

		_variant_t varRowsAff;
		CODBCRecordset rs(pDatabase);
		rs.Open(strSQL);
		if(!rs.IsEOF())
		{
			m_pPoly = new CPoly();
			m_pPoly->GetValues(rs, datainfo, strTag);
			m_Rect = m_pPoly->m_Rect;
		}

		rs.Close();
	}
	catch(CDBException*ex)
	{
		//	AfxMessageBox(ex->m_strError);
		OutputDebugString(ex->m_strError);
		ex->Delete();
		return false;
	}

	return true;
}

bool CRoadEdge::GetValues(CODBCRecordset& rs)
{
	try
	{
		m_dwId = rs.Field(_T("ID")).AsInt();
		m_dwPoly = rs.Field(_T("Poly")).AsInt();
		m_dwFromNode = rs.Field(_T("Node_From")).AsInt();
		m_dwToNode = rs.Field(_T("Node_To")).AsInt();
		m_fLength = rs.Field(_T("Length")).AsFloat();

		m_designedSpeed = rs.Field(_T("Speed_Designed")).AsInt();
		m_restrictSpeed = rs.Field(_T("Speed_Restrict")).AsInt();
		m_modeRestrictSpeed = rs.Field(_T("Speed_Restrict_Mode")).AsInt();
		m_modeToll = rs.Field(_T("Toll_Mode")).AsInt();
		m_cashToll = rs.Field(_T("Toll_Cash")).AsInt();
		return true;
	}
	catch(CDBException*ex)
	{
		//	AfxMessageBox(ex->m_strError);
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}
	return false;
}

bool CRoadEdge::PutValues(CODBCRecordset& rs) const
{
	return false;
}

void CRoadEdge::CalLength(CView* pView, const CDatainfo& datainfo, const CTopology& topology)
{
	CPoly* poly=topology.GetEdgePoly(m_dwId);
	if(poly != nullptr)
	{
		m_fLength=poly->GetLength(datainfo, datainfo.m_zoomPointToDoc);
	}
}

void CRoadEdge::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLine& line, CRect inRect, const CTopology& topology)
{
	CEdge::Draw(g, viewinfo, line, inRect, topology);

	CRect rect = m_Rect;
	if(rect.IntersectRect(rect, inRect)==TRUE)
	{
		CPoly* poly = topology.GetEdgePoly(m_dwId);
		if(poly!=nullptr)
		{
			//	if(this->IsArunB() == false || this->IsBrunA() == false)
			if(true)
			{
				CLine line;
				line.m_nWidth = 10;
				line.m_pColor = new CColorSpot(255, 100, 255, 255);
				CLine* pOld = poly->m_pLine;
				poly->m_pLine = &line;

				poly->Draw(g, viewinfo, 1.f);

				poly->m_pLine = pOld;
			}
		}
	}
}
