#include "stdafx.h"
#include "Resource.h"

#include "../Mapper/Global.h"

#include "../Geometry/Geom.h"

#include "Tool.h"
#include "InjectTool.h"

__declspec(dllexport) CInjectTool injectTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CInjectTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	pWnd->SendMessage(WM_INJECTATT, docPoint.x, docPoint.y);
}

bool CInjectTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_INJECT);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
