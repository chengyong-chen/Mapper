#include "stdafx.h"
#include "AtlasPane.h"

#include <winuser.h>
#include "Atlas1.h"

#include "CatalogTree.h"
#include "PyramidTree.h"

#include "AtlasDlg.h"

#include "../Public/LicenceDlg.h"

#include "../Information/DatabaseDlg.h"
#include <wingdi.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAtlasPane dialog

IMPLEMENT_DYNAMIC(CAtlasPane, CDockablePane)

CAtlasPane::CAtlasPane()
{
	m_pAtlas = nullptr;
}

CAtlasPane::~CAtlasPane()
{
}

BEGIN_MESSAGE_MAP(CAtlasPane, CDockablePane)
	//{{AFX_MSG_MAP(CAtlasPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_ATLAS_CREATEPYRAMID, OnCreatePyramid)
	ON_COMMAND(ID_ATLAS_ATTRIBUTE, OnAttribute)
	ON_COMMAND(ID_ATLAS_PUBLISHPC, OnPublishPC)
	ON_COMMAND(ID_ATLAS_PUBLISHCE, OnPublishCE)
	ON_COMMAND(ID_ATLAS_PUBLISHWEB, OnPublishWEB)
	ON_COMMAND(ID_PROJECTION_POUDATABASE, OnAttachPOUDatabase)
	ON_COMMAND(ID_PROJECTION_LABDATABASE, OnAttachLABDatabase)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAtlasPane message handlers
int CAtlasPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct)==-1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if(m_ComboBox.Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_BORDER|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, rectDummy, this, 1)==TRUE)
	{
		m_ComboBox.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
		m_ComboBox.EnableWindow(FALSE);
	}

	if(m_ToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PANE_ATLAS)==TRUE)
	{
		m_ToolBar.LoadToolBar(IDR_PANE_ATLAS, 0, 0, TRUE /* Is locked */);
		m_ToolBar.CleanUpLockedImages();
		m_ToolBar.LoadBitmap(IDB_PANE_ATLAS24, 0, 0, TRUE /* Locked */);

		m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()|CBRS_TOOLTIPS|CBRS_FLYBY);
		m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()&~(CBRS_GRIPPER|CBRS_SIZE_DYNAMIC|CBRS_BORDER_TOP|CBRS_BORDER_BOTTOM|CBRS_BORDER_LEFT|CBRS_BORDER_RIGHT));
		m_ToolBar.SetOwner(this);
		m_ToolBar.SetRouteCommandsViaFrame(FALSE);// All commands will be routed via this control , not via the parent frame:
	}

	if(m_CatalogTree.Create(WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|TVS_EDITLABELS|TVS_SHOWSELALWAYS, rectDummy, this, 2)==FALSE)
		TRACE0("Failed to create Catalog Tree\n");
	else
		m_CatalogTree.ShowWindow(SW_HIDE);

	if(m_PyramidTree.Create(WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|TVS_EDITLABELS|TVS_SHOWSELALWAYS, rectDummy, this, 3)==FALSE)
		TRACE0("Failed to create Pyramid Tree\n");
	else
		m_PyramidTree.ShowWindow(SW_HIDE);

	CMenu menu;
	if(menu.LoadMenu(IDR_ATLAS)==TRUE)
	{
		if(menu.GetSubMenu(0)!=nullptr)
		{
			m_ToolBar.ReplaceButton(ID_ATLAS_MAINMENU, CMFCToolBarMenuButton(-1, menu.GetSubMenu(0)->GetSafeHmenu(), -1, _T("Atlas")));
			if(CMFCToolBarMenuButton* pButton = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(0)); pButton!=nullptr)
			{
				pButton->m_bImage = TRUE;
				pButton->SetMessageWnd(this);
				pButton->EnableWindow(FALSE);
				pButton->SetVisible(FALSE);
				m_ToolBar.AdjustSizeImmediate();
			}
		}
		if(menu.GetSubMenu(1)!=nullptr)
		{
			m_ToolBar.ReplaceButton(ID_ITEM_ADD, CMFCToolBarMenuButton(-1, menu.GetSubMenu(1)->GetSafeHmenu(), -1, _T("Node")));
			if(CMFCToolBarMenuButton* pButton = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(1)); pButton!=nullptr)
			{
				pButton->m_bImage = TRUE;
				pButton->SetMessageWnd(nullptr);
				pButton->EnableWindow(FALSE);
				pButton->SetVisible(FALSE);
				m_ToolBar.AdjustSizeImmediate();
			}
		}
	}
	const HICON hIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ATLAS), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	CDockablePane::SetIcon(hIcon, FALSE);
	CDockablePane::EnableToolTips(TRUE);
	CDockablePane::EnableDocking(CBRS_ALIGN_LEFT);
	return 0;
}

void CAtlasPane::OnCreatePyramid()
{
	if(m_pAtlas!=nullptr)
	{
		m_pAtlas->m_Pyramid.Release();
		m_PyramidTree.BuildTree(nullptr);
		m_PyramidTree.BuildTree(&(m_pAtlas->m_Pyramid));
		if(m_ComboBox.GetCount()<2)
		{
			m_ComboBox.AddString(_T("Pyramid"));
			m_ComboBox.EnableWindow(TRUE);
			m_ComboBox.SetCurSel(1);
			OnTopicSelChanged();
		}
	}
}

void CAtlasPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if(GetSafeHwnd()!=nullptr)
	{
		CRect rectClient;
		GetClientRect(rectClient);

		CRect rectCombo;
		m_ComboBox.GetWindowRect(&rectCombo);
		const int cyCmb = rectCombo.Size().cy;
		const int cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;

		m_ComboBox.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE|SWP_NOZORDER);
		m_ToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top+cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE|SWP_NOZORDER);

		m_CatalogTree.SetWindowPos(nullptr, rectClient.left+1, rectClient.top+cyCmb+cyTlb+1, rectClient.Width()-2, rectClient.Height()-(cyCmb+cyTlb)-2, SWP_NOACTIVATE|SWP_NOZORDER);
		m_PyramidTree.SetWindowPos(nullptr, rectClient.left+1, rectClient.top+cyCmb+cyTlb+1, rectClient.Width()-2, rectClient.Height()-(cyCmb+cyTlb)-2, SWP_NOACTIVATE|SWP_NOZORDER);
	}
}

void CAtlasPane::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(200, 200, 200));
	dc.Draw3dRect(rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CAtlasPane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	if(m_CatalogTree.IsWindowVisible()==TRUE)
	{
		m_CatalogTree.SetFocus();
	}
	else if(m_PyramidTree.IsWindowVisible()==TRUE)
	{
		m_PyramidTree.SetFocus();
	}
}

void CAtlasPane::OnAttribute()
{
	HINSTANCE hInst = ::LoadLibrary(_T("Atlas.dll"));
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);

	CAtlasDlg dlg(this, m_pAtlas);
	if(dlg.DoModal()==IDOK)
	{
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CAtlasPane::OnPublishPC()
{
	if(m_pAtlas!=nullptr)
	{
		m_pAtlas->PublishPC();
	}
}

void CAtlasPane::OnPublishCE()
{
	if(m_pAtlas!=nullptr)
	{
		m_pAtlas->PublishCE();
	}
}

void CAtlasPane::OnPublishWEB()
{
	if(const DWORD dwKey = AfxGetAuthorizationKey(); (dwKey&LICENCE::MAPINTERNET)!=LICENCE::MAPINTERNET)
	{
		CLicenceDlg::Display();
		return;
	}

	if(m_pAtlas!=nullptr)
	{
		m_pAtlas->PublishWEB();
	}
}

void CAtlasPane::OnAttachPOUDatabase()
{
	//DWORD dwKey = AfxGetAuthorizationKey();
	//if((dwKey & LICENCE::DATABASEINFO) != LICENCE::DATABASEINFO)
	//{
	//	CLicenceDlg::Display();
	//	return;
	//}
	//if(m_pAtlas == nullptr)
	//	return;

	//if(m_pAtlas->m_databasePOU.IsOpen() == TRUE)
	//{
	//	CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE,'O',0);
	//	if(pTablePane != nullptr)
	//	{
	//		if(pTablePane->m_pPOUTDBGridCtrl != nullptr)
	//		{
	//			pTablePane->m_pPOUTDBGridCtrl->SetDatabase(nullptr);				
	//		}
	//		if(pTablePane->m_pPOUDBGridCtrl != nullptr)
	//		{
	//			pTablePane->m_pPOUDBGridCtrl->Clear();
	//		}
	//	}
	//	CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE,'I',0);
	//	if(pPropertyPane != nullptr)
	//	{
	//		CAttributeCtrl* pAttributeCtrl = (CAttributeCtrl*)pPropertyPane->GetTabWnd('A');
	//		if(pAttributeCtrl != nullptr)
	//		{
	//			pAttributeCtrl->OnTableChanged(nullptr,_T(""));
	//		}
	//	}
	//	m_pAtlas->m_databasePOU.Close();
	//}

	//CODBCDatabase databasePOU;
	//if(databasePOU.Attach() == TRUE)
	//{
	//	m_pAtlas->m_databasePOU.m_dbms = databasePOU.m_dbms;
	//	m_pAtlas->m_databasePOU.m_strServer = databasePOU.m_strServer;
	//	m_pAtlas->m_databasePOU.m_strDatabase = databasePOU.m_strDatabase;
	//	m_pAtlas->m_databasePOU.m_strUID = databasePOU.m_strUID;
	//	m_pAtlas->m_databasePOU.m_strPWD = databasePOU.m_strPWD;
	//	m_pAtlas->m_databasePOU.m_bFileSource = databasePOU.m_bFileSource;
	//	databasePOU.Close();
	//	if(m_pAtlas->m_databasePOU.Open(CDatabase::noOdbcDialog) == TRUE)
	//	{
	//		CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE,'O',0);
	//		if(pTablePane != nullptr)
	//		{
	//			if(pTablePane->m_pPOUTDBGridCtrl != nullptr)
	//			{
	//				pTablePane->m_pPOUTDBGridCtrl->SetDatabase(&m_pAtlas->m_databasePOU);
	//			}
	//		}
	//	}
	//}
}

void CAtlasPane::OnAttachLABDatabase()
{
	if(m_pAtlas!=nullptr)
	{
		HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		AfxSetResourceHandle(hInst);

		CDatabaseDlg dlg;
		dlg.strDatabase = m_pAtlas->m_databaseLAB.m_strDatabase;
		dlg.strServer = m_pAtlas->m_databaseLAB.m_strServer;
		dlg.strUID = m_pAtlas->m_databaseLAB.m_strUID;
		dlg.strPWD = m_pAtlas->m_databaseLAB.m_strPWD;
		if(dlg.DoModal()==IDOK)
		{
			m_pAtlas->m_databaseLAB.m_strDatabase = dlg.strDatabase;
			m_pAtlas->m_databaseLAB.m_strServer = dlg.strServer;
			m_pAtlas->m_databaseLAB.m_strUID = dlg.strUID;
			m_pAtlas->m_databaseLAB.m_strPWD = dlg.strPWD;
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
}

BOOL CAtlasPane::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(HIWORD(wParam))
	{
	case CBN_SELCHANGE:
	{
		OnTopicSelChanged();
	}
	break;
	default:
		break;
	}

	return CDockablePane::OnCommand(wParam, lParam);
}

void CAtlasPane::OnTopicSelChanged()
{
	const int index = m_ComboBox.GetCurSel();
	switch(index)
	{
	case 0:
	{
		m_PyramidTree.ShowWindow(SW_HIDE);
		m_CatalogTree.ShowWindow(SW_SHOW);

		CMFCToolBarMenuButton* pButton0 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(0));
		pButton0->SetVisible(TRUE);

		CMFCToolBarMenuButton* pButton1 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(1));
		pButton1->SetMessageWnd(&m_CatalogTree);

		m_ToolBar.AdjustLayout();
		m_ToolBar.AdjustSizeImmediate();
	}
	break;
	case 1:
	{
		m_CatalogTree.ShowWindow(SW_HIDE);
		m_PyramidTree.ShowWindow(SW_SHOW);

		CMFCToolBarMenuButton* pButton0 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(0));
		pButton0->SetVisible(FALSE);

		CMFCToolBarMenuButton* pButton1 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(1));
		pButton1->SetMessageWnd(&m_PyramidTree);

		m_ToolBar.AdjustLayout();
	}
	break;
	}
}

BOOL CAtlasPane::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CDockablePane::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nID)
			{
			case ID_ATLAS_CREATEPYRAMID:
			{
				if(m_pAtlas==nullptr)
					pCmdUI->Enable(FALSE);
				else if(m_pAtlas->m_Catalog.m_PageList.GetCount()==0)
					pCmdUI->Enable(FALSE);
				else if(m_pAtlas->m_Pyramid.m_DeckList.GetCount()!=0)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
			}
			return TRUE;
			case ID_ATLAS_ATTRIBUTE:
			{
				if(m_pAtlas==nullptr)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
			}
			return TRUE;
			case ID_ATLAS_PUBLISHPC:
			case ID_ATLAS_PUBLISHCE:
			case ID_ATLAS_PUBLISHWEB:
			{
				if(m_pAtlas==nullptr)
					pCmdUI->Enable(FALSE);
				else if(m_pAtlas->m_Catalog.m_PageList.GetCount()==0)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
			}
			return TRUE;
			case ID_PROJECTION_POUDATABASE:
			case ID_PROJECTION_LABDATABASE:
			{
				if(m_pAtlas==nullptr)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
			}
			return TRUE;
			default:
				break;
			}
			pCmdUI->m_bContinueRouting = false;
		}
		if(nCode==CN_COMMAND)
		{
		}
	}

	return CDockablePane::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CAtlasPane::Attach(CAtlas* pAtlas, bool showPyramid)
{
	if(pAtlas!=m_pAtlas)
	{
		m_ComboBox.EnableWindow(TRUE);
		m_ComboBox.ResetContent();

		m_CatalogTree.BuildTree(&(pAtlas->m_Catalog));
		m_ComboBox.AddString(_T("category"));
		m_ComboBox.SetCurSel(0);
		if(CMFCToolBarMenuButton* pButton0 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(0)); pButton0!=nullptr)
		{
			pButton0->EnableWindow(TRUE);
			pButton0->SetVisible(TRUE);
			pButton0->SetMessageWnd(this);
		}
		if(CMFCToolBarMenuButton* pButton1 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(1)); pButton1!=nullptr)
		{
			pButton1->EnableWindow(TRUE);
			pButton1->SetVisible(TRUE);
			pButton1->SetMessageWnd(nullptr);
		}
		m_ToolBar.EnableWindow(TRUE);
		m_ToolBar.AdjustLayout();
		//m_ToolBar.AdjustSizeImmediate();

		CRect rectClient;
		GetClientRect(rectClient);
		const int cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
		m_ToolBar.SetWindowPos(nullptr, 0, 0, rectClient.Width(), cyTlb, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);

		OnTopicSelChanged();

		if(showPyramid==true&&pAtlas->m_Pyramid.m_DeckList.GetCount()>0)
		{
			m_PyramidTree.BuildTree(&(pAtlas->m_Pyramid));
			m_ComboBox.AddString(_T("Pyramid"));
		}
		else
		{
			m_ComboBox.EnableWindow(FALSE);
		}

		m_pAtlas = pAtlas;
	}
}

void CAtlasPane::Detach(CAtlas* pAtlas)
{
	if(pAtlas==nullptr)
	{
	}
	else if(pAtlas==m_pAtlas)
	{
		m_ComboBox.EnableWindow(TRUE);
		m_ComboBox.ResetContent();
		m_ComboBox.EnableWindow(FALSE);
		m_ToolBar.EnableWindow(FALSE);
		if(CMFCToolBarMenuButton* pButton0 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(0)); pButton0!=nullptr)
		{
			pButton0->SetMessageWnd(nullptr);
			pButton0->EnableWindow(FALSE);
			pButton0->SetVisible(FALSE);
			m_ToolBar.AdjustLayout();
			m_ToolBar.AdjustSizeImmediate();
		}
		if(CMFCToolBarMenuButton* pButton1 = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(1)); pButton1!=nullptr)
		{
			pButton1->SetMessageWnd(nullptr);
			pButton1->EnableWindow(FALSE);
			pButton1->SetVisible(FALSE);
			m_ToolBar.AdjustLayout();
			m_ToolBar.AdjustSizeImmediate();
		}

		m_CatalogTree.BuildTree(nullptr);
		m_PyramidTree.BuildTree(nullptr);

		m_pAtlas = nullptr;
	}
}

BOOL CAtlasPane::DestroyWindow()
{
	const HICON hIcon = CDockablePane::GetIcon(FALSE);
	if(hIcon!=nullptr)
	{
		DestroyIcon(hIcon);
	}
	m_ToolBar.CleanUpImages();
	m_ToolBar.CleanUpLockedImages();

	return CDockablePane::DestroyWindow();
}
