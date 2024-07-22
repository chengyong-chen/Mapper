#include "stdafx.h"
#include "Laypt.h"
#include "LayerTree.h"

#include "../Style/Spot.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Tag.h"

#include "../Public/ODBCDatabase.h"
#include "../Dataview/viewinfo.h"
#include "../Database/ODBCRecordset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLaypt::CLaypt(CTree<CLayer>& tree)
	: CLaydb(tree)
{
}

CLaypt::CLaypt(CTree<CLayer>& tree, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag)
	: CLaydb(tree, minLevelObj, maxLevelObj, minLevelTag, maxLevelTag)
{
}

CLaypt::~CLaypt()
{
}

void CLaypt::Reload(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRectF& mapRect)
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

	const bool bDrawObj = (m_bShowGeom == false || viewinfo.m_levelCurrent<m_minLevelObj || viewinfo.m_levelCurrent >= m_maxLevelObj) ? false : true;
	const bool bDrawTag = (m_bShowTag == false || viewinfo.m_levelCurrent< m_minLevelTag || viewinfo.m_levelCurrent >= m_maxLevelTag) ? false : true;
	if(bDrawObj==false&&bDrawTag==false)
		return;

	CString strBasic = m_geoDatasource.GetHeaderProfile().MakeSelectStatment();
	if(strBasic==_T(""))
		return;

	strBasic.Replace(_T("A.*"), _T("A.ID,A.X,A.Y"));
	CString strSQL;
	strSQL.Format(_T("%s A.X<=%g And A.X>=%g And A.Y<=%g And A.Y>=%g"), strBasic, mapRect.right, mapRect.left, mapRect.bottom, mapRect.top);
	try
	{
		const float scaleMark = (m_bDynamic&SpotDynamic)==SpotDynamic ? viewinfo.CurrentRatio() : 1.0f;
		CRect wanRect = CRect(10000, 10000, 20000, 20000);
		CSpot* pSpot = this->GetSpot();
		CRect docRect = pSpot!=nullptr ? pSpot->GetDocRect(viewinfo.m_datainfo, m_tree.m_library) : datainfo.WanToDoc(CRect(10000, 10000, 20000, 20000));
		docRect.left *= scaleMark;
		docRect.top *= scaleMark;
		docRect.right *= scaleMark;
		docRect.bottom *= scaleMark;

		CODBCRecordset rs(pDatabase);
		rs.Open(strSQL, CRecordset::snapshot, CRecordset::readOnly);
		while(rs.IsEOF()==FALSE)
		{
			CMark* pMark = new CMark();
			pMark->GetValues(rs, datainfo, m_geoDatasource.GetHeaderProfile().m_strAnnoField);
			pMark->m_Rect = docRect;
			pMark->m_Rect.OffsetRect(pMark->m_Origin.x, pMark->m_Origin.y);

			if(pMark->m_strName.IsEmpty()==FALSE)
			{
				pMark->m_pTag = new CTag(pMark->m_strName, ANCHOR_6);
			}

			m_geomlist.AddTail(pMark);

			rs.MoveNext();
		}
		rs.Close();

		m_lastRect = mapRect;
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
}
