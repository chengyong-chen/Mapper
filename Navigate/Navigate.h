// Navigate.h : main header file for the NAVIGATION DLL

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

/////////////////////////////////////////////////////////////////////////////
// CNavigateApp
// See Navigate.cpp for the implementation of this class

class CNavigateApp : public CWinApp
{
	public:
	CNavigateApp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNavigateApp)
	public:
	BOOL InitInstance() override;
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CNavigateApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
