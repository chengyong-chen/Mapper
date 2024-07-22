#include "stdafx.h"
#include "Resource.h"

#include "Global.h"

#include "Tool.h"
#include "RouteTool.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"

#include "..\Navigate\Resource.h"
#include "..\Navigate\RouteDlg.h"

__declspec(dllexport) CRoadTool roadTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CRoadTool::OnLButtonDown(CWnd* pWnd, CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint)
{
	CTool::OnLButtonDown(pWnd, viewinfo, nFlags, docPoint);

	if(m_pGPSRoute == nullptr)
	{
		m_pGPSRoute = new CGPSRoute;

		m_pGPSRouteDlg->AddRoute(m_pGPSRoute);
	}


	CGPSHandle* fAnchor = nullptr;
	if(m_pGPSRoute->m_handlelist.GetCount() > 0)
		fAnchor = m_pGPSRoute->m_handlelist.GetTail();

	CGPSHandle* pHandle = m_pGPSRouteDlg->AddAnchor(pWnd, docPoint);
	m_pGPSRoute->m_handlelist.AddTail(pHandle);

	CClientDC dc(pWnd);
	dc.SetMapMode(MM_TEXT);
	dc.SetViewportOrg(0, 0);
	dc.SetWindowOrg(0, 0);

	if(fAnchor != nullptr)
	{
		CPointF geoMove = fAnchor->m_point;;
		CPointF mapMove;
		CPoint docMove;

		pWnd->SendMessage(WM_GEOTOMAP, (UINT)(&geoMove), (LONG)(&mapMove));
		pWnd->SendMessage(WM_MAPTODOC, (UINT)(&mapMove), (LONG)(&docMove));
		pWnd->SendMessage(WM_DOCTOCLIENT, (UINT)&docMove, 1);

		CPointF geoLine = pHandle->m_point;
		CPointF mapLine;
		CPoint docLine;

		pWnd->SendMessage(WM_GEOTOMAP, (UINT)(&geoLine), (LONG)(&mapLine));
		pWnd->SendMessage(WM_MAPTODOC, (UINT)(&mapLine), (LONG)(&docLine));
		pWnd->SendMessage(WM_DOCTOCLIENT, (UINT)&docLine, 1);

		CPen pen;
		pen.CreatePen(PS_SOLID, 7, RGB(255, 255, 0));
		CPen* OldPen = (CPen*)dc.SelectObject(&pen);
		dc.MoveTo(docMove);
		dc.LineTo(docLine);
		dc.SelectObject(OldPen);
		pen.DeleteObject();

		OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
		dc.MoveTo(docMove);
		dc.LineTo(docLine);
		dc.SelectObject(OldPen);

		fAnchor->Draw(pWnd, &dc);
	}

	m_pGPSRouteDlg->SetRouteName(m_pGPSRoute);
	pHandle->Draw(pWnd, &dc);
}

void CRoadTool::OnMouseMove(CWnd* pWnd, CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if(m_pGPSRoute != nullptr)
	{
		CGPSHandle* fAnchor = nullptr;
		if(m_pGPSRoute->m_handlelist.GetCount() > 0)
			fAnchor = m_pGPSRoute->m_handlelist.GetTail();
		if(fAnchor == nullptr)
		{
			CTool::OnMouseMove(pWnd, viewinfo, nFlags, docPoint);
			return;
		}

		CPointF geoMove = fAnchor->m_point;;
		CPointF mapMove;
		CPoint docMove;

		pWnd->SendMessage(WM_GEOTOMAP, (UINT)(&geoMove), (LONG)(&mapMove));
		pWnd->SendMessage(WM_MAPTODOC, (UINT)(&mapMove), (LONG)(&docMove));
		pWnd->SendMessage(WM_DOCTOCLIENT, (UINT)&docMove, 1);

		CClientDC dc(pWnd);
		dc.SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0, 0);
		long oldROP = dc.SetROP2(R2_NOTXORPEN);

		CPen pen;
		pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
		CPen* OldPen = (CPen*)dc.SelectObject(&pen);

		CPoint cliLast = c_docLast;
		pWnd->SendMessage(WM_DOCTOCLIENT, (UINT)&cliLast, 1);
		dc.MoveTo(docMove);
		dc.LineTo(cliLast);

		CPoint cliLine = docPoint;
		pWnd->SendMessage(WM_DOCTOCLIENT, (UINT)&cliLine, 1);
		dc.MoveTo(docMove);
		dc.LineTo(cliLine);

		dc.SetROP2(oldROP);
		dc.SelectObject(OldPen);

		pen.DeleteObject();
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, docPoint);
}

void CRoadTool::OnLButtonDblClk(CWnd* pWnd, CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint)
{
	if(m_pGPSRoute != nullptr)
	{
		long nPoints = m_pGPSRoute->m_handlelist.GetCount();
		if(nPoints>=2)
		{
			CPoly* poly = new CPoly;

			poly->m_pPoints = new CPoint[nPoints];
			poly->m_nAnchors = nPoints;
			poly->m_nAllocs = nPoints;

			int i = 0;
			POSITION pos = m_pGPSRoute->m_handlelist.GetHeadPosition();
			while(pos != nullptr)
			{
				CGPSHandle* handle = m_pGPSRoute->m_handlelist.GetNext(pos);
				CPointF geoMove = handle->m_point;;
				CPointF mapMove;
				CPoint docMove;

				pWnd->SendMessage(WM_GEOTOMAP, (UINT)(&geoMove), (LONG)(&mapMove));
				pWnd->SendMessage(WM_MAPTODOC, (UINT)(&mapMove), (LONG)(&docMove));

				poly->m_pPoints[i++] = docMove;
			}

			double length = poly->GetLength(pWnd);
			delete poly;

			m_pGPSRouteDlg->SetRouteLength(m_pGPSRoute, length);
		}

		m_pGPSRoute = nullptr;
	}

	CTool::OnLButtonDblClk(pWnd, viewinfo, nFlags, docPoint);
}

bool CRoadTool::SetCursor(CWnd* pWnd, CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_ROUTE);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
