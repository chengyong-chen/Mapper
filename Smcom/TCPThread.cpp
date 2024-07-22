#include "stdafx.h"

#include "ComThread.h"
#include "TCPThread.h"
#include "UDPThread.h"
#include "SMSThread.h"

#include "TCPSocket.h"

#include "../Coding/Coder.h"
#include "../Socket/MessageQ.h"
#include "../Socket/ExStdCFunction.h"
#include "../Coding/Instruction.h"
#include <tchar.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTCPThread

IMPLEMENT_DYNCREATE(CTCPThread, CComThread)

CTCPThread::CTCPThread()
	: CComThread()
{
	m_pTCPSocket = nullptr;
}

CTCPThread::~CTCPThread()
{
}

BOOL CTCPThread::InitInstance()
{
	m_pMainWnd = nullptr;

	CString host, name, aliase, ip;
	if(GetLocalHostInfos(host, name, aliase, ip)==FALSE)
	{
		return FALSE;
	}

	CTCPSocket* pTCPSocket = new CTCPSocket;
	if(pTCPSocket->Create(5757, SOCK_STREAM)==FALSE)
	{
		delete pTCPSocket;
		pTCPSocket = nullptr;

		PutMsgToMessageWnd(false ? _T("基于TCP的GPRS通讯服务打开失败") : _T("Failed when opening TCP's GPRS communication service"));
		return FALSE;
	}
	else
	{
		if(pTCPSocket->Listen()==FALSE)
		{
			delete pTCPSocket;
			pTCPSocket = nullptr;
			AfxMessageBox(_T("打开基于TCP的GPRS通讯服务的监听失败!"));

			return FALSE;
		}
		else
		{
			m_pTCPSocket = pTCPSocket;
			PutMsgToMessageWnd(false ? _T("基于TCP的GPRS通讯服务打开成功") : _T("Succeed openning TCP's GPRS communication service"));

			m_bValid = true;
		}
	}

	return CComThread::InitInstance();
}

int CTCPThread::ExitInstance()
{
	if(m_pTCPSocket!=nullptr)
	{
		m_pTCPSocket->Close();

		delete m_pTCPSocket;
		m_pTCPSocket = nullptr;
	}

	return CComThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CTCPThread, CComThread)
	//{{AFX_MSG_MAP(CTCPThread)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTCPThread message handlers

BOOL CTCPThread::SendVehicleMsg(WPARAM wParam, LPARAM lParam)
{
	try
	{
		if(g_send_msg_queue.GetMsgCount()<1)
			return FALSE;

		DWORD length = 0;
		CMessageWillSend* pMsg = (CMessageWillSend*)g_send_msg_queue.PopMessage(length);
		if(pMsg==nullptr)
		{
			return FALSE;
		}

		SVehicle* pVehicle = CComThread::FindVehicleInfo(pMsg->m_dwVehicle);
		if(pVehicle==nullptr)
		{
			delete pMsg;
			pMsg = nullptr;
			return FALSE;
		}
		else if(pVehicle->m_strIVUSIM.IsEmpty()==TRUE)
		{
			delete pMsg;
			pMsg = nullptr;
			return FALSE;
		}

		CCoder* pCoding = nullptr;
		const CStringA strWillSend = pMsg->m_strData;
		const CStringA strIVUSIM = pVehicle->m_strIVUSIM;
		if(m_pTCPSocket->SendMsg(strIVUSIM, strWillSend)==true)
		{
			delete pMsg;
			pMsg = nullptr;

			return FALSE;
		}
		else
		{
			CMessageReceived* msg = new CMessageReceived();
			msg->m_dwVehicle = pMsg->m_dwVehicle;;
			msg->m_strMessage.Format(_T("GSM 命令发送失败 用户:%s 车辆:%d 电话:%s 内容:%s"), pMsg->m_strUser, pMsg->m_dwVehicle, strIVUSIM, strWillSend);
			msg->m_wCommand = GPSSERVER_SEND_FAILED;
			CComThread::PushRecvMsgToQueue(msg);

			if(pMsg->m_uSendCnt<3)// 发送消息失败时，把消息重新压入发送队列，等待重发
			{
				pMsg->m_uSendCnt++;
				if(pCoding->IsSupportUDP()==true&&CComThread::m_pUDPThread!=nullptr&&CUDPThread::m_pUDPThread->m_bValid==true)
				{
					CComThread::m_pUDPThread->g_send_msg_queue.PushMessage(pMsg, sizeof(*pMsg));
				}
				else if(pCoding->IsSupportSMS()==true&&CComThread::m_pSMSThread!=nullptr&&CComThread::m_pSMSThread->m_bValid==true)
				{
					CComThread::m_pSMSThread->g_send_msg_queue.PushMessage(pMsg, sizeof(*pMsg));
				}
				else
				{
					g_send_msg_queue.PushMessage(pMsg, sizeof(*pMsg));
				}
			}
			else
			{
				delete pMsg;
				pMsg = nullptr;
			}

			return FALSE;
		}
	}
	catch(CException*ex)
	{
		ex->Delete();
	};

	return TRUE;
}
