// Framework.h : main header file for the Framework DLL

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

// CFrameworkApp
// See Framework.cpp for the implementation of this class

class CFrameworkApp : public CWinApp
{
public:
	CFrameworkApp();

	// Overrides
public:
	BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};
