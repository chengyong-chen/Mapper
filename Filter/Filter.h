#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CFilterApp
// See Filter.cpp for the implementation of this class

class CFilterApp : public CWinApp
{
public:
	CFilterApp();

	// Overrides
public:
	BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};
