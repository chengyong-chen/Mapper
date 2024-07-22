#include "stdafx.h"
#include "Resource.h"

#include "Global.h"

#include "Tool.h"
#include "RouteTool.h"

#include "../Geometry/Geom.h"
#include "../Dataview/viewinfo.h"

__declspec(dllexport) CRouteTool routeTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CRouteTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	UINT SelectionMade = 0;
	CMenu Menu;
	if(Menu.LoadMenu(IDR_ROUTE))
	{
		CMenu* pSubMenu = Menu.GetSubMenu(0);
		if(pSubMenu!=nullptr)
		{
			const Gdiplus::Point cliPoint1 = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
			CPoint cliPoint2(cliPoint1.X, cliPoint1.Y);
			pWnd->ClientToScreen(&cliPoint2);

			SelectionMade = pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, cliPoint2.x, cliPoint2.y, pWnd);

			pSubMenu->DestroyMenu();
		}

		Menu.DestroyMenu();
	}

	if(pWnd!=nullptr)
	{
		CPointF geoPoint = viewinfo.m_datainfo.DocToGeo(docPoint);
		switch(SelectionMade)
		{
		case ID_ROUTE_START:
			pWnd->SendMessage(WM_SETROUTE, 1, (LONG)&geoPoint);
			break;
		case ID_ROUTE_END:
			pWnd->SendMessage(WM_SETROUTE, 2, (LONG)&geoPoint);
			break;
		case ID_ROUTE_PASS:
			pWnd->SendMessage(WM_SETROUTE, 3, (LONG)&geoPoint);
			break;
		case ID_ROUTE_SEARCH:
			pWnd->SendMessage(WM_SETROUTE, 4, (LONG)&geoPoint);
			break;
		case ID_ROUTE_CLEAR:
			pWnd->SendMessage(WM_SETROUTE, 5, (LONG)&geoPoint);
			break;
		}
	}
}

bool CRouteTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_ROUTE);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
