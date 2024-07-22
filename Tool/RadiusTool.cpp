#include "stdafx.h"
#include "Resource.h"

#include "Tool.h"
#include "RadiusTool.h"

#include "../Viewer/Global.h"
#include "../Public/Global.h"
#include "../Dataview/Datainfo.h"
#include "../Dataview/Viewinfo.h"

__declspec(dllexport) CRadiusTool radiusTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CRadiusTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
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
			const int X = cliRect.X+cliRect.Width/2;
			const int Y = cliRect.Y+cliRect.Height/2;
			dc.MoveTo(cliRect.GetLeft(), Y);
			dc.LineTo(cliRect.GetRight(), Y);
			dc.MoveTo(X, cliRect.GetTop());
			dc.LineTo(X, cliRect.GetBottom());
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
			CPoint center;
			center.x = cliRect.X+cliRect.Width/2;
			center.y = cliRect.Y+cliRect.Height/2;

			dc.MoveTo(cliRect.GetLeft(), center.y);
			dc.LineTo(cliRect.GetRight(), center.y);

			dc.MoveTo(center.x, cliRect.GetTop());
			dc.LineTo(center.x, cliRect.GetBottom());

			CPoint points[2];
			points[0] = CPoint(c_docDown.x, c_docDown.y);
			points[1] = CPoint(docPoint.x, docPoint.y);
			double length = viewinfo.m_datainfo.CalLength(points, 2);
			CString strRadius;
			if(length>1)
			{
				strRadius.Format(_T("%.1fǧ��"), length);
			}
			else
			{
				length = length*1000;
				strRadius.Format(_T("%.0f��"), length);
			}
			const UINT oldAlign = dc.SetTextAlign(TA_BOTTOM|TA_CENTER);
			dc.TextOut(center.x, center.y, strRadius);
			dc.SetTextAlign(oldAlign);
		}

		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CRadiusTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
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
		const int X = cliRect.X+cliRect.Width/2;
		const int Y = cliRect.Y+cliRect.Height/2;
		dc.MoveTo(cliRect.GetLeft(), Y);
		dc.LineTo(cliRect.GetRight(), Y);

		dc.MoveTo(X, cliRect.GetTop());
		dc.LineTo(X, cliRect.GetBottom());

		pWnd->SendMessage(WM_RGNQUERY, (LONG)&docRect, 2);
		/*	{
				CPoint center = rect.CenterPoint();
				DWORD radius = rect.Width()/2;
				pWnd->SendMessage(WM_PICKBYCIRCLE,(UINT)&center,radius);
			}*/
	}

	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
}

void CRadiusTool::Draw(CWnd* pWnd, const CViewinfo& viewinfo, CDC* pDC)
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
			const int X = cliRect.X+cliRect.Width/2;
			const int Y = cliRect.Y+cliRect.Height/2;
			dc.MoveTo(cliRect.GetLeft(), Y);
			dc.LineTo(cliRect.GetRight(), Y);
			dc.MoveTo(X, cliRect.GetTop());
			dc.LineTo(X, cliRect.GetBottom());
		}

		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);
	}

	CTool::Draw(pWnd, viewinfo, pDC);
}

bool CRadiusTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_ROUND);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
