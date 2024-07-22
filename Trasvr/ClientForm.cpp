// ClientForm.cpp : 实现文件
//

#include "stdafx.h"
#include "Trasvr.h"
#include "Global.h"
#include "ChildView.h"
#include "ClientForm.h"
#include "VehicleForm.h"
#include "MainFrm.h"

#include "..\Socket\GpsSinkSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CClientForm, CDialog)
CClientForm::CClientForm(CWnd* pParent /*=nullptr*/)
	: CDialog(CClientForm::IDD, pParent)
{
	m_pParentView = (CChildView*)pParent;
}

CClientForm::~CClientForm()
{
}

void CClientForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_LIST, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CClientForm, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnNMDblclkList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnNMRclickList)

	ON_COMMAND(ID_USER_DISCONNECT,OnDisconnectUser)
END_MESSAGE_MAP()

BOOL CClientForm::OnInitDialog()
{
	CDialog::OnInitDialog();

	::SetWindowLong(m_listCtrl.GetHeaderCtrl()->m_hWnd, GWL_STYLE, WS_VISIBLE);
	
	ListView_SetExtendedListViewStyle(m_listCtrl.m_hWnd, /*LVS_EX_CHECKBOXES|*/LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_listCtrl.SetTextBkColor(GetSysColor(COLOR_BACKGROUND)); 
	m_listCtrl.SetTextColor(RGB(255,255,255));
	
	m_listCtrl.InsertColumn(0, false ? _T("监控员") : _T("Supervisor"));
	m_listCtrl.SetColumnWidth(0, 80);
	m_listCtrl.InsertColumn(1, _T("IP"));
	m_listCtrl.SetColumnWidth(1, 160);
	m_listCtrl.InsertColumn(2, false ? _T("端口") : _T("Port"));
	m_listCtrl.SetColumnWidth(2, 80);
	m_listCtrl.InsertColumn(3, false ? _T("监控数量") : _T("Count"));
	m_listCtrl.SetColumnWidth(3, 80);
	m_listCtrl.InsertColumn(4, false ? _T("登录时间") : _T("Login time"));
	m_listCtrl.SetColumnWidth(4, 160);
	m_listCtrl.InsertColumn(5, false ? _T("终端类型") : _T("Client's type"));
	m_listCtrl.SetColumnWidth(5, 80);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
// CClientForm 消息处理程序

void CClientForm::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	if(m_listCtrl.m_hWnd != nullptr)
	{
		m_listCtrl.MoveWindow(0, 0, cx, cy);
	}
}

void CClientForm::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW *pnmListView = (NM_LISTVIEW *)pNMHDR;
	int index = pnmListView->iItem ;
	if(index == -1)
		return;
	
	CGpsSinkSocket* pSocket = (CGpsSinkSocket*)m_listCtrl.GetItemData(index);
	
	if(m_pParentView != nullptr)
	{
		m_pParentView->ChangeContent(2,(DWORD)pSocket);

		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		pMainFrame->m_menuTreeCtrl.ActiveItem(false ? _T("当前被监控车辆") : _T("Current monitoring vechiles"));
	}

	*pResult = 0;
}

void CClientForm::OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW *pnmListView = (NM_LISTVIEW *)pNMHDR;
	int index = pnmListView->iItem ;
	if(index == -1)
		return;

	CGpsSinkSocket* pSocket = (CGpsSinkSocket*)m_listCtrl.GetItemData(index);
	if(pSocket == nullptr)
		return;

	CPoint point;
	VERIFY(::GetCursorPos(&point));
	
	CMenu menu;
	if(menu.LoadMenu(IDR_POPMENU2) == FALSE)
		return;

	CMenu* pSubMenu = menu.GetSubMenu(0);
	if(pSubMenu == nullptr)
		return;

	if(pSocket->m_mapIdVehicle.size()==0)
	{
		pSubMenu->RemoveMenu(ID_USER_VEHICLES,MF_BYCOMMAND);
	}

	UINT SelectionMode = pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL | TPM_NONOTIFY | TPM_RETURNCMD,point.x, point.y, this);

	pSubMenu->DestroyMenu();
	menu.DestroyMenu();

	switch(SelectionMode)
	{			
	case ID_USER_DISCONNECT:
		{
			OnDisconnectUser();
		}
		break;
	case ID_USER_VEHICLES:
		{
			CGpsSinkSocket* pSocket = (CGpsSinkSocket*)m_listCtrl.GetItemData(index);
			
			if(m_pParentView != nullptr)
			{
				m_pParentView->ChangeContent(2,(DWORD)pSocket);

				CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
				pMainFrame->m_menuTreeCtrl.ActiveItem(false ? _T("当前被监控车辆") : _T("Current monitoring vechiles"));
			}
		}
		break;
	default:
		break;
	}

	*pResult = 0;
}

// 显示已经登录的监控员
void CClientForm::ShowLoginUsers(CPtrList& ClientList)
{	
	m_listCtrl.DeleteAllItems();
	
	POSITION pos =ClientList.GetHeadPosition();
	while(pos != nullptr)
	{
		CGpsSinkSocket* pSocket = (CGpsSinkSocket*)ClientList.GetNext(pos);
		if(pSocket != nullptr) 
		{
			if(pSocket->IsLogin() == FALSE)	
				continue;

			CString strMsg;
			strMsg.Format(_T("%s"), pSocket->GetUserName());
			
			LVITEM lvi;
			lvi.mask = LVIF_IMAGE | LVIF_TEXT;
			lvi.iItem = m_listCtrl.GetItemCount();
			lvi.iSubItem = 0;
			lvi.pszText = (LPTSTR)(LPCTSTR)(strMsg);
			int nItem = m_listCtrl.InsertItem(&lvi);
			
			m_listCtrl.SetItemText(nItem, 1, pSocket->GetClientIP());
			strMsg.Format(_T("%d"), pSocket->m_mapIdVehicle.size());
			m_listCtrl.SetItemText(nItem, 3, strMsg);
			
			strMsg = pSocket->GetLoadTimeString();
			m_listCtrl.SetItemText(nItem, 4, strMsg);
			
			m_listCtrl.SetItemText(nItem, 5, _T("Map"));
			
			m_listCtrl.SetItemData(nItem, (DWORD)pSocket);
		}
	}
}

void CClientForm::OnDisconnectUser() 
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	while(pos != nullptr)
	{
		int nItem = m_listCtrl.GetNextSelectedItem(pos);
		CGpsSinkSocket* pSocket = (CGpsSinkSocket*)m_listCtrl.GetItemData(nItem);
		if(pSocket == nullptr)	
			return;

		if(pSocket->m_mapIdVehicle.size()>0)
		{
			AfxMessageBox(_T("目前还有车辆被该用户监控，你不能断开该监控终端!"));
		}
		else
		{
			CString str = _T("与服务器的连接被从服务器端断开");
			pSocket->SendStringToClient(0,(LPTSTR)(LPCTSTR)str);
		
			STRUCT_SERVER_LOGOFF* lp = new STRUCT_SERVER_LOGOFF();
			pSocket->SendMsg((char*)lp, sizeof(*lp));

		//	pSocket->SendStringToClient(0,(LPTSTR)(LPCTSTR)str);
		
			if(m_pParentView != nullptr)
			{
				m_pParentView->PostMessage(WM_DELETECLIENT,(UINT)pSocket,0);
			}

			m_listCtrl.DeleteItem(nItem);
		}
	}
}

void CClientForm::AddClient(CGpsSinkSocket* pSocket)
{
	if(pSocket == nullptr) 	
		return;
	
	CString strMsg;
	strMsg.Format(_T("%s"), pSocket->GetUserName());
	
	LVITEM lvi;		
	lvi.mask =  LVIF_IMAGE | LVIF_TEXT;
	lvi.iItem = m_listCtrl.GetItemCount();
	lvi.iSubItem = 0;
	lvi.pszText = (LPTSTR)(LPCTSTR)(strMsg);
	int nItem = m_listCtrl.InsertItem(&lvi);
			
	m_listCtrl.SetItemText(nItem, 1, pSocket->GetClientIP());
	strMsg.Format(_T("%d"), pSocket->m_mapIdVehicle.size());
	m_listCtrl.SetItemText(nItem, 3, strMsg);
	
	strMsg = pSocket->GetLoadTimeString();
	m_listCtrl.SetItemText(nItem, 4, strMsg);

	m_listCtrl.SetItemText(nItem, 5, _T("Map"));
	
	m_listCtrl.SetItemData(nItem, (DWORD)pSocket);
}

void CClientForm::RemoveClient(CGpsSinkSocket *pSocket)					
{
	if(pSocket == nullptr) 	
		return;
	
	int count = m_listCtrl.GetItemCount();
	for(int index=0;index<count;index++)
	{
		CGpsSinkSocket* socket = (CGpsSinkSocket*)m_listCtrl.GetItemData(index);
		if(pSocket == socket)
		{
			m_listCtrl.SetItemData(index,0);
			m_listCtrl.DeleteItem(index);
			break;
		}
	}
}

void CClientForm::ChangeValue(CGpsSinkSocket *pSocket)
{
	int count = m_listCtrl.GetItemCount();
	for(int index=0;index<count;index++)
	{
		CGpsSinkSocket* socket = (CGpsSinkSocket*)m_listCtrl.GetItemData(index);
		if(pSocket == socket)
		{
			CString str;
			str.Format(_T("%d"), pSocket->m_mapIdVehicle.size());
			m_listCtrl.SetItemText(index, 3, str);
			break;
		}
	}
}