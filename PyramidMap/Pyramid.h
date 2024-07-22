// PyramidMap.h : main header file for the PyramidMap DLL


#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CPyramidMapApp
// See PyramidMap.cpp for the implementation of this class


class CPyramidMapApp : public CWinApp
{
public:
	CPyramidMapApp();

// Overrides
public:
	virtual BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};
