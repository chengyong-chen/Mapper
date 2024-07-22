// Regtool.h : main header file for the REGTOOL application






#pragma once


#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRegtoolApp:
// See Regtool.cpp for the implementation of this class


class CRegtoolApp : public CWinApp
{
public:
	CRegtoolApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegtoolApp)
	public:
	virtual BOOL InitInstance() override;
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRegtoolApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////





