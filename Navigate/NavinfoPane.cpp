#include "stdafx.h"

#include "NavinfoPane.h"
#include "NavinfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CNavinfoPane::CNavinfoPane()
{
	m_pNavigateDlg = nullptr;
}

BEGIN_MESSAGE_MAP(CNavinfoPane, CDockablePane)
	//{{AFX_MSG_MAP(CNavinfoPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNavinfoPane message handlers

int CNavinfoPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct)==-1)
		return -1;

	if(m_pNavigateDlg==nullptr)
	{
		HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		AfxSetResourceHandle(hInst);

		CNavinfoDlg* pNavinfoDlg = new CNavinfoDlg(this);
		if(pNavinfoDlg->Create(IDD_NAVINFO, (CWnd*)this)==TRUE)
		{
			pNavinfoDlg->ShowWindow(SW_SHOWNORMAL);
			m_pNavigateDlg = pNavinfoDlg;
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
	const HICON hIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_NAVINFO), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	CDockablePane::SetIcon(hIcon, FALSE);
	return 0;
}

void CNavinfoPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if(GetSafeHwnd()!=nullptr)
	{
		CRect rect;
		GetClientRect(rect);

		if(m_pNavigateDlg!=nullptr)
		{
			m_pNavigateDlg->SetWindowPos(&wndTop, 0, 0, cx, cy, SWP_NOMOVE|SWP_SHOWWINDOW);
		}
	}
}

void CNavinfoPane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	if(m_pNavigateDlg!=nullptr)
	{
		m_pNavigateDlg->SetFocus();
	}
}

void CNavinfoPane::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(200, 200, 200));
	dc.Draw3dRect(rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

BOOL CNavinfoPane::DestroyWindow()
{
	if(m_pNavigateDlg!=nullptr)
	{
		m_pNavigateDlg->PostMessage(WM_DESTROY, 0, 0);
		m_pNavigateDlg->DestroyWindow();

		delete m_pNavigateDlg;
		m_pNavigateDlg = nullptr;
	}

	return CDockablePane::DestroyWindow();
}
