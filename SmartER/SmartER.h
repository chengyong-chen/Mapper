#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


class CSmartERApp : public CWinAppEx
{
	ULONG_PTR m_gdiplusToken;

public:
	CSmartERApp();


// Overrides
public:
	virtual BOOL InitInstance() override;
	virtual int ExitInstance() override;

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState() override;
	virtual void LoadCustomState() override;
	virtual void SaveCustomState() override;

	afx_msg void OnAppAbout();
	afx_msg void OnCloseAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CSmartERApp theApp;
