// Public.h : main header file for the Public DLL

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

// CPublicApp
// See Public.cpp for the implementation of this class

class CPublicApp : public CWinApp
{
public:
	CPublicApp();

	// Overrides
public:
	BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};
