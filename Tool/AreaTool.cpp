#include "stdafx.h"
#include "Resource.h"

#include "Tool.h"
#include "BoundTool.h"
#include "AreaTool.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Dataview/viewinfo.h"

__declspec(dllexport) CAreaTool areaTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CAreaTool::OnLButtonDblClk(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint)
{
	const long nPoints = m_points.GetSize();
	if(nPoints>=3)
	{
		CPoly* poly = new CPoly;
		poly->m_bClosed = true;
		poly->m_pPoints = new CPoint[nPoints+1];
		for(unsigned int i = 0; i<nPoints; i++)
		{
			poly->m_pPoints[i].x = m_points[i].x;
			poly->m_pPoints[i].y = m_points[i].y;
		}

		poly->m_nAnchors = nPoints+1;
		poly->m_nAllocs = nPoints+1;

		poly->m_pPoints[nPoints] = poly->m_pPoints[0];
		poly->RecalRect();

		double area = viewinfo.m_datainfo.CalArea(poly->m_pPoints, poly->m_nAnchors);
		area = std::abs(area);

		CString str;
		if(area>0.01)
		{
			CString r;
			r.Format(_T("%.2f"), area);
			r.TrimRight(_T("0"));
			r.TrimRight(_T("."));
			str.LoadString(IDS_AREA1);
			str.Replace(_T("%s"), r);
		}
		else
		{
			CString r;
			r.Format(_T("%.0f"), area*1000000);

			str.LoadString(IDS_AREA2);
			str.Replace(_T("%s"), r);
		}

		AfxMessageBox(str);

		poly->Invalidate(pWnd, viewinfo, 1);
		delete poly;
	}

	m_points.RemoveAll();

	CTool::OnLButtonDblClk(pWnd, viewinfo, nFlags, docPoint);
}

bool CAreaTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_AREA);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
