#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

class CMakerApp : public CWinAppEx
{
	ULONG_PTR m_gdiplusToken;
public:
	CMakerApp();

private:
	void ConvertFile(CString strFolder);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMakerApp)
public:
	BOOL InitInstance() override;
	int ExitInstance() override;
	int Run() override;
	//}}AFX_VIRTUAL

	// Implementation
	//{{AFX_MSG(CMakerApp)
	afx_msg void OnAppAbout();
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CMakerApp theApp;
