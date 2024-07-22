#include "stdafx.h"
#include "Node.h"
#include "RoadNode.h"

#include "../Geometry/Geom.h"
#include "../Dataview/DataInfo.h"
#include "../Database/ODBCRecordset.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRoadNode::CRoadNode()
	: CNode()
{
	m_dwId = 0;
	m_bAttribute1 = 0X00;
}

CRoadNode::~CRoadNode()
{
	m_edgelist.RemoveAll();
}

CNode* CRoadNode::Clone()
{
	CRoadNode* pNode = new CRoadNode();

	pNode->m_dwId = m_dwId;
	pNode->m_Point = m_Point;

	const BYTE nCount = m_edgelist.GetSize();
	for(int i = 0; i<nCount; i++)
	{
		const DWORD dwId = m_edgelist.GetAt(i);
		pNode->m_edgelist.Add(dwId);
	}

	pNode->m_bAttribute1 = m_bAttribute1;
	pNode->m_Forbid = m_Forbid;

	return pNode;
}

void CRoadNode::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CNode::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_bAttribute1;
	}
	else
	{
		ar>>m_bAttribute1;
	}

	m_Forbid.Serialize(ar, dwVersion);
}

bool CRoadNode::GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo)
{
	try
	{
		m_dwId = rs.Field(_T("ID")).AsInt();
		m_Point.x = datainfo.MapToDoc(rs.Field(_T("X")).AsInt());
		m_Point.y = datainfo.MapToDoc(rs.Field(_T("Y")).AsInt());

		const int nEdgeCount = rs.Field(_T("Edge_Count")).AsInt();
		const CLongBinary* pLongBinary1 = rs.Field(_T("Edge_List")).AsBinary();
		const int nSize1 = pLongBinary1->m_dwDataLength;
		if(nSize1>0)
		{
			DWORD* pDWORDList = new DWORD[nEdgeCount];
			BYTE* pData = (BYTE*)::GlobalLock(pLongBinary1->m_hData);
			memcpy((char*)pDWORDList, pData, nSize1); ///�������ݵ�������   
			::GlobalUnlock(pData);

			for(int index = 0; index<nEdgeCount; index++)
			{
				m_edgelist.Add(pDWORDList[index]);
			}

			delete[] pDWORDList;
			pDWORDList = nullptr;
		}

		m_Forbid.SetDimention(nEdgeCount);

		const CLongBinary* pLongBinary2 = rs.Field(_T("Edge_Matrix")).AsBinary();
		const int nSize2 = pLongBinary2->m_dwDataLength;
		if(nSize2>0)
		{
			BYTE* pData = (BYTE*)::GlobalLock(pLongBinary2->m_hData);
			memcpy((char*)m_Forbid.m_pArray, pData, nSize2); ///�������ݵ�������   
			::GlobalUnlock(pData);
		}
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

bool CRoadNode::PutValues(CODBCRecordset& rs, const CDatainfo& datainfo) const
{
	return false;
}

void CRoadNode::SetForbid(DWORD dwEdge1, DWORD dwEdge2, bool bValue) const
{
	const int nCount = m_edgelist.GetSize();
	for(int index1 = 0; index1<nCount; index1++)
	{
		DWORD dwId1 = m_edgelist.GetAt(index1);
		if(dwId1 = dwEdge1)
		{
			for(int index2 = 0; index2<nCount; index2++)
			{
				DWORD dwId2 = m_edgelist.GetAt(index2);
				if(dwId2 = dwEdge2)
				{
					m_Forbid.SetValue(index1, index2, bValue);
				}
			}
		}
	}
}

bool CRoadNode::CanThruEdge(const DWORD& dwEdgeId) const
{
	const BYTE index = this->GetEdgeIndex(dwEdgeId);
	for(int i = 0; i<m_edgelist.GetCount(); i++)
	{
		if(m_Forbid.GetValue(i, index)==true)
			return true;
	}

	return false;
}

bool CRoadNode::CanEdgeThru(const DWORD& dwEdgeId) const
{
	const BYTE index = this->GetEdgeIndex(dwEdgeId);
	for(int i = 0; i<m_edgelist.GetCount(); i++)
	{
		if(i==index)
			continue;

		if(m_Forbid.GetValue(index, i)==true)
			return true;
	}

	return false;
}

void CRoadNode::RemoveRelatingEdge(const DWORD& dwEdge)
{
	int index = 0;
	const int nCount = m_edgelist.GetSize();
	for(index = 0; index<nCount; index++)
	{
		const DWORD dwId = m_edgelist.GetAt(index);
		if(dwEdge==dwId)
		{
			break;
		}
	}
	if(nCount==1&&index==0)
	{
		m_Forbid.SetDimention(0);
	}
	else if(index<nCount)
	{
		m_Forbid.Remove(index);
	}

	CNode::RemoveRelatingEdge(dwEdge);
}

void CRoadNode::InsertRelatingEdge(const DWORD& dwEdge)
{
	m_Forbid.Increase();

	CNode::InsertRelatingEdge(dwEdge);
}

void CRoadNode::ExportPC1(CFile& file, DWORD& position) const
{
	CNode::ExportPC1(file, position);

	position += m_Forbid.GetByteLength();
}

void CRoadNode::ExportPC2(std::map<DWORD, DWORD>& mapIdtoIndexEdge, CFile& file) const
{
	CNode::ExportPC2(mapIdtoIndexEdge, file);

	m_Forbid.ExportPC2(file);
}
