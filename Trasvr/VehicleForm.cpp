// VehicleForm.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Trasvr.h"
#include "VehicleForm.h"

#include "ChildView.h"
#include "..\Socket\GpsSinkSocket.h"
#include "Global.h"

#include "..\Coding\Instruction.h"
#include "..\Coding\Coder.h"

#include "..\Socket\ExStdCFunction.h"
#include ".\vehicleform.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CVehicleForm �Ի���

IMPLEMENT_DYNAMIC(CVehicleForm, CDialog)
CVehicleForm::CVehicleForm(CWnd* pParent /*=nullptr*/)
: CDialog(CVehicleForm::IDD, pParent)
{
	m_pParentView = (CChildView*)pParent;
}

CVehicleForm::~CVehicleForm()
{
}

void CVehicleForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_listCtrl);
	DDX_Control(pDX, IDC_COMBOBOX, m_comInspector);
}

BEGIN_MESSAGE_MAP(CVehicleForm, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnNMClickList)
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST, OnNMRdblclkList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnNMRclickList)
	ON_CBN_SELCHANGE(IDC_COMBOBOX, OnCbnSelchangeCombobox)

	ON_COMMAND(ID_VEHICLE_DISINSPECT,OnDisInspect)
	ON_COMMAND(ID_VEHICLE_CHANGEUSER,OnChangeUser)
	ON_COMMAND(ID_VEHICLE_ASSIGNUSER,OnAssignUser)
END_MESSAGE_MAP()

BOOL CVehicleForm::OnInitDialog()
{
	CDialog::OnInitDialog();

	COMBOBOXEXITEM item;
	item.mask = CBEIF_TEXT | CBEIF_LPARAM;			
	item.iItem = 0;
	item.pszText = false ? _T("ȫ��") : _T("All");
	item.lParam = 0;
	m_comInspector.InsertItem(&item);	
	
	::SetWindowLong(m_listCtrl.GetHeaderCtrl()->m_hWnd, GWL_STYLE, WS_VISIBLE);
	ListView_SetExtendedListViewStyle(m_listCtrl.m_hWnd,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_listCtrl.SetTextBkColor(GetSysColor(COLOR_BACKGROUND)); 
	m_listCtrl.SetTextColor(RGB(255,255,255));

	m_listCtrl.InsertColumn(0, false ? _T("����ID") : _T("Vechile ID"));
	m_listCtrl.SetColumnWidth(0, 80);
	m_listCtrl.InsertColumn(1, false ? _T("��������") : _T("Driver"));
	m_listCtrl.SetColumnWidth(1, 80);
	m_listCtrl.InsertColumn(2, false ? _T("���ƺ���") : _T("Plate"));
	m_listCtrl.SetColumnWidth(2, 80);
	m_listCtrl.InsertColumn(3, false ? _T("���Ա") : _T("Supervisor"));
	m_listCtrl.SetColumnWidth(3, 80);
	m_listCtrl.InsertColumn(4, false ? _T("����") : _T("Trace"));
	m_listCtrl.SetColumnWidth(4, 40);
	m_listCtrl.InsertColumn(5, false ? _T("����") : _T("Alert"));
	m_listCtrl.SetColumnWidth(5, 40);
	m_listCtrl.InsertColumn(6, false ? _T("״̬") : _T("Status"));
	m_listCtrl.SetColumnWidth(6, 40);
	m_listCtrl.InsertColumn(7, false ? _T("���һ�λش�����ʱ��") : _T("Last time"));
	m_listCtrl.SetColumnWidth(7, 80);	

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

// CVehicleForm ��Ϣ�������
void CVehicleForm::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if(m_listCtrl.m_hWnd != nullptr)
	{
		m_listCtrl.SetWindowPos(&wndTop, 0, 0, cx, cy-30, SWP_NOMOVE);
	}
}

void CVehicleForm::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

void CVehicleForm::OnNMRdblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

void CVehicleForm::OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW *pnmListView = (NM_LISTVIEW *)pNMHDR;
	int index = pnmListView->iItem ;
	if(index == -1)
		return;

	VehicleStatus* status = (VehicleStatus*)m_listCtrl.GetItemData(index);
	if(status == nullptr)
		return;

	CPoint point;
	VERIFY(::GetCursorPos(&point));
	
	CMenu menu;
	if(menu.LoadMenu(IDR_POPMENU3) == FALSE)
		return;
	CMenu* pSubMenu = menu.GetSubMenu(0);
	if(pSubMenu == nullptr)
		return;

	if(status->m_pSocket != nullptr)
	{
		pSubMenu->RemoveMenu(ID_VEHICLE_ASSIGNUSER,MF_BYCOMMAND);
	}

	UINT SelectionMode = pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL | TPM_NONOTIFY | TPM_RETURNCMD,point.x, point.y, this);

	pSubMenu->DestroyMenu();
	menu.DestroyMenu();

	switch(SelectionMode)
	{			
	case ID_VEHICLE_DISINSPECT:
		{
			OnDisInspect();
		}
		break;
	case ID_VEHICLE_CHANGEUSER:
		{
			OnChangeUser();
		}
		break;
	case ID_VEHICLE_ASSIGNUSER:
		{
			OnAssignUser();
		}
		break;
	default:
		break;
	}

	*pResult = 0;
}

void CVehicleForm::OnCbnSelchangeCombobox()
{
	int index = m_comInspector.GetCurSel();
	if(index != -1)
	{
		CGpsSinkSocket* pSocket = (CGpsSinkSocket*)m_comInspector.GetItemData(index);
		ShowInspectVehicles(pSocket);
	}
}

void CVehicleForm::AddClient(CGpsSinkSocket* pSocket)
{
	if(pSocket == nullptr)
		return;

	if(pSocket->IsLogin() == TRUE) 
	{
		COMBOBOXEXITEM item;
		item.mask = CBEIF_TEXT | CBEIF_LPARAM;		
		item.iItem = m_comInspector.GetCount();
		item.pszText = (LPSTR)(LPCTSTR)pSocket->GetUserName();
		item.lParam = (DWORD)pSocket;
		m_comInspector.InsertItem(&item);
	}
}

void CVehicleForm::RemoveClient(CGpsSinkSocket* pSocket)
{
	int nCurent = m_comInspector.GetCurSel();
	
	int nMatch = -1;
	for(int index=1;index<m_comInspector.GetCount();index++)
	{
		CString strName;
		m_comInspector.GetLBText(index,strName);
		if(strName == pSocket->GetUserName())
		{
			m_comInspector.DeleteItem(index);
			nMatch = index;
			break;
		}
	}
	
	if(nCurent == 0 || nCurent == nMatch)
	{
		m_listCtrl.DeleteAllItems();
	
		int nCount = m_comInspector.GetCount();
		if(nCurent >= nCount)
		{
			m_comInspector.SetCurSel(nCount-1);
		}
		else
		{
			m_comInspector.SetCurSel(nCurent);
		}

		this->OnCbnSelchangeCombobox();
	}
}

// ��ʾ��ǰ��صĳ���
void CVehicleForm::AddVehicle(CGpsSinkSocket* pSocket,VehicleStatus* pVhcStatus)
{
	if(pVhcStatus == nullptr)
		return;
	
	int index = m_comInspector.GetCurSel();
	if(index == -1)
		return;

	CGpsSinkSocket* socket = (CGpsSinkSocket*)m_comInspector.GetItemData(index);
	if(socket == nullptr || socket == pSocket)
	{
		CString str;
		str.Format(_T("%d"), pVhcStatus->m_pVhcInfo->m_dwId);					

		LVITEM lvi;
		lvi.mask = LVIF_IMAGE | LVIF_TEXT;
		lvi.iItem = m_listCtrl.GetItemCount();
		lvi.iSubItem = 0;
		lvi.pszText = (LPTSTR)(LPCTSTR)(str);
		int nItem = m_listCtrl.InsertItem(&lvi);

		m_listCtrl.SetItemText(nItem, 1, pVhcStatus->m_pVhcInfo->m_strDriver);
		m_listCtrl.SetItemText(nItem, 2, pVhcStatus->m_pVhcInfo->m_strCode);
		m_listCtrl.SetItemText(nItem, 3, pVhcStatus->m_pSocket->GetUserName());
		m_listCtrl.SetItemText(nItem, 4, (pVhcStatus->m_bTrace ? _T("��") : _T("��")));
		m_listCtrl.SetItemText(nItem, 5, (pVhcStatus->m_bAlarm ? _T("��") : _T("��")));
//		m_listCtrl.SetItemText(nItem, 6, pVhcStatus->m_dwStatus);��Ҫ�ָ�
		if(pVhcStatus->m_timeLast>100)
			m_listCtrl.SetItemText(nItem, 7, pVhcStatus->m_timeLast.Format(_T("%Y/%m/%d %H:%M:%S")));
		
		m_listCtrl.SetItemData(nItem, (DWORD)pVhcStatus);
	}
}

void CVehicleForm::RemoveVehicle(CGpsSinkSocket* pSocket,VehicleStatus* pVhcStatus)
{
	if(pVhcStatus == nullptr) 	
		return;

	int index = m_comInspector.GetCurSel();
	if(index == -1)
		return;

	CGpsSinkSocket* socket = (CGpsSinkSocket*)m_comInspector.GetItemData(index);
	if(socket == nullptr || socket == pSocket)
	{
		int count = m_listCtrl.GetItemCount();
		for(int index=0;index<count;index++)
		{
			VehicleStatus* state = (VehicleStatus*)m_listCtrl.GetItemData(index);
			if(pVhcStatus == state)
			{
				m_listCtrl.DeleteItem(index);
				break;
			}
		}
	}
}

void CVehicleForm::AddOrphan(VehicleStatus* pVhcStatus)
{
	COLORREF oldColor =  m_listCtrl.GetTextBkColor();
	m_listCtrl.SetTextBkColor(RGB(255,0,0)); 

	CString str;
	str.Format(_T("%d"), pVhcStatus->m_pVhcInfo->m_dwId);					

	LVITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_TEXT;
	lvi.iItem = m_listCtrl.GetItemCount();
	lvi.iSubItem = 0;
	lvi.pszText = (LPTSTR)(LPCTSTR)(str);
	int nItem = m_listCtrl.InsertItem(&lvi);

	m_listCtrl.SetItemText(nItem, 1, pVhcStatus->m_pVhcInfo->m_strDriver);
	m_listCtrl.SetItemText(nItem, 2, pVhcStatus->m_pVhcInfo->m_strCode);
	m_listCtrl.SetItemText(nItem, 4, (pVhcStatus->m_bTrace ? _T("��") : _T("��")));
	m_listCtrl.SetItemText(nItem, 5, (pVhcStatus->m_bAlarm ? _T("��") : _T("��")));
//	m_listCtrl.SetItemText(nItem, 6, pVhcStatus->m_dwStatus);��Ҫ�ָ�
	if(pVhcStatus->m_timeLast>100)
		m_listCtrl.SetItemText(nItem, 7, pVhcStatus->m_timeLast.Format(_T("%Y/%m/%d %H:%M:%S")));
	
	m_listCtrl.SetItemData(nItem, (DWORD)pVhcStatus);

	m_listCtrl.SetTextBkColor(oldColor); 
}

void CVehicleForm::RemoveOrphan(VehicleStatus* pVhcStatus)
{
	int count = m_listCtrl.GetItemCount();
	for(int index=0;index<count;index++)
	{
		VehicleStatus* state = (VehicleStatus*)m_listCtrl.GetItemData(index);
		if(pVhcStatus == state)
		{
			m_listCtrl.DeleteItem(index);
			break;
		}
	}
}

// ��ʾ��ǰ��صĳ���
void CVehicleForm::ShowInspectVehicles(CGpsSinkSocket* pSocket)
{
	m_listCtrl.DeleteAllItems();
	if(pSocket == nullptr)
	{
		m_comInspector.SetCurSel(0);

		for(int i=1;i<m_comInspector.GetCount();i++)
		{
			CGpsSinkSocket* pSocket = (CGpsSinkSocket*)m_comInspector.GetItemData(i);
			CVehicleMap::iterator it = pSocket->m_mapIdVehicle.begin();
			while(it != pSocket->m_mapIdVehicle.end())
			{
				VehicleStatus* pVhcStatus = it->second;
				it++;
				if(pVhcStatus == nullptr) 
					continue;

				CString strMsg;
				strMsg.Format(_T("%d"), pVhcStatus->m_pVhcInfo->m_dwId);					

				LVITEM lvi;
				lvi.mask =  LVIF_IMAGE | LVIF_TEXT;
				lvi.iItem = m_listCtrl.GetItemCount();
				lvi.iSubItem = 0;
				lvi.pszText = (LPTSTR)(LPCTSTR)(strMsg);
				int nItem = m_listCtrl.InsertItem(&lvi);				
				
				m_listCtrl.SetItemText(nItem, 1, pVhcStatus->m_pVhcInfo->m_strDriver);
				m_listCtrl.SetItemText(nItem, 2, pVhcStatus->m_pVhcInfo->m_strCode);
				m_listCtrl.SetItemText(nItem, 3, pVhcStatus->m_pSocket->GetUserName());
				m_listCtrl.SetItemText(nItem, 4, (pVhcStatus->m_bTrace ? _T("��") : _T("��")));
				m_listCtrl.SetItemText(nItem, 5, (pVhcStatus->m_bAlarm ? _T("��") : _T("��")));
//��Ҫ�ָ�		m_listCtrl.SetItemText(nItem, 6, pVhcStatus->m_dwStatus);
				if(pVhcStatus->m_timeLast>100)
					m_listCtrl.SetItemText(nItem, 7, pVhcStatus->m_timeLast.Format(_T("%Y/%m/%d %H:%M:%S")));
				
				m_listCtrl.SetItemData(nItem, (DWORD)pVhcStatus);
			}
		}
	}
	else
	{
		CString strUser = pSocket->GetUserName();
		for(int index=1;index<m_comInspector.GetCount();index++)
		{
			CString str;
			m_comInspector.GetLBText(index,str);
			if(str == strUser)
			{
				m_comInspector.SetCurSel(index);
				break;
			}
		}

		CVehicleMap::iterator it = pSocket->m_mapIdVehicle.begin();
		while(it != pSocket->m_mapIdVehicle.end())
		{
			VehicleStatus* pVhcStatus = (VehicleStatus*)it->second;
			it++;
			
			if(pVhcStatus == nullptr) 
				continue;

			CString strMsg;
			strMsg.Format(_T("%d"), pVhcStatus->m_pVhcInfo->m_dwId);				

			LVITEM lvi;
			lvi.mask =  LVIF_IMAGE | LVIF_TEXT;
			lvi.iItem = m_listCtrl.GetItemCount();
			lvi.iSubItem = 0;
			lvi.pszText = (LPTSTR)(LPCTSTR)(strMsg);
			int nItem = m_listCtrl.InsertItem(&lvi);
			
			m_listCtrl.SetItemText(nItem, 1, pVhcStatus->m_pVhcInfo->m_strDriver);
			m_listCtrl.SetItemText(nItem, 2, pVhcStatus->m_pVhcInfo->m_strCode);
			m_listCtrl.SetItemText(nItem, 3, pVhcStatus->m_pSocket->GetUserName());
			m_listCtrl.SetItemText(nItem, 4, (pVhcStatus->m_bTrace ? _T("��") : _T("��")));
			m_listCtrl.SetItemText(nItem, 5, (pVhcStatus->m_bAlarm ? _T("��") : _T("��")));
//��Ҫ�ָ�	m_listCtrl.SetItemText(nItem, 6, pVhcStatus->m_dwStatus);
			if(pVhcStatus->m_timeLast>100)
			{
				m_listCtrl.SetItemText(nItem, 7, pVhcStatus->m_timeLast.Format(_T("%Y/%m/%d %H:%M:%S")));
			}

			m_listCtrl.SetItemData(nItem, (DWORD)pVhcStatus);
		}
	}

	if(m_pParentView != nullptr)
	{
		COLORREF oldColor = m_listCtrl.GetTextBkColor();
		m_listCtrl.SetTextBkColor(RGB(255,0,0)); 

		CVehicleMap::iterator it = m_pParentView->m_mapOrphanVehicle.begin();
		while(it != m_pParentView->m_mapOrphanVehicle.end())
		{
			VehicleStatus* pVhcStatus = (VehicleStatus*)it->second;
			it++;

			CString strMsg;
			strMsg.Format(_T("%d"), pVhcStatus->m_pVhcInfo->m_dwId);				

			LVITEM lvi;
			lvi.mask =  LVIF_IMAGE | LVIF_TEXT;
			lvi.iItem = m_listCtrl.GetItemCount();
			lvi.iSubItem = 0;
			lvi.pszText = (LPTSTR)(LPCTSTR)(strMsg);
			int nItem = m_listCtrl.InsertItem(&lvi);
			
			m_listCtrl.SetItemText(nItem, 1, pVhcStatus->m_pVhcInfo->m_strDriver);
			m_listCtrl.SetItemText(nItem, 2, pVhcStatus->m_pVhcInfo->m_strCode);
			m_listCtrl.SetItemText(nItem, 4, (pVhcStatus->m_bTrace ? _T("��") : _T("��")));
			m_listCtrl.SetItemText(nItem, 5, (pVhcStatus->m_bAlarm ? _T("��") : _T("��")));
//��Ҫ�ָ�	m_listCtrl.SetItemText(nItem, 6, pVhcStatus->m_dwStatus);
			if(pVhcStatus->m_timeLast>100)
			{
				m_listCtrl.SetItemText(nItem, 7, pVhcStatus->m_timeLast.Format(_T("%Y/%m/%d %H:%M:%S")));
			}

			m_listCtrl.SetItemData(nItem, (DWORD)pVhcStatus);
		}

		m_listCtrl.SetTextBkColor(oldColor); 
	}
}

void CVehicleForm::OnDisInspect()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	while(pos != nullptr)
	{
		int nItem = m_listCtrl.GetNextSelectedItem(pos);
		VehicleStatus* pVhcStatus = (VehicleStatus*)m_listCtrl.GetItemData(nItem);	
		if(pVhcStatus == nullptr) 
			continue;
		
		DWORD dwId = _ttoi(m_listCtrl.GetItemText(nItem, 0));
		if(pVhcStatus->m_pSocket != nullptr)
		{
			CVehicleMap::iterator it = pVhcStatus->m_pSocket->m_mapIdVehicle.find(dwId);
			if(it != pVhcStatus->m_pSocket->m_mapIdVehicle.end())	
			{
				pVhcStatus->m_pSocket->m_mapIdVehicle.erase(it);	
			}

			STRUCT_GPSSERVER_VEHICLE_EREASE* lp = new STRUCT_GPSSERVER_VEHICLE_EREASE(dwId);
			pVhcStatus->m_pSocket->SendMsg((char*)lp, sizeof(*lp));
			CString strMsg;
			strMsg.Format(_T("���Ա\"%s\"��صĳ���\"%d\"���ӷ���������ֹ���"), pVhcStatus->m_pSocket->GetUserName(), dwId);
			pVhcStatus->m_pSocket->SendStringToClient(dwId,(LPTSTR)(LPCTSTR)strMsg);	
		}
		else
		{
			if(m_pParentView != nullptr)
			{
				CVehicleMap::iterator it = m_pParentView->m_mapOrphanVehicle.find(dwId);
				if(it != m_pParentView->m_mapOrphanVehicle.end())
				{
					m_pParentView->m_mapOrphanVehicle.erase(it);
				}
			}
		}

		m_listCtrl.DeleteItem(nItem);

		delete pVhcStatus;
		pVhcStatus = nullptr;
	}
}

void CVehicleForm::OnChangeUser()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	while(pos != nullptr)
	{
		int nItem = m_listCtrl.GetNextSelectedItem(pos);
		VehicleStatus* pVhcStatus = (VehicleStatus*)m_listCtrl.GetItemData(nItem);
		if(pVhcStatus == nullptr) 
			continue;

		if(pVhcStatus->m_pSocket == nullptr)
			continue;

		if(false)
		{
			DWORD dwId = _ttoi(m_listCtrl.GetItemText(nItem, 0));
			CVehicleMap::iterator it = pVhcStatus->m_pSocket->m_mapIdVehicle.find(dwId);
			if(it == pVhcStatus->m_pSocket->m_mapIdVehicle.end())
				continue;

			pVhcStatus->m_pSocket->m_mapIdVehicle.erase(it);
	
			//��һ��û����
			{
				CGpsSinkSocket* pSocket;
				pVhcStatus->m_pSocket = pSocket;
				pSocket->m_mapIdVehicle[dwId] = pVhcStatus;

				m_listCtrl.SetItemText(nItem, 3, pSocket->GetUserName());
			}
		}
	}
}

void CVehicleForm::OnAssignUser()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	while(pos != nullptr)
	{
		int nItem = m_listCtrl.GetNextSelectedItem(pos);
		VehicleStatus* pVhcStatus = (VehicleStatus*)m_listCtrl.GetItemData(nItem);	
		if(pVhcStatus == nullptr) 
			continue;

		DWORD dwId = _ttoi(m_listCtrl.GetItemText(nItem, 0));
		CString str;
		if(pVhcStatus->m_pSocket != nullptr)
		{
			str.Format(_T("������%d�����ڱ����Ա��%s�������"), dwId, pVhcStatus->m_pSocket->GetUserName());
			Log_or_AfxMessageBox(str);
			continue;
		}
		
		if(m_pParentView == nullptr)
			continue;
		CGpsSinkSocket* pSocketMin = m_pParentView->GetEasiestClient(dwId,false);	
		if(pSocketMin == nullptr)
		{
			str.Format(_T("��ǰû�е�¼�ļ��Ա�ɹ�ʹ��,����û���ѵ�½�ļ��Ա��Ȩ�޼�ش˳�!"));
		}
		else if(m_pParentView->AskInspectOneVehicle(dwId, pSocketMin) == TRUE)
		{
			CVehicleMap::iterator it = m_pParentView->m_mapOrphanVehicle.find(dwId);
			if(it != m_pParentView->m_mapOrphanVehicle.end())
			{
				m_pParentView->m_mapOrphanVehicle.erase(it);
			}

			str.Format(_T("��������ָ�ɳ�����%d�������Ա��%s�����"), dwId, pSocketMin->GetUserName());
			pSocketMin->SendStringToClient(dwId, (LPTSTR)(LPCTSTR)str);

			pVhcStatus->m_pSocket = pSocketMin;
			pSocketMin->m_mapIdVehicle[dwId] = pVhcStatus;
			m_listCtrl.SetItemText(nItem, 3, pSocketMin->GetUserName());
		}
		else
		{
			str.Format(_T("��������ָ�ɼ�س�����%d��ʧ��"), dwId);
		}

		Log_or_AfxMessageBox(str);
	}
}

// ��ʾ��س����ĸ���״̬
// "��" "��"
void CVehicleForm::SetInspectedVehicleTrapMark(const DWORD& dwId, BOOL bMark, CGpsSinkSocket* pSocket)
{
	if(pSocket == nullptr)
		return;

	CVehicleMap::iterator it = pSocket->m_mapIdVehicle.find(dwId);
	if(it == pSocket->m_mapIdVehicle.end())
		return;

	VehicleStatus* pVhcStatus = (VehicleStatus*)it->second;
	if(pVhcStatus != nullptr)
	{
		pVhcStatus->m_bTrace = bMark;

		// �����ڼ�س������е�״ֵ̬
		for(int i=0; i<m_listCtrl.GetItemCount(); i++)
		{
			CString str = m_listCtrl.GetItemText(i, 0);
			CString user = m_listCtrl.GetItemText(i, 3);
			user.TrimRight(_T(" "));
			user.TrimLeft(_T(" "));
			if(dwId == atoi((LPCTSTR)str) && user == pSocket->GetUserName())
			{
				m_listCtrl.SetItemText(i, 4, (pVhcStatus->m_bTrace ? _T("��") : _T("��")));
				break;
			}
		}
	}
}

// ��ʾ��س����ľ���״̬
void CVehicleForm::SetInspectedVehicleWarningMark(const DWORD& dwId, BOOL bMark, DWORD dwStates, double time, DWORD mask)
{
	POSITION pos = m_pParentView->m_SinkSocketList.GetHeadPosition();
	while(pos != nullptr)
	{ 
		CGpsSinkSocket* pSocket = (CGpsSinkSocket*)(m_pParentView->m_SinkSocketList.GetNext(pos));
		if(pSocket == nullptr) 
			continue;

		CVehicleMap::iterator it = pSocket->m_mapIdVehicle.find(dwId);
		if(it == pSocket->m_mapIdVehicle.end()) 
			return;

		VehicleStatus* pVhcStatus = (VehicleStatus*)it->second;
		if(pVhcStatus != nullptr)
		{
			if((mask & STATE_WARNING) != 0)
				pVhcStatus->m_bAlarm = bMark;
			if((mask & STATE_STATE) != 0)
				pVhcStatus->m_dwStatus = dwStates;
			if((mask & STATE_TIME) != 0)
				pVhcStatus->m_timeLast = time;

			// �����ڼ�س������е�״ֵ̬
			for(int i=0; i<m_listCtrl.GetItemCount(); i++)
			{
				CString str = m_listCtrl.GetItemText(i, 0);
				CString user = m_listCtrl.GetItemText(i, 3);
				user.TrimRight(_T(" "));
				user.TrimLeft(_T(" "));
				if(dwId == atoi((LPCTSTR)str) && user == pSocket->GetUserName())
				{
					m_listCtrl.SetItemText(i, 5, (pVhcStatus->m_bAlarm ? _T("��") : _T("��")));
//��Ҫ�ָ�			m_listCtrl.SetItemText(i, 6, pVhcStatus->m_dwStatus);
					if(((double)pVhcStatus->m_timeLast) > 100.0f)
						m_listCtrl.SetItemText(i, 7, pVhcStatus->m_timeLast.Format(_T("%Y/%m/%d %H:%M:%S")));
					break;
				}
			}
		}
	}
}