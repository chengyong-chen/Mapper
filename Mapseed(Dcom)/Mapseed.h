#pragma once

// Mapseed.h : Mapseed.DLL 的主头文件

#if !defined(__AFXCTL_H__)
#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // 主符号


// CMapseedApp : 有关实现的信息，请参阅 Mapseed.cpp。

class CMapseedApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription);
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid);
HRESULT UnRegisterCLSIDInCategory(REFCLSID clsid, CATID catid);


