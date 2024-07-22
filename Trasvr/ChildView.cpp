// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "Trasvr.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Global.h"
#include "ChildView.h"
#include "MainFrm.h"

#include <queue>
#include <afxdb.h>
#include <string>

#include "ClientForm.h"
#include "VehicleForm.h"
#include "MessageForm.h"

#include "..\Smcom\Global.h"
#include "..\Smcom\ComThread.h"
#include "..\Smcom\SMSThread.h"
#include "..\Smcom\UDPThread.h"
#include "..\Smcom\TCPThread.h"
#include "..\Smcom\HTTPThread.h"

#include "..\Smcom\SetupDlg.h"

#include "..\Socket\ExStdCFunction.h"
#include "..\Coding\Message.h"
#include "..\Coding\Instruction.h"
#include "..\Coding\Coder.h"
#include "..\Navigate\DatabasePage.h"
#include "..\Navigate\PortDlg.h"

#include "../Atlas/WebBrowser.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  TIMER_FRESH_VHCINFO 300
#define  TIMER_PULSE 301

CMessageQueue CChildView::g_SendMsgSaveQueue;
CMessageQueue CChildView::g_RecvMsgSaveQueue;

CChildView::CChildView()
{
	m_pSMSThread = nullptr;
	m_pUDPThread = nullptr;
	m_pTCPThread = nullptr;
	m_pHTTPThread = nullptr;
	
	m_pActiveWnd = nullptr;
	m_connectPtr = nullptr,
	
	InitializeCriticalSection(&csClientlist);
}

CChildView::~CChildView()
{
	DeleteCriticalSection(&csClientlist);
}

BEGIN_MESSAGE_MAP(CChildView,CWnd)
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP	
	ON_MESSAGE(WM_DELETECLIENT,OnDeleteClient)
END_MESSAGE_MAP()


// CChildView message handlers
BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if(!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, ::LoadCursor(nullptr, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

void CChildView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	if(m_pActiveWnd != nullptr)
	{
		m_pActiveWnd->SetWindowPos(&wndTop, 0, 0, cx, cy, SWP_SHOWWINDOW);
	}
}

LRESULT CChildView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	try
	{
		switch(message)
		{
		case WM_CUSTOM_GETVHCINFO_BYTEL:
			return (LRESULT)m_vectVehicle.GetVehicleInfo((CString*)wParam, (BOOL)lParam);
			break;
		case WM_CUSTOM_GETVHCINFO_BYID:
			return (LRESULT)m_vectVehicle.GetVehicleInfo((DWORD)wParam);
			break;
		default:
			break;
		}
	}
	catch(CException* ex)
	{
	};

	return CWnd::WindowProc(message, wParam, lParam);
}

void CChildView::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case TIMER_FRESH_VHCINFO:
		this->KillTimer(TIMER_FRESH_VHCINFO);
		FreshVectVehicle();
		this->SetTimer(TIMER_FRESH_VHCINFO, 1000*60*3, nullptr);
		break;
	case TIMER_PULSE:
		{
			POSITION pos = m_SinkSocketList.GetHeadPosition();
			while(pos != nullptr)
			{
				try
				{
					CGpsSinkSocket* pSocket = (CGpsSinkSocket*)m_SinkSocketList.GetNext(pos);
					if(pSocket->IsTimerOut())
					{
						CString str;
						str.Format(false ? _T("--> 客户端可能意外中断.User:%s") : _T("--> Clinet interrupted accidentally. User:%s"), pSocket->m_strUser);
						CChildView::PutMsgToMessageWnd(str);

						OnDeleteClient((UINT)pSocket,0);
					}
					else
					{
						STRUCT_MSG_HEADER* lp = new STRUCT_MSG_HEADER();
						lp->wCmdId = SERVER_PULSE;
						pSocket->SendMsg((char*)lp, sizeof(*lp));
					}
				}
				catch(CException* e)
				{
					e->ReportError();
					e->Delete();
				}
			}
		}
		break;
	default:
		break;
	}
	
	CWnd::OnTimer(nIDEvent);
}

void CChildView::ChangeContent(const DWORD& dwId,DWORD wParam)
{
	if(m_pActiveWnd != nullptr)
	{
		m_pActiveWnd->ShowWindow(FALSE);
		m_pActiveWnd->SendMessage(WM_COMMAND,IDOK,0);
		m_pActiveWnd->SendMessage(WM_DESTROY,0,0);
		delete m_pActiveWnd;
		m_pActiveWnd = nullptr;
	}

	switch(dwId)
	{
	case 1:
		{
			m_pActiveWnd = new CClientForm(this);
			((CClientForm*)m_pActiveWnd)->Create(IDD_CLIENTFORM,this);
			((CClientForm*)m_pActiveWnd)->ShowLoginUsers(m_SinkSocketList);
		}
		break;
	case 2:
		{
			m_pActiveWnd = new CVehicleForm(this);
			((CVehicleForm*)m_pActiveWnd)->Create(IDD_VEHICLEFORM,this);

			POSITION pos = m_SinkSocketList.GetHeadPosition();
			while(pos != nullptr)
			{
				CGpsSinkSocket* pSocket = (CGpsSinkSocket*)m_SinkSocketList.GetNext(pos);
				((CVehicleForm*)m_pActiveWnd)->AddClient(pSocket);
			}
			((CVehicleForm*)m_pActiveWnd)->ShowInspectVehicles((CGpsSinkSocket*)wParam);
		}
		break;
	case 3:
		{
			m_pActiveWnd = new CMessageForm(this);
			((CMessageForm*)m_pActiveWnd)->Create(IDD_MESSAGEFORM,this);
			((CMessageForm*)m_pActiveWnd)->ShowMessages();
		}
		break;
	case 4:
		{			
			m_pActiveWnd = new CSetupDlg(this);

			HINSTANCE hInst = ::LoadLibrary(_T("Smcom.dll"));
			HINSTANCE hInstOld = AfxGetResourceHandle();
			AfxSetResourceHandle(hInst);

			((CSetupDlg*)m_pActiveWnd)->Create(IDD_SETUP,this);

			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
		}
		break;
	case 5:
		{			
			m_pActiveWnd = new CDatabasePage();

			HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
			HINSTANCE hInstOld = AfxGetResourceHandle();
			AfxSetResourceHandle(hInst);

			((CDatabasePage*)m_pActiveWnd)->Create(IDD_DATABASE,this);

			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
		}
		break;
	case 6:
		{	
			m_pActiveWnd = new CWebBrowser;
			((CWebBrowser*)m_pActiveWnd)->Create(nullptr,WS_CHILD|WS_EX_TOOLWINDOW ,CRect(0,0,0,0),this,0);
			((CWebBrowser*)m_pActiveWnd)->Navigate(_T("http://localhost:8080/Trasvr/vhcmgr.jsp?op=add"),nullptr,nullptr,nullptr,nullptr);
		}
		break;
	case 7:
		{			
			m_pActiveWnd = new CWebBrowser;
			((CWebBrowser*)m_pActiveWnd)->Create(nullptr,WS_CHILD|WS_EX_TOOLWINDOW ,CRect(0,0,0,0),this,0);
			((CWebBrowser*)m_pActiveWnd)->Navigate(_T("http://localhost:8080/Trasvr/vhcmgr.jsp?op=query"),nullptr,nullptr,nullptr,nullptr);
		}
		break;
	case 8:
		{			
			m_pActiveWnd = new CWebBrowser;
			((CWebBrowser*)m_pActiveWnd)->Create(nullptr,WS_CHILD|WS_EX_TOOLWINDOW ,CRect(0,0,0,0),this,0);
			((CWebBrowser*)m_pActiveWnd)->Navigate(_T("http://localhost:8080/Trasvr/groupmgr.jsp"),nullptr,nullptr,nullptr,nullptr);
		}
		break;
	case 9:
		{			
			m_pActiveWnd = new CWebBrowser;
			((CWebBrowser*)m_pActiveWnd)->Create(nullptr,WS_CHILD|WS_EX_TOOLWINDOW ,CRect(0,0,0,0),this,0);
			((CWebBrowser*)m_pActiveWnd)->Navigate(_T("http://localhost:8080/Trasvr/inspmgr.jsp?op=add"),nullptr,nullptr,nullptr,nullptr);
		}
		break;
	case 10:
		{			
			m_pActiveWnd = new CWebBrowser;
			((CWebBrowser*)m_pActiveWnd)->Create(nullptr,WS_CHILD|WS_EX_TOOLWINDOW ,CRect(0,0,0,0),this,0);
			((CWebBrowser*)m_pActiveWnd)->Navigate(_T("http://localhost:8080/Trasvr/recmgr.jsp?op=position"),nullptr,nullptr,nullptr,nullptr);
		}
		break;
	case 11:
		{			
			m_pActiveWnd = new CWebBrowser;
			((CWebBrowser*)m_pActiveWnd)->Create(nullptr,WS_CHILD|WS_EX_TOOLWINDOW ,CRect(0,0,0,0),this,0);
			((CWebBrowser*)m_pActiveWnd)->Navigate(_T("http://localhost:8080/Trasvr/recmgr.jsp?op=warning"),nullptr,nullptr,nullptr,nullptr);
		}
		break;
	case 12:
		{			
			m_pActiveWnd = new CWebBrowser;
			((CWebBrowser*)m_pActiveWnd)->Create(nullptr,WS_CHILD|WS_EX_TOOLWINDOW ,CRect(0,0,0,0),this,0);
			((CWebBrowser*)m_pActiveWnd)->Navigate(_T("http://localhost:8080/Trasvr/recmgr.jsp?op=message"),nullptr,nullptr,nullptr,nullptr);
		}
		break;
	}

	if(m_pActiveWnd != nullptr)
	{
		CRect rect;
		this->GetClientRect(rect);
		m_pActiveWnd->SetWindowPos(&wndTop, 0, 0, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	}
}

BOOL CChildView::DestroyWindow()
{
	if(m_pActiveWnd != nullptr)
	{
		m_pActiveWnd->DestroyWindow();
		delete m_pActiveWnd;
		m_pActiveWnd = nullptr;
	}

	Stop();

	return CWnd::DestroyWindow();
}

// 开启监听服务
BOOL CChildView::Start()
{
	CString strSMSCard = AfxGetApp()->GetProfileString(_T("Wavecom"), _T("strSIMCard"),_T("13521582294"));
	CCoder::SetSMSCard(strSMSCard); 

	if(OpenCenterDataBase(m_connectPtr) == FALSE)
	{
		AfxMessageBox(_T("系统无法连接数据库，请检查数据库参数配置。"));
		return FALSE;
	}

	if(m_vectVehicle.GetBroadCastVehicleInfos(m_connectPtr) == FALSE)
	{
		AfxMessageBox(_T("无法从数据库中获取已登记的车辆信息！"));
	}

	if(m_pSMSThread == nullptr)
	{
		CSMSThread* pThread = (CSMSThread*)::AfxBeginThread(RUNTIME_CLASS(CSMSThread));
		if(pThread != nullptr)
		{
			int nCount = 0;
			do
			{
				nCount++;
				Sleep(1000);
			}while(pThread->m_bValid == false && nCount<20);//存在风险，有可能这个时候pThread已经被ExitInstance
			
			if(pThread->m_bValid == true)
			{
				m_pSMSThread = pThread;
				CComThread::m_pSMSThread = pThread;
			}
			else
			{
				m_pSMSThread = nullptr;
				CComThread::m_pSMSThread = nullptr;

				AfxMessageBox(_T("启动SMS通讯方式失败!"));
			}
		}
		else
		{
			AfxMessageBox(_T("启动SMS通讯方式失败!"));
		}
	}

	if(m_pUDPThread == nullptr)
	{
		CUDPThread* pThread = (CUDPThread*)::AfxBeginThread(RUNTIME_CLASS(CUDPThread));
		if(pThread != nullptr)
		{
			int nCount=0;
			do
			{
				nCount++;
				Sleep(500);
			}while(pThread->m_bValid == false && nCount<10);//存在风险，有可能这个时候pThread已经被ExitInstance
			
			if(pThread->m_bValid == true)
			{
				m_pUDPThread = pThread;
				CComThread::m_pUDPThread = pThread;
			}
			else
			{
				m_pUDPThread = nullptr;
				CComThread::m_pUDPThread = nullptr;

				AfxMessageBox(_T("启动基于GPRS的UDP协议通讯方式失败!"));
			}
		}
		else
		{
			AfxMessageBox(_T("启动基于GPRS的UDP协议通讯方式失败!"));
		}
	}

	if(m_pTCPThread == nullptr)
	{
		CTCPThread* pThread = (CTCPThread*)::AfxBeginThread(RUNTIME_CLASS(CTCPThread));
		if(pThread != nullptr)
		{
			int nCount=0;
			do
			{
				nCount++;
				Sleep(1000);
			}while(pThread->m_bValid == false && nCount<10);//存在风险，有可能这个时候pThread已经被ExitInstance
			
			if(pThread->m_bValid == true)
			{
				m_pTCPThread = pThread;
				CComThread::m_pTCPThread = m_pTCPThread;
			}
			else
			{
				m_pTCPThread = nullptr;
				CComThread::m_pTCPThread = nullptr;

				AfxMessageBox(_T("启动基于GPRS的TCP协议通讯方式失败!"));
			}
		}
		else
		{
			AfxMessageBox(_T("启动基于GPRS的TCP协议通讯方式失败!"));
		}
	}

	if(m_pHTTPThread == nullptr)
	{
		CHTTPThread* pThread = (CHTTPThread*)::AfxBeginThread(RUNTIME_CLASS(CHTTPThread));
		if(pThread != nullptr)
		{
			int nCount=0;
			do
			{
				nCount++;
				Sleep(1000);
			}while(pThread->m_bValid == false && nCount<10);//存在风险，有可能这个时候pThread已经被ExitInstance
			
			if(pThread->m_bValid == true)
			{
				m_pHTTPThread = pThread;
				CComThread::m_pHTTPThread = m_pHTTPThread;
			}
			else
			{
				m_pHTTPThread = nullptr;
				CComThread::m_pHTTPThread = nullptr;

				AfxMessageBox(_T("启动基于HTTP协议通讯方式失败!"));
			}
		}
		else
		{
			AfxMessageBox(_T("启动基于HTTP协议通讯方式失败!"));
		}
	}

	if(AfxBeginThread(DealRecvMsgThread,(LPVOID)(this)) == FALSE)
	{
		AfxMessageBox(_T("启动处理接收消息线程失败!"));

		Stop();
		return FALSE;
	}
	
	if(AfxBeginThread(SaveSendMsgThread,(LPVOID)(this)) == FALSE)
	{
		AfxMessageBox(_T("启动处理接收消息线程失败!"));

		Stop();
		return FALSE;
	}

	if(AfxBeginThread(SaveRecvMsgThread,(LPVOID)(this)) == FALSE)
	{
		AfxMessageBox(_T("启动处理接收消息线程失败!"));

		Stop();
		return FALSE;
	}

	m_sockGpsListen.SetAcptCallbackFuc(AcptGpsClient);
	m_sockGpsListen.SetParent(this);
	if(m_sockGpsListen.StartListen(1501) == FALSE)
	{
		CString strMsg;
		strMsg.Format(_T("服务器端口%d监听失败，系统将退出!"), 1501);
		AfxMessageBox(strMsg);

		Stop();
		return FALSE;
	}

	m_sockXmlListen.SetAcptCallbackFuc(AcptXmlClient);
	m_sockXmlListen.SetParent(this);
	if(m_sockXmlListen.StartListen(1502) == FALSE)
	{
		CString strMsg;
		strMsg.Format(_T("服务器端口%d监听失败，系统将无法接收Xml请求!"), 1502);
		AfxMessageBox(strMsg);
	}
	
	m_sockWebListen.SetAcptCallbackFuc(AcptWebClient);
	m_sockWebListen.SetParent(this);
	if(m_sockWebListen.StartListen(1503) == FALSE)
	{
		CString strMsg;
		strMsg.Format(_T("服务器端口1503监听失败，系统将无法接收Web请求!"));
		AfxMessageBox(strMsg);
	}

	this->SetTimer(TIMER_FRESH_VHCINFO, 1000*60*3, nullptr);
	this->SetTimer(TIMER_PULSE, 1000*60, nullptr);//1分钟发送个链路监测数据包,并检测是否客户端已经中断

	return TRUE;
}

void CChildView::Stop() 
{
	this->KillTimer(TIMER_FRESH_VHCINFO);
	this->KillTimer(TIMER_PULSE);

	m_sockWebListen.Close();
	m_sockXmlListen.Close();
	m_sockGpsListen.Close();
	
	if(m_pSMSThread != nullptr)
	{
		CComThread::m_pSMSThread = nullptr;

		::PostThreadMessage(m_pSMSThread->m_nThreadID, WM_QUIT, 0, 0);
		::WaitForSingleObject(m_pSMSThread->m_hThread,INFINITE);//等待线程结束

		m_pSMSThread = nullptr;
	}

	if(m_pUDPThread != nullptr)
	{
		CComThread::m_pUDPThread = nullptr;

		::PostThreadMessage(m_pUDPThread->m_nThreadID, WM_QUIT, 0, 0);
		::WaitForSingleObject(m_pUDPThread->m_hThread,INFINITE);//等待线程结束

		m_pUDPThread = nullptr;
	}

	if(m_pTCPThread != nullptr)
	{
		CComThread::m_pTCPThread = nullptr;

		::PostThreadMessage(m_pTCPThread->m_nThreadID, WM_QUIT, 0, 0);
		::WaitForSingleObject(m_pTCPThread->m_hThread,INFINITE);//等待线程结束

		m_pTCPThread = nullptr;
	}

	if(m_pHTTPThread != nullptr)
	{
		CComThread::m_pHTTPThread = nullptr;

		::PostThreadMessage(m_pHTTPThread->m_nThreadID, WM_QUIT, 0, 0);
		::WaitForSingleObject(m_pHTTPThread->m_hThread,INFINITE);//等待线程结束

		m_pHTTPThread = nullptr;
	}
	
	//等待DealRecvMsgThread结束
	//等待SaveRecvMsgThread结束
	//等待BackupThreadProc结束
	Sleep(2000); 

	POSITION pos1 = m_SinkSocketList.GetHeadPosition();
	POSITION pos2 = pos1;
	while((pos2=pos1) != nullptr)
	{
		CGpsSinkSocket* pSocket = (CGpsSinkSocket*)m_SinkSocketList.GetNext(pos1);
		STRUCT_SERVER_DOWN* lp = new STRUCT_SERVER_DOWN();
		pSocket->SendMessage((char*)lp, sizeof(*lp));
		pSocket->IsTerminated();
	
		m_SinkSocketList.RemoveAt(pos2);

		pSocket->Close();
		delete pSocket;
		pSocket = nullptr;
	}	
	m_SinkSocketList.RemoveAll();

	CComThread::m_mapIdVhc.clear();
	m_vectVehicle.clear();
	
	CVehicleMap::iterator it2 = m_mapOrphanVehicle.begin();
	while(it2 != m_mapOrphanVehicle.end())
	{
		VehicleStatus* pVhcState = (VehicleStatus*)it2->second;
		delete pVhcState;
		it2 ++;
	}
	m_mapOrphanVehicle.clear();

	CloseCenterDatabase(m_connectPtr);
	m_connectPtr = nullptr;
}

// 验证用户名称和密码
bool CChildView::VerifyUser(const CString &strUser, const CString &strPassword , DWORD* dwErr, ADOCG::_ConnectionPtr &pDBConnect, DWORD* dwUserID)
{
	*dwErr = 0;
	bool bReConn = false;

RECONNECT:
	try
	{
		CString szSQLCommand ;
		szSQLCommand.Format("SELECT * FROM [User] Where 监控员名称='%s' AND 监控员密码='%s'", strUser, strPassword);
		TRACE(szSQLCommand);
		_variant_t varRowsAff;
		
		// 1 or 0, excute sql.
		ADOCG::_RecordsetPtr rs = pDBConnect->Execute(LPCTSTR(szSQLCommand), &varRowsAff, ADOCG::adCmdText);
		if(rs->EndofFile)
			return false;
		else
		{
			if(dwUserID != nullptr)
				*dwUserID = long(rs->Fields->Item["ID"]->Value);
			
			return true;
		}
	}	
	catch(_com_error &)
	{
		_bstr_t bstr;
		_variant_t var(short(0));

		if(pDBConnect->Errors->Count > 0)
		{
			bstr = pDBConnect->Errors->Item[var]->Description;
			Log_or_AfxMessageBox(bstr);
		}

		if(OpenCenterDataBase(pDBConnect))
		{
			if(!bReConn)
			{
				bReConn = true;
				goto RECONNECT;
			}
		}
	}	
	
	*dwErr = 0X0006;
    return false;
}


bool CChildView::PutMsgToSMSSendQueue(CMessageWillSend* pMsg, DWORD dwTimeOut)
{
	if(pMsg == nullptr)
		return false;

	if(m_pSMSThread != nullptr)
	{	
		m_pSMSThread->PushSendMsgToQueue(pMsg);
		return true;
	}
	else
	{
		CString string;
		string.Format(_T("由于SMS消息发送线程没有启动，消息 %s 无法发送"),pMsg->m_strData);
		PutMsgToMessageWnd(string);

		delete pMsg;
		pMsg = nullptr;
		return false;
	}
}

bool CChildView::PutMsgToUDPSendQueue(CMessageWillSend* pMsg, DWORD dwTimeOut)
{
	if(pMsg == nullptr)
		return false;

	if(m_pUDPThread != nullptr)
	{	
		m_pUDPThread->PushSendMsgToQueue(pMsg);
		return true;
	}
	else
	{
		CString string;
		string.Format(_T("由于UDP消息发送线程没有启动，消息 %s 无法发送"),pMsg->m_strData);
		PutMsgToMessageWnd(string);

		delete pMsg;
		pMsg = nullptr;
		return false;
	}
}

bool CChildView::PutMsgToTCPSendQueue(CMessageWillSend* pMsg, DWORD dwTimeOut)
{
	if(pMsg == nullptr)
		return false;

	if(m_pTCPThread != nullptr)
	{	
		m_pTCPThread->PushSendMsgToQueue(pMsg);
		return true;
	}
	else
	{
		CString string;
		string.Format(_T("由于TCP消息发送线程没有启动，消息 %s 无法发送"),pMsg->m_strData);
		PutMsgToMessageWnd(string);

		delete pMsg;
		pMsg = nullptr;	
		return false;
	}
}

bool CChildView::PutSendMsgToSaveQueue(CMessageWillSend* pMsg)
{
	if(pMsg != nullptr) 
	{
		CChildView::g_SendMsgSaveQueue.PushMessage((void*)pMsg, sizeof(*pMsg));
	}
	else
		return false;
}

bool CChildView::PutRecvMsgToSaveQueue(CMessageReceived* pMsg)
{
	if(pMsg != nullptr) 
	{
		CChildView::g_RecvMsgSaveQueue.PushMessage((void*)pMsg, sizeof(*pMsg));
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////
void CChildView::AddClient(CGpsSinkSocket *pSocket)
{
	if(pSocket == nullptr) 
		return;

	if(m_pActiveWnd != nullptr)
	{
		if(m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CClientForm)) == TRUE)
		{
			((CClientForm*)m_pActiveWnd)->AddClient(pSocket);
		}
		else if(m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CVehicleForm)) == TRUE)
		{
			((CVehicleForm*)m_pActiveWnd)->AddClient(pSocket);
		}
	}
}

LONG CChildView::OnDeleteClient(UINT WPARAM, LONG LPARAM)					
{
	CGpsSinkSocket* pSocket = (CGpsSinkSocket*)WPARAM;
	if(pSocket == nullptr)
		return 0;

	EnterCriticalSection(&csClientlist);

	if(m_pActiveWnd != nullptr)
	{
		if(m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CClientForm)) == TRUE)
		{
			((CClientForm*)m_pActiveWnd)->RemoveClient(pSocket);
		}
		else if(m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CVehicleForm)) == TRUE)
		{
			((CVehicleForm*)m_pActiveWnd)->RemoveClient(pSocket);
		}
	}

	POSITION pos = m_SinkSocketList.Find(pSocket);
	if(pos != nullptr)
	{
		m_SinkSocketList.RemoveAt(pos);
	}

	pSocket->Close();
	delete pSocket;
	pSocket = nullptr;
	
	LeaveCriticalSection(&csClientlist);

	return 1;
}	


CGpsSinkSocket*CChildView::GetClientSocketByVehicle(const DWORD& dwId)
{
	POSITION pos = m_SinkSocketList.GetHeadPosition();
	while(pos != nullptr)
	{
		CGpsSinkSocket* pSocket = (CGpsSinkSocket*)m_SinkSocketList.GetNext(pos);
		if(pSocket == nullptr || pSocket->IsLogin() == FALSE) 
			continue;
		
		VehicleStatus* pVhc = pSocket->GetVhcStatus(dwId);
		if(pVhc != nullptr) 
		{
			return pSocket;
		}
	}

	return nullptr;
}

CGpsSinkSocket*CChildView::GetClientSocketByUser(LPCTSTR lpszUser)
{
	POSITION pos = m_SinkSocketList.GetHeadPosition();
	while(pos != nullptr)
	{
		CGpsSinkSocket* pSocket = (CGpsSinkSocket*)m_SinkSocketList.GetNext(pos);
		if(pSocket->GetUserName() == CString(lpszUser))
			return pSocket;
	}

	return nullptr;
}

// 返回监控车辆最少的监控员
CGpsSinkSocket* CChildView::GetEasiestClient(DWORD dwVehicle,bool bForce)
{
	CGpsSinkSocket* pSocketMin = nullptr;

	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT USERID FROM View_Vehicle Where ID=%d"),dwVehicle);

		_variant_t varRowsAff;
		ADOCG::_RecordsetPtr rs = m_connectPtr->Execute(LPCTSTR(strSQL), &varRowsAff, ADOCG::adCmdText);
		if(!rs->EndofFile) 
		{
			rs->MoveFirst();
			while(!rs->EndofFile)
			{
				_variant_t var = (rs->Fields->Item[_T("USERID")]->Value);
				DWORD dwUser = long(var);
				
				POSITION pos = m_SinkSocketList.GetHeadPosition();
				while(pos != nullptr)
				{
					CGpsSinkSocket* pSocket = (CGpsSinkSocket*)m_SinkSocketList.GetNext(pos);
					if(pSocket->m_dwUser == dwUser)
					{
						if(pSocketMin == nullptr)
							pSocketMin = pSocket;	
						else if(pSocket->m_mapIdVehicle.size()<pSocketMin->m_mapIdVehicle.size())
							pSocketMin = pSocket;
					}
				}
			
				rs->MoveNext();
			}
		}
	
		rs->Close();
	}	
	catch(CException* ex)
	{
	};

	if(pSocketMin == nullptr && bForce == true)
	{
		POSITION pos = m_SinkSocketList.GetHeadPosition();
		while(pos != nullptr)
		{
			CGpsSinkSocket* pSocket = (CGpsSinkSocket*)m_SinkSocketList.GetNext(pos);
			if(pSocket == nullptr) 
				continue;
			
			if(pSocketMin == nullptr)
				pSocketMin = pSocket;	
			else if(pSocket->m_mapIdVehicle.size()<pSocketMin->m_mapIdVehicle.size())
				pSocketMin = pSocket;	
		}
	}
	
	return pSocketMin;
}

bool CChildView::BroadCastMsgToEveryClient(CMessageReceived& msg, BOOL bForceBroad/*=FALSE*/)
{
	POSITION pos = m_SinkSocketList.GetHeadPosition();
	while(pos != nullptr)
	{
		CGpsSinkSocket* pSocket = (CGpsSinkSocket*)m_SinkSocketList.GetNext(pos);
		if(pSocket->m_mapIdVehicle.find(msg.m_dwVehicle) == pSocket->m_mapIdVehicle.end())
			continue;

		pSocket->SendMsg(&msg);
	}

	return true;
}


void CChildView::PutMsgToMessageWnd(LPCTSTR lpMsg)
{
	if(lpMsg != nullptr) 
	{
		CString* pStr = new CString(lpMsg);
		((CMainFrame*)(AfxGetApp()->GetMainWnd()))->m_wndInBarEdit.AddMsg(pStr);
	}
}

BOOL CChildView::AskInspectOneVehicle(const DWORD& dwId, CGpsSinkSocket* pSocket)
{
	if(pSocket == nullptr) 
		return FALSE;

	CVehicleVect ::iterator it1 = m_vectVehicle.find(dwId);
	if(it1 == m_vectVehicle.end())
		return FALSE;

    SVehicle* pVhc = (SVehicle*)(*it1);
	if(pVhc == nullptr) 
		return FALSE; 

	if(pSocket->GetVhcStatus(pVhc->m_dwId) != nullptr) 
	{
		CString str;
		str.Format(_T("车辆%d已经处于监控状态"), pVhc->m_dwId);
		pSocket->SendStringToClient(pVhc->m_dwId, (LPTSTR)(LPCTSTR)str);
		return FALSE;
	}
	else if(pSocket->AskInspectOneVehicle(pVhc,m_connectPtr) == TRUE)
	{
		VehicleStatus* pVhcStatus = nullptr;

		CVehicleMap::iterator it2 = m_mapOrphanVehicle.find(dwId);
		if(it2 != m_mapOrphanVehicle.end())
		{
			pVhcStatus = (VehicleStatus*)it2->second;
			m_mapOrphanVehicle.erase(it2);
			if(m_pActiveWnd != nullptr && m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CVehicleForm)) == TRUE)
			{
				((CVehicleForm*)m_pActiveWnd)->RemoveOrphan(pVhcStatus);
			}

			pVhcStatus->m_pSocket = pSocket;
		}
		else
		{
			pVhcStatus = new VehicleStatus(pVhc, pSocket);
		}

		pSocket->AddVhcStatus(pVhc->m_dwId, pVhcStatus);

		if(m_pActiveWnd != nullptr && m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CVehicleForm)) == TRUE)
		{
			((CVehicleForm*)m_pActiveWnd)->AddVehicle(pSocket,pVhcStatus);
		}
		else if(m_pActiveWnd != nullptr && m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CClientForm)) == TRUE)
		{
			((CClientForm*)m_pActiveWnd)->ChangeValue(pSocket);
		}
		
		return TRUE;
	}
	else
		return FALSE;
}

void CChildView::ProcessInMsg(CMessageReceived* pMsg,int nMsgCount)
{
//	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();得到的结果不是CMainFrame
//	if(pMainFrame != nullptr)
	{
//		pMainFrame->m_strIndicatorInQueue.Format(_T("IN %d"), nMsgCount);
	}

	switch(pMsg->m_wCommand)
	{
	case GPSSERVER_VEHICLE_MOVE:
		{
			CGpsSinkSocket* pSocket = CChildView::GetClientSocketByVehicle(pMsg->m_dwVehicle);
			if(pSocket == nullptr || pSocket->IsLogin() == FALSE) 
			{
				pSocket = this->GetEasiestClient(pMsg->m_dwVehicle,false);
				if(pSocket != nullptr)
				{
					this->AskInspectOneVehicle(pMsg->m_dwVehicle, pSocket);//若车辆pMsg->m_dwVehicle目前是Orphan，本函数中会被pSocket收养
				}
			}
			
			if(pSocket != nullptr)
			{
				pSocket->SendMsg(pMsg);
				pMsg->m_dwUser = pSocket->GetUserID();
			}
			else // if 车辆pMsg->m_dwVehicle目前不是Orphan
			{
				this->AddOrphanVehicle(pMsg->m_dwVehicle);
			}

			if(m_pActiveWnd != nullptr && m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CVehicleForm)))
			{
				((CVehicleForm*)m_pActiveWnd)->SetInspectedVehicleWarningMark(pMsg->m_dwVehicle, FALSE, pMsg->m_dwStatus, pMsg->m_varRevcTime, STATE_STATE|STATE_TIME);
			}
		}
		break;
	case VEHICLE_PWD_CHANGED:
		{
			CVehicleVect ::iterator it = m_vectVehicle.find(pMsg->m_dwVehicle);
			if(it == m_vectVehicle.end())
				break;

			SVehicle* pVhc = (SVehicle*)(*it);
			if(pVhc != nullptr)
			{
				pVhc->m_strIVUPwd = pMsg->m_strMessage;
				//要修改修改数据库
			}
		}
		break;
	case VEHICLE_MODE_CHANGED:
		{
		}
		break;
	case GPSSERVER_DEALWITH_REPORT:
		{
			CString str;
			str.Format(_T("--> 命令完成信息.ID:%d Result: %s"), pMsg->m_dwVehicle, pMsg->m_strMessage);
			CChildView::PutMsgToMessageWnd(str);

			CGpsSinkSocket* pSocket = CChildView::GetClientSocketByVehicle(pMsg->m_dwVehicle);
			if(pSocket != nullptr)
			{
				pSocket->SendStringToClient(pMsg->m_dwVehicle,(LPTSTR)(LPCTSTR)pMsg->m_strMessage);
			}
		}
		break;
	case GPSSERVER_SEND_FAILED:
		{
			CString str;
			str.Format(_T("--> 命令发送失败.ID:%d Info: %s"), pMsg->m_dwVehicle, pMsg->m_strMessage);
			CChildView::PutMsgToMessageWnd(str);

			CGpsSinkSocket* pSocket = CChildView::GetClientSocketByVehicle(pMsg->m_dwVehicle);
			if(pSocket != nullptr)
			{
				pSocket->SendStringToClient(pMsg->m_dwVehicle,(LPTSTR)(LPCTSTR)pMsg->m_strMessage);
			}
		}
		break;
	default:
		break;
	}

	if(m_pActiveWnd != nullptr && m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CMessageForm)))
	{
		((CMessageForm*)m_pActiveWnd)->AddMessage(pMsg);
	}

	CChildView::PutRecvMsgToSaveQueue(pMsg);
}

void CChildView::FreshVectVehicle()
{
	if(m_connectPtr == nullptr)
		return;

	try
	{
		m_vectVehicle.DirtyFalseAllVhc();

		CString szSQLCommand ;		
		_variant_t varRowsAff;
		szSQLCommand.Format("SELECT * FROM View_Vehicle_Info");
		ADOCG::_RecordsetPtr rs = m_connectPtr->Execute(LPCTSTR(szSQLCommand), &varRowsAff, ADOCG::adCmdText);
		_variant_t var;
		if(rs->EndofFile) 
			return;

		rs->MoveFirst();
		while(!rs->EndofFile)
		{
			SVehicle *pVhcInfo = new SVehicle;
			
			var = (rs->Fields->Item[_T("ID")]->Value);
			pVhcInfo->m_dwId = long(var);
			var = (rs->Fields->Item[_T("姓名")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strDriver = var.bstrVal;
				pVhcInfo->m_strDriver.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("车牌号码")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strCode = var.bstrVal;
				pVhcInfo->m_strCode.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("IVUSIM")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strIVUSIM = var.bstrVal;
				pVhcInfo->m_strIVUSIM.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("IVUSN")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strIVUSN = var.bstrVal;
				pVhcInfo->m_strIVUSN.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("车机密码")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strIVUPwd = var.bstrVal;
				pVhcInfo->m_strIVUPwd.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("类型")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strIVUType = var.bstrVal;
				pVhcInfo->m_strIVUType.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("手机号码")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strOwnerMobile = var.bstrVal;
				pVhcInfo->m_strOwnerMobile.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("座机号码")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strOwnerPhone = var.bstrVal;
				pVhcInfo->m_strOwnerPhone.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("颜色")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strColor = var.bstrVal;
				pVhcInfo->m_strColor.TrimRight(_T(" "));			
			}
			var = (rs->Fields->Item[_T("用途")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strType = var.bstrVal;
				pVhcInfo->m_strType.TrimRight(_T(" "));
			}

			SVehicle* pExistV = m_vectVehicle.GetVehicleInfo(pVhcInfo->m_dwId);
			if(pExistV != nullptr)
			{
				pExistV->m_bDirty = TRUE;
				BOOL bChanged = FALSE;
				if(pExistV->m_strDriver != pVhcInfo->m_strDriver)
				{
					pExistV->m_strDriver = pVhcInfo->m_strDriver;
					bChanged = TRUE;
				}
				if(pExistV->m_strCode != pVhcInfo->m_strCode)
				{
					pExistV->m_strCode = pVhcInfo->m_strCode;
					bChanged = TRUE;
				}
				if(pExistV->m_strIVUSIM != pVhcInfo->m_strIVUSIM)
				{
					pExistV->m_strIVUSIM = pVhcInfo->m_strIVUSIM;
					bChanged = TRUE;
				}
				if(pExistV->m_strIVUSN != pVhcInfo->m_strIVUSN)
				{
					pExistV->m_strIVUSN = pVhcInfo->m_strIVUSN;
					bChanged = TRUE;
				}
				if(pExistV->m_strIVUPwd != pVhcInfo->m_strIVUPwd)
				{
					pExistV->m_strIVUPwd = pVhcInfo->m_strIVUPwd;
					bChanged = TRUE;
				}
				if(pExistV->m_strIVUType != pVhcInfo->m_strIVUType)
				{
					pExistV->m_strIVUType = pVhcInfo->m_strIVUType;
					bChanged = TRUE;
				}
				if(pExistV->m_strOwnerMobile != pVhcInfo->m_strOwnerMobile)
				{
					pExistV->m_strOwnerMobile = pVhcInfo->m_strOwnerMobile;
					bChanged = TRUE;
				}
				if(pExistV->m_strOwnerPhone != pVhcInfo->m_strOwnerPhone)
				{
					pExistV->m_strOwnerPhone = pVhcInfo->m_strOwnerPhone;
					bChanged = TRUE;
				}
				if(pExistV->m_strColor != pVhcInfo->m_strColor)
				{
					pExistV->m_strColor = pVhcInfo->m_strColor;
					bChanged = TRUE;
				}
				if(pExistV->m_strType != pVhcInfo->m_strType)
				{
					pExistV->m_strType = pVhcInfo->m_strType;
					bChanged = TRUE;
				}
				/*
				if(pExistV->m_dateOverdue != pVhcInfo->m_dateOverdue)
				{
					pExistV->m_dateOverdue = pVhcInfo->m_dateOverdue;
				}*/
				if(bChanged)
				{
					CString str;
					str.Format(false ? _T("车辆[%d]信息被修改") : _T("Vechile [%d]'s information has been changed"), pExistV->m_dwId);
					PutMsgToMessageWnd(str);
				}

				delete pVhcInfo;
				pVhcInfo = nullptr;
			}
			else
			{
				pVhcInfo->m_bDirty = TRUE;
				m_vectVehicle.push_back(pVhcInfo);
				
				CString str;
				str.Format(false ? _T("增加新的车辆[%d]记录") : _T("Insert new vechile's record"), pVhcInfo->m_dwId);
				PutMsgToMessageWnd(str);
			}
			rs->MoveNext();
		}	
		
		m_vectVehicle.ClearAllNotDirtyVhc();
		rs->Close();
	}	
	catch(_com_error &)
	{
		_bstr_t bstr;
		_variant_t var(short(0));

		if(m_connectPtr->Errors->Count > 0)
		{
			bstr = m_connectPtr->Errors->Item[var]->Description;
			Log_or_AfxMessageBox(bstr);
		}

		OpenCenterDataBase(m_connectPtr);
	}
	catch(CException* ex)
	{
	};
}

BOOL CChildView::AddOrphanVehicle(DWORD dwVehicle)
{
	CVehicleMap::iterator it1 = m_mapOrphanVehicle.find(dwVehicle);
	if(it1 != m_mapOrphanVehicle.end())
		return FALSE;

	CVehicleVect ::iterator it2 = m_vectVehicle.find(dwVehicle);
	if(it2 == m_vectVehicle.end())
		return FALSE;

	SVehicle* pVhc = (SVehicle*)(*it2);
	if(pVhc != nullptr) 
	{
		VehicleStatus* pVhcStatus = new VehicleStatus(pVhc, nullptr);
		m_mapOrphanVehicle[dwVehicle] = pVhcStatus;		

		if(m_pActiveWnd != nullptr && m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CVehicleForm)))
		{
			((CVehicleForm*)m_pActiveWnd)->AddOrphan(pVhcStatus);
		}

		return TRUE;
	}
	else
		return FALSE;
}