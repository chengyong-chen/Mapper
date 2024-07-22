#include "stdafx.h"
#include "Viewer.h"
#include "MainFrm.h"
#include "Splash.h"

#include <afxdlgs.h>

#include "vfw.h"
#include <stdlib.h>
#include <AfxPriv.h>

#include "ChildFrm.h"
#include "GrfDoc.h"
#include "GisDoc.h"
#include "GrfView.h"
#include "GisView.h"
#include "HtmlView.h"
#include "Global.h"
#include "POIBar.h"
#include "RibbonBar.h"

#include "../Information/Global.h"
#include "../Information/QueryPane.h"
#include "../Information/TDBGridCtrl.h"
#include "../information/POUPane.h"
#include "../information/POUListCtrl.h"
#include "../information/TablePane.h"

#include "../Public/Global.h"

#include "../Atlas/Atlas1.h"
#include "../Atlas/Page.h"
#include "../Atlas/Resource.h"
#include "../Atlas/AtlasPane.h"

#include "../Mapper/Global.h"

#include "../Geometry/Global.h"
#include "../Style/Spot.h"
#include "../Layer/BackgroundBar.h"
#include "../Layer/LayerPane.h"
#include "../Dataview/Background.h"

#include "../Navigate/Global.h"
#include "../Navigate/Vehicle.h"
#include "../Navigate/MonitorEar.h"
#include "../Navigate/MessageEar.h"
#include "../Navigate/NavinfoDlg.h"
#include "../Navigate/Port.h"
#include "../Navigate/SetupSheet.h"
#include "../Navigate/Replay.h"
#include "../Navigate/ReplayTable.h"
#include "../Navigate/PortReceiver.h"
#include "../Navigate/PortSocket.h"
#include "../Navigate/VehicleTag.h"
#include "../Navigate/RoadTopo.h"

#include "../Tool/RouteTool.h"

#include <memory>
#include "../Coding/Instruction.h"
#include "../Atlas/Global.h"

CSecurity CMainFrame::m_Security;
CAtlas* CMainFrame::m_pAtlas = nullptr;

BOOL a_bGPSAuto;

extern __declspec(dllimport) CRouteTool routeTool;

typedef LPCSTR(*GETCODE)();
typedef LPCSTR(*GETPWD)(WORD, CString);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_COMMAND(ID_GPS_DEVICE, OnGpsDevice)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_COMMAND(ID_GPS_REPLAY, OnGpsReplay)
	ON_COMMAND(ID_GPS_TRACE, OnGpsTrack)
	ON_COMMAND(ID_PANE_ATLAS, OnPaneAtlas)
	ON_COMMAND(ID_PANE_LAYER, OnPaneLayer)
	ON_COMMAND(ID_PANE_POU, OnPanePOU)
	ON_COMMAND(ID_PANE_QUERY, OnPaneQuery)
	ON_COMMAND(ID_PANE_VEHICLE, OnPaneVehicle)
	ON_COMMAND(ID_PANE_NAVIGATE, OnPaneNavigate)
	ON_COMMAND(ID_PANE_TABLE, OnPaneTable)
	ON_UPDATE_COMMAND_UI(ID_PANE_ATLAS, OnUpdatePaneAtlas)
	ON_UPDATE_COMMAND_UI(ID_PANE_LAYER, OnUpdatePaneLayer)
	ON_UPDATE_COMMAND_UI(ID_PANE_POU, OnUpdatePanePOU)
	ON_UPDATE_COMMAND_UI(ID_PANE_QUERY, OnUpdatePaneQuery)
	ON_UPDATE_COMMAND_UI(ID_PANE_TABLE, OnUpdatePaneTable)
	ON_UPDATE_COMMAND_UI(ID_PANE_NAVIGATE, OnUpdatePaneNavinfo)
	ON_UPDATE_COMMAND_UI(ID_PANE_VEHICLE, OnUpdatePaneVehicle)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGING_ACTIVE_TAB, OnChangingActiveTab)

	ON_MESSAGE(WM_KEYQUERY, OnKeyQuery)
	ON_MESSAGE(WM_GETQUERYCTRL, OnGetQueryCtrl)
	ON_MESSAGE(WM_GETPANE, OnGetPane)

	ON_MESSAGE(WM_GETVEHICLE, OnGetVehicle)
	ON_MESSAGE(WM_VEHICLECONTROL, OnVehicleControl)
	ON_MESSAGE(WM_VEHICLEMOVE, OnVehicleMove)
	ON_MESSAGE(WM_VEHICLESTATUS, OnVehicleStatus)
	ON_MESSAGE(WM_VEHICLEICON, OnVehicleIcon)
	ON_MESSAGE(WM_VEHICLEMESSAGE, OnVehicleMessage)

	ON_MESSAGE(WM_ACTIVATEVEHICLE, OnActivateVehicle)
	ON_MESSAGE(WM_SETGEOSIGN, OnSetGeoSign)
	ON_MESSAGE(WM_SEARCHROUTE, OnSearchRoute)

	ON_MESSAGE(WM_CHILDDOCCHANGED, OnChildDocChanged)
	ON_MESSAGE(WM_GETPORT, OnGetPort)
	ON_MESSAGE(WM_VEHICLEDISPATCH, OnVehicleDispatch)

	ON_MESSAGE(WM_VIEWWILLDRAW, OnViewWillDraw)
	ON_MESSAGE(WM_VIEWDRAWOVER, OnViewDrawOver)

	ON_MESSAGE(WM_BROWSEHTML, OnBrowseHtml)
	ON_MESSAGE(WM_DESTROYHTML, OnDestroyHtml)
	ON_MESSAGE(WM_MATCHMAP, OnMatchMap)
	ON_MESSAGE(WM_GETMAINCHILD, OnGetMainChild)
	ON_MESSAGE(WM_URLCHANGED, OnURLChanged)
	ON_MESSAGE(WM_FLASHGEOM, OnFlashGeomtry)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CMainFrame, CMDIFrameWndEx)
	//{{AFX_EVENTSINK_MAP(CMainFrame)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

// CMainFrame construction/destruction
CMainFrame::CMainFrame()
	: m_LayerPane(FALSE), m_TablePane(false), m_POUPane(false)
{
	CSplashWnd::ShowSplashScreen(this);

	m_pHtmlView = nullptr;

	m_pPOIBar = nullptr;

	m_pPort = nullptr;
	m_pReplay = nullptr;

	m_geoSign = CPointF(0.0f, 0.0f);

	a_bGPSAuto = AfxGetApp()->GetProfileInt(_T("Receiver"), _T("Autoin"), 1);
	const WORD key1 = ((CViewerApp*)AfxGetApp())->GetProfileInt(_T(""), _T("WinBord"), 1);
	const WORD key2 = ((CViewerApp*)AfxGetApp())->GetProfileInt(_T(""), _T("WinCach"), 1);
	m_Security.m_sysKey1 ^= key1;
	m_Security.m_sysKey2 ^= key2;
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CMDIFrameWndEx::PreCreateWindow(cs))
		return FALSE;

	cs.x = 0;
	cs.y = 0;
	cs.cx = ::GetSystemMetrics(SM_CXSCREEN);
	cs.cy = ::GetSystemMetrics(SM_CYSCREEN);
	cs.style |= WS_POPUP|WS_VISIBLE|WS_CLIPSIBLINGS;// | WS_MAXIMIZE;
	cs.dwExStyle = WS_EX_LEFT|WS_EX_LTRREADING;//| WS_EX_RIGHTSCROLLBAR;
	cs.style &= ~FWS_ADDTOTITLE;

	if(m_pAtlas!=nullptr)
		cs.lpszName = m_pAtlas->m_strTitle;
	else
		cs.lpszName = _T("Diwatu Viewer");

	return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CMDIFrameWndEx::OnCreate(lpCreateStruct)==-1)
		return -1;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
	CDockingManager::SetDockingMode(DT_SMART);
	//	CDockablePane::SetCaptionStyle(TRUE,FALSE,TRUE);
	CMFCToolBarComboBoxButton::SetFlatMode();
	CMFCToolBarComboBoxButton::SetCenterVert();
	CMFCButton::EnableWindowsTheming();
	CMFCToolBar::EnableQuickCustomization(TRUE);
	CMDIFrameWndEx::RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_UPDATENOW|RDW_FRAME|RDW_ERASE);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
	mdiTabParams.m_bActiveTabCloseButton = TRUE; // set to FALSE to place close button at right of tab area
	mdiTabParams.m_bTabIcons = FALSE; // set to TRUE to enable document icons on MDI taba
	mdiTabParams.m_bAutoColor = TRUE; // set to FALSE to disable auto-coloring of MDI tabs
	mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
	mdiTabParams.m_bTabCustomTooltips = TRUE;
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	CDockingManager::SetDockingMode(DT_SMART);

	CDockablePane* leftTabbedBar = nullptr;
	if(m_pAtlas!=nullptr&&m_pAtlas->m_Catalog.m_PageList.GetCount()>2)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Atlas.dll"));
		AfxSetResourceHandle(hInst);

		CString strName;
		strName.LoadString(IDS_PANE_ATLAS);
		if(m_AtlasPane.Create(strName, this, CRect(0, 0, 200, 150), TRUE, ID_PANE_ATLAS, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_LEFT|CBRS_FLOAT_MULTI)==TRUE)
		{
			m_AtlasPane.m_CatalogTree.ModifyStyle(TVS_EDITLABELS, 0, 0);
			m_AtlasPane.m_CatalogTree.BuildTree(&(m_pAtlas->m_Catalog));

			m_AtlasPane.EnableDocking(CBRS_ALIGN_LEFT);
			EnableDocking(CBRS_ALIGN_LEFT);
			if(leftTabbedBar==nullptr)
			{
				DockPane(&m_AtlasPane);
				leftTabbedBar = &m_AtlasPane;
			}
			else
			{
				CDockablePane* pTabbedBar = nullptr;
				m_AtlasPane.AttachToTabWnd(leftTabbedBar, DM_SHOW, TRUE, &pTabbedBar);
				leftTabbedBar = pTabbedBar;
			}
			m_AtlasPane.ShowPane(TRUE, FALSE, FALSE);
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}

	if(m_QueryPane.m_hWnd==nullptr)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
		AfxSetResourceHandle(hInst);

		CString strName;
		strName.LoadString(IDS_PANE_QUERY);
		if(m_QueryPane.Create(strName, this, CRect(0, 0, 200, 150), TRUE, ID_PANE_QUERY, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_LEFT|CBRS_FLOAT_MULTI)==TRUE)
		{
			m_QueryPane.RemoveEar(_T("Coo"));
			if(m_pAtlas==nullptr)
			{
				m_QueryPane.RemoveEar(_T("Geometry"));
			}
			else
			{
				CMap<WORD, WORD&, CString, CString&> mapIdMap;
				POSITION posMap = m_pAtlas->m_Pyramid.m_DeckList.GetHeadPosition();
				while(posMap!=nullptr)
				{
					CDeck* pDeck = m_pAtlas->m_Pyramid.m_DeckList.GetNext(posMap);
					if(pDeck->m_bQueryble==FALSE)
						continue;

					CString strMap = pDeck->m_strName;
					strMap.Replace(_T("地图"), nullptr);
					mapIdMap.SetAt(pDeck->m_wId, strMap);
				}
				CMap<DWORD, DWORD&, CString, CString&> mapIdClass;
				m_QueryPane.m_KeyQueryEar.AddMapCombo(mapIdMap);
				m_QueryPane.m_GeomQueryEar.AddMapCombo(mapIdMap);

				mapIdMap.RemoveAll();
				mapIdClass.RemoveAll();

				if(m_Security.HasDatabaseRight()==false)
				{
					m_QueryPane.RemoveEar(_T("Geometry"));
				}
				//		else if(m_pAtlas->m_DatainfobaseATT.IsOpen() == TRUE)
				//		{
				//			pMainBar->m_QueryPane.RemovePage(&(pMainBar->m_QueryPane.m_KeyQueryEar));
				//		}
			}
			m_QueryPane.EnableDocking(CBRS_ALIGN_LEFT);
			EnableDocking(CBRS_ALIGN_LEFT);
			if(leftTabbedBar==nullptr)
			{
				DockPane(&m_QueryPane);
				leftTabbedBar = &m_QueryPane;
			}
			else
			{
				CDockablePane* pTabbedBar = nullptr;
				m_QueryPane.AttachToTabWnd(leftTabbedBar, DM_SHOW, TRUE, &pTabbedBar);
				leftTabbedBar = pTabbedBar;
			}
			m_QueryPane.ShowPane(TRUE, FALSE, FALSE);
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}

	if(m_LayerPane.m_hWnd==nullptr)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Layer.dll"));
		AfxSetResourceHandle(hInst);

		CString strName;
		strName.LoadString(IDS_PANE_LAYER);
		if(m_LayerPane.Create(strName, this, CRect(0, 0, 200, 150), TRUE, ID_PANE_LAYER, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_LEFT|CBRS_FLOAT_MULTI)==TRUE)
		{
			m_LayerPane.EnableDocking(CBRS_ALIGN_LEFT);
			EnableDocking(CBRS_ALIGN_LEFT);
			if(leftTabbedBar==nullptr)
			{
				DockPane(&m_LayerPane);
				leftTabbedBar = &m_LayerPane;
			}
			else
			{
				CDockablePane* pTabbedBar = nullptr;
				m_LayerPane.AttachToTabWnd(leftTabbedBar, DM_SHOW, TRUE, &pTabbedBar);
				leftTabbedBar = pTabbedBar;
			}

			m_LayerPane.ShowPane(TRUE, FALSE, FALSE);
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
	if(m_POUPane.m_hWnd==nullptr)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
		AfxSetResourceHandle(hInst);

		CString strName;
		strName.LoadString(IDS_PANE_POU);
		if(m_POUPane.Create(strName, this, CRect(0, 0, 200, 150), TRUE, ID_PANE_POU, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_LEFT|CBRS_FLOAT_MULTI)==TRUE)
		{
			if(m_pAtlas!=nullptr&&m_Security.HasDatabaseRight()==TRUE&&m_pAtlas->m_poulist.m_pous.size()>0)
			{
				if(m_pAtlas->m_POUListCtrl.Create(&m_POUPane, false)==TRUE)
				{
					m_pAtlas->m_POUListCtrl.Build();
					m_POUPane.SetAtlCtrl(&m_pAtlas->m_POUListCtrl);
				}
				else
				{
					m_POUPane.SetAtlCtrl(nullptr);
				}
			}
			else
			{
				m_POUPane.SetAtlCtrl(nullptr);
			}
			m_POUPane.EnableDocking(CBRS_ALIGN_LEFT);
			EnableDocking(CBRS_ALIGN_LEFT);
			if(leftTabbedBar==nullptr)
			{
				DockPane(&m_POUPane);
				leftTabbedBar = &m_POUPane;
			}
			else
			{
				CDockablePane* pTabbedBar = nullptr;
				m_POUPane.AttachToTabWnd(leftTabbedBar, DM_SHOW, TRUE, &pTabbedBar);
				leftTabbedBar = pTabbedBar;
			}
			m_POUPane.ShowPane(TRUE, FALSE, FALSE);
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}

	if(m_pAtlas!=nullptr&&m_Security.HasNavigateRight()==true)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
		AfxSetResourceHandle(hInst);

		CString strName;
		strName.LoadString(IDS_PANE_NAVIGATE);
		if(m_NavinfoPane.Create(strName, this, CRect(0, 0, 200, 150), TRUE, ID_PANE_NAVIGATE, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_LEFT|CBRS_FLOAT_MULTI)==TRUE)
		{
			m_NavinfoPane.EnableDocking(CBRS_ALIGN_LEFT);
			EnableDocking(CBRS_ALIGN_LEFT);
			if(leftTabbedBar==nullptr)
			{
				DockPane(&m_NavinfoPane);
				leftTabbedBar = &m_NavinfoPane;
			}
			else
			{
				CDockablePane* pTabbedBar = nullptr;
				m_NavinfoPane.AttachToTabWnd(leftTabbedBar, DM_SHOW, TRUE, &pTabbedBar);
				leftTabbedBar = pTabbedBar;
			}
			m_NavinfoPane.ShowPane(TRUE, FALSE, FALSE);
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}

	if(m_Security.HasTracarRight()==true)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
		AfxSetResourceHandle(hInst);

		CString strName;
		strName.LoadString(IDS_PANE_VEHICLE);
		if(m_VehiclePane.Create(strName, this, CRect(0, 0, 200, 150), TRUE, ID_PANE_VEHICLE, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_LEFT|CBRS_FLOAT_MULTI)==TRUE)
		{
			m_VehiclePane.EnableDocking(CBRS_ALIGN_LEFT);
			EnableDocking(CBRS_ALIGN_LEFT);
			if(leftTabbedBar==nullptr)
			{
				DockPane(&m_VehiclePane);
				leftTabbedBar = &m_VehiclePane;
			}
			else
			{
				CDockablePane* pTabbedBar = nullptr;
				m_VehiclePane.AttachToTabWnd(leftTabbedBar, DM_SHOW, TRUE, &pTabbedBar);
				leftTabbedBar = pTabbedBar;
			}
			m_VehiclePane.ShowPane(TRUE, FALSE, FALSE);
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}

	//	CEagleWnd* pEagleWnd = new CEagleWnd();
	//	if(pEagleWnd->Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE,CRect(0,0,0,0), pMainBar, 555) == TRUE)
	//	{
	//		pEagleWnd->ShowWindow(SW_SHOW);
	//		pMainBar->m_pEagleWnd = pEagleWnd;
	//	}

	if(m_Security.HasDatabaseRight()==true||m_Security.HasTracarRight()==true)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
		AfxSetResourceHandle(hInst);

		CString strName;
		strName.LoadString(IDS_PANE_TABLE);
		if(m_TablePane.Create(strName, this, CRect(0, 0, 185, 200), TRUE, ID_PANE_TABLE, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_BOTTOM|CBRS_FLOAT_MULTI)==TRUE)
		{
			m_TablePane.SetControlBarStyle(m_TablePane.GetControlBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC);
			if(m_Security.HasTracarRight()==true)
			{
				const HINSTANCE hInstOld = AfxGetResourceHandle();
				HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
				AfxSetResourceHandle(hInst);

				if(IsOCXRegistered("MSFLXGRD.OCX")==FALSE)
				{
					RegisterOCX("MSFLXGRD.OCX", Platform::X86, "");
				}

				if(IsOCXRegistered("MSFLXGRD.OCX")==TRUE)
				{
					CMonitorEar* pMonitorEar = new CMonitorEar(&(m_TablePane.m_wndTabs));
					if(pMonitorEar->Create(IDD_MONITOR, &(m_TablePane.m_wndTabs))==TRUE)
					{
						CString strLabel;
						strLabel.LoadString(IDS_GRID_MONITOR);

						pMonitorEar->ModifyStyle(WS_VISIBLE, 0, 0);
						m_TablePane.m_wndTabs.AddTab(pMonitorEar, strLabel, 4);
						m_TablePane.m_pMonitorCtrl = pMonitorEar;
					}
					else
					{
						delete pMonitorEar;
						pMonitorEar = nullptr;
					}
				}

				CMessageEar* pMessageEar = new CMessageEar();
				if(pMessageEar->Create(WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_AUTOHSCROLL|ES_MULTILINE|ES_LEFT, CRect(0, 0, 10, 10), &(m_TablePane.m_wndTabs), 0)==TRUE)
				{
					CString strLabel;
					strLabel.LoadString(IDS_GRID_MESSAGEWND);

					pMessageEar->ModifyStyle(WS_VISIBLE, 0, 0);
					m_TablePane.m_wndTabs.AddTab(pMessageEar, strLabel, 5);
					m_TablePane.m_pMessageCtrl = pMessageEar;
				}
				else
				{
					delete pMessageEar;
					pMessageEar = nullptr;
				}

				AfxSetResourceHandle(hInstOld);
				::FreeLibrary(hInst);
			}
			m_TablePane.EnableDocking(CBRS_ALIGN_BOTTOM);
			EnableDocking(CBRS_ALIGN_BOTTOM);
			DockPane(&m_TablePane);
			m_TablePane.ShowPane(TRUE, FALSE, FALSE);
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
	if(m_wndRibbonBar.Create(this)==FALSE)
		return FALSE;
	else if(CreateRibbonBar(m_wndRibbonBar, m_imagePanel)==FALSE)
		return FALSE;
	else
	{
		CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory(_T("File"), IDB_RIBBON_FILESMALL, IDB_RIBBON_FILELARGE);
		pMainPanel->Add(new CMFCRibbonButton(ID_FILE_OPEN, _T("&Open..."), 1, 1));
		pMainPanel->Add(new CMFCRibbonButton(ID_FILE_CLOSE, _T("&Close"), 5, 5));
		pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE, _T("&Save"), 2, 2));

		CMFCRibbonQuickAccessToolBarDefaultState qatState;
		qatState.AddCommand(ID_FILE_OPEN, TRUE);
		qatState.AddCommand(ID_FILE_CLOSE, TRUE);
		qatState.AddCommand(ID_FILE_SAVE, TRUE);
		m_wndRibbonBar.SetQuickAccessDefaultState(qatState);

		m_imagePanel.SetImageSize(CSize(16, 16));
		if(m_imagePanel.Load(IDB_BMPICONS)==TRUE)
		{
			m_wndRibbonBar.AddToTabs(new CMFCRibbonButton(ID_APP_ABOUT, _T("About\na"), m_imagePanel.ExtractIcon(0)));
		}

		if(m_pAtlas!=nullptr&&m_pAtlas->m_databasePOI.IsOpen()==TRUE)
		{
			m_wndRibbonBar.CreatePOIBar(&m_pAtlas->m_databasePOI);
		}
	}
	this->SetFocus();
	SendMessage(WM_SIZE, SIZE_MAXIMIZED, 0);
	CMDIFrameWndEx::SetForegroundWindow();
	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
	return 0;
}

void CMainFrame::OnMove(int x, int y)
{
	CMDIFrameWndEx::OnMove(x, y);

	CWnd* pMDIClientAreaWnd = this->GetWindow(GW_CHILD);
	while(pMDIClientAreaWnd!=nullptr)
	{
		const CRuntimeClass* pRuntimeClass = pMDIClientAreaWnd->GetRuntimeClass();
		if(strcmp(pRuntimeClass->m_lpszClassName, "CMDIClientAreaWnd")==0)
			break;

		pMDIClientAreaWnd = pMDIClientAreaWnd->GetNextWindow(GW_HWNDNEXT);
	}

	if(pMDIClientAreaWnd!=nullptr)
	{
		CWnd* pChildFrame = pMDIClientAreaWnd->GetWindow(GW_CHILD);
		while(pChildFrame!=nullptr)
		{
			if(pChildFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
			{
				CGrfView* pView = (CGrfView*)((CMDIChildWnd*)pChildFrame)->GetActiveView();
				if(pView!=nullptr&&pView->m_viewMonitor.m_pBackground!=nullptr)
				{
					CRect rect;
					pView->GetClientRect(rect);
					pView->ClientToScreen(rect);
					pView->m_viewMonitor.m_pBackground->SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOOWNERZORDER);
				}
			}
			pChildFrame = pChildFrame->GetWindow(GW_HWNDNEXT);
		}
	}
}

LRESULT CMainFrame::OnChangingActiveTab(WPARAM wp, LPARAM lp)
{
	CMFCBaseTabCtrl* pMFCBaseTabCtrl = (CMFCBaseTabCtrl*)lp;
	if(pMFCBaseTabCtrl==nullptr)
		return 0;
	if(pMFCBaseTabCtrl->GetParent()!=(CWnd*)&(this->m_wndClientArea))
		return 0;
	const int nActiveOld = pMFCBaseTabCtrl->GetActiveTab();
	CWnd* pViewOld = nullptr;
	if(nActiveOld!=-1)
	{
		CWnd* pChildFrame = pMFCBaseTabCtrl->GetTabWnd(nActiveOld);
		if(pChildFrame!=nullptr)
		{
			pViewOld = ((CMDIChildWndEx*)pChildFrame)->GetActiveView();
		}
	}
	const int nActiveNew = wp;
	CWnd* pViewNew = nullptr;
	if(nActiveNew!=-1)
	{
		CWnd* pChildFrame = pMFCBaseTabCtrl->GetTabWnd(nActiveNew);
		if(pChildFrame!=nullptr)
		{
			pViewNew = ((CMDIChildWndEx*)pChildFrame)->GetActiveView();
		}
	}

	CBackground* pNewBackground = (pViewNew!=nullptr&&pViewNew->IsKindOf(RUNTIME_CLASS(CGisView))==TRUE) ? ((CGisView*)pViewNew)->m_viewMonitor.m_pBackground : nullptr;
	CBackground* pOldBackground = (pViewOld!=nullptr&&pViewOld->IsKindOf(RUNTIME_CLASS(CGisView))==TRUE) ? ((CGisView*)pViewOld)->m_viewMonitor.m_pBackground : nullptr;
	if(pNewBackground!=nullptr&&pOldBackground!=nullptr)
	{
		pOldBackground->ShowWindow(SW_HIDE);
		pNewBackground->ShowWindow(SW_SHOW);
		pNewBackground->SetWindowPos(AfxGetMainWnd()->GetSafeHwnd(), 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}
	else if(pNewBackground!=nullptr)
	{
		const LONG nStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
		const int nAlpha = m_LayerPane.m_pBackgroundBar->GetFroegroundTransparency();
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, nStyle|WS_EX_LAYERED);
		::SetLayeredWindowAttributes(m_hWnd, RGB(255, 255, 254), nAlpha, LWA_COLORKEY|LWA_ALPHA);

		pNewBackground->ShowWindow(SW_SHOW);
		pNewBackground->SetWindowPos(AfxGetMainWnd()->GetSafeHwnd(), 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}
	else if(pOldBackground!=nullptr)
	{
		pOldBackground->ShowWindow(SW_HIDE);
		const LONG nStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, nStyle&~WS_EX_LAYERED);
		::RedrawWindow(m_hWnd, nullptr, nullptr, RDW_ERASE|RDW_INVALIDATE|RDW_FRAME|RDW_ALLCHILDREN);
	}

	return 0;
}

BOOL CMainFrame::CreateRibbonBar(CMFCRibbonBar& wndRibbonBar, CMFCToolBarImages& panelImages)
{
	CMFCRibbonApplicationButton* pMainButton = new CMFCRibbonApplicationButton();
	pMainButton->SetImage(IDB_RIBBON_MAIN);
	pMainButton->SetToolTipText(_T("File"));
	pMainButton->SetText(_T("\nf"));
	m_wndRibbonBar.SetApplicationButton(pMainButton, CSize(45, 45));

	CMFCRibbonCategory* pCategoryMap = m_wndRibbonBar.AddCategory(_T("&Map"), IDB_MAP_SMALL, IDB_MAP_LARGE);
	if(pCategoryMap!=nullptr)
	{
		CMFCRibbonPanel* pPanelNavigation = pCategoryMap->AddPanel(_T("Navigation"));
		if(pPanelNavigation!=nullptr)
		{
			std::unique_ptr<CMFCRibbonButton> apBtn1(new CMFCRibbonButton(ID_GO_BACK, _T("Go Back"), 0, 0));
			apBtn1->SetAlwaysLargeImage();
			pPanelNavigation->Add(apBtn1.release());
			std::unique_ptr<CMFCRibbonButton> apBtn2(new CMFCRibbonButton(ID_GO_FORWARD, _T("Go Forward"), 0, 1));
			apBtn2->SetAlwaysLargeImage();
			pPanelNavigation->Add(apBtn2.release());
			std::unique_ptr<CMFCRibbonButton> apBtn3(new CMFCRibbonButton(ID_GO_HOME, _T("Home Page"), 0, 2));
			apBtn3->SetAlwaysLargeImage();
			pPanelNavigation->Add(apBtn3.release());

			pPanelNavigation->SetCenterColumnVert();
		}
		CMFCRibbonPanel* pPanelBrowse = pCategoryMap->AddPanel(_T("Browse"));
		if(pPanelBrowse!=nullptr)
		{
			std::unique_ptr<CMFCRibbonButton> apBtn1(new CMFCRibbonButton(ID_TOOL_ZOOMIN, _T("Zoom In"), 0, 3));
			apBtn1->SetAlwaysLargeImage();
			pPanelBrowse->Add(apBtn1.release());
			std::unique_ptr<CMFCRibbonButton> apBtn2(new CMFCRibbonButton(ID_TOOL_ZOOMOUT, _T("Zoom Out"), 0, 4));
			apBtn2->SetAlwaysLargeImage();
			pPanelBrowse->Add(apBtn2.release());
			std::unique_ptr<CMFCRibbonButton> apBtn3(new CMFCRibbonButton(ID_TOOL_PAN, _T("Pan Map"), 0, 5));
			apBtn3->SetAlwaysLargeImage();
			pPanelBrowse->Add(apBtn3.release());

			pPanelBrowse->SetCenterColumnVert();
		}
		//CMFCRibbonPanel* pPanelAssistant = pCategoryMap->AddPanel(_T("Assistant"));
		//if(pPanelAssistant != nullptr)
		//{
		//	std::unique_ptr<CMFCRibbonButton> apBtn1(new CMFCRibbonButton(ID_VIEW_RULER, _T("Ruler"), 0, 6));
		//	apBtn1->SetAlwaysLargeImage();
		//	pPanelAssistant->Add(apBtn1.release());
		//	std::unique_ptr<CMFCRibbonButton> apBtn2(new CMFCRibbonButton(ID_VIEW_LEGEND, _T("Legend"), 0, 7));
		//	apBtn2->SetAlwaysLargeImage();
		//	pPanelAssistant->Add(apBtn2.release());
		//	
		//	pPanelAssistant->SetCenterColumnVert();
		//}
		CMFCRibbonPanel* pPanelLevel = pCategoryMap->AddPanel(_T("Level"));
		if(pPanelLevel != nullptr)
		{
			pPanelLevel->SetCenterColumnVert();

			CMFCRibbonSlider* pBtnSlider = new CMFCRibbonSlider(ID_MAP_LEVELSLIDER, 100);
			pBtnSlider->SetZoomButtons();
			pBtnSlider->SetRange(0, 25);
			pBtnSlider->SetPos(1);
			pPanelLevel->Add(pBtnSlider);

			std::unique_ptr<CMFCRibbonButtonsGroup> apGroup(new CMFCRibbonButtonsGroup);
			apGroup->AddButton(new CMFCRibbonLabel(_T("   1 : ")));
			CMFCRibbonEdit* pEdit = new CMFCRibbonEdit(ID_MAP_LEVELEDIT, 65);
			pEdit->SetEditText(_T("25"));
			apGroup->AddButton(pEdit);
			apGroup->AddButton(new CMFCRibbonButton(ID_MAP_LEVELGO, _T(""), 0, -1));
			pPanelLevel->Add(apGroup.release());
		}
		CMFCRibbonPanel* pPanelScale = pCategoryMap->AddPanel(_T("Scale"));
		if(pPanelScale!=nullptr)
		{
			pPanelScale->SetCenterColumnVert();

			CMFCRibbonSlider* pBtnSlider = new CMFCRibbonSlider(ID_MAP_SCALESLIDER, 100);
			pBtnSlider->SetZoomButtons();
			pBtnSlider->SetRange(0, 100);
			pBtnSlider->SetPos(50);
			pPanelScale->Add(pBtnSlider);

			std::unique_ptr<CMFCRibbonButtonsGroup> apGroup(new CMFCRibbonButtonsGroup);
			apGroup->AddButton(new CMFCRibbonLabel(_T("   1 : ")));
			CMFCRibbonEdit* pEdit = new CMFCRibbonEdit(ID_MAP_SCALEEDIT, 65);
			pEdit->SetEditText(_T("100"));
			apGroup->AddButton(pEdit);
			apGroup->AddButton(new CMFCRibbonButton(ID_MAP_SCALEGO, _T(""), 0, -1));
			pPanelScale->Add(apGroup.release());
		}

		CMFCRibbonPanel* pPanelCoordinate = pCategoryMap->AddPanel(_T("Coordinate"));
		if(pPanelCoordinate!=nullptr)
		{
			pPanelCoordinate->SetCenterColumnVert();
			CMFCRibbonEdit* pBtn1 = new CMFCRibbonEdit(ID_COORDINATE_LONGITUDE, 90, _T("X:"), -1);
			pBtn1->SetEditText(_T("0"));
			pBtn1->SetTextAlign(ES_LEFT);
			pPanelCoordinate->Add(pBtn1);

			CMFCRibbonEdit* pBtn2 = new CMFCRibbonEdit(ID_COORDINATE_LATITUDE, 90, _T("Y:  "), -1);
			pBtn2->SetEditText(_T("0"));
			pBtn2->SetTextAlign(ES_LEFT);
			pPanelCoordinate->Add(pBtn2);
		}

		CMFCRibbonPanel* pPanelMeasure = pCategoryMap->AddPanel(_T("Measure"));
		if(pPanelMeasure!=nullptr)
		{
			pPanelMeasure->SetCenterColumnVert();
			pPanelMeasure->AddToolBar(IDR_MEASUREBAR);
		}
	}

	CMFCRibbonCategory* pCategoryInfo = m_wndRibbonBar.AddCategory(_T("&Information"), 0, 0);
	if(pCategoryInfo!=nullptr)
	{
		CMFCRibbonPanel* pPanelInfo = pCategoryInfo->AddPanel(_T("Information"));
		pPanelInfo->SetCenterColumnVert();
		pPanelInfo->AddToolBar(IDR_INFOBAR);

		CMFCRibbonPanel* pPanelQuery = pCategoryInfo->AddPanel(_T("Query"));
		pPanelQuery->SetCenterColumnVert();
		pPanelQuery->AddToolBar(IDR_QUERYBAR);

		CMFCRibbonPanel* pPanelLink = pCategoryInfo->AddPanel(_T("Link"));
		pPanelLink->SetCenterColumnVert();
		pPanelLink->AddToolBar(IDR_LINKBAR);

		CMFCRibbonPanel* pPanelPOI = pCategoryInfo->AddPanel(_T("POI"));
		if(pPanelPOI!=nullptr)
		{
			pPanelPOI->SetCenterColumnVert();

			CMFCRibbonComboBox* pComboBox1 = new CMFCRibbonComboBox(ID_INFO_POI1, FALSE, 150);
			pPanelPOI->Add(pComboBox1);
		}
	}

	CMFCRibbonCategory* pCategoryGPS = m_wndRibbonBar.AddCategory(_T("&GPS"), 0, IDB_GPS_LARGE);
	if(pCategoryMap!=nullptr)
	{
		CMFCRibbonPanel* pPanelTrace = pCategoryGPS->AddPanel(_T("Trace"));
		if(pPanelTrace!=nullptr)
		{
			pPanelTrace->SetCenterColumnVert();
			std::unique_ptr<CMFCRibbonButton> apBtn1(new CMFCRibbonButton(ID_GPS_TRACE, _T(""), 0, 0));
			apBtn1->SetAlwaysLargeImage();
			pPanelTrace->Add(apBtn1.release());
		}

		CMFCRibbonPanel* pPanelReceiver = pCategoryGPS->AddPanel(_T("GPS Receiver"));
		if(pPanelReceiver!=nullptr)
		{
			pPanelReceiver->SetCenterColumnVert();
			std::unique_ptr<CMFCRibbonButton> apBtn1(new CMFCRibbonButton(ID_GPS_DEVICE, _T(""), 1, 1));
			apBtn1->SetAlwaysLargeImage();
			pPanelReceiver->Add(apBtn1.release());
		}

		CMFCRibbonPanel* pPanelTools = pCategoryGPS->AddPanel(_T("Tools"));
		if(pPanelTools!=nullptr)
		{
			pPanelTools->SetCenterColumnVert();
			pPanelTools->AddToolBar(IDR_NAVIGATEBAR);
		}
	}

	CMFCRibbonCategory* pCategoryView = m_wndRibbonBar.AddCategory(_T("&View"), 0, 0);
	if(pCategoryView!=nullptr)
	{
		CMFCRibbonPanel* pPanelPanels = pCategoryView->AddPanel(_T("Panes"));
		if(pPanelPanels!=nullptr)
		{
			pPanelPanels->SetCenterColumnVert();

			pPanelPanels->Add(new CMFCRibbonCheckBox(ID_PANE_ATLAS, _T("Atlas")));
			pPanelPanels->Add(new CMFCRibbonCheckBox(ID_PANE_LAYER, _T("Layer")));
			pPanelPanels->Add(new CMFCRibbonCheckBox(ID_PANE_POU, _T("POU")));
			pPanelPanels->Add(new CMFCRibbonCheckBox(ID_PANE_QUERY, _T("Query")));
			pPanelPanels->Add(new CMFCRibbonCheckBox(ID_PANE_VEHICLE, _T("Vehicle")));
			pPanelPanels->Add(new CMFCRibbonCheckBox(ID_PANE_NAVIGATE, _T("Navigate")));
			pPanelPanels->Add(new CMFCRibbonCheckBox(ID_PANE_TABLE, _T("Database table")));
			const int count = pPanelPanels->GetCount();
			for(int index = count-2; index>=0; index--)
			{
				pPanelPanels->InsertSeparator(index+1);
			}
		}
	}
	return TRUE;
}
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG

BOOL CMainFrame::DestroyWindow()
{
	this->KillTimer(TIMER_PULSE);
	if(m_LayerPane.m_hWnd!=nullptr)
	{
		m_LayerPane.PostMessage(WM_DESTROY, 0, 0);
		m_LayerPane.DestroyWindow();
	}
	if(m_AtlasPane.m_hWnd!=nullptr)
	{
		m_AtlasPane.PostMessage(WM_DESTROY, 0, 0);
		m_AtlasPane.DestroyWindow();
	}
	if(m_QueryPane.m_hWnd!=nullptr)
	{
		m_QueryPane.PostMessage(WM_DESTROY, 0, 0);
		m_QueryPane.DestroyWindow();
	}
	if(m_POUPane.m_hWnd!=nullptr)
	{
		m_POUPane.SetAtlCtrl((CPOUListCtrl*)nullptr);
		m_POUPane.PostMessage(WM_DESTROY, 0, 0);
		m_POUPane.DestroyWindow();
	}
	if(m_TablePane.m_hWnd!=nullptr)
	{
		const BOOL bVisible2 = (m_TablePane.GetStyle()&WS_VISIBLE)!=0;
		AfxGetApp()->WriteProfileInt(_T("View"), _T("TablePane"), bVisible2);

		m_TablePane.PostMessage(WM_DESTROY, 0, 0);
		m_TablePane.DestroyWindow();
	}

	if(m_pHtmlView!=nullptr)
	{
		m_pHtmlView->PostMessage(WM_DESTROY, 0, 0);
		m_pHtmlView->DestroyWindow();
		delete m_pHtmlView;
		m_pHtmlView = nullptr;
	}

	if(m_pPOIBar!=nullptr)
	{
		m_pPOIBar->SetDatabase(nullptr);
		m_pPOIBar->DestroyWindow();
		delete m_pPOIBar;
		m_pPOIBar = nullptr;
	}

	if(m_pAtlas!=nullptr)
	{
		delete m_pAtlas->m_pTopo;
		m_pAtlas->m_pTopo = nullptr;

		delete m_pAtlas;
		m_pAtlas = nullptr;
	}

	if(m_pPort!=nullptr)
	{
		m_pPort->Close();
		delete m_pPort;
		m_pPort = nullptr;
	}

	if(m_pReplay!=nullptr)
	{
		m_pReplay->Close();
		delete m_pReplay;
		m_pReplay = nullptr;
	}

	POSITION pos = m_mapVehicle.GetStartPosition();
	while(pos!=nullptr)
	{
		DWORD dwId;
		CVehicle* pVehicle = nullptr;

		m_mapVehicle.GetNextAssoc(pos, dwId, pVehicle);
		if(pVehicle!=nullptr)
		{
			pVehicle->m_pFrame = nullptr;
			delete pVehicle;
			pVehicle = nullptr;
		}
	}

	m_mapVehicle.RemoveAll();

	CMFCRibbonApplicationButton* pMainButton = m_wndRibbonBar.GetApplicationButton();
	if(pMainButton!=nullptr)
	{
		pMainButton->DestroyCtrl();
		delete pMainButton;
		pMainButton = nullptr;
	}

	return CMDIFrameWndEx::DestroyWindow();
}

BOOL CMainFrame::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CMDIFrameWndEx::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nId)
			{
			case ID_GPS_TRACE:
				if(m_pReplay!=nullptr)
				{
					pCmdUI->Enable(FALSE);
					return TRUE;
				}
				else
				{
					pCmdUI->SetCheck(m_pPort!=nullptr);
				}
				break;
			case ID_GPS_REPLAY:
				if(m_pPort!=nullptr)
				{
					pCmdUI->Enable(FALSE);
					return TRUE;
				}
				else
				{
					pCmdUI->SetCheck(m_pReplay!=nullptr);
				}
				break;
			case ID_GO_BACK:
			{
				CFrameWnd* pChild = this->GetActiveFrame();
				if(pChild==nullptr)
					break;

				if(m_pHtmlView!=nullptr)
				{
					if(pChild!=this)
					{
						pCmdUI->Enable(TRUE);
						return TRUE;
					}
					else if(m_pHtmlView->m_bCanBack==TRUE)
					{
						pCmdUI->Enable(TRUE);
						return TRUE;
					}
				}
				else
				{
					if(pChild!=this&&((CChildFrame*)pChild)->m_Go.CanBack()==TRUE)
					{
						pCmdUI->Enable(TRUE);
						return TRUE;
					}
				}
			}
			pCmdUI->Enable(FALSE);
			return TRUE;
			break;
			case ID_GO_HOME:
			{
				CFrameWnd* pChild = this->GetActiveFrame();
				if(pChild==nullptr)
					break;

				if(m_pHtmlView!=nullptr)
				{
					if(pChild!=this)
					{
						pCmdUI->Enable(TRUE);
						return TRUE;
					}
					else if(m_pHtmlView->m_bCanBack==TRUE)
					{
						pCmdUI->Enable(TRUE);
						return TRUE;
					}
				}
				else if(pChild!=this&&((CChildFrame*)pChild)->m_Go.CanBack()==TRUE)
				{
					pCmdUI->Enable(TRUE);
					return TRUE;
				}
				else if(m_pAtlas!=nullptr)
				{
					const CString strFile = m_pAtlas->m_strHomePage;
					if(IsWebFile(strFile)==TRUE)
					{
						pCmdUI->Enable(TRUE);
						return TRUE;
					}
				}
			}
			pCmdUI->Enable(FALSE);
			return TRUE;
			break;
			case ID_VIEW_LEGEND:
				if(m_pHtmlView!=nullptr)
				{
					pCmdUI->Enable(FALSE);
				}
				else if(m_pAtlas!=nullptr)
				{
					CFrameWnd* pChild = this->GetActiveFrame();
					if(pChild==nullptr)
					{
						pCmdUI->Enable(FALSE);
						return TRUE;
						break;
					}
					CView* pView = pChild->GetActiveView();
					if(pView==nullptr)
					{
						pCmdUI->Enable(FALSE);
						return TRUE;
						break;
					}
					CGrfDoc* pDoc = (CGrfDoc*)pView->GetDocument();
					if(pDoc==nullptr)
					{
						pCmdUI->Enable(FALSE);
						return TRUE;
						break;
					}
					if(pDoc->m_pDeck!=nullptr&&pDoc->m_pDeck->m_strLegend.IsEmpty()==FALSE)
						pCmdUI->Enable(TRUE);
					else
						pCmdUI->Enable(FALSE);
				}
				return TRUE;
				break;
			default:
				break;
			}

			if(m_pHtmlView!=nullptr)
				return m_pHtmlView->OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);

			pCmdUI->m_bContinueRouting = false;
		}
	}

	if(nCode==CN_COMMAND)
	{
		if(m_pHtmlView!=nullptr)
		{
			switch(nId)
			{
			case ID_GO_BACK:
			case ID_GO_FORWARD:
			case ID_GO_HOME:
				m_pHtmlView->OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
				break;
			}
		}
	}

	return CMDIFrameWndEx::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnGpsTrack()
{
	if(m_pPort==nullptr)
	{
		const CString strPort = AfxGetApp()->GetProfileString(_T("Navigate"), _T("Port"), _T("GPS Receiver"));
		if(strPort.CompareNoCase(_T("Socket"))==0)
		{
			m_pPort = new CPortSocket(this);
			this->ActivePane('V');
		}
		else if(strPort.CompareNoCase(_T("GPS Receiver"))==0)
		{
			m_pPort = new CPortReceiver(this);
			this->ActivePane('G');
		}

		if(m_pPort!=nullptr)
		{
			if(m_pPort->Open()==TRUE)
			{
				a_bGPSAuto = AfxGetApp()->GetProfileInt(_T("Receiver"), _T("Autoin"), 1);
				AfxGetMainWnd()->SendMessage(WM_ACTIVEMANAGER, 'G', 0);
			}
			else if(m_pPort!=nullptr)
			{
				m_pPort->Close();
				delete m_pPort;
				m_pPort = nullptr;
			}
		}
	}
	else
	{
		this->SendMessage(WM_VEHICLECONTROL, SERVER_DOWN, 0);
	}
}

void CMainFrame::OnGpsDevice()
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
	AfxSetResourceHandle(hInst);

	CString strTitle;
	strTitle.LoadString(IDS_SETUP);
	CNavSetSheet sheet(strTitle, this, 0);
	if(sheet.DoModal()==IDOK)
	{
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
	case TIMER_REPLAY:
		if(m_pReplay!=nullptr)
		{
			m_pReplay->ReceiveMsg();
		}
		break;
	case TIMER_GETGPSMSE:
		if(m_pPort!=nullptr)
		{
			m_pPort->ReceiveMsg();
		}
		break;
	case TIMER_VEHICLE:
	{
		static bool bOrder = true;
		if(bOrder==true)
		{
			POSITION pos = m_mapVehicle.GetStartPosition();
			while(pos!=nullptr)
			{
				DWORD dwId;
				CVehicle* pVehicle = nullptr;
				m_mapVehicle.GetNextAssoc(pos, dwId, pVehicle);
				if(pVehicle->m_pFrame==nullptr)
					continue;

				CView* pView = pVehicle->m_pFrame->GetActiveView();
				if(pView!=nullptr)
				{
					pVehicle->Flash(pView, ((CGrfView*)pView)->m_viewMonitor, ((CGrfView*)pView)->m_nAngle);
				}
			}
		}
		else
		{
			CObList vehiclelist;
			POSITION pos1 = m_mapVehicle.GetStartPosition();
			while(pos1!=nullptr)
			{
				DWORD dwId;
				CVehicle* pVehicle = nullptr;
				m_mapVehicle.GetNextAssoc(pos1, dwId, pVehicle);
				if(pVehicle->m_pFrame==nullptr)
					continue;

				vehiclelist.AddTail(pVehicle);
			}
			POSITION pos2 = vehiclelist.GetTailPosition();
			while(pos2!=nullptr)
			{
				CVehicle* pVehicle = (CVehicle*)vehiclelist.GetPrev(pos2);
				if(pVehicle->m_pFrame==nullptr)
					continue;

				CView* pView = pVehicle->m_pFrame->GetActiveView();
				if(pView!=nullptr)
				{
					pVehicle->Flash(pView, ((CGrfView*)pView)->m_viewMonitor, ((CGrfView*)pView)->m_nAngle);
				}
			}
			vehiclelist.RemoveAll();
		}
		bOrder = 1-bOrder;
	}
	break;
	case TIMER_PULSE:
	{
		/*		CViewerApp* pApp = (CViewerApp*)AfxGetApp();
		if(pApp != nullptr && pApp->m_ClientSocket.m_bLogin == true)
		{
		*pApp->m_ClientSocket.m_pArOut << (WORD)CLIENT_PULSE;
		*pApp->m_ClientSocket.m_pArOut << (WORD)(~CLIENT_PULSE);
		pApp->m_ClientSocket.m_pArOut->Flush();

		static nTimes = 0;
		if(nTimes == 3)
		{
		if(pApp->m_ClientSocket.m_bHeart == true)
		{
		pApp->m_ClientSocket.m_bHeart = false;
		}
		else
		{
		this->KillTimer(TIMER_PULSE);
		AfxMessageBox(_T("网络已经中断，请重新连接！"));
		}

		nTimes = 0;
		}
		else
		{
		nTimes++;
		}
		}*/
	}
	break;
	default:
		break;
	}

	CMDIFrameWndEx::OnTimer(nIDEvent);
}

LONG CMainFrame::OnKeyQuery(UINT WPARAM, LONG LPARAM)
{
	CFrameWnd* pChild = this->GetActiveFrame();
	if(pChild==this)
		return 0L;

	if(pChild==nullptr)
		return 0L;

	AfxGetApp()->BeginWaitCursor();

	int count = 0;
	CView* pView = pChild->GetActiveView();
	if(pView!=nullptr)
	{
		count = pView->SendMessage(WM_KEYQUERY, WPARAM, LPARAM);
	}

	if(m_pAtlas!=nullptr)
	{
		const CString string = *(CString*)WPARAM;
		CListCtrl* listCtrl = (CListCtrl*)LPARAM;

		DWORD wMap = 0XFFFF;
		DWORD dwClass = 0XFFFFFFFF;
		_stscanf(string, _T("%X  %X"), &wMap, &dwClass);
		const CString strKey = string.Mid(14);

		count += m_pAtlas->KeyQuery(listCtrl, strKey, dwClass, wMap);

		wMap = 0;
		dwClass = 0;
	}

	AfxGetApp()->EndWaitCursor();
	return count;
}

LONG CMainFrame::OnGetQueryCtrl(UINT WPARAM, LONG LPARAM)
{
	return (LONG)&(m_QueryPane.m_KeyQueryEar.m_listCtrl);
}

void CMainFrame::OnGpsReplay()
{
	if(m_pReplay==nullptr)
	{
		const CString strPort = AfxGetApp()->GetProfileString(_T("Navigate"), _T("Port"), _T("GPS Receiver"));
		if(strPort.CompareNoCase(_T("GPS Receiver"))==0)
		{
			m_pReplay = new CReplay(this);
			if(m_pReplay->Open()==TRUE)
			{
				a_bGPSAuto = AfxGetApp()->GetProfileInt(_T("Receiver"), _T("Autoin"), 1);
				AfxGetMainWnd()->SendMessage(WM_ACTIVEMANAGER, 'G', 0);
			}
		}
		else
		{
			m_pReplay = new CReplayTable(this);
		}
	}
	else
	{
		m_pReplay->Close();
		delete m_pReplay;
		m_pReplay = nullptr;

		m_Route.Stop(nullptr);
	}
}

LONG CMainFrame::OnVehicleControl(UINT WPARAM, LONG LPARAM)
{
	//CFrameWnd* pMainChild = (CFrameWnd*)SendMessage(WM_GETMAINCHILD,0,0);
	//if(pMainChild == nullptr)
	//	return 0;

	//switch(WPARAM)
	//{		
	//case SERVER_LOGIN_SUCCEED:
	//	{
	//		AfxMessageBox(IDS_LOGINSUCCEED);
	//	}
	//	break;
	//case SERVER_LOGIN_FAILED:
	//	{
	//		if(m_pPort != nullptr)
	//		{
	//			m_pPort->Close();		
	//			delete m_pPort;
	//			m_pPort = nullptr;
	//		}
	//		AfxMessageBox(IDS_LOGINFAILD);
	//	}
	//	break;
	//case SERVER_DOWN:
	//	{
	//		this->KillTimer(TIMER_VEHICLE);

	//		m_Route.Stop(nullptr);

	//		POSITION pos = m_mapVehicle.GetStartPosition();
	//		while(pos!=nullptr)
	//		{
	//			DWORD dwId;
	//			CVehicle* pVehicle=nullptr;

	//			m_mapVehicle.GetNextAssoc(pos,dwId,pVehicle);
	//			if(pVehicle != nullptr)
	//			{
	//				pVehicle->m_pFrame = nullptr;
	//				delete pVehicle;
	//				pVehicle = nullptr;
	//			}
	//		}
	//		m_mapVehicle.RemoveAll();

	//		CWnd* pTempWnd = this->GetWindow(GW_CHILD);
	//		while(pTempWnd != nullptr)
	//		{
	//			CRuntimeClass* pRuntimeClass = pTempWnd->GetRuntimeClass();
	//			if(_tcscmp(pRuntimeClass->m_lpszClassName, _T("CWnd"))  == 0)
	//				break;

	//			pTempWnd = pTempWnd->GetNextWindow(GW_HWNDNEXT);
	//		}
	//		if(pTempWnd == nullptr)
	//			break;

	//		CMDIChildWnd* pChildFrm = (CMDIChildWnd*)pTempWnd->GetWindow(GW_CHILD);
	//		while(pChildFrm != nullptr)
	//		{
	//			CMDIChildWnd* pChild = pChildFrm;
	//			pChildFrm = (CMDIChildWnd*)pChildFrm->GetWindow(GW_HWNDNEXT);

	//			BOOL bMainChild = pChild->SendMessage(WM_ISMAINCHILD,0,0);
	//			if(bMainChild == TRUE)
	//				pChild->PostMessage(WM_MDIMAXIMIZE,0,0);
	//			else
	//				pChild->SendMessage(WM_CLOSE,0,0);
	//		}

	//		if(m_pPort != nullptr)
	//		{
	//			m_pPort->Close();		
	//			delete m_pPort;
	//			m_pPort = nullptr;
	//		}
	//		if(m_pReplay != nullptr)
	//		{
	//			m_pReplay->Close();		
	//			delete m_pReplay;
	//			m_pReplay = nullptr;
	//		}

	//		if(m_pNavinfoPane != nullptr)
	//			m_NavinfoPane...m_pNavigateDlg->SetID(0);
	//		if(m_pVehiclePane != nullptr)
	//			m_NavinfoPane..SetStatus(0);

	//		/*CMDIChildWnd *pChild = (CMDIChildWnd*)this->GetActiveFrame();
	//		if(pChild != nullptr)	
	//		{
	//			CGrfView* pView = (CGrfView*)pChild->GetActiveView();
	//			if(pView != nullptr)
	//			{
	//				CRect rect;
	//				pView->GetClientRect(rect);
	//				CPoint cliPoint = rect.CenterPoint();
	//				CPoint docPoint = cliPoint;
	//				pView->SendMessage(WM_CLIENTTODOC,(UINT)&docPoint,1);
	//				pView->RotateView(0,docPoint,cliPoint);
	//			}
	//		}*/

	//		CGrfView::m_nAngle = 0;
	//	}
	//	break;
	//case GPSSERVER_VEHICLE_NEW:
	//	{
	//		Content* content = (Content*)LPARAM;

	//		CVehicle* pVehicle = nullptr;
	//		if(m_mapVehicle.Lookup(content->dwId,pVehicle) == TRUE)
	//		{
	//			AfxMessageBox(_T("此车正处于被监控状态,你的请求非法!"));
	//			return 0;
	//		}
	//		else
	//		{
	//			if(m_mapVehicle.GetSize() == 1)//若原来只有一辆车，并且处于监控状态，则将该车移到一个单独的窗口中监视
	//			{
	//				POSITION pos = m_mapVehicle.GetStartPosition();
	//				DWORD dwId;
	//				CVehicle* pVehicle;
	//				m_mapVehicle.GetNextAssoc(pos,dwId,pVehicle);
	//				if(pVehicle != nullptr && pVehicle->m_bTrack == true)
	//				{
	//					CGrfView* pMainView = (CGrfView*)pMainChild->GetActiveView();
	//					CDocument* pDoc = pMainView->GetDocument();
	//					if(pDoc != nullptr)
	//					{
	//						CDocTemplate* pTemplate = pDoc->GetDocTemplate();
	//						ASSERT_VALID(pTemplate);
	//						CFrameWnd* pChildFrm = pTemplate->CreateNewFrame(pDoc, pMainChild);
	//						if(pChildFrm != nullptr)
	//						{
	//							((CChildFrame*)pChildFrm)->m_bMainChild = FALSE;
	//							pVehicle->m_pFrame = pChildFrm;

	//							pTemplate->InitialUpdateFrame(pChildFrm, pDoc);

	//							CRect rect;
	//							pMainView->GetClientRect(&rect);
	//							CPoint cliPoint = rect.CenterPoint();
	//							CPointF mapPoint = pMainView->m_viewMonitor.ClientToMap(cliPoint);

	//							MDITile(MDITILE_SKIPDISABLED);

	//							CGrfView* pView = (CGrfView*)pChildFrm->GetActiveView();
	//							if(pView != nullptr)
	//							{
	//								pView->m_viewMonitor.ScaleTo(pView,pMainView->m_viewMonitor.m_scaleCurrent,mapPoint);
	//							}
	//						}
	//					}
	//				}
	//			}

	//			CVehicle* pVehicle = new CVehicle;
	//			pVehicle->m_dwId = content->dwId;
	//			pVehicle->m_strUser = content->strUser;
	//			pVehicle->m_strCode = content->strCode;
	//			pVehicle->m_strTag = content->strCode;
	//			pVehicle->m_strIVU = content->strIVU;
	//			pVehicle->m_pFrame = pMainChild;
	//			m_mapVehicle.SetAt(content->dwId,pVehicle);	
	//			PostMessage(WM_CHILDDOCCHANGED,(UINT)pMainChild,0);

	//			if(m_pAtlas != nullptr)
	//			{
	//				int nTrackless  = 0;
	//				POSITION pos = m_mapVehicle.GetStartPosition();
	//				while(pos != nullptr)
	//				{
	//					DWORD dwId;
	//					CVehicle* vehicle;
	//					m_mapVehicle.GetNextAssoc(pos,dwId,vehicle);
	//					if(vehicle->m_bTrack == false)
	//					{
	//						nTrackless++;
	//					}
	//				}

	//				if(nTrackless == 1)//若只有新进来的这辆车没有被监控（位于主窗口中），自动匹配，进入首页地图
	//				{
	//					CString strFile = m_pAtlas->m_strHomePage;
	//					if(IsMapFile(strFile) == TRUE && _taccess(strFile,00) != -1)
	//					{
	//						CDocument* pDocument = (CDocument*)pMainChild->SendMessage(WM_OPENDOCUMENTFILE,(UINT)&strFile,0);
	//					}
	//				}
	//			}

	//			if(m_mapVehicle.GetCount() == 1 && m_pNavinfoPane != nullptr)
	//			{
	//				m_NavinfoPane..m_pNavigateDlg->SetID(pVehicle->m_dwId);
	//			}
	//		}
	//	}
	//	break;
	//case GPSSERVER_VEHICLE_EREASE:
	//	{
	//		CVehicle* pVehicle = nullptr;
	//		if(m_mapVehicle.Lookup(LPARAM,pVehicle) == TRUE)
	//		{
	//			if(pVehicle != nullptr)
	//			{
	//				CFrameWnd* pFrame = pVehicle->m_pFrame;
	//				pVehicle->m_pFrame = nullptr;
	//				BOOL bMainChild = pFrame->SendMessage(WM_ISMAINCHILD,0,0);
	//				m_mapVehicle.RemoveKey(LPARAM);
	//				delete pVehicle;

	//				if(bMainChild == FALSE)
	//				{
	//					pFrame->SendMessage(WM_CLOSE,0,0);
	//					MDITile(MDITILE_SKIPDISABLED);
	//					pFrame = nullptr;
	//				}
	//				else if(pFrame != nullptr)
	//				{
	//					/*CGrfView* pView = (CGrfView*)pFrame->GetActiveView();
	//					if(pView != nullptr)
	//					{
	//						CRect rect;
	//						pView->GetClientRect(rect);
	//						CPoint cliPoint = rect.CenterPoint();
	//						CPoint docPoint = cliPoint;
	//						pView->SendMessage(WM_CLIENTTODOC,(UINT)&docPoint,1);
	//						pView->RotateView(0,docPoint,cliPoint);
	//					}*/
	//				}
	//			}
	//		}

	//		if(m_pVehiclePane != nullptr && m_NavinfoPane..m_dwVehicle == LPARAM)
	//			m_NavinfoPane..SetStatus(0);
	//	}
	//	break;
	//case GPSSERVER_VEHICLE_TRACK:
	//	{
	//		int nTrackless  = 0;
	//		CVehicle* pVehicle = nullptr;
	//		POSITION pos = m_mapVehicle.GetStartPosition();
	//		while(pos != nullptr)
	//		{
	//			DWORD dwId;
	//			CVehicle* Vehicle;
	//			m_mapVehicle.GetNextAssoc(pos,dwId,Vehicle);
	//			if(Vehicle->m_dwId == LPARAM)
	//			{
	//				if(Vehicle->m_bTrack == true)
	//				{
	//					//	AfxMessageBox("此车已经被跟踪!");
	//					return 0;
	//				}

	//				pVehicle = Vehicle;
	//				pVehicle->m_bTrack = true;
	//				nTrackless++;
	//			}
	//			else if(Vehicle->m_bTrack == false)
	//			{
	//				nTrackless++;
	//			}
	//		}

	//		if(pVehicle == nullptr)
	//			break;

	//		CView* pView = pMainChild->GetActiveView();
	//		if(nTrackless == 1)
	//		{
	//			CGrfDoc* pDoc = (CGrfDoc*)pView->GetDocument();
	//			if(m_pAtlas != nullptr && pDoc->m_pDeck != nullptr)
	//			{
	//				CDeck* pDeck = m_pAtlas->m_Pyramid.GetDeck(pVehicle->m_geoCurrent);
	//				if(pDeck != nullptr && pDeck != pDoc->m_pDeck)
	//				{
	//					CRect cliRect;
	//					pView->GetClientRect(cliRect);
	//					CPoint cliPoint = cliRect.CenterPoint();

	//					CString stringX;
	//					CString stringY;
	//					stringX.Format(_T("%.5f"),pVehicle->m_geoCurrent.x);
	//					stringY.Format(_T("%.5f"),pVehicle->m_geoCurrent.y);
	//					AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"),stringX);
	//					AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"),stringY);
	//					AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"),cliPoint.x);
	//					AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"),cliPoint.y);

	//					pDeck->Open(pMainChild,0);
	//				}
	//				else
	//				{			
	//					CPointF geoPoint = pVehicle->m_geoCurrent;
	//					CPointF mapPoint;
	//					CPoint docPoint;
	//					CRect cliRect;
	//					pView->GetClientRect(cliRect);
	//					CPoint cliPoint = cliRect.CenterPoint();

	//					pView->SendMessage(WM_GEOTOMAP,(UINT)&geoPoint,(LONG)&mapPoint);
	//					pView->SendMessage(WM_MAPTODOC,(UINT)&mapPoint,(LONG)&docPoint);

	//					pView->SendMessage(WM_FIXATEVIEW,(UINT)&docPoint,(LONG)&cliPoint);		
	//				}
	//			}
	//		}
	//		else
	//		{
	//			CDocument* pDoc = pView->GetDocument();
	//			if(pDoc != nullptr)
	//			{
	//				CDocTemplate* pTemplate = pDoc->GetDocTemplate();
	//				ASSERT_VALID(pTemplate);
	//				CFrameWnd* pChildFrm = pTemplate->CreateNewFrame(pDoc, pMainChild);
	//				if(pChildFrm == nullptr)
	//					return 0;

	//				((CChildFrame*)pChildFrm)->m_bMainChild = FALSE;
	//				pVehicle->m_pFrame = pChildFrm;

	//				pTemplate->InitialUpdateFrame(pChildFrm, pDoc);
	//				MDITile(MDITILE_SKIPDISABLED);

	//				if(m_pAtlas != nullptr && ((CGrfDoc*)pDoc)->m_pDeck != nullptr)
	//				{
	//					CDeck* pDeck = m_pAtlas->m_Pyramid.GetDeck(pVehicle->m_geoCurrent);
	//					if(pDeck != nullptr && pDeck != ((CGrfDoc*)pDoc)->m_pDeck)
	//					{
	//						CRect cliRect;
	//						pView->GetClientRect(cliRect);
	//						CPoint cliPoint = cliRect.CenterPoint();

	//						CString stringX;
	//						CString stringY;
	//						stringX.Format(_T("%.5f"),pVehicle->m_geoCurrent.x);
	//						stringY.Format(_T("%.5f"),pVehicle->m_geoCurrent.y);
	//						AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"),stringX);
	//						AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"),stringY);
	//						AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"),cliPoint.x);
	//						AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"),cliPoint.y);

	//						pDeck->Open(pChildFrm,0);
	//					}
	//					else
	//					{
	//						CView* pView = pChildFrm->GetActiveView();
	//						if(pView != nullptr)
	//						{
	//							CPointF geoPoint = pVehicle->m_geoCurrent;
	//							CPointF mapPoint;
	//							CPoint docPoint;
	//							CRect cliRect;
	//							pView->GetClientRect(cliRect);
	//							CPoint cliPoint = cliRect.CenterPoint();

	//							pView->SendMessage(WM_GEOTOMAP,(UINT)&geoPoint,(LONG)&mapPoint);
	//							pView->SendMessage(WM_MAPTODOC,(UINT)&mapPoint,(LONG)&docPoint);

	//							float fZoom = 1.0f;
	//							CRect rect(docPoint,docPoint);
	//							pView->SendMessage(WM_ZOOMIN,(UINT)&rect,(LONG)&fZoom);
	//							pView->SendMessage(WM_FIXATEVIEW,(UINT)&docPoint,(LONG)&cliPoint);		
	//						}
	//					}			
	//				}
	//			}
	//		}
	//	}
	//	break;
	//case GPSCLIENT_VEHICLE_UNTRACK:
	//case GPSSERVER_VEHICLE_UNTRACK:
	//	{
	//		CVehicle* pVehicle = nullptr;
	//		if(m_mapVehicle.Lookup(LPARAM,pVehicle) == TRUE)
	//		{
	//			if(pVehicle != nullptr)
	//			{
	//				if(pVehicle->m_bTrack == false)
	//				{
	//					AfxMessageBox(_T("此车并不处于跟踪状态!"));
	//					return 0;
	//				}
	//				else
	//				{
	//					pVehicle->m_bTrack = false;
	//					BOOL bMainChild = pVehicle->m_pFrame->SendMessage(WM_ISMAINCHILD,0,0);		
	//					if(bMainChild == FALSE)
	//					{
	//						pVehicle->m_pFrame->SendMessage(WM_CLOSE,0,0);
	//						MDITile(MDITILE_SKIPDISABLED);
	//						pVehicle->m_pFrame = pMainChild;
	//						PostMessage(WM_CHILDDOCCHANGED,(UINT)pMainChild,0);
	//					}
	//				}
	//			}
	//		}
	//	}
	//	break;
	//case GPSSERVER_TRACE_DRAW:
	//	{
	//		CVehicle* pVehicle = nullptr;
	//		if(m_mapVehicle.Lookup(LPARAM,pVehicle) == TRUE)
	//		{
	//			if(pVehicle != nullptr)
	//			{
	//				if(pVehicle->m_bDrawTrace == true)
	//				{
	//					AfxMessageBox(_T("此车已经处于绘制轨迹状态!"));
	//					return 0;
	//				}

	//				if(pVehicle->m_bTrack == false)
	//				{
	//					AfxMessageBox(_T("不能为一辆处于非跟踪状态的车辆绘制轨迹!"));
	//					return 0;
	//				}
	//				pVehicle->m_bDrawTrace = true;
	//			}
	//		}	
	//	}
	//	break;
	//case GPSSERVER_TRACE_UNDRAW:
	//	{
	//		CVehicle* pVehicle = nullptr;
	//		if(m_mapVehicle.Lookup(LPARAM,pVehicle) == TRUE)
	//		{
	//			if(pVehicle != nullptr)
	//			{
	//				if(pVehicle->m_bDrawTrace == false)
	//				{
	//					AfxMessageBox(_T("此车并没处于轨迹绘制状态!"));
	//					return 0;
	//				}
	//				pVehicle->m_bDrawTrace = false;
	//			}
	//		}
	//	}
	//	break;
	//case GPSSERVER_TRACE_SAVE:
	//	{
	//		CVehicle* pVehicle = nullptr;
	//		if(m_mapVehicle.Lookup(LPARAM,pVehicle) == TRUE)
	//		{
	//			if(pVehicle != nullptr)
	//			{
	//				if(pVehicle->m_Route.m_hFile != INVALID_HANDLE_VALUE)
	//				{
	//					AfxMessageBox(_T("此车已经处于保存轨迹状态!"));
	//					return 0;
	//				}
	//				else
	//					pVehicle->Store();
	//			}
	//		}
	//	}
	//	break;
	//case GPSSERVER_TRACE_UNSAVE:
	//	{
	//		CVehicle* pVehicle = nullptr;
	//		if(m_mapVehicle.Lookup(LPARAM,pVehicle) == TRUE)
	//		{
	//			if(pVehicle != nullptr)
	//			{
	//				if(pVehicle->m_Route.m_hFile == INVALID_HANDLE_VALUE)
	//				{
	//					AfxMessageBox(_T("此车并没有处于轨迹保存状态!"));
	//					return 0;
	//				}
	//				else
	//					pVehicle->Store();
	//			}
	//		}
	//	}
	//	break;
	//default:
	//	break;
	//}	

	//if(m_TablePane.m_pMonitorCtrl != nullptr)
	//{
	//	m_TablePane.m_pMonitorCtrl->VehicleControl(WPARAM, LPARAM);
	//}

	return 1;
}

LONG CMainFrame::OnVehicleIcon(UINT WPARAM, LONG LPARAM)
{
	CVehicle* pVehicle = nullptr;
	if(m_mapVehicle.Lookup(WPARAM, pVehicle)==TRUE)
	{
		if(pVehicle!=nullptr)
		{
			pVehicle->m_bIcon = LPARAM;
		}
	}

	return 0;
}

LONG CMainFrame::OnVehicleMove(UINT WPARAM, LONG LPARAM)
{
	//if(WPARAM == 0 || LPARAM == 0)
	//	return 0;

	//if(m_TablePane.m_pMonitorCtrl != nullptr)
	//{
	//	m_TablePane.m_pMonitorCtrl->VehicleMove(WPARAM, LPARAM);
	//}

	//CVehicle* pVehicle = nullptr;
	//if(m_mapVehicle.Lookup(WPARAM,pVehicle) == TRUE)
	//{
	//	SMessage* message = (SMessage*)LPARAM;
	//	if(message->m_bAvailable == TRUE)
	//	{
	//		CFrameWnd* pChildFrm = pVehicle->m_pFrame;
	//		CView* pView = pChildFrm->GetActiveView();
	//		if(pView == nullptr)
	//			return 0;

	//		CGrfDoc* pDoc = (CGrfDoc*)pView->GetDocument(); 
	//		if(pDoc == nullptr || pDoc->m_Datainfo.m_pProjection == nullptr)
	//			return 0;

	//		if(m_Security.HasNavigateRight() == true)
	//		{
	//			m_Route.ViewWillDraw(pView);			
	//		}

	//		pVehicle->Move(pView,message->lng,message->lat,message->altitude,message->nBearing,message->fSpeed,message->nTime);
	//		pVehicle->Flash(pView,((CGrfView*)pView)->m_nAngle);

	//		if(m_Security.HasNavigateRight() == true)
	//		{
	//			m_Route.Move(pView,message->lng,message->lat,message->altitude);			
	//			if(m_pNavinfoPane != nullptr)
	//			{
	//				float length = m_Route.GetLeftLength();
	//				m_NavinfoPane..m_pNavigateDlg->SetLeftLength(length);
	//			}
	//		}

	//		CPointF geoPoint = pVehicle->m_geoCurrent;
	//		CPoint docPoint = pVehicle->m_docCurrent;
	//		CPoint cliPoint = pVehicle->m_docCurrent;
	//		pView->SendMessage(WM_DOCTOCLIENT,(UINT)(&cliPoint),1);

	//		((CChildFrame*)pChildFrm)->m_ruler.DrawH(pView, (CMDIChildWnd*)pChildFrm,cliPoint);
	//		((CChildFrame*)pChildFrm)->m_ruler.DrawV(pView, (CMDIChildWnd*)pChildFrm,cliPoint);

	//		if(pVehicle->m_bTrack == true && m_pAtlas != nullptr)
	//		{
	//			if(a_bGPSAuto == TRUE)
	//			{						
	//				CDeck* pDeck = m_pAtlas->m_Pyramid.GetDeck(geoPoint);
	//				if(pDeck != nullptr && pDeck != pDoc->m_pDeck)
	//				{
	//					CString stringX;
	//					CString stringY;
	//					stringX.Format(_T("%.5f"),geoPoint.x);
	//					stringY.Format(_T("%.5f"),geoPoint.y);
	//					AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"),stringX);
	//					AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"),stringY);
	//					AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"),cliPoint.x);
	//					AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"),cliPoint.y);

	//					pDoc->SetModifiedFlag(FALSE);
	//					return  (LONG)pDeck->Open(pChildFrm,0);
	//				}
	//			}

	//			CRect cliRect;
	//			pView->GetClientRect(cliRect);
	//			CSize size = cliRect.CSize();
	//			size.cx = size.cx/10;
	//			size.cy = size.cy/10;
	//			cliRect.DeflateRect(size);

	//			CPoint cliPoint = docPoint;
	//			pView->SendMessage(WM_DOCTOCLIENT,(UINT)&cliPoint,1);
	//			if(cliRect.PtInRect(cliPoint) == FALSE)
	//			{	
	//				cliPoint = cliRect.CenterPoint();

	//				if(message->bRotateMap == TRUE && message->fSpeed>10.0f)
	//				{
	//					cliPoint.y = cliRect.bottom;
	//					pView->SendMessage(WM_FIXATEVIEW,(UINT)&docPoint,(LONG)&cliPoint);
	//				}
	//				else
	//				{
	//					pView->SendMessage(WM_FIXATEVIEW,(UINT)&docPoint,(LONG)&cliPoint);
	//				}
	//			}	
	//			else if(message->bRotateMap == TRUE && message->fSpeed>10.0f)
	//			{
	//				((CGrfView*)pView)->RotateView(message->nBearing,docPoint,cliPoint);
	//			}
	//		}
	//	}

	//	if(m_pNavinfoPane != nullptr)
	//	{
	//		m_NavinfoPane..m_pNavigateDlg->SetInfo(pVehicle->m_dwId,*message);
	//	}
	//}

	return 1;
}

LONG CMainFrame::OnVehicleStatus(UINT WPARAM, LONG LPARAM)
{
	if(WPARAM==0||LPARAM==0)
		return 0;

	const SMessage* message = (SMessage*)LPARAM;

	CVehicle* pVehicle = nullptr;
	if(m_mapVehicle.Lookup(WPARAM, pVehicle)==FALSE)
		return 0;
	if(pVehicle==nullptr)
		return 0;
	if(pVehicle->m_dwStatus==message->dwStatus)
		return 0;

	CCoder* pCoder = CCoder::GetCoder(pVehicle->m_strIVU);
	if(pCoder==nullptr)
		return 0;
	const bool oldAlarm = pVehicle->m_bAlarm;
	pVehicle->m_bAlarm = false;
	if(message->dwStatus!=0)
	{
		if(pCoder->IsAbnomal(message->dwStatus)==TRUE)
		{
			if(pCoder->IsAlarm(message->dwStatus)==TRUE)
			{
				pVehicle->m_bAlarm = true;

				static CString strMessage;
				strMessage.Format(_T("-> [%d] 车辆报警：%s"), message->m_dwVehicle, pCoder->GetStatusDescrip(message->dwStatus));
				this->SendMessage(WM_VEHICLEMESSAGE, message->m_dwVehicle, (LONG)&strMessage);

				if(pVehicle->m_bTrack==false)
				{
					this->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_TRACK, message->m_dwVehicle);
				}

				if(m_TablePane.m_pMonitorCtrl!=nullptr)
				{
					m_TablePane.m_pMonitorCtrl->MoveFirst(pVehicle->m_dwId);
					m_TablePane.m_pMonitorCtrl->VehicleStatus(pVehicle->m_dwId, 2);
				}
				m_VehiclePane.SetVehicle(message->m_dwVehicle);
			}
			else
			{
				static CString strMessage;
				strMessage.Format(_T("-> [%d] 车辆异常：%s"), message->m_dwVehicle, pCoder->GetStatusDescrip(message->dwStatus));
				this->SendMessage(WM_VEHICLEMESSAGE, message->m_dwVehicle, (LONG)&strMessage);

				if(m_TablePane.m_pMonitorCtrl!=nullptr)
				{
					m_TablePane.m_pMonitorCtrl->VehicleStatus(pVehicle->m_dwId, 1);
				}
			}
		}
		else
		{
			if(m_TablePane.m_pMonitorCtrl!=nullptr)
			{
				m_TablePane.m_pMonitorCtrl->VehicleStatus(pVehicle->m_dwId, 0);
			}
		}
	}
	else
	{
		if(m_TablePane.m_pMonitorCtrl!=nullptr)
		{
			m_TablePane.m_pMonitorCtrl->VehicleStatus(pVehicle->m_dwId, 0);
		}
	}

	if(message->m_dwVehicle==m_VehiclePane.m_dwVehicle)
	{
		m_VehiclePane.ShowStatus(pCoder, message->m_dwVehicle, message->dwStatus);
	}

	pVehicle->m_dwStatus = message->dwStatus;

	if(oldAlarm!=pVehicle->m_bAlarm)
	{
		if(pVehicle->m_bAlarm==true)
		{
			CVehicle::IncreaseAlarm();
		}
		else
		{
			CVehicle::DecreaseAlarm();
		}
	}
	return 1;
}

LONG CMainFrame::OnVehicleMessage(UINT WPARAM, LONG LPARAM)
{
	CString string = *(CString*)LPARAM;

	if(m_TablePane.m_pMessageCtrl!=nullptr)
	{
		m_TablePane.m_pMessageCtrl->AddMessage(string);
	}

	return 1;
}

LONG CMainFrame::OnVehicleDispatch(UINT WPARAM, LONG LPARAM)
{
	const CPointF geoPoint = *(CPointF*)WPARAM;
	const CSizeF geoSize = *(CSizeF*)LPARAM;
	if(m_pPort!=nullptr)
	{
		return m_pPort->Dispatch(geoPoint, geoSize);
	}
	else
		return 0;
}

LONG CMainFrame::OnChildDocChanged(UINT WPARAM, LONG LPARAM)
{
	CChildFrame* pChildFrm = (CChildFrame*)WPARAM;
	if(pChildFrm==nullptr)
		return 0;

	CDocument* pDocument = pChildFrm->GetActiveDocument();
	if(pDocument==nullptr)
		return 0;
	CGrfDoc* pDoc = (CGrfDoc*)pDocument;
	CString strMap = ((CGrfDoc*)pDocument)->m_Datainfo.GetName();
	POSITION pos = m_mapVehicle.GetStartPosition();
	while(pos!=nullptr)
	{
		DWORD dwId;
		CVehicle* pVehicle = nullptr;
		m_mapVehicle.GetNextAssoc(pos, dwId, pVehicle);
		if(pVehicle->m_pFrame==pChildFrm)
		{
			CView* pView = pChildFrm->GetActiveView();
			pVehicle->ViewDrawOver(pView, ((CGrfView*)pView)->m_viewMonitor);
		}
	}
	return 1;
}

LONG CMainFrame::OnViewWillDraw(UINT WPARAM, LONG LPARAM)
{
	CView* pView = (CView*)WPARAM;
	if(pView==nullptr)
		return 0;

	CChildFrame* pChildFrm = (CChildFrame*)pView->GetParentFrame();
	if(pChildFrm==nullptr)
		return 0;

	this->KillTimer(TIMER_VEHICLE);
	this->KillTimer(TIMER_REPLAY);
	this->KillTimer(TIMER_GETGPSMSE);

	if(m_Security.HasNavigateRight()==true)
	{
		m_Route.ViewWillDraw(pView, ((CGrfView*)pView)->m_viewMonitor);
	}

	POSITION pos = m_mapVehicle.GetStartPosition();
	while(pos!=nullptr)
	{
		DWORD dwId;
		CVehicle* pVehicle = nullptr;
		m_mapVehicle.GetNextAssoc(pos, dwId, pVehicle);
		if(pVehicle!=nullptr&&pVehicle->m_pFrame==pChildFrm)
		{
			pVehicle->ViewWillDraw(pView, ((CGrfView*)pView)->m_viewMonitor);
		}
	}

	return 1;
}

LONG CMainFrame::OnViewDrawOver(UINT WPARAM, LONG LPARAM)
{
	CView* pView = (CView*)WPARAM;
	if(pView==nullptr)
		return 0;

	CChildFrame* pChildFrm = (CChildFrame*)pView->GetParentFrame();
	if(pChildFrm==nullptr)
		return 0;

	POSITION pos = m_mapVehicle.GetStartPosition();
	while(pos!=nullptr)
	{
		DWORD dwId;
		CVehicle* pVehicle = nullptr;
		m_mapVehicle.GetNextAssoc(pos, dwId, pVehicle);
		if(pVehicle!=nullptr&&pVehicle->m_pFrame==pChildFrm)
		{
			pVehicle->ViewDrawOver(pView, ((CGrfView*)pView)->m_viewMonitor);
		}
	}

	if(pChildFrm->m_bMainChild==TRUE&&m_pReplay!=nullptr)
	{
		m_pReplay->Draw(pView);
	}

	if(m_Security.HasNavigateRight()==true)
	{
		m_Route.DrawMark(pView, ((CGrfView*)pView)->m_viewMonitor);
		m_Route.ViewDrawOver(pView, ((CGrfView*)pView)->m_viewMonitor);
	}

	CGrfDoc* pDoc = (CGrfDoc*)pView->GetDocument();
	if(m_geoSign.x!=0&&m_geoSign.y!=0)
	{
		Gdiplus::PointF cliPoint = ((CGrfView*)pView)->m_viewMonitor.WGS84ToClient(m_geoSign);
		CRect cliRect;
		pView->GetClientRect(cliRect);
		if(cliRect.PtInRect(CPoint(cliPoint.X, cliPoint.Y))==TRUE)
		{
			CClientDC dc(pView);
			cliPoint.X -= 16;
			cliPoint.Y -= 16;
			CImageList imagelist;
			imagelist.Create(IDB_RECMARK, 32, 0, RGB(255, 0, 255));
			imagelist.Draw(&dc, 0, CPoint(cliPoint.X, cliPoint.Y), ILD_TRANSPARENT);
			imagelist.DeleteImageList();
			imagelist.Detach();
		}
	}

	if(m_mapVehicle.GetCount()>0)
	{
		this->SetTimer(TIMER_VEHICLE, 500, nullptr);
	}
	if(m_pReplay!=nullptr)
	{
		const long nReplaySpeed = AfxGetApp()->GetProfileInt(_T("Socket"), _T("ReplaySpeed"), 200);
		this->SetTimer(TIMER_REPLAY, nReplaySpeed, nullptr);
	}
	if(m_pPort!=nullptr)
	{
		this->SetTimer(TIMER_GETGPSMSE, 500, nullptr);
	}

	return 1;
}

LONG CMainFrame::OnGetVehicle(UINT WPARAM, LONG LPARAM)
{
	switch(LPARAM)
	{
	case 0:
	{
		CVehicle* pVehicle = nullptr;
		if(m_mapVehicle.Lookup(WPARAM, pVehicle)==TRUE)
			return (long)pVehicle;
	}
	break;
	case 1:
	{
		POSITION pos = m_mapVehicle.GetStartPosition();
		while(pos!=nullptr)
		{
			DWORD dwId;
			CVehicle* pVehicle = nullptr;
			m_mapVehicle.GetNextAssoc(pos, dwId, pVehicle);
			if(pVehicle!=nullptr&&(DWORD)(pVehicle->m_pFrame)==WPARAM)
				return (long)pVehicle;
		}
	}
	break;
	default:
		return 0;
		break;
	}

	return 0;
}

LONG CMainFrame::OnActivateVehicle(UINT WPARAM, LONG LPARAM)
{
	m_NavinfoPane.m_pNavigateDlg->SetID(WPARAM);

	CVehicle* pVehicle = nullptr;
	m_mapVehicle.Lookup(WPARAM, pVehicle);
	if(pVehicle!=nullptr)
	{
		if(pVehicle->m_bTrack==TRUE)
		{
			pVehicle->m_pFrame->ActivateFrame();
		}
		else
		{
			CView* pView = pVehicle->m_pFrame->GetActiveView();
			if(pView!=nullptr)
			{
				CPointF geoPoint = pVehicle->m_geoCurrent;
				if(pView->SendMessage(WM_GEOPOINTINMAP, (UINT)&geoPoint, 0)==TRUE)
				{
					const Gdiplus::PointF point = ((CGrfView*)pView)->m_viewMonitor.WGS84ToClient(geoPoint);

					CRect cliRect;
					pView->GetClientRect(cliRect);
					if(cliRect.PtInRect(CPoint(point.X, point.Y))==FALSE)
					{
						CPoint docPoint = ((CGrfView*)pView)->m_viewMonitor.m_datainfo.GeoToDoc(geoPoint);
						CPoint cliPoint = cliRect.CenterPoint();
						pView->SendMessage(WM_FIXATEVIEW, (UINT)&docPoint, (LONG)&cliPoint);
					}
				}
			}
		}

		if(m_VehiclePane.m_dwVehicle!=WPARAM)
		{
			CCoder* pCoder = CCoder::GetCoder(pVehicle->m_strIVU);
			if(pCoder!=nullptr)
			{
				m_VehiclePane.ShowStatus(pCoder, WPARAM, pVehicle->m_dwStatus);
			}
		}
	}

	return 0;
}

LONG CMainFrame::OnGetPort(UINT WPARAM, LONG LPARAM)
{
	return (LONG)m_pPort;
}

LONG CMainFrame::OnBrowseHtml(UINT WPARAM, LONG LPARAM)
{
	if(WPARAM==0)
		return 0;
	const CString strHtml = *(CString*)WPARAM;
	if(strHtml.GetLength()<5)
		return 0;

	CRect rect;
	this->GetClientRect(rect);
	if(LPARAM!=0)
	{
		CFrameWnd* pFrameWnd = this->GetActiveFrame();
		if(pFrameWnd!=this)
		{
			CRect rect1;
			pFrameWnd->GetClientRect(rect1);
			pFrameWnd->ClientToScreen(rect1);
			this->ScreenToClient(rect1);

			rect1.right = rect.right;
			rect1.bottom = rect.bottom;
			rect = rect1;
		}
		else
		{
			rect.top += 42;
		}
	}

	if(m_pHtmlView==nullptr)
	{
		m_pHtmlView = new CHtmlView;
		m_pHtmlView->Create(nullptr, nullptr, WS_CHILD|WS_VISIBLE, rect, this, AFX_IDW_PANE_FIRST, nullptr);
	}
	if(m_pHtmlView!=nullptr)
	{
		m_pHtmlView->Navigate(strHtml);
		m_pHtmlView->SetWindowPos(&wndTop, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
		/*	if(m_pEaglePane != nullptr)
		{
		m_pEaglePane->ShowWindow(SW_HIDE);
		m_pEaglePane->UpdateWindow();
		}*/
	}
	return 0;
}

LONG CMainFrame::OnDestroyHtml(UINT WPARAM, LONG LPARAM)
{
	if(m_pHtmlView!=nullptr)
	{
		m_pHtmlView->PostMessage(WM_DESTROY, 0, 0);
		m_pHtmlView->DestroyWindow();
		delete m_pHtmlView;
		m_pHtmlView = nullptr;
	}

	return 1;
}

LONG CMainFrame::OnSetGeoSign(UINT WPARAM, LONG LPARAM)
{
	if(WPARAM!=0)
		m_geoSign = *(CPointF*)WPARAM;

	return 1;
}

void CMainFrame::RecalcLayout(BOOL bNotify)
{
	bool bChildFrame = false;
	CRect rect;
	if(m_pHtmlView!=nullptr)
	{
		m_pHtmlView->GetClientRect(rect);
		m_pHtmlView->ClientToScreen(rect);
		this->ScreenToClient(rect);

		if(rect.left!=0)
			bChildFrame = true;
	}

	CMDIFrameWndEx::RecalcLayout(bNotify);

	if(m_pHtmlView!=nullptr)
	{
		CRect rect1;
		this->GetClientRect(rect1);

		rect.right = rect1.right;
		rect.bottom = rect1.bottom;
		if(bChildFrame==true)
		{
			CFrameWnd* pFrameWnd = this->GetActiveFrame();
			if(pFrameWnd!=this)
			{
				CRect rect1;
				pFrameWnd->GetClientRect(rect1);
				pFrameWnd->ClientToScreen(rect1);
				this->ScreenToClient(rect1);

				rect.left = rect1.left;
			}
		}

		m_pHtmlView->SetWindowPos(&wndTop, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	}
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if(m_pHtmlView!=nullptr)
	{
		CFrameWnd* pChild = this->GetActiveFrame();

		switch(wParam)
		{
		case ID_GO_HOME:
			if(pChild!=this&&pChild!=nullptr)
			{
				m_pHtmlView->PostMessage(WM_DESTROY, 0, 0);
				m_pHtmlView->DestroyWindow();
				delete m_pHtmlView;
				m_pHtmlView = nullptr;

				return TRUE;
			}
			else if(m_pAtlas!=nullptr)
			{
				CString strHomePage = m_pAtlas->m_strHomePage;
				if(IsWebFile(strHomePage)==TRUE)
				{
					this->SendMessage(WM_BROWSEHTML, (UINT)&strHomePage, 0);
				}
			}
			break;
		case ID_GO_BACK:
			if(m_pHtmlView->m_bCanBack==TRUE)
				m_pHtmlView->GoBack();
			else if(pChild!=this&&pChild!=nullptr)
			{
				m_pHtmlView->PostMessage(WM_DESTROY, 0, 0);
				m_pHtmlView->DestroyWindow();
				delete m_pHtmlView;
				m_pHtmlView = nullptr;
			}
			return TRUE;
			break;
		case ID_GO_FORWARD:
			m_pHtmlView->GoForward();
			return TRUE;
			break;
		default:
			break;
		}
	}

	return CMDIFrameWndEx::OnCommand(wParam, lParam);
}

LONG CMainFrame::OnMatchMap(UINT WPARAM, LONG LPARAM)
{
	CChildFrame* pChildFrame = (CChildFrame*)WPARAM;
	if(pChildFrame==nullptr)
	{
		pChildFrame = (CChildFrame*)this->SendMessage(WM_GETMAINCHILD, 0, 0);
	}
	if(pChildFrame==nullptr)
		return 0;

	if(LPARAM==0)
		return 0;

	CPointF geoPoint = *(CPointF*)LPARAM;
	CView* pView = nullptr;
	if(m_pAtlas!=nullptr)
	{
		const CGrfDoc* pDoc = (CGrfDoc*)pChildFrame->GetActiveDocument();
		CDeck* pDeck = m_pAtlas->m_Pyramid.GetDeck(geoPoint);
		if(pDeck!=nullptr&&pDeck!=pDoc->m_pDeck)
		{
			pDeck->Open(pChildFrame, 0);
		}
		pView = pChildFrame->GetActiveView();
	}
	else
	{
		pView = pChildFrame->GetActiveView();
	}

	if(pView!=nullptr)
	{
		CPoint docPoint = ((CGrfView*)pView)->m_viewMonitor.m_datainfo.GeoToDoc(geoPoint);

		CPoint cliPoint;
		cliPoint.x = AfxGetApp()->GetProfileInt(_T("NewView"), _T("CliX"), 0);
		cliPoint.y = AfxGetApp()->GetProfileInt(_T("NewView"), _T("CliY"), 0);
		if(cliPoint==CPoint(0, 0))
		{
			CRect rect;
			pView->GetClientRect(rect);
			cliPoint = rect.CenterPoint();
		}

		pView->SendMessage(WM_FIXATEVIEW, (UINT)&docPoint, (LONG)&cliPoint);
		AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"), _T(""));
		AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"), _T(""));
		AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"), 0);
		AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"), 0);
		return (LONG)pView;
	}
	else
		return 0;
}

LONG CMainFrame::OnGetMainChild(UINT WPARAM, LONG LPARAtM)
{
	CWnd* pTempWnd = this->GetWindow(GW_CHILD);
	while(pTempWnd!=nullptr)
	{
		const CRuntimeClass* pRuntimeClass = pTempWnd->GetRuntimeClass();
		if(strcmp(pRuntimeClass->m_lpszClassName, "CMDIClientAreaWnd")==0)
			break;

		pTempWnd = pTempWnd->GetNextWindow(GW_HWNDNEXT);
	}

	if(pTempWnd!=nullptr)
	{
		CWnd* pChildFrame = pTempWnd->GetWindow(GW_CHILD);
		while(pChildFrame!=nullptr)
		{
			if(pChildFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
			{
				const BOOL bMainChild = pChildFrame->SendMessage(WM_ISMAINCHILD, 0, 0);
				if(bMainChild==TRUE)
				{
					return (LONG)pChildFrame;
				}
			}
			pChildFrame = pChildFrame->GetWindow(GW_HWNDNEXT);
		}
	}

	return 0;
}

LRESULT CMainFrame::OnURLChanged(WPARAM wp, LPARAM lp)
{
	CString strURL = *(CString*)wp;
	const CString strPath = *(CString*)lp;
	if(strURL.GetLength()<=11||strURL.Left(11)!=_T("SAMSON:COM="))
		return 0;

	strURL = strURL.Mid(11);
	const int nChars = strURL.GetLength();

	if(nChars>=4&&strURL.Left(4)==_T("EXIT"))
	{
		this->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
	}
	else if(nChars>=4&&strURL.Left(4)==_T("HELP"))
	{
		this->PostMessage(WM_COMMAND, ID_APP_ABOUT, 0);
	}
	else if(nChars>=4&&strURL.Left(4)==_T("HTML"))
	{
		strURL = strURL.Mid(6);
		AfxToFullPath(strPath, strURL);

		this->SendMessage(WM_BROWSEHTML, (UINT)&strURL, 1);
	}
	else if(nChars>=11&&strURL.Left(11)==_T("RETURNTOMAP"))
	{
		if(m_pHtmlView!=nullptr)
		{
			m_pHtmlView->PostMessage(WM_DESTROY, 0, 0);
			m_pHtmlView->DestroyWindow();
			delete m_pHtmlView;
			m_pHtmlView = nullptr;
		}
	}
	else
	{
		if(m_pHtmlView!=nullptr)
		{
			m_pHtmlView->PostMessage(WM_DESTROY, 0, 0);
			m_pHtmlView->DestroyWindow();
			delete m_pHtmlView;
			m_pHtmlView = nullptr;
		}

		CString strFile;
		if(nChars>=3&&strURL.Left(3)==_T("GRF"))
		{
			strFile = strURL.Mid(5);
			if(strFile.IsEmpty()==FALSE)
			{
				AfxToFullPath(strPath, strFile);

				AfxGetApp()->OpenDocumentFile(strFile);
			}
		}

		if(strFile.IsEmpty()==TRUE&&m_pAtlas!=nullptr)
		{
			CString strFile = m_pAtlas->m_strHomePage;
			strFile.MakeLower();
			if(strFile.Right(4)==_T(".gis"))
			{
				AfxGetApp()->OpenDocumentFile(strFile);
			}
			else if(m_pAtlas->m_Pyramid.m_pApex!=nullptr)
			{
				CFrameWnd* pChild = this->GetActiveFrame();
				m_pAtlas->m_Pyramid.m_pApex->Open(pChild, 0);
			}
		}

		if(nChars>=3&&strURL.Left(3)==_T("GRF"))
		{
			this->ActivePane('A');
		}
		else if(nChars>=7&&strURL.Left(7)==_T("TRAFFIC"))
		{
		}
		else if(nChars>=8&&strURL.Left(8)==_T("NAVIGATE"))
		{
			if(m_pAtlas!=nullptr&&m_Security.HasNavigateRight()==true)
			{
				this->ActivePane('G');

				this->SendMessage(WM_COMMAND, ID_GPS_TRACE, 0);
			}
			else
			{
				AfxMessageBox(IDS_NOTSUPPORTGPS);
			}
		}
		else if(nChars>=5&&strURL.Left(5)==_T("QUERY"))
		{
			this->ActivePane('Q');
		}
		else if(nChars>=4&&strURL.Left(4)==_T("INFO"))
		{
			this->ActivePane('C');
		}
		else if(nChars>=4&&strURL.Left(4)==_T("POU"))
		{
			this->ActivePane('P');
		}
		else
			return 0;
	}

	return 1;
}

LONG CMainFrame::OnFlashGeomtry(UINT WPARAM, LONG LPARAM)
{
	if(WPARAM==3&&LPARAM!=0)
	{
		const PLACE* place = (PLACE*)WPARAM;
		this->SendMessage(WM_DESTROYHTML, 0, 0);

		CDocument* pDocument = nullptr;
		if(m_pAtlas!=nullptr)
		{
			CDeck* pDeck = m_pAtlas->m_Pyramid.GetDeck(place->wMap);
			if(pDeck!=nullptr)
			{
				CFrameWnd* pMainChild = (CFrameWnd*)this->SendMessage(WM_GETMAINCHILD, 0, 0);
				pDocument = pDeck->Open(pMainChild, WPARAM);
			}
		}
		else
		{
			CFrameWnd* pFrameWnd = this->GetActiveFrame();
			if(pFrameWnd!=nullptr)
			{
				pDocument = pFrameWnd->GetActiveDocument();
			}
		}

		if(pDocument!=nullptr)
		{
			POSITION pos = pDocument->GetFirstViewPosition();
			while(pos!=nullptr)
			{
				CView* pView = pDocument->GetNextView(pos);
				if(pView->IsKindOf(RUNTIME_CLASS(CGrfView))==FALSE)
					continue;

				if(place->dLng!=0.0f&&place->dLat!=0.0f)
				{
					CPointF geoPoint = CPointF(place->dLng, place->dLat);
					CPoint docPoint = ((CGrfView*)pView)->m_viewMonitor.m_datainfo.GeoToDoc(geoPoint);

					CRect cliRect;
					pView->GetClientRect(cliRect);
					CPoint cliPoint = cliRect.CenterPoint();

					pView->SendMessage(WM_FIXATEVIEW, (UINT)&docPoint, (LONG)&cliPoint);
					m_geoSign = geoPoint;
				}
				else
				{
					pView->SendMessage(WM_FLASHGEOM, WPARAM, LPARAM);
				}
			}
		}
	}

	return 0;
}

LONG CMainFrame::OnSearchRoute(UINT WPARAM, LONG LPARAM)
{
	if(m_pAtlas==nullptr)
		return 0;

	CPointF headPoint = m_Route.m_ptStart;
	CPointF tailPoint = m_Route.m_ptEnd;

	if(m_pAtlas->m_pTopo==nullptr)
	{
		CString strTopo = m_pAtlas->m_strFilePath;
		strTopo = strTopo.Left(strTopo.ReverseFind(_T('.')));

		CRoadTopo* pRoadTopo = new CRoadTopo();
		if(pRoadTopo->Load(strTopo)==true)
		{
			m_pAtlas->m_pTopo = pRoadTopo;
		}
		else
		{
			delete pRoadTopo;
			pRoadTopo = nullptr;

			CString strMes;
			strMes.LoadString(IDS_FILECANNOTOPEN);
			strMes.Replace(_T("%s"), strTopo);
			AfxMessageBox(strMes);
		}
	}
	if(m_pAtlas->m_pTopo!=nullptr)
	{
		CTypedPtrList<CObList, CLayer*>* layerlist = nullptr;
		if(m_Route.Search(m_pAtlas->m_pTopo)==true)
		{
			const float length = m_Route.GetShortcutLength();
			if(length!=-1.0f)
			{
				CString str;
				str.Format(_T("%.2f"), length);
				str.TrimRight(_T("0"));
				str.TrimRight(_T("."));
				CString string;
				string.LoadString(IDS_OPTIMIZEDLENGTH);
				string.Replace(_T("%s"), str);
				AfxMessageBox(string);
			}
			else
			{
				const float length = m_Route.GetBeelineLength();
				CString str;
				str.Format(_T("%.2f"), length);
				str.TrimRight(_T("0"));
				str.TrimRight(_T("."));
				CString string;
				string.LoadString(IDS_BEELENGTH);
				string.Replace(_T("%s"), str);
				AfxMessageBox(string);
			}

			this->ActivePane('G');
			m_NavinfoPane.m_pNavigateDlg->SetRouteLength(length);

			CDeck* pDeck = m_pAtlas->m_Pyramid.GetNavyDeck(headPoint, tailPoint);
			if(pDeck!=nullptr)
			{
				CFrameWnd* pMainChild = (CFrameWnd*)this->SendMessage(WM_GETMAINCHILD, 0, 0);
				CDocument* pDocument = pDeck->Open(pMainChild, 0);
				const CView* pView = nullptr;
				if(pDocument!=nullptr)
				{
					POSITION pos = pDocument->GetFirstViewPosition();
					if(pos!=nullptr)
					{
						pView = pDocument->GetNextView(pos);
					}
				}
			}
		}
		else
		{
			AfxMessageBox(IDS_NODATAFRONAVIGATION);
		}
	}

	return 0;
}

LONG CMainFrame::OnGetPane(UINT WPARAM, LONG LPARAM)
{
	switch(WPARAM)
	{
	case 'L':
		return (LONG)&m_LayerPane;
		break;
	case 'A':
		return (LONG)&m_AtlasPane;
		break;
	case 'T':
		break;
	case 'Q':
		return (LONG)&m_QueryPane;
		break;
		break;
	case 'P':
		return (LONG)&m_POUPane;
		break;
	case 'O':
		return (LONG)&m_TablePane;
		break;
	default:
		return 0;
		break;
	}
	return 0;
}
void CMainFrame::OnLevelChanged(float fLevel)
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar1;
	m_wndRibbonBar.GetElementsByID(ID_MAP_LEVELSLIDER, ar1);

	for(int i = 0; i < ar1.GetSize(); i++)
	{
		CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, ar1[i]);
		if(pSlider != nullptr)
		{
			const int nMax = pSlider->GetRangeMax();
			const int nMin = pSlider->GetRangeMin();
			const int nPos = nMax - fLevel + nMin;
			if(pSlider->GetPos() != nPos)
			{
				pSlider->SetPos(nPos);
			}
		}
	}

	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar2;
	m_wndRibbonBar.GetElementsByID(ID_MAP_LEVELEDIT, ar2);

	for(int i = 0; i < ar2.GetSize(); i++)
	{
		CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, ar2[i]);
		if(pEdit != nullptr)
		{
			CString str;
			str.Format(_T("%.2f"), fLevel);
			str.TrimRight(_T("0"));
			str.TrimRight(_T("."));
			pEdit->SetEditText(str);
		}
	}
}
void CMainFrame::OnScaleChanged(float fScale)
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar1;
	m_wndRibbonBar.GetElementsByID(ID_MAP_SCALESLIDER, ar1);

	for(int i = 0; i<ar1.GetSize(); i++)
	{
		CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, ar1[i]);
		if(pSlider!=nullptr)
		{
			const int nMax = pSlider->GetRangeMax();
			const int nMin = pSlider->GetRangeMin();
			const int nPos = nMax-fScale+nMin;
			if(pSlider->GetPos()!=nPos)
			{
				pSlider->SetPos(nPos);
			}
		}
	}

	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar2;
	m_wndRibbonBar.GetElementsByID(ID_MAP_SCALEEDIT, ar2);

	for(int i = 0; i<ar2.GetSize(); i++)
	{
		CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, ar2[i]);
		if(pEdit!=nullptr)
		{
			CString str;
			str.Format(_T("%.2f"), fScale);
			str.TrimRight(_T("0"));
			str.TrimRight(_T("."));
			pEdit->SetEditText(str);
		}
	}
}

void CMainFrame::OnPaneAtlas()
{
	m_AtlasPane.ShowPane(!m_AtlasPane.IsVisible(), FALSE, TRUE);
}

void CMainFrame::OnPaneLayer()
{
	m_LayerPane.ShowPane(!m_LayerPane.IsVisible(), FALSE, TRUE);
}

void CMainFrame::OnPanePOU()
{
	m_POUPane.ShowPane(!m_POUPane.IsVisible(), FALSE, TRUE);
}

void CMainFrame::OnPaneQuery()
{
	m_QueryPane.ShowPane(!m_QueryPane.IsVisible(), FALSE, TRUE);
}

void CMainFrame::OnPaneVehicle()
{
	m_VehiclePane.ShowPane(!m_VehiclePane.IsVisible(), FALSE, TRUE);
}

void CMainFrame::OnPaneNavigate()
{
	m_VehiclePane.ShowPane(!m_VehiclePane.IsVisible(), FALSE, TRUE);
}

void CMainFrame::OnPaneTable()
{
	m_TablePane.ShowPane(!m_TablePane.IsVisible(), FALSE, TRUE);
}

void CMainFrame::OnUpdatePaneAtlas(CCmdUI* pCmdUI)
{
	if(m_AtlasPane.m_hWnd==nullptr)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->SetCheck(m_AtlasPane.IsVisible());
}

void CMainFrame::OnUpdatePaneLayer(CCmdUI* pCmdUI)
{
	if(m_LayerPane.m_hWnd==nullptr)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->SetCheck(m_LayerPane.IsVisible());
}

void CMainFrame::OnUpdatePanePOU(CCmdUI* pCmdUI)
{
	if(m_POUPane.m_hWnd==nullptr)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->SetCheck(m_POUPane.IsVisible());
}

void CMainFrame::OnUpdatePaneQuery(CCmdUI* pCmdUI)
{
	if(m_QueryPane.m_hWnd==nullptr)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->SetCheck(m_QueryPane.IsVisible());
}

void CMainFrame::OnUpdatePaneTable(CCmdUI* pCmdUI)
{
	if(m_TablePane.m_hWnd==nullptr)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->SetCheck(m_TablePane.IsVisible());
}

void CMainFrame::OnUpdatePaneNavinfo(CCmdUI* pCmdUI)
{
	if(m_VehiclePane.m_hWnd==nullptr)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->SetCheck(m_VehiclePane.IsVisible());
}

void CMainFrame::OnUpdatePaneVehicle(CCmdUI* pCmdUI)
{
	if(m_VehiclePane.m_hWnd==nullptr)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->SetCheck(m_VehiclePane.IsVisible());
}
