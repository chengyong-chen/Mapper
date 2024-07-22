#include "stdafx.h"
#include "Mapsvr.h"
#include "MapsvrDlg.h"
#include "Global.h"

#include "ClientDlg.h"
#include "..\Coding\Instruction.h"
#include "..\Socket\ListenSocket.h"
#include "..\Socket\MapSinkSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapsvrDlg dialog

IMapdcom CMapsvrDlg::m_mapdocm;

CMapsvrDlg::CMapsvrDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CMapsvrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapsvrDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pListenSocket = nullptr;
	m_ClientDlg = nullptr;

	InitializeCriticalSection(&csClientlist);
	InitializeCriticalSection(&csClientDlg);
}

void CMapsvrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapsvrDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMapsvrDlg, CDialog)
	//{{AFX_MSG_MAP(CMapsvrDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapsvrDlg message handlers

BOOL CMapsvrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if(m_ClientDlg == nullptr)
	{
		m_ClientDlg = new CClientDlg((CWnd*)this);
		if(m_ClientDlg->Create(IDD_CLIENT, (CWnd*)this) == TRUE)
		{
			m_ClientDlg->ShowWindow(SW_SHOWNORMAL);
			m_ClientDlg->SetWindowPos(&wndTop, 0, 60, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
		}
		else
		{
			delete m_ClientDlg;
			m_ClientDlg = nullptr;
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}



// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMapsvrDlg::OnPaint() 
{
	if(IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1)/2;
		int y = (rect.Height() - cyIcon + 1)/2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMapsvrDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMapsvrDlg::OnStart() 
{
	if(m_pListenSocket == nullptr)
	{
		m_pListenSocket = new CListenSocket();
		m_pListenSocket->SetCallbackFunction(AcptClient);
		m_pListenSocket->SetParent((DWORD)this);
		if(m_pListenSocket->StartListen(1505) == TRUE)
		{
			GetDlgItem(IDC_PORT)->SetWindowText(_T("1505"));

			WSADATA wsaData;
			if(WSAStartup(0x202,&wsaData) == SOCKET_ERROR) 
			{
				char sz[256];
				sprintf(sz, "Create socket failed (Code %i).\n", GetLastError());
				AfxMessageBox(sz);

				WSACleanup();
				return;
			}		

			char szHostName[256];
			if(gethostname(szHostName,128) != 0)
			{
				char sz[256];
				sprintf(sz, "Gethostname failed (Code %i).\n", GetLastError());
				AfxMessageBox(sz);
			}
			
			LPHOSTENT pHost = gethostbyname(szHostName);
			if(pHost != nullptr)
			{
				struct in_addr* ptr = (struct in_addr*)pHost->h_addr_list[0];
				int a = ptr->S_un.S_un_b.s_b1;
				int b = ptr->S_un.S_un_b.s_b2;
				int c = ptr->S_un.S_un_b.s_b3;
				int d = ptr->S_un.S_un_b.s_b4;
			
				CString strTmp;
				strTmp.Format("%d.%d.%d.%d",a,b,c,d);
				GetDlgItem(IDC_IPADDRESS)->SetWindowText(strTmp);
			}
			else
			{
				char sz[256];
				sprintf(sz, "Gethostbyname failed (Code %i).\n", GetLastError());
				AfxMessageBox(sz);
			}
		}
		else
		{
			CString strMsg;
			strMsg.Format(_T("�������˿�%d����ʧ�ܣ�ϵͳ���˳�!"), 1505);
			AfxMessageBox(strMsg);
			PostQuitMessage(0);
		}
	}
	else
	{
		EnterCriticalSection(&csClientlist);
		POSITION pos = m_MapSinkList.GetHeadPosition();
		while(pos != nullptr)
		{
			CMapSinkSocket* pSocket = (CMapSinkSocket*)m_MapSinkList.GetNext(pos);
			if(pSocket != nullptr) 
			{
			//	pSocket->SendMessage();

				RemoveClient(pSocket);
			}
		}
		m_MapSinkList.RemoveAll();
		LeaveCriticalSection(&csClientlist);
		
		m_pListenSocket->StopListen();
//		m_pListenSocket->Close();
		delete m_pListenSocket;
		m_pListenSocket = nullptr;
	}
}

BOOL CMapsvrDlg::DestroyWindow() 
{
	EnterCriticalSection(&csClientlist);
	POSITION pos = m_MapSinkList.GetHeadPosition();
	while(pos != nullptr)
	{
		CMapSinkSocket* pSocket = (CMapSinkSocket*)m_MapSinkList.GetNext(pos);
		if(pSocket != nullptr) 
		{
			STRUCT_SERVER_DOWN* lp = new STRUCT_SERVER_DOWN();
			pSocket->SendMessage((TCHAR*)lp, sizeof(*lp));
			pSocket->IsTerminated();

		//	pSocket->SendMessage();
		
			pSocket->DisConnect();
		//	pSocket->Close();
			delete pSocket;
			pSocket = nullptr;
		}
	}
	m_MapSinkList.RemoveAll();
	LeaveCriticalSection(&csClientlist);

	if(m_pListenSocket != nullptr) 		
	{
		m_pListenSocket->StopListen();
		delete m_pListenSocket;
		m_pListenSocket = nullptr;
	}

	EnterCriticalSection(&csClientDlg);
	if(m_ClientDlg != nullptr)
	{
		m_ClientDlg->SendMessage(WM_DESTROY,0,0);
		delete m_ClientDlg;
		m_ClientDlg = nullptr;
	}	
	LeaveCriticalSection(&csClientDlg);

	DeleteCriticalSection(&csClientlist);
	DeleteCriticalSection(&csClientDlg);
	
	return CDialog::DestroyWindow();
}

void CMapsvrDlg::InsertClient(CMapSinkSocket* pSocket) 
{	
	EnterCriticalSection(&csClientDlg);

	if(m_ClientDlg != nullptr)
	{
		m_ClientDlg->InsertClient(pSocket);
	}		

	int nCount = m_MapSinkList.GetCount();
	CString strUserCount;
	strUserCount.Format(_T("%d"),nCount);
	GetDlgItem(IDC_USERCOUNT)->SetWindowText(strUserCount);

	LeaveCriticalSection(&csClientDlg);
}

void CMapsvrDlg::RemoveClient(CMapSinkSocket *pSocket)					
{
	EnterCriticalSection(&csClientlist);
	POSITION pos = m_MapSinkList.Find(pSocket);
	if(pos != nullptr)
	{
		m_MapSinkList.RemoveAt(pos);
	}
	int nCount = m_MapSinkList.GetCount();
	LeaveCriticalSection(&csClientlist);

	EnterCriticalSection(&csClientDlg);
	if(m_ClientDlg != nullptr)
	{
		m_ClientDlg->RemoveClient(pSocket);
	}
	CString strUserCount;
	strUserCount.Format(_T("%d"),nCount);
	GetDlgItem(IDC_USERCOUNT)->SetWindowText(strUserCount);
	LeaveCriticalSection(&csClientDlg);
}