// ProxyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ProxyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CProxyDlg 对话框

IMPLEMENT_DYNAMIC(CProxyDlg, CDialog)

CProxyDlg::CProxyDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CProxyDlg::IDD, pParent)
{
	m_strServer = (_T("www.china.com"));
	m_nPort = 80;

	m_nNormalWidth = 0;
	m_nExpandedWidth = 0;
	m_bExpanded = FALSE;
}

CProxyDlg::CProxyDlg(CWnd* pParent, CString strServer, int nPort)
	: CDialog(CProxyDlg::IDD, pParent)
{
	m_strServer = strServer;
	m_nPort = nPort;

	m_nNormalWidth = 0;
	m_nExpandedWidth = 0;
	m_bExpanded = FALSE;
}

CProxyDlg::~CProxyDlg()
{
}

void CProxyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SERVER, m_strServer);
	DDX_Text(pDX, IDC_PORT, m_nPort);
}

BEGIN_MESSAGE_MAP(CProxyDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_PROXY, OnSelchangeProxy)
	ON_BN_CLICKED(IDC_SETUP, OnClickedSetup)
END_MESSAGE_MAP()

// CProxyDlg 消息处理程序

BOOL CProxyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_PROXY))->SetCurSel(0);
	OnSelchangeProxy();

	CRect rect1;
	GetWindowRect(rect1);
	m_nExpandedWidth = rect1.Height();

	CRect rect2;
	GetDlgItem(IDC_SETUP)->GetWindowRect(rect2);
	m_nNormalWidth = rect2.bottom+6;

	OnClickedSetup();
	return TRUE; // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CProxyDlg::OnSelchangeProxy()
{
	const int index = ((CComboBox*)GetDlgItem(IDC_PROXY))->GetCurSel();
	switch(index)
	{
	case 0:
	case 3:
	{
		GetDlgItem(IDC_PROXYUSER)->EnableWindow(FALSE);
		GetDlgItem(IDC_PROXYSERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_PROXYPORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_PROXYPWD)->EnableWindow(FALSE);
		GetDlgItem(IDC_PROXYDOMAIN)->EnableWindow(FALSE);
	}
	break;
	default:
	{
		GetDlgItem(IDC_PROXYUSER)->EnableWindow(TRUE);
		GetDlgItem(IDC_PROXYSERVER)->EnableWindow(TRUE);
		GetDlgItem(IDC_PROXYPORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_PROXYPWD)->EnableWindow(TRUE);
		GetDlgItem(IDC_PROXYDOMAIN)->EnableWindow(TRUE);
	}
	break;
	}
}

void CProxyDlg::OnClickedSetup()
{
	HWND hWndChild = ::GetDlgItem(m_hWnd, IDC_SETUP);
	while(hWndChild!=nullptr)
	{
		hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT);
		::EnableWindow(hWndChild, m_bExpanded);
	}

	CRect rect;
	GetWindowRect(rect);
	if(m_bExpanded==FALSE)
	{
		rect.bottom = rect.top+m_nNormalWidth;
		MoveWindow(rect, TRUE);
	}

	else
	{
		rect.bottom = rect.top+m_nExpandedWidth;
		MoveWindow(rect, TRUE);
	}

	m_bExpanded = !m_bExpanded;
}
