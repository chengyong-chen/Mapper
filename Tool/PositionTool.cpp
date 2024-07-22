#include "stdafx.h"
#include "Resource.h"

#include "../Mapper/Global.h"

#include "../Geometry/Geom.h"

#include "Tool.h"
#include "PositionTool.h"

__declspec(dllexport) CPositionTool positionTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CPositionTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	pWnd->SendMessage(WM_POSITIONPOU, docPoint.x, docPoint.y);
}

bool CPositionTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_POSITION);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
