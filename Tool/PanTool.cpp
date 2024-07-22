#include "stdafx.h"
#include "Resource.h"

#include "Tool.h"
#include "PanTool.h"

#include "../DataView/ViewMonitor.h"

#include "../Mapper/Global.h"

__declspec(dllexport) CPanTool panTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPanTool::CPanTool()
{
	m_bContinuously = false;
}

void CPanTool::OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonDown(pWnd, viewinfo, nFlags, cliPoint, docPoint);
	const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
	c_docLast = CPoint(point1.X, point1.Y);
	c_docDown = CPoint(point1.X, point1.Y);

	if(m_bContinuously==false)
	{
		CClientDC dc(pWnd);

		CRect rect;
		pWnd->GetClientRect(rect);

		if(m_bitmap.m_hObject != nullptr)
		{
			m_bitmap.DeleteObject();
		}
		if(m_bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height()) == TRUE)
		{
			CDC mdc;
			mdc.CreateCompatibleDC(&dc);
			CBitmap* pOldBitmap = mdc.SelectObject(&m_bitmap);
			mdc.BitBlt(0, 0, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);
			mdc.SelectObject(pOldBitmap);
			mdc.DeleteDC();
		}
	}
}

void CPanTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if((nFlags & MK_LBUTTON) == MK_LBUTTON)
	{
		const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
		const Gdiplus::Point point2 = Gdiplus::Point(c_docLast.x, c_docLast.y);
		const CSize ms(point2.X-point1.X, point2.Y-point1.Y);
		if(ms!=CSize(0, 0))
		{
			if(m_bContinuously==true)
			{
				if(pWnd->IsKindOf(RUNTIME_CLASS(CView))==TRUE)
					((CView*)pWnd)->OnScrollBy(ms, TRUE);
				else
					pWnd->SendMessage(WM_SCROLLVIEW, ms.cx, ms.cy);
			}
			else if(m_bitmap.m_hObject != nullptr)
			{
				CRect rect;
				pWnd->GetClientRect(rect);

				CClientDC dc(pWnd);

				CDC mdc1;
				mdc1.CreateCompatibleDC(&dc);
				CBitmap bitmap;
				if(bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height()) == TRUE)
				{
					CBitmap* pOldBitmap1 = mdc1.SelectObject(&bitmap);
					mdc1.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(255, 255, 254));

					CDC mdc2;
					mdc2.CreateCompatibleDC(&dc);
					CBitmap* pOldBitmap2 = mdc2.SelectObject(&m_bitmap);
					mdc1.BitBlt(point1.X-c_docDown.x, point1.Y-c_docDown.y, rect.Width(), rect.Height(), &mdc2, 0, 0, SRCCOPY);
					mdc2.SelectObject(pOldBitmap2);
					mdc2.DeleteDC();

					dc.BitBlt(0, 0, rect.Width(), rect.Height(), &mdc1, 0, 0, SRCCOPY);

					mdc1.SelectObject(pOldBitmap1);
					bitmap.DeleteObject();
				}
				mdc1.DeleteDC();
			}
		}
		CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
	}
}

void CPanTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
	const Gdiplus::Point point2 = Gdiplus::Point(c_docDown.x, c_docDown.y);
	const CSize ms(point2.X - point1.X, point2.Y - point1.Y);

	if(ms == CSize(0, 0))
	{
		CRect rect;
		pWnd->GetClientRect(rect);
		CPoint cliPoint = rect.CenterPoint();

		pWnd->SendMessage(WM_FIXATEVIEW, (UINT)&docPoint, (UINT)&cliPoint);
	}
	else if(m_bContinuously==false)
	{
		if(m_bitmap.m_hObject != nullptr)
		{
			m_bitmap.DeleteObject();
		}

		if(pWnd->IsKindOf(RUNTIME_CLASS(CView)) == TRUE)
		{
			((CView*)pWnd)->OnScrollBy(ms, TRUE);
		}
		else
		{
			pWnd->SendMessage(WM_SCROLLVIEW, ms.cx, ms.cy);
		}
	}
}

bool CPanTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_PAN);
	if(hCursor != nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}

void CPanTool::Recover(CWnd* pWnd, const CViewinfo& viewinfo, CTool*& pTool, CTool*& oldTool)
{
	if(oldTool != nullptr)
	{
		pTool = oldTool;
		oldTool = nullptr;
	}

	if(pTool != nullptr)
		pTool->SetCursor(pWnd, viewinfo);
	else
	{
		const HCURSOR hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		::SetCursor(hCursor);
	}
}
