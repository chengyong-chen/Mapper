// Coding.h : main header file for the CODING DLL

#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCodingApp
// See Coding.cpp for the implementation of this class

class CCodingApp : public CWinApp
{
public:
	CCodingApp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCodingApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCodingApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
