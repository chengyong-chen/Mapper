// GridCtrl.h : main header file for the GridCtrl DLL

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CGridCtrlApp
// See GridCtrl.cpp for the implementation of this class

class CGridCtrlApp : public CWinApp
{
public:
	CGridCtrlApp();

	// Overrides
public:
	BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};
