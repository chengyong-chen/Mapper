#include "stdafx.h"
#include "Tool.h"
#include "Global.h"

#include "EditGeom.h"

#include "../Mapper/Global.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Dataview/viewinfo.h"

#include "SelectTool.h"
#include "PolyTool.h"
#include "PanTool.h"

extern __declspec(dllimport) CSelectTool selectTool;
extern __declspec(dllexport) CPanTool panTool;
extern __declspec(dllexport) CPolyTool polyTool;

class CLink;

CEditGeom* CTool::m_pEditgeom = nullptr;
CPoint CTool::c_cliDown;
CPoint CTool::c_cliLast;
CPoint CTool::c_docDown;
CPoint CTool::c_docLast;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CTool::OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	pWnd->SetCapture();
	if(pWnd->IsKindOf(RUNTIME_CLASS(CView)) == TRUE)
	{
		CView* pView = (CView*)pWnd;
		COleClientItem* pActiveItem = ((COleDocument*)(pView->GetDocument()))->GetInPlaceActiveItem(pView);
		if(pActiveItem != nullptr)
		{
			pActiveItem->Close();
			ASSERT(((COleDocument*)(pView->GetDocument()))->GetInPlaceActiveItem(pView) == nullptr);
		}
	}
	c_cliDown = cliPoint;
	c_cliLast = cliPoint;
	c_docDown = docPoint;
	c_docLast = docPoint;
}

void CTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	c_cliLast = cliPoint;
	c_docLast = docPoint;
	/*
		CRect  rect;
		pWnd->GetClientRect(rect);
		CPoint local = docPoint;
		pWnd->SendMessage(WM_DOCTOCLIENT,(UINT)&local,1);
		if(m_bDown == TRUE && rect.PtInRect(local) == false)
		{
			if(local.x < rect.left-3)
				pWnd->SetTimer(TIMER_SCROLLLEFT, 50, nullptr);
			else if(local.y < rect.top-3)
				pWnd->SetTimer(TIMER_SCROLLTOP, 50, nullptr);
			else if(local.x > rect.right+3)
				pWnd->SetTimer(TIMER_SCROLLRIGHT, 50, nullptr);
			else if(local.y > rect.bottom+3)
				pWnd->SetTimer(TIMER_SCROLLBOTTOM, 50, nullptr);
			else
			{
				pWnd->KillTimer(TIMER_SCROLLLEFT);
				pWnd->KillTimer(TIMER_SCROLLTOP);
				pWnd->KillTimer(TIMER_SCROLLRIGHT);
				pWnd->KillTimer(TIMER_SCROLLBOTTOM);
			}
		}*/
}

void CTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	ReleaseCapture();
}

void CTool::OnLButtonDblClk(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint)
{
	ReleaseCapture();
}

bool CTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	const HCURSOR hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	::SetCursor(hCursor);
	return FALSE;
}

void CTool::SetEditGeom(CEditGeom* pEditgeom)
{
	m_pEditgeom = pEditgeom;
}

void CTool::OnKeyDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nChar, UINT nRepCnt, UINT nFlags, CTool*& pTool, CTool*& oldTool)
{
	switch(nChar & 0x00FF)
	{
		case VK_RIGHT:
			{
				CRect rect;
				pWnd->GetClientRect(rect);
				CPoint cliPoint1 = rect.CenterPoint();
				CPoint cliPoint2 = cliPoint1 + CSize(1, 0);
				CPoint docPoint1 = viewinfo.ClientToDoc(cliPoint1, false);
				CPoint docPoint2 = viewinfo.ClientToDoc(cliPoint2, false);
				CSize Δ(docPoint2.x - docPoint1.x, docPoint2.y - docPoint1.y);
				if(Δ.cx <= 0)
					Δ.cx = 1;

				if(m_pEditgeom != nullptr && m_pEditgeom->m_pGeom != nullptr && m_pEditgeom->m_Anchors.size() != 0)
				{
					CClientDC dc(pWnd);
					m_pEditgeom->MoveAnchor(pWnd, Δ, FALSE);
					m_pEditgeom->ChangeAnchor(pWnd, &dc, Δ, FALSE);
				}
				else if(pWnd->SendMessage(WM_MOVEGEOM, 0, (LONG)&Δ) == 1)
				{
				}
				else if(pWnd->IsKindOf(RUNTIME_CLASS(CView)) == TRUE)
					((CView*)pWnd)->OnScrollBy(CSize(300, 0), TRUE);
				else
					pWnd->SendMessage(WM_SCROLLVIEW, 300, 0);
			}
			break;
		case VK_LEFT:
			{
				CRect rect;
				pWnd->GetClientRect(rect);
				CPoint cliPoint1 = rect.CenterPoint();
				CPoint cliPoint2 = cliPoint1 + CSize(-1, 0);
				CPoint docPoint1 = viewinfo.ClientToDoc(cliPoint1, false);
				CPoint docPoint2 = viewinfo.ClientToDoc(cliPoint2, false);
				CSize Δ(docPoint2.x - docPoint1.x, docPoint2.y - docPoint1.y);
				if(Δ.cx >= 0)
					Δ.cx = -1;

				if(m_pEditgeom != nullptr && m_pEditgeom->m_pGeom != nullptr && m_pEditgeom->m_Anchors.size() != 0)
				{
					CClientDC dc(pWnd);
					m_pEditgeom->MoveAnchor(pWnd, Δ, FALSE);
					m_pEditgeom->ChangeAnchor(pWnd, &dc, Δ, FALSE);
				}
				else if(pWnd->SendMessage(WM_MOVEGEOM, 0, (LONG)&Δ) == 1)
				{
				}
				else if(pWnd->IsKindOf(RUNTIME_CLASS(CView)) == TRUE)
					((CView*)pWnd)->OnScrollBy(CSize(-300, 0), TRUE);
				else
					pWnd->SendMessage(WM_SCROLLVIEW, -300, 0);
			}
			break;
		case VK_UP:
			{
				CRect rect;
				pWnd->GetClientRect(rect);
				CPoint cliPoint1 = rect.CenterPoint();
				CPoint cliPoint2 = cliPoint1 + CSize(0, -1);
				CPoint docPoint1 = viewinfo.ClientToDoc(cliPoint1, false);
				CPoint docPoint2 = viewinfo.ClientToDoc(cliPoint2, false);
				CSize Δ(docPoint2.x - docPoint1.x, docPoint2.y - docPoint1.y);
				if(Δ.cy <= 0)
					Δ.cy = 1;

				if(m_pEditgeom != nullptr && m_pEditgeom->m_pGeom != nullptr && m_pEditgeom->m_Anchors.size() != 0)
				{
					CClientDC dc(pWnd);
					m_pEditgeom->MoveAnchor(pWnd, Δ, FALSE);
					m_pEditgeom->ChangeAnchor(pWnd, &dc, Δ, FALSE);
				}
				else if(pWnd->SendMessage(WM_MOVEGEOM, 0, (LONG)&Δ) == 1)
				{
				}
				else if(pWnd->IsKindOf(RUNTIME_CLASS(CView)) == TRUE)
					((CView*)pWnd)->OnScrollBy(CSize(0, -300), TRUE);
				else
					pWnd->SendMessage(WM_SCROLLVIEW, 0, -300);
			}
			break;
		case VK_DOWN:
			{
				CRect rect;
				pWnd->GetClientRect(rect);
				CPoint cliPoint1 = rect.CenterPoint();
				CPoint cliPoint2 = cliPoint1 + CSize(0, 1);
				CPoint docPoint1 = viewinfo.ClientToDoc(cliPoint1, false);
				CPoint docPoint2 = viewinfo.ClientToDoc(cliPoint2, false);
				CSize Δ(docPoint2.x - docPoint1.x, docPoint2.y - docPoint1.y);
				if(Δ.cy >= 0)
					Δ.cy = -1;

				if(m_pEditgeom != nullptr && m_pEditgeom->m_pGeom != nullptr && m_pEditgeom->m_Anchors.size() != 0)
				{
					CClientDC dc(pWnd);
					m_pEditgeom->MoveAnchor(pWnd, Δ, FALSE);
					m_pEditgeom->ChangeAnchor(pWnd, &dc, Δ, FALSE);
				}
				else if(pWnd->SendMessage(WM_MOVEGEOM, 0, (LONG)&Δ) == 1)
				{
				}
				else if(pWnd->IsKindOf(RUNTIME_CLASS(CView)) == TRUE)
					((CView*)pWnd)->OnScrollBy(CSize(0, 300), TRUE);
				else
					pWnd->SendMessage(WM_SCROLLVIEW, 0, 300);
			}
			break;
		case VK_SPACE:
			{
				if(pTool == &panTool)
					break;

				if(GetKeyState(VK_LBUTTON) < 0)
					break;

				if((nFlags & 0X4000) == 0X0000)
				{
					oldTool = pTool;
					pTool = &panTool;
					pTool->SetCursor(pWnd, viewinfo);
				}
				else if(pTool != &panTool)
				{
					oldTool = pTool;
					pTool = &panTool;
					pTool->SetCursor(pWnd, viewinfo);
				}
			}
			break;
		case VK_CONTROL:
			{
				if(pTool == &selectTool)
					break;
				if(GetKeyState(VK_LBUTTON) < 0)
					break;
				else if((nFlags & 0X4000) == 0X0000)
				{
					oldTool = pTool;
					pTool = &selectTool;
					pTool->SetCursor(pWnd, viewinfo);
				}
				else if(pTool != &selectTool)
				{
					oldTool = pTool;
					pTool = &selectTool;
					pTool->SetCursor(pWnd, viewinfo);
				}
			}
			break;
		case VK_SHIFT:
			{
				pTool->OnShiftDown(pWnd, viewinfo, nRepCnt, nFlags);
			}
			break;
	}
}

void CTool::OnKeyUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nChar, UINT nRepCnt, UINT nFlags, CTool*& pTool, CTool*& oldTool)
{
	switch(nChar)
	{
		case VK_SPACE:
			{
				if(GetKeyState(VK_LBUTTON) < 0)
					break;
				if(oldTool == nullptr)
					break;

				ReleaseCapture();

				pTool = oldTool;
				oldTool = nullptr;

				pTool->SetCursor(pWnd, viewinfo);
			}
			break;
		case VK_CONTROL:
			{
				if(GetKeyState(VK_LBUTTON) < 0)
					break;

				if(oldTool == nullptr)
					break;

				pTool = oldTool;
				oldTool = nullptr;
				pTool->SetCursor(pWnd, viewinfo);
			}
			break;
		case VK_SHIFT:
			{
				pTool->OnShiftUp(pWnd, viewinfo, nRepCnt, nFlags);
			}
			break;
	}
}

void CTool::Draw(CWnd* pWnd, const CViewinfo& viewinfo, CDC* pDC)
{
	if(m_pEditgeom == nullptr)
		return;

	switch(m_pEditgeom->m_Mode)
	{
		case CEditGeom::Mode::FirstPolying:
			{
				CPoly::DrawSquareAnchor(pDC, viewinfo, polyTool.m_PrevPoint);
			}
			break;
		case CEditGeom::Mode::SecondPolying:
			{
			}
			break;
		case CEditGeom::Mode::FirstBeziering:
			break;
		case CEditGeom::Mode::SecondBeziering:
			break;
	}
}

bool CTool::CanPan()
{
	if((GetAsyncKeyState(VK_LBUTTON) & 0X8000) != 0X8000)// VK_LBUTTON is not down
		return false;
	else if(m_pEditgeom == nullptr)
		return true;
	else if(m_pEditgeom->m_pGeom != nullptr)
		return false;
	else
		return true;
}
