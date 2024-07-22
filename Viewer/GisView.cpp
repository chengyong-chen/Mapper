#include "stdafx.h"
#include "resource.h"
#include <afxpriv.h>
#include <math.h>
#include  <stdio.h>
#include  <stdlib.h>

#include "Global.h"
#include "GisView.h"
#include "GisDoc.h"
#include "MainFrm.h"

#include "../Dataview/Background.h"

#include "../Mapper/Global.h"

#include "../Atlas/Atlas1.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Ellipse.h"

#include "../Geometry/Mark.h"
#include "../Geometry/Text.h"

#include "../Layer/Global.h"
#include "../Layer/LayerPane.h"
#include "../Layer/BackgroundBar.h"

#include "../Tool/Global.h"
#include "../Tool/LinkTool.h"
#include "../Tool/InfoTool.h"
#include "../Tool/RouteTool.h"

#include "../Public/AttributeCtrl.h"
#include "../Public/PropertyPane.h"

#include "../Information/Global.h"
#include "../Information/TDBGridCtrl.h"
#include "../Information/Truedbgridctrl.h"
#include "../Information/POUListCtrl.h"
#include "../Information/POUPane.h"
#include "../Information/POUTDBGridCtrl.h"
#include "../Information/WhereDlg.h"
#include "../Information/TablePane.h"
#include "../Information/ATTTDBGridCtrl.h"
#include "../Information/POUDBGridCtrl.h"
#include "../Projection/Projection1.h"

#include "../Navigate/NavinfoDlg.h"
#include "../Navigate/NavinfoPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern __declspec(dllimport) CLinkTool linkTool;
extern __declspec(dllimport) CInfoTool infoTool;
extern __declspec(dllimport) CRouteTool routeTool;

IMPLEMENT_GWDYNCREATE(CGisView, CGrfView)

BEGIN_MESSAGE_MAP(CGisView, CGrfView)
	//{{AFX_MSG_MAP(CGisView)
	ON_WM_TIMER()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipNotify)
	ON_COMMAND(ID_TOOL_LINK, OnToolLink)
	ON_COMMAND(ID_TOOL_INFO, OnToolInfo)
	ON_COMMAND(ID_TOOL_ROUTE, OnToolRoute)
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_MESSAGE(WM_PICKLINK, OnPickLink)
	ON_MESSAGE(WM_RGNQUERY, OnRgnQuery)
	ON_MESSAGE(WM_HIGHLIGHTATT, OnHighlightATT)
	ON_MESSAGE(WM_SHOWRECINFO, OnShowRecInfo)
	ON_MESSAGE(WM_LAYERACTIVATED, OnLayerActivated)
	ON_MESSAGE(WM_POUACTIVATED, OnPOUActivated)
	ON_MESSAGE(WM_SETROUTE, OnSetRoute)
	ON_MESSAGE(WM_CHANGEBACKGROUND, OnChangeBackground)
	ON_MESSAGE(WM_ALPHATRANSPARENCY, OnAlphaTransparency)
END_MESSAGE_MAP()

CGisView::CGisView(CObject& parameter)
	: CGrfView(parameter), m_document((CGisDoc&)parameter)
{
}

CGisView::~CGisView()
{
	m_tooltip.PostMessage(WM_DESTROY, 0, 0);
	m_tooltip.DestroyWindow();

	//HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();
	//LONG nStyle = ::GetWindowLong(hWnd,GWL_EXSTYLE);
	//if((nStyle & WS_EX_LAYERED) == WS_EX_LAYERED)
	//{
	//	::SetWindowLong(hWnd,GWL_EXSTYLE, nStyle & ~WS_EX_LAYERED);
	//	::RedrawWindow(hWnd, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
	//}	

	//CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	//if(pMainFrame != nullptr)
	//{
	//	pMainFrame->m_LayerPane.m_pBackgroundBar->Disable();
	//}
}

#ifdef _DEBUG
void CGisView::AssertValid() const
{
	CGrfView::AssertValid();
}

void CGisView::Dump(CDumpContext& dc) const
{
	CGrfView::Dump(dc);
}

#endif//_DEBUG

void CGisView::OnInitialUpdate()
{
	CGrfView::OnInitialUpdate();

	EnableToolTips(FALSE);
	if(m_tooltip.Create(this, TTS_ALWAYSTIP) == TRUE)
	{
		m_tooltip.Activate(TRUE);
		m_tooltip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
		m_tooltip.SetDelayTime(3000);
		m_tooltip.AddTool(this, LPSTR_TEXTCALLBACK);
	}

	if(m_document.m_Datainfo.m_pProjection != nullptr)
	{
		static CString strProvider;
		static CString strVariety;
		strProvider=m_document.m_Datainfo.m_BackInfo.m_strProvider;
		strVariety=m_document.m_Datainfo.m_BackInfo.m_strVariety;
		if(strProvider != _T(""))
		{
			CWinThread* pThread=AfxBeginThread(ThreadSetBackground, (LPVOID)(this), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, nullptr);
			if(pThread != nullptr)
			{
				ResetEvent(m_hDrawOver);
				pThread->m_pMainWnd=AfxGetMainWnd();
				pThread->ResumeThread();
			}
		}
	}
}

BOOL CGisView::PreTranslateMessage(MSG* pMsg)
{
	if(m_tooltip.m_hWnd != nullptr)
	{
		m_tooltip.RelayEvent(pMsg);
	}

	return CGrfView::PreTranslateMessage(pMsg);
}

void CGisView::OnMouseMove(UINT nFlags, CPoint point)
{
	CGrfView::OnMouseMove(nFlags, point);

	static int oldX;
	static int oldY;
	if(point.x != oldX || point.y != oldY)
	{
		OutputDebugString(_T("OnMouseMove"));
		this->KillTimer(1);
		this->SetTimer(1, 200, nullptr);
	}
	oldX=point.x;
	oldY=point.y;
}

void CGisView::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
		case 1:
		{
			OutputDebugString(_T("OnTimer"));
			this->KillTimer(1);
			if(::IsWindow(m_tooltip.GetSafeHwnd()))
			{
				m_tooltip.Update();
			}
		}
		break;
		default:
			ASSERT(FALSE);
	}

	CGrfView::OnTimer(nIDEvent);
}

void CGisView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CGrfView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	if(bActivate == TRUE && pActivateView == this)
	{
		CMainFrame* pMainFrame=(CMainFrame*)AfxGetApp()->m_pMainWnd;
		if(pMainFrame != nullptr)
		{
			if(m_document.m_Datainfo.m_pProjection != nullptr)
			{
				pMainFrame->m_LayerPane.EnableBackgroundBar(TRUE);
				if(m_viewMonitor.m_pBackground != nullptr)
					pMainFrame->m_LayerPane.m_pBackgroundBar->Enable(m_viewMonitor.m_pBackground->m_strProvider, m_viewMonitor.m_pBackground->m_strVariety);
				else
					pMainFrame->m_LayerPane.m_pBackgroundBar->Enable("", "");
			}
			else
			{
				pMainFrame->m_LayerPane.EnableBackgroundBar(FALSE);
			}
		}
		CPOUPane* pPOUPane=(CPOUPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'P', 0);
		if(pPOUPane != nullptr)
		{
			pPOUPane->SetMapCtrl(&m_document.m_POUListCtrl);
		}

		bool bHasPOUDatabase=false;
		bool bHasAATDatabase=false;
		if(m_document.m_poulist.m_DBList.size() > 0)
			bHasPOUDatabase=true;
		if(m_document.m_layertree.m_AttDBList.size() > 0)
			bHasAATDatabase=true;
		if(pMainFrame->m_pAtlas != nullptr && pMainFrame->m_pAtlas->m_poulist.m_DBList.size() > 0)
			bHasPOUDatabase=true;

		if(bHasPOUDatabase == true && bHasAATDatabase == false)
			pMainFrame->m_TablePane.ActivateTab('P');
		else if(bHasPOUDatabase == false && bHasAATDatabase == true)
			pMainFrame->m_TablePane.ActivateTab('A');

		this->SendMessage(WM_LAYERACTIVATED, 0, 0);
		this->SendMessage(WM_POUACTIVATED, 0, 0);
	}
}

LONG CGisView::OnRgnQuery(UINT WPARAM, LONG LPARAM)
{
	CGrfView::OnRgnQuery(WPARAM, LPARAM);

	CMainFrame* pMainFrame=(CMainFrame*)AfxGetApp()->m_pMainWnd;
	if(WPARAM == 0)
		return 0;

	CPOU* pPOU=m_document.m_POUListCtrl.GetActive();
	if(pPOU == nullptr && pMainFrame->m_pAtlas != nullptr)
		pMainFrame->m_pAtlas->m_POUListCtrl.GetActive();
	if(pPOU != nullptr)
	{
		CODBCDatabase* pODBCDatabase=m_document.m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);
		const CHeaderProfile& headerprofile=pPOU->m_datasource.GetHeaderProfile();
		CString strTable;
		CString strWhere;

		HINSTANCE hInstOld=AfxGetResourceHandle();
		HINSTANCE hInst=::LoadLibrary(_T("Information.dll"));
		AfxSetResourceHandle(hInst);

		CWhereDlg dlg(nullptr, pODBCDatabase, strTable, strWhere);
		if(dlg.DoModal() == IDOK)
		{
			strTable=dlg.m_strTable;
			strWhere=dlg.m_strWhere;

			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
		}
		else
		{
			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
			return 0;
		}

		if(strTable.IsEmpty() == TRUE)
			return 0;

		switch(LPARAM)
		{
			case 0:
			{
				try
				{
					pODBCDatabase->ExecuteSQL(_T("DROP Table Query"));
				}
				catch(CDBException* ex)
				{
					OutputDebugString(ex->m_strError);
					ex->Delete();
				}

				CPoly* poly=(CPoly*)WPARAM;
				if(poly == nullptr)
					break;
				if(poly->m_nAnchors == 0)
					break;

				CRect docRect=poly->m_Rect;

				CPointF geoPoint1=m_document.m_Datainfo.DocToWGS84(docRect.TopLeft());
				CPointF geoPoint2=m_document.m_Datainfo.DocToWGS84(docRect.BottomRight());
				CRectF inRect;
				inRect.left=geoPoint1.x;
				inRect.top=geoPoint1.y;
				inRect.right=geoPoint2.x;
				inRect.bottom=geoPoint2.y;

				try
				{
					if(strWhere.IsEmpty() == FALSE)
						strWhere=strWhere + _T(" And ");

					CString strSQL;
					strSQL.Format(_T("Select*INTO Query FROM [%s] WHERE %s X>%g And X<%g And Y>%g And Y<%g"), strTable, strWhere, inRect.left, inRect.right, inRect.top, inRect.bottom);
					CODBCDatabase::ParseSQL(pODBCDatabase, strSQL);
					pODBCDatabase->ExecuteSQL(strSQL);
				}
				catch(CDBException* ex)
				{
					OutputDebugString(ex->m_strError);
					ex->ReportError();
					ex->Delete();
				}

				CPointF* pPoints=new CPointF[poly->m_nAnchors];
				for(unsigned int nAnchor=0; nAnchor < poly->m_nAnchors; nAnchor++)
				{
					pPoints[nAnchor]=m_document.m_Datainfo.DocToWGS84(poly->m_pPoints[nAnchor]);
				}

				try
				{
					CRecordset rs(pODBCDatabase);
					rs.Open(CRecordset::dynaset, _T("Select * From QUERY"));//,CRecordset::none);
					while(rs.IsEOF() == FALSE)
					{
						CPointF geoPoint;

						CString strValue;
						rs.GetFieldValue(_T("X"), strValue);
						geoPoint.x=_tcstod(strValue, nullptr);
						if(geoPoint.x == 0.0f)
						{
							rs.Delete();
							rs.MoveNext();
							continue;
						}

						rs.GetFieldValue(_T("Y"), strValue);
						geoPoint.y=_tcstod(strValue, nullptr);
						if(geoPoint.y == 0.0f)
						{
							rs.Delete();
							rs.MoveNext();
							continue;
						}
						if(CPoly::PointInPolygon<CPointF>(geoPoint, pPoints, poly->m_nAnchors) == false)
						{
							rs.Delete();
						}

						rs.MoveNext();
					}
					rs.Close();
					pODBCDatabase->BeginTrans();
					pODBCDatabase->CommitTrans();
				}
				catch(CDBException* ex)
				{
					OutputDebugString(ex->m_strError);
					ex->ReportError();
					ex->Delete();
				}

				delete[] pPoints;
				CString strSQL=_T("Select * FROM Query");
				pMainFrame->m_TablePane.SQLQuery('P', pODBCDatabase, strSQL, headerprofile.m_strIdField);
			}
			break;
			case 1:
			{
				CRect docRect=*(CRect*)WPARAM;
				CPointF geoPoint1=m_document.m_Datainfo.DocToWGS84(docRect.TopLeft());
				CPointF geoPoint2=m_document.m_Datainfo.DocToWGS84(docRect.BottomRight());
				CRectF inRect;
				inRect.left=geoPoint1.x;
				inRect.top=geoPoint1.y;
				inRect.right=geoPoint2.x;
				inRect.bottom=geoPoint2.y;
				if(strWhere.IsEmpty() == FALSE)
					strWhere=strWhere + _T(" And ");
				CString strSQL;
				strSQL.Format(_T("Select * FROM [%s] WHERE %s X>%g And X<%g And Y>%g And Y<%g"), strTable, strWhere, inRect.left, inRect.right, inRect.top, inRect.bottom);
				pMainFrame->m_TablePane.SQLQuery('P', pODBCDatabase, strSQL, headerprofile.m_strIdField);
			}
			break;
			case 2:
			{
				CRect docRect=*(CRect*)WPARAM;
				CPointF geoPoint1=m_document.m_Datainfo.DocToWGS84(docRect.TopLeft());
				CPointF geoPoint2=m_document.m_Datainfo.DocToWGS84(docRect.BottomRight());

				CPointF geoPoint;
				CSizeF geoSize;
				geoPoint.x=geoPoint1.x + (geoPoint2.x - geoPoint1.x)/2;
				geoPoint.y=geoPoint1.y + (geoPoint2.y - geoPoint1.y)/2;
				geoSize.cx=std::abs((geoPoint2.x - geoPoint1.x)/2);
				geoSize.cy=std::abs((geoPoint2.y - geoPoint1.y)/2);
				if(strWhere.IsEmpty() == FALSE)
					strWhere=strWhere + _T(" And ");

				CString strSQL;
				strSQL.Format(_T("SELECT * FROM [%s] WHERE %s (X-%g)*(X-%g)*%.4f*%.4f + (Y-%g)*(Y-%g)*%.4f*%.4f<(%g*%g*%g*%g)"), strTable, strWhere, geoPoint.x, geoPoint.x, geoSize.cy, geoSize.cy, geoPoint.y, geoPoint.y, geoSize.cx, geoSize.cx, geoSize.cx, geoSize.cx, geoSize.cy, geoSize.cy);
				CODBCDatabase::ParseSQL(((CODBCDatabase*)pODBCDatabase), strSQL);

				pMainFrame->m_TablePane.SQLQuery('P', pODBCDatabase, strSQL, headerprofile.m_strIdField);
			}
			break;
			case 3:
				break;
		}
	}
	else if(m_document.m_layertree.m_AttDBList.size() > 0)
	{
		CLayer* layer=this->GetActiveLayer();
		if(layer == nullptr)
			return 0;
		CATTDatasource* pDatasource=layer->GetAttDatasource();
		if(pDatasource == nullptr)
			return 0;
		CDatabase* pDatabase=m_document.m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
		if(pDatabase == nullptr)
			return 0;

		CString strTable;
		CString strWhere;
		HINSTANCE hInstOld=AfxGetResourceHandle();
		HINSTANCE hInst=::LoadLibrary(_T("Information.dll"));
		AfxSetResourceHandle(hInst);

		const CHeaderProfile& headerprofile=pDatasource->GetHeaderProfile();
		CWhereDlg dlg(nullptr, pDatabase, headerprofile.m_strKeyTable, strWhere);
		if(dlg.DoModal() == IDOK)
		{
			strTable=dlg.m_strTable;
			strWhere=dlg.m_strWhere;

			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
		}
		else
		{
			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
			return 0;
		}

		if(strTable.IsEmpty() == TRUE)
			return 0;

		CString strSQL;
		strSQL.Format(_T("Select * FROM [%s]"), strTable);
		if(strWhere.IsEmpty() == FALSE)
		{
			strSQL+=_T(" Where ") + strWhere;
		}

		switch(LPARAM)
		{
			case 0:
			{
				CPoly* poly=(CPoly*)WPARAM;
				if(poly == nullptr)
					break;
				if(poly->m_nAnchors == 0)
					break;

				pMainFrame->m_TablePane.SQLQuery('A', pDatabase, strSQL, headerprofile.m_strIdField);
			}
			break;
			case 1:
			{
				CRect docRect=*(CRect*)WPARAM;
				pMainFrame->m_TablePane.SQLQuery('A', pDatabase, strSQL, headerprofile.m_strIdField);
			}
			break;
			case 2:
			{
				CRect rect=*(CRect*)WPARAM;
				pMainFrame->m_TablePane.SQLQuery('A', pDatabase, strSQL, headerprofile.m_strIdField);
			}
			break;
			case 3:
				break;
		}
	}

	return 0;
}

LONG CGisView::OnPickLink(UINT WPARAM, LONG LPARAM)
{
	const CPoint cliPoint=*(CPoint*)WPARAM;
	CPoint docPoint=m_viewMonitor.ClientToDoc(cliPoint, false);
	CGeom* pGeom=(CGeom*)this->SendMessage(WM_LOOKUPGEOM, (UINT)&docPoint, 3);
	if(pGeom == nullptr)
		return 0;

	for(std::list<CLink*>::reverse_iterator it=m_document.m_linklist.rbegin(); it != m_document.m_linklist.rend(); ++it)
	{
		CLink* link=*it;
		if(link->m_dwGeomId == pGeom->m_geoId)
			return (LONG)link;
		const unsigned int gap=0;
		if(pGeom->PickOn(docPoint, gap) == TRUE || pGeom->PickIn(docPoint) == TRUE);
		{
			return (LONG)link;
		}
	}

	return 0;
}

BOOL CGisView::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	OutputDebugString(_T("OnToolTipNotifyn"));
	CPoint cliPoint;
	VERIFY(::GetCursorPos(&cliPoint));
	ScreenToClient(&cliPoint);

	TOOLTIPTEXT* pTTT=(TOOLTIPTEXT*)pNMHDR;
	if(m_document.m_poulist.m_DBList.size() > 0)
	{
		CPoint docPoint=this->m_viewMonitor.ClientToDoc(cliPoint, false);
		for(std::list<CPOU*>::iterator it=m_document.m_poulist.m_pous.begin(); it != m_document.m_poulist.m_pous.end(); ++it)
		{
			CPOU* pPOU=*it;
			if(pPOU->m_bVisible == false)
				continue;
			CODBCDatabase* pDatabase=m_document.m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);
			const DWORD dwId=pPOU->Pick(this, docPoint, m_viewMonitor);
			if(dwId != -1)
			{
				const CHeaderProfile& headerprofile=pPOU->m_datasource.GetHeaderProfile();
				const CString strTag=pDatabase->GetTag(headerprofile.m_strKeyTable, dwId);
				static TCHAR buffer[1024];
				memset(buffer, 0, 1024);
				_tcsncpy(buffer, strTag, min(1024, strTag.GetLength()));
				pTTT->lpszText=buffer;
				*pResult=0;
				return TRUE;
			}
		}
	}

	if(m_document.m_layertree.m_AttDBList.size() > 0)
	{
		CPoint docPoint=this->m_viewMonitor.ClientToDoc(cliPoint, false);
		CGeom* pGeom=(CGeom*)this->SendMessage(WM_LOOKUPGEOM, (UINT)&docPoint, 1);
		if(pGeom != nullptr)
		{
			CLayer* layer=m_document.m_layertree.GetLayerByGeom(pGeom);
			if(layer != nullptr)
			{
				CATTDatasource* pDatasource=layer->GetAttDatasource();
				if(pDatasource != nullptr)
				{
					CDatabase* pDatabase=m_document.m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
					if(pDatabase != nullptr && pDatabase->IsOpen() == TRUE)
					{
						const CHeaderProfile& headerprofile=pDatasource->GetHeaderProfile();
						const CString strTable=headerprofile.m_strKeyTable;
						const CString strTag=((CODBCDatabase*)pDatabase)->GetTag(strTable, pGeom->m_attId);
						static TCHAR buffer[1024];
						memset(buffer, 0, 1024);
						_tcsncpy(buffer, strTag, min(1024, strTag.GetLength()));
						pTTT->lpszText=buffer;
						*pResult=0;
						return TRUE;
					}
				}
			}
		}
	}
	*pResult=0;
	pTTT->szText[0]=0;
	pTTT->lpszText=nullptr;
	return TRUE; // message was handled
}

LONG CGisView::OnShowRecInfo(UINT WPARAM, LONG LPARAM)
{
	if(m_document.m_poulist.m_DBList.size() > 0)
	{
		CPoint docPoint(WPARAM, LPARAM);
		for(std::list<CPOU*>::iterator it=m_document.m_poulist.m_pous.begin(); it != m_document.m_poulist.m_pous.end(); ++it)
		{
			CPOU* pPOU=*it;
			if(pPOU->m_bVisible == false)
				continue;
			const DWORD dwId=pPOU->Pick(this, docPoint, m_viewMonitor);
			if(dwId != -1)
			{
				CODBCDatabase* pDatabase=m_document.m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);

				CTablePane* pTablePane=(CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
				if(pTablePane != nullptr && pTablePane->IsVisible() == TRUE)
				{
					pTablePane->ActivateTab('P');
					if(pTablePane->m_pPOUTDBGridCtrl != nullptr && pTablePane->m_pPOUTDBGridCtrl->IsWindowVisible() == TRUE)
					{
						CHeaderProfile& headerprofile=pPOU->m_datasource.GetHeaderProfile();
						pTablePane->m_pPOUTDBGridCtrl->Fill(pDatabase, &headerprofile, TRUE);
						pTablePane->m_pPOUTDBGridCtrl->ActivateRec(headerprofile.m_strIdField, dwId, TRUE);
					}
				}

				CPropertyPane* pPropertyPane=(CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
				if(pPropertyPane != nullptr && pPropertyPane->IsVisible() == TRUE)
				{
					pPropertyPane->ActivateTab('A');

					CAttributeCtrl* pAttributeCtrl=(CAttributeCtrl*)pPropertyPane->GetTabWnd('A');
					if(pAttributeCtrl != nullptr && pAttributeCtrl->IsWindowVisible() == TRUE)
					{
						const CHeaderProfile& headerprofile=pPOU->m_datasource.GetHeaderProfile();
						pAttributeCtrl->OnTableChanged(pDatabase, headerprofile.m_strKeyTable);
						pAttributeCtrl->ShowInfo(headerprofile.m_strIdField, dwId);
					}
				}

				return 1;
			}
		}
	}

	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd();
	if(pMainFrame != nullptr && pMainFrame->m_pAtlas != nullptr && pMainFrame->m_pAtlas->m_poulist.m_DBList.size() > 0)
	{
		CPoint docPoint(WPARAM, LPARAM);
		for(std::list<CPOU*>::reverse_iterator it=pMainFrame->m_pAtlas->m_poulist.m_pous.rbegin(); it != pMainFrame->m_pAtlas->m_poulist.m_pous.rend(); ++it)
		{
			CPOU* pPOU=*it;
			if(pPOU->m_bVisible == false)
				continue;
			const DWORD dwId=pPOU->Pick(this, docPoint, m_viewMonitor);
			if(dwId != -1)
			{
				CODBCDatabase* pDatabase=m_document.m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);

				const CMainFrame* pMainFrame=(CMainFrame*)AfxGetApp()->m_pMainWnd;
				CTablePane* pTablePane=(CTablePane*)pMainFrame->SendMessage(WM_GETPANE, 'O', 0);
				if(pTablePane != nullptr && pTablePane->IsVisible() == TRUE)
				{
					pTablePane->ActivateTab('P');

					if(pTablePane->m_pPOUTDBGridCtrl != nullptr && pTablePane->m_pPOUTDBGridCtrl->IsWindowVisible() == TRUE)
					{
						CHeaderProfile& headerprofile=pPOU->m_datasource.GetHeaderProfile();
						pTablePane->m_pPOUTDBGridCtrl->Fill(pDatabase, &headerprofile, TRUE);
						pTablePane->m_pPOUTDBGridCtrl->ActivateRec(headerprofile.m_strIdField, dwId, TRUE);
					}
				}

				CPropertyPane* pPropertyPane=(CPropertyPane*)pMainFrame->SendMessage(WM_GETPANE, 'I', 0);
				if(pPropertyPane != nullptr && pPropertyPane->IsVisible() == TRUE)
				{
					pPropertyPane->ActivateTab('A');

					CAttributeCtrl* pAttributeCtrl=(CAttributeCtrl*)pPropertyPane->GetTabWnd('A');
					if(pAttributeCtrl != nullptr && pAttributeCtrl->IsWindowVisible() == TRUE)
					{
						const CHeaderProfile& headerprofile=pPOU->m_datasource.GetHeaderProfile();
						pAttributeCtrl->OnTableChanged(pDatabase, headerprofile.m_strKeyTable);
						pAttributeCtrl->ShowInfo(headerprofile.m_strIdField, dwId);
					}
				}

				return 1;
			}
		}
	}

	if(m_document.m_layertree.m_AttDBList.size() > 0)
	{
		CPoint docPoint=CPoint(WPARAM, LPARAM);
		//		CPoint docPoint = this->m_viewMonitor.ClientToDoc(cliPoint);
		CGeom* pGeom=(CGeom*)this->SendMessage(WM_LOOKUPGEOM, (UINT)&docPoint, 1);
		if(pGeom != nullptr)
		{
			CLayer* layer=m_document.m_layertree.GetLayerByGeom(pGeom);
			if(layer == nullptr)
				return 0;
			CATTDatasource* pDatasource=layer->GetAttDatasource();
			if(pDatasource == nullptr)
				return 0;
			CDatabase* pDatabase=m_document.m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
			if(pDatabase == nullptr)
				return 0;

			const CMainFrame* pMainFrame=(CMainFrame*)AfxGetApp()->m_pMainWnd;
			CTablePane* pTablePane=(CTablePane*)pMainFrame->SendMessage(WM_GETPANE, 'O', 0);
			if(pTablePane != nullptr && pTablePane->IsVisible() == TRUE)
			{
				pTablePane->ActivateTab('A');
				if(pTablePane->m_pATTTDBGridCtrl != nullptr)
				{
					CHeaderProfile& headerprofile=pDatasource->GetHeaderProfile();
					pTablePane->m_pATTTDBGridCtrl->Fill(pDatabase, &headerprofile, TRUE);
					pTablePane->m_pATTTDBGridCtrl->ActivateRec(headerprofile.m_strIdField, pGeom->m_attId, TRUE);
				}
			}

			CPropertyPane* pPropertyPane=(CPropertyPane*)pMainFrame->SendMessage(WM_GETPANE, 'I', 0);
			if(pPropertyPane != nullptr && pPropertyPane->IsVisible() == TRUE)
			{
				pPropertyPane->ActivateTab('A');

				CAttributeCtrl* pAttributeCtrl=(CAttributeCtrl*)pPropertyPane->GetTabWnd('A');
				if(pAttributeCtrl != nullptr)
				{
					const CHeaderProfile& headerprofile=pDatasource->GetHeaderProfile();
					pAttributeCtrl->OnTableChanged(pDatabase, headerprofile.m_strKeyTable);
					pAttributeCtrl->ShowInfo(headerprofile.m_strIdField, pGeom->m_attId);
				}
			}
			if(m_hlGeomList.Find(pGeom) == nullptr)
			{
				CGrfView::UnhighligtAll();

				layer->Invalidate(this, m_viewMonitor, pGeom);
				m_hlGeomList.AddTail(pGeom);
			}
		}
	}

	return 1;
}

BOOL CGisView::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo == nullptr)
	{
		if(nCode == CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI=(CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CGrfView::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nId)
			{
				case ID_TOOL_ROUTE:
					pCmdUI->SetCheck(m_document.m_pTool == &routeTool ? TRUE : FALSE);
					break;
				case ID_TOOL_LINK:
					pCmdUI->SetCheck(m_document.m_pTool == &linkTool);
					if(m_document.m_linklist.size() == 0)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->Enable(TRUE);
					return TRUE;
					break;
				case ID_TOOL_INFO:
				{
					const CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd();

					pCmdUI->SetCheck(m_document.m_pTool == &infoTool);

					if(m_document.m_layertree.m_AttDBList.size() > 0)
						pCmdUI->Enable(TRUE);
					else if(m_document.m_poulist.m_pous.size() > 0)
						pCmdUI->Enable(TRUE);
					else if(pMainFrame->m_pAtlas != nullptr && pMainFrame->m_pAtlas->m_poulist.m_pous.size() > 0)
						pCmdUI->Enable(TRUE);
					else
						pCmdUI->Enable(FALSE);
				}
				return TRUE;
				break;
				case ID_TOOL_ZOOMOUT:
					if(m_viewMonitor.m_levelCurrent > m_viewMonitor.m_levelMinimum)
					{
						if(m_document.m_pDeck != nullptr)
						{
							CDeck* pDeck=m_document.m_pDeck->GetParent();
							if(pDeck != nullptr)
							{
								pCmdUI->Enable(TRUE);
								return TRUE;
							}
						}
					}
					break;
				default:
					break;
			}

			pCmdUI->m_bContinueRouting=false;
		}
	}

	return CGrfView::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}

void CGisView::OnToolLink()
{
	m_document.m_pTool=&linkTool;
}

void CGisView::OnToolInfo()
{
	//	if(m_document.m_pTool == &infoTool)
	//	{
	//		m_document.m_pTool = CTool::m_oldTool;
	//		CTool::m_oldTool = &infoTool;		
	//	}
	//	else
	//	{
	//		CTool::m_oldTool = m_document.m_pTool;
	m_document.m_pTool=&infoTool;
	//	}
}

void CGisView::OnToolRoute()
{
	m_document.m_pTool=&routeTool;
}

void CGisView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if((nFlags & MK_LBUTTON) == MK_LBUTTON)
	{
		CGrfView::OnRButtonDown(nFlags, point);
		return;
	}

	CView::OnRButtonDown(nFlags, point);

	for(CTree<CLayer>::forwiterator it=m_document.m_layertree.forwbegin(); it != m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer=*it;
		if(layer->m_nActiveCount != 1)
			continue;

		CGeom* pGeom=nullptr;
		POSITION pos2=layer->m_geomlist.GetHeadPosition();
		while(pos2 != nullptr)
		{
			CGeom* geom=layer->m_geomlist.GetNext(pos2);
			if(geom->m_bActive == true)
			{
				pGeom=geom;
				break;
			}
		}

		if(pGeom == nullptr)
			break;

		CMenu Menu;
		if(Menu.LoadMenu(IDR_GEOM) == FALSE)
		{
			break;
		}

		CMenu* pSubMenu=Menu.GetSubMenu(0);
		if(pSubMenu == nullptr)
		{
			Menu.DestroyMenu();
			break;
		}

		if(pGeom->m_bClosed == false)
		{
			pSubMenu->EnableMenuItem(ID_GEOM_AREA, MF_DISABLED | MF_GRAYED);
		}

		if(pGeom->IsKindOf(RUNTIME_CLASS(CMark)) == TRUE || pGeom->IsKindOf(RUNTIME_CLASS(CText)) == TRUE)
		{
			pSubMenu->EnableMenuItem(ID_GEOM_LENGTH, MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_GEOM_AREA, MF_DISABLED | MF_GRAYED);
		}

		m_point=m_viewMonitor.ClientToDoc(point, false);
		this->ClientToScreen(&point);
		const UINT SelectionMode=pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, this);

		pSubMenu->DestroyMenu();
		Menu.DestroyMenu();

		switch(SelectionMode)
		{
			case ID_GEOM_LENGTH:
			{
				const double length=pGeom->GetLength(m_document.m_Datainfo, m_document.m_Datainfo.m_zoomPointToDoc);
				CString string;
				CString strLen;
				if(length > 1)
				{
					strLen.Format(_T("%.2f"), length);
					strLen.TrimRight(_T("0"));
					strLen.TrimRight(_T("."));
					string.LoadString(IDS_LENGTHKM);
				}
				else
				{
					const int nlength=(int)(length*1000);
					strLen.Format(_T("%d"), nlength);
					string.LoadString(IDS_LENGTHM);
				}
				string.Replace(_T("%s"), strLen);
				AfxMessageBox(string);
			}
			break;
			case ID_GEOM_AREA:
			{
				if(pGeom->m_bClosed == true)
				{
					double area=pGeom->GetArea(m_document.m_Datainfo);
					area=std::abs(area);

					CString string;
					CString strArea;
					if(area > 0.01)
					{
						strArea.Format(_T("%.2f"), area);
						strArea.TrimRight(_T("0"));
						strArea.TrimRight(_T("."));
						string.LoadString(IDS_AREAKM);
					}
					else
					{
						area=area*1000000;
						strArea.Format(_T("%.0f"), area);
						strArea.TrimRight(_T("0"));
						strArea.TrimRight(_T("."));
						string.LoadString(IDS_AREAM);
					}
					string.Replace(_T("%s"), strArea);
					AfxMessageBox(string);
				}
			}
			break;
		}

		return;
	}

	CGrfView::OnRButtonDown(nFlags, point);
}

LONG CGisView::OnLayerActivated(UINT WPARAM, LONG LPARAM)
{
	const CMainFrame* pMainFrame=(CMainFrame*)AfxGetApp()->m_pMainWnd;
	CTablePane* pTablePane=(CTablePane*)pMainFrame->SendMessage(WM_GETPANE, 'O', 0);
	if(pTablePane != nullptr && pTablePane->IsVisible() == TRUE && pTablePane->m_pATTTDBGridCtrl != nullptr && pTablePane->m_pATTTDBGridCtrl->m_hWnd != nullptr)
	{
		CLayer* layer=(CLayer*)WPARAM;
		if(layer == nullptr)
			layer=GetActiveLayer();
		if(layer == nullptr)
		{
			pTablePane->m_pATTTDBGridCtrl->ClearGrid();
			return 0;
		}

		CATTDatasource* pDatasource=layer->GetAttDatasource();
		if(pDatasource == nullptr)
		{
			pTablePane->m_pATTTDBGridCtrl->ClearGrid();
			return 0;
		}

		CDatabase* pDatabase=m_document.m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
		if(pDatabase == nullptr)
		{
			pTablePane->m_pATTTDBGridCtrl->ClearGrid();
			return 0;
		}
		if(pDatabase->IsOpen() == FALSE)
		{
			pTablePane->m_pATTTDBGridCtrl->ClearGrid();
			return 0;
		}
		else
		{
			CHeaderProfile& headerprofile=pDatasource->GetHeaderProfile();
			pTablePane->m_pATTTDBGridCtrl->Fill(pDatabase, &headerprofile, TRUE);
			return 1;
		}
	}
	else
		return 0;
}

LONG CGisView::OnPOUActivated(UINT WPARAM, LONG LPARAM)
{
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetApp()->m_pMainWnd;
	if(pMainFrame == nullptr)
		return 0;

	CTablePane* pTablePane=(CTablePane*)pMainFrame->SendMessage(WM_GETPANE, 'O', 0);
	if(pTablePane == nullptr || pTablePane->m_pPOUTDBGridCtrl == nullptr)
		return 0;

	CPOUListCtrl* pPOUListCtrl=(CPOUListCtrl*)WPARAM;
	CPOU* pPOU=(CPOU*)LPARAM;
	if(pPOUListCtrl != nullptr && pPOU != nullptr)
	{
		CODBCDatabase* pDatabase=m_document.m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);
		CHeaderProfile& headerprofile=pPOU->m_datasource.GetHeaderProfile();
		pTablePane->m_pPOUTDBGridCtrl->Fill(pDatabase, &headerprofile, FALSE);
	}
	else
	{
		CPOU* pPOU=m_document.m_POUListCtrl.GetActive();
		if(pPOU != nullptr)
		{
			CODBCDatabase* pDatabase=m_document.m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);
			CHeaderProfile& headerprofile=pPOU->m_datasource.GetHeaderProfile();
			pTablePane->m_pPOUTDBGridCtrl->Fill(pDatabase, &headerprofile, FALSE);
		}
		else if(pMainFrame->m_pAtlas != nullptr)
		{
			CPOU* pPOU=pMainFrame->m_pAtlas->m_POUListCtrl.GetActive();
			if(pPOU != nullptr)
			{
				CODBCDatabase* pDatabase=pMainFrame->m_pAtlas->m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);
				CHeaderProfile& headerprofile=pPOU->m_datasource.GetHeaderProfile();
				pTablePane->m_pPOUTDBGridCtrl->Fill(pDatabase, &headerprofile, FALSE);
			}
			else
			{
				pTablePane->m_pPOUTDBGridCtrl->ClearGrid();
			}
		}
		else
		{
			pTablePane->m_pPOUTDBGridCtrl->ClearGrid();
		}
	}

	return TRUE;
}

LONG CGisView::OnHighlightATT(UINT WPARAM, LONG LPARAM)
{
	CGrfView::UnhighligtAll();
	const CString tableATT=*(CString*)WPARAM;

	CTypedPtrList<CObList, CGeom*> list;
	CRect docRect;
	for(CTree<CLayer>::forwiterator it=m_document.m_layertree.forwbegin(); it != m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer=*it;
		CATTDatasource* pATTDatasource=layer->GetAttDatasource();
		if(pATTDatasource == nullptr)
			continue;
		const CHeaderProfile& headerprofile=pATTDatasource->GetHeaderProfile();
		if(headerprofile.m_strKeyTable == tableATT)
		{
			CTypedPtrList<CObList, CGeom*>& geomlist=layer->GetGeomList();
			POSITION Pos3=geomlist.GetHeadPosition();
			while(Pos3 != nullptr)
			{
				CGeom* pGeom=geomlist.GetNext(Pos3);
				if(pGeom->m_attId == LPARAM)
				{
					docRect=UnionRect(docRect, pGeom->m_Rect);
					m_hlGeomList.AddTail(pGeom);
					break;
				}
			}
		}
	}

	if(m_hlGeomList.GetCount() > 0)
	{
		const CPoint docPoint=docRect.CenterPoint();
		CRect client;
		GetClientRect(client);
		const CPoint cliPoint=client.CenterPoint();
		const Gdiplus::Rect cliRect=m_viewMonitor.DocToClient <Gdiplus::Rect>(docRect);
		CRect cliView;
		GetClientRect(cliView);
		if(cliRect.Width != 0 && cliRect.Height != 0 && (cliRect.Width > cliView.Width() || cliRect.Height > cliView.Height()))
		{
			const float xRatio=(float)cliView.Width()/cliRect.Width;
			const float yRatio=(float)cliView.Height()/cliRect.Height;
			const float fRatio=min(xRatio, yRatio);
			m_viewMonitor.ZoomBy(this, fRatio, cliPoint);
		}
		const CPointF mapPoint=m_viewMonitor.m_datainfo.DocToMap(docPoint);
		m_viewMonitor.FixateAt(this, mapPoint, cliPoint);
	}
	return 0;
}

LONG CGisView::OnChangeBackground(UINT WPARAM, LONG LPARAM)
{
	if(m_document.m_Datainfo.m_pProjection == nullptr)
		return 0;
	const CStringA strProvider=*(CStringA*)WPARAM;
	const CStringA strVariety=*(CStringA*)LPARAM;

	CRect cliRect;
	this->GetClientRect(cliRect);
	const CPoint cliPoint1=cliRect.TopLeft();
	const CPoint cliPoint2=cliRect.BottomRight();
	const CPointF geoPoint1=m_viewMonitor.ClientToWGS84(cliPoint1);
	const CPointF geoPoint2=m_viewMonitor.ClientToWGS84(cliPoint2);
	const CPointF geoPoint((geoPoint1.x + geoPoint2.x)/2, (geoPoint1.y + geoPoint2.y)/2);
	const float fScale=m_viewMonitor.m_scaleCurrent;
	const HWND hWnd=AfxGetMainWnd()->GetSafeHwnd();
	const LONG nStyle=::GetWindowLong(hWnd, GWL_EXSTYLE);

	if(strProvider == _T("") || strVariety == _T(""))
	{
		if(m_viewMonitor.m_pBackground != nullptr)
		{
			m_viewMonitor.RemoveBackground();

			m_viewMonitor.m_scaleCurrent=0;
			const CPointF mapPoint=m_viewMonitor.m_datainfo.m_pProjection == nullptr ? geoPoint : m_viewMonitor.m_datainfo.m_pProjection->GeoToMap(geoPoint);
			m_viewMonitor.ScaleTo(this, fScale, mapPoint);
		}

		if((nStyle & WS_EX_LAYERED) == WS_EX_LAYERED)
		{
			::SetWindowLong(hWnd, GWL_EXSTYLE, nStyle & ~WS_EX_LAYERED);
			::RedrawWindow(hWnd, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
		}
	}
	else if(m_viewMonitor.m_pBackground == nullptr)
	{
		CRect rect=cliRect;
		this->ClientToScreen(rect);

		CBackground* pBackground=new CBackground(m_viewMonitor);
		if(pBackground->Create(this, &StaticCallback, rect) == TRUE)
		{
			AfxGetApp()->BeginWaitCursor();
			const HWND hWnd=AfxGetMainWnd()->GetSafeHwnd();
			pBackground->SetWindowPos(hWnd, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOACTIVATE);
			if(pBackground->OpenBack(this->m_hWnd, strProvider, strVariety) == TRUE)
			{
				if((nStyle & WS_EX_LAYERED) != WS_EX_LAYERED)
				{
					int nAlpha=255;
					CMainFrame* pMainFrame=(CMainFrame*)AfxGetApp()->m_pMainWnd;
					if(pMainFrame != nullptr)
					{
						nAlpha=pMainFrame->m_LayerPane.m_pBackgroundBar->GetFroegroundTransparency();
					}
					::SetWindowLong(hWnd, GWL_EXSTYLE, nStyle | WS_EX_LAYERED);
					::SetLayeredWindowAttributes(hWnd, RGB(255, 255, 254), nAlpha, LWA_COLORKEY | LWA_ALPHA);
				}

				m_viewMonitor.SetBackground(pBackground);
				pBackground->ScaleTo(fScale, geoPoint);
			}
			else
			{
				delete pBackground;
				pBackground=nullptr;
			}

			AfxGetApp()->EndWaitCursor();
		}
		else
		{
			delete pBackground;
			pBackground=nullptr;
		}
	}
	else if(m_viewMonitor.m_pBackground->m_strProvider != strProvider || m_viewMonitor.m_pBackground->m_strVariety != strVariety)
	{
		AfxGetApp()->BeginWaitCursor();
		if(m_viewMonitor.m_pBackground->OpenBack(this->m_hWnd, strProvider, strVariety) == FALSE)
		{
			if((nStyle & WS_EX_LAYERED) == WS_EX_LAYERED)
			{
				::SetWindowLong(hWnd, GWL_EXSTYLE, nStyle & ~WS_EX_LAYERED);
				::RedrawWindow(hWnd, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
			}

			m_viewMonitor.RemoveBackground();
		}

		AfxGetApp()->EndWaitCursor();
	}

	return 0;
}

LONG CGisView::OnAlphaTransparency(UINT WPARAM, LONG LPARAM)
{
	if(m_viewMonitor.m_pBackground == nullptr)
		return 0L;

	switch(WPARAM)
	{
		case 1:
			break;
		case 2:
			m_viewMonitor.m_pBackground->Alphait(LPARAM);
			break;
	}

	return 1L;
}

void CGisView::GotMessageFromPipe(CStringA strCommand, CStringA strArgument)
{
	m_viewMonitor.OnFSCommond(this, strCommand, strArgument);
}

UINT CGisView::ThreadSetBackground(LPVOID pParam)
{
	CGisView* pView=(CGisView*)pParam;
	if(pView == nullptr)
		return 0;

	if(WaitForSingleObject(pView->m_hDrawOver, 0.5*60*1000) == WAIT_OBJECT_0)
	{
		static CStringA strProvider;
		static CStringA strVariety;
		strProvider=pView->m_document.m_Datainfo.m_BackInfo.m_strProvider;
		strVariety=pView->m_document.m_Datainfo.m_BackInfo.m_strVariety;

		CMainFrame* pMainFrame=(CMainFrame*)AfxGetApp()->m_pMainWnd;
		if(pMainFrame != nullptr)
		{
			pMainFrame->m_LayerPane.m_pBackgroundBar->Enable(strProvider, strVariety);
		}
		pView->PostMessage(WM_CHANGEBACKGROUND, (UINT)&strProvider, (UINT)&strVariety);
	}

	return 0;
}

LONG CGisView::OnSetRoute(UINT WPARAM, LONG LPARAM)
{
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetApp()->m_pMainWnd;
	const CPointF geoPoint=*(CPointF*)LPARAM;;

	CRoute* pRoute=&(pMainFrame->m_Route);
	if(pRoute == nullptr)
		return 0;

	switch(WPARAM)
	{
		case 1:
		{
			pRoute->SetStart(this, m_viewMonitor, geoPoint);
		}
		break;
		case 2:
		{
			pRoute->SetEnd(this, m_viewMonitor, geoPoint);
			float length=pRoute->GetShortcutLength();
			if(length == -1.0f)
				length=pRoute->GetBeelineLength();

			pMainFrame->ActivePane('O');
			pMainFrame->m_NavinfoPane.m_pNavigateDlg->SetRouteLength(length);
		}
		break;
		case 3:
		{
			pRoute->SetPass(this, m_viewMonitor, geoPoint);
			float length=pRoute->GetShortcutLength();
			if(length == -1.0f)
				length=pRoute->GetBeelineLength();

			pMainFrame->ActivePane('O');
			pMainFrame->m_NavinfoPane.m_pNavigateDlg->SetRouteLength(length);
		}
		break;
		case 4:
		{
			pMainFrame->SendMessage(WM_SEARCHROUTE, 0, 0);
		}
		break;
		case 5:
		{
			pRoute->Over(this);
			pMainFrame->ActivePane('O');
			pMainFrame->m_NavinfoPane.m_pNavigateDlg->SetRouteLength(0.0f);
		}
		break;
	}
	return 0;
}
