// Thematics.h : main header file for the Thematics DLL

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

// CThematicsApp
// See Thematics.cpp for the implementation of this class

class CThematicsApp : public CWinApp
{
public:
	CThematicsApp();

	// Overrides
public:
	BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};
