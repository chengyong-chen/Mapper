#include "stdafx.h"
#include "resource.h"

#include "afxwinappex.h"
#include "Viewer.h"
#include "MainFrm.h"
#include <io.h>
#include <odbcinst.h>

#include "Global.h"
#include "ChildFrm.h"
#include "GrfDoc.h"
#include "GrfView.h"
#include "GisDoc.h"
#include "GisView.h"
#include "Splash.h"

#include "../Atlas/Atlas1.h"

#include "../Framework/GMultiDocManager.h"
#include "../Framework/GMultiDocTemplate.h"
#include "../Atlas/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CViewerApp

BEGIN_MESSAGE_MAP(CViewerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CViewerApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()

// CViewerApp construction

CViewerApp::CViewerApp()
{
	//	 AfxSetAmbientActCtx(FALSE);

#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif
	SetAppID(_T("Viewer.AppID.NoVersion"));
}

// The one and only CViewerApp object

CViewerApp theApp;

// CViewerApp initialization

BOOL CViewerApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

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
	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if(cmdInfo.m_bRunEmbedded||cmdInfo.m_bRunAutomated)
	{
		return TRUE;
	}
	CString strProject = cmdInfo.m_strFileName;
	if(strProject.IsEmpty()==FALSE)
	{
		CString strRegistryKey;
		strRegistryKey.Format(_T("Diwatu\\%s"), strProject);
		SetRegistryKey(strRegistryKey);
	}
	else
	{
		SetRegistryKey(_T("Diwatu"));
	}
	LoadStdProfileSettings(0);

	CWinAppEx::InitContextMenuManager();
	CWinAppEx::InitKeyboardManager();
	CWinAppEx::InitTooltipManager();
	CWinAppEx::InitShellManager();

	if(afxGlobalData.fontRegular.GetSafeHandle()!=nullptr)
	{
		::DeleteObject(afxGlobalData.fontRegular.Detach());

		LOGFONT logfont = {0};
		::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &logfont, 0);
		afxGlobalData.SetMenuFont(&logfont, true);
	}
	CTooltipManager* pTooltipManager = theApp.GetTooltipManager();
	if(pTooltipManager!=nullptr)
	{
		afxGlobalData.m_nMaxToolTipWidth = 200;

		CMFCToolTipInfo params;
		params.m_bVislManagerTheme = TRUE;
		params.m_bBoldLabel = TRUE;
		params.m_bDrawDescription = TRUE;
		params.m_bDrawIcon = TRUE;
		params.m_bRoundedCorners = TRUE;
		params.m_bDrawSeparator = TRUE;
		params.m_clrFill = RGB(255, 255, 255);
		params.m_clrFillGradient = RGB(228, 228, 240);
		params.m_clrText = RGB(61, 83, 80);
		params.m_clrBorder = RGB(144, 149, 168);
		pTooltipManager->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &params);
	}

#ifdef _AFXDLL
	Enable3dControls(); // Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	const Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);
	const CString strWeb = this->GetProfileString(_T("Html"), _T("Path"), _T(""));
	if(strWeb.IsEmpty()==TRUE)
	{
		TCHAR CurrentDir[256];
		GetCurrentDirectory(255, CurrentDir);
		CString strCurrentPath(CurrentDir);
		strCurrentPath += _T('\\');

		CWinAppEx::WriteProfileString(_T("Server"), _T("Web"), strCurrentPath);
	}
	else
	{
		CWinAppEx::WriteProfileString(_T("Server"), _T("Web"), strWeb);
	}
	const BOOL bEnable = AfxEnableMemoryTracking(FALSE);
	m_pszProfileName = _tcsdup(_T("Viewer"));
	AfxEnableMemoryTracking(bEnable);

	//	AfxGetApp()->WriteProfileString(_T("Navigate"), _T("Port"),_T("Socket"));
	//	AfxGetApp()->WriteProfileString(_T("VechileDB"),_T("DBMS"),_T("ACCESS"));	
	//	AfxGetApp()->WriteProfileString(_T("VechileDB"),_T("DSN"),_T("MS Access Database"));	
	//	AfxGetApp()->WriteProfileString(_T("VechileDB"),_T("ServerName"),_T("Trasvr"));	
	//	AfxGetApp()->WriteProfileString(_T("VechileDB"),_T("UID"),_T(""));	
	//	AfxGetApp()->WriteProfileString(_T("VechileDB"),_T("PWD"),_T(""));	

	/*	if(m_ClientSocket.m_bLogin == true)
		{
			*m_ClientSocket.m_pArOut << MAPCLIENT_ATLAS;
			*m_ClientSocket.m_pArOut << (WORD)(MAPCLIENT_ATLAS^0XFFFF);
			*m_ClientSocket.m_pArOut << strProject;
			m_ClientSocket.m_pArOut->Flush();

			*m_ClientSocket.m_pArOut << MAPCLIENT_ATLASLOCATION;
			*m_ClientSocket.m_pArOut << (WORD)(MAPCLIENT_ATLASLOCATION^0XFFFF);
			*m_ClientSocket.m_pArOut << strProject;
			m_ClientSocket.m_pArOut->Flush();
			AfxMessageBox(IDS_WELCOMETOSUERS,MB_OK);
			if(m_ClientSocket.WaitForAnswer(0,1000) == WAIT_TIMEOUT)
			{
			}
		}
	*/
	CString strAtlFile;
	const CString strMapPath = this->GetProfileString(_T("Map"), _T("Path"), nullptr);
	if(strMapPath.IsEmpty()==FALSE)
	{
		if(_taccess(strMapPath, 00)==-1)
		{
			CString string;
			string.LoadString(IDS_DIRECTORYNOTFOUND);
			string.Replace(_T("%s"), strMapPath);

			AfxMessageBox(string);
			cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
			cmdInfo.m_strFileName.Empty();
			return FALSE;
		}

		SetCurrentDirectory(strMapPath);

		//		if(m_ClientSocket.m_bLogin == false)
		{
			strAtlFile = strProject+_T(".Atl");
			if(CMainFrame::m_pAtlas==nullptr)
			{
				CMainFrame::m_pAtlas = new CAtlas(nullptr);
			}
		}
		//	else
		{
			//		CMainFrame::m_Security.SetTracarRight(false);
		}
	}
	else if(_taccess(_T("Diwatu.Atl"), 00)!=-1)
	{
		strAtlFile = _T("Diwatu.Atl");
		if(CMainFrame::m_pAtlas==nullptr)
		{
			CMainFrame::m_pAtlas = new CAtlas(nullptr);
		}
	}

	if(CMainFrame::m_pAtlas!=nullptr)
	{
		if(CMainFrame::m_pAtlas->Open(strAtlFile, CDatabase::openReadOnly)==true)
		{
			CMainFrame::m_Security.SetFreeOpenRight(CMainFrame::m_pAtlas->m_bFree);
			CMainFrame::m_Security.SetEditRight(CMainFrame::m_pAtlas->m_bEdit);
			CMainFrame::m_Security.SetRegionQueryRight(CMainFrame::m_pAtlas->m_bRegionQuery);
			CMainFrame::m_Security.SetNavigateRight(CMainFrame::m_pAtlas->m_bNavigate);
			CMainFrame::m_Security.SetHtmlRight(CMainFrame::m_pAtlas->m_bHtml);
			CMainFrame::m_Security.SetTracarRight(false);
			const CString strPort = AfxGetApp()->GetProfileString(_T("Navigate"), _T("Port"), _T("GPS Receiver"));
			if(strPort.CompareNoCase(_T("Socket"))==0)
			{
				CMainFrame::m_Security.SetBroadcastRight(false);
				CMainFrame::m_Security.SetTracarRight(true);
				CMainFrame::m_Security.SetHtmlRight(false);
				CMainFrame::m_Security.SetTrafficRight(false);
			}
		}
		else
		{
			delete CMainFrame::m_pAtlas;
			CMainFrame::m_pAtlas = nullptr;

			AfxMessageBox(IDS_ATLASFILENOTEXSIT);
			return FALSE;
		}
		const CString strFile = CMainFrame::m_pAtlas->m_strHomePage;
		if(IsWebFile(strFile)==TRUE)
		{
			cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
			cmdInfo.m_strFileName.Empty();
		}
		else if(_taccess(strFile, 00)!=-1)
		{
			cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;
			cmdInfo.m_strFileName = strFile;
		}
		else if(strFile.IsEmpty()==FALSE)
		{
			CString string;
			string.LoadString(IDS_FILENOTFOUND);
			string.Replace(_T("%s"), strFile);
			AfxMessageBox(string);

			cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
			cmdInfo.m_strFileName.Empty();
		}
	}
	else
	{
		strProject.Empty();
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	}

	InstallActiveX();

	CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);

	ASSERT(m_pDocManager==nullptr);
	m_pDocManager = new GMultiDocManager();

	GMultiDocTemplate* pGrfDocTemplate = new GMultiDocTemplate(IDR_GRFTYPE, RUNTIME_CLASS(CGrfDoc), RUNTIME_CLASS(CChildFrame), GWRUNTIME_CLASS(CGrfView), nullptr);
	pGrfDocTemplate->SetContainerInfo(IDR_GRFTYPE_CNTR_IP);
	AddDocTemplate(pGrfDocTemplate);
	if(pGrfDocTemplate->m_hMenuShared!=nullptr)
	{
		::DestroyMenu(pGrfDocTemplate->m_hMenuShared);
		pGrfDocTemplate->m_hMenuShared = nullptr;
	}
	GMultiDocTemplate* pGisDocTemplate = new GMultiDocTemplate(IDR_GISTYPE, RUNTIME_CLASS(CGisDoc), RUNTIME_CLASS(CChildFrame), GWRUNTIME_CLASS(CGisView), nullptr);
	pGisDocTemplate->SetContainerInfo(IDR_GISTYPE_CNTR_IP);
	AddDocTemplate(pGisDocTemplate);
	if(pGisDocTemplate->m_hMenuShared!=nullptr)
	{
		::DestroyMenu(pGisDocTemplate->m_hMenuShared);
		pGisDocTemplate->m_hMenuShared = nullptr;
	}

	CMainFrame* pMainFrame = new CMainFrame;
	if(pMainFrame->LoadFrame(IDR_MAINFRAME)==FALSE)
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	if(CMainFrame::m_pAtlas!=nullptr)
	{
		CString strHomePage = CMainFrame::m_pAtlas->m_strHomePage;
		if(IsWebFile(strHomePage)==TRUE)
		{
			pMainFrame->SendMessage(WM_BROWSEHTML, (UINT)&strHomePage, 0);
		}
	}

	ParseCommandLine(cmdInfo);
	if(ProcessShellCommand(cmdInfo)==FALSE)
		return FALSE;

	if(m_pMainWnd!=nullptr)
	{
		m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
		m_pMainWnd->UpdateWindow();
	}

	//	if(m_ClientSocket.m_bLogin == true)
	{
		//	m_pMainWnd->SetTimer(TIMER_PULSE,10000,nullptr);
	}

	return TRUE;
}

void CViewerApp::OnAppAbout()
{
	const CString strPath = this->GetProfileString(_T("Map"), _T("Path"), nullptr);
	const CString strProject = AfxGetApp()->m_lpCmdLine;
	if(strProject.IsEmpty()==FALSE)
	{
		CString strFile = strPath+strProject+_T(".Hlp");
		if(_taccess(strFile, 00)!=-1)
			::WinHelp(nullptr, strFile, HELP_CONTENTS, 1);
		else
		{
			strFile = strPath+strProject+_T(".Htm");
			if(_taccess(strFile, 00)!=-1)
			{
				const HWND hWndDesktop = ::GetDesktopWindow();
				::ShellExecute(hWndDesktop, _T("open"), strFile, nullptr, _T(""), SW_SHOW);
			}
		}
	}
	else
	{
		const CString strFile = _T("Diwatu.hlp");
		::WinHelp(nullptr, strFile, HELP_CONTENTS, 1);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CViewerApp commands

BOOL CViewerApp::InstallActiveX() const
{
	HKEY hKey;
	if(::RegOpenKey(HKEY_CURRENT_USER, _T("Software\\ODBC\\ODBC.INI"), &hKey)!=ERROR_SUCCESS)
	{
		AfxMessageBox(IDS_NOODBC);
		const CString strDefault = (CString)GetExeDirectory()+_T("Assistant\\");
		const CString strPath = AfxGetProfileString(HKEY_LOCAL_MACHINE, _T("Diwatu"), _T(""), _T("Assistant"), strDefault)+_T("mdac_typ.exe");
		const CStringA strPathname2(strPath);
		if(WinExec(strPathname2, SW_SHOW)>31)
		{
			const HWND hWnd = FindWindow(nullptr, _T("Microsoft Data Access Pack"));
			::SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
			::SetFocus(hWnd);
			::BringWindowToTop(hWnd);
		}
	}
	RegCloseKey(hKey);

	if(::RegOpenKey(HKEY_CURRENT_USER, _T("Software\\ODBC\\ODBC.INI\\MS Access Database"), &hKey)!=ERROR_SUCCESS)
	{
		AfxMessageBox(IDS_NOODBCDNSACCESS);
		RegCloseKey(hKey);
		const RETCODE retcode = SQLConfigDataSource(nullptr, ODBC_ADD_DSN, _T("Microsoft Access Driver (*.mdb)"), _T("DSN=MS Access Database"));
		return retcode;
	}
	RegCloseKey(hKey);

	return TRUE;
}

void CViewerApp::OnAppExit()
{
	if(AfxMessageBox(IDS_EXITCONFIRM, MB_YESNO)==IDYES)
	{
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
		CMFCVisualManager::DestroyInstance(); //this line is very important,get rid of the exit error on Windows XP
		AfxOleTerm(FALSE);

		CWinAppEx::OnAppExit();
	}
}

CString CViewerApp::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
{
	if(lpszSection!=nullptr)
	{
		CString strBase;
		strBase.Format(_T("SOFTWARE\\Diwatu\\%s\\Viewer\\"), m_lpCmdLine);

		HKEY hKey = nullptr;
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, strBase+lpszSection, 0, KEY_READ, &hKey)==ERROR_SUCCESS)
		{
			DWORD dwSize = 0;
			if(RegQueryValueEx(hKey, (LPTSTR)lpszEntry, nullptr, nullptr, nullptr, &dwSize)==ERROR_SUCCESS)
			{
				CString strValue;
				const LONG lRet = RegQueryValueEx(hKey, (LPTSTR)lpszEntry, nullptr, nullptr, (LPBYTE)strValue.GetBuffer(dwSize/sizeof(TCHAR)), &dwSize);
				strValue.ReleaseBuffer();

				if(lRet==ERROR_SUCCESS)
				{
					RegCloseKey(hKey);
					return strValue;
				}
			}

			RegCloseKey(hKey);
		}
		else
		{
			return lpszDefault;
		}
	}

	return CWinAppEx::GetProfileString(lpszSection, lpszEntry, lpszDefault);
}

BOOL CViewerApp::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
{
	if(lpszSection!=nullptr)
	{
		CString strBase;
		strBase.Format(_T("SOFTWARE\\Diwatu\\%s\\Viewer\\"), m_lpCmdLine);

		HKEY hKey = nullptr;
		if(RegCreateKey(HKEY_LOCAL_MACHINE, strBase+lpszSection, &hKey)==ERROR_SUCCESS)
		{
			if(RegSetValueEx(hKey, lpszEntry, 0, REG_SZ, (LPBYTE)lpszValue, _tcslen(lpszValue))==ERROR_SUCCESS)
			{
				RegCloseKey(hKey);
				return TRUE;
			}
			else
			{
				RegCloseKey(hKey);
				return FALSE;
			}
		}
		else
			return FALSE;
	}

	return CWinAppEx::WriteProfileString(lpszSection, lpszEntry, lpszValue);
}

UINT CViewerApp::GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
{
	if(lpszSection!=nullptr)
	{
		CString strBase;
		strBase.Format(_T("SOFTWARE\\Diwatu\\%s\\Viewer\\"), m_lpCmdLine);

		HKEY hKey = nullptr;
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, strBase+lpszSection, 0, KEY_READ, &hKey)==ERROR_SUCCESS)
		{
			DWORD dwSize = 0;
			if(RegQueryValueEx(hKey, (LPTSTR)lpszEntry, nullptr, nullptr, nullptr, &dwSize)==ERROR_SUCCESS)
			{
				DWORD dwValue;
				DWORD dwType;
				DWORD dwCount = sizeof(DWORD);
				const LONG lResult = RegQueryValueEx(hKey, (LPTSTR)lpszEntry, nullptr, &dwType, (LPBYTE)&dwValue, &dwCount);

				if(lResult==ERROR_SUCCESS)
				{
					RegCloseKey(hKey);
					return dwValue;
				}
			}

			RegCloseKey(hKey);
		}
		else
		{
			return nDefault;
		}
	}

	return CWinAppEx::GetProfileInt(lpszSection, lpszEntry, nDefault);
}

/*
LRESULT CALLBACK CViewerApp::ReceiveFromSocket(CClientSocket* pSocket,CArchive* pArIn,CArchive* pArOut)
{
	try
	{
		do
		{
			WORD wCmdType;
			WORD wVerify;

			*pArIn >> wCmdType;
			*pArIn >> wVerify;

			if((WORD)(wCmdType ^ wVerify) != 0XFFFF)
				continue;

			switch(wCmdType)
			{
			case SERVER_LOGIN_SUCCEED:
				{
					pSocket->m_bLogin = true;

					float fVer = 0.0f;
					*pArIn >> fVer;
					DWORD dwSecurity;
					*pArIn >> dwSecurity;

					float fCurentVer = AfxGetSoftVersion();
					if(fCurentVer < fVer)
					{
						AfxMessageBox(IDS_OLDVERSION);
						pSocket->m_bLogin = false;
						exit(0);
					}
					else
					{
						CMainFrame::m_Security.SetRights(dwSecurity);
						CMainFrame::m_Security.m_bViewLevel = (dwSecurity>>28);

						if(pSocket->m_hOverEvent != nullptr)
						{
							SetEvent(pSocket->m_hOverEvent);
						}
					}
				}
				break;
			case SERVER_LONGIN_FAILED:
				{
					pSocket->m_bLogin = false;

					DWORD dwError;
					*pArIn >> dwError;

					if(pSocket->m_hOverEvent != nullptr)
					{
						SetEvent(pSocket->m_hOverEvent);
					}

					pSocket->Close();

					switch(dwError)
					{
					case 0X0001:
						AfxMessageBox(_T("用户名不存在！"));
						break;
					case 0X0002:
						AfxMessageBox(IDS_WRONGPWD);
						break;
					case 0X0003:
						AfxMessageBox(_T("你的软件版本太低，请重新安装新版本！"));
						break;
					case 0X0004:
						AfxMessageBox(_T("已有同名的用户登录，\n请换一个用户名重新登录！"));
						break;
					}

					return TRUE;
				}
				break;
			case SERVER_DOWN:
				{
					AfxMessageBox(_T("服务器出现故障，请稍后再使用！"));
					pSocket->Close();
				}
				break;
			case SERVER_PULSE:
				{
					pSocket->m_bHeart = true;
				}
				break;
			case MAPSERVER_ATLAS:
				{
					if(CMainFrame::m_pAtlas == nullptr)
					{
						CMainFrame::m_pAtlas = new CAtlas();
					}


					if(CMainFrame::m_pAtlas != nullptr)
					{
						CString strOld = pArIn->m_strFileName;

						CString strPath = this->GetProfileString(_T("Map"),_T("Path"),nullptr);
						CString strProject = AfxGetApp()->m_lpCmdLine;
						CString strAtlas = strPath + strProject+_T(".Atl");
						pArIn->m_strFileName = strAtlas;

						CMainFrame::m_pAtlas->Socket(pArIn);

						if(CMainFrame::m_pAtlas->m_bDatabase == TRUE && CMainFrame::m_pAtlas->m_database.IsOpen() == FALSE)
						{
							if(CMainFrame::m_pAtlas->m_database.Open(CDatabase::noOdbcDialog) == FALSE)
							{
								CString string;
								string.Format(_T("工程数据库 ServerName: %s; DSN: %s; UID: %s; PWD: %s 无法打开,请核查!"),CMainFrame::m_pAtlas->m_database.m_strServer,CMainFrame::m_pAtlas->m_database.m_strDatabase,CMainFrame::m_pAtlas->m_database.m_strUID,CMainFrame::m_pAtlas->m_database.m_strPWD);
								AfxMessageBox(string);
							}
						}
						pArIn->m_strFileName = strOld;

						if(pSocket->m_hOverEvent != nullptr)
						{
							SetEvent(pSocket->m_hOverEvent);
						}
					}
				}
				break;
			case MAPSERVER_ATLASLOCATION:
				{
					CString strServer;
					CString strMapPath;
					CString strWebPath;

					*pArIn >> strServer;
					*pArIn >> strMapPath;
					*pArIn >> strWebPath;

					CWinAppEx::WriteProfileString(_T("Server"),_T("Path"),strServer);
					CWinAppEx::WriteProfileString(_T("Server"),_T("Map"),strMapPath);
					CWinAppEx::WriteProfileString(_T("Server"),_T("Web"),strWebPath);
				}
				break;
			case MAPSERVER_BALLOON:
				{
					CString strString;
					*pArIn >> strString;

					CBalloon*pBalloon = new CBalloon(150,100);
					pBalloon->SetText(strString);
					pBalloon->SetlifeTime(4);
					pBalloon->CreateAndShow();
				}
				break;
			case MAPSERVER_JPGIMAGE:
				{
					double dLng;
					double dLat;
					unsigned short nZoom;
					unsigned short nWidth;
					unsigned short nHeight;

					*pArIn >> dLng;
					*pArIn >> dLat;
					*pArIn >> nZoom;
					*pArIn >> nWidth;
					*pArIn >> nHeight;

					CPageItem* pMatchedItem = CMainFrame::m_pAtlas->MatchMap(nullptr,CPointF(dLng,dLat));
					if(pMatchedItem != nullptr)
					{
						HANDLE hDIB = pMatchedItem->GetIamge(dLng,dLat,nZoom,nWidth,nHeight);
						if(hDIB != nullptr)
						{
							LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)hDIB;

							int nColors = 1 << lpbi->biBitCount;
							if(lpbi->biBitCount > 8)
								nColors = 0;

							BITMAPFILEHEADER hdr;
							hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
							hdr.bfSize		= GlobalSize(hDIB) + sizeof(hdr);
							hdr.bfReserved1 = 0;
							hdr.bfReserved2 = 0;
							hdr.bfOffBits	= (DWORD)(sizeof(hdr)+ lpbi->biSize+nColors*sizeof(RGBQUAD));

							*pArOut << MAPCLIENT_JPGIMAGE;
							*pArOut << (WORD)(MAPCLIENT_JPGIMAGE^0XFFFF);

							DWORD size1 = sizeof(hdr);
							*pArOut << size1;
							pArOut->Write(&hdr, sizeof(hdr));

							DWORD size2 = GlobalSize(hDIB);
							*pArOut << size2;
							pArOut->Write(&lpbi, GlobalSize(hDIB));
							pArOut->Flush();

							GlobalFree(hDIB);
						}
					}
				}
				break;
			default:
				break;
			}
		}while(pArIn->IsBufferEmpty() == FALSE);
	}
	catch(CFileException* e)
	{
		ex->Delete();
		pArIn->Abort();
	}

	return TRUE;
}
*/
BOOL CViewerApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if(CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinAppEx::PreTranslateMessage(pMsg);
}

int CViewerApp::ExitInstance()
{
	//	AfxGetApp()->WriteProfileString(_T("Navigate"), _T("Port"),_T("GPS Receiver"));
	//	AfxGetApp()->WriteProfileString(_T("VechileDB"),_T("DBMS"),_T(""));	
	//	AfxGetApp()->WriteProfileString(_T("VechileDB"),_T("DSN"),_T(""));	
	//	AfxGetApp()->WriteProfileString(_T("VechileDB"),_T("ServerName"),_T(""));	
	//	AfxGetApp()->WriteProfileString(_T("VechileDB"),_T("UID"),_T(""));	
	//	AfxGetApp()->WriteProfileString(_T("VechileDB"),_T("PWD"),_T(""));		

	//	if(m_ClientSocket.m_bLogin == true)
	{
		//		m_ClientSocket.Close();
	}

	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	return CWinAppEx::ExitInstance();
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum
	{
		IDD = IDD_ABOUTBOX
	};

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support

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

// CViewerApp customization load/save methods

void CViewerApp::PreLoadState()
{
}

void CViewerApp::LoadCustomState()
{
}

void CViewerApp::SaveCustomState()
{
}

// CViewerApp message handlers
