#pragma once

// Mapseed.h : Mapseed.DLL ����ͷ�ļ�

#if !defined(__AFXCTL_H__)
#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // ������

// CMapseedApp : �й�ʵ�ֵ���Ϣ������� Mapseed.cpp��

class CMapseedApp : public COleControlModule
{
	ULONG_PTR m_gdiplusToken;
public:
	BOOL InitInstance();
	int ExitInstance();

public:
	void SetRegister(CString strProject);
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

