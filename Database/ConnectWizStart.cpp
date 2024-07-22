#include "stdafx.h"
//#include "agrmmccomdlg.h"
#include "ConnectWizStart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectWizStart property page

CConnectWizStart::CConnectWizStart(CODBCDatabase& connectioninfo)
	: CPropertyPage(CConnectWizStart::IDD, 0, IDS_ADOCONNECTWIZ_START, 0)

{
	m_psp.dwFlags |= PSP_HIDEHEADER;
}

CConnectWizStart::~CConnectWizStart()
{
}

void CConnectWizStart::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CConnectWizStart, CPropertyPage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectWizStart message handlers

BOOL CConnectWizStart::OnSetActive()
{
	CPropertySheet* psheet = (CPropertySheet*)GetParent();
	psheet->SetWizardButtons(PSWIZB_NEXT);

	return CPropertyPage::OnSetActive();
}
