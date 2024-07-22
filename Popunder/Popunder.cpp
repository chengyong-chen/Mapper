#include "stdafx.h"
#include "Popunder.h"
#include "PopunderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CPopunderApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CPopunderApp::CPopunderApp()
{
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

CPopunderApp theApp;


BOOL CPopunderApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual context.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	//	CCustomOccManager *pMgr = new CCustomOccManager;	
	AfxEnableControlContainer(nullptr);
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	//	Sleep(60*1000);

	//CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);
	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;

	CPopunderDlg dlg;
	m_pMainWnd = &dlg;
	
	const INT_PTR nResponse = dlg.DoModal();
	if(nResponse==IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if(nResponse==IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BEGIN_EVENTSINK_MAP(CPopunderApp, CWinAppEx)
END_EVENTSINK_MAP()