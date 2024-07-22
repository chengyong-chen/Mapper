// Projection.h : main header file for the PROJECT DLL

#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectionApp
// See Project.cpp for the implementation of this class

class CProjectionApp : public CWinApp
{
public:
	CProjectionApp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectionApp)
	 int ExitInstance() override;
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CProjectionApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
