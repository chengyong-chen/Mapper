#include "stdafx.h"
#include "Resource.h"

#include "Tool.h"
#include "SpinTool.h"

#include "../DataView/ViewMonitor.h"

#include "../Mapper/Global.h"

__declspec(dllexport) CSpinTool spinTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSpinTool::CSpinTool()
{
}

void CSpinTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);
	if(viewinfo.m_datainfo.IsLngLat())
	{
		CPointF geoPoint1 = viewinfo.m_datainfo.DocToGeo(docPoint);
		CPointF geoPoint2;
		viewinfo.m_datainfo.DocToGeo(c_docDown.x, c_docDown.y, geoPoint2.x, geoPoint2.y);
		const CSize ms((geoPoint1.x-geoPoint2.x)*100000000, (geoPoint1.y-geoPoint2.y)*100000000);
		if(ms == CSize(0, 0))
		{
		}
		else if(pWnd->IsKindOf(RUNTIME_CLASS(CView)) == TRUE)
		{
			((CView*)pWnd)->OnScrollBy(ms, 2);
		}
		else
		{
			pWnd->SendMessage(WM_SCROLLVIEW, ms.cx, ms.cy);
		}
	}
}

bool CSpinTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_ROTATE);
	if(hCursor != nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
