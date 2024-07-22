// MyPropertySheet.h : header file

// This class defines custom modal property sheet 
// COptionSheet.
 
#pragma once

#include "OptionPage.h"

/////////////////////////////////////////////////////////////////////////////
// COptionSheet

class COptionSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(COptionSheet)

// Construction
public:
	COptionSheet(CWnd* pWndParent = nullptr);

// Attributes
public:
	COptionPage1 m_Page1;
	COptionPage2 m_Page2;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COptionSheet();

// Generated message map functions
protected:
	//{{AFX_MSG(COptionSheet)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};