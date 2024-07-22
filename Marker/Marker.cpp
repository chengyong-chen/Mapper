#include "stdafx.h"
#include "Marker.h"
#include "MainFrm.h"

#include "../Framework/gfx.h"
#include "../Framework/GSingleDocTemplate.h"
#include "../Framework/GSingleDocManager.h"

#include "SymbolDoc.hpp"
#include "SymbolView.hpp"
#include "TabletView.hpp"
#include "../Style/SymbolSpot.h"
#include "../Style/SymbolLine.h"
#include "../Style/SymbolFill.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CMakerApp, CWinAppEx)
	//{{AFX_MSG_MAP(CMakerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMakerApp construction

CMakerApp::CMakerApp()
	: CWinAppEx(TRUE)
{
	//	AfxSetAmbientActCtx(FALSE);

	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
	m_nAutosaveInterval = 30*60*1000;//if idle time exceeds 30 minutes auto save it.

#ifdef _MANAGED
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif
	SetAppID(_T("Diwatu.Marker.2012"));
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}

CMakerApp theApp;

BOOL CMakerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual context.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinAppEx::InitInstance();
	//	afxAmbientActCtx = FALSE;

	// Initialize OLE libraries
	if(!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();
	EnableTaskbarInteraction();
	SetRegistryKey(_T("Diwatu"));

	LoadStdProfileSettings(4); // Load standard INI file options (including MRU)
	CWinAppEx::InitContextMenuManager();
	CWinAppEx::InitKeyboardManager();
	CWinAppEx::InitTooltipManager();
	CWinAppEx::InitShellManager();

	if(afxGlobalData.fontRegular.GetSafeHandle() != nullptr)
	{
		::DeleteObject(afxGlobalData.fontRegular.Detach());

		LOGFONT logfont = {0};
		::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &logfont, 0);
		afxGlobalData.SetMenuFont(&logfont, true);
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
		pTooltipManager->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &params);
	}
	const Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);

	m_pDocManager = new GSingleDocManager();
	GSingleDocTemplate* pDocTemplate1 = new GSingleDocTemplate(
		IDR_SPOT,
		RUNTIME_CLASS_T(CSymbolDoc, CSymbolSpot),
		RUNTIME_CLASS(CMainFrame),
		GWRUNTIME_CLASS_T(CTabletView, CSymbolSpot),
		GWRUNTIME_CLASS_T(CSymbolView, CSymbolSpot));
	AddDocTemplate(pDocTemplate1);
	GSingleDocTemplate* pDocTemplate2 = new GSingleDocTemplate(
		IDR_LINE,
		RUNTIME_CLASS_T(CSymbolDoc, CSymbolLine),
		RUNTIME_CLASS(CMainFrame),
		GWRUNTIME_CLASS_T(CTabletView, CSymbolLine),
		GWRUNTIME_CLASS_T(CSymbolView, CSymbolLine));
	AddDocTemplate(pDocTemplate2);
	GSingleDocTemplate* pDocTemplate3 = new GSingleDocTemplate(
		IDR_FILL,
		RUNTIME_CLASS_T(CSymbolDoc, CSymbolFill),
		RUNTIME_CLASS(CMainFrame),
		GWRUNTIME_CLASS_T(CTabletView, CSymbolFill),
		GWRUNTIME_CLASS_T(CSymbolView, CSymbolFill));
	AddDocTemplate(pDocTemplate3);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);
	//Dispatch commands specified on the command line
	if(!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	/// force a title-bar icon (MFC Wizard doesn't set it up correctly)
	const HICON hIcon = LoadIcon(IDR_MAINFRAME);
	const HICON hPrev = m_pMainWnd->SetIcon(hIcon, FALSE);
	if(hPrev != nullptr && hPrev != hIcon)
		DestroyIcon(hPrev);

	return TRUE;
}

int CMakerApp::ExitInstance()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	CMFCVisualManager::DestroyInstance(); //this line is very important,get rid of the exit error on Windows XP
	AfxOleTerm(FALSE);

	delete m_pDocManager;
	m_pDocManager = nullptr;

	ControlBarCleanUp();
	//	_CrtDumpMemoryLeaks();it makes false leak report, including a lot of static data which is not released yet. If you still have memory leaks, ensure that every source file has DEBUG_NEW redifinition to see allocation source file and line number.If you are working with MFC, don't call _CrtDumpMemoryLeaks and don't use _CrtSetDbgFlag at all - MFC calls _CrtDumpMemoryLeaks automatically just before exit.
	return CWinAppEx::ExitInstance();
}
int CMakerApp::Run()
{
	int nArgs = 0;
	const LPWSTR cmdline = GetCommandLineW();
	const LPWSTR* aruments = CommandLineToArgvW(cmdline, &nArgs);
	if(nArgs == 3)
	{
		if(_wcsicmp(aruments[1], L"convert") == 0)
		{
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\SynWithUsb\\Diwatu Map Data\\FlashMap"));
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\SynWithUsb\\Diwatu Map Data\\Singapore"));
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\SynWithUsb\\Diwatu Map Data\\�������ӵ�ͼ2008"));
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\SynWithUsb\\Diwatu Map Data\\ȫ�й���ͼ"));
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\SynWithUsb\\Diwatu Map Data\\ͼ�ε�ͼ"));
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\SynWithUsb\\Diwatu Map Data\\���ݿ��ͼ"));
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\Programs\\Diwatu\\Samples"));
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\Programs\\Popunder"));

			CMakerApp::ConvertFile(CString(aruments[2]));
		}
		return FALSE;
	}
	else
		return CWinAppEx::Run();
}

void CMakerApp::ConvertFile(CString strFolder)
{
	CFileException e;
	WIN32_FIND_DATA FindDate;
	const HANDLE hFile = FindFirstFile(strFolder + _T("\\*.*"), &FindDate);
	do
	{
		if(_tcscmp(FindDate.cFileName, _T(".")) == 0 || _tcscmp(FindDate.cFileName, _T("..")) == 0)
			continue;

		if((FindDate.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
			const CString subFold = strFolder + _T('\\') + FindDate.cFileName;
			ConvertFile(subFold);
		}
		else
		{
			CString strFile = FindDate.cFileName;
			strFile.MakeUpper();
			if(strFile.Right(4) == _T(".MYM") || strFile.Right(4) == _T(".LYM") || strFile.Right(4) == _T(".FYM"))
			{
				strFile = strFolder + _T("\\") + FindDate.cFileName;
				CDocument* pDocument = AfxGetApp()->OpenDocumentFile(strFile);
				if(pDocument != nullptr)
				{
					pDocument->DoSave(strFile, TRUE);
					pDocument->OnCloseDocument();
				}
			}
		}
	} while(FindNextFile(hFile, &FindDate) == TRUE);
}
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum
	{
		IDD = IDD_ABOUTBOX
	};

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CMakerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}