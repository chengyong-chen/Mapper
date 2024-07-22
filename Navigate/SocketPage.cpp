// SocketDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SocketPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSocketPage dialog

CSocketPage::CSocketPage() : CPropertyPage(CSocketPage::IDD)
{
	//{{AFX_DATA_INIT(CSocketPage)
	m_nPort = 0;
	m_strServer = _T("");
	//}}AFX_DATA_INIT

	m_strServer = AfxGetApp()->GetProfileString(_T("GPSServer"), _T("Server"), _T("localhost"));
	m_nPort = AfxGetApp()->GetProfileInt(_T("GPSServer"), _T("Port"), 1500);
}

CSocketPage::~CSocketPage()
{
}

void CSocketPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSocketPage)
	DDX_Text(pDX, IDC_PORT, m_nPort);
	DDX_Text(pDX, IDC_SERVER, m_strServer);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSocketPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSocketPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSocketPage message handlers

void CSocketPage::OnOK()
{
	CPropertyPage::OnOK();

	AfxGetApp()->WriteProfileString(_T("Navigate"), _T("Port"), _T("Socket"));

	AfxGetApp()->WriteProfileString(_T("GPSServer"), _T("Server"), m_strServer);
	AfxGetApp()->WriteProfileInt(_T("GPSServer"), _T("Port"), m_nPort);
}
