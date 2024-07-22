#include "stdafx.h"
#include "resource.h"
#include "Global.h"

#include "Tool.h"
#include "RegionTool.h"

#include "../Viewer/Global.h"
#include "../Geometry/Geom.h"

__declspec(dllexport) CRegionTool regionTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CRegionTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	CGeom* pGeom = (CGeom*)pWnd->SendMessage(WM_LOOKUPGEOM, (LONG)&docPoint, 0);
	if(pGeom!=nullptr&&pGeom->m_bClosed==true)
	{
		CLine* oldline = pGeom->m_pLine;
		CFillCompact* oldfill = pGeom->m_pFill;
		CClientDC dc(pWnd);

		CBrush* oldbrush = (CBrush*)dc.SelectStockObject(BLACK_BRUSH);
		CPen* oldpen = (CPen*)dc.SelectStockObject(NULL_PEN);

		dc.SelectObject(oldbrush);
		dc.SelectObject(oldpen);

		if(pGeom->m_bType==1)
		{
			pWnd->SendMessage(WM_RGNQUERY, (UINT)(pGeom), 0);
		}
		else if(pGeom->m_bType==7)
		{
			CRect rect = pGeom->m_Rect;
			pWnd->SendMessage(WM_RGNQUERY, (UINT)(&rect), 1);
		}
		else if(pGeom->m_bType==13)
		{
			CRect rect = pGeom->m_Rect;
			pWnd->SendMessage(WM_RGNQUERY, (UINT)(&rect), 2);
		}
	}
}

bool CRegionTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_MYCROSS);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
