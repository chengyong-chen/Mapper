#include "stdafx.h"
#include "Link.h"

#include "LinkEMail.h"
#include "LinkEMailDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLinkEMailDlg::CLinkEMailDlg(CWnd* pParent /*=nullptr*/, CLinkEMail* plink)
	: CDialog(CLinkEMailDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinkEMailDlg)
	m_strEMail = _T("");
	//}}AFX_DATA_INIT
	d_pLink = plink;

	m_strEMail = plink->m_strTarget;
}

void CLinkEMailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkEMailDlg)
	DDX_Text(pDX, IDC_EMAIL, m_strEMail);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLinkEMailDlg, CDialog)
	//{{AFX_MSG_MAP(CLinkEMailDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkEMailDlg message handlers

BOOL CLinkEMailDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	SetWindowPos(&wndTop, 10, 63, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	return TRUE;
}

void CLinkEMailDlg::OnOK()
{
	CDialog::OnOK();

	d_pLink->m_strTarget = m_strEMail;
}
