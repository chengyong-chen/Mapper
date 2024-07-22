// MessageForm.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Trasvr.h"
#include "Global.h"

#include "MessageForm.h"

#include "..\Coding\Message.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CMessageForm, CDialog)
CMessageForm::CMessageForm(CWnd* pParent /*=nullptr*/)
	: CDialog(CMessageForm::IDD, pParent)
{
}

CMessageForm::~CMessageForm()
{
}

void CMessageForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CMessageForm, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnNMRclickList)
END_MESSAGE_MAP()

BOOL CMessageForm::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	::SetWindowLong(m_listCtrl.GetHeaderCtrl()->m_hWnd, GWL_STYLE, WS_VISIBLE);
	ListView_SetExtendedListViewStyle(m_listCtrl.m_hWnd, LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_listCtrl.SetTextBkColor(GetSysColor(COLOR_BACKGROUND)); 
	m_listCtrl.SetTextColor(RGB(255,255,255));

	m_listCtrl.InsertColumn(0, false ? _T("����ID") : _T("Vechile ID"));
	m_listCtrl.SetColumnWidth(0, 60);
	m_listCtrl.InsertColumn(1, false ? _T("����ʱ��") : _T("Receive time"));
	m_listCtrl.SetColumnWidth(1, 90);
	m_listCtrl.InsertColumn(2, false ? _T("��ȷ��λ") : _T("Loction"));
	m_listCtrl.SetColumnWidth(2, 60);
	m_listCtrl.InsertColumn(3, false ? _T("����ժҪ") : _T("Brief"));
	m_listCtrl.SetColumnWidth(3, 100);

	return TRUE;
}

// CMessageForm ��Ϣ�������
void CMessageForm::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if(m_listCtrl.m_hWnd != nullptr)
		m_listCtrl.MoveWindow(0, 0, cx, cy);
}

void CMessageForm::OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult)
{				
	CPoint point;
	VERIFY(::GetCursorPos(&point));
	HWND hWndTrack = m_hWnd; // GetOwner()->GetSafeHwnd();
	ASSERT(hWndTrack != nullptr && ::IsWindow(hWndTrack));

/*today	CExtPopupMenuWnd *pPopup = new CExtPopupMenuWnd();
	VERIFY(pPopup->CreatePopupMenu(hWndTrack));
	pPopup->g_bMenuExpanding = false;
	pPopup->g_bMenuShowCoolTips = true;
	pPopup->LoadMenu(hWndTrack, IDR_POPMENU1);

	::SetFocus(hWndTrack);
	pPopup->TrackPopupMenu(TPMX_OWNERDRAW_FIXED, point.x, point.y);
*/
	*pResult = 0;
}

void CMessageForm::ShowMessages()
{
}

// ����һ�����Ϣ��map��listctrl
void CMessageForm::AddMessage(CMessageReceived* pMsg)
{
	CString strMsg;
	LVITEM lvi;	
	
	// ID
	strMsg.Format(_T("%d"), pMsg->m_dwVehicle);
	lvi.mask =  LVIF_IMAGE | LVIF_TEXT;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.pszText = (LPTSTR)(LPCTSTR)(strMsg);
	m_listCtrl.InsertItem(&lvi);
	
	m_listCtrl.SetItemText(0, 2, pMsg->m_bAvailable == true ? _T("     ��") : _T("     ��"));

	m_listCtrl.SetItemText(0, 3, pMsg->m_strMessage);
	
	// ���Ϣ��ֻ����ǰ30��
	int count = m_listCtrl.GetItemCount();
	if(count > 100)
	{
		for(int i=0; i<count-100; i++)
			m_listCtrl.DeleteItem(100);
	}
}