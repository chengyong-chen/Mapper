#include "stdafx.h"
#include "QueryPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQueryPane

IMPLEMENT_DYNAMIC(CQueryPane, CDockablePane)

CQueryPane::CQueryPane()
{
}

BEGIN_MESSAGE_MAP(CQueryPane, CDockablePane)
	//{{AFX_MSG_MAP(CQueryPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueryPane message handlers
int CQueryPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct)==-1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	const DWORD dwViewStyle = WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_BORDER|WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
	if(m_ComboBox.Create(dwViewStyle, rectDummy, this, 1)==TRUE)
	{
		m_ComboBox.AddString(_T("Key"));
		m_ComboBox.AddString(_T("Coordinate"));
		m_ComboBox.AddString(_T("Geometry"));
		m_ComboBox.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
		m_ComboBox.SetCurSel(0);
	}
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
	AfxSetResourceHandle(hInst);

	m_KeyQueryEar.Create(IDD_KEYQUERY, this);
	m_CooQueryEar.Create(IDD_COOQUERY, this);
	m_GeomQueryEar.Create(IDD_GEOMQUERY, this);

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
	const HICON hIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_QUERY), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	CDockablePane::SetIcon(hIcon, FALSE);
	CDockablePane::EnableToolTips(TRUE);
	CDockablePane::EnableDocking(CBRS_ALIGN_LEFT);

	OnSelChanged();
	return 0;
}

BOOL CQueryPane::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(HIWORD(wParam))
	{
		case CBN_SELCHANGE:
			{
				OnSelChanged();
			}
			break;
		default:
			break;
	}

	return CDockablePane::OnCommand(wParam, lParam);
}

void CQueryPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if(GetSafeHwnd()!=nullptr)
	{
		CRect rectClient;
		GetClientRect(rectClient);

		CRect rectCombo;
		m_ComboBox.GetWindowRect(&rectCombo);
		const int cyCmb = rectCombo.Size().cy;
		m_ComboBox.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE|SWP_NOZORDER);

		if(m_KeyQueryEar.m_hWnd!=nullptr)
			m_KeyQueryEar.SetWindowPos(nullptr, rectClient.left+1, rectClient.top+cyCmb+1, rectClient.Width()-2, rectClient.Height()-(cyCmb+2), SWP_NOACTIVATE|SWP_NOZORDER);
		//	if(m_SQLQueryEar.m_hWnd != nullptr)
		//		m_SQLQueryEar.SetWindowPos(nullptr, rectClient.left+1, rectClient.top + cyCmb+1, rectClient.Width()-2, rectClient.Height() -(cyCmb+2), SWP_NOACTIVATE | SWP_NOZORDER);
		if(m_CooQueryEar.m_hWnd!=nullptr)
			m_CooQueryEar.SetWindowPos(nullptr, rectClient.left+1, rectClient.top+cyCmb+1, rectClient.Width()-2, rectClient.Height()-(cyCmb+2), SWP_NOACTIVATE|SWP_NOZORDER);
		//	if(m_TabQueryEar.m_hWnd != nullptr)
		//		m_TabQueryEar.SetWindowPos(nullptr, rectClient.left+1, rectClient.top + cyCmb+1, rectClient.Width()-2, rectClient.Height() -(cyCmb+2), SWP_NOACTIVATE | SWP_NOZORDER);
		if(m_GeomQueryEar.m_hWnd!=nullptr)
			m_GeomQueryEar.SetWindowPos(nullptr, rectClient.left+1, rectClient.top+cyCmb+1, rectClient.Width()-2, rectClient.Height()-(cyCmb+2), SWP_NOACTIVATE|SWP_NOZORDER);
	}
}

void CQueryPane::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(200, 200, 200));
	dc.Draw3dRect(rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CQueryPane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	if(m_CooQueryEar.m_hWnd!=nullptr&&m_CooQueryEar.IsWindowVisible()==TRUE)
	{
		m_CooQueryEar.SetFocus();
	}
	else if(m_GeomQueryEar.m_hWnd!=nullptr&&m_GeomQueryEar.IsWindowVisible()==TRUE)
	{
		m_GeomQueryEar.SetFocus();
	}
	else if(m_KeyQueryEar.m_hWnd!=nullptr&&m_KeyQueryEar.IsWindowVisible()==TRUE)
	{
		m_KeyQueryEar.SetFocus();
	}
}

void CQueryPane::OnSelChanged()
{
	const int index = m_ComboBox.GetCurSel();
	switch(index)
	{
		case 0:
			{
				m_CooQueryEar.ShowWindow(SW_HIDE);
				m_GeomQueryEar.ShowWindow(SW_HIDE);
				m_KeyQueryEar.ShowWindow(SW_SHOW);
			}
			break;
		case 1:
			{
				m_KeyQueryEar.ShowWindow(SW_HIDE);
				m_GeomQueryEar.ShowWindow(SW_HIDE);
				m_CooQueryEar.ShowWindow(SW_SHOW);
			}
			break;
		case 2:
			{
				m_KeyQueryEar.ShowWindow(SW_HIDE);
				m_CooQueryEar.ShowWindow(SW_HIDE);
				m_GeomQueryEar.ShowWindow(SW_SHOW);
			}
			break;
	}
}

void CQueryPane::RemoveEar(CString strTab)
{
	if(strTab==_T("Coo"))
	{
		const int index = m_ComboBox.FindString(0, _T("Coordinate"));
		m_ComboBox.DeleteString(index);
	}
	else if(strTab==_T("GEOM"))
	{
		const int index = m_ComboBox.FindString(0, _T("Geometry"));
		m_ComboBox.DeleteString(index);
	}
}

BOOL CQueryPane::DestroyWindow()
{
	const HICON hIcon = CDockablePane::GetIcon(FALSE);
	if(hIcon!=nullptr)
	{
		DestroyIcon(hIcon);
	}

	return CDockablePane::DestroyWindow();
}
