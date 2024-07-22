#include "stdafx.h"

#include "Global.h"

#include "GisView.h"
#include "GisDoc.h"
#include "MainFrm.h"
#include "ChildFrm.h"

#include "../Atlas/Atlas1.h"

#include "../Tool/Global.h"
#include "../Tool/PanTool.h"
#include "../Tool/SpinTool.h"

#include "../Style/Spot.h"
#include "../Layer/Global.h"
#include "../Layer/LayerPane.h"
#include "../Layer/BackgroundBar.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Global.h"

#include "../Information/Global.h"
#include "../Information/TDBGridCtrl.h"
#include "../Information/POUTDBGridCtrl.h"
#include "../Information/POU.h"
#include "../Information/POUPane.h"
#include "../Information/POUListCtrl.h"
#include "../Information/ATTTDBGridCtrl.h"
#include "../Information/TablePane.h"

#include "../Hyperlink/Link.h"

#include "../Public/AttributeCtrl.h"
#include "../Public/PropertyPane.h"
#include "../Public/Function.h"
#include "../Public/LicenceDlg.h"

#include "../Dataview/Datainfo.h"
#include "../Dataview/Background.h"

#include "../Projection/Projection1.h"
#include "../Framework/gfx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern __declspec(dllimport) CPanTool panTool;
extern __declspec(dllimport) CSpinTool spinTool;

//HHOOK CGisView::g_mousehook = nullptr;
//HHOOK CGisView::g_procrethook = nullptr;

IMPLEMENT_GWDYNCREATE(CGisView, CGrfView)

BEGIN_MESSAGE_MAP(CGisView, CGrfView)
	//{{AFX_MSG_MAP(CGisView)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()	
	ON_COMMAND(ID_GEOGRAPH_SPHERE, OnFrameSphere)
	ON_COMMAND(ID_GEOGRAPH_GRATCULE, OnFrameGraticule)
	ON_COMMAND(ID_PREVIEW_PROJECTION, OnPreviewPeojectionChanged)
	ON_COMMAND(ID_PREVIEW_CONFIG, OnPreviewConfig)
	ON_COMMAND(ID_PREVIEW_CENTERLNG, OnPreviewCenterChanged)
	ON_COMMAND(ID_PREVIEW_CENTERLAT, OnPreviewCenterChanged)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_MESSAGE(WM_LAYERACTIVATED, OnLayerActivated)
	ON_MESSAGE(WM_POUACTIVATED, OnPOUActivated)
	ON_MESSAGE(WM_GEOMACTIVATED, OnGeomActivated)

	ON_MESSAGE(WM_POSITIONPOU, OnPositionPOU)
	ON_MESSAGE(WM_INJECTATT, OnInjectATT)

	ON_MESSAGE(WM_HIGHLIGHTATT, OnHighlightATT)
	ON_MESSAGE(WM_SHOWRECINFO, OnShowRecInfo)
	ON_MESSAGE(WM_PREDELETEGEOM, OnPreDeleteGeom)
	ON_MESSAGE(WM_CHANGEBACKGROUND, OnChangeBackground)
	ON_MESSAGE(WM_ALPHATRANSPARENCY, OnAlphaTransparency)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CGisView, CGrfView)
END_EVENTSINK_MAP()

CGisView::CGisView(CObject& parameter) noexcept
	: CGrfView(parameter), m_document((CGisDoc&)parameter)
{
	m_pAidView = nullptr;
}

CGisView::~CGisView()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if(pMainFrame->GetMDITabs().GetTabsNum()==0)
	{
		const LONG nStyle = ::GetWindowLong(pMainFrame->m_hWnd, GWL_EXSTYLE);
		if((nStyle&WS_EX_LAYERED)==WS_EX_LAYERED)
		{
			::SetWindowLong(pMainFrame->m_hWnd, GWL_EXSTYLE, nStyle&~WS_EX_LAYERED);
			::RedrawWindow(pMainFrame->m_hWnd, nullptr, nullptr, RDW_ERASE|RDW_INVALIDATE|RDW_FRAME|RDW_ALLCHILDREN);
		}
		pMainFrame->m_LayerPane.EnableBackgroundBar(FALSE);
	}
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
#endif //_DEBUG

void CGisView::OnInitialUpdate()
{
	CGrfView::OnInitialUpdate();

	if(m_document.m_Datainfo.m_pProjection!=nullptr)
	{
		static CString strProvider;
		static CString strVariety;
		strProvider = m_document.m_Datainfo.m_BackInfo.m_strProvider;
		strVariety = m_document.m_Datainfo.m_BackInfo.m_strVariety;
		if(strProvider!=_T(""))
		{
			CWinThread* pThread = AfxBeginThread(ThreadSetBackground, (LPVOID)(this), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, nullptr);
			if(pThread!=nullptr)
			{
				ResetEvent(m_hDrawOver);
				pThread->m_pMainWnd = AfxGetMainWnd();
				pThread->ResumeThread();
			}
		}
	}
	//if(m_pAidView != nullptr)
	//{
	//	if(m_pAidView->GetTab(RUNTIME_CLASS(CTDBGridCtrl)) == nullptr)
	//		m_pAidView->AddTab(RUNTIME_CLASS(CTDBGridCtrl),_T("Database"),nullptr);
	//}
}

BOOL CGisView::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CGrfView::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nId)
			{
				case ID_PREVIEW_CENTERLNG:
				case ID_PREVIEW_CENTERLAT:
					{
						pCmdUI->Enable(m_viewMonitor.m_pGeocentric!=nullptr);
					}
					return true;
				case ID_PREVIEW_CONFIG:
					{
						pCmdUI->Enable(m_viewMonitor.m_pGeocentric!=nullptr);						
					}
					return true;
				case ID_GEOGRAPH_FRAME:
					{
						pCmdUI->Enable(m_document.m_Datainfo.m_pProjection!=nullptr);
					}
					return true;
				case ID_GEOGRAPH_SPHERE:
					{
						pCmdUI->Enable(m_document.m_Datainfo.m_pProjection!=nullptr);
						pCmdUI->SetCheck(m_document.m_frame.m_bEnabledSphere);
					}
					return true;
				case ID_GEOGRAPH_GRATCULE:
					{
						pCmdUI->Enable(m_document.m_Datainfo.m_pProjection!=nullptr);
						pCmdUI->SetCheck(m_document.m_frame.m_bEnabledGraticule);
					}
					return true;
				case ID_PREVIEW_SPIN:
					pCmdUI->Enable(m_document.m_Datainfo.m_pProjection!=nullptr&&m_document.m_Datainfo.m_pProjection->IsLngLat());
					return true;
				case ID_FILE_RELEASE:
					{
						pCmdUI->Enable(TRUE);
					}
					return true;
				default:
					break;
			}
			pCmdUI->m_bContinueRouting = false;
		}
		else if(nCode==CN_COMMAND)
		{
		}
	}

	return CGrfView::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}
void CGisView::OnFrameSphere()
{
	m_document.m_frame.m_bEnabledSphere = !m_document.m_frame.m_bEnabledSphere;
	this->Invalidate();
}
void CGisView::OnFrameGraticule()
{
	m_document.m_frame.m_bEnabledGraticule = !m_document.m_frame.m_bEnabledGraticule;
	this->Invalidate();
}
void CGisView::OnPreviewPeojectionChanged()
{
	CPointF geoCenter = m_viewMonitor.GetViewCenter(this);
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CMFCRibbonComboBox* pProjectionCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pMainFrame->GetRibbonBar().FindByID(ID_PREVIEW_PROJECTION));
	int selected = pProjectionCombo->GetCurSel();
	int data = pProjectionCombo->GetItemData(selected);
	m_viewMonitor.ChangeProjection(data);

	m_viewMonitor.Refresh(this, geoCenter);
	CGisView::RefreshPreviewCenter();
	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it != m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		layer->Purify();
	}
}
void CGisView::OnPreviewConfig()
{
	if(m_viewMonitor.m_pBackground == nullptr)
	{
		CPointF geoCenter = m_viewMonitor.GetViewCenter(this);
		m_viewMonitor.ConfigGeocentric();

		m_viewMonitor.Refresh(this, geoCenter);
		CGisView::RefreshPreviewCenter();
	}
	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it != m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		layer->Purify();
	}
}
void CGisView::OnPreviewCenterChanged()
{
	CPointF geoCenter = m_viewMonitor.GetViewCenter(this);

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CMFCRibbonEdit* pCenterLngEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pMainFrame->GetRibbonBar().FindByID(ID_PREVIEW_CENTERLNG));
	CMFCRibbonEdit* pCenterLatEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pMainFrame->GetRibbonBar().FindByID(ID_PREVIEW_CENTERLAT));
	CString strLng = pCenterLngEdit->GetEditText();
	CString strLat = pCenterLatEdit->GetEditText();
	std::optional<double> lng = strLng.IsEmpty() ? (std::optional<double>)std::nullopt : _tstof(strLng);
	std::optional<double> lat = strLat.IsEmpty() ? (std::optional<double>)std::nullopt : _tstof(strLat);
	m_viewMonitor.Recenter(lng, lat);
	m_viewMonitor.Refresh(this, geoCenter);
	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it != m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		layer->Purify();
	}
}
void CGisView::RefreshPreviewCenter() const
{
	CPointF center = m_viewMonitor.m_clipper->GetCenter();

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CMFCRibbonEdit* pCenterLngEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pMainFrame->GetRibbonBar().FindByID(ID_PREVIEW_CENTERLNG));
	CMFCRibbonEdit* pCenterLatEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pMainFrame->GetRibbonBar().FindByID(ID_PREVIEW_CENTERLAT));
	if(pCenterLngEdit!=nullptr)
	{		
		CString strLng;
		strLng.Format(_T("%.8f"), center.x);
		pCenterLngEdit->SetEditText(strLng);
	}
	if(pCenterLatEdit!=nullptr)
	{
		CString strLat;
		strLat.Format(_T("%.8f"), center.y);
		pCenterLatEdit->SetEditText(strLat);
	}
}
LONG CGisView::OnPreDeleteGeom(UINT WPARAM, LONG LPARAM)
{
	CLayer* pLayer = (CLayer*)WPARAM;
	if(pLayer==nullptr)
		return 0;

	CGeom* pGeom = (CGeom*)LPARAM;
	if(pGeom==nullptr)
		return 0;

	for(std::list<CLink*>::iterator it = m_document.m_linklist.begin(); it!=m_document.m_linklist.end(); ++it)
	{
		const CLink* link = *it;
		if(link->m_dwGeomId==pGeom->m_geoId)
		{
			delete link;
			link = nullptr;
			m_document.m_linklist.erase(it);
		}
	}

	return CGrfView::OnPreDeleteGeom(WPARAM, LPARAM);
}

void CGisView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CGrfView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	if(bActivate == TRUE && pActivateView == this)
	{
		panTool.m_bContinuously = false;

		CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
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
		CMFCRibbonComboBox* pProjectionCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pMainFrame->GetRibbonBar().FindByID(ID_PREVIEW_PROJECTION));
		CMFCRibbonEdit* pCenterLngEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pMainFrame->GetRibbonBar().FindByID(ID_PREVIEW_CENTERLNG));
		CMFCRibbonEdit* pCenterLatEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pMainFrame->GetRibbonBar().FindByID(ID_PREVIEW_CENTERLAT));
		if(pProjectionCombo==nullptr)
		{
		}
		else if(m_viewMonitor.m_pGeocentric!=nullptr) 
		{		
			CString name = m_viewMonitor.m_pGeocentric->GetName();
			pProjectionCombo->SelectItem(name);
			RefreshPreviewCenter();
		}
		else if(m_viewMonitor.m_datainfo.IsLngLat())
		{
			pProjectionCombo->SelectItem(0);
			RefreshPreviewCenter();
		}
		else
		{
			pProjectionCombo->SelectItem(0);
			pCenterLngEdit->SetEditText(_T(""));
			pCenterLatEdit->SetEditText(_T(""));
		}
		CPOUPane* pPOUPane = (CPOUPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'P', 0);
		if(pPOUPane != nullptr)
		{
			pPOUPane->SetMapCtrl(&m_document.m_POUListCtrl);
		}

		bool bHasPOUDatabase = false;
		bool bHasAATDatabase = false;
		if(m_document.m_poulist.m_DBList.size() > 0)
			bHasPOUDatabase = true;
		if(m_document.m_layertree.m_AttDBList.size() > 0)
			bHasAATDatabase = true;
		if(pMainFrame->m_pAtlas != nullptr && pMainFrame->m_pAtlas->m_poulist.m_DBList.size() > 0)
			bHasPOUDatabase = true;

		CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
		if(pTablePane != nullptr)
		{
			if(bHasPOUDatabase == true && bHasAATDatabase == false)
				pTablePane->ActivateTab('P');
			else if(bHasPOUDatabase == false && bHasAATDatabase == true)
				pTablePane->ActivateTab('A');
			else if(bHasPOUDatabase == true && bHasAATDatabase == true)
				pTablePane->ActivateTab('A');
		}

		CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
		if(pPropertyPane != nullptr)
		{
			if(bHasPOUDatabase == true || bHasAATDatabase == true)
				pPropertyPane->ValidateTab('A');
			else
				pPropertyPane->InvalidateTab('A');
		}

		this->SendMessage(WM_LAYERACTIVATED, 0, 0);
		this->SendMessage(WM_POUACTIVATED, 0, 0);
		/*if(CGisView::g_mousehook == nullptr && this->m_viewMonitor.m_pBackground != nullptr)
		{
		CGisView::g_mousehook = ::SetWindowsHookEx(WH_MOUSE_LL,CGisView::MouseProc,AfxGetApp()->m_hInstance,this->m_viewMonitor.m_pBackground->m_dwThreadId);
		}
		if(CGisView::g_procrethook == nullptr && this->m_viewMonitor.m_pBackground != nullptr)
		{
		CGisView::g_procrethook = ::SetWindowsHookEx(WH_CALLWNDPROCRET,CGisView::ProcretProc,AfxGetApp()->m_hInstance,this->m_viewMonitor.m_pBackground->m_dwThreadId);
		}*/
	}
	else if(bActivate==FALSE&&pDeactiveView==this)
	{
		/*if(CGisView::g_mousehook != nullptr)
		{
		::UnhookWindowsHookEx(CGisView::g_mousehook);
		CGisView::g_mousehook = nullptr;
		}
		if(CGisView::g_procrethook != nullptr)
		{
		::UnhookWindowsHookEx(CGisView::g_procrethook);
		CGisView::g_procrethook = nullptr;
		}*/
	}
}

LONG CGisView::OnPositionPOU(UINT WPARAM, LONG LPARAM)
{
	CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
	if(pTablePane!=nullptr)
	{
		if(pTablePane->m_pPOUTDBGridCtrl!=nullptr&&pTablePane->m_pPOUTDBGridCtrl->IsWindowVisible()==TRUE)
		{
			const CPoint docPoint(WPARAM, LPARAM);
			if(m_document.m_Datainfo.m_pProjection!=nullptr)
			{
				const CPointF geoPoint = m_document.m_Datainfo.DocToWGS84(docPoint);
				const DWORD dwId = pTablePane->m_pPOUTDBGridCtrl->Position(this, geoPoint.x, geoPoint.y);
				if(dwId!=-1)
				{
					for(std::list<CPOU*>::reverse_iterator it = m_document.m_poulist.m_pous.rbegin(); it!=m_document.m_poulist.m_pous.rend(); ++it)
					{
						CPOU* pPOU = *it;
						pPOU->UpdatePosition(dwId, geoPoint);
					}

					this->Invalidate();
				}
			}
		}
	}

	return 1;
}

LONG CGisView::OnInjectATT(UINT WPARAM, LONG LPARAM)
{
	CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
	if(pTablePane==nullptr)
		return 0;
	if(pTablePane->m_pATTTDBGridCtrl==nullptr)
		return 0;
	if(pTablePane->m_pATTTDBGridCtrl->m_pHeaderProfile==nullptr)
		return 0;
	else if(pTablePane->m_pATTTDBGridCtrl->IsWindowVisible()==TRUE)
	{
		const DWORD attId = pTablePane->m_pATTTDBGridCtrl->ActiveId();
		if(attId!=0XFFFFFFFF)
		{
			CPoint point(WPARAM, LPARAM);
			CGeom* pGeom = (CGeom*)SendMessage(WM_LOOKUPGEOM, (LONG)&point, 1);
			if(pGeom==nullptr)
				return 0;
			CLayer* layer = m_document.m_layertree.GetLayerByGeom(pGeom);
			if(layer==nullptr)
				return 0L;
			CATTDatasource* pDatasource = layer->GetAttDatasource();
			if(pDatasource==nullptr)
				return 0L;
			CDatabase* pDatabase = m_document.m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
			if(pDatabase==nullptr)
				return 0L;
			if(pDatabase->IsOpen()==FALSE)
				return 0L;
			const CHeaderProfile& headerprofile = pDatasource->GetHeaderProfile();
			if(&headerprofile==pTablePane->m_pATTTDBGridCtrl->m_pHeaderProfile)
			{
				pGeom->m_attId = attId;
				OnHighlightATT((UINT)&headerprofile.m_strKeyTable, attId);
			}
			else
			{
				AfxMessageBox(IDS_INCONSISTENTTABLE);
				return 0;
			}
		}
	}
	return 1;
}

LONG CGisView::OnGeomActivated(UINT WPARAM, LONG LPARAM)
{
	CLayer* pLayer = (CLayer*)WPARAM;
	CGeom* pGeom = (CGeom*)LPARAM;
	if(pLayer==nullptr||pGeom==nullptr)
		return 0;
	if(pGeom->m_attId==0XFFFFFFFF)
		return 0;
	CATTDatasource* pDatasource = pLayer->GetAttDatasource();
	if(pDatasource==nullptr)
		return 0L;
	CDatabase* pDatabase = m_document.m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
	if(pDatabase==nullptr)
		return 0L;
	if(pDatabase->IsOpen()==FALSE)
		return 0L;

	CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
	if(pTablePane!=nullptr&&pTablePane->IsVisible()==TRUE)
	{
		if(pTablePane->m_pATTTDBGridCtrl!=nullptr&&pTablePane->m_pATTTDBGridCtrl->IsWindowVisible()==TRUE)
		{
			CATTHeaderProfile& headerprofile = pDatasource->GetHeaderProfile();
			pTablePane->m_pATTTDBGridCtrl->Fill(pDatabase, &headerprofile, false);
			pTablePane->m_pATTTDBGridCtrl->ActivateRec(headerprofile.m_strIdField, pGeom->m_attId, FALSE);
		}
	}
	CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
	if(pPropertyPane!=nullptr&&pPropertyPane->IsVisible()==TRUE)
	{
		CAttributeCtrl* pAttributeCtrl = (CAttributeCtrl*)pPropertyPane->GetTabWnd('A');
		if(pAttributeCtrl!=nullptr)
		{
			const CHeaderProfile& headerprofile = pDatasource->GetHeaderProfile();
			pAttributeCtrl->OnTableChanged(pDatabase, headerprofile.m_strKeyTable);
			pAttributeCtrl->ShowInfo(headerprofile.m_strIdField, pGeom->m_attId);
		}
	}
}

LONG CGisView::OnShowRecInfo(UINT WPARAM, LONG LPARAM)
{
	if(m_document.m_poulist.m_DBList.size()>0)
	{
		CPoint docPoint = CPoint(WPARAM, LPARAM);
		for(std::list<CPOU*>::reverse_iterator it = m_document.m_poulist.m_pous.rbegin(); it!=m_document.m_poulist.m_pous.rend(); ++it)
		{
			CPOU* pPOU = *it;
			if(pPOU->m_bVisible==false)
				continue;
			const DWORD dwId = pPOU->Pick(this, docPoint, m_viewMonitor);
			if(dwId!=-1)
			{
				CODBCDatabase* pDatabase = m_document.m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);

				CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
				if(pTablePane!=nullptr&&pTablePane->IsWindowVisible()==TRUE)
				{
					pTablePane->ActivateTab('P');
					if(pTablePane->m_pPOUTDBGridCtrl!=nullptr&&pTablePane->m_pPOUTDBGridCtrl->IsWindowVisible()==TRUE)
					{
						CPOUHeaderProfile& headerprofile = pPOU->m_datasource.GetHeaderProfile();
						pTablePane->m_pPOUTDBGridCtrl->Fill(pDatabase, &headerprofile, FALSE);
						pTablePane->m_pPOUTDBGridCtrl->ActivateRec(headerprofile.m_strIdField, dwId, TRUE);
					}
				}

				CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
				if(pPropertyPane!=nullptr)
				{
					pPropertyPane->ActivateTab('A');

					CAttributeCtrl* pAttributeCtrl = (CAttributeCtrl*)pPropertyPane->GetTabWnd('A');
					if(pAttributeCtrl!=nullptr&&pAttributeCtrl->IsWindowVisible()==TRUE)
					{
						const CHeaderProfile& headerprofile = pPOU->m_datasource.GetHeaderProfile();
						pAttributeCtrl->OnTableChanged(pDatabase, headerprofile.m_strKeyTable);
						pAttributeCtrl->ShowInfo(headerprofile.m_strIdField, dwId);
					}
				}

				CGrfView::UnhighligtAll();
				return 1;
			}
		}
	}

	const CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if(pMainFrame->m_pAtlas!=nullptr)
	{
		CPoint docPoint = CPoint(WPARAM, LPARAM);
		for(std::list<CPOU*>::reverse_iterator it = pMainFrame->m_pAtlas->m_poulist.m_pous.rbegin(); it!=pMainFrame->m_pAtlas->m_poulist.m_pous.rend(); ++it)
		{
			CPOU* pPOU = *it;
			if(pPOU->m_bVisible==false)
				continue;
			const DWORD dwId = pPOU->Pick(this, docPoint, m_viewMonitor);
			if(dwId!=-1)
			{
				CODBCDatabase* pDatabase = m_document.m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);

				CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
				if(pTablePane!=nullptr&&pTablePane->IsWindowVisible()==TRUE&&pTablePane->m_pPOUTDBGridCtrl!=nullptr)
				{
					CPOUHeaderProfile& headerprofile = pPOU->m_datasource.GetHeaderProfile();
					pTablePane->ActivateTab('P');
					pTablePane->m_pPOUTDBGridCtrl->Fill(pDatabase, &headerprofile, FALSE);
					pTablePane->m_pPOUTDBGridCtrl->ActivateRec(headerprofile.m_strIdField, dwId, TRUE);
				}

				CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
				if(pPropertyPane!=nullptr&&pPropertyPane->IsWindowVisible()==TRUE)
				{
					pPropertyPane->ActivateTab('A');

					CAttributeCtrl* pAttributeCtrl = (CAttributeCtrl*)pPropertyPane->GetTabWnd('A');
					if(pAttributeCtrl!=nullptr&&pAttributeCtrl->IsWindowVisible()==TRUE)
					{
						const CHeaderProfile& headerprofile = pPOU->m_datasource.GetHeaderProfile();
						pAttributeCtrl->OnTableChanged(pDatabase, headerprofile.m_strKeyTable);
						pAttributeCtrl->ShowInfo(headerprofile.m_strIdField, dwId);
					}
				}

				CGrfView::UnhighligtAll();
				return 1;
			}
		}
	}

	if(m_document.m_layertree.m_AttDBList.empty()==false)
	{
		const CPoint point(WPARAM, LPARAM);
		for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
		{
			CLayer* layer = *it;
			CATTDatasource* pDatasource = layer->GetAttDatasource();
			if(pDatasource==nullptr)
				continue;
			CDatabase* pDatabase = m_document.m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
			if(pDatabase==nullptr)
				continue;
			if(pDatabase->IsOpen()==FALSE)
				continue;

			CGeom* pGeom = layer->Pick(this, m_viewMonitor, point, true);
			if(pGeom==nullptr)
				continue;
			if(pGeom->m_attId!=0XFFFFFFFF)
			{
				CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
				if(pTablePane!=nullptr&&pTablePane->IsWindowVisible()==TRUE&&pTablePane->m_pATTTDBGridCtrl!=nullptr)
				{
					CATTHeaderProfile& headerprofile = pDatasource->GetHeaderProfile();
					pTablePane->ActivateTab('A');
					pTablePane->m_pATTTDBGridCtrl->Fill(pDatabase, &headerprofile, FALSE);
					pTablePane->m_pATTTDBGridCtrl->ActivateRec(headerprofile.m_strIdField, pGeom->m_attId, FALSE);
				}

				CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
				if(pPropertyPane!=nullptr&&pPropertyPane->IsWindowVisible()==TRUE)
				{
					pPropertyPane->ActivateTab('A');
					CAttributeCtrl* pAttributeCtrl = (CAttributeCtrl*)pPropertyPane->GetTabWnd('A');
					if(pAttributeCtrl!=nullptr&&pAttributeCtrl->IsWindowVisible()==TRUE)
					{
						const CHeaderProfile& headerprofile = pDatasource->GetHeaderProfile();
						pAttributeCtrl->OnTableChanged(pDatabase, headerprofile.m_strKeyTable);
						pAttributeCtrl->ShowInfo(headerprofile.m_strIdField, pGeom->m_attId);
					}
				}
				if(m_hlGeomList.Find(pGeom)==nullptr)
				{
					CGrfView::UnhighligtAll();
					layer->Invalidate(this, m_viewMonitor, pGeom);
					m_hlGeomList.AddTail(pGeom);
				}
				return 1;
			}
		}

		CGrfView::UnhighligtAll();
	}

	return 1;
}

LONG CGisView::OnLayerActivated(UINT WPARAM, LONG LPARAM)
{
	CGrfView::OnLayerActivated(WPARAM, LPARAM);

	const CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CTablePane* pTablePane = (CTablePane*)pMainFrame->SendMessage(WM_GETPANE, 'O', 0);
	if(pTablePane!=nullptr&&pTablePane->m_hWnd!=nullptr&&pTablePane->IsVisible()==TRUE&&pTablePane->m_pATTTDBGridCtrl!=nullptr&&pTablePane->m_pATTTDBGridCtrl->m_hWnd!=nullptr)
	{
		CLayer* layer = (CLayer*)WPARAM;
		if(layer==nullptr)
			layer = GetActiveLayer();
		if(layer==nullptr)
		{
			pTablePane->m_pATTTDBGridCtrl->ClearGrid();
			return 0;
		}

		CATTDatasource* pDatasource = layer->GetAttDatasource();
		if(pDatasource==nullptr)
		{
			pTablePane->m_pATTTDBGridCtrl->ClearGrid();
			return 0;
		}

		CDatabase* pDatabase = m_document.m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
		if(pDatabase==nullptr)
		{
			pTablePane->m_pATTTDBGridCtrl->ClearGrid();
			return 0;
		}
		if(pDatabase->IsOpen()==FALSE)
		{
			pTablePane->m_pATTTDBGridCtrl->ClearGrid();
			return 0;
		}
		else
		{
			CHeaderProfile& headerprofile = pDatasource->GetHeaderProfile();
			pTablePane->m_pATTTDBGridCtrl->Fill(pDatabase, &headerprofile, TRUE);
			return 1;
		}
	}
	else
		return 0;
}

LONG CGisView::OnPOUActivated(UINT WPARAM, LONG LPARAM)
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CTablePane* pTablePane = (CTablePane*)pMainFrame->SendMessage(WM_GETPANE, 'O', 0);
	if(pTablePane==nullptr||pTablePane->m_pPOUTDBGridCtrl==nullptr)
		return 0;

	CPOUListCtrl* pPOUListCtrl = (CPOUListCtrl*)WPARAM;
	CPOU* pPOU = (CPOU*)LPARAM;
	if(pPOUListCtrl!=nullptr&&pPOU!=nullptr)
	{
		CODBCDatabase* pDatabase = m_document.m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);
		CHeaderProfile& headerprofile = pPOU->m_datasource.GetHeaderProfile();
		pTablePane->m_pPOUTDBGridCtrl->Fill(pDatabase, &headerprofile, FALSE);
	}
	else
	{
		pPOU = m_document.m_POUListCtrl.GetActive();
		if(pPOU!=nullptr)
		{
			CODBCDatabase* pDatabase = m_document.m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);
			CHeaderProfile& headerprofile = pPOU->m_datasource.GetHeaderProfile();
			pTablePane->m_pPOUTDBGridCtrl->Fill(pDatabase, &headerprofile, FALSE);
		}
		else if(pMainFrame->m_pAtlas!=nullptr)
		{
			pPOU = pMainFrame->m_pAtlas->m_POUListCtrl.GetActive();
			if(pPOU!=nullptr)
			{
				CODBCDatabase* pDatabase = pMainFrame->m_pAtlas->m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);
				CHeaderProfile& headerprofile = pPOU->m_datasource.GetHeaderProfile();
				pTablePane->m_pPOUTDBGridCtrl->Fill(pDatabase, &headerprofile, FALSE);
			}
			else
				pTablePane->m_pPOUTDBGridCtrl->ClearGrid();
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
	const CString tableATT = *(CString*)WPARAM;
	DWORD dwKey = LPARAM;

	CTypedPtrList<CObList, CGeom*> list;

	CRect docRect;
	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		CATTDatasource* pDatasource = layer->GetAttDatasource();
		if(pDatasource==nullptr)
			continue;
		const CHeaderProfile& headerprofile = pDatasource->GetHeaderProfile();
		if(headerprofile.m_strKeyTable==tableATT)
		{
			CTypedPtrList<CObList, CGeom*>& geomlist = layer->GetGeomList();
			POSITION Pos3 = geomlist.GetHeadPosition();
			while(Pos3!=nullptr)
			{
				CGeom* pGeom = geomlist.GetNext(Pos3);
				if(pGeom->m_attId==LPARAM)
				{
					docRect = UnionRect(docRect, pGeom->m_Rect);
					m_hlGeomList.AddTail(pGeom);
					break;
				}
			}
		}
	}

	if(m_hlGeomList.GetCount()>0)
	{
		CPoint docPoint = docRect.CenterPoint();
		docPoint.x = docRect.left+docRect.Width()/2;// CenterPoint overflow
		docPoint.y = docRect.top+docRect.Height()/2;// CenterPoint overflow

		CRect client;
		GetClientRect(client);
		const CPoint cliPoint = client.CenterPoint();
		const Gdiplus::Rect cliRect = m_viewMonitor.DocToClient<Gdiplus::Rect>(docRect);
		CRect cliView;
		GetClientRect(cliView);
		if(cliRect.Width!=0&&cliRect.Height!=0&&(cliRect.Width>cliView.Width()||cliRect.Height>cliView.Height()))
		{
			const float xRatio = (float)cliView.Width()/cliRect.Width;
			const float yRatio = (float)cliView.Height()/cliRect.Height;
			const float fRatio = min(xRatio, yRatio);
			m_viewMonitor.ZoomBy(this, fRatio, cliPoint);

			CStatusBarEx* pStatusBar = this->GetStatusBar();
			if(pStatusBar!=nullptr)
			{
				pStatusBar->m_LevelBar.SetLevel(m_viewMonitor.m_levelCurrent);
				pStatusBar->m_ScaleBar.SetScale(m_viewMonitor.m_scaleCurrent);
			}

		}
		const CPointF mapPoint = m_viewMonitor.m_datainfo.DocToMap(docPoint);
		m_viewMonitor.FixateAt(this, mapPoint, cliPoint);
		CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
		pChildfrm->ResetRuler(m_viewMonitor);
	}
	return 0;
}

LONG CGisView::OnChangeBackground(UINT WPARAM, LONG LPARAM)
{
	const DWORD dwKey = AfxGetAuthorizationKey();
	if((dwKey&LICENCE::BACKGROUND)!=LICENCE::BACKGROUND)
	{
		CLicenceDlg::Display();
		return 0;
	}

	if(m_document.m_Datainfo.m_pProjection==nullptr)
		return 0;
	const CStringA strProvider = *(CStringA*)WPARAM;
	const CStringA strVariety = *(CStringA*)LPARAM;

	CRect cliRect;
	this->GetClientRect(cliRect);
	const CPoint cliPoint1 = cliRect.TopLeft();
	const CPoint cliPoint2 = cliRect.BottomRight();
	const CPointF geoPoint1 = m_viewMonitor.ClientToWGS84(cliPoint1);
	const CPointF geoPoint2 = m_viewMonitor.ClientToWGS84(cliPoint2);
	const CPointF geoPoint((geoPoint1.x+geoPoint2.x)/2, (geoPoint1.y+geoPoint2.y)/2);
	const float fScale = m_viewMonitor.m_scaleCurrent;
	const HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();
	const LONG nStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);

	if(strProvider==_T("")||strVariety==_T(""))
	{
		m_viewMonitor.RemoveBackground();

		m_viewMonitor.m_scaleCurrent = 0;
		m_viewMonitor.m_levelCurrent = 25;
		const CPointF mapPoint = m_viewMonitor.m_datainfo.m_pProjection==nullptr ? geoPoint : m_viewMonitor.m_datainfo.m_pProjection->GeoToMap(geoPoint);
		m_viewMonitor.ScaleTo(this, fScale, mapPoint);
		CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
		pChildfrm->ResetRuler(m_viewMonitor);

		if((nStyle&WS_EX_LAYERED)==WS_EX_LAYERED)
		{
			::SetWindowLong(hWnd, GWL_EXSTYLE, nStyle&~WS_EX_LAYERED);
			::RedrawWindow(hWnd, nullptr, nullptr, RDW_ERASE|RDW_INVALIDATE|RDW_FRAME|RDW_ALLCHILDREN);
		}
	}
	else if(m_viewMonitor.m_pBackground==nullptr)
	{
		CRect rect = cliRect;
		this->ClientToScreen(rect);

		CBackground* pBackground = new CBackground(m_viewMonitor);
		if(pBackground->Create(this, &StaticCallback, rect)==TRUE)
		{
			AfxGetApp()->BeginWaitCursor();
			const HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();
			pBackground->SetWindowPos(hWnd, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW|SWP_NOACTIVATE);
			if(pBackground->OpenBack(this->m_hWnd, strProvider, strVariety)==TRUE)
			{
				if((nStyle&WS_EX_LAYERED)!=WS_EX_LAYERED)
				{
					CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
					int nAlpha = pMainFrame->m_LayerPane.m_pBackgroundBar->GetFroegroundTransparency();
					
					::SetWindowLong(hWnd, GWL_EXSTYLE, nStyle|WS_EX_LAYERED);
					::SetLayeredWindowAttributes(hWnd, RGB(255, 255, 254), nAlpha, LWA_COLORKEY|LWA_ALPHA);
				}
				m_viewMonitor.SetBackground(pBackground);
				pBackground->ScaleTo(fScale, geoPoint);
			}
			else
			{
				delete pBackground;
				pBackground = nullptr;
			}

			AfxGetApp()->EndWaitCursor();
		}
		else
		{
			delete pBackground;
			pBackground = nullptr;
		}
	}
	else if(m_viewMonitor.m_pBackground->m_strProvider!=strProvider||m_viewMonitor.m_pBackground->m_strVariety!=strVariety)
	{
		AfxGetApp()->BeginWaitCursor();
		if(m_viewMonitor.m_pBackground->OpenBack(this->m_hWnd, strProvider, strVariety)==FALSE)
		{
			if((nStyle&WS_EX_LAYERED)==WS_EX_LAYERED)
			{
				::SetWindowLong(hWnd, GWL_EXSTYLE, nStyle&~WS_EX_LAYERED);
				::RedrawWindow(hWnd, nullptr, nullptr, RDW_ERASE|RDW_INVALIDATE|RDW_FRAME|RDW_ALLCHILDREN);
			}
			m_viewMonitor.RemoveBackground();
		}

		AfxGetApp()->EndWaitCursor();
	}

	m_document.m_Datainfo.m_BackInfo.m_strProvider = strProvider;
	m_document.m_Datainfo.m_BackInfo.m_strVariety = strVariety;
	m_document.SetModifiedFlag(TRUE);
	return 0;
}

LONG CGisView::OnAlphaTransparency(UINT WPARAM, LONG LPARAM)
{
	if(m_viewMonitor.m_pBackground==nullptr)
		return 0L;

	switch(WPARAM)
	{
		case 1:
			break;
		case 2:
			m_viewMonitor.m_pBackground->Alphait(LPARAM);
			break;
		default:
			break;
	}

	return 1L;
}

void CGisView::GotMessageFromPipe(CStringA strCommand, CStringA strArgument)
{
	m_viewMonitor.OnFSCommond(this, strCommand, strArgument);

	CStatusBarEx* pStatusBar = this->GetStatusBar();
	if(pStatusBar!=nullptr)
	{
		//		pStatusBar->SetViewScale(m_viewMonitor.m_scaleCurrent);
	}
}

UINT CGisView::ThreadSetBackground(LPVOID pParam)
{
	CGisView* pView = (CGisView*)pParam;
	if(pView==nullptr)
		return 0;

	if(WaitForSingleObject(pView->m_hDrawOver, 0.5*60*1000)==WAIT_OBJECT_0)
	{
		static CStringA strProvider;
		static CStringA strVariety;
		strProvider = pView->m_document.m_Datainfo.m_BackInfo.m_strProvider;
		strVariety = pView->m_document.m_Datainfo.m_BackInfo.m_strVariety;

		CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pMainFrame->m_LayerPane.m_pBackgroundBar->Enable(strProvider, strVariety);
		pView->PostMessage(WM_CHANGEBACKGROUND, (UINT)&strProvider, (UINT)&strVariety);
	}

	return 0;
}
