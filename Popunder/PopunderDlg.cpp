// PopunderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PopunderDlg.h"

#include <cstring>
#include <Windows.h>
#include <WinInet.h>

#include <string>
//#include <format>
#include <boost/format.hpp>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPopunderDlg::CPopunderDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CPopunderDlg::IDD, pParent)
{
	m_hReadPipe = nullptr;
	m_hWritePipe = nullptr;
	m_dwOwner = 0;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPopunderDlg::OnDestroy()
{
	CDialog::OnDestroy();

	//	UninstallHook();

	if(m_hReadPipe!=nullptr)
	{
		CloseHandle(m_hReadPipe);
		Sleep(1000);//give the thread to exit
		m_hReadPipe = nullptr;
	}
	if(m_hWritePipe!=nullptr)
	{
		CloseHandle(m_hWritePipe);
		m_hWritePipe = nullptr;
	}
}


void CPopunderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPopunderDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CPopunderDlg, CDialog)
END_EVENTSINK_MAP()

BOOL CPopunderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
//	Sleep(10*1000);
	::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	                          
	SetIcon(m_hIcon, TRUE); // Set big icon
	SetIcon(m_hIcon, FALSE); // Set small icon

	const HANDLE hReadPipe = GetStdHandle(STD_INPUT_HANDLE);
	if(hReadPipe==INVALID_HANDLE_VALUE)
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}
	else
		m_hReadPipe = hReadPipe;

	const HANDLE hWritePipe = GetStdHandle(STD_OUTPUT_HANDLE);
	if(hWritePipe==INVALID_HANDLE_VALUE)
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}
	else
		m_hWritePipe = hWritePipe;

	CWinThread* pWinThread = AfxBeginThread(CPopunderDlg::ReadFromPipe, (LPVOID)(this), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, nullptr);
	if(pWinThread!=nullptr)
	{
		pWinThread->ResumeThread();
	}
	else
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}

	CRect rect;
	this->GetWindowRect(rect);
	m_ctrlEdge.CreateAsync(WS_VISIBLE|WS_CHILD, rect, this, 1,
		[this](){
			m_ctrlEdge.Navigate(L"https://www.diwatu.com/popunder.html", [this](){
				this->SendMessageToPipe("FSCommond:GiscakeReady|<->|");
				});
		}, [this](std::string cmd, std::string args){
								const boost::format fmt = boost::format("FSCommond:%s|%s |<->|")%cmd%args;
								const std::string message = fmt.str();
			this->SendMessageToPipe(message);
		});
	
	return TRUE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPopunderDlg::OnPaint()
{
	if(IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		const int cxIcon = GetSystemMetrics(SM_CXICON);
		const int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		const int x = (rect.Width()-cxIcon+1)/2;
		const int y = (rect.Height()-cyIcon+1)/2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPopunderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPopunderDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if(m_ctrlEdge.m_hWnd!=nullptr)
	{
		m_ctrlEdge.SetWindowPos(&wndTop, 0, 0, cx, cy, SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOOWNERZORDER);//this doesn't trigger a WM_SIZE event on m_ctrlEdge
		m_ctrlEdge.ResizeToClientArea();

	}
}
BOOL CPopunderDlg::PreTranslateMessage(MSG* pMsg)
{
	if(m_dwOwner!=0)
	{
		switch(pMsg->message)
		{
			case WM_MOUSEMOVE:
			{
				CPoint point = pMsg->pt;
				ScreenToClient(&point);
				const DWORD dwPoint = MAKELONG(point.x, point.y);
				::PostMessage((HWND)m_dwOwner, pMsg->message, pMsg->wParam, dwPoint);
				return TRUE;
			}
			break;
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_LBUTTONDBLCLK:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_MBUTTONDBLCLK:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_RBUTTONDBLCLK:
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
			case WM_XBUTTONDBLCLK:
			{
				CPoint point = pMsg->pt;
				ScreenToClient(&point);
				const DWORD dwPoint = MAKELONG(point.x, point.y);
				::PostMessage((HWND)m_dwOwner, pMsg->message, 0, dwPoint);
				return TRUE;
			}
			break;
			case WM_SETCURSOR:
				break;
			default:
				break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CPopunderDlg::SendMessageToPipe(std::string message) const
{
	if(m_hWritePipe==nullptr)
		return;

	DWORD dwWritten;
	const int length = message.length();
	WriteFile(m_hWritePipe, message.c_str(), length, &dwWritten, nullptr);
}

UINT CPopunderDlg::ReadFromPipe(LPVOID lParam)
{
	CPopunderDlg* pParent = (CPopunderDlg*)lParam;
	if(pParent==nullptr)
		return 0;

	CStringA strData;
	while(pParent->m_hReadPipe!=nullptr)
	{
		DWORD dwBytes;
		if(::PeekNamedPipe(pParent->m_hReadPipe, nullptr, 0, 0, &dwBytes, 0)==FALSE)
			break;

		if(dwBytes>0)
		{
			DWORD dwRead;
			char* chars = new char[dwBytes+1];
			memset(chars, 0X00, dwBytes+1);
			if(::ReadFile(pParent->m_hReadPipe, chars, dwBytes, &dwRead, nullptr)==TRUE)
			{
				strData += CStringA(chars);
				strData = strData.Trim();
			}
			delete [] chars;

			int pos = strData.Find("|<->|");
			while(pos!=-1)
			{
				CStringA cmd = strData.Left(pos);
				cmd = cmd.Trim();
				strData = strData.Mid(pos+5);
				CEdgeWebCtrl::SendCmdToJavascritp(pParent->m_ctrlEdge.m_hWnd, cmd, pParent->m_dwOwner);

				pos = strData.Find("|<->|");
			}
		}
		else
		{
			Sleep(1000);
		}
	}

	return 1;
}