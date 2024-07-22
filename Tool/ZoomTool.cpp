#include "stdafx.h"
#include "Global.h"
#include "Resource.h"

#include "../Dataview/Datainfo.h"
#include "../Dataview/Viewinfo.h"

#include "Tool.h"
#include "ZoomTool.h"

class CLink;

__declspec(dllexport) CZoomTool zoominTool;
__declspec(dllexport) CZoomTool zoomoutTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CZoomTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
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

		CRect docRect1(c_docDown.x, c_docDown.y, c_docLast.x, c_docLast.y);
		docRect1.NormalizeRect();
		const Gdiplus::Rect cliRect1 = viewinfo.DocToClient <Gdiplus::Rect>(docRect1);
		dc.Rectangle(cliRect1.GetLeft(), cliRect1.GetTop(), cliRect1.GetRight(), cliRect1.GetBottom());

		CRect docRect2(c_docDown.x, c_docDown.y, docPoint.x, docPoint.y);
		docRect2.NormalizeRect();
		const Gdiplus::Rect cliRect2 = viewinfo.DocToClient <Gdiplus::Rect>(docRect2);
		dc.Rectangle(cliRect2.GetLeft(), cliRect2.GetTop(), cliRect2.GetRight(), cliRect2.GetBottom());
		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CZoomTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);


	Gdiplus::Rect cliRect(min(c_cliDown.x, c_cliLast.x), min(c_cliDown.y, c_cliLast.y), abs(c_cliDown.x-c_cliLast.x), abs(c_cliDown.y-c_cliLast.y));
	CRect docRect(c_docDown.x, c_docDown.y, c_docLast.x, c_docLast.y);
	docRect.NormalizeRect();

	if(cliRect.IsEmptyArea()==false)
	{
		CClientDC dc(pWnd);
		dc.SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0, 0);

		CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
		CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
		const int OldROP = dc.SetROP2(R2_NOTXORPEN);

		dc.Rectangle(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());

		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);
	}
	bool zoomingOut = GetKeyState(VK_MENU)<0||m_bOut==true;
	bool invertible = viewinfo.m_pGeocentric==nullptr||viewinfo.m_pGeocentric->IsInvertible() == true;
	
	pWnd->SendMessage(zoomingOut ? WM_ZOOMOUT : WM_ZOOMIN, invertible ? (LONG)(&docRect): (LONG)(&cliRect), invertible);
}

bool CZoomTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if((GetAsyncKeyState(VK_LBUTTON)&0X8000)!=0X8000)// VK_LBUTTON is not down
	{
		HCURSOR hCursor = nullptr;
		if(GetKeyState(VK_MENU)<0||m_bOut==true)
		{
			if(viewinfo.m_scaleCurrent>=viewinfo.m_datainfo.m_scaleMinimum)
				hCursor = AfxGetApp()->LoadCursor(IDC_ZOOMZERO);
			else
				hCursor = AfxGetApp()->LoadCursor(IDC_ZOOMOUT);
		}
		else
		{
			if(viewinfo.m_scaleCurrent<=viewinfo.m_datainfo.m_scaleMaximum)
				hCursor = AfxGetApp()->LoadCursor(IDC_ZOOMZERO);
			else
				hCursor = AfxGetApp()->LoadCursor(IDC_ZOOMIN);
		}

		if(hCursor!=nullptr)
		{
			::SetCursor(hCursor);
			return true;
		}
	}

	return true;
}
