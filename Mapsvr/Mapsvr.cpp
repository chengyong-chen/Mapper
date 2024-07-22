// Mapsvr.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Mapsvr.h"
#include "MapsvrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMapsvrApp

BEGIN_MESSAGE_MAP(CMapsvrApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMapsvrApp 构造

CMapsvrApp::CMapsvrApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMapsvrApp 对象

CMapsvrApp theApp;


// CMapsvrApp 初始化

BOOL CMapsvrApp::InitInstance()
{
	CWinApp::InitInstance();

	SetRegistryKey(_T("Diwatu"));

	if(!AfxSocketInit())
	{
		AfxMessageBox(_T("Socket初始化失败"));
		return FALSE;
	}

	AfxEnableControlContainer();

		// Standard initialization
		// If you are not using these features and wish to reduce the size
		//  of your final executable, you should remove from the following
		//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CMapsvrDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if(nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用“确定”来关闭
		//对话框的代码
	}
	else if(nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}