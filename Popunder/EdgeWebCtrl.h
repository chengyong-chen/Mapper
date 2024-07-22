#pragma once

#include <EventToken.h>
#include <functional>
#include <WebView2.h>
#include <wrl.h>

#include "../../Thirdparty/tinyxml/tinyxml2/tinyxml2.h"
#include "wil/com.h"

#include <string>
using namespace Microsoft::WRL;
using namespace std;

static constexpr UINT MSG_RUN_ASYNC_CALLBACK1 = WM_APP + 124;
static constexpr UINT MSG_RUN_ASYNC_CALLBACK2 = WM_APP + 125;

struct ICoreWebView2Environment;
struct ICoreWebView2Controller;

class CEdgeWebCtrl : public CWnd
{
protected:
	DECLARE_DYNCREATE(CEdgeWebCtrl)

public:
	CEdgeWebCtrl();
	~CEdgeWebCtrl() override;

	BOOL CreateAsync(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nId, std::function<void()> created, std::function<void(std::string, std::string)> messageReceived);

	void Navigate(CStringW  url, std::function<void()> onComplete);
	void ResizeToClientArea();
		
private:
	wil::com_ptr<ICoreWebView2Environment> m_webViewEnvironment = nullptr;
	wil::com_ptr<ICoreWebView2> m_webView = nullptr;
	wil::com_ptr<ICoreWebView2Controller> m_webController = nullptr;
	wil::com_ptr<ICoreWebView2Settings> m_webSettings = nullptr;

	std::function<void()> m_cbCreationCompleted;
	std::function<void()> m_cbNavigationCompleted;
	std::function<void(std::string commond, std::string args)> m_cbMessageReceived;

	EventRegistrationToken navigationCompletedToken = {};
	EventRegistrationToken messsageReceivedToken = {};
	EventRegistrationToken m_processFailedToken = {};
	void RunAsync(std::function<void()> function);
	void RegisterEventHandlers();

protected:
	//{{AFX_MSG(CEdgeWebCtrl)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_EVENTSINK_MAP()
		//}}AFX_MSG			
		DECLARE_MESSAGE_MAP()
		static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static PCTSTR GetWindowClass();

public:
	static void SendCmdToJavascritp(HWND hWnd, CStringA cmd, DWORD& dwOwner);
	static tinyxml2::XMLDocument m_xml;
	static std::string ReadURL(CStringA strURL);
};