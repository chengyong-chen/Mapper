#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

class CInformationApp : public CWinApp
{
public:
	CInformationApp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInformationApp)
public:
	BOOL InitInstance() override;
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CInformationApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
