// SetupSheet.cpp : implementation file
//

#include "stdafx.h"
#include "SetupSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNavSetSheet

IMPLEMENT_DYNAMIC(CNavSetSheet, CPropertySheet)

CNavSetSheet::CNavSetSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	const CString strPort = AfxGetApp()->GetProfileString(_T("Navigate"), _T("Port"), _T("GPS Receiver"));
	if(strPort==_T("Socket"))
	{
		AddPage(&m_SocketPage);
		AddPage(&m_DatabasePage);
	}
	else if(strPort==_T("GPS Receiver"))
	{
		AddPage(&m_SerialPage);
		AddPage(&m_SearchPage);
	}
	else if(strPort==_T("Comsms"))
	{
		AddPage(&m_SerialPage);
		AddPage(&m_DatabasePage);
	}

	AddPage(&m_SpeedPage);
	AddPage(&m_IconPage);
}

CNavSetSheet::CNavSetSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	const CString strPort = AfxGetApp()->GetProfileString(_T("Navigate"), _T("Port"), _T("GPS Receiver"));
	if(strPort==_T("Socket"))
	{
		AddPage(&m_SocketPage);
		AddPage(&m_DatabasePage);
	}
	else if(strPort==_T("GPS Receiver"))
	{
		AddPage(&m_SerialPage);
		AddPage(&m_SearchPage);
	}
	else if(strPort==_T("Comsms"))
	{
		AddPage(&m_SerialPage);
		AddPage(&m_DatabasePage);
	}
	else if(strPort==_T("Modem"))
	{
		AddPage(&m_ModemPage);
		AddPage(&m_DatabasePage);
	}

	AddPage(&m_SpeedPage);
	AddPage(&m_IconPage);
}

CNavSetSheet::~CNavSetSheet()
{
}

BEGIN_MESSAGE_MAP(CNavSetSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CNavSetSheet)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNavSetSheet message handlers
