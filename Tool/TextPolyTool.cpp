#include "stdafx.h"
#include "Global.h"
#include "EditGeom.h"

#include "../Geometry/Global.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/TextPoly.h"
#include "../Geometry/TextEditDlg.h"

#include "Resource.h"
#include "Tool.h"
#include "PolyTool.h"
#include "TextPolyTool.h"

CTextPolyTool textpolyTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CTextPolyTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_pEditgeom->m_Mode==CEditGeom::Mode::SecondPolying)
	{
		CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

		CPoly::DrawBlackAnchor(pWnd, viewinfo, m_PrevPoint);
		const CPoint point = ::GetKeyState(VK_SHIFT)<0 ? RegulizePoint(m_PrevPoint, docPoint, 8) : docPoint;
		CPoly::DrawSquareAnchor(pWnd, viewinfo, point);

		CTextEditDlg dlg(pWnd);
		if(dlg.DoModal()!=IDOK||dlg.m_String.IsEmpty())
		{
			m_pEditgeom->UnFocusAll(pWnd);
		}
		else
		{
			CTextPoly* pPoly = new CTextPoly(CRect(point, CSize(0, 0)));
			pPoly->m_strName = dlg.m_String;
			pPoly->AddAnchor(m_PrevPoint);
			pPoly->AddAnchor(point);
			if(pWnd->SendMessage(WM_NEWGEOMDREW, (DWORD)pPoly, 0)==TRUE)
			{
				m_pEditgeom->NewGeom(pWnd, pPoly);
				m_pEditgeom->FocusAnchor(pWnd, 2, false);
			}
		}

		m_pEditgeom->m_Mode = CEditGeom::Mode::Nothing;
	}
	else
		return CPolyTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

bool CTextPolyTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_TEXT);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);;
		return true;
	}
	else
		return false;
}
