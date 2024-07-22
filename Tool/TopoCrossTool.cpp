#include "stdafx.h"
#include "Resource.h"

#include "GLobal.h"
#include "Tool.h"
#include "TopoCrossTool.h"

#include "../Geometry/Global.h"
#include "../Dataview/viewinfo.h"

__declspec(dllexport) CTopoCrossTool topoCrossTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CTopoCrossTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
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
			CSize size = c_docLast-c_docDown;
			size.cx = abs(size.cx);
			size.cy = abs(size.cy);
			if(size.cx>size.cy)
				size.cy = size.cx;
			else
				size.cx = size.cy;

			CRect docRect = CRect(c_docDown.x, c_docDown.y, c_docDown.x, c_docDown.y);
			docRect.InflateRect(size);
			const Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(docRect);
			dc.Ellipse(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());
		}

		if(docPoint!=c_docDown)
		{
			CSize size = docPoint-c_docDown;
			size.cx = abs(size.cx);
			size.cy = abs(size.cy);
			if(size.cx>size.cy)
				size.cy = size.cx;
			else
				size.cx = size.cy;

			CRect docRect = CRect(c_docDown.x, c_docDown.y, c_docDown.x, c_docDown.y);
			docRect.InflateRect(size);
			const Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(docRect);
			dc.Ellipse(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());
		}

		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CTopoCrossTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
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
		CSize size = c_docLast-c_docDown;
		size.cx = abs(size.cx);
		size.cy = abs(size.cy);
		if(size.cx>size.cy)
			size.cy = size.cx;
		else
			size.cx = size.cy;

		CRect docRect = CRect(c_docDown.x, c_docDown.y, c_docDown.x, c_docDown.y);
		docRect.InflateRect(size);
		const Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(docRect);
		dc.Ellipse(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());
		const int nRadius = ::LineLength1(c_docDown, docPoint);
		pWnd->SendMessage(WM_TOPO_CROSS, (UINT)&c_docDown, nRadius);
	}

	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
}

void CTopoCrossTool::Draw(CWnd* pWnd, const CViewinfo& viewinfo, CDC* pDC)
{
	if((GetAsyncKeyState(VK_LBUTTON)&0X8000)==0X8000)// VK_LBUTTON is  down
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
			CSize size = c_docLast-c_docDown;
			size.cx = abs(size.cx);
			size.cy = abs(size.cy);
			if(size.cx>size.cy)
				size.cy = size.cx;
			else
				size.cx = size.cy;

			CRect docRect = CRect(c_docDown.x, c_docDown.y, c_docDown.x, c_docDown.y);
			docRect.InflateRect(size);
			const Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(docRect);
			dc.Ellipse(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());
		}

		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);
	}

	CTool::Draw(pWnd, viewinfo, pDC);
}

bool CTopoCrossTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_TOPO_CROSS);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
