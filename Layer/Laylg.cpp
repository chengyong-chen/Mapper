#include "stdafx.h"
#include "Laylg.h"
#include "LayerTree.h"

#include "../Style/Spot.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Tag.h"

#include "../Public/ODBCDatabase.h"
#include "../Dataview/viewinfo.h"
#include "../Database/ODBCRecordset.h"
#include "../Database/Datasource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLaylg::CLaylg(CTree<CLayer>& tree)
	:CLayer(tree)
{

}
CLaylg::~CLaylg()
{
	POSITION pos = m_geoms.GetTailPosition();
	while(pos != nullptr)
	{
		const CGeom* pGeom = m_geoms.GetPrev(pos);
		delete pGeom;
	}
	m_geoms.RemoveAll();
}
void CLaylg::Wipeout()
{
	POSITION pos = m_geoms.GetTailPosition();
	while(pos != nullptr)
	{
		const CGeom* pGeom = m_geoms.GetPrev(pos);
		delete pGeom;
	}
	m_geoms.RemoveAll();

	CLayer::Wipeout();
}
void CLaylg::Serializelist(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar << (int)m_geoms.GetCount();
		POSITION pos = m_geoms.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = (CGeom*)m_geoms.GetNext(pos);
			const BYTE type = pGeom->Gettype();
			ar << type;
			pGeom->Serialize(ar, dwVersion);
		}
	}
	else
	{
		int nCount;
		ar >> nCount;
		for(int index = 0; index < nCount; index++)
		{
			BYTE type;
			ar >> type;
			CGeom* pGeom = CGeom::Apply(type);
			pGeom->Serialize(ar, dwVersion);
			m_geoms.AddTail(pGeom);
		}
	}
}
void CLaylg::Rematch(CDatabase* pDatabase, CATTHeaderProfile& profile)
{
	std::map<DWORD, CGeom*> geomindex;
	POSITION pos = m_geoms.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geoms.GetNext(pos);
		if(pGeom->m_attId == 0XFFFFFFFF)
			continue;

		geomindex[pGeom->m_attId] = pGeom;
	}
	m_ifindex.clear();

	BYTE index = 0;
	CLayif* pLayif = (CLayif*)this->FirstChild();
	while(pLayif!=nullptr)
	{
		pLayif->m_geomlist.RemoveAll();
		if(pLayif->m_strIf.IsEmpty() == FALSE)
		{
			CString strSQL = profile.MakeSelectStatment(profile.m_strIdField, (CString)pLayif->m_strIf);
			CODBCDatabase::ParseSQL(pDatabase, strSQL);
			try
			{
				CODBCRecordset rs(pDatabase);
				rs.Open(strSQL, CRecordset::snapshot, CRecordset::readOnly);
				while(rs.IsEOF() == FALSE)
				{
					CString strGeomID;
					rs.GetFieldValue(profile.m_strIdField, strGeomID);
					const DWORD dwGeom = _ttol(strGeomID);
					auto pos = geomindex.find(dwGeom);
					if(pos != geomindex.end())
					{
						CGeom* pGeom = pos->second;
						if(pGeom!= nullptr)
						{
							pLayif->m_geomlist.AddTail(pGeom);
							m_ifindex[dwGeom] = index;
						}
					}

					rs.MoveNext();
				}
				rs.Close();
			}
			catch(CDBException* ex)
			{
				OutputDebugString(ex->m_strError);
				ex->ReportError();
				ex->Delete();
			}
		}
		pLayif = (CLayif*)pLayif->Nextsibling();
	}
}