#include "stdafx.h"
#include "Global.h"
#include <math.h>

#include "Tool.h"
#include "RotateTool.h"
#include "RotateDlg.h"

#include "../Geometry/Global.h"

__declspec(dllexport) CRotateTool rotateTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CRotateTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if((nFlags&MK_LBUTTON)==MK_LBUTTON)
	{
		double m11, m21, m31, m12, m22, m32;
		double b11, b21, b31, b12, b22, b32;
		double xdelta, ydelta;
		double R;

		xdelta = c_docLast.x-c_docDown.x;
		ydelta = c_docLast.y-c_docDown.y;
		R = sqrt(pow(xdelta, 2)+pow(ydelta, 2));
		if(R<10) R = 0;

		m11 = R==0 ? 1 : xdelta/R;
		m21 = R==0 ? 0 : -ydelta/R;
		m31 = (1-m11)*c_docDown.x-m21*c_docDown.y;

		m12 = -1*m21;
		m22 = m11;
		m32 = (1-m22)*c_docDown.y-m12*c_docDown.x;

		xdelta = docPoint.x-c_docDown.x;
		ydelta = docPoint.y-c_docDown.y;
		R = sqrt(pow(xdelta, 2)+pow(ydelta, 2));
		if(R<10) R = 0;

		b11 = R==0 ? 1 : xdelta/R;
		b21 = R==0 ? 0 : -ydelta/R;
		b31 = (1-b11)*c_docDown.x-b21*c_docDown.y;

		b12 = -1*b21;
		b22 = b11;
		b32 = (1-b22)*c_docDown.y-b12*c_docDown.x;

		double a[6];
		double b[6];
		a[0] = m11, a[1] = m21, a[2] = m31, a[3] = m12, a[4] = m22, a[5] = m32;
		b[0] = b11, b[1] = b21, b[2] = b31, b[3] = b12, b[4] = b22, b[5] = b32;
		pWnd->SendMessage(WM_TRANSFORM, (LONG)a, 0);
		pWnd->SendMessage(WM_TRANSFORM, (LONG)b, 0);
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CRotateTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	if(docPoint!=c_docDown)
	{
		const double xdelta = docPoint.x-c_docDown.x;
		const double ydelta = docPoint.y-c_docDown.y;
		double R = sqrt(pow(xdelta, 2)+pow(ydelta, 2));
		if(R<10)
			R = 0;

		const double m11 = R==0 ? 1 : xdelta/R;
		const double m21 = R==0 ? 0 : -ydelta/R;
		const double m31 = (1-m11)*c_docDown.x-m21*c_docDown.y;

		const double m12 = -1*m21;
		const double m22 = m11;
		const double m32 = (1-m22)*c_docDown.y-m12*c_docDown.x;

		double a[6];
		a[0] = m11, a[1] = m21, a[2] = m31, a[3] = m12, a[4] = m22, a[5] = m32;
		pWnd->SendMessage(WM_TRANSFORM, (LONG)a, 0);
		pWnd->SendMessage(WM_TRANSFORMED, (LONG)a, 0);
	}
	else if(GetKeyState(VK_MENU)<0)
	{
		CRotateDlg dlg;
		if(dlg.DoModal()==IDOK)
		{
			CPoint center = docPoint;
			if(dlg.p_bMouse==FALSE)
			{
				CRect* rect;
				rect = (CRect*)(pWnd->SendMessage(WM_GETACTIVERECT, 0, 0));
				center = rect->CenterPoint();
			}
			const double angle = -dlg.p_fAngle/180*M_PI;
			const double m11 = cos(angle);
			const double m21 = -sin(angle);
			const double m31 = (1-m11)*center.x-m21*center.y;
			const double m12 = -1*m21;
			const double m22 = m11;
			const double m32 = (1-m22)*center.y-m12*center.x;

			double a[6];
			a[0] = m11, a[1] = m21, a[2] = m31, a[3] = m12, a[4] = m22, a[5] = m32;
			pWnd->SendMessage(WM_TRANSFORMED, (LONG)a, 0);
		}
	}
}

bool CRotateTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
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
