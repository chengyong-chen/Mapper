#include "stdafx.h"
#include "PickLayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CPickLayDlg dialog

IMPLEMENT_DYNAMIC(CPickLayDlg, CDialog)

CPickLayDlg::CPickLayDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CPickLayDlg::IDD, pParent)
{
	m_nType = 0;
}

CPickLayDlg::~CPickLayDlg()
{
}

void CPickLayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPickLayDlg, CDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPickLayDlg message handlers
BOOL CPickLayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_strTitle);
	((CButton*)GetDlgItem(IDC_RADIODBPOLY))->SetCheck(TRUE);

	return TRUE;
}

void CPickLayDlg::OnOK()
{
	CDialog::OnOK();

	if(((CButton*)GetDlgItem(IDC_RADIODBPOINT))->GetState()==TRUE) m_nType = 1;
	if(((CButton*)GetDlgItem(IDC_RADIODBPOLY))->GetState()==TRUE) m_nType = 2;
	if(((CButton*)GetDlgItem(IDC_RADIODBPOIPT))->GetState()==TRUE) m_nType = 3;
}
