#include "stdafx.h"
#include "Resource.h"

#include "Tool.h"
#include "MarqueeTool.h"

#include "../Viewer/Global.h"

#include "../Dataview/Viewinfo.h"

__declspec(dllexport) CMarqueeTool marqueeTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CMarqueeTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if((nFlags&MK_LBUTTON)==MK_LBUTTON)
	{
		CClientDC dc(pWnd);
		dc.SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0, 0);

		CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
		CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
		const int OldROP = dc.SetROP2(R2_NOTXORPEN);

		if(c_docLast!=c_docDown)
		{
			CRect docRect = CRect(c_docDown.x, c_docDown.y, c_docDown.x, c_docDown.y);
			docRect.InflateRect(abs(c_docLast.x-c_docDown.x), abs(c_docLast.y-c_docDown.y));
			const Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(docRect);
			dc.Rectangle(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());
		}

		if(docPoint!=c_docDown)
		{
			CRect docRect = CRect(c_docDown.x, c_docDown.y, c_docDown.x, c_docDown.y);
			docRect.InflateRect(abs(docPoint.x-c_docDown.x), abs(docPoint.y-c_docDown.y));
			const Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(docRect);
			dc.Rectangle(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());
		}

		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);
	}
	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CMarqueeTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	CClientDC dc(pWnd);
	dc.SetMapMode(MM_TEXT);
	dc.SetViewportOrg(0, 0);
	dc.SetWindowOrg(0, 0);

	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);

	if(c_docLast!=c_docDown)
	{
		CRect docRect = CRect(c_docDown.x, c_docDown.y, c_docDown.x, c_docDown.y);
		docRect.InflateRect(abs(c_docLast.x-c_docDown.x), abs(c_docLast.y-c_docDown.y));
		const Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(docRect);
		dc.Rectangle(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());

		pWnd->SendMessage(WM_RGNQUERY, (LONG)&docRect, 1);
	}

	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
}

void CMarqueeTool::Draw(CWnd* pWnd, const CViewinfo& viewinfo, CDC* DC)
{
	if((GetAsyncKeyState(VK_LBUTTON)&0X8000)==0X8000)// VK_LBUTTON is not down
	{
		CClientDC dc(pWnd);
		dc.SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0, 0);

		CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
		CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
		const int OldROP = dc.SetROP2(R2_NOTXORPEN);

		if(c_docLast!=c_docDown)
		{
			CRect docRect = CRect(c_docDown.x, c_docDown.y, c_docDown.x, c_docDown.y);
			docRect.InflateRect(abs(c_docLast.x-c_docDown.x), abs(c_docLast.y-c_docDown.y));
			const Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(docRect);
			dc.Rectangle(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());
		}

		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);
	}
}

bool CMarqueeTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
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
