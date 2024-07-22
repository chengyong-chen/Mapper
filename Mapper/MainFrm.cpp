#include "stdafx.h"
#include <Vfw.h>
#include <cstdio>
#include <cstdlib>
#include <Windows.h>
#include "Global.h"
#include "GrfDoc.h"
#include "GisDoc.h"
#include "GrfView.h"
#include "GisView.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "AtlasNewDlg.h"
#include "PreferDlg.h"

#include "../Viewer/Global.h"
#include "../Projection/Projection1.h"

#include "../Dataview/Background.h"

#include "../Atlas/Atlas1.h"
#include "../Atlas/AtlasPane.h"

#include "../Layer/LayerPane.h"
#include "../Layer/BackgroundBar.h"

#include "../Information/Global.h"
#include "../Information/TDBGridCtrl.h"
#include "../Information/POUDBGridCtrl.h"
#include "../Information/QueryPane.h"
#include "../information/POUListCtrl.h"
#include "../Information/POUPane.h"
#include "../information/TablePane.h"
#include "../Information/POUTDBGridCtrl.h"

#include "../Topology/TopoPane.h"
#include "../Topology/RoadNodeForm.h"
#include "../Topology/RoadEdgeForm.h"

#include "../Public/PropertyPane.h"

#include "../Tool/DrawToolWnd.h"
#include "../Atlas/resource.h"
#include "../Navigate/RoadTopo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern UNIT a_UnitArray[3];
extern BYTE a_nUnitIndex;
extern long a_nGreekType;
extern long a_nUndoNumber;

extern __declspec(dllimport) BYTE d_nUnitIndex;

//used for dockablepane to reset it's pointer after close->destroy
const UINT WM_RESETMEMBER = ::RegisterWindowMessage(_T("WM_RESETMEMBER"));

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_FILE_PREFERENCES, OnFilePreferences)
	ON_COMMAND(ID_ATLAS_NEW, OnAtlasNew)
	ON_COMMAND(ID_ATLAS_OPEN, OnAtlasOpen)
	ON_COMMAND(ID_ATLAS_SAVE, OnAtlasSave)
	ON_COMMAND(ID_ATLAS_CLOSE, OnAtlasClose)
	ON_COMMAND(ID_VIEW_DRAWTOOL, OnViewDrawTool)
//	ON_COMMAND_RANGE(ID_VIEW_TOOLBAR1,ID_VIEW_TOOLBAR9, OnViewToolbar)
	ON_COMMAND_RANGE(ID_VIEW_PANE01, ID_VIEW_PANE10, OnViewPane)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGING_ACTIVE_TAB, OnChangingActiveTab)
	ON_REGISTERED_MESSAGE(AFX_WM_ON_PRESS_CLOSE_BUTTON, OnClosePane)
	ON_REGISTERED_MESSAGE(WM_RESETMEMBER, OnResetMember)
	ON_MESSAGE(WM_CROSSTHREAD, OnCrossThread)
	ON_MESSAGE(WM_KEYQUERY, OnKeyQuery)
	ON_MESSAGE(WM_GETQUERYCTRL, OnGetQueryCtrl)
	ON_MESSAGE(WM_GETPANE, OnGetPane)
	ON_MESSAGE(WM_FLASHGEOM, OnFlashGeomtry)
	ON_MESSAGE(WM_MATCHMAP, OnMatchMap)
//	ON_COMMAND(ID_WINDOW_RESETLAYOUT, &CMainFrame::OnResetwindowLayout) not working yet
END_MESSAGE_MAP()
BEGIN_EVENTSINK_MAP(CMainFrame, CMDIFrameWndEx)
	//{{AFX_EVENTSINK_MAP(CMainFrame)		-
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

CMainFrame::CMainFrame()
	: m_LayerPane(true), m_POUPane(true), m_TablePane(true)
{
	m_pAtlas = nullptr;

	a_nUnitIndex = AfxGetApp()->GetProfileInt(_T("Settings"), _T("UnitIndex"), 1);
	a_nGreekType = AfxGetApp()->GetProfileInt(_T("Settings"), _T("GreekType"), 5);
	a_nUndoNumber = AfxGetApp()->GetProfileInt(_T("Settings"), _T("Undo"), 8);

	d_nUnitIndex = a_nUnitIndex;
	CMFCPopupMenu::SetForceShadow(TRUE);
}
CMainFrame::~CMainFrame()
{
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
#endif

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

	return CMDIFrameWndEx::PreCreateWindow(cs);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CMDIFrameWndEx::OnCreate(lpCreateStruct)==-1)
		return -1;

	CDockingManager::SetDockingMode(DT_SMART);
	//	CDockablePane::SetCaptionStyle(TRUE,FALSE,TRUE);
	CMFCToolBarComboBoxButton::SetFlatMode();
	CMFCToolBarComboBoxButton::SetCenterVert();
	CMFCButton::EnableWindowsTheming();
	CMFCToolBar::EnableQuickCustomization(TRUE);
	CMDIFrameWndEx::EnableAutoHidePanes(CBRS_ALIGN_ANY);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
	mdiTabParams.m_bActiveTabCloseButton = TRUE; // set to FALSE to place close button at right of tab area
	mdiTabParams.m_bTabIcons = FALSE; // set to TRUE to enable document icons on MDI taba
	mdiTabParams.m_bAutoColor = TRUE; // set to FALSE to disable auto-coloring of MDI tabs
	mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
	mdiTabParams.m_bTabCustomTooltips = TRUE;
	CMDIFrameWndEx::EnableMDITabbedGroups(TRUE, mdiTabParams);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	CMFCRibbonComboBox* pProjectionCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_PREVIEW_PROJECTION));
	if(pProjectionCombo!= nullptr)
	{
	//	pProjectionCombo->EnableDropDownListResize(FALSE);
		pProjectionCombo->AddItem(_T("None"), 0);
		for(int index = 0; CGeocentric::ProjTypes[index].key != nullptr; index++)
		{
			const int item = pProjectionCombo->AddItem(CString(CGeocentric::ProjTypes[index].name), (DWORD)&CGeocentric::ProjTypes[index]);
		}
	}
	if(m_wndStatusBar.Create(this)==TRUE)
	{
		m_wndStatusBar.EnableWindow(TRUE);
	}

	CDockingManager::SetDockingMode(DT_SMART);// enable Visual Studio 2005 style docking window behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

	if(m_wndDrawTool.Create(this, WS_CHILD|WS_VISIBLE, 1111)==TRUE)
	{
		DWORD dwExtStyle = ::GetWindowLong(m_wndDrawTool.m_hWnd, GWL_EXSTYLE);
		dwExtStyle |= WS_EX_TOOLWINDOW;
		::SetWindowLong(m_wndDrawTool.m_hWnd, GWL_EXSTYLE, dwExtStyle);

		m_wndDrawTool.SetBorders(0, 0, 0, 0);
		m_wndDrawTool.SetWindowText(_T("Draw"));
		m_wndDrawTool.EnableDocking(FALSE);
	}

	if(m_AtlasPane.m_hWnd==nullptr)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Atlas.dll"));
		AfxSetResourceHandle(hInst);

		CString strName;
		strName.LoadString(IDS_PANE_ATLAS);
		if(m_AtlasPane.Create(strName, this, CRect(0, 0, 200, 150), TRUE, ID_VIEW_PANE01, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_LEFT|CBRS_FLOAT_MULTI)==TRUE)
		{
			m_AtlasPane.EnableDocking(CBRS_ALIGN_LEFT);
			RegCmdMsg(&m_AtlasPane);
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
		if(m_QueryPane.Create(strName, this, CRect(0, 0, 200, 150), TRUE, ID_VIEW_PANE06, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_LEFT|CBRS_FLOAT_MULTI)==TRUE)
		{
			m_QueryPane.EnableDocking(CBRS_ALIGN_LEFT);
			RegCmdMsg(&m_QueryPane);
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
		if(m_LayerPane.Create(strName, this, CRect(0, 0, 200, 150), TRUE, ID_VIEW_PANE02, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_LEFT|CBRS_FLOAT_MULTI)==TRUE)
		{
			m_LayerPane.EnableDocking(CBRS_ALIGN_LEFT);
			RegCmdMsg(&m_LayerPane);
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}

	if(m_TopoPane.m_hWnd==nullptr)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Topology.dll"));
		AfxSetResourceHandle(hInst);

		CString strName;
		strName.LoadString(IDS_PANE_TOPO);
		if(m_TopoPane.Create(strName, this, CRect(0, 0, 200, 150), TRUE, ID_VIEW_PANE08, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_LEFT|CBRS_FLOAT_MULTI)==TRUE)
		{
			UpdateMDITabbedBarsIcons();
			m_TopoPane.EnableToolTips(TRUE);
			m_TopoPane.EnableDocking(CBRS_ALIGN_LEFT);
			RegCmdMsg(&m_TopoPane);
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
		if(m_POUPane.Create(strName, this, CRect(0, 0, 200, 150), TRUE, ID_VIEW_PANE04, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_LEFT|CBRS_FLOAT_MULTI)==TRUE)
		{
			m_POUPane.EnableToolTips(TRUE);
			m_POUPane.EnableDocking(CBRS_ALIGN_LEFT);
			RegCmdMsg(&m_POUPane);
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
	if(m_OutputPane.m_hWnd==nullptr)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Logger.dll"));
		AfxSetResourceHandle(hInst);

		if(m_OutputPane.Create(_T("Message"), this, CRect(0, 0, 185, 200), TRUE, ID_VIEW_PANE10, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_BOTTOM|CBRS_FLOAT_MULTI)==TRUE)
		{
			m_OutputPane.SetControlBarStyle(m_OutputPane.GetControlBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY);
			UpdateMDITabbedBarsIcons();

			m_OutputPane.EnableDocking(CBRS_ALIGN_BOTTOM);
			RegCmdMsg(&m_OutputPane);
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
	if(m_TablePane.m_hWnd==nullptr)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
		AfxSetResourceHandle(hInst);

		CString strName;
		strName.LoadString(IDS_PANE_TABLE);
		if(m_TablePane.Create(strName, this, CRect(0, 0, 185, 200), TRUE, ID_VIEW_PANE05, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_BOTTOM|CBRS_FLOAT_MULTI)==TRUE)
		{
			m_TablePane.SetControlBarStyle(m_TablePane.GetControlBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY);
			UpdateMDITabbedBarsIcons();

			m_TablePane.EnableDocking(CBRS_ALIGN_BOTTOM);
			RegCmdMsg(&m_TablePane);
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}

	if(m_PropertyPane.m_hWnd==nullptr)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Public.dll"));
		AfxSetResourceHandle(hInst);

		CString strName;
		strName.LoadString(IDS_PANE_PROPERTY);
		if(m_PropertyPane.Create(strName, this, CRect(0, 0, 150, 400), TRUE, ID_VIEW_PANE07, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_RIGHT|CBRS_FLOAT_MULTI)==TRUE)
		{
			m_PropertyPane.SetControlBarStyle(m_PropertyPane.GetControlBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY);
			m_PropertyPane.EnableToolTips(TRUE);
			if(m_PropertyPane.m_hWnd!=nullptr)
			{
				const HINSTANCE hInstOld = AfxGetResourceHandle();
				HINSTANCE hInst = ::LoadLibrary(_T("Topology.dll"));
				AfxSetResourceHandle(hInst);

				CRoadNodeForm* pRoadNodeForm = new CRoadNodeForm(&m_PropertyPane);
				if(pRoadNodeForm->Create(IDD_ROADNODE, &m_PropertyPane)==TRUE)
				{
					pRoadNodeForm->ModifyStyle(0, WS_TABSTOP);

					pRoadNodeForm->ShowWindow(SW_HIDE);
					m_PropertyPane.m_mapForm[_T("N")] = pRoadNodeForm;
				}
				else
				{
					delete pRoadNodeForm;
					pRoadNodeForm = nullptr;
				}

				AfxSetResourceHandle(hInstOld);
				::FreeLibrary(hInst);
			}

			if(m_PropertyPane.m_hWnd!=nullptr)
			{
				const HINSTANCE hInstOld = AfxGetResourceHandle();
				HINSTANCE hInst = ::LoadLibrary(_T("Topology.dll"));
				AfxSetResourceHandle(hInst);

				CRoadEdgeForm* pRoadEdgeForm = new CRoadEdgeForm(&m_PropertyPane);
				if(pRoadEdgeForm->Create(IDD_ROADEDGE, &m_PropertyPane)==TRUE)
				{
					pRoadEdgeForm->ModifyStyle(0, WS_TABSTOP);
					pRoadEdgeForm->ShowWindow(SW_HIDE);
					m_PropertyPane.m_mapForm[_T("E")] = pRoadEdgeForm;
				}
				else
				{
					delete pRoadEdgeForm;
					pRoadEdgeForm = nullptr;
				}

				AfxSetResourceHandle(hInstOld);
				::FreeLibrary(hInst);
			}

			m_PropertyPane.EnableDocking(CBRS_ALIGN_RIGHT);
			RegCmdMsg(&m_PropertyPane);
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
//	InitUserToolbars(nullptr, AFX_IDW_CONTROLBAR_FIRST + 40, AFX_IDW_CONTROLBAR_FIRST + 40 + 10 - 1);	
//	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);
//	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."), ID_VIEW_TOOLBARS, TRUE, TRUE);// Enable toolbar and docking window menu replacement
	ModifyStyle(0, FWS_PREFIXTITLE);

	OnResetwindowLayout();
	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);
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
				CGrfView* pView = (CGrfView*)(((CMDIChildWnd*)pChildFrame)->GetActiveView());
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

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CMDIFrameWndEx::OnActivate(nState, pWndOther, bMinimized);
}

BOOL CMainFrame::OnMenuButtonToolHitTest(CMFCToolBarButton* pButton, TOOLINFO* pTI)
{
	ASSERT_VALID(pButton);
	ASSERT(pTI!=nullptr);
	if(pButton->m_nID==0||pButton->m_nID==(UINT)-1)
		return FALSE;

	CString strText = pButton->m_strText;
	strText.Remove(_T('&'));
	if(strText.IsEmpty())
		return FALSE;

	if(pTI!=nullptr)
	{
		pTI->lpszText = (LPTSTR) ::calloc((strText.GetLength()+1), sizeof(TCHAR));
		if(pTI->lpszText!=nullptr)
		{
			lstrcpy(pTI->lpszText, strText);
		}
	}

	return TRUE;
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

LRESULT CMainFrame::OnClosePane(WPARAM, LPARAM lp)
{
	//CPane* pPane = (CPane*)lp;
	//if(pPane!=nullptr && pPane->GetSafeHwnd()!=nullptr)
	//{
	//	int id = pPane->GetDlgCtrlID();
	//	pPane->ShowPane(FALSE, FALSE, FALSE);//hide pPane
	//	RemovePaneFromDockManager(pPane,TRUE,TRUE,TRUE,nullptr);//remove from manager
	//	//UnregCmdMsg(pPane);//unregister pPane from command routing list
	//	pPane->PostMessage(WM_CLOSE);//close and destroy
	//	PostMessage(WM_RESETMEMBER,id,0);//reset our member to null
	//	AdjustDockingLayout();//recalc docking layout
	//	return (LRESULT)TRUE;//prevent close , we already close it
	//}
	return FALSE;
}

LRESULT CMainFrame::OnResetMember(WPARAM wp, LPARAM)
{
	return (LRESULT)TRUE;
}

void CMainFrame::RegCmdMsg(CPane* pPane)
{
	m_regCmdMsg.AddTail(pPane);
}
//
void CMainFrame::UnregCmdMsg(CPane* pane)
{
	POSITION pos = m_regCmdMsg.GetHeadPosition();
	while(pos)
	{
		CBasePane* i = m_regCmdMsg.GetAt(pos);
		if(i==pane)
		{
			m_regCmdMsg.RemoveAt(pos);
			return;
		}
		m_regCmdMsg.GetNext(pos);
	}
}

void CMainFrame::ActivatePane(char cTab)
{
	CBasePane* pWnd = (CBasePane*)OnGetPane(cTab, 0);
	if(pWnd!=nullptr&&pWnd->IsPaneVisible()==FALSE)
	{
		CMDIFrameWndEx::ShowPane(pWnd, TRUE, FALSE, TRUE);
	}
}


void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

void CMainFrame::OnViewDrawTool()
{
	const BOOL bVisible = (m_wndDrawTool.GetStyle()&WS_VISIBLE)!=0;
	ShowControlBar(&m_wndDrawTool, !bVisible, FALSE);
}

LONG CMainFrame::OnCrossThread(UINT WPARAM, LONG LPARAM)
{
	switch(WPARAM)
	{
		case 1:
			CDocument*pDocument = (CDocument*)LPARAM;
			if(pDocument!=nullptr)
			{
				pDocument->UpdateAllViews(nullptr);
			}
			break;
	}

	return 0L;
}

void CMainFrame::OnViewPane(UINT nID)
{
	CDockablePane* pDockablePane = nullptr;
	switch(nID)
	{
		case ID_VIEW_PANE01:
			pDockablePane = &m_AtlasPane;
			break;
		case ID_VIEW_PANE02:
			pDockablePane = &m_LayerPane;
			break;
		case ID_VIEW_PANE04:
			pDockablePane = &m_POUPane;
			break;
		case ID_VIEW_PANE05:
			pDockablePane = &m_TablePane;
			break;
		case ID_VIEW_PANE06:
			pDockablePane = &m_QueryPane;
			break;
		case ID_VIEW_PANE07:
			pDockablePane = &m_PropertyPane;
			break;
		case ID_VIEW_PANE08:
			pDockablePane = &m_TopoPane;
			break;
		case ID_VIEW_PANE09:
			pDockablePane = &m_TopoPane;
			break;
		case ID_VIEW_PANE10:
			pDockablePane = &m_OutputPane;
			break;
	}
	if(pDockablePane!=nullptr)
	{
		const BOOL bVisibility = pDockablePane->IsVisible();
		pDockablePane->ShowPane(!bVisibility, FALSE, TRUE);
	}
}
BOOL CMainFrame::DestroyWindow()
{
	if(m_wndDrawTool.m_hWnd!=nullptr)
	{
		m_wndDrawTool.PostMessage(WM_DESTROY, 0, 0);
		m_wndDrawTool.DestroyWindow();
	}
	if(m_LayerPane.m_hWnd!=nullptr)
	{
		UnregCmdMsg(&m_LayerPane);
		m_LayerPane.PostMessage(WM_DESTROY, 0, 0);
		m_LayerPane.DestroyWindow();
	}
	if(m_AtlasPane.m_hWnd!=nullptr)
	{
		UnregCmdMsg(&m_AtlasPane);
		m_AtlasPane.PostMessage(WM_DESTROY, 0, 0);
		m_AtlasPane.DestroyWindow();
	}
	if(m_TopoPane.m_hWnd!=nullptr)
	{
		UnregCmdMsg(&m_TopoPane);
		m_TopoPane.PostMessage(WM_DESTROY, 0, 0);
		m_TopoPane.DestroyWindow();
	}
	if(m_QueryPane.m_hWnd!=nullptr)
	{
		UnregCmdMsg(&m_QueryPane);
		m_QueryPane.PostMessage(WM_DESTROY, 0, 0);
		m_QueryPane.DestroyWindow();
	}

	if(m_POUPane.m_hWnd!=nullptr)
	{
		UnregCmdMsg(&m_POUPane);
		m_POUPane.SetAtlCtrl((CPOUListCtrl*)nullptr);
		m_POUPane.PostMessage(WM_DESTROY, 0, 0);//this dosen't work ni
		m_POUPane.DestroyWindow();
	}
	if(m_PropertyPane.m_hWnd!=nullptr)
	{
		UnregCmdMsg(&m_PropertyPane);
		m_PropertyPane.PostMessage(WM_DESTROY, 0, 0);
		m_PropertyPane.DestroyWindow();
	}
	if(m_OutputPane.m_hWnd!=nullptr)
	{
		UnregCmdMsg(&m_OutputPane);
		m_OutputPane.PostMessage(WM_DESTROY, 0, 0);
		m_OutputPane.DestroyWindow();//this throw exception
	}
	if(m_TablePane.m_hWnd!=nullptr)
	{
		UnregCmdMsg(&m_TablePane);
		m_TablePane.PostMessage(WM_DESTROY, 0, 0);
		m_TablePane.DestroyWindow();//this throw exception
	}

	if(m_pAtlas!=nullptr)
	{
		delete m_pAtlas->m_pTopo;
		m_pAtlas->m_pTopo = nullptr;

		delete m_pAtlas;
		m_pAtlas = nullptr;
	}

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
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("UnitIndex"), a_nUnitIndex);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("GreekType"), a_nGreekType);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("Undo"), a_nUndoNumber);

	return CMDIFrameWndEx::DestroyWindow();
}

BOOL CMainFrame::OnCloseMiniFrame(CPaneFrameWnd* pWnd)
{
	return TRUE;
}

BOOL CMainFrame::OnCloseDockingPane(CDockablePane* pWnd)
{
	return TRUE;
}

void CMainFrame::OnFilePreferences()
{
	CPreferDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		CChildFrame* pChild = (CChildFrame*)GetActiveFrame();
		if(pChild!=nullptr)
		{
			CView* pView = (CView*)GetActiveView();
			if(pView!=nullptr)
			{
				/*CRuler* ruler = (CRuler*)pView->SendMessage(WM_GETRULER,0,0);
				if(ruler != nullptr)
				{
					ruler->Reset();
				}
				pChild->Invalidate(TRUE);	*/
			}
		}
	}
}

LONG CMainFrame::OnGetPane(UINT WPARAM, LONG LPARAM)
{
	switch(WPARAM)
	{
		case 'L':
			return (LONG)&m_LayerPane;
		case 'A':
			return (LONG)&m_AtlasPane;
		case 'T':
			return (LONG)&m_TopoPane;
		case 'Q':
			return (LONG)&m_QueryPane;
		case 'P':
			return (LONG)&m_POUPane;
		case 'O':
			return (LONG)&m_TablePane;
		case 'I':
			return (LONG)&m_PropertyPane;
		default:
			return 0;
	}
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	//POSITION pos = m_regCmdMsg.GetHeadPosition();
	//while (pos)
	//{
	//	CPane* pPane = m_regCmdMsg.GetAt(pos);
	//	if(pPane->m_hWnd != nullptr && pPane->IsVisible() && pPane->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
	//		return TRUE;
	//	m_regCmdMsg.GetNext(pos);
	//}

	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CMDIFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nID)
			{
				case ID_ATLAS_NEW:
				case ID_ATLAS_OPEN:
					pCmdUI->Enable(m_pAtlas==nullptr);
					return TRUE;
				case ID_ATLAS_SAVE:
					pCmdUI->Enable(m_pAtlas!=nullptr);
					return TRUE;
				case ID_ATLAS_CLOSE:
					pCmdUI->Enable(m_pAtlas!=nullptr);
					return TRUE;
				case ID_VIEW_DRAWTOOL:
					pCmdUI->SetCheck(m_wndDrawTool.IsVisible());
					break;				
				case ID_VIEW_PANE01:
					if(m_AtlasPane.m_hWnd==nullptr)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->SetCheck(m_AtlasPane.IsVisible());
					break;
				case ID_VIEW_PANE02:
					if(m_LayerPane.m_hWnd==nullptr)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->SetCheck(m_LayerPane.IsVisible());
					break;
				case ID_VIEW_PANE04:
					if(m_POUPane.m_hWnd==nullptr)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->SetCheck(m_POUPane.IsVisible());
					break;
				case ID_VIEW_PANE05:
					if(m_TablePane.m_hWnd==nullptr)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->SetCheck(m_TablePane.IsVisible());
					break;
				case ID_VIEW_PANE06:
					if(m_QueryPane.m_hWnd==nullptr)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->SetCheck(m_QueryPane.IsVisible());
					break;
				case ID_VIEW_PANE07:
					if(m_PropertyPane.m_hWnd==nullptr)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->SetCheck(m_PropertyPane.IsVisible());
					break;
				case ID_VIEW_PANE08:
					if(m_TopoPane.m_hWnd==nullptr)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->SetCheck(m_TopoPane.IsVisible());
					break;
				case ID_VIEW_PANE09:
					if(m_TopoPane.m_hWnd==nullptr)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->SetCheck(m_TopoPane.IsVisible());
					break;
				case ID_VIEW_PANE10:
					if(m_OutputPane.m_hWnd==nullptr)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->SetCheck(m_OutputPane.IsVisible());
					break;
				default:
					break;
			}
			pCmdUI->m_bContinueRouting = false;
		}
		if(nCode==CN_COMMAND)
		{
		}
	}

	return CMDIFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

LONG CMainFrame::OnMatchMap(UINT WPARAM, LONG LPARAM)
{
	if(LPARAM==0)
		return 0;

	CPointF geoPoint = *(CPointF*)LPARAM;
	geoPoint.x *= constants::degreeToradian;
	geoPoint.y *= constants::degreeToradian;
	CView* pView = nullptr;
	if(m_pAtlas!=nullptr)
	{
		CDeck* pDeck = m_pAtlas->m_Pyramid.GetDeck(geoPoint);
		if(pDeck!=nullptr)
		{
			CDocument* pDocument = pDeck->Open(nullptr, 0);
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
		CChildFrame* pChild = (CChildFrame*)GetActiveFrame();
		if(pChild!=nullptr)
		{
			pView = (CView*)pChild->GetActiveView();
		}
	}

	if(pView!=nullptr)
	{
		CPoint cliPoint;
		cliPoint.x = AfxGetApp()->GetProfileInt(_T("NewView"), _T("CliX"), 0);
		cliPoint.y = AfxGetApp()->GetProfileInt(_T("NewView"), _T("CliY"), 0);
		if(cliPoint==CPoint(0, 0))
		{
			CRect rect;
			pView->GetClientRect(rect);
			cliPoint = rect.CenterPoint();
		}

		CGrfDoc* pDoc = (CGrfDoc*)pView->GetDocument();
		if(pDoc!=nullptr&&pDoc->m_Datainfo.m_pProjection!=nullptr)
		{
			const CPointF mapPoint = pDoc->m_Datainfo.m_pProjection->GeoToMap(geoPoint);
			CPoint docPoint = pDoc->m_Datainfo.MapToDoc(mapPoint);

			pView->SendMessage(WM_FIXATEVIEW, (UINT)&docPoint, (LONG)&cliPoint);

			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"), nullptr);
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"), nullptr);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"), 0);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"), 0);
		}
		return (LONG)pView;
	}
	else
		return 0;
}

LONG CMainFrame::OnKeyQuery(UINT WPARAM, LONG LPARAM)
{
	CChildFrame* pChild = (CChildFrame*)GetActiveFrame();
	if(pChild==nullptr)
		return 0;

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
		_stscanf(string, _T("%X %X"), &wMap, &dwClass);
		CString strKey = string;
		strKey = strKey.Mid(14);

		count += m_pAtlas->KeyQuery(listCtrl, strKey, dwClass, wMap);
	}

	return 0;
}

LONG CMainFrame::OnGetQueryCtrl(UINT WPARAM, LONG LPARAM)
{
	return (LONG)&(m_QueryPane.m_KeyQueryEar.m_listCtrl);
}

LONG CMainFrame::OnFlashGeomtry(UINT WPARAM, LONG LPARAM)
{
	if(WPARAM==3)
	{
		CDocument* pDocument = nullptr;
		if(m_pAtlas!=nullptr)
		{
			PLACE* place = (PLACE*)LPARAM;
			if(place==nullptr)
				return 0;

			CDeck* pDeck = m_pAtlas->m_Pyramid.GetDeck(place->wMap);
			if(pDeck!=nullptr)
			{
				pDocument = pDeck->Open(nullptr, WPARAM);
			}
		}
		else
		{
			CFrameWnd* pFrameWnd = CMDIFrameWndEx::GetActiveFrame();
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
				const CView* pView = pDocument->GetNextView(pos);
				pView->SendMessage(WM_FLASHGEOM, WPARAM, LPARAM);
			}
		}
	}
	return 0L;
}

void CMainFrame::OnAtlasNew()
{
	OnAtlasClose();

	if(m_pAtlas!=nullptr)
		return;

	CAtlasNewDlg dlg(this);
	if(dlg.DoModal()==IDOK)
	{
		CAtlas* pAtlas = new CAtlas(&m_POUPane);
		if(dlg.m_nRadio==2)
		{
			const CString strFold = dlg.m_strPath;
			pAtlas->m_Catalog.Create(nullptr, strFold);
			pAtlas->m_Catalog.Filter();
			pAtlas->m_strFilePath = strFold+_T("\\Diwatu.Atl");
		}

		m_AtlasPane.Attach(pAtlas, true);
		m_pAtlas = pAtlas;
	}

	ActivatePane('A');
}

void CMainFrame::OnAtlasOpen()
{
	CFileDialog dlg(TRUE, _T("*.Atl"), 0, OFN_HIDEREADONLY, _T("Atlas File (*.Atl)|*.Atl||"), this);
	if(dlg.DoModal()==IDOK)
	{
		OnAtlasClose();

		if(m_pAtlas==nullptr)
		{
			m_pAtlas = new CAtlas(&m_POUPane);
			const CString strFile = dlg.GetPathName();
			if(m_pAtlas->Open(strFile, 0)==FALSE)
			{
				delete m_pAtlas;
				m_pAtlas = nullptr;
			}
		}
	}
	else
	{
		return;
	}

	if(m_pAtlas!=nullptr)
	{
		//		m_Security.SetTrafficRight(CMainFrame::m_pAtlas->m_bTraffic);
		//		m_Security.SetNavigateRight(CMainFrame::m_pAtlas->m_bNavigate);
		//		m_Security.SetHtmlRight(CMainFrame::m_pAtlas->m_bHtml);

		CMap<WORD, WORD&, CString, CString&> mapIdMap;
		POSITION posMap = m_pAtlas->m_Pyramid.m_DeckList.GetHeadPosition();
		while(posMap!=nullptr)
		{
			CDeck* pDeck = m_pAtlas->m_Pyramid.m_DeckList.GetNext(posMap);
			if(pDeck->m_bQueryble==FALSE)
				continue;

			CString strMap = pDeck->m_strName;
			strMap.Replace(_T("��ͼ"), nullptr);
			mapIdMap.SetAt(pDeck->m_wId, strMap);
		}
		CMap<DWORD, DWORD&, CString, CString&> mapIdClass;
		m_QueryPane.m_KeyQueryEar.AddMapCombo(mapIdMap);

		mapIdMap.RemoveAll();
		mapIdClass.RemoveAll();

		m_AtlasPane.Attach(m_pAtlas, true);

		this->ActivatePane('A');
		m_TablePane.ActivateTab('P');
	}

	ActivatePane('A');
}

void CMainFrame::OnAtlasSave()
{
	if(m_pAtlas!=nullptr)
	{
		m_pAtlas->Save();
	}
}

void CMainFrame::OnAtlasClose()
{
	m_AtlasPane.Detach(m_pAtlas);
	m_AtlasPane.m_pAtlas = nullptr;

	if(m_pAtlas!=nullptr)
	{
		if(AfxMessageBox(_T("Save the modified Atlas!"), MB_YESNO)==IDYES)
			OnAtlasSave();

		if(m_TablePane.m_pPOUDBGridCtrl!=nullptr)
		{
			m_TablePane.m_pPOUDBGridCtrl->Clear();
		}
		if(m_TablePane.m_pPOUTDBGridCtrl!=nullptr)
		{
			m_TablePane.m_pPOUTDBGridCtrl->SetDatabase(nullptr);
		}

		m_POUPane.SetAtlCtrl((CPOUListCtrl*)nullptr);

		delete m_pAtlas->m_pTopo;
		m_pAtlas->m_pTopo = nullptr;

		delete m_pAtlas;
		m_pAtlas = nullptr;
	}
}


void CMainFrame::OnResetwindowLayout()
{
	EnableDocking(CBRS_ALIGN_TOP);
	if(m_wndDrawTool.m_hWnd!=nullptr)
	{
		CFrameWnd::EnableDocking(CBRS_ALIGN_TOP);
		CMDIFrameWndEx::ShowControlBar(&m_wndDrawTool, TRUE, FALSE);
		CMDIFrameWndEx::FloatControlBar(&m_wndDrawTool, CPoint(130, 105), CBRS_ALIGN_TOP);
	}

	CDockablePane* leftTabbedBar = nullptr;
	EnableDocking(CBRS_ALIGN_LEFT);
	if(m_AtlasPane.m_hWnd!=nullptr)
	{
		if(leftTabbedBar==nullptr)
		{
			CMDIFrameWndEx::DockPane(&m_AtlasPane);
			leftTabbedBar = &m_AtlasPane;
		}
		else
		{
			CDockablePane* pTabbedBar = nullptr;
			m_AtlasPane.AttachToTabWnd(leftTabbedBar, DM_SHOW, TRUE, &pTabbedBar);
			leftTabbedBar = pTabbedBar;
		}
	}
	if(m_QueryPane.m_hWnd!=nullptr)
	{
		if(leftTabbedBar==nullptr)
		{
			CMDIFrameWndEx::DockPane(&m_QueryPane);
			leftTabbedBar = &m_QueryPane;
		}
		else
		{
			CDockablePane* pTabbedBar = nullptr;
			m_QueryPane.AttachToTabWnd(leftTabbedBar, DM_SHOW, TRUE, &pTabbedBar);
			leftTabbedBar = pTabbedBar;
		}
		m_QueryPane.ShowPane(TRUE, TRUE, TRUE);
	}
	if(m_LayerPane.m_hWnd!=nullptr)
	{
		if(leftTabbedBar==nullptr)
		{
			CMDIFrameWndEx::DockPane(&m_LayerPane);
			leftTabbedBar = &m_LayerPane;
		}
		else
		{
			CDockablePane* pTabbedBar = nullptr;
			m_LayerPane.AttachToTabWnd(leftTabbedBar, DM_SHOW, TRUE, &pTabbedBar);
			leftTabbedBar = pTabbedBar;
		}
		CMDIFrameWndEx::ShowPane(&m_LayerPane, TRUE, TRUE, TRUE);
	}
	if(m_TopoPane.m_hWnd!=nullptr)
	{
		EnableDocking(CBRS_ALIGN_LEFT);
		if(leftTabbedBar==nullptr)
		{
			CMDIFrameWndEx::DockPane(&m_TopoPane);
			leftTabbedBar = &m_TopoPane;
		}
		else
		{
			CDockablePane* pTabbedBar = nullptr;
			m_TopoPane.AttachToTabWnd(leftTabbedBar, DM_SHOW, TRUE, &pTabbedBar);
			leftTabbedBar = pTabbedBar;
		}
		m_TopoPane.ShowPane(FALSE, FALSE, FALSE);
	}
	if(m_POUPane.m_hWnd!=nullptr)
	{
		if(leftTabbedBar==nullptr)
		{
			CMDIFrameWndEx::DockPane(&m_POUPane);
			leftTabbedBar = &m_POUPane;
		}
		else
		{
			CDockablePane* pTabbedBar = nullptr;
			m_POUPane.AttachToTabWnd(leftTabbedBar, DM_SHOW, TRUE, &pTabbedBar);
			leftTabbedBar = pTabbedBar;
		}
	}
	CDockablePane* bottomTabbedBar = nullptr;
	EnableDocking(CBRS_ALIGN_BOTTOM);
	if(m_OutputPane.m_hWnd!=nullptr)
	{
		if(bottomTabbedBar==nullptr)
		{
			CMDIFrameWndEx::DockPane(&m_OutputPane);
			bottomTabbedBar = &m_OutputPane;
		}
		else
		{
			CDockablePane* pTabbedBar = nullptr;
			m_OutputPane.AttachToTabWnd(bottomTabbedBar, DM_SHOW, TRUE, &pTabbedBar);
			bottomTabbedBar = pTabbedBar;
		}
		CMDIFrameWndEx::ShowPane(&m_OutputPane, TRUE, TRUE, TRUE);
	}
	if(m_TablePane.m_hWnd!=nullptr)
	{
		if(bottomTabbedBar==nullptr)
		{
			CMDIFrameWndEx::DockPane(&m_TablePane);
			bottomTabbedBar = &m_TablePane;
		}
		else
		{
			CDockablePane* pTabbedBar = nullptr;
			m_TablePane.AttachToTabWnd(bottomTabbedBar, DM_SHOW, TRUE, &pTabbedBar);
			bottomTabbedBar = pTabbedBar;
		}
	}

	EnableDocking(CBRS_ALIGN_RIGHT);
	if(m_PropertyPane.m_hWnd!=nullptr)
	{
		CMDIFrameWndEx::DockPane(&m_PropertyPane);
		m_PropertyPane.ShowPane(FALSE, FALSE, FALSE);
	}
}
