#include "stdafx.h"
#include "Viewer.h"

#include "Splash.h"  // e.g. splash.h

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

BOOL CSplashWnd::c_bShowSplashWnd;
CSplashWnd* CSplashWnd::c_pSplashWnd;

CSplashWnd::CSplashWnd()
{
}

CSplashWnd::~CSplashWnd()
{
	if(m_bitmap.m_hObject!=nullptr)
	{
		m_bitmap.DeleteObject();
	}

	ASSERT(c_pSplashWnd==this);
	c_pSplashWnd = nullptr;
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSplashWnd::EnableSplashScreen(BOOL bEnable /*= TRUE*/)
{
	c_bShowSplashWnd = bEnable;
}

void CSplashWnd::ShowSplashScreen(CWnd* pParentWnd /*= nullptr*/)
{
	if(!c_bShowSplashWnd||c_pSplashWnd!=nullptr)
		return;

	// Allocate a new splash screen, and create the window.
	c_pSplashWnd = new CSplashWnd;
	if(!c_pSplashWnd->Create(pParentWnd))
		delete c_pSplashWnd;
	else
		c_pSplashWnd->UpdateWindow();
}

BOOL CSplashWnd::PreTranslateAppMessage(MSG* pMsg)
{
	if(c_pSplashWnd==nullptr)
		return FALSE;

	// If we get a keyboard or mouse message, hide the splash screen.
	if(pMsg->message==WM_KEYDOWN||
		pMsg->message==WM_SYSKEYDOWN||
		pMsg->message==WM_LBUTTONDOWN||
		pMsg->message==WM_RBUTTONDOWN||
		pMsg->message==WM_MBUTTONDOWN||
		pMsg->message==WM_NCLBUTTONDOWN||
		pMsg->message==WM_NCRBUTTONDOWN||
		pMsg->message==WM_NCMBUTTONDOWN)
	{
		c_pSplashWnd->HideSplashScreen();
		return TRUE; // message handled here
	}

	return FALSE; // message not handled
}

BOOL CSplashWnd::Create(CWnd* pParentWnd /*= nullptr*/)
{
	const CString strProject = AfxGetApp()->m_lpCmdLine;
	if(strProject.IsEmpty()==FALSE)
	{
		const CString strApp = ((CViewerApp*)AfxGetApp())->GetProfileString(_T(""), _T("Path"), nullptr);
		const CString strSplash = strApp+_T('\\')+strProject+_T(".bmp");
		const HBITMAP hBitmap = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, strSplash, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if(hBitmap!=nullptr)
		{
			m_bitmap.Attach(hBitmap);

			BITMAP bm;
			m_bitmap.GetBitmap(&bm);

			return CreateEx(0, AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)), nullptr, WS_POPUP|WS_VISIBLE, 0, 0, bm.bmWidth, bm.bmHeight, pParentWnd->GetSafeHwnd(), nullptr);
		}
	}

	return FALSE;
}

void CSplashWnd::HideSplashScreen()
{
	// Destroy the window, and update the mainframe.
	DestroyWindow();
	AfxGetMainWnd()->UpdateWindow();
}

void CSplashWnd::PostNcDestroy()
{
	// Free the C++ class.
	delete this;
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CWnd::OnCreate(lpCreateStruct)==-1)
		return -1;

	// Center the window.
	CenterWindow();
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	// Set a timer to destroy the splash screen.
	this->SetTimer(1, 3000, nullptr);

	return 0;
}

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this);

	CDC dcImage;
	if(!dcImage.CreateCompatibleDC(&dc))
		return;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
	dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
	dcImage.SelectObject(pOldBitmap);
	dcImage.DeleteDC();
	DeleteObject(&bm);
}

void CSplashWnd::OnTimer(UINT nIDEvent)
{
	// Destroy the splash screen window.
	if(nIDEvent==1)
	{
		this->KillTimer(1);
		HideSplashScreen();
		return;
	}
	CWnd::OnTimer(nIDEvent);
}
