#include "stdafx.h"
#include "Resource.h"
#include "Global.h"

#include "Tool.h"
#include "PRectTool.h"

#include "../Geometry/Geom.h"
#include "../Geometry/PRect.h"
#include "../Dataview/viewinfo.h"

__declspec(dllexport) CPRectTool prectTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CPRectTool::OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonDown(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	pWnd->SendMessage(WM_COMMAND, 33068, 0);
}

void CPRectTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if((nFlags&MK_LBUTTON)!=MK_LBUTTON)
		return CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
	if(docPoint==c_docLast)
		return CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
	const bool bSquare = (nFlags&MK_SHIFT)!=0 ? true : false;
	const bool bCentral = (nFlags&MK_CONTROL)==MK_CONTROL ? true : false;
	DrawTrack(pWnd, viewinfo, c_docDown, c_docLast, bSquare, bCentral);
	DrawTrack(pWnd, viewinfo, c_docDown, docPoint, bSquare, bCentral);

	return CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CPRectTool::OnKeyDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nChar, UINT nRepCnt, UINT nFlags, CTool*& pTool, CTool*& oldTool)
{
	CTool::OnKeyDown(pWnd, viewinfo, nChar, nRepCnt, nFlags, pTool, oldTool);

	if((nFlags&MK_LBUTTON)!=MK_LBUTTON)
		return;
	if((nFlags&0X4000)==0X4000)
		return;

	switch(nChar&0x00FF)
	{
	case VK_CONTROL:
	{
		const bool bSquare = ::GetKeyState(VK_SHIFT)<0 ? true : false;
		DrawTrack(pWnd, viewinfo, c_docDown, c_docLast, bSquare, false);
		DrawTrack(pWnd, viewinfo, c_docDown, c_docLast, bSquare, true);
	}
	break;
	case VK_SHIFT:
	{
		const bool bCentral = ::GetKeyState(VK_CONTROL)<0 ? true : false;
		DrawTrack(pWnd, viewinfo, c_docDown, c_docLast, false, bCentral);
		DrawTrack(pWnd, viewinfo, c_docDown, c_docLast, true, bCentral);
	}
	break;
	}
}

void CPRectTool::OnKeyUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nChar, UINT nRepCnt, UINT nFlags, CTool*& pTool, CTool*& oldTool)
{
	CTool::OnKeyUp(pWnd, viewinfo, nChar, nRepCnt, nFlags, pTool, oldTool);

	if((nFlags&MK_LBUTTON)!=MK_LBUTTON)
		return;

	switch(nChar)
	{
	case VK_CONTROL:
	{
		const bool bSquare = ::GetKeyState(VK_SHIFT)<0 ? true : false;
		DrawTrack(pWnd, viewinfo, c_docDown, c_docLast, bSquare, true);
		DrawTrack(pWnd, viewinfo, c_docDown, c_docLast, bSquare, false);
	}
	break;
	case VK_SHIFT:
	{
		const bool bCentral = ::GetKeyState(VK_CONTROL)<0 ? true : false;
		DrawTrack(pWnd, viewinfo, c_docDown, c_docLast, true, bCentral);
		DrawTrack(pWnd, viewinfo, c_docDown, c_docLast, false, bCentral);
	}
	break;
	}
}

void CPRectTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if(pWnd->GetCapture()==pWnd&&docPoint!=c_docDown)
	{
		const bool bSquare = (nFlags&MK_SHIFT)!=0 ? true : false;
		const bool bCentral = (nFlags&MK_CONTROL)!=0 ? true : false;
		const CRect rect = CGeom::RegulizeRect(c_docDown, docPoint, bSquare, bCentral);
		CPRect* pGeom = new CPRect(rect, nullptr, nullptr);
		pWnd->SendMessage(WM_NEWGEOMDREW, (DWORD)pGeom, 0);
	}

	return CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

bool CPRectTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_MYCROSS);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}

void CPRectTool::DrawTrack(CWnd* pWnd, const CViewinfo& viewinfo, CPoint point1, CPoint point2, bool bSquare, bool bCentral)
{
	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	int OldROP = dc.SetROP2(R2_NOTXORPEN);
	const CRect docRect = CGeom::RegulizeRect(point1, point2, bSquare, bCentral);
	const Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(docRect);;

	dc.Rectangle(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());
}
