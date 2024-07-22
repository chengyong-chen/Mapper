#include "stdafx.h"
#include "Resource.h"

#include "Global.h"
#include "..\Mapper\Global.h"
#include "../Atlas/Global.h"

#include "Tool.h"
#include "TrafficTool.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CTrafficTool::OnLButtonDown(CWnd* pWnd, UINT nFlags, const CPoint& point)
{	
	CTool::OnLButtonDown(pWnd, nFlags, point);
	
	if(pWnd->IsKindOf(RUNTIME_CLASS(CView)) == FALSE)
	{
		CPoint docPoint = point;
		CPointF mapPoint;
		static CPointF geoPoint;
		pWnd->SendMessage(WM_DOCTOMAP,(UINT)(&docPoint),(UINT)(&mapPoint));
		pWnd->SendMessage(WM_MAPTOGEO,(UINT)(&mapPoint),(UINT)(&geoPoint));

		if(m_bStart == true)
		{
			pWnd->SendMessage(WM_TRAFFICCLICK,(UINT)&geoPoint,1);
			m_bStart = false;
		}
		else
		{
			pWnd->SendMessage(WM_TRAFFICCLICK,(UINT)&geoPoint,2);
			CTool::OnLButtonUp(pWnd,nFlags, cliPoint, point);
		}
	}
}

void CTrafficTool::OnLButtonUp(CWnd* pWnd, UINT nFlags, const CPoint& point)
{	
	CTool::OnLButtonUp(pWnd, nFlags, cliPoint, point);

	if(pWnd->IsKindOf(RUNTIME_CLASS(CView)) == TRUE)
	{
		CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
		if(pMainFrame != nullptr)
		{
			pMainFrame->SendMessage(WM_TRAFFICCLICK,(UINT)pWnd,(LONG)&point);
		}
	}
}

bool CTrafficTool::SetCursor(CWnd* pWnd,CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(pWnd->IsKindOf(RUNTIME_CLASS(CView)) == TRUE)
	{
		HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_ROUTE);	
		if(hCursor != nullptr) 
		{
			::SetCursor(hCursor);
			return true;
		}		
		else
			return false;
	}
	else
	{
		HCURSOR hCursor = nullptr;
		if(m_bStart == true)
		{
			hCursor = AfxGetApp()->LoadCursor(IDC_ROUTE);
		}
		else
		{
			hCursor = AfxGetApp()->LoadCursor(IDC_ROUTE);
		}

		if(hCursor != nullptr) 
		{
			::SetCursor(hCursor);
			return true;
		}
		else
			return false;
	}
}
