#include "stdafx.h"

#include "Tool.h"
#include "RoundTool.h"

#include "ExtentDlg.h"

#include "../Viewer/Global.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Ellipse.h"

__declspec(dllexport) CRoundTool roundTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long CRoundTool::Extent = 1000;

void CRoundTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	CRect rect(docPoint, docPoint);
	rect.InflateRect(Extent, Extent);

	pWnd->SendMessage(WM_RGNQUERY, (UINT)(&rect), 2);
}

bool CRoundTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
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
