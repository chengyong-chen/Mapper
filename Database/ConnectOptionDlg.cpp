#include "stdafx.h"
//#include "agrmmccomdlg.h"
#include "ConnectOptionDlg.h"
#include "../Public/ODBCDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectOptionDlg dialog

CConnectOptionDlg::CConnectOptionDlg(CODBCDatabase& connectioninfo)
	: CDialog(CConnectOptionDlg::IDD, nullptr)
{
	//{{AFX_DATA_INIT(CConnectOptionDlg)
	m_cOptionString = _T("");
	//}}AFX_DATA_INIT
}

void CConnectOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectOptionDlg)
	DDX_Text(pDX, IDC_ED_OPTIONS, m_cOptionString);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConnectOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectOptionDlg)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectOptionDlg message handlers
