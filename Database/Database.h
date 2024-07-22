#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

// CDatabaseApp
// See Database.cpp for the implementation of this class

class CDatabaseApp : public CWinApp
{
public:
	CDatabaseApp();

	// Overrides
public:
	BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};
