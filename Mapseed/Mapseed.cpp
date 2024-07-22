// Mapseed.cpp : CMapseedApp �� DLL ע���ʵ�֡�

#include "stdafx.h"
#include "Mapseed.h"

#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>


#include "../Atlas/PageItem.h"
#include "../Atlas/Atlas1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMapseedApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =	{ 0x5F5507F6, 0x78CE, 0x4F17, { 0xAD, 0x6F, 0x5A, 0x80, 0x88, 0xEC, 0xF6, 0xAD } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CMapseedApp::InitInstance - DLL ��ʼ��
BOOL CMapseedApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);

	return bInit;
}


// CMapseedApp::ExitInstance - DLL ��ֹ
int CMapseedApp::ExitInstance()
{
//	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	return COleControlModule::ExitInstance();
}

void CMapseedApp::SetRegister(CString strProject)
{
	if(strProject.IsEmpty() == FALSE)
	{
		static CString strCmdLine;
		strCmdLine = strProject;
		m_lpCmdLine = (LPTSTR)(LPCTSTR)strCmdLine;
		
		CString strRegistryKey;
		strRegistryKey.Format(_T("Diwatu\\%s"),strProject);
		SetRegistryKey(strRegistryKey);
	}
}

// DllRegisterServer - ������ӵ�ϵͳע���
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if(!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - �����ϵͳע������Ƴ�
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if(!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
