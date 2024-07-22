// Trasvr.h : main header file for the Trasvr application

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

UINT BackupThreadProc(LPVOID lParam);

// CTrasvrApp:
// See Trasvr.cpp for the implementation of this class


class CTrasvrApp : public CWinApp
{
public:
	CTrasvrApp();

public:
	int     m_iActivityFreshTime;
	double  m_dblBckTime;
	CString m_strRun;
	CString m_strVer;

// Overrides

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrasvrApp)
	public:
	virtual BOOL InitInstance() override;
	virtual int ExitInstance() override;
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT) override;
	//}}AFX_VIRTUAL

// Implementation

public:	
	//这一项不能通过就不要运行程序
	bool CheckSysInformation() const;
	//{{AFX_MSG(CTrasvrApp)
	afx_msg void OnAppAbout();
	afx_msg void OnMenuitemHelp();
	afx_msg void OnToolOption();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HANDLE m_hMutextOnlyOne;
};

extern CTrasvrApp theApp;
