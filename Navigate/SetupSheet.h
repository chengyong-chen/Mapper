#pragma once

// SetupSheet.h : header file

#include "SerialPage.h"
#include "ModemPage.h"
#include "SocketPage.h"
#include "SpeedPage.h"
#include "DatabasePage.h"
#include "IconPage.h"
#include "SearchPage.h"

/////////////////////////////////////////////////////////////////////////////
// CNavSetSheet

class __declspec(dllexport) CNavSetSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CNavSetSheet)

	public:
	CNavSetSheet(UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	CNavSetSheet(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

	// Attributes
	public:
	CModemPage m_ModemPage;
	CSerialPage m_SerialPage;
	CSocketPage m_SocketPage;
	CIconPage m_IconPage;
	CSpeedPage m_SpeedPage;
	CSearchPage m_SearchPage;
	CDatabasePage m_DatabasePage;

	// Operations
	public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNavSetSheet)
	//}}AFX_VIRTUAL

	// Implementation
	public:
	~CNavSetSheet() override;

	// Generated message map functions
	protected:
	//{{AFX_MSG(CNavSetSheet)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
