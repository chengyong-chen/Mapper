#include "stdafx.h"
#include "Global.h"

#include "Tool.h"
#include "InsertTool.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Mark.h"

#include "Resource.h"

__declspec(dllexport) CInsertTool insertTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CInsertTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	pWnd->SendMessage(WM_COMMAND, 33068, 0);
	CMark* mark = new CMark(docPoint);
	pWnd->SendMessage(WM_NEWGEOMDREW, (UINT)mark, 0);
}

bool CInsertTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_INSERT);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
