#include "stdafx.h"
#include "Laypy.h"
#include "LayerTree.h"

#include "../Geometry/Geom.h"
#include "../Dataview/Viewinfo.h"
#include "../Public/ODBCDatabase.h"
#include "../Database/ODBCRecordset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLaypy::CLaypy(CTree<CLayer>& tree)
	: CLaydb(tree)
{
}

CLaypy::CLaypy(CTree<CLayer>& layertree, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag)
	: CLaydb(layertree, minLevelObj, maxLevelObj, minLevelTag, maxLevelTag)
{
}

CLaypy::~CLaypy()
{
}

void CLaypy::Reload(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRectF& mapRect)
{
	CLaydb::Reload(pWnd, datainfo, viewinfo, mapRect);

	if(m_bVisible==false)
		return;
	if(mapRect==m_lastRect)
		return;
	CODBCDatabase* pDatabase = m_tree.GetGeoDatabase(m_geoDatasource.m_strDatabase);
	if(pDatabase==nullptr)
		return;
	if(pDatabase->IsOpen()==FALSE)
		return;

	const bool bDrawGeom = (m_bShowGeom==false||viewinfo.m_levelCurrent<m_minLevelObj||viewinfo.m_levelCurrent>=m_maxLevelObj) ? false : true;
	const bool bDrawTag = (m_bTagShow==false||viewinfo.m_levelCurrent<m_minLevelTag||viewinfo.m_levelCurrent>=m_maxLevelTag) ? false : true;
	if(bDrawGeom==false&&bDrawTag==false)
		return;

	CString strBasic = m_geoDatasource.GetHeaderProfile().MakeSelectStatment();
	if(strBasic==_T(""))
		return;
	strBasic.Replace(_T("A.*"), _T("A.ID,A.Shape,A.minX,A.maxX,A.minY,A.maxY,A.Closed,A.Handles,A.Points"));
	CString strSQL;
	strSQL.Format(_T("%s A.minX<=%g And A.maxX>=%g And A.minY<=%g And A.maxY>=%g"), strBasic, mapRect.right, mapRect.left, mapRect.bottom, mapRect.top);

	try
	{
		CODBCRecordset rs(pDatabase);
		rs.Open(strSQL, CRecordset::snapshot, CRecordset::readOnly);
		while(rs.IsEOF()==FALSE)
		{
			CGeom* pGeom = CGeom::Load(rs, datainfo, m_geoDatasource.GetHeaderProfile().m_strAnnoField);
			if(pGeom!=nullptr)
			{
				m_geomlist.AddTail(pGeom);
			}

			rs.MoveNext();
		}
		rs.Close();
	}
	catch(CDBException*ex)
	{
		//	AfxMessageBox(ex->m_strError);
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}
	catch(CException*ex)
	{
		ex->Delete();
	}
	m_lastRect = mapRect;
}
