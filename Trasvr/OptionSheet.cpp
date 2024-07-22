// MyPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionSheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionSheet

IMPLEMENT_DYNAMIC(COptionSheet, CPropertySheet)

COptionSheet::COptionSheet(CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().

	AddPage(&m_Page1);
	AddPage(&m_Page2);
}

COptionSheet::~COptionSheet()
{
}


BEGIN_MESSAGE_MAP(COptionSheet, CPropertySheet)
	//{{AFX_MSG_MAP(COptionSheet)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COptionSheet message handlers



BOOL COptionSheet::OnEraseBkgnd(CDC* pDC) 
{
	CRect rc;
	GetClientRect(rc);
	CBrush br(RGB(160,180,220));
	pDC->FillRect(&rc, &br); 
	
	return TRUE;
}

int COptionSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetWindowText(_T("СЎПо"));
	
	return 0;
}
