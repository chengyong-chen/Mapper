#include "stdafx.h"
#include "Resource.h"

#include "Tool.h"
#include "DispatchTool.h"

__declspec(dllexport) CDispatchTool dispatchTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CDispatchTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	/*if((nFlags & MK_LBUTTON) == MK_LBUTTON)
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
			Rect cliRect = viewinfo.DocToClient(rect)

			dc.Ellipse(cliRect.GetLeft(),cliRect.GetTop(),cliRect.GetRight(),cliRect.GetBottom());

			CPoint center = cliRect.ce.CenterPoint();
			dc.MoveTo(rect.left,center.y);
			dc.LineTo(rect.right,center.y);
			dc.MoveTo(center.x,rect.top);
			dc.LineTo(center.x,rect.bottom);
		}

		if(docPoint!=c_docDown)
		{
			CSize size = docPoint - c_docDown;
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
			CPoint center = rect.CenterPoint();

			dc.MoveTo(rect.left,rect.CenterPoint().y);
			dc.LineTo(rect.right,rect.CenterPoint().y);

			dc.MoveTo(rect.CenterPoint().x,rect.top);
			dc.LineTo(rect.CenterPoint().x,rect.bottom);

			CArray<CPointF, CPointF&> points;
			CPointF mapPoint1;
			CPointF mapPoint2;
			pWnd->SendMessage(WM_DOCTOMAP,(UINT)&c_docDown,(long)&mapPoint1);
			pWnd->SendMessage(WM_DOCTOMAP,(UINT)&docPoint,(long)&mapPoint2);
			points.Add(mapPoint1);
			points.Add(mapPoint2);
			double length = *(double*)(pWnd->SendMessage(WM_CALLENGTH,(UINT)&points,0));

			CString strRadius;
			if(length>1)
			{
				strRadius.Format(_T("%.1f km"), length);
			}
			else
			{
				length = length*1000;
				strRadius.Format(_T("%.0f m"), length);
			}

			UINT oldAlign = dc.SetTextAlign(TA_BOTTOM | TA_CENTER);
			dc.TextOut(center.x,center.y,strRadius);
			dc.SetTextAlign(oldAlign);
		}

		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);
	}*/

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CDispatchTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	/*CClientDC dc(pWnd);
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

		CPoint center = rect.CenterPoint();
		dc.MoveTo(rect.left,center.y);
		dc.LineTo(rect.right,center.y);
		dc.MoveTo(center.x,rect.top);
		dc.LineTo(center.x,rect.bottom);

		CPoint docPoint1 = c_docDown;
		CPoint docPoint2 = c_docDown;
		docPoint2.Offset(size);
		CPointF mapPoint1;
		CPointF mapPoint2;
		static CPointF geoPoint1;
		static CPointF geoPoint2;

		pWnd->SendMessage(WM_DOCTOMAP,(UINT)&docPoint1,(LONG)&mapPoint1);
		pWnd->SendMessage(WM_DOCTOMAP,(UINT)&docPoint2,(LONG)&mapPoint2);
		pWnd->SendMessage(WM_MAPTOGEO,(UINT)&mapPoint1,(LONG)&geoPoint1);
		pWnd->SendMessage(WM_MAPTOGEO,(UINT)&mapPoint2,(LONG)&geoPoint2);
		static CSizeF geoSize;
		geoSize	= CSizeF(std::abs(geoPoint2.x-geoPoint1.x),std::abs(geoPoint2.y-geoPoint1.y));

		CFrameWnd* pAppFrame = (CFrameWnd*) AfxGetApp()->m_pMainWnd;
		pAppFrame->PostMessage(WM_VEHICLEDISPATCH,(LONG)&geoPoint1,(LONG)&geoSize);

		rect.DeflateRect(rect.Width()/2,rect.Height()/2);
		rect.InflateRect(4*12,12);
		rect.OffsetRect(0,-6);
		pWnd->InvalidateRect(rect);
	}

	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);*/
}

void CDispatchTool::Draw(CWnd* pWnd, const CViewinfo& viewinfo, CDC* pDC)
{
	/*if((nFlags & MK_LBUTTON) == MK_LBUTTON)
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

			CPoint center = rect.CenterPoint();
			dc.MoveTo(rect.left,center.y);
			dc.LineTo(rect.right,center.y);
			dc.MoveTo(center.x,rect.top);
			dc.LineTo(center.x,rect.bottom);
		}

		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);
	}*/

	CTool::Draw(pWnd, viewinfo, pDC);
}

bool CDispatchTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_ROUND);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);;
		return true;
	}
	else
		return false;
}
