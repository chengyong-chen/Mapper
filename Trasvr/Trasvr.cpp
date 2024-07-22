// Trasvr.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "Trasvr.h"
#include "MainFrm.h"

#include "..\Socket\ExStdCFunction.h"
#include "OptionSheet.h"

#include <float.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTrasvrApp

BEGIN_MESSAGE_MAP(CTrasvrApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_MENUITEM_HELP, OnMenuitemHelp)
	ON_COMMAND(ID_TOOL_OPTION, OnToolOption)
END_MESSAGE_MAP()


// CTrasvrApp construction

CTrasvrApp::CTrasvrApp()
{
	m_hMutextOnlyOne = nullptr;
	m_iActivityFreshTime = 3; // 活动消息丢弃时间 单位：分钟
}


// The one and only CTrasvrApp object

CTrasvrApp theApp;


// CTrasvrApp initialization
BOOL CTrasvrApp::InitInstance()
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

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if(!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Mapday"));
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object

	COleDateTime now = COleDateTime::GetCurrentTime();
	m_strRun = now.Format(_T("启动时间：%Y/%m/%d %H:%M"));
	m_strVer = _T("软件版本：1.6.0");

	// only allow one app instance is running.    
	m_hMutextOnlyOne = ::CreateMutex(nullptr, FALSE, _T("Only One GServer"));
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// AfxMessageBox(_T("对不起，同一时刻系统只可以运行一个监控服务器实例"));
	//	return FALSE;		
	}
	if(!CheckSysInformation())
		return FALSE;

	CMainFrame* pFrame = new CMainFrame;
	if(pFrame == nullptr)
		return FALSE;

	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, nullptr,nullptr);
	pFrame->ActivateFrame(SW_SHOW);
	pFrame->SetIcon(LoadIcon(IDR_MAINFRAME), TRUE);
	m_pMainWnd = pFrame;
	
//	AfxBeginThread(BackupThreadProc, (LPVOID)0);

	// auto re-run gserver
/*	if(::FindWindow(nullptr, "GPS车辆监控服务器初始化") == nullptr)
	{
		char buff[MAX_PATH];
		memset(buff, 0, MAX_PATH);
		GetModuleFileName(nullptr,buff,sizeof(buff)); 	
		CString strPath = buff;
		strPath = strPath.Left(strPath.ReverseFind(_T('\\')));
		strPath += "\\gpstool.exe";	
		WinExec(strPath, SW_SHOW);
	}
*/
	// The one and only window has been initialized, so show and update it
//	pFrame->ShowWindow(SW_SHOW);
//	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

int CTrasvrApp::ExitInstance() 
{
	if(m_hMutextOnlyOne != nullptr)
	{
		CloseHandle(m_hMutextOnlyOne);
	}

	HWND hWnd = ::FindWindow(nullptr, "GPS车辆监控服务器初始化");
	if(hWnd && ::IsWindow(hWnd))
	{
		::SendMessage(hWnd, WM_DESTROY, 0, 0);
		::SendMessage(hWnd, WM_QUIT, 0, 0);
	}
	
	return CWinApp::ExitInstance();
}
// CTrasvrApp message handlers


void CTrasvrApp::WinHelp(DWORD dwData, UINT nCmd) 
{
	CString helpPath = GetCurrentDir() + _T("\\GISHelp.chm");
	// if(31 >= WinExec(helpPath, SW_SHOW))
	if(32 >= (int)ShellExecute(nullptr, "open", helpPath, nullptr, nullptr, SW_SHOW))
	{
		::MessageBox(nullptr, _T("没有找到相关的帮助文件!"), "GServer", MB_OK|MB_ICONINFORMATION); 
	}

	// CWinApp::WinHelp(dwData, nCmd);
}

void CTrasvrApp::OnMenuitemHelp() 
{
	WinHelp(1);
}

void CTrasvrApp::OnToolOption() 
{
	COptionSheet propSheet;
	propSheet.SetTitle(_T("选项"), 0);
	propSheet.DoModal();	
}


bool CTrasvrApp::CheckSysInformation() const
{
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	GetVersionEx(&os);
	// for windows 9x or 3.x
	switch(os.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:		
	case VER_PLATFORM_WIN32s:
		AfxMessageBox("系统必须在Windows 2000及其以上版本才可以运行" , MB_OK|MB_ICONSTOP);
		return false;		
	default:
		break;
	}
	// for windows nt 4.0 or earlier
	if(os.dwMajorVersion < 5)
	{
			AfxMessageBox("系统必须在Windows 2000及其以上版本才可以运行" , MB_OK|MB_ICONSTOP);
			return false;
	}
	
	// check time .
	LPCTSTR szCompilerTime	 = __DATE__" " __TIME__ ;
	COleDateTime dateCompiler;
	dateCompiler.ParseDateTime(szCompilerTime);
	COleDateTime dateCur	 = COleDateTime::GetCurrentTime();
	if(dateCur < dateCompiler)
	{
		AfxMessageBox("您的系统时间不正确，请调整好时间再启动本程序" , MB_OK|MB_ICONSTOP);
		return false;
	}
	return true;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CTrasvrApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CTrasvrApp message handlers


UINT BackupThreadProc(LPVOID lParam)
{
	UINT uInterval = 1000*60*50; // 50min;
	CString helpPath = GetCurrentDir() + _T("\\backup.exe");
	
	do
	{
		try
		{
			if(COleDateTime::GetCurrentTime().GetHour() != 2)
			{
				Sleep(uInterval);
				continue;
			}

			// backup log
			if(32 >= (int)ShellExecute(nullptr, "open", helpPath, "-s -l -r", nullptr, SW_SHOW))
			{
			}

			// bakcup data
			double time = COleDateTime::GetCurrentTime();		
			double lastTime = 0.0;
			CRegKey regKey;		
			char buff[MAX_PATH];
			if(ERROR_SUCCESS == regKey.Open(HKEY_CURRENT_USER,	_T("Software\\Mapday\\GServer\\1.0\\backup")))
			{
				DWORD dwDay;
				if(ERROR_SUCCESS != regKey.QueryValue(dwDay, _T("IntervalDay")) || dwDay == 0)
				{
					dwDay = 30; // default 30 day backup
				}

				DWORD len = sizeof(buff);
				memset(buff, 0, MAX_PATH);
				if(regKey.QueryValue(buff, _T("LastDay"), &len) != ERROR_SUCCESS)
				{
					sprintf(buff, "%g", time);
					regKey.SetValue(buff, _T("LastDay"));
				}

				lastTime = _ttof(buff);				
				if(UINT(time) == UINT(lastTime))
				{
					memset(buff, 0, sizeof(buff));
					sprintf(buff, "%g", time+dwDay);
					regKey.SetValue(buff, _T("LastDay"));						
					if(32 >= (int)ShellExecute(nullptr, "open", helpPath, "-s -d", nullptr, SW_SHOW))
					{
						// ::MessageBox(nullptr, _T("没有找到备份程序!"), "GServer", MB_OK|MB_ICONINFORMATION); 
					}
				}
				regKey.Close();
			}		
			
			Sleep(uInterval);
		}
		catch(CException* ex)
		{
			ex->Delete();
		};
	}while(TRUE);
}