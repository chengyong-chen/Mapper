// MapTreeCtrl.cpp: implementation of the CMenuTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "global.h"
#include "MenuTreeCtrl.h"
#include "ChildView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMenuTreeCtrl, CTreeCtrl)

BEGIN_MESSAGE_MAP(CMenuTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)
	ON_WM_SETCURSOR()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
	ON_WM_DELETEITEM()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTvnSelchanged)
END_MESSAGE_MAP()

CMenuTreeCtrl::CMenuTreeCtrl() 
{
	hHand = AfxGetApp()->LoadCursor(IDC_HANDCUR);

	m_pChildView = nullptr;
}

CMenuTreeCtrl::~CMenuTreeCtrl()
{
}

BOOL CMenuTreeCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	HTREEITEM hItem = HitTest(pt);

	if(0 != hItem &&
		!ItemHasChildren(hItem)) 
	{
		::SetCursor(hHand);
		return true;
	}
	
	return CTreeCtrl::OnSetCursor(pWnd, nHitTest, message);
}


void CMenuTreeCtrl::OnRClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// 先模拟一次左键单击
	// OnClick(pNMHDR, pResult);

	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	HTREEITEM hItem = HitTest(pt);
	if(0 == hItem)
		return;

	// 选择右击的项
	SelectItem(hItem);

	DWORD dw = GetItemData(hItem);

	if(dw != 0) // 在主窗口中弹出右键菜单
	{
		::SendMessage(AfxGetMainWnd()->m_hWnd,WM_CUSTOM_LEFTTREE_CONTEXTMENU, dw, 0L);
	}
	*pResult = 0;
}

int CMenuTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_imgList.Create(IDB_TREEIMAGE, 16, 0, RGB(0,128,128));
	SetImageList(&m_imgList, TVSIL_NORMAL);

	HTREEITEM hRoot = this->InsertItem(false ? _T("GPS车辆监控服务器") : _T("GPS Tracing server"), 4, 4); 
	if(hRoot != nullptr)
	{
		this->SetItemData(hRoot, -1);
		
		HTREEITEM hItem1 = this->InsertItem(false ? _T("监控窗") : _T("Monitoring window"), 0, 0, hRoot);
		if(hItem1 != nullptr)
		{
			this->SetItemData(hItem1, -1);

			HTREEITEM hSubItem1 = this->InsertItem(false ? _T("已登录监控员") : _T("Login Monitors"), 1, 1, hItem1);
			if(hSubItem1 != nullptr)
			{
				this->SetItemData(hSubItem1, 1);
			}

			HTREEITEM hSubItem2 = this->InsertItem(false ? _T("当前被监控车辆") : _T("Current monitoring vechiles"), 1, 1, hItem1);
			if(hSubItem2 != nullptr)
			{
				this->SetItemData(hSubItem2, 2);
			}

			HTREEITEM hSubItem3 = this->InsertItem(false ? _T("当前活动消息") : _T("Current active message"), 1, 1, hItem1);
			if(hSubItem3 != nullptr)
			{
				this->SetItemData(hSubItem3, 3);	
			}
		}
		
		HTREEITEM hItem2 = this->InsertItem(false ? _T("通信参数设置") : _T("Set the communication's arguments"), 0, 0, hRoot);
		if(hItem2 != nullptr)
		{
			this->SetItemData(hItem2, -1);
			
			HTREEITEM hSubItem = this->InsertItem(false ? _T("通讯参数") : _T("communication's arguments"), 1, 1, hItem2);
			if(hSubItem != nullptr)
			{
				this->SetItemData(hSubItem, 4);
			}
		}		

		HTREEITEM hItem3 = this->InsertItem(false ? _T("数据库配置") : _T("Database's setup"), 0, 0, hRoot);
		if(hItem3 != nullptr)
		{
			this->SetItemData(hItem3, -1);

			HTREEITEM hSubItem = this->InsertItem(false ? _T("配置数据库服务器") : _T("Server's setup"), 1, 1, hItem3);
			if(hSubItem != nullptr)
			{	
				this->SetItemData(hSubItem, 5);
			}
		}

		HTREEITEM hItem4 = this->InsertItem(false ? _T("车辆管理") : _T("Manage vechiles"), 0, 0, hRoot);
		if(hItem4 != nullptr)
		{
			this->SetItemData(hItem4, -1);

			HTREEITEM hSubItem1 = this->InsertItem(false ? _T("增加车辆") : _T("Insert new vechile"), 1, 1, hItem4);
			if(hSubItem1 != nullptr)
			{
				this->SetItemData(hSubItem1, 6);
			}

			HTREEITEM hSubItem2 = this->InsertItem(false ? _T("车辆查询") : _T("search vechile"), 1, 1, hItem4);
			if(hSubItem2 != nullptr)
			{
				this->SetItemData(hSubItem2, 7);
			}
		}

		HTREEITEM hItem5 = this->InsertItem(false ? _T("监控员管理") : _T("Supervisor's management"), 0, 0, hRoot);
		if(hItem5 != nullptr)
		{
			this->SetItemData(hItem5, -1);

			HTREEITEM hSubItem1 = this->InsertItem(false ? _T("分组") : _T("Group"), 1, 1, hItem5);
			if(hSubItem1 != nullptr)
			{
				this->SetItemData(hSubItem1, 8);
			}

			HTREEITEM hSubItem2 = this->InsertItem(false ? _T("监控员") : _T("Supervisors") , 1, 1, hItem5);
			if(hSubItem2 != nullptr)
			{
				this->SetItemData(hSubItem2, 9);
			}
		}

		
		HTREEITEM hItem6 = this->InsertItem(false ? _T("记录管理") : _T("Data's managment"), 0, 0, hRoot);
		if(hItem6 != nullptr)
		{
			this->SetItemData(hItem6, -1);

			HTREEITEM hSubItem1 = this->InsertItem(false ? _T("位置记录") : _T("Position"), 1, 1, hItem6);
			if(hSubItem1 != nullptr)
			{
				this->SetItemData(hSubItem1, 10);
			}

			HTREEITEM hSubItem2 = this->InsertItem(false ? _T("报警记录") : _T("Alert"), 1, 1, hItem6);
			if(hSubItem2 != nullptr)
			{
				this->SetItemData(hSubItem2, 11);
			}

			HTREEITEM hSubItem3 = this->InsertItem(false ? _T("短信记录") : _T("SMS"), 1, 1, hItem6);
			if(hSubItem3 != nullptr)
			{
				this->SetItemData(hSubItem3, 12);
			}
		}

		this->Expand(hRoot, TVE_EXPAND);
	}

	return 0;
}

void CMenuTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	AfxGetMainWnd()->SendMessage(WM_CUSTOM_LEFTTREE_CONTEXTMENU, 0, 0);
	// _ContextMenuTrack();
}

void CMenuTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
//	pForm->Create(id, &m_pChildView);
//	if(::IsWindow(pForm->m_hWnd))
//		pForm->ShowWindow(style);

	
	if(pNMTreeView->itemOld.hItem == 0)
	{
		*pResult = 0;
		return;
	}
	if(pNMTreeView->action == 0)
	{
		*pResult = 0;
		return;
	}

	HTREEITEM hItem=pNMTreeView->itemNew.hItem;	
	if(hItem ==nullptr)
		return;

	DWORD dwDate = this->GetItemData(hItem); 
	if(m_pChildView != nullptr)
	{
		m_pChildView->ChangeContent(dwDate);
	}

	*pResult = 0;
}


void CMenuTreeCtrl::ActiveItem(CString strItem)
{
	HTREEITEM hItem = this->GetRootItem();
	if(hItem == nullptr)
		return;

	do
	{
		CString str = this->GetItemText(hItem);
		if(str == strItem)
		{
			this-SelectItem(hItem);
			break;
		}
	}while((hItem=this->GetDownItem(hItem))!=nullptr);
}

HTREEITEM CMenuTreeCtrl::GetDownItem(HTREEITEM hitem)
{
	if(this->GetChildItem(hitem) != nullptr)
	{
		hitem = this->GetChildItem(hitem);
		return hitem;
	}
	
	if(this->GetNextSiblingItem(hitem) != nullptr)
	{
		hitem = this->GetNextSiblingItem(hitem);
		return hitem;
	}
	
	while(this->GetParentItem(hitem) != nullptr)
	{
		hitem = this->GetParentItem(hitem);
		if(this->GetNextSiblingItem(hitem) != nullptr)
		{
			hitem = this->GetNextSiblingItem(hitem);
			return hitem;
		}
	}
		
	return nullptr;
}