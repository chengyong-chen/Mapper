#include "stdafx.h"
#include "Resource.h"

#include "GLobal.h"
#include "Tool.h"
#include "NodeTool.h"

#include "../Viewer/Global.h"
#include "..\Mapper\Global.h"
#include "../Atlas/Global.h"


__declspec(dllexport) CNodeTool nodeTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CNodeTool::OnMouseMove(CWnd* pWnd, CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if((nFlags & MK_LBUTTON) == MK_LBUTTON)
	{
		CClientDC dc(pWnd);
		dc.SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0, 0);

		CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
		CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
		int     OldROP = dc.SetROP2(R2_NOTXORPEN);

		if(c_docLast != c_docDown)
		{
			CSize size = c_docLast - c_docDown;
			size.cx = abs(size.cx);
			size.cy = abs(size.cy);
			if(size.cx > size.cy)
				size.cy = size.cx;
			else
				size.cx = size.cy;

			CRect docRect = CRect(c_docDown.x, c_docDown.y, c_docDown.x, c_docDown.y);
			docRect.InflateRect(size);
			Rect rect = viewinfo.DocToClient(rect);
			dc.Ellipse(rect);
		}

		if(docPoint != c_docDown)
		{
			CSize size = docPoint - c_docDown;
			size.cx = abs(size.cx);
			size.cy = abs(size.cy);
			if(size.cx > size.cy)
				size.cy = size.cx;
			else
				size.cx = size.cy;

			CRect rect = CRect(c_docDown.x, c_docDown.y, c_docDown.x, c_docDown.y);
			rect.InflateRect(size);
			pWnd->SendMessage(WM_DOCTOCLIENT, (UINT)&rect, 3);
			dc.Ellipse(rect);
		}

		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, docPoint);
}

void CNodeTool::OnLButtonUp(CWnd* pWnd,CViewinfo& viewinfo,UINT nFlags, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd,viewinfo,nFlags, cliPoint, docPoint);

	CClientDC dc(pWnd);	
	dc.SetMapMode(MM_TEXT);
	dc.SetViewportOrg(0, 0);
	dc.SetWindowOrg(0,0);

	CPen*	OldPen   = (CPen* )dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	int     OldROP   =          dc.SetROP2(R2_NOTXORPEN);

	if(c_docLast!=c_docDown)
	{
		CSize size = c_docLast - c_docDown;
		size.cx = abs(size.cx);
		size.cy = abs(size.cy);
		if(size.cx > size.cy)
			size.cy = size.cx;
		else
			size.cx = size.cy;

		CRect rect = CRect(c_docDown.x,c_docDown.y,c_docDown.x,c_docDown.y);
		rect.InflateRect(size);
		
		CRect cliRect = rect;
		pWnd->SendMessage(WM_DOCTOCLIENT,(UINT)&cliRect,3);
		dc.Ellipse(cliRect);

		pWnd->SendMessage(WM_RGNQUERY,(LONG)&rect,2);
	}
	
	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
}

void CNodeTool::Draw(CWnd* pWnd,CViewinfo& viewinfo,CDC* pDC)
{
	if((nFlags & MK_LBUTTON) == MK_LBUTTON)
	{
		CClientDC dc(pWnd);	
		dc.SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0,0);

		CPen*	OldPen   = (CPen* )dc.SelectStockObject(BLACK_PEN);
		CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
		int     OldROP   =          dc.SetROP2(R2_NOTXORPEN);

		if(c_docLast != c_docDown)
		{
			CSize size = c_docLast - c_docDown;
			size.cx = abs(size.cx);
			size.cy = abs(size.cy);
			if(size.cx > size.cy)
				size.cy = size.cx;
			else
				size.cx = size.cy;

			CRect rect = CRect(c_docDown.x,c_docDown.y,c_docDown.x,c_docDown.y);
			rect.InflateRect(size);
			pWnd->SendMessage(WM_DOCTOCLIENT,(UINT)&rect,3);
			dc.Ellipse(rect);
		}
		
		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);
	}

	CTool::Draw(pWnd,viewinfo,pDC);
}

bool CNodeTool::SetCursor(CWnd* pWnd,CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_REMOVENODE);	
	if(hCursor != nullptr) 
	{
		::SetCursor(hCursor);
		return true;
	}		
	else
		return false;
}
