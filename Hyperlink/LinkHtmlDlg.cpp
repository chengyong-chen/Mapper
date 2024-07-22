#include "stdafx.h"
#include "Link.h"
#include "LinkHtml.h"
#include "LinkHtmlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLinkHtmlDlg::CLinkHtmlDlg(CWnd* pParent /*=nullptr*/, CLinkHtml* plink)
	: CDialog(CLinkHtmlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinkHtmlDlg)
	m_strHtml = _T("");
	//}}AFX_DATA_INIT
	d_pLink = plink;

	m_strHtml = plink->m_strTarget;
}

void CLinkHtmlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkHtmlDlg)
	DDX_Text(pDX, IDC_HTML, m_strHtml);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLinkHtmlDlg, CDialog)
	//{{AFX_MSG_MAP(CLinkHtmlDlg)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkHtmlDlg message handlers

BOOL CLinkHtmlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndTop, 10, 63, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	return TRUE;
}

void CLinkHtmlDlg::OnOK()
{
	CDialog::OnOK();

	d_pLink->m_strTarget = m_strHtml;
}

void CLinkHtmlDlg::OnSearch()
{
	TCHAR CurrentDir[256];
	GetCurrentDirectory(255, CurrentDir);

	CString strPath = AfxGetApp()->GetProfileString(_T(""), _T("Link Html Path"), nullptr);
	SetCurrentDirectory(strPath);

	CFileDialog dlg(TRUE,
		nullptr,
		nullptr,
		OFN_HIDEREADONLY,
		_T("Html Files(*.htm,*.html)|*.htm;*.html;*.htm;||"),
		this);

	if(dlg.DoModal()==IDOK)
	{
		strPath = dlg.GetPathName();

		((CStatic*)GetDlgItem(IDC_HTML))->SetWindowText(strPath);
		m_strHtml = strPath;

		strPath = strPath.Left(strPath.ReverseFind(_T('\\'))+1);
		AfxGetApp()->WriteProfileString(_T(""), _T("Link Html Path"), strPath);
	}

	SetCurrentDirectory(CurrentDir);
}
