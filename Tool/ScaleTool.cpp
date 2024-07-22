#include "stdafx.h"
#include "Global.h"

#include "Tool.h"
#include "ScaleTool.h"
#include "ScaleDlg.h"

#include "../Dataview/viewinfo.h"
#include "../Geometry/Global.h"

__declspec(dllexport) CScaleTool scaleTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CScaleTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if((nFlags&MK_LBUTTON)==MK_LBUTTON)
	{
		const Gdiplus::Point cliDown = viewinfo.DocToClient<Gdiplus::Point>(c_docDown);
		const Gdiplus::Point cliLast = viewinfo.DocToClient<Gdiplus::Point>(c_docLast);
		const Gdiplus::Point cliMove = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
		const double xdelta1 = (cliLast.X-cliDown.X)/50.f;
		const double ydelta1 = (cliLast.Y-cliDown.Y)/50.f;
		const double m11 = 1+xdelta1;
		const double m21 = 0;
		const double m31 = -c_docDown.x*xdelta1;
		const double m12 = 0;
		const double m22 = 1+ydelta1;
		const double m32 = -c_docDown.y*ydelta1;
		const double xdelta2 = (cliMove.X-cliDown.X)/50.f;
		const double ydelta2 = (cliMove.Y-cliDown.Y)/50.f;
		const double b11 = 1+xdelta2;
		const double b21 = 0;
		const double b31 = -c_docDown.x*xdelta2;
		const double b12 = 0;
		const double b22 = 1+ydelta2;
		const double b32 = -c_docDown.y*ydelta2;

		double a[6];
		double b[6];
		a[0] = m11, a[1] = m21, a[2] = m31, a[3] = m12, a[4] = m22, a[5] = m32;
		b[0] = b11, b[1] = b21, b[2] = b31, b[3] = b12, b[4] = b22, b[5] = b32;
		pWnd->SendMessage(WM_TRANSFORM, (LONG)a, 0);
		pWnd->SendMessage(WM_TRANSFORM, (LONG)b, 0);
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CScaleTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	if(docPoint!=c_docDown)
	{
		const Gdiplus::Point cliDown = viewinfo.DocToClient<Gdiplus::Point>(c_docDown);
		const Gdiplus::Point cliMove = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
		const double xdelta = (cliMove.X-cliDown.X)/50.f;
		const double ydelta = (cliMove.Y-cliDown.Y)/50.f;

		const double m11 = 1+xdelta;
		const double m21 = 0;
		const double m31 = -c_docDown.x*xdelta;

		const double m12 = 0;
		const double m22 = 1+ydelta;
		const double m32 = -c_docDown.y*ydelta;

		double a[6];
		a[0] = m11, a[1] = m21, a[2] = m31, a[3] = m12, a[4] = m22, a[5] = m32;
		pWnd->SendMessage(WM_TRANSFORM, (LONG)a, 0);
		pWnd->SendMessage(WM_TRANSFORMED, (LONG)a, 0);
	}
	else if(GetKeyState(VK_MENU)<0)
	{
		CScaleDlg dlg;
		if(dlg.DoModal()==IDOK&&(dlg.p_fHscale!=100||dlg.p_fVscale!=100))
		{
			CPoint center = docPoint;
			if(!dlg.p_bMouse)
			{
				CRect* rect;
				rect = (CRect*)(pWnd->SendMessage(WM_GETACTIVERECT, 0, 0));
				center = rect->CenterPoint();
			}
			const double m11 = dlg.p_fHscale/100;
			const double m21 = 0;
			const double m31 = (1-dlg.p_fHscale/100)*center.x;
			const double m12 = 0;
			const double m22 = dlg.p_fVscale/100;
			const double m32 = (1-dlg.p_fVscale/100)*center.y;

			double a[6];
			a[0] = m11, a[1] = m21, a[2] = m31, a[3] = m12, a[4] = m22, a[5] = m32;
			pWnd->SendMessage(WM_TRANSFORMED, (LONG)a, 0);
		}
	}
}

bool CScaleTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_STAR);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
