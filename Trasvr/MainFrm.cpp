// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Trasvr.h"

#include "MainFrm.h"
#include "resource.h"
#include "global.h"
#include "LoginDlg.h"

#include "..\Socket\ExStdCFunction.h"
#include "..\Smcom\Global.h"
#include "..\Smcom\ComThread.h"
#include "..\Smcom\SMSThread.h"
#include "..\Smcom\WavecomDlg.h"

#include "../Public/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  FRESH_VHCINFO_TIMER     300
#define  CLIENT_PULS_TIMER       301

// CMainFrame
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_COMMAND(ID_SERVER_START, OnServerStart)
	ON_UPDATE_COMMAND_UI(ID_SERVER_START, OnUpdateServerStart)
	ON_COMMAND(ID_SERVER_STOP, OnServerStop)
	ON_UPDATE_COMMAND_UI(ID_SERVER_STOP, OnUpdateServerStop)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP

	ON_COMMAND_EX(ID_VIEW_MENUBAR, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MENUBAR, OnUpdateControlBarMenu)

	ON_COMMAND_EX(IDR_TOOLBAR2, OnBarCheck)
	ON_UPDATE_COMMAND_UI(IDR_TOOLBAR2, OnUpdateControlBarMenu)

	// ON_COMMAND_EX(ID_VIEW_RESIZABLE_BAR_0, OnBarCheck)
	// ON_UPDATE_COMMAND_UI(ID_VIEW_RESIZABLE_BAR_0, OnUpdateControlBarMenu)

	ON_COMMAND_EX(ID_VIEW_RESIZABLE_BAR_1, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESIZABLE_BAR_1, OnUpdateControlBarMenu)

	// ON_COMMAND_EX(ID_VIEW_RESIZABLE_BAR_2, OnBarCheck)
	// ON_UPDATE_COMMAND_UI(ID_VIEW_RESIZABLE_BAR_2, OnUpdateControlBarMenu)

	// ON_COMMAND_EX(ID_VIEW_RESIZABLE_BAR_3, OnBarCheck)
	// ON_UPDATE_COMMAND_UI(ID_VIEW_RESIZABLE_BAR_3, OnUpdateControlBarMenu)

	ON_COMMAND_EX(ID_VIEW_RESIZABLE_BAR_4, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESIZABLE_BAR_4, OnUpdateControlBarMenu)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_OUTQUEUE,
	ID_INDICATOR_INQUEUE,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// create a view to occupy the client area of the frame
	if(!m_ChildView.Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW,CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, nullptr))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	
	if(!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) || !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if(!m_wndStatusBar.Create(this) ||	!m_wndStatusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	if(m_wndResizableBar1.Create(this, _T("������"), CRect(0,0,185,400),WS_CHILD | WS_VISIBLE | CBRS_LEFT,101) == TRUE)
	{	
		m_wndResizableBar1.SetBarStyle(m_wndResizableBar1.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_ALIGN_RIGHT | TBSTYLE_FLAT | CBRS_ALIGN_LEFT);
		m_wndResizableBar1.SetBarStyle(m_wndResizableBar1.GetBarStyle() & ~CBRS_FLOATING);

		if(m_menuTreeCtrl.Create(WS_CHILD|WS_VISIBLE|TVS_LINESATROOT|TVS_HASLINES|TVS_HASBUTTONS|TVS_SHOWSELALWAYS,CRect(0,0,0,0),&m_wndResizableBar1,m_wndResizableBar1.GetDlgCtrlID()) == TRUE)
		{
			m_menuTreeCtrl.m_pChildView = &m_ChildView;
			m_wndResizableBar1.m_pDelete = &m_menuTreeCtrl;
		}
		else
		{
			TRACE0("Failed to create m_wndInBarListBox\n");
			return -1;		
		}
	}
	else
	{
		m_wndResizableBar1.DestroyWindow();
	}

	if(m_wndResizableBar2.Create(this, _T("�����"), CRect(0,0,185,150),WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,102) == TRUE)
	{
		m_wndResizableBar2.SetBarStyle(m_wndResizableBar2.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_ALIGN_RIGHT | TBSTYLE_FLAT | CBRS_ALIGN_BOTTOM);
		m_wndResizableBar2.SetBarStyle(m_wndResizableBar2.GetBarStyle() & ~CBRS_FLOATING);

		if(m_wndInBarEdit.Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_LEFT|ES_MULTILINE|ES_AUTOHSCROLL|WS_EX_RIGHTSCROLLBAR,CRect(0,0,0,0),&m_wndResizableBar2,m_wndResizableBar2.GetDlgCtrlID()) == TRUE)
		{
			m_wndInBarEdit.ShowScrollBar(SB_VERT, TRUE);
			CComThread::SetMapEditMsgWndHnd(m_wndInBarEdit);//����������ڵ�ָ�빩Smcommuʹ�á�
			CComThread::SetMainWndHnd(m_ChildView.m_hWnd);
			m_wndResizableBar2.m_pDelete = &m_wndInBarEdit;
		}
		else
		{
			TRACE0("Failed to create m_wndInBarEdit\n");
			return -1;	
		}
	}
	else
	{
		m_wndResizableBar2.DestroyWindow();
	}
	

	if(m_wndToolBar.m_hWnd != nullptr)
	{
		EnableDocking(CBRS_ALIGN_TOP);
		m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
		DockControlBar(&m_wndToolBar);
	}
	
	if(m_wndResizableBar2.m_hWnd != nullptr)
	{
		EnableDocking(CBRS_ALIGN_BOTTOM);
		m_wndResizableBar2.EnableDocking(CBRS_ALIGN_BOTTOM);
		ShowControlBar(&m_wndResizableBar2, TRUE, FALSE);
		DockControlBar(&m_wndResizableBar2, AFX_IDW_DOCKBAR_BOTTOM);
	}
	if(m_wndResizableBar1.m_hWnd != nullptr)
	{
		EnableDocking(CBRS_ALIGN_LEFT);
		m_wndResizableBar1.EnableDocking(CBRS_ALIGN_LEFT);
		ShowControlBar(&m_wndResizableBar1, TRUE, FALSE);
		DockControlBar(&m_wndResizableBar1, AFX_IDW_DOCKBAR_LEFT);
	}

	RecalcLayout();
	CRect wrAlredyDockedBar;
	m_wndToolBar.GetWindowRect(&wrAlredyDockedBar);
	wrAlredyDockedBar.OffsetRect(1, 0);

	RecalcLayout();


	if(CheckAndRegister(_T("MSCOMM32.OCX")) == TRUE)
	{
	}

	SendMessage(WM_SIZE, SIZE_MAXIMIZED, 0);
	SetForegroundWindow();
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.x = 0;
	cs.y = 0;
	cs.cx = ::GetSystemMetrics(SM_CXSCREEN);
	cs.cy = ::GetSystemMetrics(SM_CYSCREEN);
	cs.style |=WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS;// | WS_MAXIMIZE;
	cs.dwExStyle = WS_EX_LEFT | WS_EX_LTRREADING ;//| WS_EX_RIGHTSCROLLBAR;

	if(!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	if(m_ChildView.m_hWnd != nullptr)
	{
		m_ChildView.SetFocus();
	}
}

BOOL CMainFrame::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if(m_ChildView.OnCmdMsg(nId, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}


BOOL CMainFrame::DestroyWindow() 
{
	m_ChildView.DestroyWindow();
	return CFrameWnd::DestroyWindow();
}

void CMainFrame::OnServerStart() 
{
	m_ChildView.Start();
}

void CMainFrame::OnServerStop() 
{
	m_ChildView.Stop();
}

void CMainFrame::OnUpdateServerStart(CCmdUI* pCmdUI) 
{
	if(m_ChildView.m_pSMSThread != nullptr)// �������
	{
		pCmdUI->Enable(FALSE);
	}
	else if(m_ChildView.m_pUDPThread != nullptr)
	{
		pCmdUI->Enable(FALSE);
	}
	else if(m_ChildView.m_pTCPThread != nullptr)
	{
		pCmdUI->Enable(FALSE);
	}
	else if(m_ChildView.m_pHTTPThread != nullptr)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}

void CMainFrame::OnUpdateServerStop(CCmdUI* pCmdUI) 
{
	if(m_ChildView.m_pSMSThread != nullptr)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(m_ChildView.m_pUDPThread != nullptr)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(m_ChildView.m_pTCPThread != nullptr)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(m_ChildView.m_pHTTPThread != nullptr)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}


// �޸Ĺ���Ա�˺�ROOT����
void CMainFrame::OnMenuitemChangeRootpass() 
{
	CLoginDlg dlg;// root�˻���¼����
	dlg.m_bChangePass = TRUE;
	dlg.DoModal();
}