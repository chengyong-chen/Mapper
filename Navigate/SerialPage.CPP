#include "stdafx.h"
#include "SerialPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSerialPage::CSerialPage() : CPropertyPage(CSerialPage::IDD)
{
	//{{AFX_DATA_INIT(CSerialPage)
	//}}AFX_DATA_INIT
	m_dwBaudRate = AfxGetApp()->GetProfileInt(_T("Gps Receiver"), _T("BaudRate"), 4800);
	m_dwDataBits = AfxGetApp()->GetProfileInt(_T("Gps Receiver"), _T("DataBits"), 8);
	m_dwStopBits = AfxGetApp()->GetProfileInt(_T("Gps Receiver"), _T("StopBits"), 0);
	m_dwParity = AfxGetApp()->GetProfileInt(_T("Gps Receiver"), _T("Parity"), _T('N'));
}

CSerialPage::~CSerialPage()
{
}

void CSerialPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSerialPage)	
	DDX_Control(pDX, IDC_PORT, m_ctrlPort);
	//}}AFX_DATA_MAP

	DDX_CBData(pDX, IDC_BAUDRATE, m_dwBaudRate);
	DDX_CBData(pDX, IDC_DATABITS, m_dwDataBits);
	DDX_CBData(pDX, IDC_PARITY, m_dwParity);
	DDX_CBData(pDX, IDC_STOPBITS, m_dwStopBits);
}

BEGIN_MESSAGE_MAP(CSerialPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSerialPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSerialPage::DDX_CBData(CDataExchange* pDX, int nIDC, DWORD& dwData)
{
	const HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	if(pDX->m_bSaveAndValidate)
	{
		const int nSelected = ::SendMessage(hWndCtrl, CB_GETCURSEL, 0, 0L);
		dwData = ::SendMessage(hWndCtrl, CB_GETITEMDATA, nSelected, 0L);
	}
	else
	{
		const int nCount = ::SendMessage(hWndCtrl, CB_GETCOUNT, 0, 0L);
		for(int i = 0; i<nCount; i++)
		{
			const DWORD dwItemData = ::SendMessage(hWndCtrl, CB_GETITEMDATA, i, 0L);
			if(dwItemData==dwData)
			{
				::SendMessage(hWndCtrl, CB_SETCURSEL, i, 0L);
				break;
			}
		}
	}
}

BOOL CSerialPage::OnInitDialog()
{
	HWND hWndCtrl;
	int nIndex;

	hWndCtrl = ::GetDlgItem(GetSafeHwnd(), IDC_BAUDRATE);
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("2400"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 2400);
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("4800"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 4800);
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("9600"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 9600);
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("14400"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 14400);
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("19200"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 19200);
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("38400"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 38400);
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("57600"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 57600);
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("115200"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 115200);

	hWndCtrl = ::GetDlgItem(GetSafeHwnd(), IDC_DATABITS);
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("7"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 7);
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("8"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 8);

	hWndCtrl = ::GetDlgItem(GetSafeHwnd(), IDC_PARITY);
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("Even"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 'E');
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("Odd"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 'O');
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("None"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 'N');

	hWndCtrl = ::GetDlgItem(GetSafeHwnd(), IDC_STOPBITS);
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("1"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 0);
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("1.5"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 1);
	nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("2"));
	::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 2);

	CPropertyPage::OnInitDialog();

	m_ctrlPort.ResetContent();
	for(int nPort = 1; nPort<16; nPort++)
	{
		CString strPort;
		strPort.Format(_T("COM%d"), nPort);
		const HANDLE hFile = ::CreateFile(strPort,
			GENERIC_READ|GENERIC_WRITE,
			0,
			0,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			0);

		if(hFile==INVALID_HANDLE_VALUE)
		{
			continue;
		}
		::CloseHandle(hFile);
		const int index = m_ctrlPort.AddString(strPort);
		m_ctrlPort.SetItemData(index, nPort);
	}
	const int nPort = AfxGetApp()->GetProfileInt(_T("Gps Receiver"), _T("Port"), 1);
	const int nCount = m_ctrlPort.GetCount();
	if(nCount==0)
	{
		AfxMessageBox(_T("无可用端口或已有端口已被其它程序打开！"));
		return FALSE;
	}
	else
	{
		for(int index = 1; index<=nCount; index++)
		{
			const int port = m_ctrlPort.GetItemData(index-1);
			if(port==nPort)
			{
				m_ctrlPort.SetCurSel(index-1);
				break;
			}
		}
	}

	SetWindowPos(&wndTop, 10, 57, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);

	return TRUE;
}

void CSerialPage::OnOK()
{
	// TODO: Add extra validation here
	CPropertyPage::OnOK();

	AfxGetApp()->WriteProfileString(_T("Navigate"), _T("Port"), _T("GPS Receiver"));
	const int index = m_ctrlPort.GetCurSel();
	if(index!=LB_ERR)
	{
		const DWORD nPort = m_ctrlPort.GetItemData(index);
		AfxGetApp()->WriteProfileInt(_T("Gps Receiver"), _T("Port"), nPort);
	}

	AfxGetApp()->WriteProfileInt(_T("Gps Receiver"), _T("BaudRate"), m_dwBaudRate);
	AfxGetApp()->WriteProfileInt(_T("Gps Receiver"), _T("DataBits"), m_dwDataBits);
	AfxGetApp()->WriteProfileInt(_T("Gps Receiver"), _T("StopBits"), m_dwStopBits);
	AfxGetApp()->WriteProfileInt(_T("Gps Receiver"), _T("Parity"), m_dwParity);
}
