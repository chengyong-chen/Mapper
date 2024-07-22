#include "stdafx.h"

#include "Global.h"
#include "SQLQueryPage.h"
#include "SQLDlg.h"

#include "../Geometry/Geom.h"
#include "../Viewer/Global.h"
#include "..\Mapper\Global.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSQLQueryPage dialog
IMPLEMENT_DYNCREATE(CSQLQueryPage, CPropertyPage)

CSQLQueryPage::CSQLQueryPage()
	: CPropertyPage(CSQLQueryPage::IDD)
{
	//{{AFX_DATA_INIT(CSQLQueryPage)

	//}}AFX_DATA_INIT
	m_strSQL.Empty();
}

void CSQLQueryPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSQLQueryPage)
	DDX_Control(pDX, IDC_LISTCTRL, m_listCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSQLQueryPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSQLQueryPage)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_QUERY, OnQuery)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LISTCTRL, OnDeleteItemListCtrl)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTCTRL, OnDblclkListCtrl)
	ON_BN_CLICKED(IDC_GETSQL, OnGetSQL)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETCOUNT,   OnSetCount)
END_MESSAGE_MAP()

BOOL CSQLQueryPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	LV_COLUMN lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt  = LVCFMT_LEFT;
	
	lvcolumn.iSubItem = 0;
	lvcolumn.pszText .LoadString(IDS_KEYNAME);
	lvcolumn.cx       = 150;
	m_listCtrl.InsertColumn(0,&lvcolumn);

	lvcolumn.iSubItem = 1;
	lvcolumn.pszText .LoadString(IDS_KEYMAP);
	lvcolumn.cx       = 100;
	m_listCtrl.InsertColumn(2,&lvcolumn);
	
	if(m_strSQL.IsEmpty() == TRUE)
	{
		((CButton*)GetDlgItem(IDC_QUERY))->EnableWindow(false);
	}

	return TRUE;  // return TRUE unless you set the focus to a control	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSQLQueryPage::OnOK()
{
}

void CSQLQueryPage::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);
	if(m_listCtrl.m_hWnd != nullptr)
	{
		m_listCtrl.SetWindowPos(&wndTop, 0, 0, cx, cy-90, SWP_NOMOVE);
	}
}

void CSQLQueryPage::OnQuery() 
{
	m_listCtrl.DeleteAllItems();

	AfxGetApp()->BeginWaitCursor();
	
	CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	int count = pAppFrame->SendMessage(WM_SQLQUERY,(UINT)&m_strSQL,(UINT)&m_listCtrl);
	OnSetCount(count,0);
	
	AfxGetApp()->EndWaitCursor();
}

void CSQLQueryPage::OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW *pnmListView = (NM_LISTVIEW *)pNMHDR;
	int index = pnmListView->iItem ;
	if(index == -1)
		return;

	PLACE* place = (PLACE*)m_listCtrl.GetItemData(index);
	if(place == nullptr)
		return;
	
	CFrameWnd* pMainFrame = (CFrameWnd*) AfxGetMainWnd();
	if(pMainFrame == nullptr)
	{
		AfxMessageBox(_T("No map file!"));
		*pResult = 0;
		return;
	}

	pMainFrame->SendMessage(WM_FLASHGEOM,3,(UINT)place);

	*pResult = 0;
}

void CSQLQueryPage::OnDeleteItemListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	PLACE* place = (PLACE*)pNMListView->lParam;
	delete place;
	
	*pResult = 0;
}

void CSQLQueryPage::OnGetSQL() 
{
	if(m_pDatabase == nullptr || m_pDatabase->IsOpen() == FALSE)
	{
		AfxMessageBox(_T("This map or atlas has no attached database��"));
		return;
	}

	HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst    = ::LoadLibrary(_T("Information.dll"));
	AfxSetResourceHandle(hInst);

	CSQLDlg dlg(this,m_pDatabase);
	if(dlg.DoModal() == IDOK)
	{
		CString strSQL;
		if(dlg.m_strWhere.IsEmpty() == FALSE)
			strSQL.Format(_T("Select %s FROM [%s] WHERE %s"),dlg.m_strColumns,dlg.m_strTable,dlg.m_strWhere);
		else
			strSQL.Format(_T("Select %s FROM [%s]"),dlg.m_strColumns,dlg.m_strTable);

		GetDlgItem(IDC_SQL)->SetWindowText(strSQL);
		m_strSQL = strSQL;
	}		

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
	
	if(m_strSQL.IsEmpty() == false)
	{
		((CButton*)GetDlgItem(IDC_QUERY))->EnableWindow(TRUE);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_QUERY))->EnableWindow(FALSE);
	}

}

LONG CSQLQueryPage::OnSetCount(UINT WPARAM, LONG LPARAM)
{	
	CString strCount;
	strCount.Format(_T("%d ��"),WPARAM);
	GetDlgItem(IDC_COUNT)->SetWindowText(strCount);
	return 0;
}

BOOL CSQLQueryPage::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
			return TRUE; 
			break;
		case VK_RETURN:
			OnQuery();
			break;
		}
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}
