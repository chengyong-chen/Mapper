#include "stdafx.h"
#include "Resource.h"

#include "../Mapper/Global.h"

#include "../Geometry/Geom.h"

#include "Tool.h"
#include "InfoTool.h"

__declspec(dllexport) CInfoTool infoTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CInfoTool::OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonDown(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	pWnd->SendMessage(WM_SHOWRECINFO, docPoint.x, docPoint.y);
}

bool CInfoTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_INFO);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
