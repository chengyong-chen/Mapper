#include "stdafx.h"
#include "Resource.h"
#include "Global.h"
#include "Tool.h"
#include "SplitTool.h"

CSplitTool splitTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CSplitTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	pWnd->SendMessage(WM_SPLITGEOM, 0, (LONG)&docPoint);
}

bool CSplitTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_MYCROSS);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);;
		return true;
	}
	else
		return false;
}
