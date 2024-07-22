#include "stdafx.h"
#include "Global.h"

#include "Tool.h"
#include "ReflectTool.h"
#include "ReflectDlg.h"

#include "../Geometry/Global.h"

__declspec(dllexport) CReflectTool reflectTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CReflectTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if((nFlags&MK_LBUTTON)==MK_LBUTTON)
	{
		if(c_docLast!=c_docDown)
		{
			const double xdelta = c_docLast.x-c_docDown.x;
			const double ydelta = c_docLast.y-c_docDown.y;
			double R = sqrt(pow(xdelta, 2)+pow(ydelta, 2));
			if(R<10) R = 0;
			const double sinQ = R==0 ? 1 : ydelta/R;
			const double cosQ = R==0 ? 0 : xdelta/R;
			const double a11 = pow(cosQ, 2)-pow(sinQ, 2);
			const double a21 = 2*sinQ*cosQ;
			const double a31 = -c_docDown.x*a11-c_docDown.y*a21+c_docDown.x;
			const double a12 = a21;
			const double a22 = -a11;
			const double a32 = -c_docDown.x*a12-c_docDown.y*a22+c_docDown.y;

			double a[6];
			a[0] = a11, a[1] = a21, a[2] = a31, a[3] = a12, a[4] = a22, a[5] = a32;
			pWnd->SendMessage(WM_TRANSFORM, (LONG)a, 0);
		}
		if(docPoint!=c_docDown)
		{
			const double xdelta = docPoint.x-c_docDown.x;
			const double ydelta = docPoint.y-c_docDown.y;
			double R = sqrt(pow(xdelta, 2)+pow(ydelta, 2));
			if(R<10) R = 0;
			const double sinQ = R==0 ? 1 : ydelta/R;
			const double cosQ = R==0 ? 0 : xdelta/R;
			const double b11 = pow(cosQ, 2)-pow(sinQ, 2);
			const double b21 = 2*sinQ*cosQ;
			const double b31 = -c_docDown.x*b11-c_docDown.y*b21+c_docDown.x;
			const double b12 = b21;
			const double b22 = -b11;
			const double b32 = -c_docDown.x*b12-c_docDown.y*b22+c_docDown.y;

			double b[6];
			b[0] = b11, b[1] = b21, b[2] = b31, b[3] = b12, b[4] = b22, b[5] = b32;

			pWnd->SendMessage(WM_TRANSFORM, (LONG)b, 0);
		}
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CReflectTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
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
		const double sinQ = R==0 ? 1 : ydelta/R;
		const double cosQ = R==0 ? 0 : xdelta/R;
		const double m11 = pow(cosQ, 2)-pow(sinQ, 2);
		const double m21 = 2*sinQ*cosQ;
		const double m31 = -c_docDown.x*m11-c_docDown.y*m21+c_docDown.x;
		const double m12 = m21;
		const double m22 = -m11;
		const double m32 = -c_docDown.x*m12-c_docDown.y*m22+c_docDown.y;

		double a[6];
		a[0] = m11, a[1] = m21, a[2] = m31, a[3] = m12, a[4] = m22, a[5] = m32;
		pWnd->SendMessage(WM_TRANSFORM, (LONG)a, 0);
		pWnd->SendMessage(WM_TRANSFORMED, (LONG)a, 0);
	}
	else if(GetKeyState(VK_MENU)<0)
	{
		CReflectDlg dlg;
		if(dlg.DoModal()==IDOK)
		{
			CPoint center = docPoint;
			if(!dlg.p_bMouse)
			{
				CRect* rect;
				rect = (CRect*)(pWnd->SendMessage(WM_GETACTIVERECT, 0, 0));
				center = rect->CenterPoint();
			}
			const double sinQ = sin(dlg.p_fAngle);
			const double cosQ = cos(dlg.p_fAngle);
			const double m11 = pow(cosQ, 2)-pow(sinQ, 2);
			const double m21 = 2*sinQ*cosQ;
			const double m31 = -center.x*m11-center.y*m21+center.x;
			const double m12 = m21;
			const double m22 = -m11;
			const double m32 = -center.x*m12-center.y*m22+center.y;

			double a[6];
			a[0] = m11, a[1] = m21, a[2] = m31, a[3] = m12, a[4] = m22, a[5] = m32;
			pWnd->SendMessage(WM_TRANSFORMED, (LONG)a, 0);
		}
	}
}

bool CReflectTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_STAR);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
	{
		return false;
	}
}
