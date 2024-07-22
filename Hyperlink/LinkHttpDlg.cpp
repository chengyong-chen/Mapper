#include "stdafx.h"
#include "Link.h"
#include "LinkHttp.h"
#include "LinkHttpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLinkHttpDlg::CLinkHttpDlg(CWnd* pParent /*=nullptr*/, CLinkHttp* plink)
	: CDialog(CLinkHttpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinkHttpDlg)
	m_strHttp = _T("");
	//}}AFX_DATA_INIT
	d_pLink = plink;

	m_strHttp = plink->m_strTarget;
}

void CLinkHttpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkHttpDlg)
	DDX_Text(pDX, IDC_HTTP, m_strHttp);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLinkHttpDlg, CDialog)
	//{{AFX_MSG_MAP(CLinkHttpDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkHttpDlg message handlers

BOOL CLinkHttpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndTop, 10, 63, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	return TRUE;
}

void CLinkHttpDlg::OnOK()
{
	CDialog::OnOK();

	d_pLink->m_strTarget = m_strHttp;
}
