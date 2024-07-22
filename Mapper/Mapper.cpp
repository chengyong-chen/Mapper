#include "stdafx.h"
#include <odbcinst.h>
#include <algorithm>
#include <fstream>

#include "Mapper.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "GrfDoc.h"
#include "GrfView.h"
#include "GisDoc.h"
#include "GisView.h"
#include "GeoDoc.h"
#include "GeoView.h"
#include "Global.h"
#include "../Framework/AidView.hpp"

#include "../Public/Global.h"
#include "../Public/IPlugin.h"
#include "../Public/AutoUpdater.h"

#include "../Framework/GMultiDocManager.h"
#include "../Framework/GMultiDocTemplate.h"

#include "../Port/ImportGdal.h"
#include <filesystem> 
#include <regex>
#include <algorithm>
#include <string>
#include <iostream>

using namespace std;

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef HRESULT(STDAPICALLTYPE* CTLREGPROC)(void); //see COMPOBJ.H

/////////////////////////////////////////////////////////////////////////////
// CMapperApp

BEGIN_MESSAGE_MAP(CMapperApp, CWinAppEx)
	//{{AFX_MSG_MAP(CMapperApp)
	ON_COMMAND(ID_APP_ABOUT, &CMapperApp::OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	// Standard print setup command
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_IMPORT, CMapperApp::OnFileImport)
	ON_COMMAND(ID_WINDOW_CLOSEALL, OnCloseAll)
	ON_COMMAND_RANGE(WM_PLUGINITEMS, WM_PLUGINITEMS+100, OnPlugin)
END_MESSAGE_MAP()

CMapperApp::CMapperApp() noexcept
	: CWinAppEx(TRUE), m_gdiplusToken(0)
{
	//	AfxSetAmbientActCtx(FALSE);
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
	m_nAutosaveInterval = 30*60*1000;//if idle time exceeds 30 minutes auto save it.

#ifdef _MANAGED
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	SetAppID(_T("Diwatu.Mapper.2012.09.15"));

#ifdef _DEBUG	//using visual studio diagnose tool is easy to find memory leak
	//	AfxEnableMemoryLeakDump(TRUE);
	//	AfxEnableMemoryTracking(TRUE);
	//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF |	_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));//_CRTDBG_LEAK_CHECK_DF will make _CrtDumpMemoryLeaks called automatically
	//	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif
}


int CMapperApp::Run()
{
	int nArgs = 0;
	const LPWSTR cmdline = GetCommandLineW();
	const LPWSTR* aruments = CommandLineToArgvW(cmdline, &nArgs);
	if(nArgs==5)
	{
		BYTE action = _ttoi(aruments[4]);
		if(_wcsicmp(aruments[1], L"convert")==0)
		{
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\SynWithUsb\\Diwatu Map Data\\FlashMap"));
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\SynWithUsb\\Diwatu Map Data\\Singapore"));
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\SynWithUsb\\Diwatu Map Data\\�������ӵ�ͼ2008"));
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\SynWithUsb\\Diwatu Map Data\\ȫ�й���ͼ"));
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\SynWithUsb\\Diwatu Map Data\\ͼ�ε�ͼ"));
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\SynWithUsb\\Diwatu Map Data\\���ݿ��ͼ"));
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\Programs\\Diwatu\\Samples"));
			//	CMapperApp::ConvertFile(_T("C:\\Chen\\Programs\\Popunder"));

			CMapperApp::ConvertFile(CString(aruments[2]), CString(aruments[3]), action);
		}
		return FALSE;
	}
	else
		return CWinAppEx::Run();
}


CMapperApp theApp;

BOOL CMapperApp::InitInstance()
{
	CWinAppEx::InitInstance();
	//afxAmbientActCtx = FALSE;

	SetRegistryKey(_T("Diwatu"));
	LoadStdProfileSettings(4);

	const Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);
	if(afxGlobalData.fontRegular.GetSafeHandle()!=nullptr)
	{
		::DeleteObject(afxGlobalData.fontRegular.Detach());

		LOGFONT logfont = {0};
		::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &logfont, 0);
		afxGlobalData.SetMenuFont(&logfont, true);
	}

	const DWORD dwVersion = AfxGetSoftVersion();
	const WORD year = (dwVersion>>16);
	const BYTE month = ((dwVersion<<16)>>24);
	const BYTE day = ((dwVersion<<24)>>24);
	CString thisVersion;
	thisVersion.Format(_T("%02d.%02d.%02d"), year, month, day);
	const CString serverVersion = CAutoUpdater::GetNewVersion("Diwatu", std::string(CT2A(thisVersion)));
	if(serverVersion!="")
	{
		if(serverVersion!=thisVersion)
		{
			CString strMessage;
			strMessage.Format(_T("A new version %s is available now. Download it?"), serverVersion);
			if(AfxMessageBox(strMessage, MB_YESNO)==IDYES)
			{
				const HWND hWndDesktop = ::GetDesktopWindow();
				ShellExecute(hWndDesktop, _T("open"), _T("http://www.diwatu.com"), nullptr, nullptr, SW_SHOW);
				return FALSE;
			}
		}
	}

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	if(!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	if(!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	::OleInitialize(nullptr);
	AfxEnableControlContainer();
	EnableTaskbarInteraction();

	CWinAppEx::InitContextMenuManager();
	CWinAppEx::InitKeyboardManager();
	CWinAppEx::InitTooltipManager();
	CWinAppEx::InitShellManager();

	CTooltipManager* pTooltipManager = theApp.GetTooltipManager();
	if(pTooltipManager!=nullptr)
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

	ASSERT(m_pDocManager==nullptr);
	m_pDocManager = new GMultiDocManager();
	GMultiDocTemplate* pDocTemplate = new GMultiDocTemplate(
		IDR_GRFTYPE,
		RUNTIME_CLASS(CGrfDoc),
		RUNTIME_CLASS(CChildFrame),
		GWRUNTIME_CLASS(CGrfView), nullptr);
	pDocTemplate->SetContainerInfo(IDR_GRFTYPE_CNTR_IP);

	m_pDocManager->AddDocTemplate(pDocTemplate);//AddDocTemplate(pDocTemplate); 
	if(pDocTemplate->m_hMenuShared!=nullptr)
	{
		::DestroyMenu(pDocTemplate->m_hMenuShared);
		pDocTemplate->m_hMenuShared = nullptr;
	}
	GMultiDocTemplate* pGisDocTemplate = new GMultiDocTemplate(
		IDR_GISTYPE,
		RUNTIME_CLASS(CGisDoc),
		RUNTIME_CLASS(CChildFrame),
		GWRUNTIME_CLASS(CGisView),
		GWRUNTIME_CLASS(CAidView<CGisView>));
	pGisDocTemplate->SetContainerInfo(IDR_GISTYPE_CNTR_IP);
	m_pDocManager->AddDocTemplate(pGisDocTemplate); //AddDocTemplate(pGisDocTemplate);
	if(pGisDocTemplate->m_hMenuShared!=nullptr)
	{
		::DestroyMenu(pGisDocTemplate->m_hMenuShared);
		pGisDocTemplate->m_hMenuShared = nullptr;
	}

	GMultiDocTemplate* pGeoDocTemplate = new GMultiDocTemplate(
		IDR_GEOTYPE,
		RUNTIME_CLASS(CGeoDoc),
		RUNTIME_CLASS(CChildFrame),
		GWRUNTIME_CLASS(CGeoView),
		GWRUNTIME_CLASS(CAidView<CGisView>));
	pGeoDocTemplate->SetContainerInfo(IDR_GEOTYPE_CNTR_IP);
	m_pDocManager->AddDocTemplate(pGeoDocTemplate); //AddDocTemplate(pGeoDocTemplate);
	if(pGeoDocTemplate->m_hMenuShared!=nullptr)
	{
		::DestroyMenu(pGeoDocTemplate->m_hMenuShared);
		pGeoDocTemplate->m_hMenuShared = nullptr;
	}

	CMainFrame* pMainFrame = new CMainFrame();
	EnableLoadWindowPlacement(FALSE);
	if(pMainFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW|FWS_ADDTOTITLE, nullptr, nullptr)==TRUE)
	{
		m_pMainWnd = pMainFrame;

		CWinAppEx::EnableShellOpen();
		CWinAppEx::RegisterShellFileTypes(TRUE);

		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
		if(ProcessShellCommand(cmdInfo)==FALSE)
			return FALSE;

		if(!ReloadWindowPlacement(pMainFrame))
		{
			pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
			pMainFrame->UpdateWindow();
		}
		/// force a title-bar icon (MFC Wizard doesn't set it up correctly)
		const HICON hIcon = LoadIcon(IDR_MAINFRAME);
		const HICON hPrev = pMainFrame->SetIcon(hIcon, FALSE);
		if(hPrev!=nullptr&&hPrev!=hIcon)
			DestroyIcon(hPrev);

		//		CWinAppEx::m_bResourceSmartUpdate = FALSE;
		AfxIncreaseCounter();

		AddPlugins();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
// If the main window has gone away there is no need to do pre-translation.
// If the pre-translation were allowed to proceed the
//  CWinAppEx::PreTranslateMessage may bail out without calling
/// the app's ExitInstance nor destructing the app object.

int CMapperApp::ExitInstance()
{
	CMFCVisualManager::DestroyInstance(); //this line is very important,get rid of the exit error on Windows XP
	AfxOleTerm(FALSE);

	delete m_pDocManager;
	m_pDocManager = nullptr;

	ControlBarCleanUp();

	//if(m_pszRegistryKey!=nullptr)//otherwise memory leak
	//{
	//	free((void*)m_pszRegistryKey);
	//	m_pszRegistryKey = nullptr;
	//}
	//if(m_pszProfileName!=nullptr)//otherwise memory leak
	//{
	//	free((void*)m_pszProfileName);
	//	m_pszProfileName = nullptr;
	//}

	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	return CWinAppEx::ExitInstance();
}

// CMapperApp customization load/save methods

void CMapperApp::PreLoadState()
{
}

void CMapperApp::LoadCustomState()
{
}

void CMapperApp::SaveCustomState()
{
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

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
	BOOL OnInitDialog() override;

	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	const DWORD dwVersion = AfxGetSoftVersion();
	const WORD year = (dwVersion>>16);
	const BYTE month = ((dwVersion<<16)>>24);
	const BYTE day = ((dwVersion<<24)>>24);

	CString strVersion;
	strVersion.Format(_T("%4d.%2d.%2d"), year, month, day);
	GetDlgItem(IDC_VERSION)->SetWindowText(strVersion);
	return TRUE;
}

void CMapperApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CMapperApp::OnFileImport()
{
	TCHAR CurrentDir[256];
	GetCurrentDirectory(255, CurrentDir);
	CString strPath = AfxGetApp()->GetProfileString(_T(""), _T("Import Path"), CurrentDir);

	TCHAR BASED_CODE szIM_IM_Filter[] = _T("All Diwatu Files(*.adf,*.shp,*.e00,*.Tab,*.mif,*.dxf,*.dwg,*.dgn,*.RT1,*.fme,*.geojson,*.json,*.geo.json,*.Ai,*.eps,*.pdf,*.svg)|*.adf;*.shp;*.e00;*.Tab;*.mif;*.dxf;*.dwg;*.dgn;*.RT1;*.fme;*.geojson;*.json;*.Ai;*.eps;*.pdf;*.svg;*.js|")
		//	_T("ArcInfo Coverages(*.adf,*.arc,*.lab,*.cnt,*.pal,*.pat,*.pax,*.txt)|*.abf|")
		_T("ArcInfo Coverage Files(*.adf)|*.adf|")
		_T("ArcInfo E00 Files(*.e00)|*.e00|")
		_T("ArcView Shape Files(*.shp)|*.shp|")
		_T("AutoCAD DXF Files(*.dxf)|*.dxf|")
		_T("AutoCAD DWG Files(*.dwg)|*.dwg|")
		_T("MapInfo File Files(*.Mif)|*.Mif|")
		_T("MapInfo File Files(*.Tab)|*.Tab|")
		_T("GeoJSON Files(*.geojson;*.geo.json)|*.geojson;*.geo.json|")
		_T("JSON Files(*.json)|*.json|")
		_T("Microstation DGN Files(*.Dgn)|*.Dgn|")
		_T("U.S. Census TIGER/Line Files(*.RT1)|*.RT1|")
		//	_T("FME Files(*.fme)|*.fme|")
		_T("Adobe Illustrator Pdf Files(*.pdf)|*.pdf|")
		_T("Adobe Illustrator AI Files(*.ai)|*.ai|")
		_T("Encapsulated Postscript Files(*.eps)|*.eps|");
	_T("Scalable Vector Gdiplus::Graphics Files(*.svg)|*.svg|");
	//	_T("CorelDRAW Gdiplus::Graphics files(*.CDR)|*.CDR|");

	CFileDialog dlg(true,
		_T(""),
		strPath,
		OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT,
		szIM_IM_Filter,
		AfxGetMainWnd());
	TCHAR buffer[65535];
	memset(buffer, 0, 65535);
	dlg.m_ofn.lpstrFile = buffer;
	dlg.m_ofn.nMaxFile = 65535;
	dlg.m_ofn.lpstrTitle = _T("Select Files to Import");
	if(dlg.DoModal()==IDOK)
	{
		CString strPath;// = dlg.GetFolderPath();
		CStringList files;
		POSITION pos1 = dlg.GetStartPosition();
		while(pos1!=nullptr)
		{
			CString strFile = dlg.GetNextPathName(pos1);
			CString strExt = strFile.Mid(strFile.ReverseFind(_T('.'))+1);
			strExt.MakeUpper();
			strPath = strFile.Left(strFile.ReverseFind(_T('\\')));
			if(strExt==_T("JSON") || strExt==_T("GEOJSON"))
			{
				CMultiDocTemplate* pDocTemplate = (CMultiDocTemplate*)m_pDocManager->GetBestTemplate(_T("*.gis"));
				if(pDocTemplate!=nullptr)
				{
					CDocument* pDocument = pDocTemplate->CreateNewDocument();
					if(pDocument!=nullptr)
					{
						BOOL bAutoDelete = pDocument->m_bAutoDelete;
						pDocument->m_bAutoDelete = FALSE; // don't destroy if something goes wrong
						CFrameWnd* pFrame = pDocTemplate->CreateNewFrame(pDocument, nullptr);
						pDocument->m_bAutoDelete = bAutoDelete;
						if(pFrame==nullptr)
						{
							AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
							pDocument->m_bAutoDelete = FALSE;
							pDocument->OnCloseDocument();
							delete pDocument;
							SetCurrentDirectory(CurrentDir);
							return;
						}
						ASSERT_VALID(pFrame);
						pDocTemplate->SetDefaultTitle(pDocument);
						if(((CGisDoc*)pDocument)->ImportJson(strFile)==TRUE)
						{
							pDocTemplate->InitialUpdateFrame(pFrame, pDocument, TRUE);
							pDocument->SetModifiedFlag(TRUE);
							pDocument->UpdateAllViews(nullptr);
						}
						else
						{
							pFrame->PostMessage(WM_CLOSE, 0, 0);
						}
					}
				}
				continue;
			}
			else if((strExt==_T("SVG")&&CImportGdal::Support(strFile)==FALSE)||strExt==_T("EPS")||strExt==_T("AI")||strExt==_T("PDF")||strExt==_T("CDR"))
			{
				CMultiDocTemplate* pDocTemplate = (CMultiDocTemplate*)m_pDocManager->GetBestTemplate(_T("*.grf"));
				if(CDocument* pDocument = pDocTemplate->CreateNewDocument(); pDocument!=nullptr)
				{
					BOOL bAutoDelete = pDocument->m_bAutoDelete;
					pDocument->m_bAutoDelete = FALSE; // don't destroy if something goes wrong
					CFrameWnd* pFrame = pDocTemplate->CreateNewFrame(pDocument, nullptr);
					pDocument->m_bAutoDelete = bAutoDelete;
					if(pFrame==nullptr)
					{
						AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
						pDocument->m_bAutoDelete = FALSE;
						pDocument->OnCloseDocument();
						delete pDocument;
						SetCurrentDirectory(CurrentDir);
						return;
					}
					ASSERT_VALID(pFrame);
					pDocTemplate->SetDefaultTitle(pDocument);
					//	pDocTemplate->->m_nUntitledCount++;
					if(((CGrfDoc*)pDocument)->Import(strFile)==TRUE)
					{
						pDocTemplate->InitialUpdateFrame(pFrame, pDocument, TRUE);
						pDocument->SetModifiedFlag(TRUE);
						pDocument->UpdateAllViews(nullptr);
					}
					else
					{
						pFrame->PostMessage(WM_CLOSE, 0, 0);
					}
				}
				continue;
			}
			else if(strExt==_T("ADF"))
			{
				strFile = strPath.Mid(strPath.ReverseFind(_T('\\'))+1);
				strPath = strPath.Left(strPath.GetLength()-strFile.GetLength()-1);
			}
			else
				strFile = strFile.Mid(strPath.GetLength()+1);

			if(files.Find(strFile)==nullptr)
				files.AddTail(strFile);
		}

		if(files.GetCount()>0)
		{
			CMultiDocTemplate* pDocTemplate = (CMultiDocTemplate*)m_pDocManager->GetBestTemplate(_T("*.gis"));
			if(pDocTemplate!=nullptr)
			{
				CDocument* pDocument = pDocTemplate->CreateNewDocument();
				if(pDocument!=nullptr)
				{
					BOOL bAutoDelete = pDocument->m_bAutoDelete;
					pDocument->m_bAutoDelete = FALSE; // don't destroy if something goes wrong
					CFrameWnd* pFrame = pDocTemplate->CreateNewFrame(pDocument, nullptr);
					pDocument->m_bAutoDelete = bAutoDelete;
					if(pFrame==nullptr)
					{
						AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
						pDocument->m_bAutoDelete = FALSE;
						pDocument->OnCloseDocument();
						delete pDocument;
						SetCurrentDirectory(CurrentDir);
						return;
					}
					ASSERT_VALID(pFrame);
					pDocTemplate->SetDefaultTitle(pDocument);
					//	pDocTemplate->->m_nUntitledCount++;
					if(((CGisDoc*)pDocument)->ImportGdal(strPath, files, false)==TRUE)
					{
						pDocTemplate->InitialUpdateFrame(pFrame, pDocument, TRUE);
						pDocument->SetModifiedFlag(TRUE);
						pDocument->UpdateAllViews(nullptr);
					}
					else
					{
						pFrame->PostMessage(WM_CLOSE, 0, 0);
					}
				}
			}
		}

		SetCurrentDirectory(CurrentDir);
		AfxGetApp()->WriteProfileString(_T(""), _T("Import Path"), strPath);
	}
}

void CMapperApp::OnCloseAll()
{
	((GMultiDocManager*)m_pDocManager)->CloseAllDocumentsWithSaveModified(TRUE);
}

typedef IPlugin* (*PLUGIN_FACTORY)();
typedef void(*PLUGIN_CLEANUP)(IPlugin*);
typedef char* (*PLUGIN_GETTITLE)();

void CMapperApp::OnPlugin(UINT nId)
{
	for(std::list<Plugin>::iterator it = m_plugins.begin(); it!=m_plugins.end(); it++)
	{
		if(it->ID==nId)
		{
			const HMODULE hMod = ::LoadLibraryA(it->file);
			if(hMod!=nullptr)
			{
				const PLUGIN_FACTORY p_factory_function = (PLUGIN_FACTORY) ::GetProcAddress(hMod, "CreatePlugin");
				const PLUGIN_CLEANUP p_cleanup_function = (PLUGIN_CLEANUP) ::GetProcAddress(hMod, "ReleasePlugin");
				if(p_factory_function!=nullptr&&p_cleanup_function!=nullptr)
				{
					IPlugin* plugin = (*p_factory_function)();
					if(plugin!=nullptr)
						plugin->Invoke(0);
					(*p_cleanup_function)(plugin);
				}
				::FreeLibrary(hMod);
			}
			break;
		}
	}
}

#include <Windows.h>

void CMapperApp::AddPlugins()
{
	return;

	//Sharper::IPlugin^ c = gcnew Sharper::Plugin();
	//   IntPtr p = Marshal::GetFunctionPointerForDelegate(c->GetDelegate());
	//
	//   NativeToManaged funcPointer = (NativeToManaged) p.ToPointer();
	//
	//   // invoke the delegate
	//   funcPointer(data);
	const CStringA exefolder = GetExeDirectory();
	WIN32_FIND_DATAA find_data;
	const HANDLE hFind = ::FindFirstFileA(exefolder+"plugins\\*.dll", &find_data);
	BOOL fOK = TRUE;
	int counter = 0;
	while(hFind!=INVALID_HANDLE_VALUE&&fOK)
	{
		CStringA strFile = exefolder+"plugins\\"+find_data.cFileName;
		const HMODULE hMod = ::LoadLibraryA(strFile);
		if(hMod!=nullptr)
		{
			const PLUGIN_FACTORY p_factory_function = (PLUGIN_FACTORY) ::GetProcAddress(hMod, "CreatePlugin");
			const PLUGIN_CLEANUP p_cleanup_function = (PLUGIN_CLEANUP) ::GetProcAddress(hMod, "ReleasePlugin");
			const PLUGIN_GETTITLE p_gettitle_function = (PLUGIN_GETTITLE) ::GetProcAddress(hMod, "GetTitle");
			if(p_factory_function!=nullptr&&p_cleanup_function!=nullptr&&p_gettitle_function!=nullptr)
			{
				char* title = (*p_gettitle_function)();
				Plugin plugin;
				plugin.ID = WM_PLUGINITEMS+counter++;
				plugin.title = CString(title);
				plugin.file = strFile;
				m_plugins.push_back(plugin);
			}
			::FreeLibrary(hMod);
		}
		else
		{
			DWORD error = GetLastError();
		}
		fOK = ::FindNextFileA(hFind, &find_data);
	}

	//	CMFCMenuBar& pMenuBar = ((CMainFrame*)AfxGetMainWnd())->GetMenuBar();
	//	CMenu* pMainMenu = CMenu::FromHandle(pMenuBar.GetHMenu());
	//	CMenu* pPluginMenu = nullptr;
	//	for(int index = 0; index < pMainMenu->GetMenuItemCount(); index++)
	//	{
	//		CString str;
	//		if(pMainMenu->GetMenuString(index, str, MF_BYPOSITION) && str.Compare(_T("Plugins")) == 0)
	//		{
	//			CMenu* pSubmenu = pMainMenu->GetSubMenu(index);
	//			if(pSubmenu == nullptr && !m_plugins.empty())
	//			{
	//				pPluginMenu = new CMenu();
	//				pPluginMenu->CreatePopupMenu();
	//				pMainMenu->ModifyMenuW(index, MF_BYPOSITION | MF_POPUP, (UINT)pPluginMenu->m_hMenu, str);
	//			}
	//			else if(pSubmenu != nullptr && m_plugins.empty())
	//				pMainMenu->RemoveMenu(index, MF_BYPOSITION);
	//			else if(pSubmenu != nullptr)
	//				pSubmenu->RemoveMenu(0, MF_BYPOSITION);
	//			break;
	//		}
	//	}
	//
	//	if(pPluginMenu != nullptr && m_plugins.empty() == false)
	//	{
	//		for(std::list<Plugin>::iterator it = m_plugins.begin(); it != m_plugins.end(); it++)
	//		{
	//			pPluginMenu->AppendMenu(MF_STRING, it->ID, it->title);
	//		}
	//		pMenuBar.CreateFromMenu(pMainMenu->m_hMenu, TRUE, TRUE);
	//		pPluginMenu->DestroyMenu();
	//		delete pPluginMenu;
	//	}
	//	pMainMenu->Detach();
}
void CMapperApp::ConvertFile(CString input, CString output, BYTE action)
{
	if(PathFileExists(input)==FALSE)
		return;
	if(PathFileExists(output)==FALSE)
		CreateDirectory(output, nullptr);

	struct {
		std::string code2;
		std::string code3;
		std::string name;
	} map[] = {
		{"AW", "ABW", "Aruba"},
		{"AF", "AFG", "Afghanistan"},
		{"AO", "AGO", "Angola"},
		{"AI", "AIA", "Anguilla"},
		{"AX", "ALA", "Åland Islands"},
		{"AL", "ALB", "Albania"},
		{"AD", "AND", "Andorra"},
		{"AE", "ARE", "United Arab Emirates"},
		{"AR", "ARG", "Argentina"},
		{"AM", "ARM", "Armenia"},
		{"AS", "ASM", "American Samoa"},
		{"AQ", "ATA", "Antarctica"},
		{"TF", "ATF", "French Southern Territories"},
		{"AG", "ATG", "Antigua and Barbuda"},
		{"AU", "AUS", "Australia"},
		{"AT", "AUT", "Austria"},
		{"AZ", "AZE", "Azerbaijan"},
		{"BI", "BDI", "Burundi"},
		{"BE", "BEL", "Belgium"},
		{"BJ", "BEN", "Benin"},
		{"BQ", "BES", "Bonaire, Saint Eustatius and Saba"},
		{"BF", "BFA", "Burkina Faso"},
		{"BD", "BGD", "Bangladesh"},
		{"BG", "BGR", "Bulgaria"},
		{"BH", "BHR", "Bahrain"},
		{"BS", "BHS", "Bahamas"},
		{"BA", "BIH", "Bosnia and Herzegovina"},
		{"BL", "BLM", "Saint Barthélemy"},
		{"BY", "BLR", "Belarus"},
		{"BZ", "BLZ", "Belize"},
		{"BM", "BMU", "Bermuda"},
		{"BO", "BOL", "Bolivia"},
		{"BR", "BRA", "Brazil"},
		{"BB", "BRB", "Barbados"},
		{"BN", "BRN", "Brunei Darussalam"},
		{"BT", "BTN", "Bhutan"},
		{"BV", "BVT", "Bouvet Island"},
		{"BW", "BWA", "Botswana"},
		{"CF", "CAF", "Central African Republic"},
		{"CA", "CAN", "Canada"},
		{"CC", "CCK", "Cocos Islands"},
		{"CH", "CHE", "Switzerland"},
		{"CL", "CHL", "Chile"},
		{"CN", "CHN", "China"},
		{"CI", "CIV", "Côte dIvoire"},
		{"CM", "CMR", "Cameroon"},
		{"CD", "COD", "Democratic Republic of the Congo"},
		{"CG", "COG", "Congo"},
		{"CK", "COK", "Cook Islands"},
		{"CO", "COL", "Colombia"},
		{"KM", "COM", "Comoros"},
		{"CV", "CPV", "Cape Verde"},
		{"CR", "CRI", "Costa Rica"},
		{"CU", "CUB", "Cuba"},
		{"CW", "CUW", "Curaçao"},
		{"CX", "CXR", "Christmas Island"},
		{"KY", "CYM", "Cayman Islands"},
		{"CY", "CYP", "Cyprus"},
		{"CZ", "CZE", "Czech Republic"},
		{"DE", "DEU", "Germany"},
		{"DJ", "DJI", "Djibouti"},
		{"DM", "DMA", "Dominica"},
		{"DK", "DNK", "Denmark"},
		{"DO", "DOM", "Dominican Republic"},
		{"DZ", "DZA", "Algeria"},
		{"EC", "ECU", "Ecuador"},
		{"EG", "EGY", "Egypt"},
		{"ER", "ERI", "Eritrea"},
		{"EH", "ESH", "Western Sahara"},
		{"ES", "ESP", "Spain"},
		{"EE", "EST", "Estonia"},
		{"ET", "ETH", "Ethiopia"},
		{"FI", "FIN", "Finland"},
		{"FJ", "FJI", "Fiji"},
		{"FK", "FLK", "Falkland Islands"},
		{"FR", "FRA", "France"},
		{"FO", "FRO", "Faroe Islands"},
		{"FM", "FSM", "Micronesia"},
		{"GA", "GAB", "Gabon"},
		{"GB", "GBR", "United Kingdom"},
		{"GE", "GEO", "Georgia"},
		{"GG", "GGY", "Guernsey"},
		{"GH", "GHA", "Ghana"},
		{"GI", "GIB", "Gibraltar"},
		{"GN", "GIN", "Guinea"},
		{"GP", "GLP", "Guadeloupe"},
		{"GM", "GMB", "Gambia"},
		{"GW", "GNB", "Guinea-Bissau"},
		{"GQ", "GNQ", "Equatorial Guinea"},
		{"GR", "GRC", "Greece"},
		{"GD", "GRD", "Grenada"},
		{"GL", "GRL", "Greenland"},
		{"GT", "GTM", "Guatemala"},
		{"GF", "GUF", "French Guiana"},
		{"GU", "GUM", "Guam"},
		{"GY", "GUY", "Guyana"},
		{"HM", "HMD", "Heard Island and McDonald Islands"},
		{"HN", "HND", "Honduras"},
		{"HR", "HRV", "Croatia"},
		{"HT", "HTI", "Haiti"},
		{"HU", "HUN", "Hungary"},
		{"ID", "IDN", "Indonesia"},
		{"IM", "IMN", "Isle of Man"},
		{"IN", "IND", "India"},
		{"IO", "IOT", "British Indian Ocean Territory"},
		{"IE", "IRL", "Ireland"},
		{"IR", "IRN", "Iran"},
		{"IQ", "IRQ", "Iraq"},
		{"IS", "ISL", "Iceland"},
		{"IL", "ISR", "Israel"},
		{"IT", "ITA", "Italy"},
		{"JM", "JAM", "Jamaica"},
		{"JE", "JEY", "Jersey"},
		{"JO", "JOR", "Jordan"},
		{"JP", "JPN", "Japan"},
		{"KZ", "KAZ", "Kazakhstan"},
		{"KE", "KEN", "Kenya"},
		{"KG", "KGZ", "Kyrgyzstan"},
		{"KH", "KHM", "Cambodia"},
		{"KI", "KIR", "Kiribati"},
		{"KN", "KNA", "Saint Kitts and Nevis"},
		{"KR", "KOR", "North Korea"},
		{"KW", "KWT", "Kuwait"},
		{"LA", "LAO", "Laos"},
		{"LB", "LBN", "Lebanon"},
		{"LR", "LBR", "Liberia"},
		{"LY", "LBY", "Libya"},
		{"LC", "LCA", "Saint Lucia"},
		{"LI", "LIE", "Liechtenstein"},
		{"LK", "LKA", "Sri Lanka"},
		{"LS", "LSO", "Lesotho"},
		{"LT", "LTU", "Lithuania"},
		{"LU", "LUX", "Luxembourg"},
		{"LV", "LVA", "Latvia"},
		{"MF", "MAF", "Saint Martin(French part)"},
		{"MA", "MAR", "Morocco"},
		{"MC", "MCO", "Monaco"},
		{"MD", "MDA", "Moldova"},
		{"MG", "MDG", "Madagascar"},
		{"MV", "MDV", "Maldives"},
		{"MX", "MEX", "Mexico"},
		{"MH", "MHL", "Marshall Islands"},
		{"MK", "MKD", "Macedonia"},
		{"ML", "MLI", "Mali"},
		{"MT", "MLT", "Malta"},
		{"MT", "MLT", "Malta"},
		{"MM", "MMR", "Myanmar"},
		{"ME", "MNE", "Montenegro"},
		{"MN", "MNG", "Mongolia"},
		{"MP", "MNP", "Northern Mariana Islands"},
		{"MZ", "MOZ", "Mozambique"},
		{"MR", "MRT", "Mauritania"},
		{"MS", "MSR", "Montserrat"},
		{"MQ", "MTQ", "Martinique"},
		{"MU", "MUS", "Mauritius"},
		{"MW", "MWI", "Malawi"},
		{"MY", "MYS", "Malaysia"},
		{"YT", "MYT", "Mayotte"},
		{"NA", "NAM", "Namibia"},
		{"NC", "NCL", "New Caledonia"},
		{"NE", "NER", "Niger"},
		{"NF", "NFK", "Norfolk Island"},
		{"NG", "NGA", "Nigeria"},
		{"NI", "NIC", "Nicaragua"},
		{"NU", "NIU", "Niue"},
		{"NL", "NLD", "Netherlands"},
		{"NO", "NOR", "Norway"},
		{"NP", "NPL", "Nepal"},
		{"NR", "NRU", "Nauru"},
		{"NZ", "NZL", "New Zealand"},
		{"OM", "OMN", "Oman"},
		{"PK", "PAK", "Pakistan"},
		{"PA", "PAN", "Panama"},
		{"PN", "PCN", "Pitcairn"},
		{"PE", "PER", "Peru"},
		{"PH", "PHL", "Philippines"},
		{"PW", "PLW", "Palau"},
		{"PG", "PNG", "Papua New Guinea"},
		{"PL", "POL", "Poland"},
		{"PR", "PRI", "Puerto Rico"},
		{"KP", "PRK", "South Korea"},
		{"PT", "PRT", "Portugal"},
		{"PY", "PRY", "Paraguay"},
		{"PS", "PSE", "Palestine"},
		{"PF", "PYF", "French Polynesia"},
		{"QA", "QAT", "Qatar"},
		{"RE", "REU", "Réunion"},
		{"RO", "ROU", "Romania"},
		{"RU", "RUS", "Russia"},
		{"RW", "RWA", "Rwanda"},
		{"SA", "SAU", "Saudi Arabia"},
		{"SD", "SDN", "Sudan"},
		{"SN", "SEN", "Senegal"},
		{"SG", "SGP", "Singapore"},
		{"GS", "SGS", "South Georgia and the South Sandwich Islands"},
		{"SH", "SHN", "Saint Helena"},
		{"SJ", "SJM", "Svalbard and Jan Mayen"},
		{"SB", "SLB", "Solomon Islands"},
		{"SL", "SLE", "Sierra Leone"},
		{"SV", "SLV", "El Salvador"},
		{"SM", "SMR", "San Marino"},
		{"SO", "SOM", "Somalia"},
		{"PM", "SPM", "Saint Pierre and Miquelon"},
		{"RS", "SRB", "Serbia"},
		{"SS", "SSD", "South Sudan"},
		{"ST", "STP", "Sao Tome and Principe"},
		{"SR", "SUR", "Suriname"},
		{"SK", "SVK", "Slovakia"},
		{"SI", "SVN", "Slovenia"},
		{"SE", "SWE", "Sweden"},
		{"SZ", "SWZ", "Eswatini"},
		{"SX", "SXM", "Sint Maarten (Dutch part)"},
		{"SC", "SYC", "Seychelles"},
		{"SY", "SYR", "Syria"},
		{"TC", "TCA", "Turks and Caicos Islands"},
		{"TD", "TCD", "Chad"},
		{"TG", "TGO", "Togo"},
		{"TH", "THA", "Thailand"},
		{"TJ", "TJK", "Tajikistan"},
		{"TK", "TKL", "Tokelau"},
		{"TM", "TKM", "Turkmenistan"},
		{"TL", "TLS", "Timor-Leste"},
		{"TO", "TON", "Tonga"},
		{"TT", "TTO", "Trinidad and Tobago"},
		{"TN", "TUN", "Tunisia"},
		{"TR", "TUR", "Turkey"},
		{"TV", "TUV", "Tuvalu"},
		{"TW", "TWN", "Taiwan, Province of China"},
		{"TZ", "TZA", "Tanzania"},
		{"UG", "UGA", "Uganda"},
		{"UA", "UKR", "Ukraine"},
		{"UM", "UMI", "United States Minor Outlying Islands"},
		{"UY", "URY", "Uruguay"},
		{"US", "USA", "United States"},
		{"UZ", "UZB", "Uzbekistan"},
		{"VA", "VAT", "Holy See"},
		{"VC", "VCT", "Saint Vincent and the Grenadines"},
		{"VE", "VEN", "Venezuela"},
		{"VG", "VGB", "Virgin Islands, British"},
		{"VI", "VIR", "Virgin Islands, U.S."},
		{"VN", "VNM", "Vietnam"},
		{"VU", "VUT", "Vanuatu"},
		{"WF", "WLF", "Wallis and Futuna"},
		{"WS", "WSM", "Samoa"},
		{"YE", "YEM", "Yemen"},
		{"ZA", "ZAF", "South Africa"},
		{"ZM", "ZMB", "Zambia"},
		{"ZW", "ZWE", "Zimbabwe"},
		{"", ""},
	};
	switch(action)
	{
		case 0://GDAM: convert shp to gis		
			{
				const std::regex pattern("gadm41_([A-Z]+)_([1-9]).shp$");
				CFileFind fileFind1;
				BOOL bMoreFiles1 = fileFind1.FindFile(input+"\\*");
				while(bMoreFiles1)
				{
					bMoreFiles1 = fileFind1.FindNextFile();
					if(fileFind1.IsDots())
					{
					}
					else if(fileFind1.IsDirectory()==TRUE)
					{
						CString strFolder = fileFind1.GetFilePath();
						if(PathFileExists(strFolder+_T("\\GADM"))==TRUE)
						{
							CFileFind fileFind2;
							BOOL bMoreFiles2 = fileFind2.FindFile(strFolder+_T("\\GADM\\*.shp"));
							while(bMoreFiles2)
							{
								bMoreFiles2 = fileFind2.FindNextFile();
								CString strShpFile = fileFind2.GetFilePath();
								std::smatch match;
								std::string fff(CW2A(strShpFile.GetString()));
								if(std::regex_search(fff, match, pattern))
								{
									OutputDebugString(strShpFile+_T("\n"));
									std::string code = match[1];
									std::string level = match[2];
									std::string name;
									for(int index = 0; map[index].code3!=""; index++)
									{
										if(map[index].code3==code)
										{
											name = std::format("Map of {} Administrative {}", map[index].name, level);
											break;
										}
									}

									CString basemap(code.c_str());
									basemap += _T("_adm");
									basemap += CString(level.c_str());
									if(PathFileExists(strFolder+_T("\\")+basemap+_T(".gis"))==false)
									{
										CGisDoc* pDocument = new CGisDoc();
										pDocument->hiddenlevel = atoi(level.c_str());
										CStringList files;
										files.AddTail(fileFind2.GetFileName());
										pDocument->ImportGdal(strFolder+_T("\\GADM"), files, false);
										pDocument->DoSave(strFolder+_T("\\GADM\\")+basemap+_T(".gis"), TRUE);
										CopyFile(strFolder+_T("\\GADM\\")+basemap+_T(".gis"), strFolder+"\\"+basemap+_T(".gis"), TRUE);
										CopyFile(strFolder+_T("\\GADM\\gadm41_")+CString(code.c_str())+_T("_")+CString(level.c_str())+_T(".mdb"), strFolder+_T("\\gadm41_")+CString(code.c_str())+"_"+CString(level.c_str())+_T(".mdb"), TRUE);
										pDocument->m_bAutoDelete = FALSE;
										pDocument->OnCloseDocument();
										delete pDocument;
										Sleep(1000);
									}
								}
							}
							DeleteDirectory(strFolder+"\\GADM");
						}
					}
				}
			}
			break;
		case 1://convert gis to wvppbf
			{
				CFileFind fileFind;
				BOOL bMoreFiles = fileFind.FindFile(input+_T("\\*"));
				while(bMoreFiles)
				{
					bMoreFiles = fileFind.FindNextFile();
					if(fileFind.IsDots())
						continue;
					else if(fileFind.IsDirectory()==TRUE)
					{
						CString strSubDirectry = fileFind.GetFilePath();
						CString strSubFolder = fileFind.GetFileName();
						CMapperApp::ConvertFile(strSubDirectry, output+"\\"+strSubFolder, action);
					}
					CString strFile = fileFind.GetFilePath();
					const std::filesystem::path filepath(strFile.GetString());
					std::string ext = filepath.extension().string();
					const std::string name = filepath.stem().string();
					std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
					if(ext==".grf"||ext==".gis")
					{
						//	const std::regex pattern("([A-Z]+)_adm([1-9]).gis$");
						//	std::smatch match;
						//	std::string fff(CW2A(strGisFile.GetString()));
						//	if(std::regex_search(fff, match, pattern))
						{
							OutputDebugString(strFile+_T("\n"));
							//std::string code = match[1];
							//	std::string level = match[2];
							//std::string name;
							//for(int index = 0; map[index].code2 != ""; index++)
							//{
							//	if(std::equal(map[index].code2.begin(), map[index].code2.end(),
							//		code.begin(), code.end(), [](char a, char b) {
							//			return tolower(a) == tolower(b);
							//		}))
							//	{
							//		//		name = std::format("Map of {} Administrative {}", map[index].name, level);
							//		name = std::format("Topological map of {} ", map[index].name);
							//		break;
							//	}
							//}
							CGrfDoc* pDocument = (CGrfDoc*)AfxGetApp()->OpenDocumentFile(strFile);
							if(pDocument!=nullptr)
							{
								pDocument->hiddenlevel = 0;
								//	pDocument->hiddenlevel = atoi(level.c_str());
								CString strPbfFile = output+_T("/")+CString(name.c_str())+_T(".wvppbf");
								pDocument->ReleaseWEBVectorPage(strPbfFile, false);

								Gdiplus::Bitmap* preview = pDocument->CreateThumbnail(240, 160);
								if(preview!=nullptr)
								{
									CLSID pngClsid;
									CLSIDFromString(L"{557cf406-1a04-11d3-9a73-0000f81ef32e}", &pngClsid);
									CString strPngFile = output+_T("/")+CString(name.c_str())+_T(".png");

									preview->Save(strPngFile, &pngClsid, NULL);

									::delete preview;
								}
								pDocument->m_bAutoDelete = FALSE;
								pDocument->OnCloseDocument();
							}
							Sleep(1000);
							delete pDocument;
						}
					}
				}
			}
			break;
		case 2://create thumbnail png
			{
				CFileFind fileFind;
				BOOL bMoreFiles = fileFind.FindFile(input+_T("\\*"));
				while(bMoreFiles)
				{
					bMoreFiles = fileFind.FindNextFile();
					if(fileFind.IsDots())
						continue;
					else if(fileFind.IsDirectory()==TRUE)
					{
						CString strSubDirectry = fileFind.GetFilePath();
						CString strSubFolder = fileFind.GetFileName();
						CMapperApp::ConvertFile(strSubDirectry, output+"\\"+strSubFolder, action);
					}
				}
				CFileFind fileFind2;
				BOOL bMoreFiles2 = fileFind2.FindFile(input+_T("\\*.gis"));
				while(bMoreFiles2)
				{
					bMoreFiles2 = fileFind2.FindNextFile();

					CString strGisFile = fileFind2.GetFilePath();
					CString strPngFile = strGisFile;
					strPngFile.Replace(_T(".gis"), _T(".png"));
					if(PathFileExists(strPngFile)==true)
						continue;

					CGisDoc* pDocument = new CGisDoc();
					if(pDocument->OnOpenDocument((CString)strGisFile))
					{
						//	OutputDebugStringA(_T("\"")+strGisFile +"\", \""+ pDocument->m_Datainfo.GetName()+"\"\n");
						Gdiplus::Bitmap* preview = pDocument->CreateThumbnail(240, 160);
						if(preview!=nullptr)
						{
							CLSID pngClsid;
							CLSIDFromString(L"{557cf406-1a04-11d3-9a73-0000f81ef32e}", &pngClsid);

							preview->Save(strPngFile, &pngClsid, NULL);

							::delete preview;
						}
						pDocument->m_bAutoDelete = FALSE;
						pDocument->OnCloseDocument();
					}
					Sleep(1000);
					delete pDocument;
				}
			}
			break;
		case 3://smallmap: convert "D:\smallmap" c:\1.wvpjson	
			{
				CFileFind fileFind1;
				BOOL bMoreFiles1 = fileFind1.FindFile(input+_T("\\*"));
				while(bMoreFiles1)
				{
					bMoreFiles1 = fileFind1.FindNextFile();
					if(fileFind1.IsDots())
					{
					}
					else if(fileFind1.IsDirectory()==TRUE)
					{
						CString strFolder = fileFind1.GetFilePath();
						CFileFind fileFind2;
						BOOL bMoreFiles2 = fileFind2.FindFile(strFolder+_T("\\*.geojson"));
						while(bMoreFiles2)
						{
							bMoreFiles2 = fileFind2.FindNextFile();
							CStringA strJsonFile = (CStringA)fileFind2.GetFilePath();
							OutputDebugStringA(strJsonFile+"\n");

							CGisDoc* pDocument = new CGisDoc();
							pDocument->hiddenlevel = 0;
							CStringList files;
							files.AddTail(fileFind2.GetFileName());
							if(pDocument->ImportGdal(strFolder, files, true)==TRUE)
							{
								CSize docSize = pDocument->m_Datainfo.GetDocCanvas();
								pDocument->Transform(1, 0, 0, 0, -1, docSize.cy);
							}

							pDocument->DoSave(strFolder+_T("\\")+fileFind2.GetFileTitle()+_T(".grf"), TRUE);
							pDocument->m_bAutoDelete = FALSE;
							pDocument->OnCloseDocument();
							delete pDocument;
							Sleep(1000);
						}
					}
				}
			}
			break;
		case 4://topo.json
			{
				CFileFind fileFind1;
				BOOL bMoreFiles1 = fileFind1.FindFile(input+_T("\\*"));
				while(bMoreFiles1)
				{
					bMoreFiles1 = fileFind1.FindNextFile();
					if(fileFind1.IsDots())
					{
					}
					else if(fileFind1.IsDirectory()==TRUE)
					{
						CString strSubDirectry = fileFind1.GetFilePath();
						CString strSubFolder = fileFind1.GetFileName();
						CMapperApp::ConvertFile(strSubDirectry, output+"\\"+strSubFolder, action);
					}
				}

				CFileFind fileFind2;
				BOOL bMoreFiles2 = fileFind2.FindFile(input+_T("\\*.topo.json"));
				while(bMoreFiles2)
				{
					bMoreFiles2 = fileFind2.FindNextFile();
					CString strJsonFile = fileFind2.GetFilePath();

					OutputDebugStringA(strJsonFile+"\n");

					CGisDoc* pDocument = new CGisDoc();
					pDocument->hiddenlevel = 0;
					if(pDocument->ImportJson(strJsonFile)==TRUE)
					{
						CString fileTitle = fileFind2.GetFileTitle();
						fileTitle.Replace(_T(".topo"), _T(""));

						pDocument->DoSave(output+_T("\\")+fileTitle+_T(".gis"), TRUE);
						pDocument->m_bAutoDelete = FALSE;
						pDocument->OnCloseDocument();
						delete pDocument;
						Sleep(1000);
					}
				}
			}
			break;
		case 5:
			{
				std::string	str;
				const std::filesystem::path filepath(input.GetString());

				const std::string ext = filepath.extension().string();
				const std::string name = filepath.filename().string();
				if(ext==".svg"||ext==".ai"||ext==".eps"||ext==".cdr")
				{
					CGrfDoc* pDocument = new CGrfDoc();
					if(pDocument->Import(input)==TRUE)
					{
						str = pDocument->Convert();
					}
					pDocument->m_bAutoDelete = FALSE;
					pDocument->OnCloseDocument();
					delete pDocument;
				}
				else if(ext==".dxf"||ext==".dwg"||ext==".mif"||ext==".tab"||ext==".dgn")
				{
				}
				else if(CImportGdal::Support(input))
				{
				}

				if(str.empty()==false)
				{
					str = std::regex_replace(str, std::regex("\\\\u"), "\\u");
					const char* data = str.c_str();
					CStdioFile file;
					if(file.Open(output, CFile::modeCreate|CFile::modeWrite|CFile::typeText))
					{
						file.Write(data, strlen(data));
						file.Close();
					}
				}
			}
			break;
		case 6://resave mapper files
			{
				CTime time1(2023, 6, 25, 00, 00, 00);
				CFileException e;
				WIN32_FIND_DATA FindDate;
				const HANDLE hFile = FindFirstFile(input+_T("\\*.*"), &FindDate);
				do
				{
					if(wcscmp(FindDate.cFileName, _T("."))==0||wcscmp(FindDate.cFileName, _T(".."))==0)
						continue;

					if((FindDate.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY)
					{
						const CString subInput = input+_T('\\')+FindDate.cFileName;
						const CString subOutput = output+_T('\\')+FindDate.cFileName;
						CMapperApp::ConvertFile(subInput, subOutput, action);
					}
					else
					{
						FILETIME lasttime = FindDate.ftLastWriteTime;
						CTime time2(lasttime);
						//	if(time2<time1)
						{
							CString strFile = FindDate.cFileName;
							std::filesystem::path filePath = strFile.GetBuffer();
							std::wstring ext = filePath.extension();
							std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
							if(ext==L".gis"||ext==L".grf"||ext==L".geo")
							{
								CString strFileInput = input+_T("\\")+FindDate.cFileName;
								CString strFileOutput = output+_T("\\")+FindDate.cFileName;

								CGrfDoc* pDocument = (CGrfDoc*)AfxGetApp()->OpenDocumentFile(strFileInput);
								if(pDocument!=nullptr)
								{
									pDocument->OnCmdMsg(ID_LABELOID_RESET, 0, nullptr, nullptr);
									pDocument->SetModifiedFlag(TRUE);
									pDocument->DoSave(strFileOutput, TRUE);

									pDocument->m_bAutoDelete = FALSE;
									pDocument->OnCloseDocument();

									OutputDebugString(_T("Passed: ")+strFileInput);

									OutputDebugString(_T("\n"));
								}
								else
								{
									AfxDebugBreak();
									OutputDebugString(_T("Failed: ")+strFileInput);
									OutputDebugString(_T("\n"));
								}
								delete pDocument;
							}
							strFile.ReleaseBuffer();
						}
					}
				} while(FindNextFile(hFile, &FindDate)==TRUE);
			}
			break;
	}
}