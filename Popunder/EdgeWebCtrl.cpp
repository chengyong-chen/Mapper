#include "stdafx.h"

//#include "atlstr.h"

#include "EdgeWebCtrl.h"

#include "WebView2.h"

#include <sstream>
#include <future>
#include <Wininet.h>
#pragma comment(lib,"shlwapi.lib")
#include <regex>

#include "ShlObj.h"

#include <string>
#include "../../ThirdParty/tinyxml/tinyxml2/tinyxml2.h"

#include "../Utility/string.hpp"

tinyxml2::XMLDocument CEdgeWebCtrl::m_xml;
using namespace str;

#define CHECK_FAILURE_STRINGIFY(arg)         #arg
#define CHECK_FAILURE_FILE_LINE(file, line)  ([](HRESULT hr){ CheckFailure(hr, "Failure at " CHECK_FAILURE_STRINGIFY(file) "(" CHECK_FAILURE_STRINGIFY(line) ")"); })
#define CHECK_FAILURE                        CHECK_FAILURE_FILE_LINE(__FILE__, __LINE__)
#define CHECK_FAILURE_BOOL(value)            CHECK_FAILURE((value) ? S_OK : E_UNEXPECTED)

void ShowFailure(HRESULT hr, CString const& message)
{
	CString text;
	text.Format("%s (0x%08X)", (LPCTSTR)message, hr);
	::MessageBox(nullptr, static_cast<LPCTSTR>(text), "Failure", MB_OK);
}

void CheckFailure(HRESULT hr, CString const& message)
{
	if(FAILED(hr))
	{
		CString text;
		text.Format("%s : 0x%08X", (LPCTSTR)message, hr);

		std::exit(hr);
	}
}

IMPLEMENT_DYNCREATE(CEdgeWebCtrl, CWnd)

BEGIN_MESSAGE_MAP(CEdgeWebCtrl, CWnd)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CEdgeWebCtrl, CWnd)
END_EVENTSINK_MAP()

CEdgeWebCtrl::CEdgeWebCtrl()
{
	char buff[MAX_PATH];
	memset(buff, 0, MAX_PATH);
	::GetModuleFileNameA(nullptr, buff, sizeof(buff));
	CStringA strPath(buff);
	strPath = strPath.Left(strPath.ReverseFind('\\'));
	if(m_xml.LoadFile(strPath+"\\Assistant\\popunder.xml")==tinyxml2::XMLError::XML_SUCCESS)
	{
		std::string content = CEdgeWebCtrl::ReadURL("http://maps.google.com");
		if(content.empty() == false && false)//google html changed
		{
			tinyxml2::XMLElement* pElem = m_xml.FirstChildElement();
			if(pElem!=nullptr)
			{
				tinyxml2::XMLElement* pProvider = pElem->FirstChildElement("background")->FirstChildElement("provider");
				while(pProvider!=nullptr)
				{
					const char* provider = pProvider->Attribute("name");
					if(strcmp(provider, "Google")==0)
					{
						//var google1 = this.GetBackgroundProvider('Google-Road');
						//if(google1!=null){
						//	var regex = /\"*http:\/\/mt0.googleapis.com\/vt\?lyrs=m@(\d*)/;
						//		var result = regex.exec(html);
						//	if(result!=null)
						//		google1.version1 = result[1];
						//	else
						//		google1.version1 = '25000000';
						//}
						//var google2 = this.GetBackgroundProvider('Google-Satellite');
						//if(google2!=null){
						//	var regex = /\"*http:\/\/khm0.googleapis.com\/kh\?v=(\d*)/;
						//		var result = regex.exec(html);
						//	if(result!=null)
						//		google2.version1 = result[1];
						//	else
						//		google2.version1 = '123';
						//}
						//var google3 = this.GetBackgroundProvider('Google-Hybrid');
						//if(google3!=null){
						//	var regex = /\"*http:\/\/mt0.googleapis.com\/vt\?lyrs=h@(\d*)/;
						//		var result = regex.exec(html);
						//	if(result!=null)
						//		google3.version1 = result[1];
						//	else
						//		google3.version1 = '25000000';
						//}
						//var google4 = this.GetBackgroundProvider('Google-Terrian');
						//if(google4!=null){
						//	var regex = /\"*http:\/\/mt0.googleapis.com\/vt\?lyrs=t@(\d*),r@(\d*)/;
						//		var result = regex.exec(html);
						//	if(result!=null){
						//		google4.version1 = result[1];
						//		google4.version2 = result[2];
						//	}
						//	else{
						//		google4.version1 = '130';
						//		google4.version2 = '25000000';
						//	}
						//}						
						tinyxml2::XMLElement* pVariety = pProvider->FirstChildElement("provider");
						while(pVariety!=nullptr)
						{
							const char* name = pVariety->Attribute("name");
							if(strcmp(name, "Road")==0)
							{
								const std::regex regex1("\"*http://mt0.google.com/vt/lyrs=m@(\\d*)");
								const std::regex regex2("\"*https://mts0.google.com/vt/lyrs=m@(\\d*)");
								std::smatch  match;
								if(std::regex_search(content, match, regex1, regex_constants::format_default)||std::regex_search(content, match, regex2, regex_constants::format_default))
								{
									std::string version = match[1].str();
									const char* pOldPath = pVariety->Attribute("path");
									char* pNewPath =str::replace(pOldPath, "{version}", version.c_str());
									if(pNewPath!=nullptr)
									{
										pVariety->SetAttribute("path", pNewPath);
										free(pNewPath);
									}
								}
							}
							else if(strcmp(name, "Satellite")==0)
							{
								const std::regex regex1("\"*http://khm0.google.com/kh/v=(\\d*)");
								const std::regex regex2("\"*https://khms0.google.com/kh/v=(\\d*)");
								std::smatch  match;
								if(std::regex_search(content, match, regex1, regex_constants::format_default)||std::regex_search(content, match, regex2, regex_constants::format_default))
								{
									std::string version = match[1].str();
									const char* pOldPath = pVariety->Attribute("path");
									char* pNewPath = str::replace(pOldPath, "{version}", version.c_str());
									if(pNewPath!=nullptr)
									{
										pVariety->SetAttribute("path", pNewPath);
										free(pNewPath);
									}
								}
							}
							else if(strcmp(name, "Hybrid")==0)
							{
								const std::regex regex1("\"*http://mt0.google.com/vt/lyrs=h@(\\d*)");
								const std::regex regex2("\"*https://mts0.google.com/vt/lyrs=h@(\\d*)");
								std::smatch  match;
								if(std::regex_search(content, match, regex1, regex_constants::format_default)||std::regex_search(content, match, regex2, regex_constants::format_default))
								{
									std::string version = match[1].str();
									const char* pOldPath = pVariety->Attribute("path");
									char* pNewPath = str::replace(pOldPath, "{version}", version.c_str());
									if(pNewPath!=nullptr)
									{
										pVariety->SetAttribute("path", pNewPath);
										free(pNewPath);
									}
								}
							}
							else if(strcmp(name, "Terrian")==0)
							{
								const std::regex regex1("\"*http://mt0.google.com/vt/lyrs=t@(\\d*),r@(\\d*)");
								const std::regex regex2("\"*https://mts0.google.com/vt/lyrs=t@(\\d*),r@(\\d*)");
								std::smatch  match;
								if(std::regex_search(content, match, regex1, regex_constants::format_default)||std::regex_search(content, match, regex2, regex_constants::format_default))
								{
									std::string version1 = match[1].str();
									std::string version2 = match[2].str();
									const char* pOldPath = pVariety->Attribute("path");
									char* pNewPath1 = str::replace(pOldPath, "{version1}", version1.c_str());
									char* pNewPath2 = str::replace(pNewPath1, "{version2}", version2.c_str());
									if(pNewPath2!=nullptr)
									{
										pVariety->SetAttribute("path", pNewPath2);
										free(pNewPath2);
									}
									if(pNewPath1!=nullptr)
									{
										free(pNewPath1);
									}
								}
							}
							pVariety = pVariety->NextSiblingElement();
						}
						m_xml.SaveFile(strPath+"\\popunder.xml");
						break;
					}
					pProvider = pProvider->NextSiblingElement();
				}
			}

		}
	}
}

CEdgeWebCtrl::~CEdgeWebCtrl()
{
	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, 0);
	if(m_webView)
	{
		m_webView->remove_NavigationCompleted(navigationCompletedToken);
		m_webView->remove_DocumentTitleChanged(messsageReceivedToken);
		m_webView->remove_DocumentTitleChanged(m_processFailedToken);
		m_webView = nullptr;
	}
	if(m_webController)
	{
		m_webController->Close();
		m_webController = nullptr;
	}
	m_webSettings = nullptr;
	m_webViewEnvironment = nullptr;
}

void CEdgeWebCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if(m_webController!=nullptr)
	{
		const RECT bounds(0L, 0L, cx, cy);
		m_webController->put_Bounds(bounds);
	}
}
BOOL CEdgeWebCtrl::CreateAsync(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nId, std::function<void()> created, std::function<void(std::string, std::string)> messageReceived)
{
	const LPCTSTR	lpszClassName = GetWindowClass();
	if(!CWnd::Create(lpszClassName, nullptr, dwStyle, rect, pParentWnd, nId))
		return FALSE;

	::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

	m_cbCreationCompleted = created;
	m_cbMessageReceived = messageReceived;

	const HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[this](HRESULT result, ICoreWebView2Environment* env) -> HRESULT{
				CHECK_FAILURE(result);
				CHECK_FAILURE(env->QueryInterface(IID_PPV_ARGS(&m_webViewEnvironment)));

				CHECK_FAILURE(m_webViewEnvironment->CreateCoreWebView2Controller(m_hWnd,
					Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
						[this](HRESULT result, ICoreWebView2Controller* controller)->HRESULT{
							if(result==S_OK)
							{
								if(controller!=nullptr)
								{
									m_webController = controller;
									CHECK_FAILURE(controller->get_CoreWebView2(&m_webView));
								}

								CHECK_FAILURE(m_webView->get_Settings(&m_webSettings));
								m_webSettings->put_AreDefaultScriptDialogsEnabled(FALSE);
								m_webSettings->put_IsScriptEnabled(TRUE);
								m_webSettings->put_IsWebMessageEnabled(TRUE);
								this->RegisterEventHandlers();
								this->ResizeToClientArea();
						//		m_webView->OpenDevToolsWindow();

								if(m_cbCreationCompleted!=nullptr)
								{
									RunAsync(this->m_cbCreationCompleted);
								}
							}
							else
							{
								ShowFailure(result, "Cannot create webview environment.");
							}
							return S_OK;
						}).Get()));
				return S_OK;
			}).Get());

	if(!SUCCEEDED(hr))
	{
		const CString text = hr==HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) ? "Cannot find the Edge browser." : "Cannot create the webview environment.";
		ShowFailure(hr, text);
	}
	return TRUE;
}

void CEdgeWebCtrl::RegisterEventHandlers()
{
	CHECK_FAILURE(m_webView->add_NavigationCompleted(
		Callback<ICoreWebView2NavigationCompletedEventHandler>(
			[this](ICoreWebView2*, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
				BOOL success;
				CHECK_FAILURE(args->get_IsSuccess(&success));
				if(!success)
				{
					COREWEBVIEW2_WEB_ERROR_STATUS webErrorStatus{};
					CHECK_FAILURE(args->get_WebErrorStatus(&webErrorStatus));
					if(webErrorStatus==COREWEBVIEW2_WEB_ERROR_STATUS_DISCONNECTED)
					{
						// Do something here if you want to handle a specific error case.
						// In most cases this isn't necessary, because the WebView will
						// display its own error page automatically.
					}
				}
				wil::unique_cotaskmem_string uri;
				m_webView->get_Source(&uri);
				if(wcscmp(uri.get(), L"about:blank")==0)
				{
					uri = wil::make_cotaskmem_string(L"");
				}
				if(m_cbNavigationCompleted!=nullptr)
				{
					RunAsync(this->m_cbNavigationCompleted);
				}
				return S_OK;
			}).Get(), &navigationCompletedToken));

	CHECK_FAILURE(m_webView->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>(
		[this](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
			PWSTR message;
			args->TryGetWebMessageAsString(&message);
			if(m_cbMessageReceived!=nullptr)
			{
				char chars[512];
				const char defChar = ' ';
				WideCharToMultiByte(CP_ACP, 0, message, -1, chars, 512, &defChar, nullptr);
				const std::string  strMessage(chars);
				const int pos = strMessage.find_first_of(' ');
				if(pos>1)
				{
					const std::string cmd = strMessage.substr(0, pos);
					const std::string arg = strMessage.substr(pos+1);
					std::async(std::launch::async, [this, cmd, arg]					{
						this->m_cbMessageReceived(cmd, arg);
						});
				}
			}
			CoTaskMemFree(message);
			return S_OK;
		}).Get(), &messsageReceivedToken));


	CHECK_FAILURE(m_webView->add_ProcessFailed(Callback<ICoreWebView2ProcessFailedEventHandler>(
		[this](ICoreWebView2* sender, ICoreWebView2ProcessFailedEventArgs* args) -> HRESULT			{
			COREWEBVIEW2_PROCESS_FAILED_KIND failureType;
			CHECK_FAILURE(args->get_ProcessFailedKind(&failureType));
			if(failureType==COREWEBVIEW2_PROCESS_FAILED_KIND_BROWSER_PROCESS_EXITED)
			{

			}
			else if(failureType==COREWEBVIEW2_PROCESS_FAILED_KIND_RENDER_PROCESS_UNRESPONSIVE)
			{

			}
			else if(failureType==COREWEBVIEW2_PROCESS_FAILED_KIND_RENDER_PROCESS_EXITED)
			{

			}
			return S_OK;
		}).Get(), &m_processFailedToken));
}

void CEdgeWebCtrl::ResizeToClientArea()
{
	if(m_webController)
	{
		RECT bounds;
		GetClientRect(&bounds);
		m_webController->put_Bounds(bounds);
	}
}

void CEdgeWebCtrl::Navigate(CStringW url, std::function<void()> complete)
{
	if(m_webView)
	{
		m_cbNavigationCompleted = complete;
		if(url.Find(L"://")<0)
		{
			if(url.GetLength()>1&&url[1]==L':')
				url = L"file://"+url;
			else
				url = L"http://"+url;
		}
		m_webView->Navigate(url);
	}
}

PCTSTR CEdgeWebCtrl::GetWindowClass()
{
	static PCTSTR windowClass = []
	{
		static TCHAR const* className = "EdgeBrowserHost";
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.lpfnWndProc = WndProcStatic;
		wcex.style = CS_HREDRAW|CS_VREDRAW;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = AfxGetInstanceHandle();
		wcex.hIcon = nullptr;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = className;
		wcex.hIconSm = nullptr;
		const ATOM result = RegisterClassEx(&wcex);
		if(result==0)
		{
			[[maybe_unused]] DWORD lastError = ::GetLastError();
		}

		return className;
	}();

	return windowClass;
}

void CEdgeWebCtrl::SendCmdToJavascritp(HWND hWnd, CStringA cmd, DWORD& dwOwner)
{
	char* script = (char*)malloc(512);
	if(cmd.Find("ScaleAtTo:")!=-1)
	{
		cmd = cmd.Mid(10);
		float scale;
		double lng;
		double lat;
		sscanf_s(cmd, "%f %lf %lf", &scale, &lng, &lat);
		sprintf_s(script, 512, "adapter.ScaleAtTo(%f, %f, %f);", scale, lng, lat);
	}
	else if(cmd.Find("OpenBackmap:")!=-1)
	{
		cmd = cmd.Mid(12);
		char strProvider[256];
		char strVariety[256];
		sscanf_s(cmd, "%d %s %s", &dwOwner, strProvider, 255, strVariety, 255);
		tinyxml2::XMLElement* pElem = CEdgeWebCtrl::m_xml.FirstChildElement();
		if(pElem!=nullptr)
		{
			tinyxml2::XMLElement* pProvider = pElem->FirstChildElement("background")->FirstChildElement("provider");
			while(pProvider!=nullptr)
			{
				const char* provider = pProvider->Attribute("name");
				if(strcmp(provider, strProvider)==0)
				{
					const char* minb = pProvider->Attribute("minBalancer", nullptr);
					const char* maxb = pProvider->Attribute("maxBalancer", nullptr);
					tinyxml2::XMLElement* pVariety = pProvider->FirstChildElement("varieties")->FirstChildElement("variety");
					while(pVariety!=nullptr)
					{
						const char* variety = pVariety->Attribute("name");
						if(strcmp(variety, strVariety)==0)
						{
							const char* server = pVariety->Attribute("server");
							const char* path = pVariety->Attribute("path");
							const int minl = pProvider->IntAttribute("minLevel", 0);
							const int maxl = pProvider->IntAttribute("maxLevel", 20);
							sprintf_s(script, 512, "window.adapter.OnoffBackground('%s','%s', '%s', '%s', %d, %d, '%c', '%c');", strProvider, strVariety, server, path, minl, maxl, (minb!=nullptr ? minb[0] : ' '), (maxb!=nullptr ? maxb[0] : ' '));
							break;
						}
						pVariety = pVariety->NextSiblingElement();
					}
					break;
				}
				pProvider = pProvider->NextSiblingElement();
			}
		}
	}
	else if(cmd.Find("ZoomByAt:")!=-1)
	{
		cmd = cmd.Mid(9);
		float ratio;
		int x;
		int y;
		sscanf_s(cmd, "%g %d %d", &ratio, &x, &y);
		const float delta = log(ratio)/log(2.0);
		sprintf_s(script, 512, "adapter.LevelByAt(%f, %d, %d);", delta, x, y);
	}
	else if(cmd.Find("ZoomAtTo:")!=-1)
	{
		cmd = cmd.Mid(9);
		float ratio;
		int x;
		int y;
		sscanf_s(cmd, "%g %d %d", &ratio, &x, &y);
		const float delta = log(ratio)/log(2.0);
		sprintf_s(script, 512, "adapter.LevelByTo(%f, %d, %d);", delta, x, y);
	}
	else if(cmd.Find("MoveBy:")!=-1)
	{
		cmd = cmd.Mid(7);
		int cx;
		int cy;
		sscanf_s(cmd, "%d %d", &cx, &cy);
		sprintf_s(script, 512, "adapter.MoveBy(%d, %d);", cx, cy);
	}
	else if(cmd.Find("JumpTo:")!=-1)
	{
		cmd = cmd.Mid(7);
		double x;
		double y;
		sscanf_s(cmd, "%lf %lf", &x, &y);
		sprintf_s(script, 512, "adapter.JumpTo(%f, %f);", x, y);
	}
	else if(cmd.Find("Alphait:")!=-1)
	{
		cmd = cmd.Mid(8);
		float alpha;
		sscanf_s(cmd, "%f", &alpha);
		sprintf_s(script, 512, "adapter.AlphaThis(%f);", alpha);
	}
	else
	{
		free(script);
		return;
	}
	::SendMessage(hWnd, MSG_RUN_ASYNC_CALLBACK2, (UINT)script, 0);//!! ExecuteScript has to be in the thread that created
}

void CEdgeWebCtrl::RunAsync(std::function<void()> function)
{
	auto* task = new std::function<void()>(function);
	PostMessage(MSG_RUN_ASYNC_CALLBACK1, reinterpret_cast<WPARAM>(task), 0);
}
LRESULT CALLBACK CEdgeWebCtrl::WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(const auto pEdgeWebCtrl = (CEdgeWebCtrl*)::GetWindowLongPtr(hWnd, GWLP_USERDATA))
	{
		switch(message)
		{
			case WM_SIZE:
			{
				if(lParam!=0)
				{
					pEdgeWebCtrl->ResizeToClientArea();
					return true;
				}
			}
			break;
			case MSG_RUN_ASYNC_CALLBACK1:
			{
				const auto* task = reinterpret_cast<std::function<void()>*>(wParam);
				(*task)();
				delete task;
				return true;
			}
			case MSG_RUN_ASYNC_CALLBACK2:
			{
				const CStringW script = CStringW((char*)wParam);
				const HRESULT hr = pEdgeWebCtrl->m_webView->ExecuteScript(CStringW(script),
					Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
						[](HRESULT hr, PCWSTR result) -> HRESULT{
							return S_OK;
						}).Get());
				CHECK_FAILURE(hr);
				free((char*)wParam);
				return true;
			}
		}
		return false;
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}
std::string CEdgeWebCtrl::ReadURL(CStringA strURL)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	std::string content;
	const HINTERNET hInternet = InternetOpenA("AutoUpdateAgent", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
	if(hInternet != nullptr)
	{
		DWORD dwType;
		if(InternetGetConnectedState(&dwType, 0) == TRUE)
		{
			char canonicalURL[1024];
			DWORD nSize = 1024;//
			InternetCanonicalizeUrlA(strURL, canonicalURL, &nSize, ICU_BROWSER_MODE);
			const HINTERNET hSession = InternetOpenUrlA(hInternet, canonicalURL, nullptr, 0, SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_WEAK_SIGNATURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_NO_UI | INTERNET_FLAG_NEED_FILE | INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_RESYNCHRONIZE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_RAW_DATA | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_DONT_CACHE, 0);
			if(hSession != nullptr)
			{
				char pBuf[4097];
				memset(pBuf, 0, 4096);
				DWORD dwRead;
				while(InternetReadFile(hSession, pBuf, 4096, &dwRead) == TRUE && dwRead > 0)
				{
					pBuf[dwRead] = 0;
					content += std::string(pBuf);
				}
				InternetCloseHandle(hSession);
			}
			else
			{
				const HINTERNET hConnect = InternetConnectA(hInternet, "https://www.diwatu.com", 443, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
				if(hConnect != nullptr)
				{
					const HINTERNET hRequest = HttpOpenRequestA(hConnect, "GET", "popunder.html", "HTTP/1.1", nullptr, nullptr, INTERNET_FLAG_SECURE, 0);
					if(hRequest != nullptr)
					{
						DWORD dwFlags;
						DWORD dwBuffLen = sizeof(dwFlags);
						if(InternetQueryOption(hRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, &dwBuffLen))
						{
							dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
							InternetSetOption(hRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));
						}
						if(HttpSendRequest(hRequest, nullptr, 0, nullptr, 0))
						{
							char pBuf[4097];
							memset(pBuf, 0, 4096);
							DWORD dwRead;
							while(InternetReadFile(hRequest, pBuf, 4096, &dwRead) == TRUE && dwRead > 0)
							{
								pBuf[dwRead] = 0;
								content += std::string(pBuf);
							}
						}
						InternetCloseHandle(hRequest);
					}
					InternetCloseHandle(hConnect);
				}
			}
		}
		InternetCloseHandle(hInternet);
	}
	return content;
}
