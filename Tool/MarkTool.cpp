#include "stdafx.h"
#include "Resource.h"
#include "Global.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Mark.h"

#include "Tool.h"
#include "MarkTool.h"

__declspec(dllexport) CMarkTool markTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CMarkTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	pWnd->SendMessage(WM_COMMAND, 33068, 0);
	CMark* mark = new CMark(docPoint);
	pWnd->SendMessage(WM_NEWGEOMDREW, (LONG)mark, 0);
}

bool CMarkTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_MARK);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);;
		return true;
	}
	else
		return false;
}
