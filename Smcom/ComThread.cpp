// ComThread.cpp : implementation file
//

#include "stdafx.h"
#include "ComThread.h"

#include "../Trasvr/Global.h"
#include "../Coding/Message.h"
#include "../Socket/MessageQ.h"
#include "../Socket/ExStdCFunction.h"

const UINT WM_MAPEDITADDSTRING = WM_USER+4;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMessageQueue CComThread::g_recv_msg_queue;

CIDVehicleMap CComThread::m_mapIdVhc;
HWND CComThread::g_hMainWnd = nullptr;
HWND CComThread::g_MapEditMsgWnd = nullptr;

CSMSThread* CComThread::m_pSMSThread = nullptr;
CUDPThread* CComThread::m_pUDPThread = nullptr;
CTCPThread* CComThread::m_pTCPThread = nullptr;
CHTTPThread* CComThread::m_pHTTPThread = nullptr;

/////////////////////////////////////////////////////////////////////////////
// CComThread

IMPLEMENT_DYNCREATE(CComThread, CWinThread)

CComThread::CComThread()
{
	m_bValid = false;
	DealSendThreadEnded = nullptr;
}

CComThread::~CComThread()
{
	m_bValid = false;
}

BOOL CComThread::InitInstance()
{
	if(AfxBeginThread(DealSendMsgThread, (LPVOID)this)==nullptr)
	{
		AfxMessageBox(_T("发送消息线程启动失败！"));
		m_bValid = false;
		return FALSE;
	}
	else
	{
	}

	return TRUE;
}

int CComThread::ExitInstance()
{
	if(m_bValid==true)
	{
		m_bValid = false;

		if(DealSendThreadEnded!=nullptr)
		{
			WaitForSingleObject(DealSendThreadEnded, INFINITE);//等待DealSendMsgThread 结束
			CloseHandle(DealSendThreadEnded);
		}
	}

	g_send_msg_queue.Clear();

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CComThread, CWinThread)
	//{{AFX_MSG_MAP(CComThread)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComThread message handlers

void CComThread::PushRecvMsgToQueue(CMessageReceived* pMsg)
{
	try
	{
		if(pMsg==nullptr)
			return;

		if(g_recv_msg_queue.GetMsgCount()<=1000)
		{
			g_recv_msg_queue.PushMessage((void*)pMsg, sizeof(*pMsg));
		}
		else
		{
			delete pMsg;
			pMsg = nullptr;
		}
	}
	catch(CException*ex)
	{
		ex->Delete();
	}
}

void CComThread::PushSendMsgToQueue(CMessageWillSend* pMsg)
{
	if(pMsg==nullptr)
		return;

	try
	{
		g_send_msg_queue.PushMessage((void*)pMsg, sizeof(*pMsg));
	}
	catch(CException*ex)
	{
		ex->Delete();
	}
}

void CComThread::PutMsgToMessageWnd(LPCTSTR lpMsg)
{
	if(lpMsg==nullptr)
		return;

	if(IsWindow(g_MapEditMsgWnd))
	{
		CString* pStr = new CString(lpMsg);
		::PostMessage(g_MapEditMsgWnd, WM_MAPEDITADDSTRING, (WPARAM)pStr, 0);
	}
}

void CComThread::SetMapEditMsgWndHnd(HWND hWnd)
{
	if(IsWindow(hWnd))
	{
		g_MapEditMsgWnd = hWnd;
		SetMsgOutHandle(hWnd);
	}
	else
		g_MapEditMsgWnd = nullptr;
}

void CComThread::SetMainWndHnd(HWND hWnd)
{
	if(IsWindow(hWnd))
	{
		g_hMainWnd = hWnd;
	}
	else
		g_hMainWnd = nullptr;
}

SVehicle* CComThread::FindVehicleInfo(const DWORD& dwId)
{
	try
	{
		const CIDVehicleMap::iterator it = m_mapIdVhc.find(dwId);
		if(it==m_mapIdVhc.end())
		{
			SVehicle* pVhcInfo = (SVehicle*)::SendMessage(g_hMainWnd, WM_CUSTOM_GETVHCINFO_BYID, (LONG)dwId, 0);
			if(pVhcInfo!=nullptr)
			{
				m_mapIdVhc[pVhcInfo->m_dwId] = pVhcInfo;
				return pVhcInfo;
			}
			else
				return nullptr;
		}
		else
			return (SVehicle*)it->second;
	}
	catch(CException*ex)
	{
		ex->Delete();
		return nullptr;
	}
}

SVehicle* CComThread::FindVehicleInfo(LPCSTR lpIVUSIM, bool bUseGPRS)
{
	try
	{
		if(lpIVUSIM==nullptr||strlen(lpIVUSIM)<5)
			return nullptr;

		static CString strTel;
		strTel = lpIVUSIM;

		for(CIDVehicleMap::iterator it = m_mapIdVhc.begin(); it!=m_mapIdVhc.end(); it++)
		{
			SVehicle* pVhcInfo = (SVehicle*)it->second;
			if(bUseGPRS)
			{
				if(pVhcInfo->m_strIVUSN==lpIVUSIM)
					return pVhcInfo;
			}
			else
			{
				if(pVhcInfo->m_strIVUSIM==lpIVUSIM)
					return pVhcInfo;
			}
		}

		SVehicle* pVhcInfo = (SVehicle*)::SendMessage(g_hMainWnd, WM_CUSTOM_GETVHCINFO_BYTEL, (LONG)&strTel, (LONG)bUseGPRS);
		if(pVhcInfo!=nullptr)
		{
			m_mapIdVhc[pVhcInfo->m_dwId] = pVhcInfo;
			return pVhcInfo;
		}
	}
	catch(CException*ex)
	{
		ex->Delete();
	}

	return nullptr;
}

// 接收活动消息线程
UINT CComThread::DealRecvMsgThread(LPVOID lParam)
{
	//将来需要从Trasvr中移植过来
	return 0;
}

UINT CComThread::DealSendMsgThread(LPVOID lParam)
{
	CComThread* pComThread = (CComThread*)lParam;
	if(pComThread==nullptr)
		return 0;

	pComThread->DealSendThreadEnded = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	while(pComThread->m_bValid==true)
	{
		try
		{
			if(pComThread->g_send_msg_queue.WaitForMsg()!=WAIT_OBJECT_0)
				continue;

			pComThread->SendVehicleMsg(0, 0);
		}
		catch(CException*ex)
		{
			TCHAR buff[MAX_PATH+1];
			memset(buff, 0, MAX_PATH+1);
			ex->GetErrorMessage(buff, MAX_PATH);
			ex->Delete();
			CString str = _T("SendMsgThread MemoryException: ");
			str += buff;
			//	Log(str);
		}
	}

	SetEvent(pComThread->DealSendThreadEnded);
	return 0;
}
