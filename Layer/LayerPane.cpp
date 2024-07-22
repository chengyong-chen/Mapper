#include "stdafx.h"
#include "Layer.h"
#include "LayerPane.h"

#include "LayerTreeCtrl.h"
#include "BackgroundBar.h"

#include "../Public/PaneMenuButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayerPane dialog
CLayerPane::CLayerPane(bool bEditable)
{
	m_pLayerTreeCtrl = nullptr;
	m_pBackgroundBar = nullptr;
	m_bEditable = bEditable;
}

BEGIN_MESSAGE_MAP(CLayerPane, CDockablePane)
	//{{AFX_MSG_MAP(CLayerPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CLayerPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(m_ToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PANE_LAYER) == TRUE)
	{
		m_ToolBar.LoadToolBar(IDR_PANE_LAYER, 0, 0, TRUE);
		m_ToolBar.CleanUpLockedImages();
		m_ToolBar.LoadBitmap(IDB_PANE_LAYER, 0, 0, TRUE);
		m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
		m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
		m_ToolBar.SetOwner(this);
		m_ToolBar.SetRouteCommandsViaFrame(FALSE);// All commands will be routed via this control , not via the parent frame:
	}

	CMenu menu;
	if(menu.LoadMenu(IDR_PANE_LAYER) == TRUE)
	{
		CMenu* pSubMenu1 = menu.GetSubMenu(0);
		if(pSubMenu1 != nullptr)
		{
			if(m_bEditable == false)
			{
				const int count = pSubMenu1->GetMenuItemCount();
				for(int index = count - 1; index >= 0; index--)
				{
					const UINT dwID = pSubMenu1->GetMenuItemID(index);
					switch(dwID)
					{
						case ID_LAYER_ALLON:
						case ID_LAYER_ALLOFF:
							break;
						default:
							pSubMenu1->DeleteMenu(index, MF_BYPOSITION);
							break;
					}
				}
			}
			const int iImage = m_ToolBar.GetButton(0)->GetImage();
			m_ToolBar.ReplaceButton(ID_LAYER_MENU1, CPaneMenuButton(pSubMenu1->GetSafeHmenu()));
			CMFCToolBarMenuButton* pButton = DYNAMIC_DOWNCAST(CPaneMenuButton, m_ToolBar.GetButton(0));
			if(pButton != nullptr)
			{
				pButton->SetImage(iImage);
				//	pButton->m_bText = FALSE;
				pButton->m_bImage = TRUE;
				pButton->SetMessageWnd(nullptr);
				pButton->EnableWindow(FALSE);
				pButton->m_strText = _T("Layer");
				m_ToolBar.AdjustSizeImmediate();
			}
		}
		CMenu* pSubMenu2 = menu.GetSubMenu(1);
		if(pSubMenu2 != nullptr)
		{
			const int iImage = m_ToolBar.GetButton(1)->GetImage();
			m_ToolBar.ReplaceButton(ID_LAYER_MENU2, CPaneMenuButton(pSubMenu2->GetSafeHmenu()));
			CMFCToolBarMenuButton* pButton = DYNAMIC_DOWNCAST(CPaneMenuButton, m_ToolBar.GetButton(1));
			if(pButton != nullptr)
			{
				pButton->SetImage(iImage);
				//	pButton->m_bText = FALSE;
				pButton->m_bImage = TRUE;
				pButton->SetMessageWnd(nullptr);
				pButton->EnableWindow(FALSE);
				m_ToolBar.AdjustSizeImmediate();
				pButton->m_strText = _T("Database");
			}
		}
	}

	if(m_bEditable == false)
	{
		m_ToolBar.RemoveButton(3);
		m_ToolBar.RemoveButton(2);
	}
	const HICON hIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_LAYER), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	CDockablePane::SetIcon(hIcon, FALSE);
	CDockablePane::EnableToolTips(TRUE);
	CDockablePane::EnableDocking(CBRS_ALIGN_LEFT);
	m_ToolBar.SetOwner(nullptr);

	if(m_pBackgroundBar == nullptr)
	{
		CBackgroundBar* pBackgroundBar = new CBackgroundBar();
		if(pBackgroundBar->Create(IDD_BACKGROUND, this) == TRUE)
		{
			pBackgroundBar->ShowWindow(SW_HIDE);
			m_pBackgroundBar = pBackgroundBar;
		}
		else
		{
			delete pBackgroundBar;
			pBackgroundBar = nullptr;
		}
	}
	CMFCToolBar::AddToolBarForImageCollection(IDR_LAYER_MENUITEMS, IDB_LAYER_MENUITEMS);
	return 0;
}

void CLayerPane::SetCtrl(CLayerTreeCtrl* pLayerTreeCtrl)
{
	if(m_pLayerTreeCtrl == pLayerTreeCtrl)
		return;

	CMFCToolBarMenuButton* pButton1 = DYNAMIC_DOWNCAST(CPaneMenuButton, m_ToolBar.GetButton(0));
	CMFCToolBarMenuButton* pButton2 = DYNAMIC_DOWNCAST(CPaneMenuButton, m_ToolBar.GetButton(1));
	CMFCToolBarButton* pButton3 = m_ToolBar.GetButton(2);
	CMFCToolBarButton* pButton4 = m_ToolBar.GetButton(3);
	if(m_pLayerTreeCtrl != nullptr)
	{
		if(pButton1 != nullptr)
		{
			pButton1->SetMessageWnd(nullptr);
			pButton1->EnableWindow(FALSE);
		}
		if(pButton2 != nullptr)
		{
			pButton2->SetMessageWnd(nullptr);
			pButton2->EnableWindow(FALSE);
		}
		if(pButton3 != nullptr)
		{
			pButton3->EnableWindow(FALSE);
		}
		if(pButton4 != nullptr)
		{
			pButton4->EnableWindow(FALSE);
		}

		m_ToolBar.SetOwner(nullptr);
		m_ToolBar.AdjustSizeImmediate();
		m_pLayerTreeCtrl->ShowWindow(SW_HIDE);
		m_pLayerTreeCtrl = nullptr;
	}

	if(pLayerTreeCtrl != nullptr)
	{
		CRect rectClient;
		GetClientRect(rectClient);

		CRect rectBack;
		if(m_pBackgroundBar != nullptr && m_pBackgroundBar->IsWindowVisible() == TRUE)
		{
			m_pBackgroundBar->GetClientRect(rectBack);
		}
		const int cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
		pLayerTreeCtrl->SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2 - rectBack.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
		pLayerTreeCtrl->ShowWindow(SW_SHOW);
		if(pButton1 != nullptr)
		{
			pButton1->EnableWindow(TRUE);
			pButton1->SetMessageWnd(pLayerTreeCtrl);
		}

		if(pButton2 != nullptr)
		{
			const CStringA strClassName = pLayerTreeCtrl->m_document.GetRuntimeClass()->m_lpszClassName;
			if(strClassName == "CGrfDoc")
			{
				pButton2->SetVisible(FALSE);
				pButton2->EnableWindow(FALSE);
			}
			else
			{
				pButton2->SetVisible(TRUE);
				pButton2->EnableWindow(TRUE);
				pButton2->SetMessageWnd(pLayerTreeCtrl);
			}
		}
		if(pButton3 != nullptr)
		{
			pButton3->EnableWindow(TRUE);
		}
		if(pButton4 != nullptr)
		{
			pButton4->EnableWindow(TRUE);
		}
		m_ToolBar.SetOwner(pLayerTreeCtrl);
		m_ToolBar.AdjustSizeImmediate();
		m_ToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
		m_pLayerTreeCtrl = pLayerTreeCtrl;
	}
}

void CLayerPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if(GetSafeHwnd() != nullptr)
	{
		CRect rectClient;
		GetClientRect(rectClient);
		const int cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
		m_ToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);

		int cyBack = 0;
		if(m_pBackgroundBar != nullptr)
		{
			CRect rectBack;
			m_pBackgroundBar->GetClientRect(rectBack);
			m_pBackgroundBar->SetWindowPos(nullptr, rectClient.left + 1, rectClient.Height() - rectBack.Height() - 1, rectClient.Width() - 2, rectBack.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
			cyBack = m_pBackgroundBar->IsWindowVisible() == TRUE ? rectBack.Height() : 0;
		}

		if(m_pLayerTreeCtrl != nullptr)
		{
			m_pLayerTreeCtrl->SetWindowPos(nullptr, 0, 0, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2 - cyBack, SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
		}
	}
}

void CLayerPane::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(200, 200, 200));
	dc.Draw3dRect(rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CLayerPane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	if(m_pLayerTreeCtrl != nullptr)
	{
		m_pLayerTreeCtrl->SetFocus();
	}
}

BOOL CLayerPane::DestroyWindow()
{
	const HICON hIcon = CDockablePane::GetIcon(FALSE);
	if(hIcon != nullptr)
	{
		DestroyIcon(hIcon);
	}
	if(m_pBackgroundBar != nullptr)
	{
		m_pBackgroundBar->PostMessage(WM_DESTROY, 0, 0);
		m_pBackgroundBar->DestroyWindow();
		delete m_pBackgroundBar;
		m_pBackgroundBar = nullptr;
	}

	m_ToolBar.SetOwner(nullptr);
	m_ToolBar.CleanUpImages();
	m_ToolBar.CleanUpLockedImages();

	return CDockablePane::DestroyWindow();
}

void CLayerPane::EnableBackgroundBar(BOOL bEnable)
{
	if(m_pBackgroundBar == nullptr)
		return;
	if(m_pBackgroundBar->IsWindowEnabled() == TRUE && m_pBackgroundBar->IsWindowVisible() == TRUE && bEnable == TRUE)
		return;
	else if(m_pBackgroundBar->IsWindowEnabled() == FALSE && m_pBackgroundBar->IsWindowVisible() == FALSE && bEnable == FALSE)
		return;
	else
	{
		m_pBackgroundBar->EnableWindow(bEnable);
		m_pBackgroundBar->ShowWindow(bEnable ? SW_SHOW : SW_HIDE);
		if(m_pLayerTreeCtrl != nullptr)
		{
			CRect rectBack;
			m_pBackgroundBar->GetClientRect(rectBack);

			CRect rectClient;
			GetClientRect(rectClient);
			const int cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;

			m_pLayerTreeCtrl->SetWindowPos(nullptr, 0, 0, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2 + (bEnable ? -rectBack.Height() : 0), SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
		}
	}
}
