#include "stdafx.h"

#include "Global.h"
#include "UDPSocket.h"
#include "ComThread.h"

#include "../Coding/Coder.h"
#include "../Socket/ExStdCFunction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

map<DWORD, Address_IP_Port> CUDPSocket::g_mapVhcNumToAddress;
CMutex CUDPSocket::g_mutexMapVhcNumToAddress;

void CUDPSocket::SetIPPort(const DWORD& dwId, CStringA& strIP, UINT uPort)
{
	Address_IP_Port addr;
	addr.m_strIP = LPCSTR(strIP);
	addr.m_uPort = uPort;

	g_mutexMapVhcNumToAddress.Lock();
	g_mapVhcNumToAddress[dwId] = addr;
	g_mutexMapVhcNumToAddress.Unlock();
}

BOOL CUDPSocket::GetIPPort(const DWORD& dwId, CStringA& strIP, UINT& uPort)
{
	if(dwId<=0)
		return FALSE;

	g_mutexMapVhcNumToAddress.Lock();
	const map<DWORD, Address_IP_Port>::const_iterator it = g_mapVhcNumToAddress.find(dwId);
	if(it==g_mapVhcNumToAddress.end())
	{
		strIP = _T("NO IP");
		uPort = 0;
		g_mutexMapVhcNumToAddress.Unlock();
		return FALSE;
	}
	else
	{
		strIP = it->second.m_strIP.c_str();
		uPort = it->second.m_uPort;
		g_mutexMapVhcNumToAddress.Unlock();
		return TRUE;
	}
}

HANDLE m_hGroupSendMutex;
ADOCG::_ConnectionPtr connectPtr = nullptr;

CUDPSocket::CUDPSocket()
{
}

CUDPSocket::~CUDPSocket()
{
	TRACE("CUDPSocket::~CUDPSocket()");
}

#if 0
BEGIN_MESSAGE_MAP(CUDPSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CUDPSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

bool CUDPSocket::SendMsg(LPCSTR IVUNo, LPCSTR Msg)
{
	_ASSERTE(FALSE);
	return false;
}

bool CUDPSocket::SendMsg(LPCSTR IVUNo, LPCSTR Msg, BOOL Chinese, BOOL NeedReport)
{
	if(Msg==nullptr)
		return false;

	if(SendTo(Msg, strlen(Msg), 6868, _T("10.1.1.38"))==SOCKET_ERROR)
	{
		return false;
	}
	else
	{
		return true;
	}
}

UINT CUDPSocket::DealRecvMsgThread(LPVOID lParam)
{
	try
	{
		RecvUDPPackage* pPack = (RecvUDPPackage*)lParam;

		if(pPack==nullptr)
			return 1;

		if(pPack->len<=13||pPack->len>=pPack->lMaxLen)
		{
			delete pPack;
			pPack = nullptr;
			return 1;
		}

		CMessageReceived* pMsg = new CMessageReceived;
		pMsg->m_wCommand = 0XFF;
		pMsg->m_dwVehicle = 0;
		pMsg->m_Direct = 0.0;
		pMsg->m_Lon = 0.0;
		pMsg->m_Lat = 0.0;
		pMsg->m_Veo = 0.0;
		pMsg->m_strMessage = _T("");
		pMsg->m_dwStatus = 0;
		pMsg->m_varRevcTime = COleDateTime::GetCurrentTime();

		CCoder* pCoder = nullptr;
		if(pPack->buff[0]==_T('>')||pPack->buff[1]==_T('>'))
		{
			pCoder = CCoder::GetCoder("NP860");
		}

		if(pCoder==nullptr)
		{
			delete pPack;
			pPack = nullptr;

			delete pMsg;
			return 0;
		}
		CStringA dddd(pPack->buff);
		if(pCoder->DeCoding(dddd, *pMsg)==true)
		{
			pMsg->m_strIVUSN.MakeUpper();
			SVehicle* pVehicle = CComThread::FindVehicleInfo(pMsg->m_strIVUSN, true);// ���ճ������кŲ��ҳ���
			if(pVehicle!=nullptr)
			{
				pMsg->m_dwVehicle = pVehicle->m_dwId;
				pMsg->m_strIVUSN = pVehicle->m_strIVUSN;

				CUDPSocket::SetIPPort(pMsg->m_dwVehicle, pPack->strIP, pPack->nPort);

				CComThread::PushRecvMsgToQueue(pMsg);
			}
			else
			{
				pMsg->m_dwVehicle = 0;

				delete pMsg; //���ɾ����ִ��CComThread::PushRecvMsgToQueue(pMsg); ���������ݿ���ʱ�޶�Ӧ��vehicleID������
				pMsg = nullptr;

				delete pPack;
				pPack = nullptr;
			}
		}
		else
		{
			if(pMsg->m_strMessage.IsEmpty()==FALSE)
			{
				CComThread::PutMsgToMessageWnd(pMsg->m_strMessage);
				Log_or_AfxMessageBox(pMsg->m_strMessage);
			}

			delete pMsg;
		}

		delete pPack;
		pPack = nullptr;
	}
	catch(CException*ex)
	{
		ex->Delete();
	};

	return 0;
}

void CUDPSocket::OnReceive(int nErrorCode)
{
	try
	{
		RecvUDPPackage* pPack = new RecvUDPPackage;
		if(pPack==nullptr)
			return;
		CString straIP(pPack->strIP);
		pPack->len = ReceiveFrom(pPack->buff, pPack->lMaxLen, straIP, pPack->nPort);
		if(pPack->len==SOCKET_ERROR)
		{
			delete pPack;
			pPack = nullptr;
		}
		else
		{
			AfxBeginThread(CUDPSocket::DealRecvMsgThread, (LPVOID)pPack);
		}
	}
	catch(CException*ex)
	{
		ex->Delete();
	};

	CAsyncSocket::OnReceive(nErrorCode);
}
