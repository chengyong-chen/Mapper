#include "stdafx.h"

#include "../Framework/GCreateContext.h"
#include "../Framework/GRuntimeClass.h"

#include "../Public/Global.h"
#include "resource.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
extern UNIT a_UnitArray[3];
extern BYTE a_nUnitIndex;
extern long a_nGreekType;
extern long a_nUndoNumber;

extern __declspec(dllimport) BYTE d_nUnitIndex;

extern __declspec(dllimport) bool s_bGUI32;
extern __declspec(dllimport) bool o_bGUI32;

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PREFERENCES, OnFilePreferences)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR, // status line indicator
	ID_INDICATOR_NUM,
	ID_INDICATOR_NUM,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	a_nUnitIndex = AfxGetApp()->GetProfileInt(_T("Settings"), _T("UnitIndex"), 1);
	a_nGreekType = AfxGetApp()->GetProfileInt(_T("Settings"), _T("GreekType"), 5);
	a_nUndoNumber = AfxGetApp()->GetProfileInt(_T("Settings"), _T("Undo"), 8);
	d_nUnitIndex = a_nUnitIndex;
	m_wndMenuBar.m_bClearHashOnClose = TRUE;
	CMFCPopupMenu::SetForceShadow(TRUE);
}

CMainFrame::~CMainFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}

#endif //_DEBUG

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CFrameWndEx::OnCreate(lpCreateStruct)==-1)
		return -1;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
	CDockingManager::SetDockingMode(DT_SMART);

	//		CDockablePane::SetCaptionStyle(TRUE,FALSE,TRUE);
	CMFCToolBarComboBoxButton::SetFlatMode();
	CMFCToolBarComboBoxButton::SetCenterVert();
	CMFCButton::EnableWindowsTheming();
	CMFCToolBar::EnableQuickCustomization(TRUE);
	CFrameWndEx::RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_UPDATENOW|RDW_FRAME|RDW_ERASE);

	if(m_wndMenuBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE)==TRUE)
	{
		CMFCPopupMenu::SetForceMenuFocus(FALSE);

		m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle()|CBRS_SIZE_FIXED|CBRS_TOOLTIPS);
		m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle()&~(CBRS_GRIPPER|CBRS_FLOATING));
		m_wndMenuBar.EnableCustomizeButton(TRUE, -1, _T(""));
		m_wndMenuBar.EnableMenuShadows();
		m_wndMenuBar.SetMaximizeMode(true);
		m_wndMenuBar.EnableToolTips(TRUE);
		m_wndMenuBar.EnableDocking(CBRS_ALIGN_TOP);
		CMFCPopupMenu::SetForceMenuFocus(FALSE);
	}
	if(m_toolbarMain.Create(this, WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_GRIPPER|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_FIXED, ID_VIEW_TOOLBARS_MAIN)==TRUE&&m_toolbarMain.LoadToolBar(IDR_TOOLBAR_MAIN, 0, 0, FALSE, 0, 0, IDB_TOOLBAR_MAIN)==TRUE)
	{
		m_toolbarMain.SetWindowText(_T("Standard"));

		m_toolbarMain.SetPaneStyle(m_toolbarMain.GetPaneStyle()|CBRS_SIZE_FIXED|TBSTYLE_FLAT|CBRS_ALIGN_TOP);
		m_toolbarMain.SetPaneStyle(m_toolbarMain.GetPaneStyle()&~CBRS_FLOATING);

		m_toolbarMain.EnableDocking(CBRS_ALIGN_TOP);
		m_toolbarMain.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."));
	}
	if(m_toolbarMagnify.Create(this, WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_GRIPPER|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_FIXED, ID_VIEW_TOOLBARS_MAGNIFY)==TRUE&&m_toolbarMagnify.LoadToolBar(IDR_TOOLBAR_MAGNIFY, 0, 0, FALSE, 0, 0, IDB_TOOLBAR_MAGNIFY)==TRUE)
	{
		m_toolbarMagnify.SetWindowText(_T("Magnify"));

		m_toolbarMagnify.SetPaneStyle(m_toolbarMagnify.GetPaneStyle()|CBRS_SIZE_FIXED|TBSTYLE_FLAT|CBRS_ALIGN_TOP);
		m_toolbarMagnify.SetPaneStyle(m_toolbarMagnify.GetPaneStyle()&~CBRS_FLOATING);

		m_toolbarMagnify.EnableDocking(CBRS_ALIGN_TOP);
		m_toolbarMagnify.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."));
	}
	if(m_toolbarGeomAlign.Create(this, WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_GRIPPER|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_FIXED, ID_VIEW_TOOLBARS_GEOMALIGN)==TRUE&&m_toolbarGeomAlign.LoadToolBar(IDR_TOOLBAR_GEOMALIGN, 0, 0, FALSE, 0, 0, IDB_TOOLBAR_GEOMALIGN)==TRUE)
	{
		m_toolbarGeomAlign.SetWindowText(_T("Align"));

		m_toolbarGeomAlign.SetPaneStyle(m_toolbarGeomAlign.GetPaneStyle()|CBRS_SIZE_FIXED|TBSTYLE_FLAT|CBRS_ALIGN_TOP);
		m_toolbarGeomAlign.SetPaneStyle(m_toolbarGeomAlign.GetPaneStyle()&~CBRS_FLOATING);

		m_toolbarGeomAlign.EnableDocking(CBRS_ALIGN_TOP);
		m_toolbarGeomAlign.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."));
	}
	if(m_toolbarAxisAlign.Create(this, WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_GRIPPER|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_FIXED, ID_VIEW_TOOLBARS_AXISALIGN)==TRUE&&m_toolbarAxisAlign.LoadToolBar(IDR_TOOLBAR_AXISALIGN, 0, 0, FALSE, 0, 0, IDB_TOOLBAR_AXISALIGN)==TRUE)
	{
		m_toolbarAxisAlign.SetWindowText(_T("Align"));

		m_toolbarAxisAlign.SetPaneStyle(m_toolbarAxisAlign.GetPaneStyle()|CBRS_SIZE_FIXED|TBSTYLE_FLAT|CBRS_ALIGN_TOP);
		m_toolbarAxisAlign.SetPaneStyle(m_toolbarAxisAlign.GetPaneStyle()&~CBRS_FLOATING);

		m_toolbarAxisAlign.EnableDocking(CBRS_ALIGN_TOP);
		m_toolbarAxisAlign.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."));
	}
	if(m_wndStatusBar.Create(this)==TRUE)
	{
		m_wndStatusBar.EnableWindow(TRUE);
		m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
		m_wndStatusBar.SetPaneInfo(ID_QUERYCOUNT, ID_SEPARATOR, 0, 80);
		m_wndStatusBar.SetPaneInfo(ID_PICKANCHORXY, ID_SEPARATOR, 0, 120);
		m_wndStatusBar.SetPaneInfo(ID_MOUSECOORDINATE, ID_SEPARATOR, 0, 120);
	}

	CDockingManager::SetDockingMode(DT_SMART);
	if(m_wndDrawTool.Create(this, WS_CHILD|WS_VISIBLE, 1111)==TRUE)
	{
		m_wndDrawTool.SetWindowText(_T("Draw"));
		m_wndDrawTool.EnableDocking(FALSE);
	}
	//	CMFCToolBar::AddToolBarForImageCollection(IDR_MENUITEMS,IDB_MENUITEMS);
	//	InitUserToolbars(nullptr, AFX_IDW_CONTROLBAR_FIRST + 40, AFX_IDW_CONTROLBAR_FIRST + 40 + 10 - 1);
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."), ID_VIEW_TOOLBARS, TRUE, TRUE);// Enable toolbar and docking window menu replacement
	ModifyStyle(0, FWS_PREFIXTITLE);

	OnResetwindowLayout();
	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if(!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		TRACE(_T("Failed to CreateStaticSplitter\n"));
		return FALSE;
	}
	else if(!m_wndSplitter.CreateView(0, 0, ((GCreateContext*)pContext)->m_pFirstViewClass, CSize(150, 0), pContext))
	{
		TRACE(_T("Failed to create second pane\n"));
		return FALSE;
	}
	else if(!m_wndSplitter.CreateView(0, 1, ((GCreateContext*)pContext)->m_pSecondViewClass, CSize(0, 0), pContext))
	{
		TRACE(_T("Failed to create first pane\n"));
		return FALSE;
	}
	else
	{
		// activate the input view
		SetActiveView((CView*)m_wndSplitter.GetPane(0, 0));
		m_wndSplitter.SetColumnInfo(0, 200, 0);
		return TRUE;
	}
	//
	if(pContext!=nullptr&&pContext->m_pNewViewClass!=nullptr)
	{
		if(CreateView(pContext, AFX_IDW_PANE_FIRST)==nullptr)
			return FALSE;
	}
	return TRUE;
	return CFrameWndEx::OnCreateClient(lpcs, pContext);
}

CWnd* CMainFrame::CreateView(CCreateContext* pContext, UINT nId)
{
	ASSERT(m_hWnd!=nullptr);
	ASSERT(::IsWindow(m_hWnd));
	ENSURE_ARG(pContext!=nullptr);
	ENSURE_ARG(pContext->m_pNewViewClass!=nullptr);

	// Note: can be a CWnd with PostNcDestroy self cleanup
	CWnd* pView = (CWnd*)((GRuntimeClass*)(pContext->m_pNewViewClass))->CreateObject(*(pContext->m_pCurrentDoc));
	if(pView==nullptr)
	{
		TRACE(traceAppMsg, 0, "Warning: Dynamic create of view type %hs failed.\n", pContext->m_pNewViewClass->m_lpszClassName);
		return nullptr;
	}
	ASSERT_KINDOF(CWnd, pView);

	// views are always created with a border!
	if(!pView->Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, nId, pContext))
	{
		TRACE(traceAppMsg, 0, "Warning: could not create view for frame.\n");
		return nullptr; // can't continue without a view
	}

	if(pView->GetExStyle()&WS_EX_CLIENTEDGE)
	{
		// remove the 3d style from the frame, since the view is
		//  providing it.
		// make sure to recalc the non-client area
		ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	}
	return pView;
}

BOOL CMainFrame::DestroyWindow()
{
	CRect rect;
	m_wndDrawTool.GetWindowRect(rect);
	const BOOL bVisible = (m_wndDrawTool.GetStyle()&WS_VISIBLE)!=0;

	AfxGetApp()->WriteProfileInt(_T("Draw Tool"), _T("x"), rect.left);
	AfxGetApp()->WriteProfileInt(_T("Draw Tool"), _T("y"), rect.top);
	AfxGetApp()->WriteProfileInt(_T("Draw Tool"), _T("Show"), bVisible);

	ShowControlBar(&m_wndDrawTool, FALSE, FALSE);
	m_wndDrawTool.PostMessage(WM_DESTROY, 0, 0);
	m_wndDrawTool.DestroyWindow();
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("UnitIndex"), a_nUnitIndex);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("GreekType"), a_nGreekType);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("Undo"), a_nUndoNumber);

	return CFrameWndEx::DestroyWindow();
}

void CMainFrame::OnResetwindowLayout()
{
	//HKEY hkey;
	//RegOpenKeyEx(HKEY_CURRENT_USER,L"Software\\Diwatu\\Mapper",0,KEY_ALL_ACCESS,&hkey);
	//RegDeleteKey(hkey,L"Workspace");
	EnableDocking(CBRS_ALIGN_TOP);
	if(m_wndMenuBar.m_hWnd!=nullptr)
	{
		CFrameWndEx::DockPane(&m_wndMenuBar);
	}
	if(m_toolbarAxisAlign.m_hWnd!=nullptr)
	{
		CFrameWndEx::DockPane(&m_toolbarAxisAlign);
		m_toolbarAxisAlign.ShowPane(TRUE, TRUE, FALSE);
	}
	if(m_toolbarGeomAlign.m_hWnd!=nullptr)
	{
		CFrameWndEx::DockPaneLeftOf(&m_toolbarGeomAlign, &m_toolbarAxisAlign);
		m_toolbarGeomAlign.ShowPane(TRUE, TRUE, FALSE);
	}
	if(m_toolbarMagnify.m_hWnd!=nullptr)
	{
		CFrameWndEx::DockPaneLeftOf(&m_toolbarMagnify, &m_toolbarGeomAlign);
		m_toolbarMagnify.ShowPane(TRUE, TRUE, FALSE);
	}
	if(m_toolbarMain.m_hWnd!=nullptr)
	{
		CFrameWndEx::DockPaneLeftOf(&m_toolbarMain, &m_toolbarMagnify);
		m_toolbarMain.ShowPane(TRUE, TRUE, FALSE);
	}
	if(m_wndDrawTool.m_hWnd!=nullptr)
	{
		CFrameWnd::EnableDocking(CBRS_ALIGN_TOP);
		CFrameWndEx::ShowControlBar(&m_wndDrawTool, TRUE, FALSE);
		CFrameWndEx::FloatControlBar(&m_wndDrawTool, CPoint(130, 105), CBRS_ALIGN_TOP);
	}
}

void CMainFrame::OnFilePreferences()
{
}
