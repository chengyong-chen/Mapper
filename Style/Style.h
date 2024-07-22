#pragma once
#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

class CStyleApp : public CWinApp
{
public:
	CStyleApp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStyleApp)
public:
	BOOL InitInstance() override;
	int ExitInstance() override;

	//}}AFX_VIRTUAL

	//{{AFX_MSG(CStyleApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};