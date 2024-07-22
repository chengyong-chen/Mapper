#include "stdafx.h"
#include "Global.h"

#include "Tool.h"
#include "RRectTool.h"

#include "../Geometry/Geom.h"
#include "../Geometry/RRect.h"
#include "../Dataview/viewinfo.h"

__declspec(dllexport) CRRectTool rrectTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CRRectTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if(pWnd->GetCapture()==pWnd&&docPoint!=CTool::c_docDown)
	{
		const bool bSquare = (nFlags&MK_SHIFT)!=0 ? true : false;
		const bool bCentral = (nFlags&MK_CONTROL)!=0 ? true : false;
		const CRect rect = CGeom::RegulizeRect(CTool::c_docDown, docPoint, bSquare, bCentral);
		CRRect* pGeom = new CRRect(rect, nullptr, nullptr);
		pWnd->SendMessage(WM_NEWGEOMDREW, (DWORD)pGeom, 0);
	}

	return CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CRRectTool::DrawTrack(CWnd* pWnd, const CViewinfo& viewinfo, CPoint point1, CPoint point2, bool bSquare, bool bCentral)
{
	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	int OldROP = dc.SetROP2(R2_NOTXORPEN);
	const CRect docRect = CGeom::RegulizeRect(point1, point2, bSquare, bCentral);
	const Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(docRect);;

	dc.RoundRect(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom(), 5, 5);
}
