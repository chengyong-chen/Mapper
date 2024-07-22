#include "stdafx.h"

#include "POUPane.h"
#include "POUListCtrl.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPOUPane dialog

IMPLEMENT_DYNAMIC(CPOUPane, CDockablePane)

CPOUPane::CPOUPane(bool bEditable)
{
	m_bEditable = bEditable;
	m_pAtlPOUListCtrl = nullptr;
	m_pMapPOUListCtrl = nullptr;
}

BEGIN_MESSAGE_MAP(CPOUPane, CDockablePane)
	//{{AFX_MSG_MAP(CPOUPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(1, OnTopicChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPOUPane message handlers

int CPOUPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	if(m_ToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PANE_POU)==TRUE)
	{
		m_ToolBar.LoadToolBar(IDR_PANE_POU, 0, 0, TRUE);
		m_ToolBar.CleanUpLockedImages();
		m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()|CBRS_TOOLTIPS|CBRS_FLYBY);
		m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()&~(CBRS_GRIPPER|CBRS_SIZE_DYNAMIC|CBRS_BORDER_TOP|CBRS_BORDER_BOTTOM|CBRS_BORDER_LEFT|CBRS_BORDER_RIGHT));
		m_ToolBar.SetOwner(this);
		m_ToolBar.SetRouteCommandsViaFrame(FALSE);// All commands will be routed via this control , not via the parent frame:
	}

	CMenu menu;
	menu.LoadMenu(m_bEditable==true ? IDR_POU1 : IDR_POU2);
	const HMENU hMenu = menu.GetSubMenu(0)->GetSafeHmenu();
	m_ToolBar.ReplaceButton(ID_MENUBUTTON, CMFCToolBarMenuButton(-1, hMenu, -1, _T("Menu")));
	CMFCToolBarMenuButton* pButton = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(0));
	if(pButton!=nullptr)
	{
		pButton->m_bImage = TRUE;
		pButton->SetMessageWnd(nullptr);
		m_ToolBar.AdjustLayout();
	}
	m_ToolBar.SetOwner(nullptr);
	const HICON hIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_POU), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	CDockablePane::SetIcon(hIcon, FALSE);
	CDockablePane::EnableToolTips(TRUE);
	CDockablePane::EnableDocking(CBRS_ALIGN_LEFT);
	return 0;
}

void CPOUPane::OnSize(UINT nType, int cx, int cy)
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

		if(m_pAtlPOUListCtrl!=nullptr&&m_pAtlPOUListCtrl->m_hWnd!=nullptr)
		{
			m_pAtlPOUListCtrl->SetWindowPos(nullptr, rectClient.left+1, rectClient.top+cyTlb+cyCmb+1, rectClient.Width()-2, rectClient.Height()-(cyCmb+cyTlb)-2, SWP_NOACTIVATE|SWP_NOZORDER);
		}
		if(m_pMapPOUListCtrl!=nullptr&&m_pMapPOUListCtrl->m_hWnd!=nullptr)
		{
			m_pMapPOUListCtrl->SetWindowPos(nullptr, rectClient.left+1, rectClient.top+cyTlb+cyCmb+1, rectClient.Width()-2, rectClient.Height()-(cyCmb+cyTlb)-2, SWP_NOACTIVATE|SWP_NOZORDER);
		}
	}
}

void CPOUPane::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(200, 200, 200));
	dc.Draw3dRect(rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CPOUPane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	if(m_pAtlPOUListCtrl!=nullptr&&m_pAtlPOUListCtrl->IsWindowVisible()==TRUE)
	{
		m_pAtlPOUListCtrl->SetFocus();
	}
	else if(m_pMapPOUListCtrl!=nullptr&&m_pMapPOUListCtrl->IsWindowVisible()==TRUE)
	{
		m_pMapPOUListCtrl->SetFocus();
	}
}

void CPOUPane::SetAtlCtrl(CPOUListCtrl* pPOUListCtrl)
{
	if(pPOUListCtrl==m_pAtlPOUListCtrl)
		return;

	if(m_pAtlPOUListCtrl!=nullptr)
	{
		m_pAtlPOUListCtrl->ShowWindow(SW_HIDE);
		m_pAtlPOUListCtrl = nullptr;

		m_ComboBox.EnableWindow(TRUE);
		const int nItem = m_ComboBox.FindString(0, _T("Atlas POU"));
		if(nItem!=CB_ERR)
		{
			m_ComboBox.DeleteString(nItem);
		}
	}

	if(pPOUListCtrl!=nullptr)
	{
		CRect rectClient;
		GetClientRect(rectClient);

		CRect rectCombo;
		m_ComboBox.GetWindowRect(&rectCombo);
		const int cyCmb = rectCombo.Size().cy;
		const int cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
		/*
				m_ComboBox.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyCmb, SWP_NOACTIVATE | SWP_NOZORDER);
				m_ToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top+cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
		*/
		pPOUListCtrl->SetWindowPos(nullptr, rectClient.left+1, rectClient.top+cyCmb+cyTlb+1, rectClient.Width()-2, rectClient.Height()-(cyCmb+cyTlb)-2, SWP_NOACTIVATE|SWP_NOZORDER);
		m_pAtlPOUListCtrl = pPOUListCtrl;

		if(m_ComboBox.FindString(0, _T("Atlas POU"))==CB_ERR)
		{
			m_ComboBox.EnableWindow(TRUE);
			const int nItem = m_ComboBox.InsertString(0, _T("Atlas POU"));
			m_ComboBox.SetCurSel(nItem);
		}
	}

	if(m_ComboBox.GetCount()<2)
		m_ComboBox.EnableWindow(FALSE);
	else
		m_ComboBox.EnableWindow(TRUE);

	OnTopicChanged();
}

void CPOUPane::SetMapCtrl(CPOUListCtrl* pPOUListCtrl)
{
	if(pPOUListCtrl==m_pMapPOUListCtrl)
		return;

	if(m_pMapPOUListCtrl!=nullptr)
	{
		m_pMapPOUListCtrl->ShowWindow(SW_HIDE);
		m_pMapPOUListCtrl = nullptr;

		m_ComboBox.EnableWindow(TRUE);
		const int nItem = m_ComboBox.FindString(0, _T("Map POU"));
		if(nItem!=-1)
		{
			m_ComboBox.DeleteString(nItem);
		}
	}

	if(pPOUListCtrl!=nullptr)
	{
		CRect rectClient;
		GetClientRect(rectClient);

		CRect rectCombo;
		m_ComboBox.GetWindowRect(&rectCombo);
		const int cyCmb = rectCombo.Size().cy;
		const int cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
		/*
				m_ComboBox.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyCmb, SWP_NOACTIVATE | SWP_NOZORDER);
				m_ToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top+cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);*/

		pPOUListCtrl->SetWindowPos(nullptr, rectClient.left+1, rectClient.top+cyCmb+cyTlb+1, rectClient.Width()-2, rectClient.Height()-(cyCmb+cyTlb)-2, SWP_NOACTIVATE|SWP_NOZORDER);
		m_pMapPOUListCtrl = pPOUListCtrl;

		if(m_ComboBox.FindString(0, _T("Map POU"))==CB_ERR)
		{
			m_ComboBox.EnableWindow(TRUE);
			const int nItem = m_ComboBox.AddString(_T("Map POU"));
			m_ComboBox.SetCurSel(nItem);
		}
	}

	if(m_ComboBox.GetCount()<2)
		m_ComboBox.EnableWindow(FALSE);
	else
		m_ComboBox.EnableWindow(TRUE);

	OnTopicChanged();
}

void CPOUPane::OnTopicChanged()
{
	const int nItem = m_ComboBox.GetCurSel();
	if(nItem==LB_ERR)
		return;

	CString strName;
	m_ComboBox.GetLBText(nItem, strName);
	const int topic = strName=="Atlas POU" ? 0 : (strName=="Map POU" ? 1 : -1);
	switch(topic)
	{
	case 0:
	{
		CMFCToolBarButton* pButton = m_ToolBar.GetButton(0);
		if(pButton!=nullptr&&pButton->IsKindOf(RUNTIME_CLASS(CMFCToolBarMenuButton)))
		{
			((CMFCToolBarMenuButton*)pButton)->SetMessageWnd(m_pAtlPOUListCtrl);
		}
		m_ToolBar.SetOwner(m_pAtlPOUListCtrl);

		if(m_pMapPOUListCtrl!=nullptr)
		{
			m_pMapPOUListCtrl->ShowWindow(SW_HIDE);
		}
		if(m_pAtlPOUListCtrl!=nullptr)
		{
			m_pAtlPOUListCtrl->ShowWindow(SW_SHOW);
		}
	}
	break;
	case 1:
	{
		CMFCToolBarButton* pButton = m_ToolBar.GetButton(0);
		if(pButton!=nullptr&&pButton->IsKindOf(RUNTIME_CLASS(CMFCToolBarMenuButton)))
		{
			((CMFCToolBarMenuButton*)pButton)->SetMessageWnd(m_pMapPOUListCtrl);
		}
		m_ToolBar.SetOwner(m_pMapPOUListCtrl);

		if(m_pAtlPOUListCtrl!=nullptr)
		{
			m_pAtlPOUListCtrl->ShowWindow(SW_HIDE);
		}
		if(m_pMapPOUListCtrl!=nullptr)
		{
			m_pMapPOUListCtrl->ShowWindow(SW_SHOW);
		}
	}
	break;
	}
}

BOOL CPOUPane::DestroyWindow()
{
	const HICON hIcon = CDockablePane::GetIcon(FALSE);
	if(hIcon!=nullptr)
	{
		DestroyIcon(hIcon);
	}

	m_ToolBar.SetOwner(this);
	m_ToolBar.CleanUpImages();
	m_ToolBar.CleanUpLockedImages();

	return CDockablePane::DestroyWindow();
}
