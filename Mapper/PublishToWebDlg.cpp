#include "stdafx.h"
#include "PublishToWebDlg.h"
#include "afxdialogex.h"
#include "Resource.h"


IMPLEMENT_DYNAMIC(CPublishToWebDlg, CDialogEx)

CPublishToWebDlg::CPublishToWebDlg(CWnd* pParent, CString strTitle, CString strDescription, BYTE minMapLevel, BYTE maxMapLevel, bool partialable)
	: CDialogEx(IDD_PUBLISHTOWEB, pParent), m_minMapLevel(minMapLevel), m_maxMapLevel(maxMapLevel),m_partialable(partialable)
	, m_strName(strTitle)
	, m_strDescription(strDescription)
	,m_minLevel(minMapLevel)
	,m_maxLevel(maxMapLevel),
	m_bExportProperty(FALSE),
	m_bPartial(FALSE)
{
	m_strAPIUrl = AfxGetApp()->GetProfileString(_T("PublishToWeb"), _T("APIUrl"), _T("http://api.diwatu.com/Map.svc/"));
	m_strClient = AfxGetApp()->GetProfileString(_T("PublishToWeb"), _T("Client"), _T(""));
	m_strUsername = AfxGetApp()->GetProfileString(_T("PublishToWeb"), _T("Username"), _T("Admin"));
	m_strPassword = AfxGetApp()->GetProfileString(_T("PublishToWeb"), _T("Password"), _T(""));
}

CPublishToWebDlg::~CPublishToWebDlg()
{
}

void CPublishToWebDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_EXPORTPROPERTY, m_bExportProperty);
	DDX_Text(pDX, IDC_EDIT5, m_strName);
	DDX_Text(pDX, IDC_EDIT6, m_strDescription);
	DDX_Text(pDX, IDC_EDIT1, m_strAPIUrl);
	DDX_Text(pDX, IDC_EDIT2, m_strClient);
	DDX_Text(pDX, IDC_EDIT3, m_strUsername);
	DDX_Text(pDX, IDC_EDIT4, m_strPassword);
	DDX_Text(pDX, IDC_LEVELMIN, m_minLevel);
	DDX_Text(pDX, IDC_LEVELMAX, m_maxLevel);
	DDV_MinMaxByte(pDX, m_minLevel, m_minMapLevel, m_maxMapLevel);
	DDV_MinMaxByte(pDX, m_maxLevel, m_minMapLevel, m_maxMapLevel);
	DDX_Check(pDX, IDC_PARTIAL, m_bPartial);
}


BEGIN_MESSAGE_MAP(CPublishToWebDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPublishToWebDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_LOCAL, &CPublishToWebDlg::OnBnClickedLocal)
	ON_BN_CLICKED(IDC_UPDATE, &CPublishToWebDlg::OnBnClickedUpdate)
	ON_EN_CHANGE(IDC_WEBUID, &CPublishToWebDlg::OnBnWebUidChanged)
END_MESSAGE_MAP()

BOOL CPublishToWebDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if(m_partialable == false)
	{
		GetDlgItem(IDC_PARTIAL)->ShowWindow(FALSE);
	}
	return TRUE;
}
void CPublishToWebDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();

	AfxGetApp()->WriteProfileString(_T("PublishToWeb"), _T("APIUrl"), m_strAPIUrl);
	AfxGetApp()->WriteProfileString(_T("PublishToWeb"), _T("Client"), m_strClient);
	AfxGetApp()->WriteProfileString(_T("PublishToWeb"), _T("Username"), m_strUsername);
	AfxGetApp()->WriteProfileString(_T("PublishToWeb"), _T("Password"), m_strPassword);

	m_bUpdate = false;
}
void CPublishToWebDlg::OnBnClickedLocal()
{
	CPublishToWebDlg::OnBnClickedOk();

	EndDialog(IDC_LOCAL);
}
void CPublishToWebDlg::OnBnClickedUpdate()
{
	CDialogEx::OnOK();

	AfxGetApp()->WriteProfileString(_T("PublishToWeb"), _T("APIUrl"), m_strAPIUrl);
	AfxGetApp()->WriteProfileString(_T("PublishToWeb"), _T("Client"), m_strClient);
	AfxGetApp()->WriteProfileString(_T("PublishToWeb"), _T("Username"), m_strUsername);
	AfxGetApp()->WriteProfileString(_T("PublishToWeb"), _T("Password"), m_strPassword);

	m_bUpdate = true;
}

void CPublishToWebDlg::OnBnWebUidChanged()
{
	UpdateData();
}