// WavecomDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Global.h"
#include "WavecomDlg.h"
#include "MSCommDlg.h"
#include "MSCommThread.h"
#include "Frame.h"

#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HANDLE CWavecomDlg::m_hEvent = nullptr;

IMPLEMENT_DYNAMIC(CWavecomDlg, CDialog)

CWavecomDlg::CWavecomDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CWavecomDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWavecomDlg)

	//}}AFX_DATA_INIT

	m_pMSCommThread = nullptr;

	m_hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	ASSERT(m_hEvent);
}

CWavecomDlg::~CWavecomDlg()
{
	m_pMSCommThread = nullptr;
}

void CWavecomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAtlasAttDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWavecomDlg, CDialog)
	//{{AFX_MSG_MAP(CWavecomDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CWavecomDlg message handlers
///////////////////////////////////////////////////////////////////////////
// 接口函数
// 打开通讯控件接口
BOOL CWavecomDlg::Open()
{
	CMSCommThread* pMSCommThread = (CMSCommThread*)AfxBeginThread(RUNTIME_CLASS(CMSCommThread));
	if(pMSCommThread!=nullptr)
	{
		int nCount = 0;
		do
		{
			nCount++;
			Sleep(1000);
		} while(pMSCommThread->m_pMSCommDlg==nullptr&&nCount<10);

		if(pMSCommThread->m_pMSCommDlg==nullptr)
		{
			AfxMessageBox(_T("打开Wavecom模块失败!"));
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	CMSCommDlg::m_sendFrame.nOutBufferSize = 2048;

	::PostMessage(pMSCommThread->m_pMSCommDlg->m_hWnd, WM_OPENPORT, 0, 0);

	ResetEvent(m_hEvent);
	const DWORD dwRtn = WaitForSingleObject(m_hEvent, 7000); // 端口打开超时，缺省7秒
	if(dwRtn==WAIT_OBJECT_0)
	{
		m_pMSCommThread = pMSCommThread;

		//	if(m_comStting.Find(_T("9600")) >= 0)
		//		SendATCmd(BSTR("AT+CBST=7,0,1\r"));// 设置波特率

		if(this->SendATCmd("ATE0\r")==false)// 关闭回显
		{
		}

		if(this->SendATCmd("AT+CMGF=0\r")==false) // 设置命令失败认为串口未打开
		{
			Close();// 则关闭通讯窗口
			return FALSE;
		}

		if(this->SendATCmd("AT+CNMI=2,2,0,0,0\r")==false)
		{
		}
		return TRUE;
	}
	else
	{
		::PostThreadMessage(pMSCommThread->m_nThreadID, WM_QUIT, 0, 0);
		WaitForSingleObject(pMSCommThread->m_hThread, INFINITE);

		Sleep(200);
		pMSCommThread = nullptr;
		return FALSE;
	}
}

void CWavecomDlg::Close()
{
	if(m_pMSCommThread!=nullptr)
	{
		::PostThreadMessage(m_pMSCommThread->m_nThreadID, WM_QUIT, 0, 0);
		WaitForSingleObject(m_pMSCommThread->m_hThread, INFINITE);

		m_pMSCommThread = nullptr;
	}
}

bool CWavecomDlg::SendATCmd(CStringA strAtCmd) const
{
	if(m_pMSCommThread==nullptr)
		return false;

	if(strAtCmd.Find("CMGR")!=-1)// AT 指令
	{
		CMSCommDlg::m_sendFrame.m_ATID = AT_CMG_HASMSG;
		CMSCommDlg::m_recvFrame.m_ATID = AT_CMG_HASMSG;
	}
	else
	{
		CMSCommDlg::m_sendFrame.m_ATID = AT_CMG_NOMSG;
		CMSCommDlg::m_recvFrame.m_ATID = AT_CMG_NOMSG;
	}

	CMSCommDlg::m_sendFrame.m_Vt.vt = VT_BSTR;// 数据内容
	CMSCommDlg::m_sendFrame.m_Vt.bstrVal = strAtCmd.AllocSysString();
	CMSCommDlg::m_sendFrame.ParseSendData();

	m_pMSCommThread->m_pMSCommDlg->PostMessage(WM_DLGMSCOMM_SENDMSG, 0, 0);

	ResetEvent(m_hEvent);
	if(WaitForSingleObject(m_hEvent, 7000)!=WAIT_OBJECT_0)// 发送超时，缺省7秒
	{
		//	m_pMSCommThread->m_pMSCommDlg->PostMessage(WM_DLGMSCOMM_TIMEOUT, 0, 0);
		return false;
	}
	else
		return true;
}

// 发送消息接口
// return:
//         1 - send message failed
//         0 - successful
//        -1 - port is not opened
//        -2 - time out for sending
bool CWavecomDlg::SendMsg(CStringA strPhoneNo, CStringA strMsg, BOOL bUseUnicode)
{
	if(strPhoneNo.GetLength()==0)
	{
		return SendATCmd(strMsg);
	}
	else
	{
		return SendNormalMsg(strPhoneNo, strMsg, bUseUnicode);
	}
}

// 发送正常消息
bool CWavecomDlg::SendNormalMsg(CStringA strPhoneNo, CStringA strMsg, BOOL bUseUnicode) const
{
	if(m_pMSCommThread==nullptr)
		return false;

	DWORD dwRtn = WaitForSingleObject(m_hEvent, 7000);

	// 发送编码
	CMSCommDlg::m_sendFrame.m_bSendUseUnicode = bUseUnicode;
	CMSCommDlg::m_sendFrame.m_ATID = WAVCOM_SEND_AT;

	// 数据内容
	CMSCommDlg::m_sendFrame.m_Vt.vt = VT_BSTR;
	CMSCommDlg::m_sendFrame.m_Vt.bstrVal = strMsg.AllocSysString();

	// 消息相关的电话号码
	CMSCommDlg::m_sendFrame.m_vtPhoneNo.vt = VT_BSTR;
	CMSCommDlg::m_sendFrame.m_vtPhoneNo.bstrVal = strPhoneNo.AllocSysString();

	CMSCommDlg::m_sendFrame.ParseSendData(); // TRUE - 发送数据解析
	m_pMSCommThread->m_pMSCommDlg->PostMessage(WM_DLGMSCOMM_SENDMSG, 0, 0);

	ResetEvent(m_hEvent);
	if(WaitForSingleObject(m_hEvent, 7000)!=WAIT_OBJECT_0)// 发送超时，缺省7秒
	{
		m_pMSCommThread->m_pMSCommDlg->PostMessage(WM_DLGMSCOMM_TIMEOUT, 0, 0);
		return false;
	}
	else if(CMSCommDlg::m_recvFrame.nErrCode!=0)
	{
		return false; // send message failed
	}
	else
		return true;
}
