#include "stdafx.h"
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>

#include "columns.h"
#include "column.h"
#include "TabQueryEar.h"

#include "..//Mapper//Global.h"
#include "..//Viewer//Global.h"
#include "..//Public//Global.h"
#include "../Dataview/Global.h"
#include "../Dataview/Datainfo.h"
#include "../Dataview/Viewinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabQueryPage property page

IMPLEMENT_DYNCREATE(CTabQueryPage, CPropertyPage)

CTabQueryPage::CTabQueryPage()
	: CPropertyPage(CTabQueryPage::IDD)
{
	//{{AFX_DATA_INIT(CTabQueryPage)
	//}}AFX_DATA_INIT
}

void CTabQueryPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabQueryPage)
	DDX_Control(pDX, IDC_TABLETRDC, m_Rdc);
	DDX_Control(pDX, IDC_TABLETGRID, m_DBGridCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTabQueryPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTabQueryPage)
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_STREET, OnChangeStreet)
	ON_EN_CHANGE(IDC_TABLET, OnChangeTablet)
	ON_BN_CLICKED(IDC_LOCATE, OnLocate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabQueryPage message handlers

void CTabQueryPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	if(m_DBGridCtrl.m_hWnd!=nullptr)
	{
		CRect rect;
		m_DBGridCtrl.GetWindowRect(rect);
		this->ScreenToClient(rect);

		m_DBGridCtrl.SetWindowPos(&wndTop, 0, 0, cx, cy-rect.top, SWP_NOMOVE);
	}
}

BOOL CTabQueryPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	if(m_Rdc.m_hWnd!=nullptr)
	{
		IUnknown* pCursor = m_Rdc.GetDSCCursor();
		if(pCursor!=nullptr)
		{
			m_DBGridCtrl.SetDataSource(pCursor);
			m_DBGridCtrl.BindProperty(0x9, &m_Rdc);
			pCursor->Release();
		}
	}

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTabQueryPage::OnChangeStreet()
{
	CString strStreet;
	CString strTablet;

	GetDlgItem(IDC_STREET)->GetWindowText(strStreet);
	GetDlgItem(IDC_TABLET)->GetWindowText(strTablet);

	RefillGrid(strStreet, strTablet);
}

void CTabQueryPage::OnChangeTablet()
{
	CString strStreet;
	CString strTablet;

	GetDlgItem(IDC_STREET)->GetWindowText(strStreet);
	GetDlgItem(IDC_TABLET)->GetWindowText(strTablet);

	RefillGrid(strStreet, strTablet);
}

void CTabQueryPage::RefillGrid(CString strStreet, CString strTablet)
{
	CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
	if(pFrame==nullptr)
	{
		m_Rdc.SetConnect(nullptr);
		m_Rdc.SetSql(nullptr);
		m_Rdc.Refresh();
		//m_DBGridCtrl.Refresh();
		return;
	}

	CMDIChildWnd* pChildFrame = (CMDIChildWnd*)pFrame->GetActiveFrame();
	if(pChildFrame==nullptr)
	{
		m_Rdc.SetConnect(nullptr);
		m_Rdc.SetSql(nullptr);
		m_Rdc.Refresh();
		//	m_DBGridCtrl.Refresh();
		return;
	}

	CDocument* pDocument = (CDocument*)pChildFrame->GetActiveDocument();
	if(pDocument==nullptr)
	{
		m_Rdc.SetConnect(nullptr);
		m_Rdc.SetSql(nullptr);
		m_Rdc.Refresh();
		//m_DBGridCtrl.Refresh();
		return;
	}

	CString strPath = pDocument->GetPathName();
	int pos = strPath.ReverseFind(_T('\\'));
	if(pos==-1)
	{
		m_Rdc.SetConnect(nullptr);
		m_Rdc.SetSql(nullptr);
		m_Rdc.Refresh();
		//		m_DBGridCtrl.Refresh();
		return;
	}

	strPath = strPath.Left(pos+1);
	strPath += _T("Tablet.mdb");
	if(_taccess(strPath, 0)==-1)
	{
		m_Rdc.SetConnect(nullptr);
		m_Rdc.SetSql(nullptr);
		m_Rdc.Refresh();
		//m_DBGridCtrl.Refresh();
		return;
	}

	if(strStreet.IsEmpty()==TRUE&&strTablet.IsEmpty()==TRUE)
	{
		m_Rdc.SetConnect(nullptr);
		m_Rdc.SetSql(nullptr);
		m_Rdc.Refresh();
		//m_DBGridCtrl.Refresh();
		return;
	}

	CString strConnect;
	strConnect.Format(_T("DSN=%s;DBQ=%s;UID=%s;PWD=%s"), _T("MS Access Database"), strPath, _T(""), _T(""));
	CString strSQL;
	strSQL.Format(_T("SELECT Street,Tablet,Name,X,Y FROM %s WHERE Street Like '%c%s%c' And Tablet Like '%c%s%c'"), _T("ADDRESS"), '%', strStreet, '%', '%', strTablet, '%');

	m_Rdc.SetConnect(strConnect);
	m_Rdc.SetSql(strSQL);
	m_Rdc.Refresh();
	//m_DBGridCtrl.Refresh();

	VARIANT varX;
	VARIANT varY;
	varX.vt = VT_BSTR;
	varY.vt = VT_BSTR;
	varX.bstrVal = ::SysAllocString(L"X");
	varY.bstrVal = ::SysAllocString(L"Y");

	CColumns columns = m_DBGridCtrl.GetColumns();
	CColumn columnX = columns.GetItem(varX);
	CColumn columnY = columns.GetItem(varY);
	columnX.SetVisible(FALSE);

	columnY.SetVisible(FALSE);
}

void CTabQueryPage::OnLocate()
{
	CString strConnect = m_Rdc.GetConnect();
	CString strSQL = m_Rdc.GetSql();
	if(strConnect.IsEmpty()==TRUE||strSQL.IsEmpty()==TRUE)
		return;

	VARIANT varX;
	VARIANT varY;
	varX.vt = VT_BSTR;
	varY.vt = VT_BSTR;
	varX.bstrVal = ::SysAllocString(L"X");
	varY.bstrVal = ::SysAllocString(L"Y");
	CColumn columnX = m_DBGridCtrl.GetColumn(varX);
	CColumn columnY = m_DBGridCtrl.GetColumn(varY);
	varX = columnX.GetValue();
	varY = columnY.GetValue();
	CString strX = CString(varX.bstrVal);
	CString strY = CString(varY.bstrVal);

	static CPointF geoPoint;
	geoPoint.x = _tcstod(strX, nullptr);
	geoPoint.y = _tcstod(strY, nullptr);

	CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
	CMDIChildWnd* pChildFrame = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CDocument* pDocument = (CDocument*)pChildFrame->GetActiveDocument();
	if(pDocument==nullptr)
		return;

	CView* pView = nullptr;
	POSITION pos = pDocument->GetFirstViewPosition();
	if(pos!=nullptr)
	{
		CView* pView = pDocument->GetNextView(pos);
		if(pView!=nullptr)
		{
			VARIANT var1;
			VARIANT var2;
			VARIANT var3;
			var1.vt = VT_BSTR;
			var2.vt = VT_BSTR;
			var3.vt = VT_BSTR;
			var1.bstrVal = ::SysAllocString(L"Street");
			var2.bstrVal = ::SysAllocString(L"Tablet");
			var3.bstrVal = ::SysAllocString(L"Name");
			CColumn column1 = m_DBGridCtrl.GetColumn(var1);
			CColumn column2 = m_DBGridCtrl.GetColumn(var2);
			CColumn column3 = m_DBGridCtrl.GetColumn(var3);
			var1 = column1.GetValue();
			var2 = column2.GetValue();
			var3 = column3.GetValue();
			CString str1 = CString(var1.bstrVal);
			CString str2 = CString(var2.bstrVal);
			CString str3 = CString(var3.bstrVal);

			AfxGetMainWnd()->SendMessage(WM_SETGEOSIGN, (UINT)&geoPoint, 0);

			CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO, 0, 0);
			if(pViewinfo!=nullptr)
			{
				CPoint docPoint = pViewinfo->m_datainfo.GeoToDoc(geoPoint);
				CRect rClient;
				pView->GetClientRect(rClient);
				CPoint cliPoint = rClient.CenterPoint();
				pView->SendMessage(WM_FIXATEVIEW, (UINT)&docPoint, (LONG)&cliPoint);
			}
		}
	}
}

BEGIN_EVENTSINK_MAP(CTabQueryPage, CPropertyPage)
	//{{AFX_EVENTSINK_MAP(CTabQueryPage)
	ON_EVENT(CTabQueryPage, IDC_TABLETGRID, -601 /* DblClick */, OnDblClickGrid, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CTabQueryPage::OnDblClickGrid()
{
	OnLocate();
}
