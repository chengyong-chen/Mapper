#include "stdafx.h"

#include "Resource.h"

#include "Tool.h"
#include "DatumTool.h"
#include "../Rectify/Datum.h"
#include "../Rectify/Tin.h"
#include "../Dataview/viewinfo.h"

//__declspec(dllexport) CDatumTool datumTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//CDatumTool::CDatumTool()
//{
//}

CDatumTool::CDatumTool(CTin& tin)
	: m_tin(tin)
{
}

void CDatumTool::OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonDown(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	CDatum* pDatum = m_tin.PickDatum(viewinfo, docPoint);
	if(pDatum!=nullptr)
		m_pDatum = pDatum;
	else
	{
		CDatum* pDatum = new CDatum(docPoint);
		m_tin.AddDatum(pDatum);
		pDatum->Draw(pWnd, viewinfo);
		m_pDatum = pDatum;
	}
}

void CDatumTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if((nFlags&MK_LBUTTON)==MK_LBUTTON)// VK_LBUTTON is down
	{
		if(m_pDatum!=nullptr)
		{
			const CSize size1 = c_docLast-c_docDown;
			const CSize size2 = docPoint-c_docDown;
			m_pDatum->Moving(pWnd, viewinfo, size1);
			m_pDatum->Moving(pWnd, viewinfo, size2);
		}
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CDatumTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	if(m_pDatum!=nullptr)
	{
		const CSize size = docPoint-c_docDown;
		m_pDatum->Moving(pWnd, viewinfo, size);

		m_pDatum->Invalidate(pWnd, viewinfo);
		m_pDatum->Move(size);
		m_pDatum->Draw(pWnd, viewinfo);
		m_pDatum = nullptr;
	}
}

void CDatumTool::OnRButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint)
{
	CTool::OnRButtonDown(pWnd, viewinfo, nFlags, docPoint);
}

void CDatumTool::OnRButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CTool::OnRButtonUp(pWnd, viewinfo, nFlags, docPoint);

	CDatum* pDatum = m_tin.PickDatum(viewinfo, docPoint);
	UINT SelectionMade = -1;
	CMenu Menu;
	if(Menu.LoadMenu(IDR_DATUM))
	{
		CMenu* pSubMenu = Menu.GetSubMenu(0);
		if(pSubMenu!=nullptr)
		{
			if(pDatum!=nullptr)
				pSubMenu->EnableMenuItem(ID_DATUM_ADD, MF_DISABLED|MF_GRAYED);
			else
				pSubMenu->EnableMenuItem(ID_DATUM_DELETE, MF_DISABLED|MF_GRAYED);
			if(m_tin.m_datumlist.GetSize()<3)
				pSubMenu->EnableMenuItem(ID_DATUM_CREATETINS, MF_DISABLED|MF_GRAYED);
			if(m_tin.m_datumlist.GetSize()==0)
				pSubMenu->EnableMenuItem(ID_DATUM_CLEAR, MF_DISABLED|MF_GRAYED);
			if(m_tin.m_trianglelist.GetSize()==0)
				pSubMenu->EnableMenuItem(ID_DATUM_CLEARTINS, MF_DISABLED|MF_GRAYED);
			const Gdiplus::Point cliPoint1 = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
			CPoint cliPoint2(cliPoint1.X, cliPoint1.Y);
			pWnd->ClientToScreen(&cliPoint2);
			SelectionMade = pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, cliPoint2.x, cliPoint2.y, pWnd);
			pSubMenu->DestroyMenu();
		}

		Menu.DestroyMenu();
	}

	if(pWnd!=nullptr)
	{
		CPointF geoPoint = viewinfo.m_datainfo.DocToGeo(docPoint);
		switch(SelectionMade)
		{
		case ID_DATUM_ADD:
		{
			pDatum = new CDatum(docPoint);
			m_tin.AddDatum(pDatum);
			pDatum->Draw(pWnd, viewinfo);
			m_pDatum = pDatum;
		}
		break;
		case ID_DATUM_DELETE:
			if(pDatum!=nullptr)
			{
				m_tin.Invalidate(pWnd, viewinfo);
				m_tin.RemoveDatum(pDatum);
				m_tin.Create();
				m_tin.Invalidate(pWnd, viewinfo);
			}
			break;
		case ID_DATUM_CREATETINS:
		{
			m_tin.Invalidate(pWnd, viewinfo);
			m_tin.Create();
			m_tin.Invalidate(pWnd, viewinfo);
		}
		break;
		case ID_DATUM_CLEARTINS:
		{
			m_tin.Invalidate(pWnd, viewinfo);
			m_tin.ClearTins();
		}
		break;
		case ID_DATUM_CLEAR:
		{
			m_tin.Invalidate(pWnd, viewinfo);
			m_tin.Clear();
		}
		break;
		}
	}
}

bool CDatumTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_DATUM);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
