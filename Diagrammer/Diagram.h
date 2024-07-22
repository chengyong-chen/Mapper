// Diagram.h : main header file for the Diagram DLL


#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDiagramApp
// See Diagram.cpp for the implementation of this class


class CDiagramApp : public CWinApp
{
public:
	CDiagramApp();

// Overrides
public:
	virtual BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};
