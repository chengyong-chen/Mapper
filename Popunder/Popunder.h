#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

class CPopunderApp : public CWinAppEx
{
public:
	CPopunderApp();

	// Overrides
public:
	BOOL InitInstance() override;
	// Implementation

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
};

extern CPopunderApp theApp;
