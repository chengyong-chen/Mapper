#include "stdafx.h"
#include "Background.h"
#include <math.h>

#include "../Projection/Geocentric.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HANDLE CBackground::m_eventGiscakeReady = nullptr;

HANDLE CBackground::m_eventMapLoaded = nullptr;

HANDLE CBackground::m_eventVarietyChanged = nullptr;

HANDLE CBackground::m_eventMapViewChanged = nullptr;

// CBackground
CBackground::CBackground(CViewMonitor& foreground)
	: m_ViewMonitor(foreground), m_geocentric("merc", "WGS84")
{
	m_hReadPipe = nullptr;
	m_hWritePipe = nullptr;

	funcCallBack = nullptr;
	//85.05112877980659
	//86.467559846708710
	m_mapOrigin = CPointF(-M_PI, -85.05112877980659*M_PI/180);
	m_mapCanvas = CSizeF(2*M_PI, 2*85.05112877980659*M_PI/180);

	m_eventGiscakeReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	m_eventMapLoaded = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	m_eventVarietyChanged = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	m_eventMapViewChanged = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	ResetEvent(m_eventGiscakeReady);
	ResetEvent(m_eventMapLoaded);
	ResetEvent(m_eventVarietyChanged);
	ResetEvent(m_eventMapViewChanged);

	m_bThreadAlive = false;

	m_geocentric.m_strUnit = "Meter";
	m_bufGeocentric = nullptr;
}

CBackground::~CBackground()
{
	const HANDLE handle = ::OpenProcess(PROCESS_ALL_ACCESS, TRUE, m_dwProcessId);
	if(handle!=nullptr)
	{
		DWORD dwExitCode = 0;
		::GetExitCodeProcess(handle, &dwExitCode);
		::TerminateProcess(handle, dwExitCode);
	}
	const HWND hWnd = GetWndFromThread(m_dwThreadId, m_dwProcessId);
	if(hWnd!=nullptr)
	{
		::SendMessage(hWnd, WM_CLOSE, 0, 0);
	}

	if(m_hWritePipe!=nullptr)
	{
		CloseHandle(m_hWritePipe);
		m_hWritePipe = nullptr;
	}

	if(m_hReadPipe!=nullptr)
	{
		CloseHandle(m_hReadPipe);
		m_hReadPipe = nullptr;
	}
	while(m_bThreadAlive==true)
	{
		Sleep(100);//give the thread time to exit				
	}
}

BOOL CBackground::Create(void* point, void (WINAPI* callback)(void* point, CStringA strCommand, CStringA strArgument), CRect rect)
{
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = nullptr;

	HANDLE hServerReadPipe = nullptr;
	HANDLE hClientWritePipe = nullptr;
	if(CreatePipe(&hServerReadPipe, &hClientWritePipe, nullptr, 0)==TRUE)
	{
		HANDLE hClientWritePipeDup = nullptr;
		if(DuplicateHandle(GetCurrentProcess(), hClientWritePipe, GetCurrentProcess(), &hClientWritePipeDup, 0, TRUE, DUPLICATE_SAME_ACCESS)==FALSE)
		{
			CloseHandle(hServerReadPipe);
			CloseHandle(hClientWritePipe);
			return FALSE;
		}
		else if(hClientWritePipeDup!=INVALID_HANDLE_VALUE)
		{
			CloseHandle(hClientWritePipe);
			hClientWritePipe = hClientWritePipeDup;
		}
	}

	HANDLE hClientReadPipe = nullptr;
	HANDLE hServerWritePipe = nullptr;
	if(CreatePipe(&hClientReadPipe, &hServerWritePipe, nullptr, 0)==TRUE)
	{
		HANDLE hClientReadPipeDup = nullptr;
		if(DuplicateHandle(GetCurrentProcess(), hClientReadPipe, GetCurrentProcess(), &hClientReadPipeDup, 0, TRUE, DUPLICATE_SAME_ACCESS)==FALSE)
		{
			CloseHandle(hClientReadPipe);
			CloseHandle(hServerWritePipe);

			CloseHandle(hServerReadPipe);
			CloseHandle(hClientWritePipe);
			return FALSE;
		}
		else if(hClientReadPipeDup!=INVALID_HANDLE_VALUE)
		{
			CloseHandle(hClientReadPipe);
			hClientReadPipe = hClientReadPipeDup;
		}
	}

	STARTUPINFOA startupinfo;
	PROCESS_INFORMATION procinfo;
	memset(&startupinfo, 0, sizeof(STARTUPINFO));
	startupinfo.cb = sizeof(STARTUPINFO);
	startupinfo.hStdInput = hClientReadPipe;
	startupinfo.hStdOutput = hClientWritePipe;
	startupinfo.hStdError = hClientWritePipe;
	startupinfo.dwX = rect.left;
	startupinfo.dwY = rect.top;
	startupinfo.dwXSize = rect.Width();
	startupinfo.dwYSize = rect.Height();
	startupinfo.dwFlags = STARTF_USESTDHANDLES|STARTF_USEPOSITION|STARTF_USESIZE;

	ResetEvent(m_eventGiscakeReady);
	CHAR buff[MAX_PATH];
	memset(buff, 0, MAX_PATH);
	::GetModuleFileNameA(nullptr, buff, sizeof buff);
	CStringA strPath = buff;
	strPath = strPath.Left(strPath.ReverseFind('\\'));
	CHAR szCommandLine[512];
	sprintf_s(szCommandLine, "%s\\Popunder.exe %s", strPath, "");
	if(::CreateProcessA(nullptr, szCommandLine, nullptr, nullptr, TRUE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &startupinfo, &procinfo)==TRUE)
	{
		WaitForInputIdle(procinfo.hProcess, INFINITE);
		m_dwThreadId = procinfo.dwThreadId;
		m_dwProcessId = procinfo.dwProcessId;

		CloseHandle(procinfo.hThread);

		m_hReadPipe = hServerReadPipe;
		m_hWritePipe = hServerWritePipe;

		CloseHandle(hClientWritePipe);
		CloseHandle(hClientReadPipe);

		m_pPoint = point;
		funcCallBack = callback;
		CWinThread* pWinThread = AfxBeginThread(CBackground::ReadFromPipe, (LPVOID)this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, nullptr);
		if(pWinThread==nullptr)
		{
			CloseHandle(hServerReadPipe);
			CloseHandle(hServerWritePipe);
			return FALSE;
		}
		else
		{
			pWinThread->ResumeThread();
		}
		return WaitForSingleObject(m_eventGiscakeReady, 0.5*60*1000)==WAIT_OBJECT_0 ? TRUE : FALSE;
	}
	else
	{
		if(procinfo.hProcess)
			CloseHandle(procinfo.hProcess);
		if(procinfo.hThread)
			CloseHandle(procinfo.hThread);

		CloseHandle(hServerReadPipe);
		CloseHandle(hClientWritePipe);
		CloseHandle(hClientReadPipe);
		CloseHandle(hServerWritePipe);
		return FALSE;
	}
}

BOOL CBackground::ShowWindow(int nCmdShow) const
{
	const HWND hWnd = GetWndFromThread(m_dwThreadId, m_dwProcessId);
	return hWnd!=nullptr ? ::ShowWindow(hWnd, nCmdShow) : FALSE;
}

BOOL CBackground::SetWindowPos(HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) const
{
	const HWND hWnd = GetWndFromThread(m_dwThreadId, m_dwProcessId);
	return hWnd!=nullptr ? ::SetWindowPos(hWnd, hWndInsertAfter, x, y, cx, cy, nFlags) : FALSE;
}

BOOL CBackground::OpenBack(HWND hWnd, CStringA strProvider, CStringA strVariety)
{
	ResetEvent(m_eventVarietyChanged);

	CStringA strMessage;
	strMessage.Format("OpenBackmap: %d %s %s|<->|", hWnd, strProvider, strVariety);
	if(this->SendMessageToPipe(strMessage)==TRUE)
	{
		if(WaitForSingleObject(m_eventVarietyChanged, 0.5*60*1000)==WAIT_OBJECT_0)
		{
			m_strProvider = strProvider;
			m_strVariety = strVariety;
			return TRUE;
		}
		else
			return FALSE;
	}
	else
		return FALSE;
}
BOOL CBackground::LevelTo(float fLevel, const CPointF& geoPoint) const
{
	ResetEvent(m_eventMapViewChanged);

	CStringA strMessage;
	strMessage.Format("LevelAtTo:%g %g %g|<->|", fLevel, geoPoint.x, geoPoint.y);
	return this->SendMessageToPipe(strMessage) == TRUE ? WaitForSingleObject(m_eventMapViewChanged, 0.5*60*1000) == WAIT_OBJECT_0 : FALSE;
}
BOOL CBackground::ScaleTo(float fScale, const CPointF& geoPoint) const
{
	ResetEvent(m_eventMapViewChanged);

	CStringA strMessage;
	strMessage.Format("ScaleAtTo:%g %g %g|<->|", fScale, geoPoint.x, geoPoint.y);
	return this->SendMessageToPipe(strMessage)==TRUE ? WaitForSingleObject(m_eventMapViewChanged, 0.5*60*1000)==WAIT_OBJECT_0 : FALSE;
}

BOOL CBackground::ZoomBy(const CPoint& cliPoint, float fRatio) const
{
	ResetEvent(m_eventMapViewChanged);

	CStringA strMessage;
	strMessage.Format("ZoomByAt:%g %d %d|<->|", fRatio, cliPoint.x, cliPoint.y);
	return this->SendMessageToPipe(strMessage)==TRUE ? WaitForSingleObject(m_eventMapViewChanged, 0.5*60*1000)==WAIT_OBJECT_0 : FALSE;
}
BOOL CBackground::MoveBy(int cx, int cy) const
{
	ResetEvent(m_eventMapViewChanged);

	CStringA strMessage;
	strMessage.Format("MoveBy:%d %d|<->|", cx, cy);
	return this->SendMessageToPipe(strMessage)==TRUE ? WaitForSingleObject(m_eventMapViewChanged, 0.5*60*1000)==WAIT_OBJECT_0 : FALSE;
}

BOOL CBackground::JumpTo(double geoX, double geoY) const
{
	ResetEvent(m_eventMapViewChanged);

	CStringA strMessage;
	strMessage.Format("JumpTo:%lf %lf|<->|", geoX, geoY);
	return this->SendMessageToPipe(strMessage)==TRUE ? WaitForSingleObject(m_eventMapViewChanged, 0.5*60*1000)==WAIT_OBJECT_0 : FALSE;
}

void CBackground::Alphait(int alpha) const
{
	CStringA strMessage;
	strMessage.Format("Alphait:%d|<->|", alpha);
	this->SendMessageToPipe(strMessage);
}

BOOL CBackground::SendMessageToPipe(CStringA strMessage) const
{
	if(m_hWritePipe==nullptr)
		return FALSE;

	OutputDebugStringA("Out: ");
	OutputDebugStringA(strMessage);
	OutputDebugStringA("\r\n");

	DWORD dwWritten;
	const int length = strMessage.GetLength();
	return WriteFile(m_hWritePipe, strMessage, length, &dwWritten, nullptr);
}

UINT CBackground::ReadFromPipe(LPVOID lpVoid)
{
	CBackground* pParent = (CBackground*)lpVoid;
	if(pParent==nullptr)
		return 0;

	pParent->m_bThreadAlive = true;
	CStringA strData;
	while(pParent->m_hReadPipe!=nullptr)
	{
		DWORD dwBytes;
		if(PeekNamedPipe(pParent->m_hReadPipe, nullptr, 0, 0, &dwBytes, 0)==FALSE)
			break;

		if(dwBytes>0)
		{
			DWORD dwRead;
			char chBuf[1024];
			if(ReadFile(pParent->m_hReadPipe, chBuf, 1024, &dwRead, nullptr)==FALSE)
				break;
			else if(dwRead<=0)
				break;
			else
			{
				strData += CStringA(chBuf, dwRead);
				while(strData.Find("FSCommond:")!=-1&&strData.Find("|<->|")!=-1)
				{
					const int nEnd = strData.Find("|<->|");
					CStringA message = strData.Left(nEnd);
					strData = strData.Mid(nEnd+5);

					message = message.Mid(10);
					OutputDebugStringA("In:  ");
					OutputDebugStringA(message);
					OutputDebugStringA("\r\n");
					const int pos = message.Find("|");
					CStringA strCommand = pos!=-1 ? message.Left(pos) : message;
					CStringA strArgument = pos!=-1 ? message.Mid(pos+1) : CStringA("");
					if(strCommand=="GiscakeReady")
					{
						SetEvent(m_eventGiscakeReady);
					}
					else if(strCommand=="MapLoaded")
					{
						SetEvent(m_eventMapLoaded);

						double a;
						double b;
						double f;
						double originX;
						double originY;
						double canvasCX;
						double canvasCY;
						sscanf_s(strArgument, "%lf;%lf;%lf;%lf;%lf;%lf;%lf", &a, &b, &f, &originX, &originY, &canvasCX, &canvasCY);
						pParent->m_mapOrigin.x = originX;
						pParent->m_mapOrigin.y = originY;
						pParent->m_mapCanvas.cx = canvasCX;
						pParent->m_mapCanvas.cy = canvasCY;

						f = (a-b)/a;
						pParent->m_geocentric.GCS.Costmize(a, f, 0, 0, 0, 0, 0, 0, 1);
						free(pParent->m_geocentric.GCS.m_Key);
						pParent->m_geocentric.GCS.m_Key = _strdup("WGS84");//!!
						pParent->m_geocentric.Reconfig();
					}
					else if(strCommand=="VarietyChanged")
					{
						SetEvent(m_eventVarietyChanged);
						Sleep(1000);
					}
					else if(strCommand=="ViewChanged")
					{
						SetEvent(m_eventMapViewChanged);
						if(pParent->funcCallBack!=nullptr)
						{
							pParent->funcCallBack(pParent->m_pPoint, strCommand, strArgument);
						}
					}
					else if(strCommand=="EverythingOK")
					{
						SetEvent(m_eventGiscakeReady);
						SetEvent(m_eventMapLoaded);
						SetEvent(m_eventVarietyChanged);
						SetEvent(m_eventMapViewChanged);
					}
				}
			}
		}
		else
			Sleep(1000);
	}

	pParent->m_bThreadAlive = false;
	return 1;
}
