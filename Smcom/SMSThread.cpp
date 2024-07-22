#include "stdafx.h"
#include "WavecomDlg.h"
#include "ComThread.h"
#include "SMSThread.h"
#include "UDPThread.h"
#include "TCPThread.h"

#include "../Coding/Coder.h"
#include "../Socket/MessageQ.h"
#include "../Coding/Instruction.h"
#include <tchar.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSMSThread

IMPLEMENT_DYNCREATE(CSMSThread, CComThread)

CSMSThread::CSMSThread()
	: CComThread()
{
	m_pWavecomDlg = nullptr;
}

CSMSThread::~CSMSThread()
{
}

BOOL CSMSThread::InitInstance()
{
	m_pMainWnd = nullptr;

	CWavecomDlg* pWavecomDlg = new CWavecomDlg;
	if(pWavecomDlg->Create(IDD_WAVECOM, CWnd::FromHandle(::GetDesktopWindow()))==TRUE)
	{
		if(pWavecomDlg->Open()==TRUE)
		{
			pWavecomDlg->ShowWindow(SW_HIDE);
			m_pWavecomDlg = pWavecomDlg;

			PutMsgToMessageWnd(false ? _T("����GSMͨѶ��SMS����򿪳ɹ�") : _T("Succeed opening GSM's communication service"));

			m_bValid = true;
		}
		else
		{
			CComThread::PutMsgToMessageWnd(false ? _T("ͨѶ�ؼ�����ʧ�ܣ����Ź��ܽ�����ʹ��") : _T("Failed creating Commucation's Control, sms is not available"));//�����Ի���ʧ��

			pWavecomDlg->PostMessage(WM_DESTROY, 0, 0);
			pWavecomDlg->DestroyWindow();
			delete pWavecomDlg;
			pWavecomDlg = nullptr;

			return FALSE;
		}
	}
	else
	{
		CComThread::PutMsgToMessageWnd(false ? _T("ͨѶ�ؼ�����ʧ�ܣ����Ź��ܽ�����ʹ��") : _T("Failed creating Commucation's Control, sms is not available"));//�����Ի���ʧ��

		delete pWavecomDlg;
		pWavecomDlg = nullptr;

		return FALSE;
	}

	return CComThread::InitInstance();
}

int CSMSThread::ExitInstance()
{
	if(m_pWavecomDlg!=nullptr)
	{
		m_pWavecomDlg->Close();

		Sleep(1500); // ����ʱ�䣬��ͨѶ�̴߳��ݹر�

		delete m_pWavecomDlg;
		m_pWavecomDlg = nullptr;
	}

	return CComThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CSMSThread, CComThread)
	//{{AFX_MSG_MAP(CSMSThread)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSMSThread message handlers
BOOL CSMSThread::SendVehicleMsg(WPARAM wParam, LPARAM lParam)
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

		CCoder* pCoding = CCoder::GetCoder(pVehicle->m_strIVUType);
		if(pCoding==nullptr)
		{
			delete pMsg;
			pMsg = nullptr;
			return FALSE;
		}

		CStringA strIVUSIM = pVehicle->m_strIVUSIM;
		if(strIVUSIM.Find("13")==0)
		{
			strIVUSIM.Insert(0, "86");
		}
		else if(strIVUSIM.Find("09")==0)
		{
			strIVUSIM.Delete(0);
			strIVUSIM.Insert(0, "886");
		}
		const CStringA strWillSend = pMsg->m_strData;
		if(m_pWavecomDlg->SendMsg(strIVUSIM, strWillSend, pCoding->IsUseUnicodeCharSet())==true)
		{
			delete pMsg;
			pMsg = nullptr;
		}
		else
		{
			CMessageReceived* msg = new CMessageReceived();
			msg->m_dwVehicle = pMsg->m_dwVehicle;;
			msg->m_strMessage.Format(_T("GSM �����ʧ�� �û�:%s ����:%d �绰:%s ����:%s"), pMsg->m_strUser, pMsg->m_dwVehicle, strIVUSIM, strWillSend);
			msg->m_wCommand = GPSSERVER_SEND_FAILED;
			CComThread::PushRecvMsgToQueue(msg);

			if(pMsg->m_uSendCnt<3)// ������Ϣʧ��ʱ������Ϣ����ѹ�뷢�Ͷ��У��ȴ��ط�
			{
				pMsg->m_uSendCnt++;
				if(pCoding->IsSupportUDP()==true&&CComThread::m_pUDPThread!=nullptr&&CUDPThread::m_pUDPThread->m_bValid==true)
				{
					CComThread::m_pUDPThread->g_send_msg_queue.PushMessage(pMsg, sizeof(*pMsg));
				}
				else if(pCoding->IsSupportTCP()==true&&CComThread::m_pTCPThread!=nullptr&&CTCPThread::m_pTCPThread->m_bValid==true)
				{
					CComThread::m_pTCPThread->g_send_msg_queue.PushMessage(pMsg, sizeof(*pMsg));
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
