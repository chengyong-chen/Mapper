// Fast.h : main header file for the FAST application





#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CFastApp:
// See Fast.cpp for the implementation of this class


class CFastApp : public CWinApp
{
	ULONG_PTR m_gdiplusToken;
public:
	CFastApp();

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CFastApp)
public:
	virtual BOOL InitInstance() override;
	virtual int ExitInstance() override;
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFastApp)
	afx_msg void OnAppAbout();
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////


// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


