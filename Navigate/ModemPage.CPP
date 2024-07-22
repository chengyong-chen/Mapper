#include "stdafx.h"
#include "ModemPage.h"
#include "ModemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModemPage dialog

CModemPage::CModemPage(CWnd* pParent)
	: CPropertyPage(CModemPage::IDD)
{
	//{{AFX_DATA_INIT(CModemPage)
	m_nPort = 0;
	m_strPassword = _T("");
	m_strTelephone = _T("");
	m_strUser = _T("");
	m_strZone = _T("");
	m_bIVU = FALSE;
	//}}AFX_DATA_INIT

	m_dwModem = AfxGetApp()->GetProfileInt(_T("Modem"), _T("Index"), 0);
	m_strZone = AfxGetApp()->GetProfileString(_T("Modem"), _T("Zone"), nullptr);
	m_strTelephone = AfxGetApp()->GetProfileString(_T("Modem"), _T("Telephone"), nullptr);
	m_strUser = AfxGetApp()->GetProfileString(_T("Modem"), _T("User"), nullptr);;

	m_nPort = AfxGetApp()->GetProfileInt(_T("Modem"), _T("Port"), 3);
	m_dwBaudRate = AfxGetApp()->GetProfileInt(_T("Modem"), _T("BaudRate"), 4800);
}

void CModemPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemPage)	
	DDX_Control(pDX, IDC_MODEM, m_ctrlModem);
	DDX_Text(pDX, IDC_PORT, m_nPort);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_TELEPHONE, m_strTelephone);
	DDX_Text(pDX, IDC_USER, m_strUser);
	DDX_Text(pDX, IDC_ZONE, m_strZone);
	DDX_Check(pDX, IDC_IVU, m_bIVU);
	//}}AFX_DATA_MAP

	DDX_CBData(pDX, IDC_BAUDRATE, m_dwBaudRate);
}

BEGIN_MESSAGE_MAP(CModemPage, CPropertyPage)
	//{{AFX_MSG_MAP(CModemPage)
	ON_CBN_SELCHANGE(IDC_MODEM, OnSelchangeModem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CModemPage::DDX_CBData(CDataExchange* pDX, int nIDC, DWORD& dwData)
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

BOOL CModemPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	const CModemInfo modemInfo;
	if(modemInfo.GetModemCount()<1)
	{
		AfxMessageBox(_T("你没有安装调制解调器！"));
		//		return FALSE;
	}

	modemInfo.FillComboBoxWithModemNames(&m_ctrlModem);
	m_ctrlModem.SetCurSel(m_dwModem);
	OnSelchangeModem();

	SetWindowPos(&wndTop, 10, 50, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);

	return TRUE;
}

void CModemPage::OnSelchangeModem()
{
	m_dwModem = m_ctrlModem.GetCurSel();
	const CModemInfo modemInfo;
	const CString strModem = modemInfo.GetModemName(m_dwModem);
	m_nPort = modemInfo.GetModemComPort(m_dwModem);
	AfxGetApp()->WriteProfileInt(_T("Modem"), _T("Port"), m_nPort);

	((CComboBox*)GetDlgItem(IDC_BAUDRATE))->ResetContent();
	const HWND hWndCtrl = ::GetDlgItem(GetSafeHwnd(), IDC_BAUDRATE);;
	int nIndex;
	const DWORD dwMaxBaudRate = modemInfo.GetModemBps(strModem);
	if(dwMaxBaudRate>=2400)
	{
		nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("2400"));
		::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 2400);
	}
	if(dwMaxBaudRate>=4800)
	{
		nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("4800"));
		::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 4800);
	}
	if(dwMaxBaudRate>=9600)
	{
		nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("9600"));
		::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 9600);
	}
	if(dwMaxBaudRate>=14400)
	{
		nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("14400"));
		::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 14400);
	}
	if(dwMaxBaudRate>=19200)
	{
		nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("19200"));
		::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 19200);
	}
	if(dwMaxBaudRate>=38400)
	{
		nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("38400"));
		::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 38400);
	}
	if(dwMaxBaudRate>=57600)
	{
		nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("57600"));
		::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 57600);
	}
	if(dwMaxBaudRate>=115200)
	{
		nIndex = ::SendMessage(hWndCtrl, CB_ADDSTRING, 0, (LPARAM)_T("115200"));
		::SendMessage(hWndCtrl, CB_SETITEMDATA, nIndex, 115200);
	}

	UpdateData(FALSE);
}

void CModemPage::OnOK()
{
	CPropertyPage::OnOK();

	AfxGetApp()->WriteProfileString(_T("Navigate"), _T("Port"), _T("Modem"));

	AfxGetApp()->WriteProfileInt(_T("Modem"), _T("BaudRate"), m_dwBaudRate);
	AfxGetApp()->WriteProfileInt(_T("Modem"), _T("Index"), m_dwModem);
	AfxGetApp()->WriteProfileString(_T("Modem"), _T("Zone"), m_strZone);
	AfxGetApp()->WriteProfileString(_T("Modem"), _T("Telephone"), m_strTelephone);
	AfxGetApp()->WriteProfileString(_T("Modem"), _T("User"), m_strUser);
	AfxGetApp()->WriteProfileString(_T("Modem"), _T("Password"), m_strPassword);

	AfxGetApp()->WriteProfileInt(_T("Modem"), _T("IVU"), m_bIVU);
}
