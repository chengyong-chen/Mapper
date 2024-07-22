#include "stdafx.h"
#include "MSCommDlg.h"
#include "WavecomDlg.h"
#include "Global.h"

#include <mmsystem.h>

#include "../Coding/Coder.h"
#include "ComThread.h"

MSCommFrame CMSCommDlg::m_sendFrame; // ������Ϣ��������ַ
MSCommFrame CMSCommDlg::m_recvFrame; // ������Ϣ��������ַ

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMSCommDlg, CDialog)

CMSCommDlg::CMSCommDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CMSCommDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMSCommDlg)

	//}}AFX_DATA_INIT

	m_vtRecvMsg.Clear();
}

CMSCommDlg::~CMSCommDlg()
{
}

void CMSCommDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMSCommDlg)
	DDX_Control(pDX, IDC_MSCOMM, m_comm);
	//{{AFX_DATA_MAP(CMSCommDlg)
}

BEGIN_MESSAGE_MAP(CMSCommDlg, CDialog)
	//{{AFX_MSG_MAP(CMSCommDlg)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_OPENPORT, OnOpenPort)
	ON_MESSAGE(WM_DLGMSCOMM_TIMEOUT, OnTimeOut)
	ON_MESSAGE(WM_DLGMSCOMM_SENDMSG, OnSendMsg)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CMSCommDlg, CDialog)
	//{{AFX_EVENTSINK_MAP(CMSCommDlg)
	ON_EVENT(CMSCommDlg, IDC_MSCOMM, 1, OnCommMscomm, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CMSCommDlg::OnCommMscomm()
{
	switch(m_comm.get_CommEvent())
	{
	case 1: // �����¼�
		break;
	case 2: // �����¼�
	{
		const short cnt = m_comm.get_InBufferCount();
		if(cnt>0)
		{
			const VARIANT vt = m_comm.get_Input();
			char* p = (char*)(unsigned char*)vt.parray->pvData;

			int zero_num = 0;
			for(int i = 0; i<cnt; i++)
			{
				if(*(p+i)!=0)
				{
					break;
				}
				else
					zero_num++;
			}

			if(zero_num==cnt)
				break;

			if(m_vtRecvMsg.vt==VT_EMPTY)
			{
				m_vtRecvMsg = vt;

				SAFEARRAY* psa = SafeArrayCreateVector(VT_UI1, 0, cnt-zero_num);
				if(psa!=nullptr)
				{
					LPVOID lpArrayData;
					::SafeArrayAccessData(psa, &lpArrayData);
					::memcpy((char*)lpArrayData, (BYTE*)((vt.parray)->pvData)+zero_num, cnt-zero_num);
					::SafeArrayUnaccessData(psa);

					m_vtRecvMsg.vt = VT_ARRAY|VT_UI1;
					m_vtRecvMsg.parray = psa;

					::SafeArrayDestroy(psa);
				}
			}
			else
			{
				const int oldSize = GetSafeArraySize(m_vtRecvMsg.parray);

				SAFEARRAY* psa = SafeArrayCreateVector(VT_UI1, 0, oldSize+cnt-zero_num);
				if(psa!=nullptr)
				{
					LPVOID lpArrayData;
					::SafeArrayAccessData(psa, &lpArrayData);
					::memcpy((char*)lpArrayData, (BYTE*)(m_vtRecvMsg.parray)->pvData, oldSize);
					::memcpy((char*)lpArrayData+oldSize, (BYTE*)((vt.parray)->pvData)+zero_num, cnt-zero_num);
					::SafeArrayUnaccessData(psa);

					// m_vtRecvMsg.Clear();
					m_vtRecvMsg.vt = VT_ARRAY|VT_UI1;
					m_vtRecvMsg.parray = psa;

					::SafeArrayDestroy(psa);
				}
			}
		}

		if(m_vtRecvMsg.vt==VT_EMPTY)
			break;

		char* p = (char*)(unsigned char*)m_vtRecvMsg.parray->pvData;
		const int size = GetSafeArraySize(m_vtRecvMsg.parray);
		if(size>2&&p[size-1]==0X0A&&p[size-2]==0X0D)//�յ�һ����������Ϣ����ʼ����
		{
			BOOL bRtn = FALSE;
			switch(m_sendFrame.m_ATID)
			{
			case WAVCOM_SEND_AT: // ������ϢӦ��
			case AT_CMG_NOMSG: // ATָ��Ӧ��
				bRtn = NormalRespProc();
				break;
			case AT_CMG_HASMSG: // ATָ��Ӧ��
				bRtn = ATCmdRespProc();
				break;
			default: // ��֧�ֵ�ָ��
				break;
			}

			if(bRtn==FALSE&&size>=5)
			{
				// 0891683108100005F0040D91683119809221F00008306090516233230E4F60597D00480045004C004C004F
				// ATָ��PDU��ʽ���ձ���ע��
				// 08 - Length of the SMSC information (in this case 7 octets)
				// 91 - Type-of-address of the SMSC.(91 means international format of the phone number)
				// 683108100005F0 - Service center number(in decimal semi-octets). 
				// 04 - First octet of this SMS-DELIVER message
				// 0D - Address-Length. Length of the sender number (0B hex = 11 dec
				// 91 - Type-of-address of the sender number
				// 683119809221F0 - Sender number (decimal semi-octets), with a trailing F
				// 00 - Protocol identifier
				// 08 - Data coding scheme (0: ASCII; 8:UNICODE)
				// 30609051623323 - Time stamp (semi-octets)
				// 0E - User data length
				// 4F60597D00480045004C004C004F - TP-UD 8-bit octets representing 7-bit data
				if(strstr(p, "CMTI: \"SM\"")!=nullptr)// +CMTI: "SM",5   �µ���Ϣ֪ͨ
				{
					m_vtRecvMsg.vt = VT_EMPTY; //// ��ն��뻺���� .Clear();
					m_sendFrame.m_ATID = AT_CMG_HASMSG;// ��sim���ж�ȡ��һ����Ϣ
					const BSTR bstr = SysAllocString(L"AT+CMGR=1\r");
					m_sendFrame.m_Vt.vt = VT_ARRAY|VT_UI1;
					VectorFromBstr(bstr, &(m_sendFrame.m_Vt.parray));
					SysFreeString(bstr);

					this->PostMessage(WM_DLGMSCOMM_SENDMSG, 0, 0);
				}
				else if(strstr(p, "+CMT:")!=nullptr)// �µ���Ϣ
				{
					// +CMT: ,26
					// 0891683108100005F0040D91683119809221F00000306031613082230631D98C56B301
					// ������װ�����û�����
					m_recvFrame.m_Vt.vt = VT_EMPTY; // .Clear();
					m_recvFrame.m_Vt = m_vtRecvMsg;
					m_recvFrame.m_ATID = WAVCOM_RECV_AT; // ������Ϣ��־
					if(m_recvFrame.ParseRecvData()==false) //// ������Ϣ���� δ����,���ؼ�����
					{
					}
					else
					{
						m_vtRecvMsg.vt = VT_EMPTY; //// ������뻺�����е�����	
						OnMsgReceived();
					}
				}
			}
		}
	}
	break;
	case 3: // clear-to-send �߱仯
		break;
	case 4: // data-set-ready �߱仯
		break;
	case 5: // carrier detect �߱仯
		break;
	case 6: // ������
		break;
	case 7: // ���������г����ļ�����(ASCII��26)�ַ�
		break;
	default: // ����
		break;
	}
}

// ���� MSComm����
long CMSCommDlg::OnOpenPort(WPARAM wParam, LPARAM lParam)
{
	const int nPort = AfxGetApp()->GetProfileInt(_T("Wavecom"), _T("Port"), 1);

	m_comm.put_CommPort(nPort);
	m_comm.put_InBufferSize(1024);
	m_comm.put_OutBufferSize(2048);
	m_comm.put_InputMode(1);
	m_comm.put_Settings(_T(""));
	m_comm.put_RThreshold(1);
	m_comm.put_InputLen(0);
	m_comm.put_RTSEnable(TRUE);

	if(m_comm.get_PortOpen()==FALSE)
	{
		m_comm.put_PortOpen(TRUE);
	}

	if(m_comm.get_PortOpen()==TRUE)
	{
		SetEvent(CWavecomDlg::m_hEvent);
	}

	return m_comm.get_PortOpen();
}

// �˳��Ի���
void CMSCommDlg::Close()
{
	if(m_comm.get_PortOpen()==TRUE)
	{
		m_comm.put_PortOpen(FALSE);
	}
}

// ATָ���Ӧ����
BOOL CMSCommDlg::ATCmdRespProc()
{
	if(m_vtRecvMsg.vt==VT_EMPTY)
		return FALSE;
	const int size = GetSafeArraySize(m_vtRecvMsg.parray);

	char* p = new char[size+1];
	memset(p, 0, size+1);
	memcpy(p, (char*)(unsigned char*)m_vtRecvMsg.parray->pvData, size);
	if(strstr(p, "+CMGR")!=nullptr&&(strstr(p, "OK")!=nullptr||strstr(p, "ERROR")!=nullptr))
	{
		if(strstr(p, "OK")!=nullptr)
			m_recvFrame.nErrCode = 0;
		else if(strstr(p, "ERROR")!=nullptr)
			m_recvFrame.nErrCode = 1;

		m_recvFrame.m_Vt = m_vtRecvMsg;
		if(m_recvFrame.ParseRecvData()==false) //// ������Ϣ���� δ����
		{
		}
		else
		{
			m_vtRecvMsg.vt = VT_EMPTY; //// ������뻺�����е����� Clear();
			SetEvent(CWavecomDlg::m_hEvent);// �����߳������ź���

			OnMsgReceived();//��ʵ�ľ仰
		}

		delete[] p;
		return TRUE;
	}
	else
	{
		delete[] p;
		return FALSE;
	}
}

// ��������ָ��Ӧ����
BOOL CMSCommDlg::NormalRespProc()
{
	if(VT_EMPTY==m_vtRecvMsg.vt)
		return FALSE;
	const int size = GetSafeArraySize(m_vtRecvMsg.parray);

	char* p_fordelete = new char[size+1];
	char* p = p_fordelete;
	memset(p, 0, size+1);
	memcpy(p, (char*)(unsigned char*)m_vtRecvMsg.parray->pvData, size);

	char cc[2];
	cc[0] = 0x1A;
	cc[1] = 0;
	if(strstr(p, "AT+CMGS=")!=nullptr&&strstr(p, cc)!=nullptr)
	{
		p = strstr(p, cc);
		if(p!=nullptr&&(size-(p-p_fordelete))>3)
			p += 3;
	}

	BOOL bRtn = FALSE;
	if(strstr(p, "+CMT:")!=nullptr)
	{
		m_sendFrame.nErrCode = 0;
		m_recvFrame.nErrCode = 0;

		ResetEvent(CWavecomDlg::m_hEvent);

		bRtn = FALSE;
	}
	else if(strstr(p, "ERROR")!=nullptr)
	{
		m_vtRecvMsg.vt = VT_EMPTY;
		m_recvFrame.nErrCode = 1;
		SetEvent(CWavecomDlg::m_hEvent);

		bRtn = TRUE;
	}
	else
	{
		m_vtRecvMsg.vt = VT_EMPTY;
		m_sendFrame.nErrCode = 0;
		m_recvFrame.nErrCode = 0;

		SetEvent(CWavecomDlg::m_hEvent);
		bRtn = TRUE;
	}

	delete[] p_fordelete;
	p_fordelete = nullptr;

	return bRtn;
}

LONG CMSCommDlg::OnTimeOut(WPARAM wParam, LPARAM lParam)
{
	m_vtRecvMsg.vt = VT_EMPTY; // .Clear();

	return 0;
}

LONG CMSCommDlg::OnSendMsg(WPARAM wParam, LPARAM lParam)
{
	switch(m_sendFrame.m_ATID)
	{
	case WAVCOM_SEND_AT: // wavcom ������Ϣ
	{
		Sleep(10);
		char* p = (char*)m_sendFrame.m_Vt.parray->pvData;
		char* p2 = strstr(p, "\r");

		*p2 = 0;
		p2++;
		m_comm.put_Output(_variant_t(p));
		m_comm.put_Output(_variant_t("\r"));
		Sleep(50);
		m_comm.put_Output(_variant_t(p2));
	}
	break;
	case AT_CMG_HASMSG:
	case AT_CMG_NOMSG: // ATָ���������
		m_comm.put_Output(COleVariant(m_sendFrame.m_Vt));
		break;
	default: // ��֧�ֵ�ָ��
		break;
	}

	return 0;
}

void CMSCommDlg::OnMsgReceived() const
{
	RecvSMSPackage* pPack = new RecvSMSPackage();

	pPack->strMsg = m_recvFrame.m_MsgContent.bstrVal;
	pPack->strPhone = m_recvFrame.m_vtPhoneNo.bstrVal;
	pPack->dateRecv = m_recvFrame.m_RecvTime.dblVal;

	AfxBeginThread(CMSCommDlg::ProcessRecvSMSThread, (LPVOID)pPack);
}

////////////////////////////////////////////////////////////////////////////////
UINT CMSCommDlg::ProcessRecvSMSThread(LPVOID lParam)
{
	RecvSMSPackage* pPack = (RecvSMSPackage*)lParam;
	if(pPack==nullptr)
		return -1;

	if(pPack->strMsg.IsEmpty())
	{
		delete pPack;
		return -1;
	}

	try
	{
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

		CStringA szPhone = pPack->strPhone;
		int idx = -1;
		if((idx = szPhone.Find("+86"))==0)
			szPhone = szPhone.Mid(3);
		else if((idx = szPhone.Find("86"))==0)
			szPhone = szPhone.Mid(2);
		else if((idx = szPhone.Find("+886"))==0)
			szPhone = szPhone.Mid(4);
		else if((idx = szPhone.Find("886"))==0)
			szPhone = szPhone.Mid(3);
		const COleDateTime date(pPack->dateRecv);
		CString str = date.Format();

		SVehicle* pVehicle = CComThread::FindVehicleInfo(szPhone);
		if(pVehicle!=nullptr)
		{
			pMsg->m_dwVehicle = pVehicle->m_dwId;
			pMsg->m_strIVUSN = pVehicle->m_strIVUSN;
		}
		else
		{
			delete pMsg;
			pMsg = nullptr;

			delete pPack;
			pPack = nullptr;
			return -1;
		}

		CCoder* pCoder = CCoder::GetCoder(pVehicle->m_strIVUType);
		if(pCoder!=nullptr)
		{
			if(pCoder->DeCoding(pPack->strMsg, *pMsg)==true)
			{
				pMsg->m_varGPSTime += LOCAL_SYS_TIME_SPAN;
				CComThread::PushRecvMsgToQueue(pMsg);
			}
			else
			{
				delete pMsg;
				pMsg = nullptr;
			}

			delete pPack;
			pPack = nullptr;
		}
		else
		{
			delete pMsg;
			pMsg = nullptr;

			delete pPack;
			pPack = nullptr;
			return -1;
		}
	}
	catch(CException*ex)
	{
		delete pPack;
		pPack = nullptr;
		ex->Delete();
		return -1;
	}

	return 0;
}
