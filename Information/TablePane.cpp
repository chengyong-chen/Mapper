#include "stdafx.h"
#include "resource.h"

#include "TablePane.h"

#include "../Public/Global.h"
#include "../Information/POUDBGridCtrl.h"
#include "../Information/POUTDBGridCtrl.h"
#include "../Information/ATTDBGridCtrl.h"
#include "../Information/ATTTDBGridCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTablePane::CTablePane(bool bEditable)
{
	m_pATTTDBGridCtrl = nullptr;
	m_pPOUTDBGridCtrl = nullptr;
	m_pATTDBGridCtrl = nullptr;
	m_pPOUDBGridCtrl = nullptr;

	m_pMonitorCtrl = nullptr;
	m_pMessageCtrl = nullptr;

	CTDBGridCtrl::m_bEditable = bEditable;
}

CTablePane::~CTablePane()
{
}

BEGIN_MESSAGE_MAP(CTablePane, CDockablePane)
	//{{AFX_MSG_MAP(CTablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTablePane message handlers
int CTablePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct)==-1)
		return -1;

	CMFCToolBarComboBoxButton::SetFlatMode();
	const CRect rectDummy(0, 0, 0, 0);
	//	rectDummy.SetRectEmpty();
	if(m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1)==FALSE)
	{
		TRACE0("Failed to create output tab window\n");
		return -1; // fail to create
	}
	const LONG nStyle = ::GetWindowLong(m_wndTabs.m_hWnd, GWL_EXSTYLE);
	::SetWindowLong(m_wndTabs.m_hWnd, GWL_EXSTYLE, nStyle&~WS_EX_CLIENTEDGE&~WS_BORDER);

	m_wndTabs.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
	m_wndTabs.HideNoTabs(TRUE);
	m_wndTabs.AutoDestroyWindow(FALSE);

	if(IsOCXRegistered("MSADODC.OCX")==FALSE)
	{
		RegisterOCX("MSADODC.OCX", Platform::X86, "");
	}
	const WCHAR wcLicenseKey[] = {0x0047, 0x004C, 0x0005, 0x0008, 0x0001, 0x0005,	0x0002, 0x0008, 0x0001, 0x0004};
	const BSTR bstrLicense = ::SysAllocStringLen(wcLicenseKey, sizeof(wcLicenseKey)/sizeof(WCHAR));
	if(IsOCXRegistered("TODG7.OCX", bstrLicense)==FALSE)
	{
		RegisterOCX("TODG7.OCX", Platform::X86, "todgub7.dll");
	}
	SysFreeString(bstrLicense);
	if(IsOCXRegistered("MSADODC.OCX")==TRUE&&IsOCXRegistered("TODG7.OCX")==TRUE)
	{
		CPOUTDBGridCtrl* pPOUTDBGridCtrl = new CPOUTDBGridCtrl();
		if(pPOUTDBGridCtrl->Create(nullptr, _T("Database table of user interesting points"), WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, CRect(0, 0, 0, 0), &(m_wndTabs), 3, nullptr)==TRUE)
		{
			const CString strName = GetTabLabel('P');
			m_wndTabs.AddTab(pPOUTDBGridCtrl, strName);

			m_pPOUTDBGridCtrl = pPOUTDBGridCtrl;
		}
		else
		{
			delete pPOUTDBGridCtrl;
			pPOUTDBGridCtrl = nullptr;
		}

		CATTTDBGridCtrl* pATTTDBGridCtrl = new CATTTDBGridCtrl();
		if(pATTTDBGridCtrl->Create(nullptr, _T("Database table associated with the map elements"), WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, CRect(0, 0, 0, 0), &(m_wndTabs), 2, nullptr)==TRUE)
		{
			const CString strName = GetTabLabel('A');
			m_wndTabs.AddTab(pATTTDBGridCtrl, strName);

			m_pATTTDBGridCtrl = pATTTDBGridCtrl;
		}
		else
		{
			delete pATTTDBGridCtrl;
			pATTTDBGridCtrl = nullptr;
		}
	}
	const HICON hIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_TABLE), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	CDockablePane::SetIcon(hIcon, FALSE);
	CDockablePane::EnableToolTips(TRUE);
	CDockablePane::EnableDocking(CBRS_ALIGN_BOTTOM);
	return 0;
}

CWnd* CTablePane::GetTabWnd(char cTab) const
{
	CWnd* pWnd = nullptr;
	switch(cTab)
	{
	case 'A':
		pWnd = (CWnd*)m_pATTTDBGridCtrl;
		break;
	case 'P':
		pWnd = (CWnd*)m_pPOUTDBGridCtrl;
		break;
	case 'J':
		pWnd = (CWnd*)m_pATTDBGridCtrl;
		break;
	case 'U':
		pWnd = (CWnd*)m_pPOUDBGridCtrl;
		break;
	case 'C':
		break;
	case 'G':
		break;
	case 'R':
		break;
	case 'I':
		break;
	default:
		break;
	}

	return pWnd;
}

void CTablePane::ActivateTab(char cTab)
{
	CWnd* pWnd = GetTabWnd(cTab);
	if(pWnd!=nullptr)
	{
		const int index = m_wndTabs.GetTabFromHwnd(pWnd->m_hWnd);
		if(index!=-1)
		{
			m_wndTabs.SetActiveTab(index);
		}
	}
}

void CTablePane::OnDestroy()
{
	m_wndTabs.RemoveAllTabs();
	const HICON hIcon = CDockablePane::GetIcon(FALSE);
	if(hIcon!=nullptr)
	{
		DestroyIcon(hIcon);
	}

	if(m_pATTTDBGridCtrl!=nullptr)
	{
		m_pATTTDBGridCtrl->PostMessage(WM_DESTROY, 0, 0);
		m_pATTTDBGridCtrl->DestroyWindow();//Do not use the C++ delete operator to destroy a frame window. Use CWnd::DestroyWindow instead. 
		//	delete m_pATTTDBGridCtrl;//Do not use the C++ delete operator to destroy a frame window. Use CWnd::DestroyWindow instead. 
		m_pATTTDBGridCtrl = nullptr;
	}
	if(m_pPOUTDBGridCtrl!=nullptr)
	{
		m_pPOUTDBGridCtrl->PostMessage(WM_DESTROY, 0, 0);
		m_pPOUTDBGridCtrl->DestroyWindow();
		//	delete m_pPOUTDBGridCtrl;//Do not use the C++ delete operator to destroy a frame window. Use CWnd::DestroyWindow instead. 
		m_pPOUTDBGridCtrl = nullptr;
	}
	if(m_pATTDBGridCtrl!=nullptr)
	{
		m_pATTDBGridCtrl->PostMessage(WM_DESTROY, 0, 0);
		m_pATTDBGridCtrl->DestroyWindow();
		//	delete m_pATTDBGridCtrl;//Do not use the C++ delete operator to destroy a frame window. Use CWnd::DestroyWindow instead. 
		m_pATTDBGridCtrl = nullptr;
	}
	if(m_pPOUDBGridCtrl!=nullptr)
	{
		m_pPOUDBGridCtrl->PostMessage(WM_DESTROY, 0, 0);
		m_pPOUDBGridCtrl->DestroyWindow();
		//	delete m_pPOUDBGridCtrl;//Do not use the C++ delete operator to destroy a frame window. Use CWnd::DestroyWindow instead. 
		m_pPOUDBGridCtrl = nullptr;
	}
	m_wndTabs.CleanUp();

	CDockablePane::OnDestroy();
}
void CTablePane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos(nullptr, 0, 0, cx, cy, SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
}

void CTablePane::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(200, 200, 200));
	dc.Draw3dRect(rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CTablePane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	CWnd* pWnd = CMFCTabCtrl::GetActiveWindow();
	if(pWnd!=nullptr)
	{
		pWnd->SetFocus();
	}
}

void CTablePane::SQLQuery(char type, CDatabase* pDatabase, CString strSQL, CString strIdField)
{
	if(IsOCXRegistered("MSRDC20.OCX")==FALSE)
	{
		RegisterOCX("MSRDC20.OCX", Platform::X86, "");
	}
	if(IsOCXRegistered("msrdo20.dll")==FALSE)
	{
		RegisterOCX("msrdo20.dll", Platform::X86, "");
	}
	if(IsOCXRegistered("DBGRID32.OCX")==FALSE)
	{
		RegisterOCX("DBGRID32.OCX", Platform::X86, "");
	}

	if(pDatabase==nullptr)
		return;
	else if(pDatabase->IsOpen()==FALSE)
		return;
	else if(strSQL.IsEmpty()==TRUE)
		return;
	else if(IsOCXRegistered("MSRDC20.OCX")==FALSE)
		return;
	else if(IsOCXRegistered("DBGRID32.OCX")==FALSE)
		return;

	strSQL.TrimLeft();
	strSQL = strSQL.Mid(strSQL.Find(_T(' ')));
	strSQL.TrimLeft();

	CString strTable;
	CString strWhere;

	CString strFields = strSQL.Left(strSQL.Find(_T("FROM")));
	strFields.TrimLeft();
	strFields.TrimRight();

	strSQL = strSQL.Mid(strSQL.Find(_T("FROM"))+4);
	strSQL.TrimLeft();
	if(strSQL.Find(_T("WHERE"))!=-1)
	{
		strTable = strSQL.Left(strSQL.Find(_T("WHERE")));
		strSQL = strSQL.Mid(strSQL.Find(_T("WHERE"))+5);
		strSQL.TrimLeft();
		strWhere = strSQL;

		strWhere.TrimLeft();
		strWhere.TrimRight();
	}
	else
	{
		strTable = strSQL;
	}
	strTable.TrimLeft();
	strTable.TrimRight();

	if(type=='A')
	{
		if(m_pATTDBGridCtrl==nullptr)
		{
			CAATDBGridCtrl* pAATDBGridCtrl = new CAATDBGridCtrl();
			pAATDBGridCtrl->m_keyVisible = true;
			if(pAATDBGridCtrl->Create(nullptr, nullptr, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, CRect(0, 0, 0, 0), &m_wndTabs, nullptr, 0, nullptr)==TRUE)
			{
				pAATDBGridCtrl->ModifyStyle(WS_VISIBLE, 0, 0);
				const CString strName = GetTabLabel('J');
				m_wndTabs.AddTab(pAATDBGridCtrl, strName);

				m_pATTDBGridCtrl = pAATDBGridCtrl;
			}
			else
			{
				delete pAATDBGridCtrl;
				pAATDBGridCtrl = nullptr;
			}
		}
		if(m_pATTDBGridCtrl!=nullptr)
		{
			if(m_pPOUDBGridCtrl!=nullptr)
			{
				m_pPOUDBGridCtrl->Clear();
			}
			this->ActivateTab('J');
			m_pATTDBGridCtrl->Fill(pDatabase, strTable, strFields, strWhere, strIdField);
		}
	}
	else if(type=='P')
	{
		if(m_pPOUDBGridCtrl==nullptr)
		{
			CPOUDBGridCtrl* pPOUDBGridCtrl = new CPOUDBGridCtrl();
			pPOUDBGridCtrl->m_keyVisible = false;
			if(pPOUDBGridCtrl->Create(nullptr, nullptr, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, CRect(0, 0, 0, 0), &m_wndTabs, nullptr, 0, nullptr)==TRUE)
			{
				pPOUDBGridCtrl->ModifyStyle(WS_VISIBLE, 0, 0);
				const CString strName = GetTabLabel('U');
				m_wndTabs.AddTab(pPOUDBGridCtrl, strName);

				m_pPOUDBGridCtrl = pPOUDBGridCtrl;
			}
			else
			{
				delete pPOUDBGridCtrl;
				pPOUDBGridCtrl = nullptr;
			}
		}
		if(m_pPOUDBGridCtrl!=nullptr)
		{
			if(m_pATTDBGridCtrl!=nullptr)
			{
				m_pATTDBGridCtrl->Clear();
			}

			this->ActivateTab('U');
			m_pPOUDBGridCtrl->Fill(pDatabase, strTable, strFields, strWhere, strIdField);
		}
	}
}

CString CTablePane::GetTabLabel(char cTab)
{
	HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);

	CString str;
	switch(cTab)
	{
	case 'A':
		str.LoadString(IDS_GRID_AATDATABASE);
		break;
	case 'P':
		str.LoadString(IDS_GRID_POUDATABASE);
		break;
	case 'J':
		str.LoadString(IDS_GRID_QUERY);
		break;
	case 'U':
		str.LoadString(IDS_GRID_QUERY);
		break;
	case 'C':
		break;
	case 'G':
		break;
	case 'R':
		break;
	case 'I':
		break;
	default:
		break;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
	return str;
}
