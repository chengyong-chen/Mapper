// Tool.h : main header file for the Tool DLL

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

// CToolApp
// See Tool.cpp for the implementation of this class

class CToolApp : public CWinApp
{
public:
	CToolApp();

	// Overrides
public:
	BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};
