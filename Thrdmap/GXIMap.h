// Thrdmap.h : main header file for the Thrdmap DLL


#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CThrdmapApp
// See Thrdmap.cpp for the implementation of this class


class CThrdmapApp : public CWinApp
{
public:
	CThrdmapApp();

// Overrides
public:
	virtual BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};
