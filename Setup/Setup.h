// Setup.h : main header file for the SETUP application






#pragma once


#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "SingleInstanceApp.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupApp:
// See Setup.cpp for the implementation of this class


class CSetupApp : public CSingleInstanceApp
{
public:
	CSetupApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupApp)
	public:
	virtual BOOL InitInstance() override;
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSetupApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CSetupApp theApp;
/////////////////////////////////////////////////////////////////////////////





