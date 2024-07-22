#include "stdafx.h"
//#include "agrmmccomdlg.h"
#include "ConnectWizComplete.h"
#include "../Public/ODBCDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectWizComplete property page

CConnectWizComplete::CConnectWizComplete(CODBCDatabase& connectioninfo)
	: CPropertyPage(CConnectWizComplete::IDD, 0, IDS_ADOCONNECTWIZ_COMPLETE, 0)
{
	//{{AFX_DATA_INIT(CConnectWizComplete)

	//}}AFX_DATA_INIT

	m_psp.dwFlags |= PSP_HIDEHEADER;
}

CConnectWizComplete::~CConnectWizComplete()
{
}

void CConnectWizComplete::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectWizComplete)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConnectWizComplete, CPropertyPage)
	//{{AFX_MSG_MAP(CConnectWizComplete)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectWizComplete message handlers

BOOL CConnectWizComplete::OnSetActive()
{
	CPropertySheet* psheet = (CPropertySheet*)GetParent();
	psheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);

	return CPropertyPage::OnSetActive();
}
