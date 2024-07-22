#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "SmartER.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "SmartERDoc.h"
#include "SmartERView.h"
#include "..\Framework\GMultiDocManager.h"
#include "..\Framework\GMultiDocTemplate.h"

#ifdef _DEBUG
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CSmartERApp

BEGIN_MESSAGE_MAP(CSmartERApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, CSmartERApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CSmartERApp construction

CSmartERApp::CSmartERApp()
	:CWinAppEx(TRUE)
{
	m_bHiColorIcons = TRUE;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("SmartER.AppID.NoVersion"));

#ifdef _DEBUG
	//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
#endif
}

// The one and only CSmartERApp object

CSmartERApp theApp;


// CSmartERApp initialization

BOOL CSmartERApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual context.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();
	afxAmbientActCtx = FALSE;

	if(!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	::OleInitialize(nullptr);
	AfxEnableControlContainer();
	EnableTaskbarInteraction();

	SetRegistryKey(_T("Diwatu"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	CWinAppEx::InitContextMenuManager();
	CWinAppEx::InitKeyboardManager();
	CWinAppEx::InitTooltipManager();
	CWinAppEx::InitShellManager();

	if(afxGlobalData.fontRegular.GetSafeHandle() != nullptr)
	{
		::DeleteObject(afxGlobalData.fontRegular.Detach());

		LOGFONT logfont = {0};
		::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &logfont, 0);
		afxGlobalData.SetMenuFont(&logfont,true);
	}
	CTooltipManager* pTooltipManager = theApp.GetTooltipManager();
	if(pTooltipManager != nullptr)
	{
		afxGlobalData.m_nMaxToolTipWidth = 200;	  
		CMFCToolTipInfo params;
		params.m_bDrawIcon = TRUE;
		params.m_bDrawSeparator = TRUE;
		params.m_bDrawDescription = TRUE;
		params.m_bVislManagerTheme = TRUE;
		params.m_bBoldLabel = TRUE;
		params.m_bRoundedCorners = TRUE;
		params.m_clrFill = RGB(104, 165, 225);
		params.m_clrFillGradient = RGB(37, 92, 222);
		params.m_clrText = RGB(255, 255, 255);	 	
		params.m_clrBorder = RGB(113, 166, 246);  		
		pTooltipManager->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,RUNTIME_CLASS(CMFCToolTipCtrl),&params);
	}

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);

	ASSERT(m_pDocManager == nullptr);
	m_pDocManager = new GMultiDocManager();

	GMultiDocTemplate* pDocTemplate = new GMultiDocTemplate(
		IDR_SMARTERTYPE,
		RUNTIME_CLASS(CSmartERDoc),
		RUNTIME_CLASS(CChildFrame),
		GWRUNTIME_CLASS(CSmartERView),
		nullptr);
	m_pDocManager->AddDocTemplate(pDocTemplate);//AddDocTemplate(pDocTemplate); 
	if(pDocTemplate->m_hMenuShared != nullptr)
	{
		::DestroyMenu(pDocTemplate->m_hMenuShared);
		pDocTemplate->m_hMenuShared = nullptr;
	}

	CMainFrame* pMainFrame = new CMainFrame();
	EnableLoadWindowPlacement(FALSE);
	if(pMainFrame->LoadFrame(IDR_MAINFRAME) == TRUE)
	{
		m_pMainWnd = pMainFrame;

		CWinAppEx::EnableShellOpen();
		CWinAppEx::RegisterShellFileTypes(TRUE);

		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
		if(ProcessShellCommand(cmdInfo) == FALSE)
			return FALSE;

		if (!ReloadWindowPlacement(pMainFrame))
		{
			pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
			pMainFrame->UpdateWindow();
		}
		//		CWinAppEx::m_bResourceSmartUpdate = FALSE;
		//		AfxIncreaseCounter();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CSmartERApp::ExitInstance()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	CMFCVisualManager::DestroyInstance();	//this line is very important,get rid of the exit error on Windows XP
	AfxOleTerm(FALSE);

	delete m_pDocManager;
	m_pDocManager = nullptr;
	
	ControlBarCleanUp(); 
//	_CrtDumpMemoryLeaks();it makes false leak report, including a lot of static data which is not released yet. If you still have memory leaks, ensure that every source file has DEBUG_NEW redifinition to see allocation source file and line number.If you are working with MFC, don't call _CrtDumpMemoryLeaks and don't use _CrtSetDbgFlag at all - MFC calls _CrtDumpMemoryLeaks automatically just before exit.
	return CWinAppEx::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CSmartERApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSmartERApp customization load/save methods

void CSmartERApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CSmartERApp::LoadCustomState()
{
}

void CSmartERApp::SaveCustomState()
{
}

// CSmartERApp message handlers



void CSmartERApp::OnCloseAll()
{
	m_pDocManager->CloseAllDocuments(TRUE);	
}