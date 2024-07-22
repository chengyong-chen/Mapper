#include "stdafx.h"
#include "resource.h"

#include "Tool.h"
#include "RulerTool.h"
#include "SelectTool.h"

#include "../DataView/ViewMonitor.h"

#include "../Mapper/Global.h"

__declspec(dllexport) CRulerTool rulerTool;

RulerMode CRulerTool::rulMode = rulNone;

extern __declspec(dllexport) CSelectTool selectTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CRulerTool::OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonDown(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	if(cliPoint.x>12&&cliPoint.y>12)
		rulMode = rulNone;

	if(cliPoint.x<12&&cliPoint.y<12)
	{
		rulMode = rulOrigin;
	}

	CPoint local = cliPoint;
	local.Offset(-12, -12);
	if(cliPoint.x>12&&cliPoint.y<12)
	{
	}

	if(cliPoint.x<12&&cliPoint.y > 12)
	{
	}

	c_cliDown = local;
	c_cliLast = local;
}

void CRulerTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CClientDC dc(pWnd);

	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);

	switch(rulMode)
	{
		case rulOrigin:
			{
				CRect client;
				pWnd->GetClientRect(client);

				dc.MoveTo(client.left, c_docLast.y);
				dc.LineTo(client.right, c_docLast.y);
				dc.MoveTo(c_docLast.x, client.top);
				dc.LineTo(c_docLast.x, client.bottom);

				dc.MoveTo(client.left, docPoint.y);
				dc.LineTo(client.right, docPoint.y);
				dc.MoveTo(docPoint.x, client.top);
				dc.LineTo(docPoint.x, client.bottom);
			}
			break;
		case rulNone:
			break;
		default:
			break;
	}

	dc.SelectObject(OldPen);
	dc.SetROP2(OldROP);

	c_cliLast = docPoint;

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CRulerTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	CSize paper = *(CSize*)pWnd->SendMessage(WM_GETPAPERSIZE, 0, 0);
	paper.cx *= 10000;
	paper.cy *= 10000;

	CDC* pDC = pWnd->GetDC();
	const CRect pRect = CRect(0, 0, paper.cx, paper.cy);

	CPen* OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	const int OldROP = pDC->SetROP2(R2_NOTXORPEN);

	switch(rulMode)
	{
		case rulOrigin:
			{
				CRect client;
				pWnd->GetClientRect(client);

				pDC->MoveTo(client.left, cliPoint.y);
				pDC->LineTo(client.right, cliPoint.y);
				pDC->MoveTo(cliPoint.x, client.top);
				pDC->LineTo(cliPoint.x, client.bottom);

				if(pRect.PtInRect(docPoint)==false||(cliPoint.x<0&&cliPoint.y<0))
				{
					break;
				}
				else
				{
					const CMDIChildWnd* pChild = (CMDIChildWnd*)pWnd->GetParent();
					pChild->SendMessage(WM_SETRULER, (UINT)(&docPoint), 0);
				}
			}
			break;
		default:
			break;
	}

	pDC->SelectObject(OldPen);
	pDC->SetROP2(OldROP);

	rulMode = rulNone;
	ReleaseCapture();

	selectTool.m_actMode = CSelectTool::ActionMode::Wandering;

	pWnd->ReleaseDC(pDC);
}

bool CRulerTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HCURSOR hCursor = nullptr;
	switch(rulMode)
	{
		case rulOrigin:
			hCursor = AfxGetApp()->LoadCursor(IDC_MYCROSS);
			break;
	}

	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
