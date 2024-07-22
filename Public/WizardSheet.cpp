#include "stdafx.h"
#include "Wizard.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizard

IMPLEMENT_DYNAMIC(CWizard, CPropertySheet)

	CWizard::CWizard(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage, HBITMAP hbmWatermark, HPALETTE hpalWatermark, HBITMAP hbmHeader, const bool& bHasPreFinishBtn /*=false*/,const bool& bHasHelpIcon /*=false*/)
	: CPropertySheet(nIDCaption, pParentWnd, iSelectPage, hbmWatermark, hpalWatermark, hbmHeader)
{
	// set the WIZARD97 flag so we'll get the new look
	m_psh.dwFlags |= PSH_WIZARD97;
	m_psh.dwFlags |= PSH_WIZARDCONTEXTHELP;
	if (bHasPreFinishBtn)
		m_psh.dwFlags |= PSH_WIZARDHASFINISH;
	m_bHasHelpIcon = bHasHelpIcon;

}

CWizard::CWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage, HBITMAP hbmWatermark, HPALETTE hpalWatermark, HBITMAP hbmHeader, const bool& bHasPreFinishBtn /*=false*/,const bool& bHasHelpIcon /*=false*/)
	: CPropertySheet(pszCaption, pParentWnd, iSelectPage, hbmWatermark, hpalWatermark, hbmHeader)
{
	// set the WIZARD97 flag so we'll get the new look
	m_psh.dwFlags |= PSH_WIZARD97;
	if (bHasPreFinishBtn)
		m_psh.dwFlags |= PSH_WIZARDHASFINISH;
	m_bHasHelpIcon = bHasHelpIcon;
}

BOOL CWizard::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	// Set the icon for cs help.
	if (m_bHasHelpIcon)
		ModifyStyleEx(0, WS_EX_CONTEXTHELP);

	//     // add the minimize button to the window
	//     SetWindowLong( m_hWnd, GWL_STYLE, GetStyle() | WS_MINIMIZEBOX );
	//    // add the minimize command to the system menu
	//    GetSystemMenu( FALSE )->InsertMenu( -1, MF_BYPOSITION | MF_STRING,
	//SC_ICON, "Minimize" );
	return bResult;
}


CWizard::~CWizard()
{
}


BEGIN_MESSAGE_MAP(CWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CWizard)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizard message handlers


