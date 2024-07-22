#include "stdafx.h"

#include "TCPSocket.h"
#include "Global.h"

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

CTCPSocket::CTCPSocket()
{
}

CTCPSocket::~CTCPSocket()
{
	TRACE("CTCPSocket::~CTCPSocket()");
}

#if 0
BEGIN_MESSAGE_MAP(CTCPSocket, CSocket)
	//{{AFX_MSG_MAP(CTCPSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

void CTCPSocket::OnAccept(int nErrorCode)
{
	CSocket::OnAccept(nErrorCode);

	CTCPSocket* pSocket = new CTCPSocket();
	Accept(*pSocket);
}

void CTCPSocket::OnReceive(int nErrorCode)
{
	RecvUDPPackage* pPack = new RecvUDPPackage;
	if(pPack!=nullptr)
	{
		pPack->len = this->Receive(pPack->buff, pPack->lMaxLen);
		if(pPack->len==SOCKET_ERROR)
		{
			delete pPack;
			pPack = nullptr;
		}
		else
		{
			AfxBeginThread(CTCPSocket::DealRecvMsgThread, (LPVOID)pPack);
		}
	}

	CSocket::OnReceive(nErrorCode);

	this->Close();
	delete this;
}

bool CTCPSocket::SendMsg(LPCSTR IVUNo, LPCSTR Msg)
{
	_ASSERTE(FALSE);
	return false;
}

bool CTCPSocket::SendMsg(LPCSTR IVUNo, LPCSTR Msg, BOOL Chinese, BOOL NeedReport)
{
	if(Msg==nullptr)
		return false;

	if(SendTo(Msg, strlen(Msg), 6868, _T("10.1.1.38"))==SOCKET_ERROR)
		return false;
	else
		return true;
}

UINT CTCPSocket::DealRecvMsgThread(LPVOID lParam)
{
	try
	{
		const RecvUDPPackage* pPack = (RecvUDPPackage*)lParam;
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
		if(pPack->buff[0]==_T('$')&&pPack->buff[4]==_T('G'))
		{
			pCoder = CCoder::GetCoder("STARG3_PS660");
		}

		if(pCoder==nullptr)
		{
			delete pPack;
			pPack = nullptr;

			delete pMsg;
			return 0;
		}
		CStringA ggg(pPack->buff);
		if(pCoder->DeCoding(ggg, *pMsg)==true)
		{
			pMsg->m_strIVUSN.MakeUpper();
			SVehicle* pVehicle = CComThread::FindVehicleInfo(pMsg->m_strIVUSN, true);// ���ճ������кŲ��ҳ���
			if(pVehicle!=nullptr)
			{
				pMsg->m_dwVehicle = pVehicle->m_dwId;
				pMsg->m_strIVUSN = pVehicle->m_strIVUSN;

				CComThread::PushRecvMsgToQueue(pMsg);
				//��Ҫ�����ڻط���Ϣ��IP��ַ�Ͷ˿ڵ���Ϣ
			}
			else
			{
				delete pMsg;
				pMsg = nullptr;

				delete pPack;
				pPack = nullptr;

				pMsg->m_dwVehicle = 0;
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
