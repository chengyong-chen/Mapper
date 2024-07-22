#include "stdafx.h"
#include "resource.h"
#include "TopoPane.h"
#include "Topoer.h"
#include "TopoCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTopoPane dialog

CTopoPane::CTopoPane()
{
	d_pTopoCtrl = nullptr;
}

BEGIN_MESSAGE_MAP(CTopoPane, CDockablePane)
	//{{AFX_MSG_MAP(CTopoPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTopoPane message handlers

int CTopoPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct)==-1)
		return -1;

	if(m_ToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PANE_TOPO)==TRUE)
	{
		m_ToolBar.LoadToolBar(IDR_PANE_TOPO, 0, 0, TRUE);
		m_ToolBar.CleanUpLockedImages();
		m_ToolBar.LoadBitmap(IDB_PANE_TOPO, 0, 0, TRUE);
		m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()|CBRS_TOOLTIPS|CBRS_FLYBY);
		m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()&~(CBRS_GRIPPER|CBRS_SIZE_DYNAMIC|CBRS_BORDER_TOP|CBRS_BORDER_BOTTOM|CBRS_BORDER_LEFT|CBRS_BORDER_RIGHT));
		m_ToolBar.SetOwner(this);
		m_ToolBar.SetRouteCommandsViaFrame(FALSE);// All commands will be routed via this control , not via the parent frame:

		CMenu menu1;
		CMenu menu2;
		CMenu menu3;
		menu1.LoadMenu(IDR_TOPO_NEW);
		menu2.LoadMenu(IDR_TOPO_REGION);
		menu3.LoadMenu(IDR_TOPO_TRAFFIC);
		const HMENU hMenu1 = menu1.GetSubMenu(0)->GetSafeHmenu();
		const HMENU hMenu2 = menu2.GetSubMenu(0)->GetSafeHmenu();
		const HMENU hMenu3 = menu3.GetSubMenu(0)->GetSafeHmenu();
		m_ToolBar.ReplaceButton(ID_TOPO_BUTTON1, CMFCToolBarMenuButton(-1, hMenu1, -1, _T("Topo")));
		m_ToolBar.ReplaceButton(ID_TOPO_BUTTON2, CMFCToolBarMenuButton(-1, hMenu2, -1, _T("Region")));
		m_ToolBar.ReplaceButton(ID_TOPO_BUTTON3, CMFCToolBarMenuButton(-1, hMenu3, -1, _T("Road")));
		CMFCToolBarMenuButton* pButton1 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(0));
		CMFCToolBarMenuButton* pButton2 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(1));
		CMFCToolBarMenuButton* pButton3 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(2));
		if(pButton1!=nullptr)
		{
			pButton1->m_bImage = TRUE;
			pButton1->SetMessageWnd(nullptr);
			pButton1->EnableWindow(FALSE);
			pButton1->SetVisible(FALSE);
		}
		if(pButton2!=nullptr)
		{
			pButton2->m_bImage = TRUE;
			pButton2->SetMessageWnd(nullptr);
			pButton2->EnableWindow(FALSE);
			pButton2->SetVisible(FALSE);
		}
		if(pButton3!=nullptr)
		{
			pButton3->m_bImage = TRUE;
			pButton3->SetMessageWnd(nullptr);
			pButton3->EnableWindow(FALSE);
			pButton3->SetVisible(FALSE);
		}

		m_ToolBar.AdjustSizeImmediate();
	}
	const HICON hIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_TOPO), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	CDockablePane::SetIcon(hIcon, FALSE);
	return 0;
}

BOOL CTopoPane::DestroyWindow()
{
	const HICON hIcon = CDockablePane::GetIcon(FALSE);
	if(hIcon!=nullptr)
	{
		DestroyIcon(hIcon);
	}

	CMFCToolBarMenuButton* pButton1 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(0));
	if(pButton1!=nullptr)
	{
	}
	CMFCToolBarMenuButton* pButton2 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(1));
	if(pButton2!=nullptr)
	{
	}
	CMFCToolBarMenuButton* pButton3 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(1));
	if(pButton3!=nullptr)
	{
	}

	m_ToolBar.CleanUpImages();
	m_ToolBar.CleanUpLockedImages();

	return CDockablePane::DestroyWindow();
}

void CTopoPane::SetCtrl(CTopoCtrl* pTopoCtrl)
{
	if(d_pTopoCtrl==pTopoCtrl)
		return;

	if(d_pTopoCtrl!=nullptr)
	{
		d_pTopoCtrl->ShowWindow(SW_HIDE);

		CMFCToolBarMenuButton* pButton1 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(0));
		if(pButton1!=nullptr)
		{
			pButton1->SetMessageWnd(nullptr);
			pButton1->EnableWindow(FALSE);
			pButton1->SetVisible(FALSE);
		}
		CMFCToolBarMenuButton* pButton2 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(1));
		if(pButton2!=nullptr)
		{
			pButton2->SetMessageWnd(nullptr);
			pButton2->EnableWindow(FALSE);
			pButton2->SetVisible(FALSE);
		}
		CMFCToolBarMenuButton* pButton3 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(1));
		if(pButton3!=nullptr)
		{
			pButton3->SetMessageWnd(nullptr);
			pButton3->EnableWindow(FALSE);
			pButton3->SetVisible(FALSE);
		}

		m_ToolBar.AdjustSizeImmediate();
	}

	if(pTopoCtrl!=nullptr)
	{
		CRect rectClient;
		GetClientRect(rectClient);
		const int cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
		pTopoCtrl->SetWindowPos(nullptr, rectClient.left+1, rectClient.top+cyTlb+1, rectClient.Width()-2, rectClient.Height()-cyTlb-2, SWP_NOACTIVATE|SWP_NOZORDER|SWP_SHOWWINDOW);

		CMFCToolBarMenuButton* pButton1 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(0));
		if(pButton1!=nullptr)
		{
			pButton1->SetMessageWnd(pTopoCtrl);
			pButton1->EnableWindow(TRUE);
			pButton1->SetVisible(TRUE);
		}
		CMFCToolBarMenuButton* pButton2 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(1));
		if(pButton2!=nullptr)
		{
			pButton2->SetMessageWnd(pTopoCtrl);
			pButton2->EnableWindow(TRUE);
			pButton2->SetVisible(TRUE);
		}
		CMFCToolBarMenuButton* pButton3 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(2));
		if(pButton3!=nullptr)
		{
			pButton3->SetMessageWnd(pTopoCtrl);
			pButton3->EnableWindow(TRUE);
			pButton3->SetVisible(TRUE);
		}

		m_ToolBar.AdjustSizeImmediate();
	}

	d_pTopoCtrl = pTopoCtrl;
}

void CTopoPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if(GetSafeHwnd()!=nullptr)
	{
		CRect rectClient;
		GetClientRect(rectClient);
		const int cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
		m_ToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE|SWP_NOZORDER);

		if(d_pTopoCtrl!=nullptr)
		{
			d_pTopoCtrl->SetWindowPos(nullptr, rectClient.left+1, rectClient.top+cyTlb+1, rectClient.Width()-2, rectClient.Height()-cyTlb-2, SWP_NOACTIVATE|SWP_NOZORDER);
		}
	}
}

void CTopoPane::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(200, 200, 200));
	dc.Draw3dRect(rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CTopoPane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	if(d_pTopoCtrl!=nullptr)
	{
		d_pTopoCtrl->SetFocus();
	}
}

/*HINSTANCE hInst = ::LoadLibrary(_T("Topology.dll"));
HINSTANCE hInstOld = AfxGetResourceHandle();
AfxSetResourceHandle(hInst);

CTopology* topo = d_pTopoCtrl->GetSelTopo();
if(topo != nullptr)
{
	if(topo->m_bType == 11 == TRUE)
	{
		CMenu Menu;
		if(Menu.LoadMenu(IDR_REGION) == TRUE)
		{
			CMenu* pSubMenu = Menu.GetSubMenu(0);
			if(((CRegionTopoer*)topo)->m_polygonlist.GetCount() != 0)
			{
				pSubMenu->EnableMenuItem(ID_TOPO_FILTEEDGE,    MF_DISABLED | MF_GRAYED);
			}
			else
			{
				pSubMenu->EnableMenuItem(ID_TOPO_STYLE,        MF_DISABLED | MF_GRAYED);
				pSubMenu->EnableMenuItem(ID_TOPO_TYPICALGROUP, MF_DISABLED | MF_GRAYED);
				pSubMenu->EnableMenuItem(ID_TOPO_MANUALGROUP,  MF_DISABLED | MF_GRAYED);
				pSubMenu->EnableMenuItem(ID_TOPO_COVERTPOLYGONS, MF_DISABLED | MF_GRAYED);
			}

			if(topo->m_mapIDEdge.size() != 0)
			{
			}
			else
			{
				pSubMenu->EnableMenuItem(ID_TOPO_CREATEPOLYGONS, MF_DISABLED | MF_GRAYED);
				pSubMenu->EnableMenuItem(ID_TOPO_FILTEEDGE,      MF_DISABLED | MF_GRAYED);
			}

			pMenubitmap->GetWindowRect(&rect);
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,rect.right,rect.top,d_pTopoCtrl);
			pSubMenu->DestroyMenu();

			Menu.DestroyMenu();
		}
	}
	else if(topo->IsKindOf(RUNTIME_CLASS(CRoadTopodb)) == TRUE)
	{
		CMenu Menu;
		if(Menu.LoadMenu(IDR_TRAFFIC) == TRUE)
		{
			CMenu* pSubMenu = Menu.GetSubMenu(0);

			pMenubitmap->GetWindowRect(&rect);
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,rect.right,rect.top,d_pTopoCtrl);
			pSubMenu->DestroyMenu();

			Menu.DestroyMenu();
		}
	}
}
else
{

}

AfxSetResourceHandle(hInstOld);
::FreeLibrary(hInst);*/
