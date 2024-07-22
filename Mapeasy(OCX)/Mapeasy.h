#pragma once

// Mapeasy.h : Mapeasy.DLL ����ͷ�ļ�

#if !defined(__AFXCTL_H__)
#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // ������


// CMapeasyApp : �й�ʵ�ֵ���Ϣ������� Mapeasy.cpp��

class CMapeasyApp : public COleControlModule
{
	ULONG_PTR m_gdiplusToken;
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


